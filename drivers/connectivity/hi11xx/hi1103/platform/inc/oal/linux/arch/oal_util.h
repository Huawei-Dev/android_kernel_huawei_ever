

#ifndef __OAL_LINUX_UTIL_H__
#define __OAL_LINUX_UTIL_H__

/* ?????????????? */
/*lint -e322*/
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <asm/string.h>
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
#include <asm/system.h>
#endif
#include <linux/thread_info.h>
#include <asm/byteorder.h>
#include <linux/byteorder/generic.h>
#include <linux/bitops.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/delay.h>
#include <asm/memory.h>

#include <linux/kobject.h> /* hi1102-cb add sys for 51/02 */
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/etherdevice.h> /* hi1102-cb for random mac address */
#include <linux/version.h>
#include <linux/math64.h>

#include <linux/ktime.h>
#include <linux/random.h>
#include "securec.h"
/*lint +e322*/
/* ?????? */
/* 32???????????????? */
#define OAL_SWAP_BYTEORDER_32(_val)  \
    ((((_val)&0x000000FF) << 24) +   \
        (((_val)&0x0000FF00) << 8) + \
        (((_val)&0x00FF0000) >> 8) + \
        (((_val)&0xFF000000) >> 24))

#define OAL_CONST const

/* ????oal_print_hex_dump()???????????????????????????????????????????????????????? */
#define HEX_DUMP_GROUP_SIZE 32

/* ????CORE ID */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#define OAL_GET_CORE_ID() 0
#else /* ??offload???? */
#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
#define OAL_GET_CORE_ID() smp_processor_id()
#else
#define OAL_GET_CORE_ID() 0 /* ??????E5???????? */
#endif
#endif

typedef oal_uint oal_bitops;

typedef struct file oal_file_stru;
typedef loff_t oal_file_pos;
#define OAL_FILE_FAIL OAL_PTR_NULL

#define OAL_LIKELY(_expr)   likely(_expr)
#define OAL_UNLIKELY(_expr) unlikely(_expr)
#define OAL_FUNC_NAME       __func__
#define OAL_RET_ADDR        ((uintptr_t)__builtin_return_address(0))

/* ?????????? */
#define OAL_RMB() rmb()

/* ?????????? */
#define OAL_WMB() wmb()

/* ???????? */
#define OAL_MB() mb()

#define OAL_OFFSET_OF offsetof

#define __OAL_DECLARE_PACKED __attribute__((__packed__))

#ifndef HISI_LOG_TAG
#define HISI_LOG_TAG
#endif
#define OAL_IO_PRINT(fmt, arg...)                      \
    do {                                               \
        printk(KERN_INFO HISI_LOG_TAG "" fmt, ##arg); \
    } while (0)
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)

/* bug on not allow to use */
#define OAL_BUG_ON(_con) OAL_BUILD_BUG_ON(1)
#else
#define OAL_BUG_ON(_con) BUG_ON(_con)
#endif
#define OAL_WARN_ON(condition) WARN_ON(condition)

/* E5????????????????????host????????????MEM_BASE_ADDR???????? */
#if (_PRE_TARGET_PRODUCT_TYPE_E5 == _PRE_CONFIG_TARGET_PRODUCT)
extern oal_uint32 gul_dscr_fstvirt_addr;
extern oal_uint32 gul_dscr_fstphy_addr;

#ifdef _PRE_E5_750_PLATFORM
#define OAL_PLAT_MEM_BASE_ADDR 0x40000000
#else
#define OAL_PLAT_MEM_BASE_ADDR 0xA0000000
#endif
#define OAL_DSCR_VIRT_TO_PHY(_virt_addr) \
    (((oal_uint)(_virt_addr) - (gul_dscr_fstvirt_addr)) + (gul_dscr_fstphy_addr)-OAL_PLAT_MEM_BASE_ADDR)
#define OAL_DSCR_PHY_TO_VIRT(_phy_addr) \
    (((oal_uint)(_phy_addr) + OAL_PLAT_MEM_BASE_ADDR) - (gul_dscr_fstphy_addr) + (gul_dscr_fstvirt_addr))

#elif (_PRE_TARGET_PRODUCT_TYPE_CPE == _PRE_CONFIG_TARGET_PRODUCT)
extern oal_uint32 gul_dscr_fstvirt_addr;
extern oal_uint32 gul_dscr_fstphy_addr;

