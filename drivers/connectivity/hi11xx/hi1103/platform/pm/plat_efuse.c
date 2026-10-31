

/* ?????????? */
#include "plat_efuse.h"
#include "board.h"
#include "securec.h"

/* ???????????? */
uint8 hi110x_ec_version_etc = V100;

/*
 * ?? ?? ??  : read_efuse_ec_version_etc
 * ????????  : ??device EFUSE??????EC??????
 */
void read_efuse_ec_version_etc(void)
{
    int32 ret;
    uint8 buff[EFUSE_REG_WIDTH] = { 0x00 };
    uint8 uc_ec_version;
    uint32 i = 0;

    if (get_hi110x_subchip_type() != BOARD_VERSION_HI1102) {
        PS_PRINT_INFO("no need read device ec version\n");
        return;
    }

    ret = number_type_cmd_send_etc(RMEM_CMD_KEYWORD, GET_EFUSE_EC_VERSION);
    if (ret < 0) {
        PS_PRINT_WARNING("send cmd %s:%s fail,ret = %d\n", RMEM_CMD_KEYWORD, GET_EFUSE_EC_VERSION, ret);
        return;
    }

    ret = read_msg_etc((uint8 *)buff, sizeof(buff));
    if (ret < 0) {
        PS_PRINT_WARNING("read efuse ec version fail, read_len = %d, return = %d\n", (int32)sizeof(buff), ret);
        return;
    }

    for (i = 0; i < sizeof(buff); i++) {
        PS_PRINT_DBG("ec version[%d]=0x%x\n", i, buff[i]);
    }

    uc_ec_version = buff[1];         // Byte24(0x50000771)????bit[191:184]
    uc_ec_version &= ((uint8)0x03);  // bit[185:184]????EC version

    if (uc_ec_version == V100) {
        PS_PRINT_INFO("hi110x read efuse V100[0x%x]\n", uc_ec_version);
        hi110x_ec_version_etc = V100;
    } else {
        PS_PRINT_INFO("hi110x read efuse V120[0x%x]\n", uc_ec_version);
        hi110x_ec_version_etc = V120;
    }

    return;
}

/*
 * ?? ?? ??  : get_ec_version_etc
 * ????????  : ????hi110x????EC??????
 * ?? ?? ??  : EC??????
 */
uint8 get_ec_version_etc(void)
{
    return hi110x_ec_version_etc;
}

/*
 * ?? ?? ??  : mask_bits
 * ????????  : ????????????
 */
static void mask_bits(uint32 value[], uint32 start_bits, uint32 end_bits)
{
    uint32 index = 0;
    uint32 i = 0;
    uint32 j = 0;

    if (value == NULL) {
        return;
    }

    for (index = start_bits; index <= end_bits; index++) {
        i = index / EFUSE_VALUE_WIDTH;
        j = index % EFUSE_VALUE_WIDTH;
        value[i] &= ~(1u << j);
    }
}

/*
 * ?? ?? ??  : check_efuse_file_exist
 * ????????  : ????????????????
 */
static int32 check_efuse_file_exist(void)
{
    mm_segment_t fs;
    struct file *fp = NULL;

    fs = get_fs();
    set_fs(KERNEL_DS);
    fp = filp_open(EFUSE_FILE_PATH, O_RDONLY, 0);
    if (IS_ERR(fp)) {
        set_fs(fs);
        return -EFAIL;
    }

    filp_close(fp, NULL);
    set_fs(fs);

    return SUCC;
}

/*
 * ?? ?? ??  : get_efuse_from_device
 * ????????  : ??device????efuse????
 */
static int32 get_efuse_from_device(uint32 *buff, int32 len)
{
    int32 ret;

    if (buff == NULL) {
        PS_PRINT_ERR("efuse buff is NULL\n");
        return -EFAIL;
    }

    ret = number_type_cmd_send_etc(RMEM_CMD_KEYWORD, GET_EFUSE_CMD);
    if (ret < 0) {
        PS_PRINT_WARNING("send cmd %s:%s fail,ret = %d\n", RMEM_CMD_KEYWORD, GET_EFUSE_CMD, ret);
        return -EFAIL;
    }

    ret = read_msg_etc((uint8 *)buff, len);
    if (ret < 0) {
        PS_PRINT_WARNING("read efuse fail, read_len = %d, return = %d\n", len, ret);
        return -EFAIL;
    }

    return SUCC;
}
/*
 * ?? ?? ??  : store_efuse_into_file
 * ????????  : ??efuse????????????
 */
static int32 store_efuse_into_file(uint32 *buff, uint32 buff_len)
{
    struct file *fp = NULL;
    loff_t pos;
    int32 index = 0;
    ssize_t ret = 0;
    mm_segment_t fs;

    if (buff == NULL) {
        PS_PRINT_ERR("efuse buff is NULL\n");
        return -EFAIL;
    }

    mask_bits(buff, DIEID_BIT_4, DIEID_BIT_21);
    mask_bits(buff, DIEID_BIT_45, DIEID_BIT_45);
    mask_bits(buff, DIEID_BIT_53, DIEID_BIT_53);
    mask_bits(buff, DIEID_BIT_79, DIEID_BIT_95);

    memset_s(&fs, sizeof(fs), 0x00, sizeof(fs));

    fs = get_fs();
    set_fs(KERNEL_DS);
    fp = filp_open(EFUSE_FILE_PATH, O_CREAT | O_RDWR, 0644);
    if (IS_ERR(fp)) {
        PS_PRINT_ERR("open %s fail, errno = %ld\n", EFUSE_FILE_PATH, PTR_ERR(fp));
        set_fs(fs);
        return -EFAIL;
    }

    pos = 0;
    for (index = 0; index < buff_len / sizeof(uint32); index++) {
        ret = vfs_write(fp, (uint8 *)(&buff[index]), sizeof(uint16), &pos);
        if (ret < 0) {
            PS_PRINT_ERR("write %s fail, ret = %d\n", EFUSE_FILE_PATH, (int32)ret);
            filp_close(fp, NULL);
            set_fs(fs);
            return -EFAIL;
        }
    }

    filp_close(fp, NULL);
    set_fs(fs);

    return SUCC;
}

void store_efuse_info_etc(void)
{
    int32 ret;
    uint32 buff[EFUSE_REG_NUM] = { 0x00 };
    static int32 retry_count = 0;

    if (retry_count >= EFUSE_RETRY) {
        return;
    }
    retry_count++;

    read_efuse_ec_version_etc();

    ret = check_efuse_file_exist();
    if (ret == SUCC) {
        retry_count = EFUSE_RETRY;
        return;
    }

    ret = get_efuse_from_device(buff, sizeof(buff));
    if (ret != SUCC) {
        PS_PRINT_ERR("get efuse from device fail\n");
        return;
    }

    ret = store_efuse_into_file(buff, sizeof(buff));
    if (ret != SUCC) {
        PS_PRINT_ERR("store efuse into %s fail\n", EFUSE_FILE_PATH);
        return;
    }

    retry_count = EFUSE_RETRY;
}
