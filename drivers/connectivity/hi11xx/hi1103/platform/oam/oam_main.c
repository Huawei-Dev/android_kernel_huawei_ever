

/* ?????????? */
#include "oam_main.h"
#include "oam_log.h"
#include "oam_event.h"
#include "oam_alarm.h"
#include "oam_trace.h"
#include "oam_statistics.h"
#if (!defined(_PRE_PRODUCT_ID_HI110X_DEV))
#include "oam_config.h"
#endif
#include "oam_linux_netlink.h"
#include "oam_ext_if.h"
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
#include "oal_hcc_host_if.h"
#endif
#include "plat_pm_wlan.h"
#include "securec.h"

/* OAM??????????????????????????????????????OAM???????????????????? */
oam_mng_ctx_stru oam_mng_ctx_etc;

/* ???????????????? */
OAL_STATIC oal_print_func oam_print_type_func[OAM_OUTPUT_TYPE_BUTT] = {
    oam_print_to_console_etc, /* OAM_OUTPUT_TYPE_CONSOLE ?????????? */
    oam_print_to_file_etc,    /* OAM_OUTPUT_TYPE_FS ???????????? */
    oam_print_to_sdt_etc,     /* OAM_OUTPUT_TYPE_SDT ??????SDT,??????????????????2048 */
};

/* ??????SDT?????????????????? */
oam_sdt_func_hook_stru oam_sdt_func_hook_etc;
oam_wal_func_hook_stru oam_wal_func_hook_etc;
oam_sdt_stat_info_stru sdt_stat_info_etc;