#ifdef _PRE_CPE_711_PLATFORM
#define OAL_PLAT_MEM_BASE_ADDR 0x15E00000 /* DDR????????0xA0000000 - V200????????????????0x40000000 */
#elif defined _PRE_CPE_722_PLATFORM
#define OAL_PLAT_MEM_BASE_ADDR 0x60000000 /* DDR????????0x55E00000 - V200????????????????0x40000000 */
#endif

#define OAL_DSCR_VIRT_TO_PHY(_virt_addr) \
    (((oal_uint)(_virt_addr) - (gul_dscr_fstvirt_addr)) + (gul_dscr_fstphy_addr)-OAL_PLAT_MEM_BASE_ADDR)
#define OAL_DSCR_PHY_TO_VIRT(_phy_addr) \
    (((oal_uint)(_phy_addr) + OAL_PLAT_MEM_BASE_ADDR) - (gul_dscr_fstphy_addr) + (gul_dscr_fstvirt_addr))

#else

#ifdef _PRE_WLAN_PRODUCT_1151V200
#define OAL_PLAT_MEM_BASE_ADDR 0x40000000
#else
#define OAL_PLAT_MEM_BASE_ADDR 0x80000000
#endif

#define OAL_DSCR_VIRT_TO_PHY(_virt_addr) (virt_to_phys(_virt_addr) - OAL_PLAT_MEM_BASE_ADDR)
#define OAL_DSCR_PHY_TO_VIRT(_phy_addr)  phys_to_virt((_phy_addr) + OAL_PLAT_MEM_BASE_ADDR)
#endif

/* ????????????????????????????,????netbuf?? */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 38))
#define OAL_VIRT_TO_PHY_ADDR(_virt_addr) (virt_to_phys((const volatile void *)(_virt_addr)) - OAL_PLAT_MEM_BASE_ADDR)
#else
#define OAL_VIRT_TO_PHY_ADDR(_virt_addr) (virt_to_phys((void *)(_virt_addr)) - OAL_PLAT_MEM_BASE_ADDR)
#endif
#define OAL_PHY_TO_VIRT_ADDR(_phy_addr) phys_to_virt((_phy_addr) + OAL_PLAT_MEM_BASE_ADDR)

#define OAL_CFG_FILE_PATH "/tmp/1151_cfg.ini"

typedef ssize_t oal_ssize_t; /* hi1102-cb for sys interface  51/02 */
typedef size_t oal_size_t;
typedef struct device_attribute oal_device_attribute_stru;
typedef struct device oal_device_stru;
#define OAL_DEVICE_ATTR DEVICE_ATTR
#define OAL_S_IRUGO     S_IRUGO
#define OAL_S_IWGRP     S_IWGRP
#define OAL_S_IWUSR     S_IWUSR
typedef struct kobject oal_kobject;

#define OAL_STRLEN      strlen
#define OAL_MEMCMP      memcmp
#define OAL_STRSTR      strstr
#define OAL_STRCMP      strcmp
#define OAL_STRNCMP     strncmp
#define OAL_STRNCASECMP strncasecmp

/* us time cost sub */
#define declare_time_cost_stru(name) \
    oal_uint64 trans_us_##name;      \
    ktime_t local_cost_time_start_##name, local_cost_time_end_##name
#define time_cost_var_start(name) local_cost_time_start_##name
#define time_cost_var_end(name)   local_cost_time_end_##name
#define time_cost_var_sub(name)   trans_us_##name

#define oal_get_time_cost_start(name) time_cost_var_start(name) = ktime_get()
#define oal_get_time_cost_end(name)   time_cost_var_end(name) = ktime_get()
#define oal_calc_time_cost_sub(name)  time_cost_var_sub(name) =                                      \
                                         (oal_uint64)ktime_to_us(ktime_sub(time_cost_var_end(name), \
                                             time_cost_var_start(name)))

/* #define random_ether_addr(addr) eth_random_addr(addr) */
/* static inline void eth_random_addr(u8 *addr) */
static OAL_INLINE void oal_random_ether_addr(oal_uint8 *addr)
{
    random_ether_addr(addr);
}

/*
 * ?? ?? ??  : oal_bit_atomic_test_and_set
 * ????????  : ????????????????????1??????????????????????????
 * ????????  : nr: ????????????
             p_addr??????????????????
 */
