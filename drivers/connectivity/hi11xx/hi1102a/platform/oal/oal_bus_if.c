

/* ?????????? */
#include "oal_bus_if.h"

#include "oal_ext_if.h"
#include "oam_ext_if.h"

/* ???????????? */
/* ????chip???? */
OAL_STATIC oal_uint8 bus_chip_num = 0;
#if (((_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION) && (_PRE_TEST_MODE == _PRE_TEST_MODE_UT)) ||  \
     (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151))
#ifdef _PRE_WLAN_FEATURE_DOUBLE_CHIP
OAL_STATIC oal_bus_chip_stru bus_chip[WLAN_CHIP_MAX_NUM_PER_BOARD] = {{0}, {0}};
#else
OAL_STATIC oal_bus_chip_stru bus_chip[WLAN_CHIP_MAX_NUM_PER_BOARD] = {{0}};
#endif
#endif

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
OAL_STATIC oal_int8 *pac_irq_pc_name[OAL_1151_IRQ_NAME_BUFF_SIZE] = { "wlan1", "wlan2", "wlan3", "wlan4" };

/*
 * ?? ?? ??  : oal_bus_find_dev_instance
 * ????????  : ????????????????
 * ????????  : oal_bus_dev_stru **ppst_bus_dev, oal_uint8 uc_index
 * ????????  : ppst_bus_dev: ????????
 */
oal_void oal_bus_find_dev_instance(oal_bus_dev_stru **ppst_bus_dev, oal_void *p_dev)
{
    oal_uint8 uc_chip_index;
    oal_uint8 uc_device_index;

    for (uc_chip_index = 0; uc_chip_index < bus_chip_num; uc_chip_index++) {
        for (uc_device_index = 0; uc_device_index < bus_chip[uc_chip_index].uc_device_num; uc_device_index++) {
            /* ???????????? */
            if (bus_chip[uc_chip_index].st_bus_dev[uc_device_index].p_dev == p_dev) {
                *ppst_bus_dev = &bus_chip[uc_chip_index].st_bus_dev[uc_device_index];
                return;
            }
        }
    }

    *ppst_bus_dev = OAL_PTR_NULL;
}
/*
 * ?? ?? ??  : oal_bus_get_dev_instance
 * ????????  : ????bus dev
 * ????????  : uc_chip_id     : CHIP ID
 *             uc_device_id   : DEVICE ID
 * ?? ?? ??  : ??????
 */
OAL_STATIC oal_bus_dev_stru *oal_bus_get_dev_instance(oal_uint8 uc_chip_id, oal_uint8 uc_device_id)
{
    oal_bus_chip_stru *pst_bus_chip;

    if (uc_chip_id >= WLAN_CHIP_MAX_NUM_PER_BOARD) {
        OAL_IO_PRINT("oal_bus_register_irq: uc_chip_id = %d\n", uc_chip_id);
        return OAL_PTR_NULL;
    }

    pst_bus_chip = &bus_chip[uc_chip_id];

    if (uc_device_id >= pst_bus_chip->uc_device_num) {
        OAL_IO_PRINT("oal_bus_register_irq: uc_device_id = %d\n", uc_device_id);
        return OAL_PTR_NULL;
    }

    return &pst_bus_chip->st_bus_dev[uc_device_id];
}
/*
 * ?? ?? ??  : oal_bus_unregister_irq
 * ????????  : ????????
 * ?? ?? ??  : ??????
 */
oal_void oal_bus_unregister_irq(oal_uint8 uc_chip_id, oal_uint8 uc_device_id)
{
    oal_bus_dev_stru *pst_bus_dev;

    pst_bus_dev = oal_bus_get_dev_instance(uc_chip_id, uc_device_id);
    if (pst_bus_dev == OAL_PTR_NULL) {
        return;
    }

    oal_irq_free(&pst_bus_dev->st_irq_info);
}
/*
 * ?? ?? ??  : oal_bus_register_irq
 * ????????  : ????????
 * ????????  : pst      : ??????????
 *             p_func   : ????????????
 * ?? ?? ??  : ??????
 */