oal_uint8 bcast_addr_etc[WLAN_MAC_ADDR_LEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

#ifdef _PRE_DEBUG_MODE /* ???????????????????? */
oal_uint32 debug_feature_switch_etc[OAM_DEBUG_TYPE_BUTT] = {
    OAL_SWITCH_OFF, /* OAM_DEBUG_TYPE_ECHO_REG */
};
#endif

/* ???????? */
oam_software_feature_stru gst_oam_feature_list_etc[OAM_SOFTWARE_FEATURE_BUTT] = {
    /* ??????ID                  ?????????? */
    /* 0 */
    { OAM_SF_SCAN,         "scan" },
    { OAM_SF_AUTH,         "auth" },
    { OAM_SF_ASSOC,        "assoc" },
    { OAM_SF_FRAME_FILTER, "ff" },
    { OAM_SF_WMM,          "wmm" },

    /* 5 */
    { OAM_SF_DFS,              "dfs" },
    { OAM_SF_NETWORK_MEASURE,  "nm" },
    { OAM_SF_ENTERPRISE_VO,    "ev" },
    { OAM_SF_HOTSPOTROAM,      "roam" },
    { OAM_SF_NETWROK_ANNOUNCE, "11u" },

    /* 10 */
    { OAM_SF_NETWORK_MGMT, "11k" },
    { OAM_SF_NETWORK_PWS,  "pws" },
    { OAM_SF_PROXYARP,     "proxyarp" },
    { OAM_SF_TDLS,         "tdls" },
    { OAM_SF_CALIBRATE,    "cali" },

    /* 15 */
    { OAM_SF_EQUIP_TEST, "equip" },
    { OAM_SF_CRYPTO,     "crypto" },
    { OAM_SF_WPA,        "wpa" },
    { OAM_SF_WEP,        "wep" },
    { OAM_SF_WPS,        "wps" },

    /* 20 */
    { OAM_SF_PMF,   "pmf" },
    { OAM_SF_WAPI,  "wapi" },
    { OAM_SF_BA,    "ba" },
    { OAM_SF_AMPDU, "ampdu" },
    { OAM_SF_AMSDU, "amsdu" },

    /* 25 */
    { OAM_SF_STABILITY, "dfr" },
    { OAM_SF_TCP_OPT,   "tcp" },
    { OAM_SF_ACS,       "acs" },
    { OAM_SF_AUTORATE,  "autorate" },
    { OAM_SF_TXBF,      "txbf" },

    /* 30 */
    { OAM_SF_DYN_RECV,      "weak" },
    { OAM_SF_VIVO,          "vivo" },
    { OAM_SF_MULTI_USER,    "muser" },
    { OAM_SF_MULTI_TRAFFIC, "mtraff" },
    { OAM_SF_ANTI_INTF,     "anti_intf" },

    /* 35 */
    { OAM_SF_EDCA,          "edca" },
    { OAM_SF_SMART_ANTENNA, "ani" },
    { OAM_SF_TPC,           "tpc" },
    { OAM_SF_TX_CHAIN,      "txchain" },
    { OAM_SF_RSSI,          "rssi" },

    /* 40 */
    { OAM_SF_WOW,      "wow" },
    { OAM_SF_GREEN_AP, "green" },
    { OAM_SF_PWR,      "pwr" },
    { OAM_SF_SMPS,     "smps" },
    { OAM_SF_TXOP,     "txop" },

    /* 45 */
    { OAM_SF_WIFI_BEACON, "beacon" },
    { OAM_SF_KA_AP,       "alive" },
    { OAM_SF_MULTI_VAP,   "mvap" },
    { OAM_SF_2040,        "2040" },
    { OAM_SF_DBAC,        "dbac" },

    /* 50 */
    { OAM_SF_PROXYSTA, "proxysta" },
    { OAM_SF_UM,       "um" },
    { OAM_SF_P2P,      "p2p" },
    { OAM_SF_M2U,      "m2u" },
    { OAM_SF_IRQ,      "irq" },

    /* 55 */
    { OAM_SF_TX,       "tx" },
    { OAM_SF_RX,       "rx" },
    { OAM_SF_DUG_COEX, "dugcoex" },
    { OAM_SF_CFG,      "cfg" },
    { OAM_SF_FRW,      "frw" },

    /* 60 */
    { OAM_SF_KEEPALIVE, "keepalive" },
    { OAM_SF_COEX,      "coex" },
    { OAM_SF_HS20,      "hs20" },
    { OAM_SF_MWO_DET,   "mwodet" },
    { OAM_SF_CCA_OPT,   "cca" },

    /* 65 */
    { OAM_SF_ROAM,   "roam" },
    { OAM_SF_DFT,    "dft" },
    { OAM_SF_DFR,    "dfr" },
    { OAM_SF_RRM,    "rrm" },
    { OAM_SF_VOWIFI, "vowifi" },

    /* 70 */
    { OAM_SF_OPMODE, "opmode" },
    { OAM_SF_M2S,    "m2s" },
    { OAM_SF_DBDC,   "dbdc" },
    { OAM_SF_HILINK, "hilink" },
    { OAM_SF_WDS,    "wds" },

    /* 75 */
    { OAM_SF_WMMAC,       "wmmac" },
    { OAM_SF_USER_EXTEND, "ue" },
    { OAM_SF_PKT_CAP,     "pktcap" },
    { OAM_SF_SOFT_CRYPTO, "crypto" },
    { OAM_SF_CAR,         "car" }, /* ????????  */
    { OAM_SF_11AX,        "11ax" },
    { OAM_SF_CSA,         "csa" },
    { OAM_SF_QOS,         "qos" },
    { OAM_SF_RESERVE4,    "rev4" },
    { OAM_SF_RESERVE5,    "rev5" },
    { OAM_SF_RESERVE6,    "rev6" },
    { OAM_SF_RESERVE7,    "rev7" },
    { OAM_SF_RESERVE8,    "rev8" },
    { OAM_SF_RESERVE9,    "rev9" },
    { OAM_SF_RESERVE10,   "rev10" },
    { OAM_SF_CONN,        "conn" },
    { OAM_SF_CHAN,        "chnn" },
    { OAM_SF_CUSTOM,      "custom" },

    { OAM_SF_ANY, "any" },
};

/*
 * ?? ?? ??  : oam_print_etc
 * ????????  : OAM??????????????????
 */
oal_uint32 oam_print_etc(const char *pc_string)
{
    oam_output_type_enum_uint8 en_output_type;
    oal_uint32 ul_rslt;

    if (OAL_UNLIKELY(pc_string == NULL)) {
        OAL_WARN_ON(1);
        return OAL_FAIL;
    }

    ul_rslt = oam_get_output_type_etc(&en_output_type);
    if (ul_rslt != OAL_SUCC) {
        return ul_rslt;
    }

    ul_rslt = oam_print_type_func[en_output_type](pc_string);
    if (ul_rslt != OAL_SUCC) {
        return ul_rslt;
    }

    return OAL_SUCC;
}

/*
 * ?? ?? ??  : oam_print_to_console_etc
 * ????????  : ????????????????????????
 */
oal_uint32 oam_print_to_console_etc(const char *pc_string)
{
    if (OAL_UNLIKELY(pc_string == OAL_PTR_NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAL_IO_PRINT("%s\r\n", pc_string);

    return OAL_SUCC;
}

/*
 * ?? ?? ??  : oam_print_to_file_etc
 * ????????  : pc_string : ????????????????????????????\0??????
 */
oal_uint32 oam_print_to_file_etc(const char *pc_string)
{
#ifdef _PRE_WIFI_DMT

    oal_file_stru *f_file_ret = NULL; /* ???????????????????????? */
    oal_file_stru *f_event_file = NULL;
    oal_int32 l_rslt;

    if (OAL_UNLIKELY(pc_string == OAL_PTR_NULL)) {
        OAM_IO_PRINTK("null param. \r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    f_event_file = oal_file_open_append(oam_mng_ctx_etc.ac_file_path);
    if (OAL_UNLIKELY(f_event_file == OAL_FILE_FAIL)) {
        OAM_IO_PRINTK("open file failed. \r\n");
        return OAL_ERR_CODE_OPEN_FILE_FAIL;
    }

    f_file_ret = oal_file_write(f_event_file, pc_string, (OAL_STRLEN(pc_string) + 1));
    if (f_file_ret == OAL_FILE_FAIL) {
        l_rslt = oal_file_close(f_event_file);
        if (l_rslt != 0) {
            OAM_IO_PRINTK("close file failed. \r\n");
            return OAL_ERR_CODE_CLOSE_FILE_FAIL;
        }

        OAM_IO_PRINTK("write file failed. \r\n");
        return OAL_ERR_CODE_WRITE_FILE_FAIL;
    }

    l_rslt = oal_file_close(f_event_file);

    if (l_rslt != 0) {
        OAM_IO_PRINTK("close file failed. \r\n");
        return OAL_ERR_CODE_CLOSE_FILE_FAIL;
    }
#endif
    return OAL_SUCC;
}

/*
 * ?? ?? ??  : oam_print_to_sdt_etc
 * ????????  : ??????????PC????????????????????
 */
oal_uint32 oam_print_to_sdt_etc(const char *pc_string)
{
    oal_netbuf_stru *pst_skb = NULL;
    oal_uint32 ul_ret;
    oal_uint16 us_strlen;

    if (OAL_UNLIKELY(pc_string == OAL_PTR_NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (OAL_UNLIKELY(oam_sdt_func_hook_etc.p_sdt_report_data_func == OAL_PTR_NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ????sdt????????'0'???? */
    us_strlen = (oal_uint16)OAL_STRLEN(pc_string);
    if (us_strlen == 0) {
        OAL_IO_PRINT("us_strlen is 0\n");
        return ERANGE;
    }

    us_strlen = (us_strlen > OAM_REPORT_MAX_STRING_LEN) ? OAM_REPORT_MAX_STRING_LEN : us_strlen;

    pst_skb = oam_alloc_data2sdt_etc(us_strlen);
    if (pst_skb == OAL_PTR_NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* COPY?????????? */
    memset_s(oal_netbuf_data(pst_skb), us_strlen, 0, us_strlen);
    ul_ret = memcpy_s(oal_netbuf_data(pst_skb), pst_skb->len, pc_string, (oal_uint32)us_strlen);
    if (ul_ret != EOK) {
        OAL_IO_PRINT("oam_print_to_sdt_etc::netbuf data copy failed.\n");
        oal_mem_sdt_netbuf_free_etc(pst_skb, OAL_TRUE);
        return OAL_FAIL;
    }

    /* ??????sdt???????????????????????????? */
    ul_ret = oam_report_data2sdt_etc(pst_skb, OAM_DATA_TYPE_STRING, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

oal_uint32 oam_upload_log_to_sdt_etc(oal_int8 *pc_string)
{
    oal_netbuf_stru *pst_skb = NULL;
    oal_uint32 ul_ret;

    if (OAL_UNLIKELY(oam_sdt_func_hook_etc.p_sdt_report_data_func == OAL_PTR_NULL)) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (pc_string == OAL_PTR_NULL) {
        OAL_IO_PRINT("oam_upload_log_to_sdt_etc::pc_string is null!\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_skb = oam_alloc_data2sdt_etc(OAL_SIZEOF(oam_log_info_stru));
    if (pst_skb == OAL_PTR_NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* COPY?????????? */
    ul_ret = memcpy_s(oal_netbuf_data(pst_skb), pst_skb->len, pc_string, OAL_SIZEOF(oam_log_info_stru));
    if (ul_ret != EOK) {
        OAL_IO_PRINT("oam_upload_log_to_sdt_etc::netbuf data copy failed\n");
        return OAL_FAIL;
    }
    /* ??????sdt???????????????????????????? */
    ul_ret = oam_report_data2sdt_etc(pst_skb, OAM_DATA_TYPE_LOG, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

oal_uint32 oam_upload_device_log_to_sdt_etc(oal_uint8 *pc_string, oal_uint16 len)
{
    oal_netbuf_stru *pst_skb = NULL;
    oal_uint32 ul_ret;

    if (pc_string == OAL_PTR_NULL) {
        OAL_IO_PRINT("oam_upload_log_to_sdt_etc::pc_string is null!\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if (len == 0) {
        OAL_IO_PRINT("len is 0\n");
        return ERANGE;
    }

    pst_skb = oam_alloc_data2sdt_etc(len);
    if (pst_skb == OAL_PTR_NULL) {
        OAL_IO_PRINT("alloc netbuf stru failed!\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* COPY?????????? */
    ul_ret = memcpy_s(oal_netbuf_data(pst_skb), pst_skb->len, pc_string, len);
    if (ul_ret != EOK) {
        OAL_IO_PRINT("netbuf valid data load failed!\n");
        oal_mem_sdt_netbuf_free_etc(pst_skb, OAL_TRUE);
        return ul_ret;
    }

    /* ??????sdt???????????????????????????? */
    ul_ret = oam_report_data2sdt_etc(pst_skb, OAM_DATA_TYPE_DEVICE_LOG, OAM_PRIMID_TYPE_OUTPUT_CONTENT);

    return ul_ret;
}

oal_uint32 oam_send_device_data2sdt_etc(oal_uint8 *pc_string, oal_uint16 len)
{
    oal_uint32 ul_ret;
    if (pc_string == NULL) {
        return OAL_EFAIL;
    }

    ul_ret = oam_upload_device_log_to_sdt_etc(pc_string, len);

    return ul_ret;
}

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
oal_int32 oam_rx_post_action_function_etc(struct hcc_handler *hcc, oal_uint8 stype,
                                          hcc_netbuf_stru *pst_hcc_netbuf, oal_uint8 *pst_context)
{
    oal_uint8 *puc_data;
    OAL_REFERENCE(pst_context);
    OAL_REFERENCE(hcc);

    if (OAL_WARN_ON(pst_hcc_netbuf == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: pst_hcc_netbuf is null", __FUNCTION__);
        return OAL_FAIL;
    };

    puc_data = oal_netbuf_data(pst_hcc_netbuf->pst_netbuf);
    if ((stype == DUMP_REG) || (stype == DUMP_MEM)) {
        exception_bcpu_dump_recv_etc(puc_data, pst_hcc_netbuf->pst_netbuf);
        oal_netbuf_free(pst_hcc_netbuf->pst_netbuf);
        return OAL_SUCC;
    }

    /* ????OAM???? */
    oam_send_device_data2sdt_etc(puc_data, (oal_uint16)pst_hcc_netbuf->len);

    oal_netbuf_free(pst_hcc_netbuf->pst_netbuf);
    return OAL_SUCC;
}
oal_int32 chr_rx_post_action_function_etc(struct hcc_handler *hcc, oal_uint8 stype,
                                          hcc_netbuf_stru *pst_hcc_netbuf, oal_uint8 *pst_context)
{
    oal_uint8 *puc_data;
    OAL_REFERENCE(pst_context);
    OAL_REFERENCE(hcc);

    if (OAL_WARN_ON(pst_hcc_netbuf == NULL)) {
        oal_print_hi11xx_log(HI11XX_LOG_ERR, "%s error: pst_hcc_netbuf is null", __FUNCTION__);
        return OAL_FAIL;
    };

#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    if (wlan_pm_wkup_src_debug_get() == OAL_TRUE) {
        OAL_IO_PRINT("wifi_wake_src:rx chr error!\n");
        wlan_pm_wkup_src_debug_set(OAL_FALSE);
    }
#endif

    puc_data = oal_netbuf_data(pst_hcc_netbuf->pst_netbuf);
#ifdef _PRE_CONFIG_HW_CHR
    chr_dev_exception_callback_etc(puc_data, (oal_uint16)pst_hcc_netbuf->len);
#endif
    oal_netbuf_free(pst_hcc_netbuf->pst_netbuf);
    return OAL_SUCC;
}

#endif

oal_uint32 oam_get_output_type_etc(oam_output_type_enum_uint8 *pen_output_type)
{
    if (OAL_UNLIKELY(pen_output_type == NULL)) {
        OAL_WARN_ON(1);
        return OAL_FAIL;
    }
    *pen_output_type = oam_mng_ctx_etc.en_output_type;

    return OAL_SUCC;
}

oal_uint32 oam_set_output_type_etc(oam_output_type_enum_uint8 en_output_type)
{
    if (en_output_type >= OAM_OUTPUT_TYPE_BUTT) {
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    oam_mng_ctx_etc.en_output_type = en_output_type;

    return OAL_SUCC;
}

/*
 * ?? ?? ??  : oam_set_file_path_etc
 * ????????  : 1) ????????????????????????
 *             2) ????????????????????(????\0)
 */
oal_uint32 oam_set_file_path_etc(oal_int8 *pc_file_path, oal_uint32 ul_length)
{
#ifdef _PRE_WIFI_DMT
    oal_file_stru *f_event_file = NULL;
    oal_uint8 *puc_file_path = NULL;

    if (pc_file_path == OAL_PTR_NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (ul_length > OAM_FILE_PATH_LENGTH) {
        return OAL_ERR_CODE_ARRAY_OVERFLOW;
    }

    puc_file_path = DmtStub_GetDebugFilePath();
    if (memcpy_s(oam_mng_ctx_etc.ac_file_path, sizeof(oam_mng_ctx_etc.ac_file_path),
                 puc_file_path, strlen(puc_file_path)) != EOK) {
        OAL_IO_PRINT("memcpy_s error, destlen=%u, srclen=%u\n ",
                     sizeof(oam_mng_ctx_etc.ac_file_path), strlen(puc_file_path));
        return -OAL_EFAIL;
    }

    /* ???????????????????????????????????? */
    f_event_file = oal_file_open_rw(oam_mng_ctx_etc.ac_file_path);
    if (f_event_file == OAL_FILE_FAIL) {
        return OAL_ERR_CODE_WRITE_FILE_FAIL;
    }

    if (oal_file_close(f_event_file) != 0) {
        return OAL_ERR_CODE_CLOSE_FILE_FAIL;
    }

#else

    if (pc_file_path == OAL_PTR_NULL) {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (memcpy_s(oam_mng_ctx_etc.ac_file_path, sizeof(oam_mng_ctx_etc.ac_file_path),
                 pc_file_path, ul_length) != EOK) {
        OAL_IO_PRINT("memcpy_s error, destlen=%u, srclen=%u\n ",
                     (oal_uint32)sizeof(oam_mng_ctx_etc.ac_file_path), ul_length);
        return -OAL_EFAIL;
    }

#endif

    return OAL_SUCC;
}

/*
 * ?? ?? ??  : oam_dump_buff_by_hex_etc
 * ????????  : buff????????????????????????????????????l_num??????
 */
oal_void oam_dump_buff_by_hex_etc(oal_uint8 *puc_buff, oal_int32 l_len, oal_int32 l_num)
{
    oal_int32 l_loop;

    for (l_loop = 0; l_loop < l_len; l_loop++) {
        OAL_IO_PRINT("%02x ", puc_buff[l_loop]);

        if ((l_loop + 1) % l_num == 0) {
            OAL_IO_PRINT("\n");
        }
    }

    OAL_IO_PRINT("\n");
}

OAL_STATIC oal_void oam_drv_func_hook_init(oal_void)
{
    /* sdt???????????????????? */
    oam_sdt_func_hook_etc.p_sdt_report_data_func = OAL_PTR_NULL;
    oam_sdt_func_hook_etc.p_sdt_get_wq_len_func = OAL_PTR_NULL;

    /* wal???????????????????? */
    oam_wal_func_hook_etc.p_wal_recv_cfg_data_func = OAL_PTR_NULL;
    oam_wal_func_hook_etc.p_wal_recv_mem_data_func = OAL_PTR_NULL;
    oam_wal_func_hook_etc.p_wal_recv_reg_data_func = OAL_PTR_NULL;
    oam_wal_func_hook_etc.p_wal_recv_global_var_func = OAL_PTR_NULL;
}

/*
 * ?? ?? ??  : oam_sdt_func_fook_register_etc
 * ????????  : oam????????sdt??????????????,??????????????????
 */
oal_void oam_sdt_func_fook_register_etc(oam_sdt_func_hook_stru *pfun_st_oam_sdt_hook)
{
    if (OAL_UNLIKELY(pfun_st_oam_sdt_hook == NULL)) {
        OAL_WARN_ON(1);
        return;
    }
    oam_sdt_func_hook_etc.p_sdt_report_data_func = pfun_st_oam_sdt_hook->p_sdt_report_data_func;
    oam_sdt_func_hook_etc.p_sdt_get_wq_len_func = pfun_st_oam_sdt_hook->p_sdt_get_wq_len_func;
}

/*
 * ?? ?? ??  : oam_wal_func_fook_register_etc
 * ????????  : oam????????wal??????????????,??????????????????
 */
oal_void oam_wal_func_fook_register_etc(oam_wal_func_hook_stru *pfun_st_oam_wal_hook)
{
    if (OAL_UNLIKELY(pfun_st_oam_wal_hook == NULL)) {
        OAL_WARN_ON(1);
        return;
    }
    oam_wal_func_hook_etc.p_wal_recv_cfg_data_func = pfun_st_oam_wal_hook->p_wal_recv_cfg_data_func;
    oam_wal_func_hook_etc.p_wal_recv_mem_data_func = pfun_st_oam_wal_hook->p_wal_recv_mem_data_func;
    oam_wal_func_hook_etc.p_wal_recv_reg_data_func = pfun_st_oam_wal_hook->p_wal_recv_reg_data_func;
    oam_wal_func_hook_etc.p_wal_recv_global_var_func = pfun_st_oam_wal_hook->p_wal_recv_global_var_func;
#if defined(_PRE_WLAN_FEATURE_DATA_SAMPLE) || defined(_PRE_WLAN_FEATURE_PSD_ANALYSIS)
    oam_wal_func_hook_etc.p_wal_recv_sample_data_func = pfun_st_oam_wal_hook->p_wal_recv_sample_data_func;
#endif
#ifdef _PRE_WLAN_RF_AUTOCALI
    oam_wal_func_hook_etc.p_wal_recv_autocali_data_func = pfun_st_oam_wal_hook->p_wal_recv_autocali_data_func;
#endif
}

/*
 * ?? ?? ??  : oam_filter_data2sdt_etc
 * ????????  : sdt????????????????????
 */
oal_uint32 oam_filter_data2sdt_etc(oam_data_type_enum_uint8 en_type)
{
    if (sdt_stat_info_etc.ul_wq_len < WLAN_SDT_MSG_FLT_HIGH_THD) {
        sdt_stat_info_etc.en_filter_switch = OAL_FALSE;
        return OAM_FLT_PASS;
    } else if ((sdt_stat_info_etc.ul_wq_len >= WLAN_SDT_MSG_FLT_HIGH_THD)
               && (sdt_stat_info_etc.ul_wq_len < WLAN_SDT_MSG_QUEUE_MAX_LEN)) {
        /* ???????????????????????????????????? */
        sdt_stat_info_etc.en_filter_switch = OAL_TRUE;

        return (en_type == ((oal_uint8)OAM_DATA_TYPE_LOG)) ? OAM_FLT_PASS : OAM_FLT_DROP;
    }

    /* ?????????????????? */
    return OAM_FLT_DROP;
}

/*
 * ?? ?? ??  : oam_alloc_data2sdt_etc
 * ????????  : ????????sdt app????netlink????
 *             1) ul_data_len??oal_mem_sdt_netbuf_alloc ????????????????SDT??????????????
 *             2) ????????netbuf??????????????????????
 *             SDT??NLK????????????????????????????????????????????
 */
oal_netbuf_stru *oam_alloc_data2sdt_etc(oal_uint16 us_data_len)
{
    oal_netbuf_stru *pst_netbuf = OAL_PTR_NULL;
#if ((_PRE_OS_VERSION_RAW != _PRE_OS_VERSION) && (_PRE_OS_VERSION_WIN32_RAW != _PRE_OS_VERSION))
    pst_netbuf = oal_mem_sdt_netbuf_alloc_etc(us_data_len + WLAN_SDT_SKB_RESERVE_LEN, OAL_TRUE);
    if (pst_netbuf == OAL_PTR_NULL) {
        OAL_IO_PRINT("oal_mem_sdt_netbuf_alloc_etc failed!\n");
        return OAL_PTR_NULL;
    }

    oal_netbuf_reserve(pst_netbuf, WLAN_SDT_SKB_HEADROOM_LEN);

    oal_netbuf_put(pst_netbuf, us_data_len);
#endif
    return pst_netbuf;
}

/*
 * ?? ?? ??  : oam_report_data2sdt_etc
 * ????????  : oam????????????sdt??????sdt????????????app??
 */
oal_uint32 oam_report_data2sdt_etc(oal_netbuf_stru *pst_netbuf,
                                   oam_data_type_enum_uint8 en_type,
                                   oam_primid_type_enum_uint8 en_prim)
{
    if (OAL_UNLIKELY(pst_netbuf == NULL)) {
        OAL_WARN_ON(1);
        return OAL_FAIL;
    }
    /* ????sdt???????????????????????????????????? */
    if (OAL_LIKELY(oam_sdt_func_hook_etc.p_sdt_get_wq_len_func != OAL_PTR_NULL)) {
        sdt_stat_info_etc.ul_wq_len = (oal_uint32)oam_sdt_func_hook_etc.p_sdt_get_wq_len_func();
    }

    if (oam_filter_data2sdt_etc(en_type) != OAM_FLT_PASS) {
        OAM_SDT_STAT_INCR(ul_filter_cnt);
        oal_mem_sdt_netbuf_free_etc(pst_netbuf, OAL_TRUE);

        /* Note: ????????????????????????????????warning???????? */
        return OAL_FAIL;
    }

    if (OAL_UNLIKELY(oam_sdt_func_hook_etc.p_sdt_report_data_func == OAL_PTR_NULL)) {
        OAL_IO_PRINT("oam_report_data2sdt_etc p_sdt_report_data_func is NULL. \n");
        return OAL_FAIL;
    }

    oam_sdt_func_hook_etc.p_sdt_report_data_func(pst_netbuf, en_type, en_prim);

    return OAL_SUCC;
}

oal_void oam_sdt_func_fook_unregister_etc(oal_void)
{
    /* ???????????? */
    oam_sdt_func_hook_etc.p_sdt_report_data_func = OAL_PTR_NULL;
    oam_sdt_func_hook_etc.p_sdt_get_wq_len_func = OAL_PTR_NULL;
}

/*
 * ?? ?? ??  : oam_wal_func_fook_unregister_etc
 * ????????  : wal??????????????????
 */
oal_void oam_wal_func_fook_unregister_etc(oal_void)
{
    /* ???????????? */
    oam_wal_func_hook_etc.p_wal_recv_cfg_data_func = OAL_PTR_NULL;
    oam_wal_func_hook_etc.p_wal_recv_global_var_func = OAL_PTR_NULL;
    oam_wal_func_hook_etc.p_wal_recv_mem_data_func = OAL_PTR_NULL;
    oam_wal_func_hook_etc.p_wal_recv_reg_data_func = OAL_PTR_NULL;
}

/*
 * ?? ?? ??  : oam_main_init_etc
 * ????????  : OAM??????????????????????OAM??????????????????????????
 */
oal_int32 oam_main_init_etc(oal_void)
{
    oal_uint32 ul_rslt;

#ifdef _PRE_WLAN_REPORT_PRODUCT_LOG
    oam_pdt_log_init();
#endif

    /* ????????????????FILE???? */
    ul_rslt = oam_set_file_path_etc(WLAN_OAM_FILE_PATH, (OAL_STRLEN(WLAN_OAM_FILE_PATH) + 1));
    if (ul_rslt != OAL_SUCC) {
        OAL_IO_PRINT("oam_main_init_etc call oam_set_file_path_etc fail %d\n", ul_rslt);
        return -OAL_EFAIL;  //lint !e527
    }

    /* ?????????????????????? */
    ul_rslt = oam_set_output_type_etc(OAM_OUTPUT_TYPE_SDT);
    if (ul_rslt != OAL_SUCC) {
        OAL_IO_PRINT("oam_main_init_etc call oam_set_output_type_etc fail %d\n", ul_rslt);
        return -OAL_EFAIL;  //lint !e527
    }

    /* ????LOG???????????????? */
    ul_rslt = oam_log_init_etc();
    if (ul_rslt != OAL_SUCC) {
        OAL_IO_PRINT("oam_main_init_etc call oam_log_init_etc fail %d\n", ul_rslt);
        return -OAL_EFAIL;  //lint !e527
    }

    /* ????EVENT???????????????? */
    ul_rslt = oam_event_init_etc();
    if (ul_rslt != OAL_SUCC) {
        OAL_IO_PRINT("oam_main_init_etc call oam_event_init_etc fail %d\n", ul_rslt);
        return -OAL_EFAIL;  //lint !e527
    }

    /* ??????5115timer???????????????????????????? */
    oal_5115timer_init();

#ifdef _PRE_PROFILING_MODE
    /* ????PROFILING???????????????? */
    ul_rslt = oam_profiling_init();
    if (ul_rslt != OAL_SUCC) {
        OAL_IO_PRINT("oam_main_init_etc call oam_profiling_init fail %d\n", ul_rslt);
        return -OAL_EFAIL;
    }
#endif

    /* ??????oam?????????????? */
    oam_drv_func_hook_init();

    /* ?????????????? */
    oam_statistics_init_etc();

#ifdef _PRE_WLAN_DFT_REG
    oam_reg_init_etc();
#endif

#ifndef BFGX_UART_DOWNLOAD_SUPPORT
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    hcc_rx_register_etc(hcc_get_110x_handler(), HCC_ACTION_TYPE_OAM, oam_rx_post_action_function_etc, NULL);
    hcc_rx_register_etc(hcc_get_110x_handler(), HCC_ACTION_TYPE_CHR, chr_rx_post_action_function_etc, NULL);
#endif
#endif
    return OAL_SUCC;
}

oal_void oam_main_exit_etc(oal_void)
{
    /* ??????5115timer???????????????????????????? */
    oal_5115timer_exit();

#ifdef _PRE_WLAN_DFT_REG
    oam_reg_exit_etc();
#endif
/* ???????????????????????? */
#ifdef _PRE_WLAN_REPORT_PRODUCT_LOG
    oam_pdt_log_exit();
#endif

    return;
}

/*lint -e578*/ /*lint -e19*/
/*lint -e19*/
oal_module_symbol(oam_main_init_etc);
oal_module_symbol(oam_main_exit_etc);
oal_module_symbol(oam_send_device_data2sdt_etc);
oal_module_symbol(oam_set_file_path_etc);
oal_module_symbol(oam_set_output_type_etc);
oal_module_symbol(oam_get_output_type_etc);
oal_module_symbol(oam_print_etc);
oal_module_symbol(oam_mng_ctx_etc);
oal_module_symbol(oam_dump_buff_by_hex_etc);
oal_module_symbol(oam_sdt_func_hook_etc);
oal_module_symbol(oam_wal_func_hook_etc);
oal_module_symbol(oam_sdt_func_fook_register_etc);
oal_module_symbol(oam_sdt_func_fook_unregister_etc);
oal_module_symbol(oam_wal_func_fook_register_etc);
oal_module_symbol(oam_wal_func_fook_unregister_etc);
oal_module_symbol(oam_report_data2sdt_etc);
oal_module_symbol(sdt_stat_info_etc);
oal_module_symbol(oam_alloc_data2sdt_etc);
oal_module_symbol(gst_oam_feature_list_etc);
oal_module_symbol(bcast_addr_etc);

#ifdef _PRE_DEBUG_MODE
oal_module_symbol(debug_feature_switch_etc);
#endif

oal_module_license("GPL");