OAL_STATIC OAL_INLINE oal_bitops oal_bit_atomic_test_and_set(oal_bitops nr, OAL_VOLATILE oal_bitops *p_addr)
{
    return test_and_set_bit(nr, p_addr);
}

/*
 * ?? ?? ??  : oal_bit_atomic_clear
 * ????????  : ????????????????????????????????????????0??????
 * ????????  : nr: ????????????
 *             p_addr??????????????????
 */
OAL_STATIC OAL_INLINE oal_void oal_bit_atomic_clear(oal_bitops nr, OAL_VOLATILE oal_bitops *p_addr)
{
    clear_bit(nr, p_addr);
}

/*
 * ?? ?? ??  : oal_byteorder_host_to_net_uint16
 * ????????  : ??16????????????????????????????
 * ????????  : us_byte: ????????????????????
 * ?? ?? ??  : ????????????
 */
OAL_STATIC OAL_INLINE __attribute_const__ oal_uint16 oal_byteorder_host_to_net_uint16(oal_uint16 us_byte)
{
    return htons(us_byte);
}

/*
 * ?? ?? ??  : oal_byteorder_host_to_net_uint16
 * ????????  : ??16????????????????????????????
 * ????????  : us_byte: ????????????????????
 * ?? ?? ??  : ????????????
 */
OAL_STATIC OAL_INLINE __attribute_const__ oal_uint16 oal_byteorder_net_to_host_uint16(oal_uint16 us_byte)
{
    return ntohs(us_byte);
}

/*
 * ?? ?? ??  : oal_byteorder_host_to_net_uint32
 * ????????  : ??32????????????????????????????
 * ????????  : us_byte: ????????????????????
 * ?? ?? ??  : ????????????
 */
OAL_STATIC OAL_INLINE __attribute_const__ oal_uint32 oal_byteorder_host_to_net_uint32(oal_uint32 ul_byte)
{
    return htonl(ul_byte);
}

/*
 * ?? ?? ??  : oal_byteorder_net_to_host_uint32
 * ????????  : ??32????????????????????????????
 * ????????  : ul_byte: ????????????????????
 * ?? ?? ??  : ????????????
 */
OAL_STATIC OAL_INLINE __attribute_const__ oal_uint32 oal_byteorder_net_to_host_uint32(oal_uint32 ul_byte)
{
    return ntohl(ul_byte);
}

/*
 * ?? ?? ??  : oal_file_open_rw
 * ????????  : ??????????????rw????????????
 * ????????  : pc_path: ????????
 */
OAL_STATIC OAL_INLINE oal_file_stru *oal_file_open_rw(const oal_int8 *pc_path)
{
    mm_segment_t fs;
    oal_file_stru *file = NULL;

    fs = get_fs();
    set_fs(KERNEL_DS);
    file = filp_open(pc_path, (O_CREAT | O_RDWR), 0);
    set_fs(fs);
    if (IS_ERR(file)) {
        return OAL_PTR_NULL;
    }

    return file;
}

/*
 * ?? ?? ??  : oal_file_open_append
 * ????????  : ????????,????append????????????
 * ????????  : pc_path: ????????
 */