oal_uint32 oal_bus_register_irq(oal_void *pst, oal_irq_intr_func p_func, oal_uint8 uc_chip_id, oal_uint8 uc_device_id)
{
    oal_bus_dev_stru *pst_bus_dev;

    pst_bus_dev = oal_bus_get_dev_instance(uc_chip_id, uc_device_id);
    if (pst_bus_dev == OAL_PTR_NULL) {
        return OAL_FAIL;
    }

    OAL_IRQ_INIT_MAC_DEV(pst_bus_dev->st_irq_info,
                         pst_bus_dev->ul_irq_num,
                         OAL_SA_SHIRQ,
                         pac_irq_pc_name[uc_chip_id * WLAN_DEVICE_MAX_NUM_PER_CHIP + uc_device_id],
                         pst,
                         p_func);

    if (oal_irq_setup(&pst_bus_dev->st_irq_info)) {
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

/*
 * ?? ?? ??  : oal_bus_get_chip_instance
 * ????????  : ????????????????
 * ????????  : oal_bus_dev_stru **ppst_bus_dev, oal_uint8 uc_index
 * ????????  : ppst_bus_dev: ????????
 */
oal_void oal_bus_get_chip_instance(oal_bus_chip_stru **ppst_bus_chip, oal_uint8 uc_index)
{
    if (uc_index < WLAN_CHIP_MAX_NUM_PER_BOARD) {
        *ppst_bus_chip = &bus_chip[uc_index];
    } else {
        *ppst_bus_chip = OAL_PTR_NULL;
        OAL_IO_PRINT("oal_bus_get_chip_instance: uc_index = %d\n", uc_index);
    }
}

#endif

/*
 * ?? ?? ??  : oal_bus_get_chip_num
 * ????????  : ????????????????????
 * ?? ?? ??  : chip ????
 */
oal_uint8 oal_bus_get_chip_num(oal_void)
{
    return bus_chip_num;
}

/*
 * ?? ?? ??  : oal_bus_inc_chip_num
 * ????????  : ????????????????????
 * ?? ?? ??  : chip ????
 */
oal_uint32 oal_bus_inc_chip_num(oal_void)
{
    if (bus_chip_num < WLAN_CHIP_MAX_NUM_PER_BOARD) {
        bus_chip_num++;
    } else {
        OAL_IO_PRINT("oal_bus_inc_chip_num FAIL: bus_chip_num = %d\n", bus_chip_num);
        return OAL_FAIL;
    }

    /* WINDOWS??UT???? */
#if (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION) && (_PRE_TEST_MODE == _PRE_TEST_MODE_UT)
    bus_chip[0].uc_device_num = bus_chip_num;
#endif

    return OAL_SUCC;
}

oal_void oal_bus_init_chip_num(oal_void)
{
    bus_chip_num = 0;

    /* WINDOWS??UT???? */
#if (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION) && (_PRE_TEST_MODE == _PRE_TEST_MODE_UT)
    bus_chip[0].uc_device_num = bus_chip_num;
#endif
    return;
}

#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
/*
 * ?? ?? ??  : oal_bus_irq_affinity_init
 * ????????  : ??????????????????
 */
oal_void oal_bus_irq_affinity_init(oal_uint8 uc_chip_id, oal_uint8 uc_device_id, oal_uint32 ul_core_id)
{
    oal_bus_dev_stru *pst_bus_dev;

    pst_bus_dev = oal_bus_get_dev_instance(uc_chip_id, uc_device_id);
    if (pst_bus_dev == OAL_PTR_NULL) {
        return;
    }

    oal_irq_set_affinity(pst_bus_dev->st_irq_info.ul_irq, ul_core_id);
}
#endif

/*lint -e19*/
oal_module_symbol(oal_bus_get_chip_num);
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
oal_module_symbol(oal_bus_get_chip_instance);
oal_module_symbol(oal_bus_register_irq);
oal_module_symbol(oal_bus_unregister_irq);
#endif

#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
oal_module_symbol(oal_bus_irq_affinity_init);
#endif
/*lint +e19*/