OAL_STATIC OAL_INLINE oal_file_stru *oal_file_open_append(const oal_int8 *pc_path)
{
    mm_segment_t fs;
    oal_file_stru *file = NULL;

    if (OAL_UNLIKELY(pc_path == NULL)) {
        OAL_WARN_ON(1);
        return NULL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    file = filp_open(pc_path, (O_CREAT | O_APPEND), 0);
    set_fs(fs);
    if (IS_ERR(file)) {
        return OAL_PTR_NULL;
    }

    return file;
}

/*
 * ?? ?? ??  : oal_file_open_rw
 * ????????  : ??????????????????
 * ????????  : pc_path: ????????
 */
OAL_STATIC OAL_INLINE oal_file_stru *oal_file_open_readonly(const oal_int8 *pc_path)
{
    mm_segment_t fs;
    oal_file_stru *file = NULL;

    if (OAL_UNLIKELY(pc_path == NULL)) {
        OAL_WARN_ON(1);
        return NULL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    file = filp_open(pc_path, (O_RDONLY), 0);
    set_fs(fs);
    if (IS_ERR(file)) {
        return OAL_PTR_NULL;
    }

    return file;
}

/*
 * ?? ?? ??  : oal_file_write
 * ????????  : ??????
 * ????????  : file: ????????
 *           : pc_string: ????????????
 *           : ul_length: ????????????
 */
OAL_STATIC OAL_INLINE oal_file_stru *oal_file_write(oal_file_stru *file, oal_int8 *pc_string, oal_uint32 ul_length)
{
    oal_int i_ret;
    mm_segment_t fs;

    if (OAL_UNLIKELY((file == NULL) || (pc_string == NULL))) {
        OAL_WARN_ON(1);
        return NULL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    i_ret = file->f_op->write(file, pc_string, ul_length, &file->f_pos);
    set_fs(fs);

    return file;
}

OAL_STATIC OAL_INLINE oal_int32 oal_file_close(oal_file_stru *file)
{
    int ret;
    mm_segment_t fs;

    if (OAL_UNLIKELY(file == NULL)) {
        OAL_WARN_ON(1);
        return OAL_ERR_CODE_CLOSE_FILE_FAIL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    ret = filp_close(file, 0);
    set_fs(fs);
    if (ret == 0) {
        return OAL_SUCC;
    } else {
        return OAL_ERR_CODE_CLOSE_FILE_FAIL;
    }
}

/*
 * ?? ?? ??  : oal_file_read
 * ????????  : ??????????????????????
 * ????????  : file:??????????????????????
 *             pc_buf:??????????????????????buf
 *             ul_count:????????????????
 */
OAL_STATIC OAL_INLINE oal_int32 oal_file_read(oal_file_stru *file,
                                              oal_int8 *pc_buf,
                                              oal_uint32 ul_count)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
    loff_t pos = 0;
#endif
    if (OAL_UNLIKELY((file == NULL) || (pc_buf == NULL))) {
        OAL_WARN_ON(1);
        return -OAL_ERR_CODE_READ_FILE_FAIL;
    }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
    return kernel_read(file, pc_buf, ul_count, &pos);
#else
    return kernel_read(file, 0, pc_buf, ul_count);
#endif
}

OAL_STATIC OAL_INLINE oal_int32 oal_file_read_ext(oal_file_stru *file,
                                                  oal_file_pos pos,
                                                  oal_int8 *pc_buf,
                                                  oal_uint32 ul_count)
{
    if (OAL_UNLIKELY((file == NULL) || (pc_buf == NULL))) {
        OAL_WARN_ON(1);
        return -OAL_ERR_CODE_READ_FILE_FAIL;
    }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
    return kernel_read(file, pc_buf, ul_count, &pos);
#else
    return kernel_read(file, pos, pc_buf, ul_count);
#endif
}

OAL_STATIC OAL_INLINE oal_int32 oal_file_size(oal_uint32 *pul_file_size)
{
    oal_file_stru *p_file = NULL;

    if (OAL_UNLIKELY(pul_file_size == NULL)) {
        OAL_WARN_ON(1);
        return OAL_FAIL;
    }

    p_file = oal_file_open_readonly(OAL_CFG_FILE_PATH);
    if (p_file == OAL_PTR_NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 1, 0))
    *pul_file_size = (file_inode(p_file))->i_size;
#else
    *pul_file_size = ((p_file->f_path).dentry)->d_inode->i_size;
#endif
    oal_file_close(p_file);

    return OAL_SUCC;
}

/*
 * ?? ?? ??  : oal_atoi
 * ????????  : ????????????????????
 * ????????  : c_string: ??????????
 */
OAL_STATIC OAL_INLINE oal_int32 oal_atoi(const oal_int8 *c_string)
{
    oal_int32 l_sign;
    oal_uint32 ul_digit;
    oal_int32 l_ret = 0;

    if (OAL_UNLIKELY(c_string == NULL)) {
        OAL_WARN_ON(1);
        return 0;
    }

    /* Skip any leading blanks */
    while ((*c_string) == ' ') {
        c_string += 1;
    }

    /* Check for a sign */
    if (*c_string == '-') {
        l_sign = 1;
        c_string += 1;
    } else {
        l_sign = 0;
        if (*c_string == '+') {
            c_string += 1;
        }
    }

    for (;; c_string += 1) {
        ul_digit = *c_string - '0';
        if (ul_digit > 9) { /* ????????????'0'~'9'???????????? */
            break;
        }

        l_ret = (l_ret * 10) + ul_digit; /* ?????????????????????? */
    }

    return ((l_sign) ? (-l_ret) : l_ret);
}

/*
 * ?? ?? ??  : oal_itoa
 * ????????  : ????????????
 */
OAL_STATIC OAL_INLINE oal_void oal_itoa(oal_int32 l_val, oal_int8 *c_string, oal_uint8 uc_strlen)
{
    if (OAL_UNLIKELY(c_string == NULL)) {
        OAL_WARN_ON(1);
        return;
    }

    if (snprintf_s(c_string, uc_strlen, uc_strlen - 1, "%d", l_val) < 0) {
        OAL_IO_PRINT("oal_itoa::snprintf_s failed.\n");
    }
}

OAL_STATIC OAL_INLINE oal_int8 *oal_strtok(oal_int8 *pc_token, OAL_CONST oal_int8 *pc_delemit, oal_int8 **ppc_context)
{
    static oal_uint8 *pc_nextoken = NULL;

    oal_uint8 *pc_str = NULL;
    OAL_CONST oal_uint8 *pc_ctrl = (oal_uint8 *)pc_delemit;
    const oal_uint32 ul_map_size = 32;
    oal_uint8 uc_map[ul_map_size]; /* ??????????????????????const???????????????? */
    oal_int32 l_count;

    if (OAL_UNLIKELY(pc_delemit == NULL)) {
        OAL_WARN_ON(1);
        return NULL;
    }

    /* Clear control map */
    for (l_count = 0; l_count < ul_map_size; l_count++) {
        uc_map[l_count] = 0;
    }

    /* Set bits in delimiter table */
    do {
        uc_map[*pc_ctrl >> 3] |= (1 << (*pc_ctrl & 7));
    } while (*pc_ctrl++);

    /*
     * Initialize str. If string is NULL, set str to the saved
     * pointer (i.e., continue breaking tokens out of the string
     * from the last strtok call)
     */
    if (pc_token != NULL) {
        pc_str = (oal_uint8 *)pc_token;
    } else {
        pc_str = pc_nextoken;
    }

    /*
     * Find beginning of token (skip over leading delimiters). Note that
     * there is no token iff this loop sets str to point to the terminal
     * null (*str == '\0')
     */
    while ((uc_map[*pc_str >> 3] & (1 << (*pc_str & 7))) && *pc_str) {
        pc_str++;
    }

    pc_token = (oal_int8 *)pc_str;

    /* Find the end of the token. If it is not the end of the string, put a null there. */
    for (; *pc_str; pc_str++) {
        if (uc_map[*pc_str >> 3] & (1 << (*pc_str & 7))) {
            *pc_str++ = '\0';
            break;
        }
    }

    /* Update nextoken (or the corresponding field in the per-thread data structure */
    pc_nextoken = pc_str;

    /* Determine if a token has been found. */
    if ((uintptr_t)pc_token == (uintptr_t)pc_str) {
        return NULL;
    } else {
        return pc_token;
    }
}

/* Works only for digits and letters, but small and fast */
#define TOLOWER(c) (((unsigned char)(c)) | 0x20)

#define isdigit(c) ('0' <= (c) && (c) <= '9')

#define isxdigit(c) (('0' <= (c) && (c) <= '9') || \
                     ('a' <= (c) && (c) <= 'f') || \
                     ('A' <= (c) && (c) <= 'F'))

#ifndef __round_mask
#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#endif

#ifndef round_up
#define round_up(x, y) ((((x)-1) | __round_mask(x, y)) + 1)
#endif
#define OAL_ROUND_UP round_up

#ifndef round_down
#define round_down(x, y) ((x) & ~__round_mask(x, y))
#endif
#define OAL_ROUND_DOWN round_down

OAL_STATIC OAL_INLINE unsigned int simple_guess_base(const char *cp)
{
    if (cp[0] == '0') {
        if (TOLOWER(cp[1]) == 'x' && isxdigit(cp[2])) {
            return 16; /* 0x...??????16???? */
        } else {
            return 8; /* 0...????????????????8???? */
        }
    } else {
        return 10; /* ????0??????????????????strtol????????????0????????10???? */
    }
}

OAL_STATIC OAL_INLINE unsigned long long oal_simple_strtoull(const oal_int8 *cp, oal_int8 **endp, unsigned int base)
{
    unsigned long long result = 0;

    if (OAL_UNLIKELY(cp == NULL)) {
        OAL_WARN_ON(1);
        return 0;
    }

    if (!base) {
        base = simple_guess_base(cp);
    }

    if (base == 16 && cp[0] == '0' && TOLOWER(cp[1]) == 'x') { /* ????????strtol??????????????????16 */
        cp += 2;                                               /* ????16??????0x?????? */
    }

    while (isxdigit(*cp)) {
        unsigned int value;

        value = isdigit(*cp) ? *cp - '0' : TOLOWER(*cp) - 'a' + 10; /* ??10??????????????'a'~'f'????10~15 */
        if (value >= base) {
            break;
        }
        result = result * base + value;
        cp++;
    }
    if (endp != NULL) {
        *endp = (oal_int8 *)cp;
    }

    return result;
}

OAL_STATIC OAL_INLINE oal_int oal_strtol(OAL_CONST oal_int8 *pc_nptr, oal_int8 **ppc_endptr, oal_int32 l_base)
{
    if (OAL_UNLIKELY(pc_nptr == NULL)) {
        OAL_WARN_ON(1);
        return 0;
    }
    /* ???????? */
    while ((*pc_nptr) == ' ') {
        pc_nptr++;
    }

    if (*pc_nptr == '-') {
        return -oal_simple_strtoull(pc_nptr + 1, ppc_endptr, l_base);
    }

    return oal_simple_strtoull(pc_nptr, ppc_endptr, l_base);
}

/*
 * ?? ?? ??  : oal_udelay
 * ????????  : ??????????????
 */
OAL_STATIC OAL_INLINE oal_void oal_udelay(oal_uint u_loops)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 44))
    udelay(u_loops);
#else
    __udelay(u_loops);
#endif
}

/*
 * ?? ?? ??  : oal_mdelay
 * ????????  : ??????????????
 */
OAL_STATIC OAL_INLINE oal_void oal_mdelay(oal_uint u_loops)
{
    mdelay(u_loops);
}

/*
 * ?? ?? ??  : oal_kallsyms_lookup_name
 * ????????  : ????????????????????????????????
 */
OAL_STATIC OAL_INLINE oal_uint32 oal_kallsyms_lookup_name(OAL_CONST oal_uint8 *uc_var_name)
{
    return kallsyms_lookup_name(uc_var_name);
}

/*
 * ?? ?? ??  : oal_dump_stack
 * ????????  : ??????????????
 */
OAL_STATIC OAL_INLINE oal_void oal_dump_stack(oal_void)
{
#if (_PRE_CONFIG_TARGET_PRODUCT != _PRE_TARGET_PRODUCT_TYPE_WS835DMB)
    // 835????????dump_stack??????????????
    dump_stack();
#endif
}

OAL_STATIC OAL_INLINE oal_void oal_msleep(oal_uint32 ul_usecs)
{
    msleep(ul_usecs);
}

OAL_STATIC OAL_INLINE oal_void oal_usleep_range(oal_ulong min_us, oal_ulong max_us)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 44))
    usleep_range(min_us, max_us); /* ????????????????????CPU */
#else
    OAL_REFERENCE(max_us);
    oal_udelay(min_us);
#endif
}

OAL_STATIC OAL_INLINE oal_void oal_print_hex_dump(oal_uint8 *addr, oal_int32 len,
                                                  oal_int32 groupsize, oal_int8 *pre_str)
{
#ifdef CONFIG_PRINTK
    OAL_REFERENCE(groupsize);
    if (OAL_UNLIKELY((addr == NULL) || (pre_str == NULL))) {
        OAL_WARN_ON(1);
        return;
    }
    printk(KERN_INFO "buf %p,len:%d\n",
           addr,
           len);
    print_hex_dump(KERN_INFO, pre_str, DUMP_PREFIX_ADDRESS, 16, 1,
                   addr, len, true); /* ?????????????????? */
    printk(KERN_INFO "\n");
#endif
}

/*
 * ?? ?? ??  : oal_div_u64
 * ????????  : linux uint64 ????
 */
OAL_STATIC OAL_INLINE oal_uint64 oal_div_u64(oal_uint64 dividend, oal_uint32 divisor)
{
    oal_uint64 ull_tmp;

    ull_tmp = div_u64(dividend, divisor);

    return ull_tmp;
}

/*
 * ?? ?? ??  : oal_div_s64
 * ????????  : linux int64 ????
 */
OAL_STATIC OAL_INLINE oal_int64 oal_div_s64(oal_int64 dividend, oal_int32 divisor)
{
    oal_int64 ll_tmp;

    ll_tmp = div_s64(dividend, divisor);

    return ll_tmp;
}

#endif /* end of oal_util.h */
