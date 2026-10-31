


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ??????????
*****************************************************************************/
#include "oal_ext_if.h"
#include "oal_profiling.h"
#include "oal_kernel_file.h"
#include "oal_cfg80211.h"

#include "oam_ext_if.h"
#include "frw_ext_if.h"
#if (_PRE_FRW_FEATURE_PROCCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCCESS_ENTITY_THREAD)
#ifndef  CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT
#include "frw_task.h"
#endif
#endif

#include "wlan_spec.h"
#include "wlan_types.h"

#include "mac_vap.h"
#include "mac_resource.h"
#include "mac_regdomain.h"
#include "mac_ie.h"

#include "hmac_ext_if.h"
#include "hmac_chan_mgmt.h"
#include "hmac_tx_data.h"
#include "wal_main.h"
#include "wal_ext_if.h"
#include "wal_config.h"
#include "wal_regdb.h"
#include "wal_linux_scan.h"
#include "wal_linux_ioctl.h"
#include "wal_linux_bridge.h"
#include "wal_linux_flowctl.h"
#include "wal_linux_atcmdsrv.h"
#include "wal_linux_event.h"
#include "hmac_resource.h"
#include "hmac_p2p.h"

#include "wal_linux_cfg80211.h"
#include "wal_linux_cfgvendor.h"

#include "wal_dfx.h"
#include "hmac_isolation.h"

#ifdef _PRE_WLAN_FEATURE_P2P
#include "wal_linux_cfg80211.h"
#endif

#include "wal_dfx.h"


#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#include "oal_hcc_host_if.h"
#include "plat_cali.h"
#endif

#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/notifier.h>
#include <linux/inetdevice.h>
#include <net/addrconf.h>
#endif
#include "hmac_arp_offload.h"
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,59)) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "../fs/proc/internal.h"
#endif

#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
#include "hmac_auto_adjust_freq.h"
#endif
#ifdef _PRE_WLAN_FEATURE_MULTI_NETBUF_AMSDU
#include "hmac_tx_amsdu.h"
#endif
#ifdef _PRE_WLAN_FEATURE_WDS
#include "hmac_wds.h"
#endif

#ifdef _PRE_WLAN_FEATURE_ROAM
#include "hmac_roam_main.h"
#endif //_PRE_WLAN_FEATURE_ROAM
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#include "oal_sdio_comm.h"
#include "oal_net.h"
#include "hisi_customize_wifi.h"
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "plat_pm_wlan.h"
#include "plat_firmware.h"
#endif

#include "wal_linux_ioctl.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_LINUX_IOCTL_C
#define MAX_PRIV_CMD_SIZE          4096

#if (defined(CONFIG_BALONG_SPE) && defined(_PRE_WLAN_SPE_SUPPORT))
#define WAL_MAX_SPE_PKT_NUM       512      /* SPE???????????????????????????????????????? */
extern oal_int32 wifi_spe_port_alloc(oal_net_device_stru *pst_netdev, struct spe_port_attr *pst_attr);
extern void wifi_spe_port_free(oal_uint32 ul_port_num);
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "board.h"
#endif
#if defined (WIN32) && (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
#include "hisi_customize_wifi_hi110x.h"
#endif
#include "securec.h"
#include "securectype.h"

extern oal_uint32  wal_hipriv_fem_lowpower(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);

/*****************************************************************************
  2 ??????????
*****************************************************************************/
typedef struct
{
    oal_int8                    *pc_country;          /* ?????????? */
    mac_dfs_domain_enum_uint8    en_dfs_domain;       /* DFS ???????? */
}wal_dfs_domain_entry_stru;

typedef struct
{
    oal_uint32 ul_ap_max_user;                      /* ap?????????? */
    oal_int8   ac_ap_mac_filter_mode[257];          /* AP mac????????????????,????256 */
    oal_int32  l_ap_power_flag;                     /* AP???????? */
}wal_ap_config_stru;

/*****************************************************************************
  2 ????????????
*****************************************************************************/
mac_rssi_cfg_table_stru g_ast_mac_rssi_config_table[] =
{
    {"show_info",         MAC_RSSI_LIMIT_SHOW_INFO},
    {"enable",            MAC_RSSI_LIMIT_ENABLE},
    {"delta",             MAC_RSSI_LIMIT_DELTA},
    {"threshold",         MAC_RSSI_LIMIT_THRESHOLD}
};

#ifdef _PRE_FEATURE_FAST_AGING
mac_fast_aging_cfg_table_stru g_ast_dmac_fast_aging_config_table[] =
{
    {"enable",              MAC_FAST_AGING_ENABLE},
    {"timeout",             MAC_FAST_AGING_TIMEOUT},
    {"count",               MAC_FAST_AGING_COUNT},
};
#endif

#ifdef _PRE_WLAN_FEATURE_TCP_ACK_BUFFER
mac_tcp_ack_buf_cfg_table_stru g_ast_hmac_tcp_ack_buf_cfg_table[] =
{
    {"enable",              MAC_TCP_ACK_BUF_ENABLE},
    {"timeout",             MAC_TCP_ACK_BUF_TIMEOUT},
    {"count",               MAC_TCP_ACK_BUF_MAX},
};
#endif

#ifdef _PRE_WLAN_FEATURE_CAR
hmac_car_cfg_table_stru g_ast_hmac_car_config_table[] =
{
    {"device_bw",         MAC_CAR_DEVICE_LIMIT},
    {"vap_bw",            MAC_CAR_VAP_LIMIT},
    {"user_bw",           MAC_CAR_USER_LIMIT},
    {"timer_cycle",       MAC_CAR_TIMER_CYCLE_MS},
    {"car_enable",        MAC_CAR_ENABLE},
    {"show_info",         MAC_CAR_SHOW_INFO},
    {"multicast",         MAC_CAR_MULTICAST},
    {"multicast_pps",     MAC_CAR_MULTICAST_PPS}
};
#endif

/* 110x ?????????????????????????????? */
typedef struct {
    oal_uint8     *pc_priv_cmd;
    oal_uint32     ul_case_entry;
} wal_ioctl_priv_cmd_stru;

OAL_STATIC oal_proc_dir_entry_stru *g_pst_proc_entry = OAL_PTR_NULL;

OAL_STATIC wal_ap_config_stru g_st_ap_config_info = {0};    /* AP????????,??????vap ???????????? */

#if (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION)
/*Just For UT*/
OAL_STATIC oal_uint32 g_wal_wid_queue_init_flag = OAL_FALSE;
#endif
OAL_STATIC wal_msg_queue g_wal_wid_msg_queue;


/* hi1102-cb add sys for 51/02 */
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
    struct kobject     *gp_sys_kobject_etc;
#endif

uint8 g_auc_wifinvmac_etc[MAC_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 g_auc_wifistamac_etc[MAC_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 g_auc_wifiapmac_etc[MAC_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 g_auc_wifip2p0mac_etc[MAC_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8 g_auc_wifiGCGOmac_etc[MAC_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };



#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_int32 wal_hipriv_inetaddr_notifier_call_etc(struct notifier_block *this, oal_uint event, oal_void *ptr);

OAL_STATIC struct notifier_block wal_hipriv_notifier = {
    .notifier_call = wal_hipriv_inetaddr_notifier_call_etc
};

oal_int32 wal_hipriv_inet6addr_notifier_call_etc(struct notifier_block *this, oal_uint event, oal_void *ptr);

OAL_STATIC struct notifier_block wal_hipriv_notifier_ipv6 = {
    .notifier_call = wal_hipriv_inet6addr_notifier_call_etc
};
#endif
#endif

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
/* ????????????????vap????????????????????????wlan p2p iface???????????? */
OAL_STATIC oal_uint8 g_uc_cfg_once_flag = OAL_FALSE;
/* TBD:????????????wifi????????staut??aput??????????????ini?????????????????? */
//OAL_STATIC oal_uint8 g_uc_cfg_flag = OAL_FALSE;
#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
extern host_speed_freq_level_stru g_host_speed_freq_level_etc[4];
extern device_speed_freq_level_stru g_device_speed_freq_level_etc[4];
#endif
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */
#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
extern oal_bool_enum_uint8 aen_tas_switch_en[];
#endif

#if (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION) && (_PRE_TEST_MODE == _PRE_TEST_MODE_UT)
/* UT??????????frw_event_process_all_event */
extern oal_void  frw_event_process_all_event_etc(oal_uint ui_data);
#endif

#ifdef _PRE_WLAN_FEATURE_DFR
extern  hmac_dfr_info_stru    g_st_dfr_info_etc;
#endif //_PRE_WLAN_FEATURE_DFR

#ifdef _PRE_WLAN_CFGID_DEBUG
extern OAL_CONST wal_hipriv_cmd_entry_stru  g_ast_hipriv_cmd_debug_etc[];
#endif

/* ???????????? */
OAL_STATIC oal_uint32  wal_hipriv_vap_log_level(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_set_mcast_data_dscr_param(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);

OAL_STATIC oal_uint32  wal_hipriv_setcountry(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_getcountry(oal_net_device_stru *pst_net_dev,oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_set_bw(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
OAL_STATIC oal_uint32  wal_hipriv_always_tx(oal_net_device_stru * pst_net_dev, oal_int8 * pc_param);
OAL_STATIC oal_uint32  wal_hipriv_always_tx_num(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_always_tx_aggr_num(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_always_tx_hw_cfg(oal_net_device_stru * pst_net_dev, oal_int8 * pc_param);
OAL_STATIC oal_uint32  wal_hipriv_always_tx_hw(oal_net_device_stru * pst_net_dev, oal_int8 * pc_param);
#endif

OAL_STATIC oal_uint32  wal_hipriv_always_rx(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#ifdef _PRE_WLAN_FEATURE_MONITOR
OAL_STATIC oal_uint32  wal_hipriv_set_sniffer(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_enable_monitor_mode(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
OAL_STATIC oal_uint32  wal_hipriv_pcie_pm_level(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_user_info(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_add_vap(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param);

#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_DFT_EVENT
OAL_STATIC oal_void  wal_event_report_to_sdt(wal_msg_type_enum_uint8   en_msg_type, oal_uint8 *puc_param, wal_msg_stru *pst_cfg_msg);
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_LTECOEX
OAL_STATIC oal_uint32 wal_ioctl_ltecoex_mode_set(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_NRCOEX
OAL_STATIC oal_uint32 wal_ioctl_nrcoex_priority_set(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_WDS
OAL_STATIC oal_uint32  wal_hipriv_wds_vap_mode(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_wds_vap_show(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_wds_sta_add(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_wds_sta_del(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_wds_sta_age(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_DHCP_REQ_DISABLE
OAL_STATIC oal_uint32  wal_hipriv_dhcp_req_disable_switch(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32  wal_hipriv_aifsn_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_cw_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_SMPS
OAL_STATIC oal_uint32  wal_hipriv_set_smps_vap_mode(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_set_smps_mode(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif

OAL_STATIC oal_uint32  wal_hipriv_set_random_mac_addr_scan(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_add_user(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_del_user(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);

OAL_STATIC oal_uint32  wal_hipriv_reg_info(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);

#if (defined(_PRE_PRODUCT_ID_HI110X_DEV) || defined(_PRE_PRODUCT_ID_HI110X_HOST))
OAL_STATIC oal_uint32  wal_hipriv_sdio_flowctrl(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
#ifdef _PRE_WLAN_DELAY_STATISTIC
OAL_STATIC oal_uint32  wal_hipriv_pkt_time_switch(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
OAL_STATIC oal_uint32  wal_hipriv_set_2040_coext_support(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_rx_fcs_info(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
oal_int32  wal_netdev_open_etc(oal_net_device_stru *pst_net_dev,oal_uint8 uc_entry_flag);
OAL_STATIC oal_int32  wal_net_device_ioctl(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr, oal_int32 ul_cmd);
#if ((!defined(_PRE_PRODUCT_ID_HI110X_DEV)) && (!defined(_PRE_PRODUCT_ID_HI110X_HOST)))
OAL_STATIC oal_int32  wal_ioctl_get_assoc_req_ie(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32  wal_ioctl_set_auth_mode(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32  wal_ioctl_set_country_code(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32  wal_ioctl_set_max_user(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32  wal_ioctl_nl80211_priv_connect(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32  wal_ioctl_nl80211_priv_disconnect(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_uint32 wal_get_sta_info(oal_net_sta_link_info_stru *pst_all_sta_link_info,oam_user_stat_info_stru *pst_oam_user_stat,mac_user_stru *pst_mac_user);
OAL_STATIC oal_uint32 wal_get_sta_info_ext(oal_net_sta_link_info_ext_stru *pst_all_sta_link_info_ext,oam_user_stat_info_stru *pst_oam_user_stat,mac_user_stru *pst_mac_user);
OAL_STATIC oal_int32  wal_ioctl_get_all_sta_info(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32  wal_ioctl_set_vendor_ie(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32  wal_ioctl_get_sta_11h_ability(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
#ifdef _PRE_WLAN_FEATURE_11R_AP
OAL_STATIC oal_int32  wal_ioctl_get_sta_11r_ability(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32 wal_ioctl_set_mlme(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);

#endif
OAL_STATIC oal_int32 wal_ioctl_set_channel(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32 wal_ioctl_set_wps_ie(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32 wal_ioctl_set_frag(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
OAL_STATIC oal_int32 wal_ioctl_set_rts(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
oal_int32 wal_ioctl_set_ssid(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
#endif
OAL_STATIC oal_int32 wal_ioctl_set_mlme_ie_etc(oal_net_device_stru *pst_net_dev, oal_mlme_ie_stru *pst_mlme_ie);

OAL_STATIC oal_net_device_stats_stru*  wal_netdev_get_stats(oal_net_device_stru *pst_net_dev);
OAL_STATIC oal_int32  wal_netdev_set_mac_addr(oal_net_device_stru *pst_net_dev, void *p_addr);

OAL_STATIC oal_uint32  wal_hipriv_set_mode(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_set_freq(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);

/* E5 SPE module relation */
#if (defined(CONFIG_BALONG_SPE) && defined(_PRE_WLAN_SPE_SUPPORT))
OAL_STATIC oal_int32 wal_netdev_spe_init(oal_net_device_stru *pst_net_dev);
OAL_STATIC oal_void wal_netdev_spe_exit(oal_net_device_stru *pst_net_dev);
OAL_STATIC oal_int32 wal_finish_spe_rd(oal_int32 l_port_num, oal_int32 l_src_port_num, oal_netbuf_stru *pst_buf, oal_uint32 ul_dma, oal_uint32 ul_flags);
OAL_STATIC oal_int32 wal_finish_spe_td(oal_int32 l_port_num, oal_netbuf_stru *pst_buf, oal_uint32 ul_flags);
#endif /* defined(CONFIG_BALONG_SPE) && defined(_PRE_WLAN_SPE_SUPPORT) */

#if (defined(_PRE_E5_722_PLATFORM) || defined(_PRE_CPE_711_PLATFORM) || defined(_PRE_CPE_722_PLATFORM))
extern oal_void wlan_set_driver_lock(oal_int32 locked);
#endif

OAL_STATIC oal_uint32  wal_hipriv_rssi_limit(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#ifdef _PRE_FEATURE_FAST_AGING
OAL_STATIC oal_uint32  wal_hipriv_fast_aging_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_TCP_ACK_BUFFER
OAL_STATIC oal_uint32  wal_hipriv_tcp_ack_buf_cfg(oal_net_device_stru * pst_net_dev, oal_int8 * pc_param);
#endif



#ifdef _PRE_WLAN_FEATURE_CAR
OAL_STATIC oal_uint32  wal_hipriv_car_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
oal_int32 wal_ioctl_get_car_info(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
#endif
#if (_PRE_WLAN_FEATURE_BLACKLIST_LEVEL != _PRE_WLAN_FEATURE_BLACKLIST_NONE)
oal_int32 wal_ioctl_get_blkwhtlst(oal_net_device_stru *pst_net_dev, oal_net_dev_ioctl_data_stru *pst_ioctl_data);
#endif
#ifdef _PRE_WLAN_REPORT_PRODUCT_LOG
OAL_STATIC oal_uint32  wal_hipriv_report_product_log_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
OAL_STATIC int wal_ioctl_get_essid(oal_net_device_stru *pst_net_dev,
                                    oal_iw_request_info_stru *pst_info,
                                    oal_iwreq_data_union *pst_wrqu,
                                    char *pc_param);
OAL_STATIC int wal_ioctl_get_apaddr(oal_net_device_stru *pst_net_dev,
                                    oal_iw_request_info_stru *pst_info,
                                    oal_iwreq_data_union *pst_wrqu,
                                    char *pc_extra);
OAL_STATIC int wal_ioctl_get_iwname(oal_net_device_stru *pst_net_dev,
                                    oal_iw_request_info_stru *pst_info,
                                    oal_iwreq_data_union *pst_wrqu,
                                    char *pc_extra);
OAL_STATIC oal_uint32  wal_hipriv_set_regdomain_pwr(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_reg_write(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);


OAL_STATIC oal_uint32  wal_hipriv_tpc_log(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_dump_all_rx_dscr(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
OAL_STATIC oal_uint32   wal_hipriv_set_edca_opt_switch_sta(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_UAPSD
OAL_STATIC oal_uint32  wal_hipriv_set_uapsd_cap(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_DFS
OAL_STATIC oal_uint32  wal_hipriv_dfs_radartool(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif


OAL_STATIC oal_uint32  wal_hipriv_bgscan_enable(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param);
#ifdef _PRE_WLAN_FEATURE_STA_PM
OAL_STATIC oal_uint32  wal_hipriv_sta_ps_mode(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param);
#ifdef _PRE_PSM_DEBUG_MODE
OAL_STATIC oal_uint32  wal_hipriv_sta_ps_info(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param);
#endif
OAL_STATIC oal_uint32  wal_hipriv_set_fasts_sleep_para(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_SINGLE_CHIP_DUAL_BAND
OAL_STATIC oal_uint32  wal_hipriv_set_restrict_band(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
#if defined(_PRE_WLAN_FEATURE_DBAC) && defined(_PRE_WLAN_FEATRUE_DBAC_DOUBLE_AP_MODE)
OAL_STATIC oal_uint32  wal_hipriv_set_omit_acs(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
OAL_STATIC oal_uint32  wal_hipriv_set_uapsd_para(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param);
#endif
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
/* hi1102-cb add sys for 51/02 */

OAL_STATIC oal_ssize_t  wal_hipriv_sys_write(struct kobject *dev, struct kobj_attribute *attr, const char *buf, oal_size_t count);
OAL_STATIC oal_ssize_t  wal_hipriv_sys_read(struct kobject *dev, struct kobj_attribute *attr, char *buf);
OAL_STATIC struct kobj_attribute dev_attr_hipriv =
    __ATTR(hipriv, (OAL_S_IRUGO|OAL_S_IWUSR), wal_hipriv_sys_read, wal_hipriv_sys_write);
OAL_STATIC struct attribute *hipriv_sysfs_entries[] = {
        &dev_attr_hipriv.attr,
        NULL
};
OAL_STATIC struct attribute_group hipriv_attribute_group = {
        .attrs = hipriv_sysfs_entries,
};
/* hi1102-cb add sys for 51/02 */
#endif
OAL_STATIC oal_int32 wal_ioctl_set_dc_status(oal_net_device_stru *pst_net_dev, oal_int32 dc_param);

#ifdef _PRE_WLAN_FEATURE_P2P
OAL_STATIC oal_int32 wal_ioctl_set_p2p_miracast_status(oal_net_device_stru *pst_net_dev, oal_uint8 uc_param);
OAL_STATIC oal_int32  wal_ioctl_set_p2p_noa(oal_net_device_stru * pst_net_dev, mac_cfg_p2p_noa_param_stru * pst_p2p_noa_param);
OAL_STATIC oal_int32  wal_ioctl_set_p2p_ops(oal_net_device_stru * pst_net_dev, mac_cfg_p2p_ops_param_stru * pst_p2p_ops_param);

#endif  /* _PRE_WLAN_FEATURE_P2P */
#ifdef _PRE_WLAN_FEATURE_VOWIFI
OAL_STATIC oal_int32 wal_ioctl_set_vowifi_param(oal_net_device_stru *pst_net_dev, oal_int8* puc_command, wal_wifi_priv_cmd_stru *pst_priv_cmd);
OAL_STATIC oal_int32 wal_ioctl_get_vowifi_param(oal_net_device_stru *pst_net_dev, oal_int8 *puc_command, oal_int32 *pl_value);
#endif

#ifdef _PRE_WLAN_FEATURE_M2S_MSS
OAL_STATIC oal_uint32  wal_ioctl_set_m2s_blacklist(oal_net_device_stru *pst_net_dev, oal_uint8 *puc_buf, oal_uint32 ul_buf_len, oal_uint8 uc_m2s_blacklist_cnt);
oal_uint32  wal_ioctl_set_m2s_mss(oal_net_device_stru *pst_net_dev, oal_uint8 uc_m2s_mode);
#endif

#ifdef _PRE_WLAN_FEATURE_HS20
OAL_STATIC oal_int32 wal_ioctl_set_qos_map(oal_net_device_stru *pst_net_dev,
                                                   hmac_cfg_qos_map_param_stru *pst_qos_map_param);
#endif/* #ifdef _PRE_WLAN_FEATURE_HS20 */

oal_int32 wal_ioctl_set_wps_p2p_ie_etc(oal_net_device_stru  *pst_net_dev,
                                    oal_uint8           *puc_buf,
                                    oal_uint32           ul_len,
                                    en_app_ie_type_uint8 en_type);

OAL_STATIC oal_int32 wal_set_ap_max_user(oal_net_device_stru *pst_net_dev, oal_uint32 ul_ap_max_user);
OAL_STATIC oal_int32 wal_config_mac_filter(oal_net_device_stru *pst_net_dev, oal_int8 *pc_command);
OAL_STATIC oal_int32 wal_kick_sta(oal_net_device_stru *pst_net_dev, oal_uint8 *auc_mac_addr, oal_uint8 uc_mac_addr_len, oal_uint16 us_reason_code);
OAL_STATIC int  wal_ioctl_set_ap_config(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info, oal_iwreq_data_union *pst_wrqu, char *pc_extra);
OAL_STATIC int  wal_ioctl_set_mac_filters(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info, oal_iwreq_data_union *pst_wrqu, char *pc_extra);
OAL_STATIC int  wal_ioctl_get_assoc_list(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info, oal_iwreq_data_union *pst_wrqu, char *pc_extra);
OAL_STATIC int  wal_ioctl_set_ap_sta_disassoc(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info, oal_iwreq_data_union *pst_wrqu, char *pc_extra);
OAL_STATIC oal_uint32  wal_get_parameter_from_cmd(oal_int8 *pc_cmd, oal_int8 *pc_arg, OAL_CONST oal_int8 *puc_token, oal_uint32 *pul_cmd_offset, oal_uint32 ul_param_max_len);
OAL_STATIC oal_int32 wal_ioctl_priv_cmd_set_ap_wps_p2p_ie_etc( oal_net_device_stru * pst_net_dev, oal_int8 * pc_command, wal_wifi_priv_cmd_stru st_priv_cmd);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_int32 wal_ioctl_reduce_sar(oal_net_device_stru *pst_net_dev, oal_uint16 ul_tx_power);
#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
OAL_STATIC oal_int32 wal_ioctl_tas_pow_ctrl(oal_net_device_stru *pst_net_dev, oal_uint8 uc_coreindex,
                                                   oal_bool_enum_uint8 en_needImprove);
OAL_STATIC oal_int32 wal_ioctl_tas_rssi_access(oal_net_device_stru *pst_net_dev, oal_uint8 uc_coreindex);
#endif
#ifndef CONFIG_HAS_EARLYSUSPEND
OAL_STATIC oal_int32  wal_ioctl_set_suspend_mode(oal_net_device_stru *pst_net_dev, oal_uint8 uc_suspend);
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_STA_PM
OAL_STATIC oal_uint32 wal_ioctl_set_fastsleep_switch(oal_net_device_stru *pst_net_dev,oal_int8 *puc_para_str);
#endif

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
oal_int32 wal_init_wlan_vap_etc(oal_net_device_stru *pst_net_dev);
oal_int32 wal_deinit_wlan_vap_etc(oal_net_device_stru *pst_net_dev);
oal_int32 wal_start_vap_etc(oal_net_device_stru *pst_net_dev);
oal_int32  wal_stop_vap_etc(oal_net_device_stru *pst_net_dev);
OAL_STATIC oal_int32 wal_set_mac_addr(oal_net_device_stru *pst_net_dev);
oal_int32 wal_init_wlan_netdev_etc(oal_wiphy_stru *pst_wiphy, const char *dev_name);
oal_int32  wal_setup_ap_etc(oal_net_device_stru *pst_net_dev);
#endif

#ifdef _PRE_WLAN_FEATURE_11D
oal_int32  wal_regdomain_update_for_dfs_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_country);
oal_int32  wal_regdomain_update_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_country);
#endif

#ifdef _PRE_WLAN_FEATURE_GREEN_AP
OAL_STATIC oal_uint32  wal_hipriv_green_ap_en(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif

#ifdef _PRE_WLAN_FEATURE_11K
OAL_STATIC oal_uint32  wal_hipriv_send_neighbor_req(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_beacon_req_table_switch(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
OAL_STATIC oal_uint32  wal_hipriv_voe_enable(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#if (defined _PRE_WLAN_RF_CALI) || (defined _PRE_WLAN_RF_CALI_1151V2)
OAL_STATIC oal_uint32  wal_hipriv_auto_cali(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
OAL_STATIC oal_uint32  wal_hipriv_set_cali_vref(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
OAL_STATIC oal_uint32  wal_hipriv_load_ini_power_gain(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_APF
OAL_STATIC oal_uint32  wal_hipriv_apf_filter_list(oal_net_device_stru *pst_net_dev,oal_int8 *pc_param);
#endif
OAL_STATIC oal_uint32  wal_hipriv_remove_app_ie(oal_net_device_stru *pst_net_dev,oal_int8 *pc_param);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_int32 wal_ioctl_get_wifi_priv_feature_cap_param(oal_net_device_stru *pst_net_dev, oal_int8 *puc_command, oal_int32 *pl_value);
#endif
#ifdef  _PRE_WLAN_FEATURE_FORCE_STOP_FILTER
OAL_STATIC oal_uint32  wal_ioctl_force_stop_filter(oal_net_device_stru *pst_net_dev,oal_uint8 uc_param);
#endif
extern oal_uint8 wal_cfg80211_convert_value_to_vht_width(oal_int32 l_channel_value);
extern oal_uint32 wal_hipriv_set_rfch(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
extern oal_uint32 wal_hipriv_set_rxch(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
extern oal_uint32  wal_hipriv_set_nss(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
extern oal_uint32  wal_hipriv_send_cw_signal(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
extern oal_uint32  wal_hipriv_adjust_ppm(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
extern oal_uint32  wal_hipriv_chip_check(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);
extern oal_uint32  wal_hipriv_set_txpower(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param);

/*****************************************************************************
  ??????????????. ????????????:
         ?????? ?????? ????
  hipriv "Hisilicon0 create vap0"
*****************************************************************************/
/* private command strings */
#define CMD_SET_AP_WPS_P2P_IE   "SET_AP_WPS_P2P_IE"
#define CMD_SET_MLME_IE         "SET_MLME_IE"
#define CMD_P2P_SET_NOA         "P2P_SET_NOA"
#define CMD_P2P_SET_PS          "P2P_SET_PS"
#define CMD_SET_POWER_ON        "SET_POWER_ON"
#define CMD_SET_POWER_MGMT_ON   "SET_POWER_MGMT_ON"
#define CMD_COUNTRY             "COUNTRY"
#define CMD_GET_CAPA_DBDC       "GET_CAPAB_RSDB"
#define CMD_CAPA_DBDC_SUPP      "RSDB:1"
#define CMD_CAPA_DBDC_NOT_SUPP  "RSDB:0"
#define CMD_SET_DC_STATE        "SET_DC_STATE"

#ifdef _PRE_WLAN_FEATURE_LTECOEX
#define CMD_LTECOEX_MODE        "LTECOEX_MODE"
#endif
#ifdef _PRE_WLAN_FEATURE_NRCOEX
#define CMD_SET_NRCOEX_PRIOR    "SET_NRCOEX_PRIOR"
#define CMD_GET_NRCOEX_INFO     "GET_NRCOEX_INFO"
#endif
#define CMD_SET_QOS_MAP         "SET_QOS_MAP"
#define CMD_TX_POWER            "TX_POWER"
#define CMD_WPAS_GET_CUST       "WPAS_GET_CUST"
#define CMD_SET_STA_PM_ON       "SET_STA_PM_ON"

#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
/* TAS???????????? */
#define CMD_SET_MEMO_CHANGE     "SET_ANT_CHANGE"
/* ???????? */
#define CMD_MEASURE_TAS_RSSI    "SET_TAS_MEASURE"
/* ?????? */
#define CMD_SET_TAS_TXPOWER     "SET_TAS_TXPOWER"
/* ???????? */
#define CMD_TAS_GET_ANT         "TAS_GET_ANT"
#endif

#ifdef _PRE_WLAN_FEATURE_M2S
#define CMD_SET_M2S_SWITCH      "SET_M2S_SWITCH"
#define CMD_SET_M2S_BLACKLIST   "SET_M2S_BLACKLIST"
#endif

#ifdef _PRE_WLAN_FEATURE_VOWIFI
/*
VOWIFI_DETECT SET/GET MODE [param]
VOWIFI_DETECT SET/GET PERIOD [param]
VOWIFI_DETECT SET/GET LOW_THRESHOLD [param]
VOWIFI_DETECT SET/GET HIGH_THRESHOLD [param]
VOWIFI_DETECT SET/GET TRIGGER_COUNT [param]
VOWIFI_DETECT VOWIFi_IS_SUPPORT
*/

#define CMD_VOWIFI_SET_MODE             "VOWIFI_DETECT SET MODE"
#define CMD_VOWIFI_GET_MODE             "VOWIFI_DETECT GET MODE"
#define CMD_VOWIFI_SET_PERIOD           "VOWIFI_DETECT SET PERIOD"
#define CMD_VOWIFI_GET_PERIOD           "VOWIFI_DETECT GET PERIOD"
#define CMD_VOWIFI_SET_LOW_THRESHOLD    "VOWIFI_DETECT SET LOW_THRESHOLD"
#define CMD_VOWIFI_GET_LOW_THRESHOLD    "VOWIFI_DETECT GET LOW_THRESHOLD"
#define CMD_VOWIFI_SET_HIGH_THRESHOLD   "VOWIFI_DETECT SET HIGH_THRESHOLD"
#define CMD_VOWIFI_GET_HIGH_THRESHOLD   "VOWIFI_DETECT GET HIGH_THRESHOLD"
#define CMD_VOWIFI_SET_TRIGGER_COUNT    "VOWIFI_DETECT SET TRIGGER_COUNT"
#define CMD_VOWIFI_GET_TRIGGER_COUNT    "VOWIFI_DETECT GET TRIGGER_COUNT"

#define CMD_VOWIFI_SET_PARAM            "VOWIFI_DETECT SET"
#define CMD_VOWIFI_GET_PARAM            "VOWIFI_DETECT GET"

#define CMD_VOWIFI_IS_SUPPORT_REPLY     "true"
#else
#define CMD_VOWIFI_IS_SUPPORT_REPLY     "false"
#endif /* _PRE_WLAN_FEATURE_VOWIFI */
#define CMD_VOWIFI_IS_SUPPORT           "VOWIFI_DETECT VOWIFI_IS_SUPPORT"

#define CMD_GET_WIFI_PRIV_FEATURE_CAPABILITY   "GET_WIFI_PRIV_FEATURE_CAPABILITY"

#define CMD_SETSUSPENDOPT               "SETSUSPENDOPT"
#define CMD_SETSUSPENDMODE              "SETSUSPENDMODE"

#define CMD_SET_SOFTAP_MIMOMODE         "SET_AP_MODE"

#define CMD_GET_AP_BANDWIDTH            "GET_AP_BANDWIDTH"

#define CMD_GET_VHT160_SUPPORTED        "GET_VHT160_SUPPORTED"

#define CMD_SET_VHT160_FEM_LOWER        "SET_VHT160_FEM_LOWER"

oal_int8 *g_pac_mib_operation_cmd_name[] =
{
    "smps_vap_mode",
    "info",
    "2040_coexistence",
    "freq",
    "mode",
    "set_mcast_data",
    "set_edca_switch_sta",
    "beacon_offload_test",/* debug begin */
    "auto_ba",
    "reassoc_req",
    "coex_preempt_type",
    "protocol_debug",
    "start_scan",
    "start_join",
    "kick_user",
    "ampdu_tx_on",
    "amsdu_tx_on",
    "ampdu_amsdu",
    "frag_threshold",
    "wmm_switch",
    "hide_ssid",
    "set_stbc_cap",
    "set_ldpc_cap",
    "set_psm_para",
    "set_sta_pm_on",
    "enable_pmf",
    "send_pspoll",
    "send_nulldata",
    "set_default_key",
    "add_key",
    "auto_protection",
    "send_2040_coext",
    "set_opmode_notify",
    "m2u_igmp_pkt_xmit",
    "smps_info",
    "vap_classify_tid",
    "essid",
    "bintval",
    "ampdu_mmss",
    "set_tx_classify_switch",
    "custom_info",
    "11v_cfg_wl_mgmt",
    "11v_cfg_bsst",
    "send_radio_meas_req",
    "11k_switch"
};

#ifdef _PRE_WLAN_FEATURE_IP_FILTER
#define CMD_SET_RX_FILTER_ENABLE    "set_rx_filter_enable"
#define CMD_ADD_RX_FILTER_ITEMS     "add_rx_filter_items"
#define CMD_CLEAR_RX_FILTERS        "clear_rx_filters"
#define CMD_GET_RX_FILTER_PKT_STATE "get_rx_filter_pkt_state"

#define CMD_FILTER_SWITCH    "FILTER"
#endif /* _PRE_WLAN_FEATURE_IP_FILTER */
#ifdef _PRE_WLAN_FEATURE_PSM_FLT_STAT
#define CMD_GET_APF_PKTS_CNT  "GET_APF_PKTS_CNT"
#define CMD_GET_APF_PKTS_CNT_LEN                                     (OAL_STRLEN(CMD_GET_APF_PKTS_CNT))
#endif
#define CMD_SET_FASTSLEEP_SWITCH       "SET_FASTSLEEP_SWITCH "
#define CMD_SET_FASTSLEEP_SWITCH_LEN   (OAL_STRLEN(CMD_SET_FASTSLEEP_SWITCH))

#define CMD_GET_FAST_SLEEP_CNT       "GET_FAST_SLEEP_CNT"
#define CMD_GET_FAST_SLEEP_CNT_LEN   (OAL_STRLEN(CMD_GET_FAST_SLEEP_CNT))

#define CMD_RXFILTER_START "RXFILTER-START"
#define CMD_RXFILTER_STOP  "RXFILTER-STOP"
#define CMD_MIRACAST_START "MIRACAST 1"
#define CMD_MIRACAST_STOP  "MIRACAST 0"

/* ?????????????????? */
#define   CMD_SET_AP_WPS_P2P_IE_LEN                                     (OAL_STRLEN(CMD_SET_AP_WPS_P2P_IE))
#define   CMD_P2P_SET_NOA_LEN                                           (OAL_STRLEN(CMD_P2P_SET_NOA))
#define   CMD_P2P_SET_PS_LEN                                            (OAL_STRLEN(CMD_P2P_SET_PS))
#define   CMD_SET_STA_PM_ON_LEN                                         (OAL_STRLEN(CMD_SET_STA_PM_ON))
#define   CMD_SET_QOS_MAP_LEN                                           (OAL_STRLEN(CMD_SET_QOS_MAP))
#define   CMD_COUNTRY_LEN                                               (OAL_STRLEN(CMD_COUNTRY))
#define   CMD_LTECOEX_MODE_LEN                                          (OAL_STRLEN(CMD_LTECOEX_MODE))
#define   CMD_TX_POWER_LEN                                              (OAL_STRLEN(CMD_TX_POWER))
#define   CMD_WPAS_GET_CUST_LEN                                         (OAL_STRLEN(CMD_WPAS_GET_CUST))
#define   CMD_VOWIFI_SET_PARAM_LEN                                      (OAL_STRLEN(CMD_VOWIFI_SET_PARAM))
#define   CMD_VOWIFI_GET_PARAM_LEN                                      (OAL_STRLEN(CMD_VOWIFI_GET_PARAM))
#define   CMD_SETSUSPENDOPT_LEN                                         (OAL_STRLEN(CMD_SETSUSPENDOPT))
#define   CMD_SETSUSPENDMODE_LEN                                        (OAL_STRLEN(CMD_SETSUSPENDMODE))
#define   CMD_GET_WIFI_PRIV_FEATURE_CAPABILITY_LEN                      (OAL_STRLEN(CMD_GET_WIFI_PRIV_FEATURE_CAPABILITY))
#define   CMD_VOWIFI_IS_SUPPORT_LEN                                     (OAL_STRLEN(CMD_VOWIFI_IS_SUPPORT))
#define   CMD_VOWIFI_IS_SUPPORT_REPLY_LEN                               (OAL_STRLEN(CMD_VOWIFI_IS_SUPPORT_REPLY))
#define   CMD_FILTER_SWITCH_LEN                                         (OAL_STRLEN(CMD_FILTER_SWITCH))
#define   CMD_RXFILTER_START_LEN                                        (OAL_STRLEN(CMD_RXFILTER_START))
#define   CMD_RXFILTER_STOP_LEN                                         (OAL_STRLEN(CMD_RXFILTER_STOP))
#define   CMD_SET_MLME_IE_LEN                                           (OAL_STRLEN(CMD_SET_MLME_IE))
#define   CMD_MIRACAST_START_LEN                                        (OAL_STRLEN(CMD_MIRACAST_START))
#define   CMD_MIRACAST_STOP_LEN                                         (OAL_STRLEN(CMD_MIRACAST_STOP))
#define   CMD_SET_POWER_ON_LEN                                          (OAL_STRLEN(CMD_SET_POWER_ON))
#define   CMD_SET_POWER_MGMT_ON_LEN                                     (OAL_STRLEN(CMD_SET_POWER_MGMT_ON))
#define   CMD_GET_CAPA_DBDC_LEN                                         (OAL_STRLEN(CMD_GET_CAPA_DBDC))
#define   CMD_CAPA_DBDC_SUPP_LEN                                        (OAL_STRLEN(CMD_CAPA_DBDC_SUPP))
#define   CMD_CAPA_DBDC_NOT_SUPP_LEN                                    (OAL_STRLEN(CMD_CAPA_DBDC_NOT_SUPP))
#define   CMD_SET_TAS_TXPOWER_LEN                                       (OAL_STRLEN(CMD_SET_TAS_TXPOWER))
#define   CMD_MEASURE_TAS_RSSI_LEN                                      (OAL_STRLEN(CMD_MEASURE_TAS_RSSI))
#define   CMD_TAS_GET_ANT_LEN                                           (OAL_STRLEN(CMD_TAS_GET_ANT))
#define   CMD_SET_MEMO_CHANGE_LEN                                       (OAL_STRLEN(CMD_SET_MEMO_CHANGE))
#define   CMD_SET_M2S_BLACKLIST_LEN                                     (OAL_STRLEN(CMD_SET_M2S_BLACKLIST))
#define   CMD_GET_AP_BANDWIDTH_LEN                                      (OAL_STRLEN(CMD_GET_AP_BANDWIDTH))
#define   CMD_GET_VHT160_SUPPORTED_LEN                                  (OAL_STRLEN(CMD_GET_VHT160_SUPPORTED))
#define   CMD_SET_VHT160_FEM_LOWER_LEN                                  (OAL_STRLEN(CMD_SET_VHT160_FEM_LOWER))
#define   CMD_SET_DC_STATE_LEN                                          (OAL_STRLEN(CMD_SET_DC_STATE))

#ifdef _PRE_WLAN_FEATURE_VOWIFI
OAL_STATIC OAL_CONST wal_ioctl_priv_cmd_stru g_ast_vowifi_cmd_table[VOWIFI_CMD_BUTT] = {
    {CMD_VOWIFI_SET_MODE,                   VOWIFI_SET_MODE},
    {CMD_VOWIFI_GET_MODE,                   VOWIFI_GET_MODE},
    {CMD_VOWIFI_SET_PERIOD,                 VOWIFI_SET_PERIOD},
    {CMD_VOWIFI_GET_PERIOD,                 VOWIFI_GET_PERIOD},
    {CMD_VOWIFI_SET_LOW_THRESHOLD,          VOWIFI_SET_LOW_THRESHOLD},
    {CMD_VOWIFI_GET_LOW_THRESHOLD,          VOWIFI_GET_LOW_THRESHOLD},
    {CMD_VOWIFI_SET_HIGH_THRESHOLD,         VOWIFI_SET_HIGH_THRESHOLD},
    {CMD_VOWIFI_GET_HIGH_THRESHOLD,         VOWIFI_GET_HIGH_THRESHOLD},
    {CMD_VOWIFI_SET_TRIGGER_COUNT,          VOWIFI_SET_TRIGGER_COUNT},
    {CMD_VOWIFI_GET_TRIGGER_COUNT,          VOWIFI_GET_TRIGGER_COUNT},
    {CMD_VOWIFI_IS_SUPPORT,                 VOWIFI_SET_IS_SUPPORT},
};
#endif

OAL_STATIC OAL_CONST wal_hipriv_cmd_entry_stru  g_ast_hipriv_cmd[] =
{
    /************************??????????????????????*******************/
    {"info",                    wal_hipriv_vap_info_etc},               /* ????vap??????????????: hipriv "vap0 info" */
    {"setcountry",              wal_hipriv_setcountry},             /*?????????????? hipriv "Hisilicon0 setcountry param"param?????????????????????????? CN US*/
    {"getcountry",              wal_hipriv_getcountry},             /*?????????????? hipriv "Hisilicon0 getcountry"*/
#ifdef _PRE_WLAN_FEATURE_GREEN_AP
    {"green_ap_en",             wal_hipriv_green_ap_en},            /* green AP????: hipriv "wlan0 green_ap_en 0 | 1" */
#endif
#ifdef _PRE_WLAN_FEATURE_IP_FILTER
    {"ip_filter",               wal_hipriv_set_ip_filter_etc},          /* ip filter(????????????)hipriv "wlan0 ip_filter cmd param0 param1 ...."
                                                                       ????:???????? "wlan0 ip_filter set_rx_filter_enable 1/0"
                                                                           ?????????? "wlan0 ip_filter clear_rx_filters"
                                                                           ?????????? "wlan0 ip_filter add_rx_filter_items ????????(0/1/2...) ????????(protocol0 port0 protocol1 port1...)",????????????????????20??????
                                                                    */
#endif //_PRE_WLAN_FEATURE_IP_FILTER

    {"userinfo",                wal_hipriv_user_info},              /* ????????mac????user??????????????: hipriv "vap0 userinfo XX XX XX XX XX XX(16????oal_strtohex)" */
    {"reginfo",                 wal_hipriv_reg_info},               /* ??????????????: hipriv "Hisilicon0 reginfo 16|32(51????16????????????????) regtype(soc/mac/phy) startaddr endaddr" */
    {"pcie_pm_level",           wal_hipriv_pcie_pm_level},          /* ????pcie?????????? hipriv "Hisilicon0 pcie_pm_level level(01/2/3/4)" */
    {"regwrite",                wal_hipriv_reg_write},              /* ??????????????: hipriv "Hisilicon0 regwrite 32/16(51????16??????????????) regtype(soc/mac/phy) addr val" addr val????????16????0x???? */
    {"dump_all_dscr",           wal_hipriv_dump_all_rx_dscr},       /* ????????????????????, hipriv "Hisilicon0 dump_all_dscr" */
    {"random_mac_addr_scan",    wal_hipriv_set_random_mac_addr_scan}, /* ????????mac addr??????????sh hipriv.sh "Hisilicon0 random_mac_addr_scan 0|1(????|????)" */
    {"bgscan_enable",           wal_hipriv_bgscan_enable},        /* ???????????????? hipriv "Hisilicon0 bgscan_enable param1" param1????'0' '1',??????????????????????, '2' ???????????????? */
    {"2040_coexistence",        wal_hipriv_set_2040_coext_support},                /* ????20/40????????: hipriv "vap0 2040_coexistence 0|1" 0????20/40MHz??????????1????20/40MHz?????????? */

#ifdef _PRE_WLAN_FEATURE_STA_PM
    {"set_ps_mode",             wal_hipriv_sta_ps_mode},           /* ????PSPOLL???? sh hipriv.sh 'wlan0 set_ps_mode 3'*/
#ifdef _PRE_PSM_DEBUG_MODE
    {"psm_info_debug",          wal_hipriv_sta_ps_info},            /* sta psm?????????????? sh hipriv.sh 'wlan0 psm_info_debug 1' */
#endif
    {"set_fast_sleep_para",     wal_hipriv_set_fasts_sleep_para},
#endif
#ifdef _PRE_WLAN_FEATURE_SMPS
    {"smps_vap_mode",          wal_hipriv_set_smps_vap_mode},            /* ????vap??smps???? sh hipriv.sh 'wlan0 smps_vap_mode 1/2/3' static/dynamic/disable*/
#endif
#ifdef _PRE_WLAN_FEATURE_UAPSD
    {"uapsd_en_cap",           wal_hipriv_set_uapsd_cap},        /* hipriv "vap0 uapsd_en_cap 0\1" */
#endif
#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
    {"set_uapsd_para",          wal_hipriv_set_uapsd_para},        /* ????uapsd?????????? sh hipriv.sh 'wlan0 set_uapsd_para 3 1 1 1 1 */
#endif
    {"create",                  wal_hipriv_add_vap},                /* ????vap??????????: hipriv "Hisilicon0 create vap0 ap|sta" */
#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    {"al_tx",                   wal_hipriv_always_tx},               /* ????????????:       hipriv "vap0 al_tx <value: 0/1/2>  <len>" ????mac??????11a,b,g????????4095????????????,????????set_mcast_data??????????????*/
    {"al_tx_1102",              wal_hipriv_always_tx},               /* al_tx_02?????????? */
    {"al_tx_num",               wal_hipriv_always_tx_num},           /* ????????????:       hipriv "vap0 al_tx_num <value>" */
    {"al_tx_hw_cfg ",           wal_hipriv_always_tx_hw_cfg},        /* ????????????????:   hipriv "vap0 al_tx_hw_cfg devid XXX XXX" ??devid mode??????????????*/
    {"al_tx_aggr_num",          wal_hipriv_always_tx_aggr_num},      /* ????????????????:   hipriv "vap0 al_tx_hw_cfg devid XXX XXX" ??devid mode??????????????*/
    /* ????????????:       hipriv "vap0 al_tx_hw  0/1/2/3  0/1/2/3  content len times"
    1. 0/1/2/3 ????????bit 1????dev0/dev1????????bit 0??????????????????????????????????????????????????????
    ??????????????????dev????/??????????????????????????????????????
    2. 0/1/2/3 ??????????????0:??0??1:??1??2:?????? 3:????byte????????
    3. content??????2????????3????????????????????????????
    4. len??????????
    5. ????????????                   */
    {"al_tx_hw",                wal_hipriv_always_tx_hw},              /* ????????????:       hipriv "vap0 al_tx_hw <switch(x)|device id(x)| flag(xx)|xxxx> <content> <len> <times> <ifs>"*/
#endif
    {"al_rx",                   wal_hipriv_always_rx},               /* ????????????:               hipriv "vap0 al_rx <value: 0/1/2>" */
#ifdef _PRE_WLAN_FEATURE_MONITOR
    {"sniffer",                 wal_hipriv_set_sniffer},                       /* ????????????:               sh hipriv.sh "wlan0 sniffer <value: 0/1>" */
    {"monitor",                 wal_hipriv_enable_monitor_mode},               /* ????monitor????:               sh hipriv.sh "wlan0 monitor <value: 0/1>" */
#endif
    {"rate",                    wal_hipriv_set_rate_etc },               /* ????non-HT????????????:     hipriv "vap0 rate  <value>" */
    {"mcs",                     wal_hipriv_set_mcs_etc  },               /* ????HT????????????:         hipriv "vap0 mcs   <value>" */
    {"mcsac",                   wal_hipriv_set_mcsac_etc},               /* ????VHT????????????:        hipriv "vap0 mcsac <value>" */
#ifdef _PRE_WLAN_FEATURE_11AX
    {"mcsax",                   wal_hipriv_set_mcsax},                   /* ????HE????????????:         hipriv "vap0 mcsax <value>" */
#ifdef _PRE_WLAN_FEATURE_11AX_ER_SU
    {"mcsax_er",                wal_hipriv_set_mcsax_er},                /* ????HE ER????????????:      hipriv "vap0 mcsax_er <value>" */
#endif
#endif
    {"freq",                    wal_hipriv_set_freq},                    /* ????AP ???? */
    {"mode",                    wal_hipriv_set_mode},                    /* ????AP ???????? */
    {"bw",                      wal_hipriv_set_bw   },               /* ????????:                   hipriv "vap0 bw    <value>" */
    {"set_mcast_data",          wal_hipriv_set_mcast_data_dscr_param},    /* ??????????????: hipriv "vap0 set_mcast_data <param name> <value>" */
    {"rx_fcs_info",             wal_hipriv_rx_fcs_info},            /* ????????????FCS??????????????:hipriv "vap0 rx_fcs_info 0/1 1/2/3/4" 0/1  0????????????1???????? */
    {"set_regdomain_pwr",       wal_hipriv_set_regdomain_pwr},      /* ????????????????????????hipriv "Hisilicon0 set_regdomain_pwr 20",????dBm */
    {"add_user",                wal_hipriv_add_user},               /* ??????????????????????: hipriv "vap0 add_user xx xx xx xx xx xx(mac????) 0 | 1(HT??????) "  ????????????????VAP */
    {"del_user",                wal_hipriv_del_user},               /* ??????????????????????: hipriv "vap0 del_user xx xx xx xx xx xx(mac????)" ????????????????VAP */
    {"alg_cfg",                 wal_hipriv_alg_cfg_etc},                /* ????????????: hipriv "vap0 alg_cfg sch_vi_limit 10"*/
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    {"set_edca_switch_sta",        wal_hipriv_set_edca_opt_switch_sta},       /* STA????????????edca???????????? */
#endif
    {"alg_tpc_log",             wal_hipriv_tpc_log},                /* tpc????????????????:*/
#if (defined(_PRE_PRODUCT_ID_HI110X_DEV) || defined(_PRE_PRODUCT_ID_HI110X_HOST))
    {"sdio_flowctrl",           wal_hipriv_sdio_flowctrl},
#endif
#ifdef _PRE_WLAN_DELAY_STATISTIC
    /* ??????????????????: hipriv "Hisilicon0 pkt_time_switch on |off */
    {"pkt_time_switch",         wal_hipriv_pkt_time_switch},
#endif
#ifdef _PRE_WLAN_FEATURE_DFS
    {"radartool",               wal_hipriv_dfs_radartool},
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    {"aifsn_cfg",               wal_hipriv_aifsn_cfg},              /* wfa??????????????AC??aifsn??, sh hipriv.sh "Hisilicon0 aifsn_cfg 0|1(????|????) 0|1|2|3(be-vo) val" */
    {"cw_cfg",                  wal_hipriv_cw_cfg},                 /* wfa??????????????AC??cwmaxmin??, sh hipriv.sh "Hisilicon0 cw_cfg 0|1(????|????) 0|1|2|3(be-vo) val" */
#endif
#ifdef _PRE_WLAN_FEATURE_11K
    {"send_neighbor_req",       wal_hipriv_send_neighbor_req},          /* sh hipriv.sh "wlan0 send_neighbor_req WiFi1" */
    {"beacon_req_table_switch", wal_hipriv_beacon_req_table_switch}, /* sh hipriv.sh "wlan0 beacon_req_table_switch 0/1" */
#endif
    {"voe_enable",              wal_hipriv_voe_enable},             /* VOE?????????????????????? sh hipriv.sh "wlan0 voe_enable 0/1" (Bit0:11r  Bit1:11V Bit2:11K Bit3:????????????IE70(voe ????????),Bit4:11r????,B5-B6:11k auth operating class  Bit7:????????)*/
    {"log_level",               wal_hipriv_vap_log_level},          /* VAP???????????? hipriv "VAPX log_level {1|2}"  Warning??Error??????????VAP?????? */
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    {"load_ini_gain",           wal_hipriv_load_ini_power_gain}, /* ???????????????????????????????? hipriv.sh "Hisilicon0 load_ini_gain" */
#endif

#if (defined _PRE_WLAN_RF_CALI) || (defined _PRE_WLAN_RF_CALI_1151V2)
    {"auto_cali",        wal_hipriv_auto_cali},      /* ??????????????:hipriv "Hisilicon0 auto_cali" */
    {"cali_vref",        wal_hipriv_set_cali_vref},  /* ????????????:hipriv "wlan0 cali_vref value" */
#endif
#ifdef _PRE_WLAN_FEATURE_SINGLE_CHIP_DUAL_BAND
    {"restrict_band",    wal_hipriv_set_restrict_band}, /* ?????????????????? */
#endif
#if defined(_PRE_WLAN_FEATURE_DBAC) && defined(_PRE_WLAN_FEATRUE_DBAC_DOUBLE_AP_MODE)
    {"omit_acs",         wal_hipriv_set_omit_acs},      /* ????????ACS */
#endif

#ifdef _PRE_WLAN_FEATURE_WDS
    {"wds_vap_mode",   wal_hipriv_wds_vap_mode}, /* ????vap?? wds????: hipriv "wlan0 wds_vap_mode 0/1/2/3" ?????????? 0:????WDS, 1:RootAP????, 2:RepeaterAP????????STA Vap, 3:RepeaterAP????????AP Vap */
    {"wds_vap_show",   wal_hipriv_wds_vap_show}, /* ????WDS Vap???????????????????? */
    {"wds_sta_add",   wal_hipriv_wds_sta_add}, /* ????WDS STA?????? WDS????: hipriv "wlan0 wds_sta_add StaMac PeerNodeMac" ?????????? StaMac?????????? WDS STA?? Mac????, ?????????? PeerNodeMac??WDS STA??????WDS??????????Mac(??????STA??????Vap,????????????????????????????????) */
    {"wds_sta_del",   wal_hipriv_wds_sta_del}, /* ????WDS????????WDS STA????: hipriv "wlan0 wds_sta_del StaMac" ?????????? StaMac?????????? WDS STA?? Mac???? */
    {"wds_sta_age",   wal_hipriv_wds_sta_age}, /* ????WDS????WDS STA????????: hipriv "wlan0 wds_sta_age AgeTime" ?????????? WDS Sta??????????????(????????) */
#endif

#ifdef _PRE_WLAN_FEATURE_DHCP_REQ_DISABLE
    {"dhcp_req_disable",         wal_hipriv_dhcp_req_disable_switch},           /* dhcp_req_disable????????????????: hipriv "vap0 dhcp_req_disable 0 | 1" */
#endif
#ifdef _PRE_WLAN_FEATURE_SMPS
    {"smps_mode",       wal_hipriv_set_smps_mode},      /* ????smps mode */
#endif
    {"rssi_limit",      wal_hipriv_rssi_limit},         /* rssi?????????????? */
#ifdef _PRE_WLAN_FEATURE_CAR
    {"car_cfg",         wal_hipriv_car_cfg},            /* car????????????: */
#endif
#ifdef _PRE_FEATURE_FAST_AGING
    {"fast_aging",       wal_hipriv_fast_aging_cfg},    /* ???????? */
#endif

#ifdef _PRE_WLAN_FEATURE_TCP_ACK_BUFFER
    {"tcp_ack_buf",       wal_hipriv_tcp_ack_buf_cfg},    /* ???????? */
#endif

#ifdef _PRE_WLAN_REPORT_PRODUCT_LOG
    {"product_log_flag",         wal_hipriv_report_product_log_cfg},            /* ????log??????????hipriv "wlan0 product_log_flag 0/1/2"; 0/1??????????2???????????????? */
#endif

#ifdef _PRE_WLAN_FEATURE_NEGTIVE_DET
    {"pk_mode_debug",         wal_hipriv_pk_mode_debug},                        /* pkmode?????????????????? hipriv "wlan0 pk_mode_debug 0/1(high/low) 0/1/2/3/4(BW) 0/1/2/3(protocol) ??????????" */
#endif
#ifdef _PRE_WLAN_FEATURE_APF
    {"apf_filter_list",       wal_hipriv_apf_filter_list},
#endif
    {"remove_app_ie",         wal_hipriv_remove_app_ie},      /* ????eid????????????????????IE hipriv "wlan0 remove_app_ie 0/1 eid" 0??????ie,1??????ie */
};

/*****************************************************************************
  net_device????????net_device_ops????
*****************************************************************************/
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_net_device_ops_stru g_st_wal_net_dev_ops_etc =
{
    .ndo_get_stats          = wal_netdev_get_stats,
    .ndo_open               = wal_netdev_open_ext,
    .ndo_stop               = wal_netdev_stop_etc,
    .ndo_start_xmit         = wal_bridge_vap_xmit_etc,

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44))
            /* TBD */
#else
    .ndo_set_multicast_list = OAL_PTR_NULL,
#endif

    .ndo_do_ioctl           = wal_net_device_ioctl,
    .ndo_change_mtu         = oal_net_device_change_mtu,
    .ndo_init               = oal_net_device_init,

#if (defined(_PRE_WLAN_FEATURE_FLOWCTL) || defined(_PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL))
    .ndo_select_queue       = wal_netdev_select_queue_etc,
#endif

    .ndo_set_mac_address    = wal_netdev_set_mac_addr
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 44))
oal_ethtool_ops_stru g_st_wal_ethtool_ops_etc = { 0 };
#endif

#elif (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION)
oal_net_device_ops_stru g_st_wal_net_dev_ops_etc =
{
    oal_net_device_init,
    wal_netdev_open_ext,
    wal_netdev_stop_etc,
    wal_bridge_vap_xmit_etc,
    OAL_PTR_NULL,
    wal_netdev_get_stats,
    wal_net_device_ioctl,
    oal_net_device_change_mtu,
    wal_netdev_set_mac_addr
};
oal_ethtool_ops_stru g_st_wal_ethtool_ops_etc = { 0 };
#endif

/*****************************************************************************
  ????ioctl??????????
*****************************************************************************/
OAL_STATIC OAL_CONST oal_iw_handler g_ast_iw_handlers[] =
{
    OAL_PTR_NULL,                               /* SIOCSIWCOMMIT, */
    (oal_iw_handler)wal_ioctl_get_iwname,       /* SIOCGIWNAME, */
    OAL_PTR_NULL,                               /* SIOCSIWNWID, */
    OAL_PTR_NULL,                               /* SIOCGIWNWID, */
    OAL_PTR_NULL,                               /* SIOCSIWFREQ, ???????????? */
    OAL_PTR_NULL,                               /* SIOCGIWFREQ, ???????????? */
    OAL_PTR_NULL,                               /* SIOCSIWMODE, ????bss type */
    OAL_PTR_NULL,                               /* SIOCGIWMODE, ????bss type */
    OAL_PTR_NULL,                               /* SIOCSIWSENS, */
    OAL_PTR_NULL,                               /* SIOCGIWSENS, */
    OAL_PTR_NULL,                               /* SIOCSIWRANGE, */ /* not used */
    OAL_PTR_NULL,                               /* SIOCGIWRANGE, */
    OAL_PTR_NULL,                               /* SIOCSIWPRIV, */  /* not used */
    OAL_PTR_NULL,                               /* SIOCGIWPRIV, */  /* kernel code */
    OAL_PTR_NULL,                               /* SIOCSIWSTATS, */ /* not used */
    OAL_PTR_NULL,                               /* SIOCGIWSTATS, */
    OAL_PTR_NULL,                               /* SIOCSIWSPY, */
    OAL_PTR_NULL,                               /* SIOCGIWSPY, */
    OAL_PTR_NULL,                               /* -- hole -- */
    OAL_PTR_NULL,                               /* -- hole -- */
    OAL_PTR_NULL,                               /* SIOCSIWAP, */
    (oal_iw_handler)wal_ioctl_get_apaddr,       /* SIOCGIWAP, */
    OAL_PTR_NULL,                               /* SIOCSIWMLME, */
    OAL_PTR_NULL,                               /* SIOCGIWAPLIST, */
    OAL_PTR_NULL,                               /* SIOCSIWSCAN, */
    OAL_PTR_NULL,                               /* SIOCGIWSCAN, */
    OAL_PTR_NULL,                               /* SIOCSIWESSID, ????ssid */
    (oal_iw_handler)wal_ioctl_get_essid,        /* SIOCGIWESSID, ????ssid */
    OAL_PTR_NULL,                               /* SIOCSIWNICKN */
    OAL_PTR_NULL,                               /* SIOCGIWNICKN */
    OAL_PTR_NULL,                               /* -- hole -- */
    OAL_PTR_NULL,                               /* -- hole -- */
    OAL_PTR_NULL,                               /* SIOCSIWRATE */
    OAL_PTR_NULL,                               /* SIOCGIWRATE  get_iwrate*/
    OAL_PTR_NULL,                               /* SIOCSIWRTS */
    OAL_PTR_NULL,                               /* SIOCGIWRTS  get_rtsthres*/
    OAL_PTR_NULL,                               /* SIOCSIWFRAG */
    OAL_PTR_NULL,                               /* SIOCGIWFRAG  get_fragthres*/
    OAL_PTR_NULL,                               /* SIOCSIWTXPOW, ???????????????? */
    OAL_PTR_NULL,                               /* SIOCGIWTXPOW, ???????????????? */
    OAL_PTR_NULL,                               /* SIOCSIWRETRY */
    OAL_PTR_NULL,                               /* SIOCGIWRETRY */
    OAL_PTR_NULL,                               /* SIOCSIWENCODE */
    OAL_PTR_NULL,                               /* SIOCGIWENCODE  get_iwencode*/
    OAL_PTR_NULL,                               /* SIOCSIWPOWER */
    OAL_PTR_NULL,                               /* SIOCGIWPOWER */
    OAL_PTR_NULL,                               /* -- hole -- */
    OAL_PTR_NULL,                               /* -- hole -- */
    OAL_PTR_NULL,                               /* SIOCSIWGENIE */
    OAL_PTR_NULL,                               /* SIOCGIWGENIE */
    OAL_PTR_NULL,                               /* SIOCSIWAUTH */
    OAL_PTR_NULL,                               /* SIOCGIWAUTH */
    OAL_PTR_NULL,                               /* SIOCSIWENCODEEXT */
    OAL_PTR_NULL                                /* SIOCGIWENCODEEXT */
};

/*****************************************************************************
  ????ioctl????????????
*****************************************************************************/
OAL_STATIC OAL_CONST oal_iw_priv_args_stru g_ast_iw_priv_args[] =
{
#ifdef _PRE_WLAN_FEATURE_HILINK_HERA_PRODUCT_DEBUG
    {WAL_IOCTL_PRIV_SET_VENDOR_REQ,  OAL_IW_PRIV_TYPE_CHAR | 64, 0, "set_vendor_req"},
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    {WAL_IOCTL_PRIV_SET_AP_CFG, OAL_IW_PRIV_TYPE_CHAR |  256, 0,"AP_SET_CFG"},
    {WAL_IOCTL_PRIV_AP_MAC_FLTR, OAL_IW_PRIV_TYPE_CHAR | 256, OAL_IW_PRIV_TYPE_CHAR | OAL_IW_PRIV_SIZE_FIXED | 0, "AP_SET_MAC_FLTR"},
    {WAL_IOCTL_PRIV_AP_GET_STA_LIST, 0, OAL_IW_PRIV_TYPE_CHAR | 1024, "AP_GET_STA_LIST"},
    {WAL_IOCTL_PRIV_AP_STA_DISASSOC, OAL_IW_PRIV_TYPE_CHAR | 256, OAL_IW_PRIV_TYPE_CHAR | 0, "AP_STA_DISASSOC"},
#endif

#ifdef _PRE_WLAN_FEATURE_WDS
    {WLAN_CFGID_WDS_VAP_MODE,   OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1,  0,  "wds_vap_mode"},  /* ????VAP??WDS???????? */
    {WLAN_CFGID_WDS_STA_ADD,    OAL_IW_PRIV_TYPE_CHAR | 40,                         0,  "wds_sta_add"},
    {WLAN_CFGID_WDS_STA_DEL,    OAL_IW_PRIV_TYPE_CHAR | 40,                         0,  "wds_sta_del"},
    {WLAN_CFGID_WDS_VAP_MODE,   0,   OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_wds_vap_mode"},  /*????VAP??WDS???????? */
#endif
#if (_PRE_WLAN_FEATURE_BLACKLIST_LEVEL != _PRE_WLAN_FEATURE_BLACKLIST_NONE)
    {WLAN_CFGID_ADD_BLACK_LIST,     OAL_IW_PRIV_TYPE_CHAR | 40, 0, "blkwhtlst_add"},
    {WLAN_CFGID_DEL_BLACK_LIST,     OAL_IW_PRIV_TYPE_CHAR | 40, 0, "blkwhtlst_del"},
    {WLAN_CFGID_CLR_BLACK_LIST,     OAL_IW_PRIV_TYPE_CHAR | 40, 0, "blkwhtlst_clr"},
    {WLAN_CFGID_BLACKLIST_MODE,     OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, "blkwhtlst_mode"},  /* ?????????????? */
    {WLAN_CFGID_BLACKLIST_MODE,     0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_blkwhtmode"},  /*?????????????? */
#endif

#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
    {WLAN_CFGID_GET_2040BSS_SW,         0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_obss_sw"},
    {WLAN_CFGID_2040BSS_ENABLE,         OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, "set_obss_sw"},
#endif

#ifdef _PRE_DEBUG_MODE
    {WLAN_CFGID_GET_FRW_MAX_EVENT,         0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_frw_max"},
    {WLAN_CFGID_SET_FRW_MAX_EVENT,         OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, 0, "set_frw_max"},
#endif
#ifdef _PRE_FEATURE_WAVEAPP_CLASSIFY
    {WLAN_CFGID_GET_WAVEAPP_FLAG,         0, OAL_IW_PRIV_TYPE_INT | OAL_IW_PRIV_SIZE_FIXED | 1, "get_waveapp_flag"}, /*????????waveapp??????????????*/
#endif
};

/*****************************************************************************
  ????ioctl??????????.
*****************************************************************************/
OAL_STATIC OAL_CONST oal_iw_handler g_ast_iw_priv_handlers[] =
{
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+0 */  /* sub-ioctl set ???? */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+1 */  /* sub-ioctl get ???? */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+2 */  /* setkey */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+3 */  /* setwmmparams */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+4 */  /* delkey */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+5 */  /* getwmmparams */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+6 */  /* setmlme */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+7 */  /* getchaninfo */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+8 */  /* setcountry */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+9 */  /* getcountry */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+10 */  /* addmac */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+11 */  /* getscanresults */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+12 */  /* delmac */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+13 */  /* getchanlist */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+14 */  /* setchanlist */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+15 */  /* setmac */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+16 */  /* chanswitch */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+17 */  /* ????????, ??: iwpriv vapN get_mode */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+18 */  /* ????????, ??: iwpriv vapN mode 11g */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+19 */  /* getappiebuf */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+20 */  /* null */
    (oal_iw_handler)wal_ioctl_get_assoc_list,           /* SIOCWFIRSTPRIV+21 */  /* APUT????????STA???? */
    (oal_iw_handler)wal_ioctl_set_mac_filters,          /* SIOCWFIRSTPRIV+22 */  /* APUT????STA???? */
    (oal_iw_handler)wal_ioctl_set_ap_config,            /* SIOCWFIRSTPRIV+23 */  /* ????APUT???? */
    (oal_iw_handler)wal_ioctl_set_ap_sta_disassoc,      /* SIOCWFIRSTPRIV+24 */  /* APUT??????STA */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+25 */  /* getStatistics */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+26 */  /* sendmgmt */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+27 */  /* null */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+28 */  /* null */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+29 */  /* null */
    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+30 */  /* sethbrparams */

    OAL_PTR_NULL,                                       /* SIOCWFIRSTPRIV+31 */  /* setrxtimeout */
};

/*****************************************************************************
  ????????iw_handler_def????
*****************************************************************************/
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
oal_iw_handler_def_stru g_st_iw_handler_def_etc =
{
    .standard           = g_ast_iw_handlers,
    .num_standard       = OAL_ARRAY_SIZE(g_ast_iw_handlers),
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,59))
#ifdef CONFIG_WEXT_PRIV
    .private            = g_ast_iw_priv_handlers,
    .num_private        = OAL_ARRAY_SIZE(g_ast_iw_priv_handlers),
    .private_args       = g_ast_iw_priv_args,
    .num_private_args   = OAL_ARRAY_SIZE(g_ast_iw_priv_args),
#endif
#else
    .private            = g_ast_iw_priv_handlers,
    .num_private        = OAL_ARRAY_SIZE(g_ast_iw_priv_handlers),
    .private_args       = g_ast_iw_priv_args,
    .num_private_args   = OAL_ARRAY_SIZE(g_ast_iw_priv_args),
#endif
    .get_wireless_stats = OAL_PTR_NULL
};

#elif (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION)
oal_iw_handler_def_stru g_st_iw_handler_def_etc =
{
    OAL_PTR_NULL,                      /* ????ioctl handler */
    0,
    OAL_ARRAY_SIZE(g_ast_iw_priv_handlers),
    {0, 0},                                 /* ???????? */
    OAL_ARRAY_SIZE(g_ast_iw_priv_args),
    g_ast_iw_priv_handlers,                 /* ????ioctl handler */
    g_ast_iw_priv_args,
    OAL_PTR_NULL
};
#endif

/*****************************************************************************
  ??????????????????
*****************************************************************************/
OAL_CONST wal_ioctl_mode_map_stru g_ast_mode_map_etc[] =
{
    /* legacy */
    {"11a",         WLAN_LEGACY_11A_MODE,       WLAN_BAND_5G,   WLAN_BAND_WIDTH_20M},
    {"11b",         WLAN_LEGACY_11B_MODE,       WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M},
    {"11bg",        WLAN_MIXED_ONE_11G_MODE,    WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M},
    {"11g",         WLAN_MIXED_TWO_11G_MODE,    WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M},

    /* 11n */
    {"11na20",      WLAN_HT_MODE,           WLAN_BAND_5G,   WLAN_BAND_WIDTH_20M},
    {"11ng20",      WLAN_HT_MODE,           WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M},
    {"11na40plus",  WLAN_HT_MODE,           WLAN_BAND_5G,   WLAN_BAND_WIDTH_40PLUS},
    {"11na40minus", WLAN_HT_MODE,           WLAN_BAND_5G,   WLAN_BAND_WIDTH_40MINUS},
    {"11ng40plus",  WLAN_HT_MODE,           WLAN_BAND_2G,   WLAN_BAND_WIDTH_40PLUS},
    {"11ng40minus", WLAN_HT_MODE,           WLAN_BAND_2G,   WLAN_BAND_WIDTH_40MINUS},

    /* 11ac */
    {"11ac20",              WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_20M},
    {"11ac40plus",          WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_40PLUS},
    {"11ac40minus",         WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_40MINUS},
    {"11ac80plusplus",      WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_80PLUSPLUS},
    {"11ac80plusminus",     WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_80PLUSMINUS},
    {"11ac80minusplus",     WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_80MINUSPLUS},
    {"11ac80minusminus",    WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_80MINUSMINUS},
#ifdef _PRE_WLAN_FEATURE_160M
    {"11ac160plusplusplus",      WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160PLUSPLUSPLUS},
    {"11ac160plusplusminus",     WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160PLUSPLUSMINUS},
    {"11ac160plusminusplus",     WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160PLUSMINUSPLUS},
    {"11ac160plusminusminus",    WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160PLUSMINUSMINUS},
    {"11ac160minusplusplus",      WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160MINUSPLUSPLUS},
    {"11ac160minusplusminus",     WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160MINUSPLUSMINUS},
    {"11ac160minusminusplus",     WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160MINUSMINUSPLUS},
    {"11ac160minusminusminus",    WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160MINUSMINUSMINUS},
#endif

    {"11ac2g20",            WLAN_VHT_MODE,  WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M},
    {"11ac2g40plus",        WLAN_VHT_MODE,  WLAN_BAND_2G,   WLAN_BAND_WIDTH_40PLUS},
    {"11ac2g40minus",       WLAN_VHT_MODE,  WLAN_BAND_2G,   WLAN_BAND_WIDTH_40MINUS},
    /* 11n only and 11ac only, ????20M???? */
    {"11nonly2g",           WLAN_HT_ONLY_MODE,   WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M},
    {"11nonly5g",           WLAN_HT_ONLY_MODE,   WLAN_BAND_5G,   WLAN_BAND_WIDTH_20M},
    {"11aconly",            WLAN_VHT_ONLY_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_20M},

/* 1151????????ONT?????????????????????????????????????????? */
    {"11ng40",              WLAN_HT_MODE,   WLAN_BAND_2G,   WLAN_BAND_WIDTH_40M},
    {"11ac2g40",            WLAN_VHT_MODE,  WLAN_BAND_2G,   WLAN_BAND_WIDTH_40M},
    {"11na40",              WLAN_HT_MODE,   WLAN_BAND_5G,   WLAN_BAND_WIDTH_40M},
    {"11ac40",              WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_40M},
    {"11ac80",              WLAN_VHT_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_80M},

#ifdef _PRE_WLAN_FEATURE_11AX
    /* 11ax */
    {"11ax2g20",              WLAN_HE_MODE,           WLAN_BAND_2G,   WLAN_BAND_WIDTH_20M},
    {"11ax2g40plus",          WLAN_HE_MODE,           WLAN_BAND_2G,   WLAN_BAND_WIDTH_40PLUS},
    {"11ax2g40minus",         WLAN_HE_MODE,           WLAN_BAND_2G,   WLAN_BAND_WIDTH_40MINUS},
    {"11ax5g20",              WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_20M},
    {"11ax5g40plus",          WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_40PLUS},
    {"11ax5g40minus",         WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_40MINUS},
    {"11ax5g80plusplus",      WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_80PLUSPLUS},
    {"11ax5g80plusminus",     WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_80PLUSMINUS},
    {"11ax5g80minusplus",     WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_80MINUSPLUS},
    {"11ax5g80minusminus",    WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_80MINUSMINUS},

#ifdef _PRE_WLAN_FEATURE_160M
    {"11ax5g160plusplusplus",      WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160PLUSPLUSPLUS},
    {"11ax5g160plusplusminus",     WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160PLUSPLUSMINUS},
    {"11ax5g160plusminusplus",     WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160PLUSMINUSPLUS},
    {"11ax5g160plusminusminus",    WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160PLUSMINUSMINUS},
    {"11ax5g160minusplusplus",     WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160MINUSPLUSPLUS},
    {"11ax5g160minusplusminus",    WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160MINUSPLUSMINUS},
    {"11ax5g160minusminusplus",    WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160MINUSMINUSPLUS},
    {"11ax5g160minusminusminus",   WLAN_HE_MODE,  WLAN_BAND_5G,   WLAN_BAND_WIDTH_160MINUSMINUSMINUS},
#endif
#endif

    {OAL_PTR_NULL}
};


/* ????! ???????????????????? g_dmac_config_set_dscr_param????????????????????! */
OAL_CONST oal_int8   *pauc_tx_dscr_param_name_etc[WAL_DSCR_PARAM_BUTT] =
{
    "pgl",
    "mtpgl",
    "ta",
    "ra",
    "cc",
    "data0",/*05 ??????????????????????mcs*????  */
    "data1",/*05 ??????????????????????mcs*????  */
    "data2",/*05 ??????????????????????mcs*????  */
    "data3",/*05 ??????????????????????mcs*????  */
    "power",
    "shortgi",
    "preamble",
    "rtscts",
    "lsigtxop",
    "smooth",
    "snding",
    "txbf",
    "stbc",
    "rd_ess",
    "dyn_bw",
    "dyn_bw_exist",
    "ch_bw_exist",
    "rate",
    "mcs",
    "mcsac",
    "mcsax",
    "nss",
    "bw",
    "ltf",
    "gi",
    "txchain",
    "dcm"
};

OAL_CONST oal_int8   *pauc_tx_pow_param_name[WAL_TX_POW_PARAM_BUTT] =
{
    "rf_reg_ctl",
    "fix_level",
    "mag_level",
    "ctl_level",
    "amend",
    "no_margin",
    "show_log",
    "sar_level",
#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
    "tas_pwr_ctrl",
    "tas_rssi_measure",
    "tas_ant_switch",
#endif
    "show_tpc_tbl_gain",
    "pow_save",
};

OAL_STATIC OAL_CONST oal_int8   *pauc_tx_dscr_nss_tbl[WLAN_NSS_LIMIT] =
{
    "1",
    "2",
    "3",
    "4"
};

//#ifdef    _PRE_WLAN_CHIP_TEST
OAL_CONST oal_int8   *pauc_bw_tbl[WLAN_BANDWITH_CAP_BUTT] =
{
    "20",
    "40",
    "d40",
    "80",
    "d80",
    "160",
    "d160",
    "80_80",
#ifdef _PRE_WLAN_FEATURE_11AX_ER_SU
    "242tone",
    "106tone",
#endif
};

OAL_STATIC OAL_CONST oal_int8   *pauc_non_ht_rate_tbl[WLAN_LEGACY_RATE_VALUE_BUTT] =
{
    "1",
    "2",
    "5.5",
    "11",
    "rsv0",
    "rsv1",
    "rsv2",
    "rsv3",
    "48",
    "24",
    "12",
    "6",
    "54",
    "36",
    "18",
    "9"
};
//#endif  /* _PRE_WLAN_CHIP_TEST */

OAL_CONST wal_ioctl_dyn_cali_stru g_ast_dyn_cali_cfg_map[]=
{
    {"realtime_cali_adjust",    MAC_DYN_CALI_CFG_SET_EN_REALTIME_CALI_ADJUST},
    {"2g_dscr_interval",        MAC_DYN_CALI_CFG_SET_2G_DSCR_INT},
    {"5g_dscr_interval",        MAC_DYN_CALI_CFG_SET_5G_DSCR_INT},
    {"chain_interval",          MAC_DYN_CALI_CFG_SET_CHAIN_INT},
    {"pdet_min_th",             MAC_DYN_CALI_CFG_SET_PDET_MIN_TH},
    {"pdet_max_th",             MAC_DYN_CALI_CFG_SET_PDET_MAX_TH},
#ifdef _PRE_WLAN_DPINIT_CALI
    {"get_dpinit_val",          MAC_DYN_CALI_CFG_GET_DPINIT_VAL},
#endif
    {OAL_PTR_NULL}
};


OAL_CONST wal_ioctl_alg_cfg_stru g_ast_alg_cfg_map_etc[] =
{
    {"sch_vi_ctrl_ena",         MAC_ALG_CFG_SCHEDULE_VI_CTRL_ENA},
    {"sch_bebk_minbw_ena",      MAC_ALG_CFG_SCHEDULE_BEBK_MIN_BW_ENA},
    {"sch_mvap_sch_ena",        MAC_ALG_CFG_SCHEDULE_MVAP_SCH_ENA},
    {"sch_vi_sch_ms",           MAC_ALG_CFG_SCHEDULE_VI_SCH_LIMIT},
    {"sch_vo_sch_ms",           MAC_ALG_CFG_SCHEDULE_VO_SCH_LIMIT},
    {"sch_vi_drop_ms",          MAC_ALG_CFG_SCHEDULE_VI_DROP_LIMIT},
    {"sch_vi_ctrl_ms",          MAC_ALG_CFG_SCHEDULE_VI_CTRL_MS},
    {"sch_vi_life_ms",          MAC_ALG_CFG_SCHEDULE_VI_MSDU_LIFE_MS},
    {"sch_vo_life_ms",          MAC_ALG_CFG_SCHEDULE_VO_MSDU_LIFE_MS},
    {"sch_be_life_ms",          MAC_ALG_CFG_SCHEDULE_BE_MSDU_LIFE_MS},
    {"sch_bk_life_ms",          MAC_ALG_CFG_SCHEDULE_BK_MSDU_LIFE_MS},
    {"sch_vi_low_delay",        MAC_ALG_CFG_SCHEDULE_VI_LOW_DELAY_MS},
    {"sch_vi_high_delay",       MAC_ALG_CFG_SCHEDULE_VI_HIGH_DELAY_MS},
    {"sch_cycle_ms",            MAC_ALG_CFG_SCHEDULE_SCH_CYCLE_MS},
    {"sch_ctrl_cycle_ms",       MAC_ALG_CFG_SCHEDULE_TRAFFIC_CTRL_CYCLE},
    {"sch_cir_nvip_kbps",       MAC_ALG_CFG_SCHEDULE_CIR_NVIP_KBPS},
    {"sch_cir_nvip_be",         MAC_ALG_CFG_SCHEDULE_CIR_NVIP_KBPS_BE},
    {"sch_cir_nvip_bk",         MAC_ALG_CFG_SCHEDULE_CIR_NVIP_KBPS_BK},
    {"sch_cir_vip_kbps",        MAC_ALG_CFG_SCHEDULE_CIR_VIP_KBPS},
    {"sch_cir_vip_be",          MAC_ALG_CFG_SCHEDULE_CIR_VIP_KBPS_BE},
    {"sch_cir_vip_bk",          MAC_ALG_CFG_SCHEDULE_CIR_VIP_KBPS_BK},
    {"sch_cir_vap_kbps",        MAC_ALG_CFG_SCHEDULE_CIR_VAP_KBPS},
    {"sch_sm_delay_ms",         MAC_ALG_CFG_SCHEDULE_SM_TRAIN_DELAY},
    {"sch_drop_pkt_limit",      MAC_ALG_CFG_VIDEO_DROP_PKT_LIMIT},
    {"sch_flowctl_ena",         MAC_ALG_CFG_FLOWCTRL_ENABLE_FLAG},
    {"sch_log_start",           MAC_ALG_CFG_SCHEDULE_LOG_START},
    {"sch_log_end",             MAC_ALG_CFG_SCHEDULE_LOG_END},
    {"sch_method",              MAC_ALG_CFG_SCHEDULE_SCH_METHOD},
    {"sch_fix_mode",            MAC_ALG_CFG_SCHEDULE_FIX_SCH_MODE},
    {"sch_vap_prio",            MAC_ALG_CFG_SCHEDULE_VAP_SCH_PRIO},

    {"txbf_switch",             MAC_ALG_CFG_TXBF_MASTER_SWITCH},
    {"txbf_txmode_enb",         MAC_ALG_CFG_TXBF_TXMODE_ENABLE},
    {"txbf_11nbfee_enb",        MAC_ALG_CFG_TXBF_11N_BFEE_ENABLE},
    {"txbf_2g_bfer",            MAC_ALG_CFG_TXBF_2G_BFER_ENABLE},
    {"txbf_2nss_bfer",          MAC_ALG_CFG_TXBF_2NSS_BFER_ENABLE},
    {"txbf_fix_mode",           MAC_ALG_CFG_TXBF_FIX_MODE},
    {"txbf_fix_sound",          MAC_ALG_CFG_TXBF_FIX_SOUNDING},
    {"txbf_probe_int",          MAC_ALG_CFG_TXBF_PROBE_INT},
    {"txbf_rm_worst",           MAC_ALG_CFG_TXBF_REMOVE_WORST},
    {"txbf_stbl_num",           MAC_ALG_CFG_TXBF_STABLE_NUM},
    {"txbf_probe_cnt",          MAC_ALG_CFG_TXBF_PROBE_COUNT},
#if WLAN_TXBF_BFER_LOG_ENABLE
    {"txbf_log_enb",            MAC_ALG_CFG_TXBF_LOG_ENABLE},
    {"txbf_log_sta",            MAC_ALG_CFG_TXBF_RECORD_LOG_START},
    {"txbf_log_out",            MAC_ALG_CFG_TXBF_LOG_OUTPUT},
#endif
    {"ar_enable",               MAC_ALG_CFG_AUTORATE_ENABLE},                   /* ????????????????????????: sh hipriv.sh "vap0 alg_cfg ar_enable [1|0]" */
    {"ar_use_lowest",           MAC_ALG_CFG_AUTORATE_USE_LOWEST_RATE},          /* ??????????????????????: sh hipriv.sh "vap0 alg_cfg ar_use_lowest [1|0]" */
    {"ar_short_num",            MAC_ALG_CFG_AUTORATE_SHORT_STAT_NUM},           /* ????????????????????:sh hipriv.sh "vap0 alg_cfg ar_short_num [??????]" */
    {"ar_short_shift",          MAC_ALG_CFG_AUTORATE_SHORT_STAT_SHIFT},         /* ??????????????????????:sh hipriv.sh "vap0 alg_cfg ar_short_shift [??????]" */
    {"ar_long_num",             MAC_ALG_CFG_AUTORATE_LONG_STAT_NUM},            /* ????????????????????:sh hipriv.sh "vap0 alg_cfg ar_long_num [??????]" */
    {"ar_long_shift",           MAC_ALG_CFG_AUTORATE_LONG_STAT_SHIFT},          /* ??????????????????????:sh hipriv.sh "vap0 alg_cfg ar_long_shift [??????]" */
    {"ar_min_probe_up_no",      MAC_ALG_CFG_AUTORATE_MIN_PROBE_UP_INTVL_PKTNUM},   /* ??????????????????:sh hipriv.sh "vap0 alg_cfg ar_min_probe_no [??????]" */
    {"ar_min_probe_down_no",    MAC_ALG_CFG_AUTORATE_MIN_PROBE_DOWN_INTVL_PKTNUM},   /* ??????????????????:sh hipriv.sh "vap0 alg_cfg ar_min_probe_no [??????]" */
    {"ar_max_probe_no",         MAC_ALG_CFG_AUTORATE_MAX_PROBE_INTVL_PKTNUM},   /* ??????????????????:sh hipriv.sh "vap0 alg_cfg ar_max_probe_no [??????]" */
    {"ar_keep_times",           MAC_ALG_CFG_AUTORATE_PROBE_INTVL_KEEP_TIMES},   /* ????????????????????:sh hipriv.sh "vap0 alg_cfg ar_keep_times [????]" */
    {"ar_delta_ratio",          MAC_ALG_CFG_AUTORATE_DELTA_GOODPUT_RATIO},      /* ????goodput????????(??????????300):sh hipriv.sh "vap0 alg_cfg ar_delta_ratio [??????]" */
    {"ar_vi_per_limit",         MAC_ALG_CFG_AUTORATE_VI_PROBE_PER_LIMIT},       /* ????vi??per????(??????????300):sh hipriv.sh "vap0 alg_cfg ar_vi_per_limit [??????]" */
    {"ar_vo_per_limit",         MAC_ALG_CFG_AUTORATE_VO_PROBE_PER_LIMIT},       /* ????vo??per????(??????????300):sh hipriv.sh "vap0 alg_cfg ar_vo_per_limit [??????]" */
    {"ar_ampdu_time",           MAC_ALG_CFG_AUTORATE_AMPDU_DURATION},           /* ????ampdu??durattion??:sh hipriv.sh "vap0 alg_cfg ar_ampdu_time [??????]" */
    {"ar_cont_loss_num",        MAC_ALG_CFG_AUTORATE_MCS0_CONT_LOSS_NUM},       /* ????mcs0??????????????:sh hipriv.sh "vap0 alg_cfg ar_cont_loss_num [??????]" */
    {"ar_11b_diff_rssi",        MAC_ALG_CFG_AUTORATE_UP_PROTOCOL_DIFF_RSSI},    /* ????????11b??rssi????:sh hipriv.sh "vap0 alg_cfg ar_11b_diff_rssi [????]" */
    {"ar_rts_mode",             MAC_ALG_CFG_AUTORATE_RTS_MODE},                 /* ????rts????:sh hipriv.sh "vap0 alg_cfg ar_rts_mode [0(??????)|1(????)|2(rate[0]????RTS, rate[1..3]????RTS)|3(rate[0]????RTS, rate[1..3]????RTS)]" */
    {"ar_legacy_loss",          MAC_ALG_CFG_AUTORATE_LEGACY_1ST_LOSS_RATIO_TH}, /* ????Legacy??????????????:sh hipriv.sh "vap0 alg_cfg ar_legacy_loss [????]" */
    {"ar_ht_vht_loss",          MAC_ALG_CFG_AUTORATE_HT_VHT_1ST_LOSS_RATIO_TH}, /* ????Legacy??????????????:sh hipriv.sh "vap0 alg_cfg ar_ht_vht_loss [????]" */
    {"ar_stat_log_do",          MAC_ALG_CFG_AUTORATE_STAT_LOG_START},           /* ????????????????:sh hipriv.sh "vap0 alg_cfg ar_stat_log_do [mac????] [????????] [??????]" ??: sh hipriv.sh "vap0 alg_cfg ar_stat_log_do 06:31:04:E3:81:02 1 1000" */
    {"ar_sel_log_do",           MAC_ALG_CFG_AUTORATE_SELECTION_LOG_START},      /* ????????????????:sh hipriv.sh "vap0 alg_cfg ar_sel_log_do [mac????] [????????] [??????]" ??: sh hipriv.sh "vap0 alg_cfg ar_sel_log_do 06:31:04:E3:81:02 1 200" */
    {"ar_fix_log_do",           MAC_ALG_CFG_AUTORATE_FIX_RATE_LOG_START},       /* ????????????????:sh hipriv.sh "vap0 alg_cfg ar_fix_log_do [mac????] [tidno] [per????]" ??: sh hipriv.sh "vap0 alg_cfg ar_sel_log_do 06:31:04:E3:81:02 1 200" */
    {"ar_aggr_log_do",          MAC_ALG_CFG_AUTORATE_AGGR_STAT_LOG_START},      /* ??????????????????:sh hipriv.sh "vap0 alg_cfg ar_fix_log_do [mac????] [tidno]" ??: sh hipriv.sh "vap0 alg_cfg ar_sel_log_do 06:31:04:E3:81:02 1 " */
    {"ar_st_log_out",           MAC_ALG_CFG_AUTORATE_STAT_LOG_WRITE},           /* ????????????????:sh hipriv.sh "vap0 alg_cfg ar_st_log_out 06:31:04:E3:81:02" */
    {"ar_sel_log_out",          MAC_ALG_CFG_AUTORATE_SELECTION_LOG_WRITE},      /* ????????????????:sh hipriv.sh "vap0 alg_cfg ar_sel_log_out 06:31:04:E3:81:02" */
    {"ar_fix_log_out",          MAC_ALG_CFG_AUTORATE_FIX_RATE_LOG_WRITE},       /* ????????????????:sh hipriv.sh "vap0 alg_cfg ar_fix_log_out 06:31:04:E3:81:02" */
    {"ar_aggr_log_out",         MAC_ALG_CFG_AUTORATE_AGGR_STAT_LOG_WRITE},      /* ????????????????:sh hipriv.sh "vap0 alg_cfg ar_fix_log_out 06:31:04:E3:81:02" */
    {"ar_disp_rateset",         MAC_ALG_CFG_AUTORATE_DISPLAY_RATE_SET},         /* ????????????:sh hipriv.sh "vap0 alg_cfg ar_disp_rateset 06:31:04:E3:81:02" */
    {"ar_cfg_fix_rate",         MAC_ALG_CFG_AUTORATE_CONFIG_FIX_RATE},          /* ????????????:sh hipriv.sh "vap0 alg_cfg ar_cfg_fix_rate 06:31:04:E3:81:02 0" */
    {"ar_disp_rx_rate",         MAC_ALG_CFG_AUTORATE_DISPLAY_RX_RATE},          /* ????????????????:sh hipriv.sh "vap0 alg_cfg ar_disp_rx_rate 06:31:04:E3:81:02" */
    {"ar_log_enable",           MAC_ALG_CFG_AUTORATE_LOG_ENABLE},               /* ????????????????????????: sh hipriv.sh "vap0 alg_cfg ar_log_enable [1|0]" */
    {"ar_max_vo_rate",          MAC_ALG_CFG_AUTORATE_VO_RATE_LIMIT},            /* ??????????VO????: sh hipriv.sh "vap0 alg_cfg ar_max_vo_rate [??????]" */
    {"ar_fading_per_th",        MAC_ALG_CFG_AUTORATE_JUDGE_FADING_PER_TH},      /* ????????????per??????: sh hipriv.sh "vap0 alg_cfg ar_fading_per_th [per??????(??????)]"*/
    {"ar_aggr_opt",             MAC_ALG_CFG_AUTORATE_AGGR_OPT},                 /* ??????????????????: sh hipriv.sh "vap0 alg_cfg ar_aggr_opt [1|0]"*/
    {"ar_aggr_pb_intvl",        MAC_ALG_CFG_AUTORATE_AGGR_PROBE_INTVL_NUM},     /* ??????????????????????: sh hipriv.sh "vap0 alg_cfg ar_aggr_pb_intvl [????????]"*/
    {"ar_aggr_st_shift",        MAC_ALG_CFG_AUTORATE_AGGR_STAT_SHIFT},          /* ????????????????????????: sh hipriv.sh "vap0 alg_cfg ar_aggr_st_shift [??????????]"*/
    {"ar_dbac_aggrtime",        MAC_ALG_CFG_AUTORATE_DBAC_AGGR_TIME},           /* ????DBAC????????????????????: sh hipriv.sh "vap0 alg_cfg ar_dbac_aggrtime [????????????(us)]"*/
    {"ar_dbg_vi_status",        MAC_ALG_CFG_AUTORATE_DBG_VI_STATUS},            /* ????????????VI????: sh hipriv.sh "vap0 alg_cfg ar_dbg_vi_status [0/1/2]"*/
    {"ar_dbg_aggr_log",         MAC_ALG_CFG_AUTORATE_DBG_AGGR_LOG},             /* ??????????log????: sh hipriv.sh "vap0 alg_cfg ar_dbg_aggr_log [0/1]"*/
    {"ar_aggr_pck_num",         MAC_ALG_CFG_AUTORATE_AGGR_NON_PROBE_PCK_NUM},   /* ????????????????????????????????????: sh hipriv.sh "vap0 alg_cfg ar_aggr_pck_num [??????]"*/
    {"ar_min_aggr_idx",         MAC_ALG_CFG_AUTORATE_AGGR_MIN_AGGR_TIME_IDX},   /* ????????????????: sh hipriv.sh "vap0 alg_cfg ar_aggr_min_idx [??????]"*/
    {"ar_250us_dper_th",        MAC_ALG_CFG_AUTORATE_AGGR_250US_DELTA_PER_TH},  /* ????????250us??????deltaPER????: sh hipriv.sh "vap0 alg_cfg ar_250us_dper_th [??????]"*/
    {"ar_max_aggr_num",         MAC_ALG_CFG_AUTORATE_MAX_AGGR_NUM},             /* ????????????????: sh hipriv.sh "vap0 alg_cfg ar_max_aggr_num [????????]" */
    {"ar_1mpdu_per_th",         MAC_ALG_CFG_AUTORATE_LIMIT_1MPDU_PER_TH},       /* ??????????MCS??????????1??PER????: sh hipriv.sh "vap0 alg_cfg ar_1mpdu_per_th [per??????(??????)]" */

    {"ar_btcoxe_probe",         MAC_ALG_CFG_AUTORATE_BTCOEX_PROBE_ENABLE},      /* ??????????????????????: sh hipriv.sh "vap0 alg_cfg ar_btcoxe_probe [1|0]" */
    {"ar_btcoxe_aggr",          MAC_ALG_CFG_AUTORATE_BTCOEX_AGGR_ENABLE},       /* ??????????????????????: sh hipriv.sh "vap0 alg_cfg ar_btcoxe_aggr [1|0]" */
    {"ar_coxe_intvl",           MAC_ALG_CFG_AUTORATE_COEX_STAT_INTVL},          /* ????????????????????????: sh hipriv.sh "vap0 alg_cfg ar_coxe_intvl [????????ms]"*/
    {"ar_coxe_low_th",          MAC_ALG_CFG_AUTORATE_COEX_LOW_ABORT_TH},        /* ????????abort??????????????: sh hipriv.sh "vap0 alg_cfg ar_coxe_low_th [??????]"*/
    {"ar_coxe_high_th",         MAC_ALG_CFG_AUTORATE_COEX_HIGH_ABORT_TH},       /* ????????abort??????????????: sh hipriv.sh "vap0 alg_cfg ar_coxe_high_th [??????]"*/
    {"ar_coxe_agrr_th",         MAC_ALG_CFG_AUTORATE_COEX_AGRR_NUM_ONE_TH},     /* ??????????????????1??????????: sh hipriv.sh "vap0 alg_cfg ar_coxe_agrr_th [??????]"*/

    {"ar_dyn_bw_en",            MAC_ALG_CFG_AUTORATE_DYNAMIC_BW_ENABLE},        /* ????????????????????: sh hipriv.sh "vap0 alg_cfg ar_dyn_bw_en [0/1]" */
    {"ar_thpt_wave_opt",        MAC_ALG_CFG_AUTORATE_THRPT_WAVE_OPT},           /* ??????????????????: sh hipriv.sh "vap0 alg_cfg ar_thpt_wave_opt [0/1]" */
    {"ar_gdpt_diff_th",         MAC_ALG_CFG_AUTORATE_GOODPUT_DIFF_TH},          /* ????????????????????goodput????????????(??????): sh hipriv.sh "vap0 alg_cfg ar_gdpt_diff_th [goodput????????????(??????)]" */
    {"ar_per_worse_th",         MAC_ALG_CFG_AUTORATE_PER_WORSE_TH},             /* ????????????????????PER??????????(??????): sh hipriv.sh "vap0 alg_cfg ar_per_worse_th [PER????????(??????)]" */
    {"ar_cts_no_ack_num",       MAC_ALG_CFG_AUTORATE_RX_CTS_NO_BA_NUM},         /* ??????RTS????CTS????DATA??????BA??????????????????????: sh hipriv.sh "vap0 alg_cfg ar_cts_no_ba_num [????]" */
    {"ar_vo_aggr",              MAL_ALG_CFG_AUTORATE_VOICE_AGGR},               /* ????????????voice????????: sh hipriv.sh "vap0 alg_cfg ar_vo_aggr [0/1]" */
    {"ar_fast_smth_shft",       MAC_ALG_CFG_AUTORATE_FAST_SMOOTH_SHIFT},        /* ????????????????????????????????: sh hipriv.sh "vap0 alg_cfg ar_fast_smth_shft [??????]" (??255????????????????)*/
    {"ar_fast_smth_aggr_num",   MAC_ALG_CFG_AUTORATE_FAST_SMOOTH_AGGR_NUM},     /* ??????????????????????????????????: sh hipriv.sh "vap0 alg_cfg ar_fast_smth_aggr_num [????????????]" */
    {"ar_sgi_punish_per",       MAC_ALG_CFG_AUTORATE_SGI_PUNISH_PER},           /* ????short GI??????PER??????(??????): sh hipriv.sh "vap0 alg_cfg ar_sgi_punish_per [PER??????(??????)]" */
    {"ar_sgi_punish_num",       MAC_ALG_CFG_AUTORATE_SGI_PUNISH_NUM},           /* ????short GI??????????????????: sh hipriv.sh "vap0 alg_cfg ar_sgi_punish_num [????????????]" */
#ifdef _PRE_WLAN_FEATURE_MWO_DET
    {"ar_fourth_rate",          MAL_ALG_CFG_AUTORATE_LAST_RATE_RANK_INDEX},     /*????????????????????????????????8ms ????????*/
    {"ar_mwo_det_debug",        MAL_ALG_CFG_AUTORATE_MWO_DET_DEBUG},            /*????autorate????????????????????????????????????*/
    {"ar_mwo_per_log",          MAL_ALG_CFG_AUTORATE_MWO_DET_PER_LOG},          /*????????????????????per??????????????????????????3000ms,????ms */
#endif
    {"ar_rxch_agc_opt",         MAC_ALG_CFG_AUTORATE_RXCH_AGC_OPT},             /* ????????????AGC????????????: sh hipriv.sh "vap0 alg_cfg ar_rxch_agc_opt [1/0]" */
    {"ar_rxch_agc_log",         MAC_ALG_CFG_AUTORATE_RXCH_AGC_LOG},             /* ????????????AGC????????????: sh hipriv.sh "vap0 alg_cfg ar_rxch_agc_log [1/0]" */
    {"ar_weak_rssi_th",         MAC_ALG_CFG_AUTORATE_WEAK_RSSI_TH},             /* ????????????AGC????????????RSSI????: sh hipriv.sh "vap0 alg_cfg ar_weak_rssi_th [RSSI??????]" (????: RSSI??-90dBm, ??????????90) */
    {"ar_rxch_stat_period",     MAC_ALG_CFG_AUTORATE_RXCH_STAT_PERIOD},         /* ??????????????????????(??????????): sh hipriv.sh "vap0 alg_cfg ar_weak_rssi_th [????????]" */
    {"ar_rts_one_tcp_dbg",      MAC_ALG_CFG_AUTORATE_RTS_ONE_TCP_DBG},          /* ????RTS??????????TCP??????????????: sh hipriv.sh "vap0 alg_cfg ar_rts_one_tcp_dbg [1/0]" */
    {"ar_scan_user_opt",        MAC_ALG_CFG_AUTORATE_SCAN_USER_OPT},            /* ??????????????????????: sh hipriv.sh "vap0 alg_cfg ar_scan_user_opt [1/0]" */
    {"ar_max_tx_cnt",           MAC_ALG_CFG_AUTORATE_MAX_TX_COUNT},             /* ??????????????????????????????: sh hipriv.sh "vap0 alg_cfg ar_max_tx_cnt [????????]" */
    {"ar_80M_40M_switch",       MAC_ALG_CFG_AUTORATE_80M_40M_SWITCH},           /* ????????????80M ??40M??????  sh hipriv.sh "vap0 alg_cfg ar_80M_40M_switch [1/0]" */
    {"ar_40M_switch_thr",       MAC_ALG_CFG_AUTORATE_40M_SWITCH_THR},           /* ????80M ??40M mcs????  sh hipriv.sh "vap0 alg_cfg ar_40M_switch_thr [mcs]" */
    {"ar_collision_det",        MAC_ALG_CFG_AUTORATE_COLLISION_DET_EN},
    {"sm_train_num",            MAC_ALG_CFG_SMARTANT_TRAINING_PACKET_NUMBER},
    {"sm_change_ant",           MAC_ALG_CFG_SMARTANT_CHANGE_ANT},
    {"sm_enable",               MAC_ALG_CFG_SMARTANT_ENABLE},
    {"sm_certain_ant",          MAC_ALG_CFG_SMARTANT_CERTAIN_ANT},
    {"sm_start",                MAC_ALG_CFG_SMARTANT_START_TRAIN},
    {"sm_train_packet",         MAC_ALG_CFG_SMARTANT_SET_TRAINING_PACKET_NUMBER},
    {"sm_min_packet",           MAC_ALG_CFG_SMARTANT_SET_LEAST_PACKET_NUMBER},
    {"sm_ant_interval",         MAC_ALG_CFG_SMARTANT_SET_ANT_CHANGE_INTERVAL},
    {"sm_user_interval",        MAC_ALG_CFG_SMARTANT_SET_USER_CHANGE_INTERVAL},
    {"sm_max_period",           MAC_ALG_CFG_SMARTANT_SET_PERIOD_MAX_FACTOR},
    {"sm_change_freq",          MAC_ALG_CFG_SMARTANT_SET_ANT_CHANGE_FREQ},
    {"sm_change_th",            MAC_ALG_CFG_SMARTANT_SET_ANT_CHANGE_THRESHOLD},

    {"anti_inf_imm_en",         MAC_ALG_CFG_ANTI_INTF_IMM_ENABLE},      /* ????????????non-direct????: sh hipriv.sh "vap0 alg_cfg anti_inf_imm_en 0|1" */
    {"anti_inf_unlock_en",      MAC_ALG_CFG_ANTI_INTF_UNLOCK_ENABLE},   /* ????????????dynamic unlock????: sh hipriv.sh "vap0 alg_cfg anti_inf_unlock_en 0|1" */
    {"anti_inf_stat_time",      MAC_ALG_CFG_ANTI_INTF_RSSI_STAT_CYCLE}, /* ????????????rssi????????: sh hipriv.sh "vap0 anti_inf_stat_time [time]" */
    {"anti_inf_off_time",       MAC_ALG_CFG_ANTI_INTF_UNLOCK_CYCLE},    /* ????????????unlock????????: sh hipriv.sh "vap0 anti_inf_off_time [time]" */
    {"anti_inf_off_dur",        MAC_ALG_CFG_ANTI_INTF_UNLOCK_DUR_TIME}, /* ????????????unlock????????????: sh hipriv.sh "vap0 anti_inf_off_dur [time]" */
    {"anti_inf_nav_en",         MAC_ALG_CFG_ANTI_INTF_NAV_IMM_ENABLE},  /* ??????nav????????: sh hipriv.sh "vap0 alg_cfg anti_inf_nav_en 0|1" */
    {"anti_inf_gd_th",          MAC_ALG_CFG_ANTI_INTF_GOODPUT_FALL_TH}, /* ??????????goodput????????: sh hipriv.sh "vap0 alg_cfg anti_inf_gd_th [num]" */
    {"anti_inf_keep_max",       MAC_ALG_CFG_ANTI_INTF_KEEP_CYC_MAX_NUM},/* ????????????????????????????: sh hipriv.sh "vap0 alg_cfg anti_inf_keep_max [num]" */
    {"anti_inf_keep_min",       MAC_ALG_CFG_ANTI_INTF_KEEP_CYC_MIN_NUM},/* ????????????????????????????: sh hipriv.sh "vap0 alg_cfg anti_inf_keep_min [num]" */
    {"anti_inf_per_pro_en",     MAC_ALG_CFG_ANTI_INTF_PER_PROBE_EN},    /* ??????????????????tx time????: sh hipriv.sh "vap0 anti_inf_tx_pro_en 0|1" */
    {"anti_inf_txtime_th",      MAC_ALG_CFG_ANTI_INTF_TX_TIME_FALL_TH}, /* tx time????????: sh hipriv.sh "vap0 alg_cfg anti_inf_txtime_th [val]"*/
    {"anti_inf_per_th",         MAC_ALG_CFG_ANTI_INTF_PER_FALL_TH},     /* per????????: sh hipriv.sh "vap0 alg_cfg anti_inf_per_th [val]"*/
    {"anti_inf_gd_jitter_th",   MAC_ALG_CFG_ANTI_INTF_GOODPUT_JITTER_TH},/* goodput????????: sh hipriv.sh "vap0 alg_cfg anti_inf_gd_jitter_th [val]"*/
    {"anti_inf_debug_mode",     MAC_ALG_CFG_ANTI_INTF_DEBUG_MODE},      /* ??????????debug??????????: sh hipriv.sh "vap0 alg_cfg anti_inf_debug_mode 0|1|2" */

    {"intf_det_cycle",          MAC_ALG_CFG_INTF_DET_CYCLE},            /* ????????????????(ms):sh hipriv.sh "vap0 alg_cfg intf_det_cycle [val]" */
    {"intf_det_mode",           MAC_ALG_CFG_INTF_DET_MODE},             /* ????????????????(??????/??????????/????):sh hipriv.sh "vap0 alg_cfg intf_det_mode 0|1" */
    {"intf_det_debug",          MAC_ALG_CFG_INTF_DET_DEBUG},            /* ????????????debug????(????bit????????):sh hipriv.sh "vap0 alg_cfg intf_det_debug 0|1" */
    {"intf_det_cothr_sta",      MAC_ALG_CFG_INTF_DET_COCH_THR_STA},     /* ????????????sta????(??????x):sh hipriv.sh "vap0 alg_cfg intf_det_cothr_sta [val]" */
    {"intf_det_nointf_thr_sta", MAC_ALG_CFG_INTF_DET_COCH_NOINTF_STA},  /* ????????????sta??????????(??????x):sh hipriv.sh "vap0 alg_cfg intf_det_nointf_thr_sta [val]" */
    {"intf_det_cothr_udp",      MAC_ALG_CFG_INTF_DET_COCH_THR_UDP},     /* ????????????ap udp????(??????x):sh hipriv.sh "vap0 alg_cfg intf_det_cothr_udp [val]" */
    {"intf_det_cothr_tcp",      MAC_ALG_CFG_INTF_DET_COCH_THR_TCP},     /* ????????????ap tcp????(??????x):sh hipriv.sh "vap0 alg_cfg intf_det_cothr_tcp [val]" */
    {"intf_det_adjscan_cyc",    MAC_ALG_CFG_INTF_DET_ADJCH_SCAN_CYC},   /* ????????????????????????????:sh hipriv.sh "vap0 alg_cfg intf_det_adjscan_cyc [val]" */
    {"intf_det_adjratio_thr",   MAC_ALG_CFG_INTF_DET_ADJRATIO_THR},     /* ??????????????????????????????????(??????x):sh hipriv.sh "vap0 alg_cfg intf_det_adjratio_thr [val]" */
    {"intf_det_sync_th",        MAC_ALG_CFG_INTF_DET_SYNC_THR},         /* ????????????????????????sync error????(??????x):sh hipriv.sh "vap0 alg_cfg intf_det_sync_th [val]"*/
    {"intf_det_ave_rssi",       MAC_ALG_CFG_INTF_DET_AVE_RSSI},         /* ????????????????????????????rssi????(??????x):sh hipriv.sh "vap0 alg_cfg intf_det_ave_rssi [val]"*/
    {"intf_det_no_adjratio_th", MAC_ALG_CFG_INTF_DET_NO_ADJRATIO_TH},   /* ????????????????/??????????????????(??????x):sh hipriv.sh "vap0 alg_cfg intf_det_no_adjratio_th [val]" */
    {"intf_det_no_adjcyc_th",   MAC_ALG_CFG_INTF_DET_NO_ADJCYC_TH},     /* ????????????????/????????????????:sh hipriv.sh "vap0 alg_cfg intf_det_no_adjcyc_th [val]" */
    {"intf_det_collision_th",   MAC_ALG_CFG_INTF_DET_COLLISION_TH},     /* ????????????????/????????????????:sh hipriv.sh "vap0 alg_cfg intf_det_collision_th [val] */

    {"neg_det_noprobe_th",      MAC_ALG_CFG_NEG_DET_NONPROBE_TH},       /* ????????????????????????????:sh hipriv.sh "vap0 alg_cfg neg_det_noprobe_th [val]" */
    {"intf_det_stat_log_do",    MAC_ALG_CFG_INTF_DET_STAT_LOG_START},   /* ????????????:sh hipriv.sh "vap0 alg_intf_det_log intf_det_stat_log_do [val]" */
    {"intf_det_stat_log_out",   MAC_ALG_CFG_INTF_DET_STAT_LOG_WRITE},   /* ????????????:sh hipriv.sh "vap0 alg_intf_det_log intf_det_stat_log_out" */

    {"edca_opt_en_ap",          MAC_ALG_CFG_EDCA_OPT_AP_EN_MODE},       /* ap??????edca????????????: sh hipriv.sh "vap0 alg_cfg edca_opt_en_ap 0|1|2" */
    {"edca_opt_en_sta",         MAC_ALG_CFG_EDCA_OPT_STA_EN},           /* sta??????edca????????????: sh hipriv.sh "vap0 alg_cfg edca_opt_en_sta 0|1" */
    {"txop_limit_en_sta",       MAC_ALG_CFG_TXOP_LIMIT_STA_EN},         /* sta??????edca txop limit????????????: sh hipriv.sh "vap0 alg_cfg txop_limit_en_sta 0|1" */
    {"edca_opt_sta_weight",     MAC_ALG_CFG_EDCA_OPT_STA_WEIGHT},       /* sta??????edca??????weighting????: sh hipriv.sh "vap0 alg_cfg edca_opt_sta_weight 0~3"*/
    {"edca_opt_debug_mode",     MAC_ALG_CFG_EDCA_OPT_DEBUG_MODE},       /* ???????????????????????????????????? */
    {"edca_opt_one_tcp_opt",    MAC_ALG_CFG_EDCA_ONE_BE_TCP_OPT},       /* ??????????BE TCP??????????????: sh hipriv.sh "vap0 alg_cfg edca_opt_one_tcp_opt 0|1" */
    {"edca_opt_one_tcp_dbg",    MAC_ALG_CFG_EDCA_ONE_BE_TCP_DBG},       /* ??????????BE TCP??????????????: sh hipriv.sh "vap0 alg_cfg edca_opt_one_tcp_dbg 0|1" */
    {"edca_opt_one_tcp_th_no_intf",  MAC_ALG_CFG_EDCA_ONE_BE_TCP_TH_NO_INTF},    /* ??????????BE TCP??????EDCA??????????????????: sh hipriv.sh "vap0 alg_cfg edca_opt_one_tcp_th_no_intf [val]" */
    {"edca_opt_one_tcp_th_intf",     MAC_ALG_CFG_EDCA_ONE_BE_TCP_TH_INTF},       /* ??????????BE TCP??????EDCA????????????????: sh hipriv.sh "vap0 alg_cfg edca_opt_one_tcp_th_intf [val]" */

    {"cca_opt_alg_en_mode",         MAC_ALG_CFG_CCA_OPT_ALG_EN_MODE},           /* CCA????????????: sh hipriv.sh "vap0 alg_cfg cca_opt_alg_en_mode 0|1" */
    {"cca_opt_debug_mode",          MAC_ALG_CFG_CCA_OPT_DEBUG_MODE},            /* CCA????DEBUG????????: sh hipriv.sh "vap0 alg_cfg cca_opt_debug_mode 0|1" */
    {"cca_opt_set_cca_th_debug",    MAC_ALG_CFG_CCA_OPT_SET_CCA_TH_DEBUG},      /* CCA??????????????????(ms):sh hipriv.sh "vap0 alg_cfg cca_opt_set_sync_err_th [time]" */
    {"cca_opt_log",                 MAC_ALG_CFG_CCA_OPT_LOG},                   /* CCA log???? sh hipriv.sh "vap0 alg_cfg cca_opt_log 0|1"*/

    {"tpc_mode",                MAC_ALG_CFG_TPC_MODE},                          /* ????TPC???????? */
    {"tpc_dbg",                 MAC_ALG_CFG_TPC_DEBUG},                         /* ????TPC??debug???? */
    {"tpc_log",                 MAC_ALG_CFG_TPC_LOG},                           /* ????TPC??log????:sh hipriv.sh "vap0 alg_cfg tpc_log 1 */
    {"tpc_stat_log_do",         MAC_ALG_CFG_TPC_STAT_LOG_START},                /* ????????????????:sh hipriv.sh "vap0 alg_tpc_log tpc_stat_log_do [mac????] [????????] [??????]" ??: sh hipriv.sh "vap0 alg_tpc_log tpc_stat_log_do 06:31:04:E3:81:02 1 1000" */
    {"tpc_stat_log_out",        MAC_ALG_CFG_TPC_STAT_LOG_WRITE},                /* ????????????????:sh hipriv.sh "vap0 alg_tpc_log tpc_stat_log_out 06:31:04:E3:81:02" */
    {"tpc_pkt_log_do",          MAC_ALG_CFG_TPC_PER_PKT_LOG_START},             /* ????????????????:sh hipriv.sh "vap0 alg_tpc_log tpc_pkt_log_do [mac????] [????????] [??????]" ??: sh hipriv.sh "vap0 alg_tpc_log tpc_pkt_log_do 06:31:04:E3:81:02 1 1000" */
    {"tpc_get_frame_pow",       MAC_ALG_CFG_TPC_GET_FRAME_POW},                 /* ??????????????:sh hipriv.sh "vap0 alg_tpc_log tpc_get_frame_pow beacon_pow" */
    {"tpc_reset_stat",          MAC_ALG_CFG_TPC_RESET_STAT},                    /*????????????*/
    {"tpc_reset_pkt",           MAC_ALG_CFG_TPC_RESET_PKT},                     /*????????????*/
    {"tpc_over_temp_th",        MAC_ALG_CFG_TPC_OVER_TMP_TH},                   /* TPC???????? */
    {"tpc_dpd_enable_rate",     MAC_ALG_CFG_TPC_DPD_ENABLE_RATE},               /* ????DPD??????????INDEX */
    {"tpc_target_rate_11b",     MAC_ALG_CFG_TPC_TARGET_RATE_11B},               /* 11b???????????? */
    {"tpc_target_rate_11ag",    MAC_ALG_CFG_TPC_TARGET_RATE_11AG},              /* 11ag???????????? */
    {"tpc_target_rate_11n20",   MAC_ALG_CFG_TPC_TARGET_RATE_HT40},              /* 11n20???????????? */
    {"tpc_target_rate_11n40",   MAC_ALG_CFG_TPC_TARGET_RATE_HT40},              /* 11n40???????????? */
    {"tpc_target_rate_11ac20",  MAC_ALG_CFG_TPC_TARGET_RATE_VHT20},             /* 11ac20???????????? */
    {"tpc_target_rate_11ac40",  MAC_ALG_CFG_TPC_TARGET_RATE_VHT40},             /* 11ac40???????????? */
    {"tpc_target_rate_11ac80",  MAC_ALG_CFG_TPC_TARGET_RATE_VHT80},             /* 11ac80???????????? */

#ifdef _PRE_WLAN_FEATURE_MU_TRAFFIC_CTL
    {"traffic_ctl_enable",      MAC_ALG_CFG_TRAFFIC_CTL_ENABLE},
    {"traffic_ctl_timeout",     MAC_ALG_CFG_TRAFFIC_CTL_TIMEOUT},
    {"traffic_ctl_min_thres",   MAC_ALG_CFG_TRAFFIC_CTL_MIN_THRESHOLD},
    {"traffic_ctl_log_debug",   MAC_ALG_CFG_TRAFFIC_CTL_LOG_DEBUG},
    {"traffic_ctl_buf_thres",   MAC_ALG_CFG_TRAFFIC_CTL_BUF_THRESHOLD},
    {"traffic_ctl_buf_adj_enb", MAC_ALG_CFG_TRAFFIC_CTL_BUF_ADJ_ENABLE},
    {"traffic_ctl_buf_adj_num", MAC_ALG_CFG_TRAFFIC_CTL_BUF_ADJ_NUM},
    {"traffic_ctl_buf_adj_cyc", MAC_ALG_CFG_TRAFFIC_CTL_BUF_ADJ_CYCLE},
    {"traffic_ctl_rx_rst_enb",  MAC_ALG_CFG_TRAFFIC_CTL_RX_RESTORE_ENABLE},
    {"traffic_ctl_rx_rst_num",  MAC_ALG_CFG_TRAFFIC_RX_RESTORE_NUM},
    {"traffic_ctl_rx_rst_thres",  MAC_ALG_CFG_TRAFFIC_RX_RESTORE_THRESHOLD},

    {"rx_dscr_ctl_enable",      MAC_ALG_CFG_RX_DSCR_CTL_ENABLE},
    {"rx_dscr_ctl_log_debug",   MAC_ALG_CFG_RX_DSCR_CTL_LOG_DEBUG},
#endif


#ifdef _PRE_WLAN_FEATURE_MWO_DET
    {"mwo_det_enable",          MAC_ALG_CFG_MWO_DET_ENABLE},                    /* ??????????????????*/
    {"mwo_det_end_rssi_th",     MAC_ALG_CFG_MWO_DET_END_RSSI_TH},               /* ????????????????????????????????????????????????dBm??*/
    {"mwo_det_start_rssi_th",   MAC_ALG_CFG_MWO_DET_START_RSSI_TH},             /* ????????????????????????????????????????????????dBm??*/
    {"mwo_det_debug",           MAC_ALG_CFG_MWO_DET_DEBUG},                     /*????????????anti_intf_1thr,c2 ??????????????????????????c2????????????????????????*/
#endif

    {OAL_PTR_NULL}
};

OAL_CONST wal_ioctl_tlv_stru   g_ast_set_tlv_table[] =
{
    /* cmd: wlan0 set_tlv xx xx */
    {"tx_pkts_stat",    WLAN_CFGID_SET_DEVICE_PKT_STAT},
    {"mem_free_stat",   WLAN_CFGID_SET_DEVICE_MEM_FREE_STAT},
#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
    {"auto_freq",       WLAN_CFGID_SET_DEVICE_FREQ},
#endif

    {"set_adc_dac_freq",  WLAN_CFGID_SET_ADC_DAC_FREQ},             /* ????ADC DAC???? */
    {"set_mac_freq",     WLAN_CFGID_SET_MAC_FREQ},             /* ??MAC???? */


    {"rx_ampdu_num",  WLAN_CFGID_SET_ADDBA_RSP_BUFFER},

    /* cmd: wlan0 set_val xx */
    {"tx_ampdu_type",   WLAN_CFGID_SET_TX_AMPDU_TYPE},   /* ??????????????????*/
    {"tx_ampdu_amsdu",  WLAN_CFGID_AMSDU_AMPDU_SWITCH},  /* ????tx amsdu ampdu??????????????????*/
    {"rx_ampdu_amsdu",  WLAN_CFGID_SET_RX_AMPDU_AMSDU},  /* ????rx ampdu amsdu ??????????????????*/
#ifdef  _PRE_WLAN_FEATURE_11AX
    {"set_addba_rsp_extend",  WLAN_CFGID_SET_ADDBA_RSP_EXTEND},
#endif
    {"sk_pacing_shift", WLAN_CFGID_SET_SK_PACING_SHIFT},

    {"trx_stat_log_en", WLAN_CFGID_SET_TRX_STAT_LOG}, /* ????tcp ack???????????????????????????????????????? */
    {"mimo_blacklist",  WLAN_CFGID_MIMO_BLACKLIST},   /* ????????MIMO?????????????? */
#ifdef _PRE_WLAN_FEATURE_DFS_ENABLE
    /* cmd: wlan0 set_val dfs_debug 0|1 */
    {"dfs_debug",  WLAN_CFGID_SET_DFS_MODE},             /* ????dfs??????????????????????*/
#endif

#ifdef _PRE_WLAN_FEATURE_HIMIT
    {"himit_debug", WLAN_CFGID_HIMIT_SWITCH},           /* ????himit???? 0 | 1 */
#endif

    {OAL_PTR_NULL}
};


#ifdef _PRE_WLAN_FEATURE_11AX
OAL_CONST wal_ioctl_tlv_stru   g_ast_11ax_debug_table[] =
{
    /* cmd: wlan0 xxx  2  xxx 1 xxx 0 */
    {"print_log",                    MAC_VAP_11AX_DEBUG_PRINT_LOG},
    {"tid",                          MAC_VAP_11AX_DEBUG_HE_TB_PPDU_TID_NUM},
    {"htc_order",                    MAC_VAP_11AX_DEBUG_HE_TB_PPDU_HTC_ORGER},
    {"htc_val",                      MAC_VAP_11AX_DEBUG_HE_TB_PPDU_HTC_VALUE},
    {"fix_power",                    MAC_VAP_11AX_DEBUG_HE_TB_PPDU_FIX_POWER},
    {"power_val",                    MAC_VAP_11AX_DEBUG_HE_TB_PPDU_POWER_VALUE},
    {"disable_ba_check",             MAC_VAP_11AX_DEBUG_HE_TB_PPDU_DISABLE_BA_CHECK},
    {"disable_mu_edca",              MAC_VAP_11AX_DEBUG_DISABLE_MU_EDCA},
    {"manual_cfo",                   MAC_VAP_11AX_DEBUG_MANUAL_CFO},
    {"bsrp",                         MAC_VAP_11AX_DEBUG_BSRP_CFG},
    {"bsrp_tid",                     MAC_VAP_11AX_DEBUG_BSRP_TID},
    {"bsrp_size",                    MAC_VAP_11AX_DEBUG_BSRP_QUEUE_SIZE},
    {"mpad_dur",                     MAC_VAP_11AX_DEBUG_MAC_PADDING},
    {"tb_pwr_test",                  MAC_VAP_11AX_DEBUG_POW_TEST},
    {"tb_ppdu_len",                  MAC_VAP_11AX_DEBUG_TB_PPDU_LEN},
    {"tb_ppdu_ac",                   MAC_VAP_11AX_DEBUG_TB_PPDU_AC},
    {"print_rx_trig",                MAC_VAP_11AX_DEBUG_PRINT_RX_TRIG_FRAME},

    {OAL_PTR_NULL}
};
#endif

/* MAC TX RX common info report???????? */
OAL_CONST wal_ioctl_tlv_stru   g_ast_mac_tx_report_debug_table[] =
{
    /* cmd: sh hipriv.sh "wlan0 set_str mac_report_tx 9(????????????) index 0 status 0 bw 0 protocol 8 fr_type 3 sub_type 0 ampdu 1 psdu 1 hw_retry 1"
       ????:tx????index0????status????????????index??????
       ????index3??????????????????:sh hipriv.sh "wlan0 set_str mac_report_tx 2 index 3 clear 0" */
    {"index",                        MAC_TX_COMMON_REPORT_INDEX},          /* tx rx??????8?? index:0-7*/
    {"status",                       MAC_TX_COMMON_REPORT_STATUS},         /* 0?????????? 1???????????? 2???????????? 3?????????????? */
    {"bw",                           MAC_TX_COMMON_REPORT_BW_MODE},        /* bandwidth 0:20M 1:40M 2:80M 3:160M */
    /* 0000: The data rate is 11b type                    0001: The data rate is legacy OFDM type
       0010: The data rate is HT Mixed mode Frame type    0011: The data rate is HT Green Field Frame type
       0100: The data rate is VHT type                    0101~0111??reserved
       1000??The data rate is HE  SU Format type          1001??The data rate is HE  MU Format type
       1010??The data rate is HE  EXT SU Format type      1011??The data rate is HE  TRIG Format type
       1100~1111:reserved */
    {"protocol",                     MAC_TX_COMMON_REPORT_PROTOCOL_MODE},  /* ???????? */
    {"fr_type",                      MAC_TX_COMMON_REPORT_FRAME_TYPE},     /* ?????? */
    {"sub_type",                     MAC_TX_COMMON_REPORT_SUB_TYPE},       /* ?????? */
    {"ampdu",                        MAC_TX_COMMON_REPORT_APMDU},          /* 0????ampdu?????? 1??ampdu?????? */
    {"psdu",                         MAC_TX_COMMON_REPORT_PSDU},           /* ??psdu??????????mpdu??????0????mpdu???? 1????psdu????????psdu????????status??????fcs??????fcs????????psdu????*/
    {"hw_retry",                     MAC_TX_COMMON_REPORT_HW_RETRY},       /* 0????hw retry???????? 1??hw retry???????? */
    {"clear",                        MAC_TX_COMMON_REPORT_CTRL_REG_CLEAR}, /* ????????index???????????? */

    {OAL_PTR_NULL}
};

OAL_CONST wal_ioctl_tlv_stru   g_ast_mac_rx_report_debug_table[] =
{
    /* cmd: sh hipriv.sh "wlan0 set_str mac_report_rx 11(????????????) index 5 status 1 bw 2 protocol 1 fr_type 2 sub_type 4 ampdu 1 psdu 1 vap_mode 3 bss 1
                          direct 1"
       ????:??????????????????????????????????????????????????????????????????
       clear????: sh hipriv.sh "wlan0 set_str mac_report_rx index 5 clear 0"*/
    {"index",                        MAC_RX_COMMON_REPORT_INDEX},          /* tx rx??????8?? index:0-7*/
    /* 0??Invalid  1??RX successful
       2??Duplicate detected 3??FCS check failed
       4??Key search failed 5??MIC check failed
       6??ICV failed        others??Reserved */
    {"status",                       MAC_RX_COMMON_REPORT_STATUS},         /* ???????? */
    {"bw",                           MAC_RX_COMMON_REPORT_BW_MODE},        /* bandwidth 0:20M 1:40M 2:80M 3:160M */
    /* 0000: The data rate is 11b type                    0001: The data rate is legacy OFDM type
       0010: The data rate is HT Mixed mode Frame type    0011: The data rate is HT Green Field Frame type
       0100: The data rate is VHT type                    0101~0111??reserved
       1000??The data rate is HE  SU Format type          1001??The data rate is HE  MU Format type
       1010??The data rate is HE  EXT SU Format type      1011??The data rate is HE  TRIG Format type
       1100~1111:reserved */
    {"protocol",                     MAC_RX_COMMON_REPORT_PROTOCOL_MODE},  /* ???????? */
    {"fr_type",                      MAC_RX_COMMON_REPORT_FRAME_TYPE},     /* ?????? */
    {"sub_type",                     MAC_RX_COMMON_REPORT_SUB_TYPE},       /* ?????? */
    {"ampdu",                        MAC_RX_COMMON_REPORT_APMDU},          /* 0????ampdu?????? 1??ampdu?????? */
    {"psdu",                         MAC_RX_COMMON_REPORT_PSDU},           /* ??psdu??????????mpdu??????0????mpdu???? 1????psdu????????psdu????????status??????fcs??????fcs????????psdu????*/
    {"vap_mode",                     MAC_RX_COMMON_REPORT_VAP_CHK},        /* ??????vap??????????????0~4????????????vap0~4???????? 5~6??reserved 7????????vap*/
    {"bss",                          MAC_RX_COMMON_REPORT_BSS_CHK},        /* 0??????bss?????? 1????bss??????*/
    {"direct",                       MAC_RX_COMMON_REPORT_DIRECT_CHK},     /* 0????direct???????? 1??direct???????? */
    {"clear",                        MAC_RX_COMMON_REPORT_CTRL_REG_CLEAR}, /* ????????index???????????? */

    {OAL_PTR_NULL}
};

OAL_CONST wal_ioctl_tlv_stru g_ast_common_debug_table[] =
{
    {"pg_switch",                    PG_EFFICIENCY_STATISTICS_ENABLE},
    {"pg_info",                      PG_EFFICIENCY_INFO},

    {"hw_txq",                    MAC_VAP_COMMON_SET_TXQ},
    {"tx_info",                   MAC_VAP_COMMON_TX_INFO},

    {OAL_PTR_NULL}
};


/* cmd: wlan0 xxx  2  xxx 1 xxx 0 */
OAL_CONST wal_ioctl_str_stru   g_ast_set_str_table[] =
{
#ifdef _PRE_WLAN_FEATURE_11AX
    {"11ax_debug",      WLAN_CFGID_11AX_DEBUG,           (wal_ioctl_tlv_stru*)&g_ast_11ax_debug_table},
#endif

    {"mac_report_tx",   WLAN_CFGID_MAC_TX_COMMON_REPORT, (wal_ioctl_tlv_stru*)&g_ast_mac_tx_report_debug_table},
    {"mac_report_rx",   WLAN_CFGID_MAC_RX_COMMON_REPORT, (wal_ioctl_tlv_stru*)&g_ast_mac_rx_report_debug_table},
    {"common_debug",    WLAN_CFGID_COMMON_DEBUG,         (wal_ioctl_tlv_stru*)&g_ast_common_debug_table},


    {OAL_PTR_NULL}
};


OAL_CONST wal_dfs_domain_entry_stru g_ast_dfs_domain_table_etc[] =
{
    {"AE", MAC_DFS_DOMAIN_ETSI},
    {"AL", MAC_DFS_DOMAIN_NULL},
    {"AM", MAC_DFS_DOMAIN_ETSI},
    {"AN", MAC_DFS_DOMAIN_ETSI},
    {"AR", MAC_DFS_DOMAIN_FCC},
    {"AT", MAC_DFS_DOMAIN_ETSI},
    {"AU", MAC_DFS_DOMAIN_FCC},
    {"AZ", MAC_DFS_DOMAIN_ETSI},
    {"BA", MAC_DFS_DOMAIN_ETSI},
    {"BE", MAC_DFS_DOMAIN_ETSI},
    {"BG", MAC_DFS_DOMAIN_ETSI},
    {"BH", MAC_DFS_DOMAIN_ETSI},
    {"BL", MAC_DFS_DOMAIN_NULL},
    {"BN", MAC_DFS_DOMAIN_ETSI},
    {"BO", MAC_DFS_DOMAIN_ETSI},
    {"BR", MAC_DFS_DOMAIN_FCC},
    {"BY", MAC_DFS_DOMAIN_ETSI},
    {"BZ", MAC_DFS_DOMAIN_ETSI},
    {"CA", MAC_DFS_DOMAIN_FCC},
    {"CH", MAC_DFS_DOMAIN_ETSI},
    {"CL", MAC_DFS_DOMAIN_NULL},
    {"CN", MAC_DFS_DOMAIN_CN},
    {"CO", MAC_DFS_DOMAIN_FCC},
    {"CR", MAC_DFS_DOMAIN_FCC},
    {"CS", MAC_DFS_DOMAIN_ETSI},
    {"CY", MAC_DFS_DOMAIN_ETSI},
    {"CZ", MAC_DFS_DOMAIN_ETSI},
    {"DE", MAC_DFS_DOMAIN_ETSI},
    {"DK", MAC_DFS_DOMAIN_ETSI},
    {"DO", MAC_DFS_DOMAIN_FCC},
    {"DZ", MAC_DFS_DOMAIN_NULL},
    {"EC", MAC_DFS_DOMAIN_FCC},
    {"EE", MAC_DFS_DOMAIN_ETSI},
    {"EG", MAC_DFS_DOMAIN_ETSI},
    {"ES", MAC_DFS_DOMAIN_ETSI},
    {"FI", MAC_DFS_DOMAIN_ETSI},
    {"FR", MAC_DFS_DOMAIN_ETSI},
    {"GB", MAC_DFS_DOMAIN_ETSI},
    {"GE", MAC_DFS_DOMAIN_ETSI},
    {"GR", MAC_DFS_DOMAIN_ETSI},
    {"GT", MAC_DFS_DOMAIN_FCC},
    {"HK", MAC_DFS_DOMAIN_FCC},
    {"HN", MAC_DFS_DOMAIN_FCC},
    {"HR", MAC_DFS_DOMAIN_ETSI},
    {"HU", MAC_DFS_DOMAIN_ETSI},
    {"ID", MAC_DFS_DOMAIN_NULL},
    {"IE", MAC_DFS_DOMAIN_ETSI},
    {"IL", MAC_DFS_DOMAIN_ETSI},
    {"IN", MAC_DFS_DOMAIN_NULL},
    {"IQ", MAC_DFS_DOMAIN_NULL},
    {"IR", MAC_DFS_DOMAIN_NULL},
    {"IS", MAC_DFS_DOMAIN_ETSI},
    {"IT", MAC_DFS_DOMAIN_ETSI},
    {"JM", MAC_DFS_DOMAIN_FCC},
    {"JO", MAC_DFS_DOMAIN_ETSI},
    {"JP", MAC_DFS_DOMAIN_MKK},
    {"KP", MAC_DFS_DOMAIN_NULL},
    {"KR", MAC_DFS_DOMAIN_KOREA},
    {"KW", MAC_DFS_DOMAIN_ETSI},
    {"KZ", MAC_DFS_DOMAIN_NULL},
    {"LB", MAC_DFS_DOMAIN_NULL},
    {"LI", MAC_DFS_DOMAIN_ETSI},
    {"LK", MAC_DFS_DOMAIN_FCC},
    {"LT", MAC_DFS_DOMAIN_ETSI},
    {"LU", MAC_DFS_DOMAIN_ETSI},
    {"LV", MAC_DFS_DOMAIN_ETSI},
    {"MA", MAC_DFS_DOMAIN_NULL},
    {"MC", MAC_DFS_DOMAIN_ETSI},
    {"MK", MAC_DFS_DOMAIN_ETSI},
    {"MO", MAC_DFS_DOMAIN_FCC},
    {"MT", MAC_DFS_DOMAIN_ETSI},
    {"MX", MAC_DFS_DOMAIN_FCC},
    {"MY", MAC_DFS_DOMAIN_FCC},
    {"NG", MAC_DFS_DOMAIN_NULL},
    {"NL", MAC_DFS_DOMAIN_ETSI},
    {"NO", MAC_DFS_DOMAIN_ETSI},
    {"NP", MAC_DFS_DOMAIN_NULL},
    {"NZ", MAC_DFS_DOMAIN_FCC},
    {"OM", MAC_DFS_DOMAIN_FCC},
    {"PA", MAC_DFS_DOMAIN_FCC},
    {"PE", MAC_DFS_DOMAIN_FCC},
    {"PG", MAC_DFS_DOMAIN_FCC},
    {"PH", MAC_DFS_DOMAIN_FCC},
    {"PK", MAC_DFS_DOMAIN_NULL},
    {"PL", MAC_DFS_DOMAIN_ETSI},
    {"PR", MAC_DFS_DOMAIN_FCC},
    {"PT", MAC_DFS_DOMAIN_ETSI},
    {"QA", MAC_DFS_DOMAIN_NULL},
    {"RO", MAC_DFS_DOMAIN_ETSI},
    {"RU", MAC_DFS_DOMAIN_FCC},
    {"SA", MAC_DFS_DOMAIN_FCC},
    {"SE", MAC_DFS_DOMAIN_ETSI},
    {"SG", MAC_DFS_DOMAIN_NULL},
    {"SI", MAC_DFS_DOMAIN_ETSI},
    {"SK", MAC_DFS_DOMAIN_ETSI},
    {"SV", MAC_DFS_DOMAIN_FCC},
    {"SY", MAC_DFS_DOMAIN_NULL},
    {"TH", MAC_DFS_DOMAIN_FCC},
    {"TN", MAC_DFS_DOMAIN_ETSI},
    {"TR", MAC_DFS_DOMAIN_ETSI},
    {"TT", MAC_DFS_DOMAIN_FCC},
    {"TW", MAC_DFS_DOMAIN_NULL},
    {"UA", MAC_DFS_DOMAIN_NULL},
    {"US", MAC_DFS_DOMAIN_FCC},
    {"UY", MAC_DFS_DOMAIN_FCC},
    {"UZ", MAC_DFS_DOMAIN_FCC},
    {"VE", MAC_DFS_DOMAIN_FCC},
    {"VN", MAC_DFS_DOMAIN_ETSI},
    {"YE", MAC_DFS_DOMAIN_NULL},
    {"ZA", MAC_DFS_DOMAIN_FCC},
    {"ZW", MAC_DFS_DOMAIN_NULL},
};


/*****************************************************************************
  3 ????????
*****************************************************************************/

oal_uint32  wal_get_cmd_one_arg_etc(oal_int8 *pc_cmd, oal_int8 *pc_arg, oal_uint32 ul_arg_len, oal_uint32 *pul_cmd_offset)
{
    oal_int8   *pc_cmd_copy = OAL_PTR_NULL;
    oal_uint32  ul_pos = 0;

    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR3(pc_cmd,pc_arg,pul_cmd_offset)))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "{wal_get_cmd_one_arg_etc::pc_cmd/pc_arg/pul_cmd_offset null ptr error %x, %x, %x!}\r\n", (uintptr_t)pc_cmd, (uintptr_t)pc_arg, (uintptr_t)pul_cmd_offset);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pc_cmd_copy = pc_cmd;

    /* ???????????????????? */
    while (' ' == *pc_cmd_copy)
    {
        ++pc_cmd_copy;
    }

    while ((' ' != *pc_cmd_copy) && ('\0' != *pc_cmd_copy))
    {
        pc_arg[ul_pos] = *pc_cmd_copy;
        ++ul_pos;
        ++pc_cmd_copy;

        if (OAL_UNLIKELY(ul_pos >= ul_arg_len))
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_get_cmd_one_arg_etc::ul_pos >= WAL_HIPRIV_CMD_NAME_MAX_LEN, ul_pos %d!}\r\n", ul_pos);
            return OAL_ERR_CODE_ARRAY_OVERFLOW;
        }
    }

    pc_arg[ul_pos]  = '\0';

    /* ????????????????????????(????????????) */
    if (0 == ul_pos)
    {
        OAM_INFO_LOG0(0, OAM_SF_ANY, "{wal_get_cmd_one_arg_etc::return param pc_arg is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    *pul_cmd_offset = (oal_uint32)(pc_cmd_copy - pc_cmd);

    return OAL_SUCC;
}


oal_uint8 *wal_get_reduce_sar_ctrl_params(oal_uint8 uc_tx_power_lvl)
{
#if defined(_PRE_WLAN_FEATURE_TPC_OPT) && defined(_PRE_PLAT_FEATURE_CUSTOMIZE)
    wlan_init_cust_nvram_params  *pst_cust_nv_params = hwifi_get_init_nvram_params_etc();  /* ?????????????????????? */
    if ((uc_tx_power_lvl <= CUS_NUM_OF_SAR_LVL) && (uc_tx_power_lvl > 0))
    {
        uc_tx_power_lvl--;
    }
    else
    {
        return OAL_PTR_NULL;
    }

    return (oal_uint8 *)pst_cust_nv_params->st_sar_ctrl_params[uc_tx_power_lvl];
#else
    return OAL_PTR_NULL;
#endif
}


oal_uint32  wal_hipriv_set_fix_rate_pre_config(oal_net_device_stru *pst_net_dev, oal_bool_enum_uint8 en_fix_rate_enable, mac_cfg_set_dscr_param_stru *pc_stu)
{
    mac_vap_stru                   *pst_mac_vap = OAL_PTR_NULL;
    oal_uint32                      ul_ret;
    oal_uint8                       uc_ampdu_cfg_idx;
    oal_int8                        ac_sw_ampdu_cmd[WAL_AMPDU_CFG_BUTT][WAL_HIPRIV_CMD_NAME_MAX_LEN] = {{"0"},{"1"}};

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        OAM_ERROR_LOG0(0 , OAM_SF_ANY, "{wal_hipriv_set_fix_rate_pre_config::OAL_NET_DEV_PRIV(pst_net_dev) is null ptr.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ????????????????????????autorate????????????????  */
    if(OAL_FALSE == en_fix_rate_enable)
    {
        /*  ????autorate????  */
        ul_ret = wal_hipriv_alg_cfg_etc(pst_net_dev, "ar_enable 1");
        if(OAL_SUCC != ul_ret)
        {
            OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_hipriv_set_fix_rate_pre_config:: autorate enable command config failed!err[d%]}\n", ul_ret);
            return OAL_FAIL;
        }

        if(OAL_TRUE == pst_mac_vap->st_fix_rate_pre_para.en_rate_cfg_tag)
        {
            /*  ????ampdu????  */
            uc_ampdu_cfg_idx = pst_mac_vap->st_fix_rate_pre_para.en_tx_ampdu_last;
            ul_ret = wal_hipriv_ampdu_tx_on(pst_net_dev, ac_sw_ampdu_cmd[uc_ampdu_cfg_idx]);
            if(OAL_SUCC != ul_ret)
            {
                OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_hipriv_set_fix_rate_pre_config:: software ampdu command config failed!err[d%]}\n", ul_ret);
                return OAL_FAIL;
            }

            /*????????????????????????????????*/
            pst_mac_vap->st_fix_rate_pre_para.en_rate_cfg_tag = OAL_FALSE;
        }

        return OAL_SUCC;
    }

    /*  ????autorate????  */
    ul_ret = wal_hipriv_alg_cfg_etc(pst_net_dev, "ar_enable 0");
    if(OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_hipriv_set_fix_rate_pre_config:: set autorate disable failed!err[d%]}\n", ul_ret);
        return OAL_FAIL;
    }

    /*11abg????????????????????????ampdu????*/
    if(WAL_DSCR_PARAM_RATE == pc_stu->uc_function_index)
    {
        if(OAL_TRUE != pst_mac_vap->st_fix_rate_pre_para.en_rate_cfg_tag)
        {
            /*  ????ampdu????????  */
            pst_mac_vap->st_fix_rate_pre_para.en_tx_ampdu_last = mac_mib_get_CfgAmpduTxAtive(pst_mac_vap);
            pst_mac_vap->st_fix_rate_pre_para.en_rate_cfg_tag = OAL_TRUE;     /*????????????????????????????????*/
        }

        /*  ????ampdu????  */
        ul_ret = wal_hipriv_ampdu_tx_on(pst_net_dev, ac_sw_ampdu_cmd[WAL_AMPDU_DISABLE]);
        if(OAL_SUCC != ul_ret)
        {
            OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_hipriv_set_fix_rate_pre_config:: disable software ampdu failed!err[d%]}\n", ul_ret);
            return OAL_FAIL;
        }
    }

    return OAL_SUCC;

}


oal_uint32  wal_hipriv_process_rate_params(oal_net_device_stru *pst_net_dev, oal_int8 *pc_cmd, mac_cfg_set_dscr_param_stru *pc_stu)
{
    oal_uint32                       ul_ret;
    oal_uint32                       ul_off_set;
    oal_int32                        l_val;
    wal_dscr_param_enum_uint8        en_param_index;
    mac_cfg_non_ht_rate_stru* pst_set_non_ht_rate_param = OAL_PTR_NULL;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];

    ul_ret = wal_get_cmd_one_arg_etc(pc_cmd, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /*  ??????????????255?????????????? */
    l_val = oal_strtol(ac_arg, OAL_PTR_NULL, 0);
    if(WAL_IOCTL_IS_INVALID_FIXED_RATE(l_val,pc_stu))
    {
         ul_ret = wal_hipriv_set_fix_rate_pre_config(pst_net_dev, OAL_FALSE, pc_stu);
         if(OAL_SUCC != ul_ret)
         {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params: config invalid rate handle failed!err[%d]}\n", ul_ret);
            return OAL_FAIL;
         }
         return OAL_ERR_CODE_INVALID_CONFIG;
    }

    switch(pc_stu->uc_function_index)
    {
        case WAL_DSCR_PARAM_RATE:
            pst_set_non_ht_rate_param = (mac_cfg_non_ht_rate_stru*)(&(pc_stu->l_value));
            /* ????????*/
            for (en_param_index = 0; en_param_index < WLAN_LEGACY_RATE_VALUE_BUTT; en_param_index++)
            {
                if(!oal_strcmp(pauc_non_ht_rate_tbl[en_param_index], ac_arg))
                {
                    pst_set_non_ht_rate_param->en_rate = en_param_index;
                    break;
                }
            }

            /* ????????????TX?????????????????? ; ????05????03 ram ????dmac ???????????????? */
            if (en_param_index <= WLAN_SHORT_11b_11_M_BPS)
            {
                pst_set_non_ht_rate_param->en_protocol_mode = WLAN_11B_PHY_PROTOCOL_MODE;
            }
            else if (en_param_index >= WLAN_LEGACY_OFDM_48M_BPS && en_param_index <= WLAN_LEGACY_OFDM_9M_BPS)
            {
                pst_set_non_ht_rate_param->en_protocol_mode = WLAN_LEGACY_OFDM_PHY_PROTOCOL_MODE;
            }
            else
            {
                OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params::invalid rate!}\r\n");
                return OAL_ERR_CODE_INVALID_CONFIG;
            }
            ul_ret = wal_hipriv_set_fix_rate_pre_config(pst_net_dev, OAL_TRUE, pc_stu);
            if(OAL_SUCC != ul_ret)
            {
               OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params: config fixed legacy rate handle failed!err[%d]}\n", ul_ret);
               return OAL_FAIL;
            }
            break;

        case WAL_DSCR_PARAM_MCS:
            l_val = oal_strtol(ac_arg, OAL_PTR_NULL, 0);
            if (l_val < WAL_HIPRIV_HT_MCS_MIN || l_val > WAL_HIPRIV_HT_MCS_MAX)
            {
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params::input mcs val out of range [%d]!}\r\n", l_val);
                return OAL_ERR_CODE_INVALID_CONFIG;
            }
            pc_stu->l_value = l_val;
            ul_ret = wal_hipriv_set_fix_rate_pre_config(pst_net_dev, OAL_TRUE, pc_stu);
            if(OAL_SUCC != ul_ret)
            {
               OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params: config fixed mcs rate handle failed!err[%d]}\n", ul_ret);
               return OAL_FAIL;
            }
            break;

        case WAL_DSCR_PARAM_MCSAC:
            l_val = oal_strtol(ac_arg, OAL_PTR_NULL, 0);
            if (l_val < WAL_HIPRIV_VHT_MCS_MIN || l_val > WAL_HIPRIV_VHT_MCS_MAX)
            {
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params::input mcsac val out of range [%d]!}\r\n", l_val);
                return OAL_ERR_CODE_INVALID_CONFIG;
            }
            pc_stu->l_value = l_val;

            ul_ret = wal_hipriv_set_fix_rate_pre_config(pst_net_dev, OAL_TRUE, pc_stu);
            if(OAL_SUCC != ul_ret)
            {
               OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params: config fixed mcsac rate handle failed!err[%d]}\n", ul_ret);
               return OAL_FAIL;
            }

            break;
        case WAL_DSCR_PARAM_MCSAX:
            l_val = oal_strtol(ac_arg, OAL_PTR_NULL, 0);
            if (l_val < WAL_HIPRIV_HE_MCS_MIN || l_val > WAL_HIPRIV_HE_MCS_MAX)
            {
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params::input mcsax val out of range [%d]!}\r\n", l_val);
                return OAL_ERR_CODE_INVALID_CONFIG;
            }
            pc_stu->l_value = l_val;

            ul_ret = wal_hipriv_set_fix_rate_pre_config(pst_net_dev, OAL_TRUE, pc_stu);
            if(OAL_SUCC != ul_ret)
            {
               OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params: config fixed mcsax rate handle failed!err[%d]}\n", ul_ret);
               return OAL_FAIL;
            }

            break;

        case WAL_DSCR_PARAM_NSS:
            for (en_param_index = 0; en_param_index < WLAN_NSS_LIMIT; en_param_index++)
            {
                if(!oal_strcmp(pauc_tx_dscr_nss_tbl[en_param_index], ac_arg))
                {
                    pc_stu->l_value = en_param_index;
                    break;
                }
            }
            if (WAL_DSCR_PARAM_BUTT == en_param_index)
            {
                OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params::invalid param for nss!}\r\n");
                return OAL_ERR_CODE_INVALID_CONFIG;
            }
            break;

        case WAL_DSCR_PARAM_BW:
            for (en_param_index = 0; en_param_index < WLAN_BANDWITH_CAP_BUTT; en_param_index++)
            {
                if(!oal_strcmp(pauc_bw_tbl[en_param_index], ac_arg))
                {
                    pc_stu->l_value = en_param_index;
                    break;
                }
            }
            if (en_param_index >= WLAN_BANDWITH_CAP_BUTT)
            {
                OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params::invalid param for bandwidth!}\r\n");
                return OAL_ERR_CODE_INVALID_CONFIG;
            }
            break;

        default:
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_process_rate_params::invalid command!}\r\n");
            return OAL_ERR_CODE_INVALID_CONFIG;
    }
    return OAL_SUCC;
}


oal_void wal_msg_queue_init_etc(oal_void)
{
    memset_s((oal_void*)&g_wal_wid_msg_queue, OAL_SIZEOF(g_wal_wid_msg_queue), 0, OAL_SIZEOF(g_wal_wid_msg_queue));
    oal_dlist_init_head(&g_wal_wid_msg_queue.st_head);
    g_wal_wid_msg_queue.count = 0;
    oal_spin_lock_init(&g_wal_wid_msg_queue.st_lock);
    OAL_WAIT_QUEUE_INIT_HEAD(&g_wal_wid_msg_queue.st_wait_queue);
}

OAL_STATIC oal_void _wal_msg_request_add_queue_(wal_msg_request_stru* pst_msg)
{
    oal_dlist_add_tail(&pst_msg->pst_entry, &g_wal_wid_msg_queue.st_head);
    g_wal_wid_msg_queue.count++;
}


oal_uint32 wal_get_request_msg_count_etc(oal_void)
{
    return g_wal_wid_msg_queue.count;
}

oal_uint32 wal_check_and_release_msg_resp_etc(wal_msg_stru   *pst_rsp_msg)
{
    wal_msg_write_rsp_stru     *pst_write_rsp_msg = OAL_PTR_NULL;
    if(OAL_PTR_NULL != pst_rsp_msg)
    {
        oal_uint32 ul_err_code;
        wlan_cfgid_enum_uint16 en_wid;
        pst_write_rsp_msg = (wal_msg_write_rsp_stru *)(pst_rsp_msg->auc_msg_data);
        ul_err_code = pst_write_rsp_msg->ul_err_code;
        en_wid = pst_write_rsp_msg->en_wid;
        oal_free(pst_rsp_msg);

        if (OAL_SUCC != ul_err_code)
        {
            OAM_WARNING_LOG2(0, OAM_SF_SCAN, "{wal_check_and_release_msg_resp_etc::detect err code:[%u],wid:[%u]}",
                           ul_err_code, en_wid);
            return ul_err_code;
        }
    }

    return OAL_SUCC;
}


oal_void wal_msg_request_add_queue_etc(wal_msg_request_stru* pst_msg)
{
#if (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION)
    if(OAL_FALSE== g_wal_wid_queue_init_flag)
    {
        wal_msg_queue_init_etc();
        g_wal_wid_queue_init_flag = OAL_TRUE;
    }
#endif
    oal_spin_lock_bh(&g_wal_wid_msg_queue.st_lock);
    _wal_msg_request_add_queue_(pst_msg);
    oal_spin_unlock_bh(&g_wal_wid_msg_queue.st_lock);
}

OAL_STATIC oal_void _wal_msg_request_remove_queue_(wal_msg_request_stru* pst_msg)
{
    g_wal_wid_msg_queue.count--;
    oal_dlist_delete_entry(&pst_msg->pst_entry);
}


oal_void wal_msg_request_remove_queue_etc(wal_msg_request_stru* pst_msg)
{
    oal_spin_lock_bh(&g_wal_wid_msg_queue.st_lock);
    _wal_msg_request_remove_queue_(pst_msg);
    oal_spin_unlock_bh(&g_wal_wid_msg_queue.st_lock);
}


oal_int32  wal_set_msg_response_by_addr_etc(oal_ulong addr,oal_void * pst_resp_mem ,oal_uint32 ul_resp_ret,
                                                 oal_uint32 uc_rsp_len)
{
    oal_int32 l_ret = -OAL_EINVAL;
    oal_dlist_head_stru * pst_pos = OAL_PTR_NULL;
    oal_dlist_head_stru * pst_entry_temp = OAL_PTR_NULL;
    wal_msg_request_stru* pst_request = NULL;

    oal_spin_lock_bh(&g_wal_wid_msg_queue.st_lock);
    OAL_DLIST_SEARCH_FOR_EACH_SAFE(pst_pos, pst_entry_temp, (&g_wal_wid_msg_queue.st_head))
    {
        pst_request = (wal_msg_request_stru *)OAL_DLIST_GET_ENTRY(pst_pos,wal_msg_request_stru,
                                                                  pst_entry);
        if(pst_request->ul_request_address == (oal_ulong)addr)
        {
            /*address match*/
            if(OAL_UNLIKELY(NULL != pst_request->pst_resp_mem))
            {
                OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_set_msg_response_by_addr_etc::wal_set_msg_response_by_addr_etc response had been set!");
            }
            pst_request->pst_resp_mem = pst_resp_mem;
            pst_request->ul_ret = ul_resp_ret;
            pst_request->ul_resp_len = uc_rsp_len;
            l_ret = OAL_SUCC;
            break;
        }
    }
    oal_spin_unlock_bh(&g_wal_wid_msg_queue.st_lock);

    return l_ret;
}


oal_uint32  wal_alloc_cfg_event_etc
                (oal_net_device_stru     *pst_net_dev,
                 frw_event_mem_stru     **ppst_event_mem,
                 oal_void*               pst_resp_addr,
                 wal_msg_stru           **ppst_cfg_msg,
                 oal_uint16               us_len)
{
    mac_vap_stru               *pst_vap = OAL_PTR_NULL;
    frw_event_mem_stru         *pst_event_mem = OAL_PTR_NULL;
    frw_event_stru             *pst_event = OAL_PTR_NULL;
    oal_uint16                  us_resp_len = 0;

    wal_msg_rep_hdr* pst_rep_hdr = NULL;

    pst_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_vap))
    {
        /* ????wifi????????????????hipriv????????error???? */
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_alloc_cfg_event_etc::OAL_NET_DEV_PRIV(pst_net_dev) is null ptr! pst_net_dev=[%p]}", (uintptr_t)pst_net_dev);
        return OAL_ERR_CODE_PTR_NULL;
    }

    us_resp_len += OAL_SIZEOF(wal_msg_rep_hdr);

    us_len += us_resp_len;

    pst_event_mem = FRW_EVENT_ALLOC(us_len);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG2(pst_vap->uc_vap_id, OAM_SF_ANY, "{wal_alloc_cfg_event_etc::pst_event_mem null ptr error,request size:us_len:%d,resp_len:%d}",
                        us_len, us_resp_len);
        return OAL_ERR_CODE_PTR_NULL;
    }

    *ppst_event_mem = pst_event_mem;    /* ???????? */

    pst_event = frw_get_event_stru(pst_event_mem);

    /* ?????????? */
    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                       FRW_EVENT_TYPE_HOST_CRX,
                       WAL_HOST_CRX_SUBTYPE_CFG,
                       us_len,
                       FRW_EVENT_PIPELINE_STAGE_0,
                       pst_vap->uc_chip_id,
                       pst_vap->uc_device_id,
                       pst_vap->uc_vap_id);

    /*fill the resp hdr*/
    pst_rep_hdr = (wal_msg_rep_hdr*)pst_event->auc_event_data;
    if(NULL == pst_resp_addr)
    {
        /*no response*/
        pst_rep_hdr->ul_request_address = (oal_ulong)0;
    }
    else
    {
        /*need response*/
         pst_rep_hdr->ul_request_address = (oal_ulong)(uintptr_t)pst_resp_addr;
    }


    *ppst_cfg_msg = (wal_msg_stru *)((oal_uint8*)pst_event->auc_event_data + us_resp_len);  /* ???????? */

    return OAL_SUCC;
}


OAL_STATIC OAL_INLINE oal_int32 wal_request_wait_event_condition(wal_msg_request_stru *pst_msg_stru)
{
    oal_int32 l_ret = OAL_FALSE;
    oal_spin_lock_bh(&g_wal_wid_msg_queue.st_lock);
    if((NULL != pst_msg_stru->pst_resp_mem) || (OAL_SUCC != pst_msg_stru->ul_ret))
    {
        l_ret = OAL_TRUE;
    }
    oal_spin_unlock_bh(&g_wal_wid_msg_queue.st_lock);
    return l_ret;
}

oal_void wal_cfg_msg_task_sched_etc(oal_void)
{
    OAL_WAIT_QUEUE_WAKE_UP(&g_wal_wid_msg_queue.st_wait_queue);
}


oal_int32 wal_send_cfg_event_etc(oal_net_device_stru      *pst_net_dev,
                                   wal_msg_type_enum_uint8   en_msg_type,
                                   oal_uint16                us_len,
                                   oal_uint8                *puc_param,
                                   oal_bool_enum_uint8       en_need_rsp,
                                   wal_msg_stru            **ppst_rsp_msg)
{
    wal_msg_stru                *pst_cfg_msg = OAL_PTR_NULL;
    frw_event_mem_stru          *pst_event_mem = OAL_PTR_NULL;
    wal_msg_stru                *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                   ul_ret;
    oal_int32                    l_ret;
#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
    mac_vap_stru                *pst_mac_vap;
#endif

    DECLARE_WAL_MSG_REQ_STRU(st_msg_request);

    WAL_MSG_REQ_STRU_INIT(st_msg_request);

    if(NULL != ppst_rsp_msg)
    {
        *ppst_rsp_msg = NULL;
    }

    if (OAL_WARN_ON((OAL_TRUE == en_need_rsp)&&(OAL_PTR_NULL == ppst_rsp_msg)))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_send_cfg_event_etc::OAL_PTR_NULL == ppst_rsp_msg!}\r\n");
        return -OAL_EINVAL;
    }

    /* ???????? */
    ul_ret = wal_alloc_cfg_event_etc(pst_net_dev, &pst_event_mem,
                                ((OAL_TRUE == en_need_rsp) ? &st_msg_request : NULL),
                                &pst_cfg_msg,
                                WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_send_cfg_event_etc::wal_alloc_cfg_event_etc return err code %d!}\r\n", ul_ret);
        return -OAL_ENOMEM;
    }

    /* ???????????? */
    WAL_CFG_MSG_HDR_INIT(&(pst_cfg_msg->st_msg_hdr),
                         en_msg_type,
                         us_len,
                         WAL_GET_MSG_SN());

    /* ????WID???? */
    if (EOK != memcpy_s(pst_cfg_msg->auc_msg_data, us_len, puc_param, us_len)) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_send_cfg_event_etc::memcpy fail!");
        FRW_EVENT_FREE(pst_event_mem);
        return -OAL_EINVAL;
    }

#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_DFT_EVENT
    wal_event_report_to_sdt(en_msg_type, puc_param, pst_cfg_msg);
#endif
#endif

    if (OAL_TRUE == en_need_rsp)
    {
        /*add queue before post event!*/
        wal_msg_request_add_queue_etc(&st_msg_request);
    }

/* ???????? */
#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        OAM_ERROR_LOG0(0 , OAM_SF_ANY, "{wal_send_cfg_event_etc::OAL_NET_DEV_PRIV(pst_net_dev) is null ptr.}");
        FRW_EVENT_FREE(pst_event_mem);
        return -OAL_EINVAL;
    }

    frw_event_post_event_etc(pst_event_mem, pst_mac_vap->ul_core_id);
#else
    frw_event_dispatch_event_etc(pst_event_mem);
#endif
    FRW_EVENT_FREE(pst_event_mem);

    /* win32 UT?????????????????????? */
#if (_PRE_OS_VERSION_WIN32 == _PRE_OS_VERSION) && (_PRE_TEST_MODE == _PRE_TEST_MODE_UT)
    frw_event_process_all_event_etc(0);
#endif

    if (OAL_FALSE == en_need_rsp)
    {
        return OAL_SUCC;
    }

    /*context can't in interrupt*/
    if(OAL_WARN_ON(oal_in_interrupt()))
    {
        DECLARE_DFT_TRACE_KEY_INFO("wal_cfg_in_interrupt", OAL_DFT_TRACE_EXCEP);
    }

    if(OAL_WARN_ON(oal_in_atomic()))
    {
        DECLARE_DFT_TRACE_KEY_INFO("wal_cfg_in_atomic", OAL_DFT_TRACE_EXCEP);
    }

    /***************************************************************************
        ????????????
    ***************************************************************************/
    wal_wake_lock();


    /*lint -e730*//*lint -e666*//* info, boolean argument to function */
    l_ret = OAL_WAIT_EVENT_TIMEOUT((g_wal_wid_msg_queue.st_wait_queue),
                                   (oal_bool_enum_uint8)(OAL_TRUE == wal_request_wait_event_condition(&st_msg_request)),
                                   (10 * OAL_TIME_HZ));
    /*lint +e730*//*lint +e666*/

    /*response had been set, remove it from the list*/
    if (OAL_TRUE == en_need_rsp)
    {
        wal_msg_request_remove_queue_etc(&st_msg_request);
    }

    if (OAL_WARN_ON(0 == l_ret))
    {
        /* ???? .*/
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_send_cfg_event_etc:: wait queue timeout,10s!}\r\n");
        OAL_IO_PRINT("[E]timeout,request info:%p,ret=%u,addr:0x%lx\n", st_msg_request.pst_resp_mem,
                                               st_msg_request.ul_ret,
                                               st_msg_request.ul_request_address);
        WAL_MSG_REQ_RESP_MEM_FREE(st_msg_request);
        wal_wake_unlock();
        DECLARE_DFT_TRACE_KEY_INFO("wal_send_cfg_timeout",OAL_DFT_TRACE_FAIL);
        /*????CFG EVENT????????????????*/
        oal_print_hex_dump((oal_uint8 *)pst_cfg_msg, (WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len), 32, "cfg event: ");
        //????????100%??????cpu 100%??????????????????????????????????

#if (_PRE_FRW_FEATURE_PROCCESS_ENTITY_TYPE == _PRE_FRW_FEATURE_PROCCESS_ENTITY_THREAD)
#ifndef  CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT
        if(event_task_etc[0].pst_event_kthread)
        {
            sched_show_task(event_task_etc[0].pst_event_kthread);
        }
#endif
#endif
        return -OAL_ETIMEDOUT;
    }
    /*lint +e774*/

    pst_rsp_msg = (wal_msg_stru *)(st_msg_request.pst_resp_mem);
    if (OAL_PTR_NULL == pst_rsp_msg)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_send_cfg_event_etc:: msg mem null!}");
        /*lint -e613*/
        // tscancode-suppress *
        *ppst_rsp_msg  = OAL_PTR_NULL;
        /*lint +e613*/
        wal_wake_unlock();
        return -OAL_EFAUL;
    }

    if (0 == pst_rsp_msg->st_msg_hdr.us_msg_len)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_send_cfg_event_etc:: no msg resp!}");
        /*lint -e613*/
        *ppst_rsp_msg  = OAL_PTR_NULL;
        /*lint +e613*/
        oal_free(pst_rsp_msg);
        wal_wake_unlock();
        return -OAL_EFAUL;
    }
    /* ?????????????????????????? */
    /*lint -e613*/
    *ppst_rsp_msg  = pst_rsp_msg;
    /*lint +e613*/
    wal_wake_unlock();
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_P2P


wlan_p2p_mode_enum_uint8 wal_wireless_iftype_to_mac_p2p_mode_etc(enum nl80211_iftype en_iftype)
{
    wlan_p2p_mode_enum_uint8 en_p2p_mode = WLAN_LEGACY_VAP_MODE;

    switch(en_iftype)
    {
        case NL80211_IFTYPE_P2P_CLIENT:
            en_p2p_mode = WLAN_P2P_CL_MODE;
            break;
        case NL80211_IFTYPE_P2P_GO:
            en_p2p_mode = WLAN_P2P_GO_MODE;
            break;
        case NL80211_IFTYPE_P2P_DEVICE:
            en_p2p_mode = WLAN_P2P_DEV_MODE;
            break;
        case NL80211_IFTYPE_AP:
        case NL80211_IFTYPE_STATION:
            en_p2p_mode = WLAN_LEGACY_VAP_MODE;
            break;
        default:
            en_p2p_mode = WLAN_P2P_BUTT;
    }
    return en_p2p_mode;
}
#endif


oal_int32  wal_cfg_vap_h2d_event_etc(oal_net_device_stru *pst_net_dev)
{
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    oal_wireless_dev_stru      *pst_wdev = OAL_PTR_NULL;
    mac_wiphy_priv_stru        *pst_wiphy_priv = OAL_PTR_NULL;
    hmac_vap_stru              *pst_cfg_hmac_vap = OAL_PTR_NULL;
    mac_device_stru            *pst_mac_device = OAL_PTR_NULL;
    oal_net_device_stru        *pst_cfg_net_dev = OAL_PTR_NULL;

    oal_int32                   l_ret;
    wal_msg_stru                *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                  ul_err_code;
    wal_msg_write_stru          st_write_msg;

    pst_wdev = OAL_NETDEVICE_WDEV(pst_net_dev);
    if(OAL_PTR_NULL == pst_wdev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_cfg_vap_h2d_event_etc::pst_wdev is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_wiphy_priv  = (mac_wiphy_priv_stru *)oal_wiphy_priv(pst_wdev->wiphy);
    if(OAL_PTR_NULL == pst_wiphy_priv)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_cfg_vap_h2d_event_etc::pst_wiphy_priv is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_mac_device  = pst_wiphy_priv->pst_mac_device;
    if(OAL_PTR_NULL == pst_mac_device)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_cfg_vap_h2d_event_etc::pst_mac_device is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_cfg_hmac_vap= (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_device->uc_cfg_vap_id);
    if (OAL_PTR_NULL == pst_cfg_hmac_vap)
    {
        OAM_WARNING_LOG1(0,OAM_SF_ANY,"{wal_cfg_vap_h2d_event_etc::mac_res_get_hmac_vap fail.vap_id[%u]}",pst_mac_device->uc_cfg_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_cfg_net_dev = pst_cfg_hmac_vap->pst_net_device;
    if(OAL_PTR_NULL == pst_cfg_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_cfg_vap_h2d_event_etc::pst_cfg_net_dev is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /***************************************************************************
    ????????wal??????
    ***************************************************************************/
    /* ???????? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_CFG_VAP_H2D, OAL_SIZEOF(mac_cfg_vap_h2d_stru));
    ((mac_cfg_vap_h2d_stru *)st_write_msg.auc_value)->pst_net_dev = pst_cfg_net_dev;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
             WAL_MSG_TYPE_WRITE,
             WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_vap_h2d_stru),
             (oal_uint8 *)&st_write_msg,
             OAL_TRUE,
             &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_cfg_vap_h2d_event_etc::wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_cfg_vap_h2d_event_etc::hmac cfg vap h2d fail,err code[%u]\r\n", ul_err_code);
        return -OAL_EINVAL;
    }

#endif

    return OAL_SUCC;

}
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

OAL_STATIC oal_int32  wal_host_dev_config(oal_net_device_stru *pst_net_dev, wlan_cfgid_enum_uint16 en_wid)
{
    oal_wireless_dev_stru      *pst_wdev = OAL_PTR_NULL;
    mac_wiphy_priv_stru        *pst_wiphy_priv = OAL_PTR_NULL;
    hmac_vap_stru              *pst_cfg_hmac_vap = OAL_PTR_NULL;
    mac_device_stru            *pst_mac_device = OAL_PTR_NULL;
    oal_net_device_stru        *pst_cfg_net_dev = OAL_PTR_NULL;

    oal_int32                   l_ret;
    wal_msg_stru                *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                  ul_err_code;
    wal_msg_write_stru          st_write_msg;

    pst_wdev = OAL_NETDEVICE_WDEV(pst_net_dev);
    if(NULL == pst_wdev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_host_dev_config::pst_wdev is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_wiphy_priv  = (mac_wiphy_priv_stru *)oal_wiphy_priv(pst_wdev->wiphy);
    if (OAL_PTR_NULL == pst_wiphy_priv)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_host_dev_config::pst_mac_device is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_mac_device  = pst_wiphy_priv->pst_mac_device;
    if(NULL == pst_mac_device)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_host_dev_config::pst_mac_device is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_cfg_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_device->uc_cfg_vap_id);
    if (NULL == pst_cfg_hmac_vap)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_host_dev_config::pst_cfg_hmac_vap is null vap_id:%d!}\r\n",pst_mac_device->uc_cfg_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_cfg_net_dev = pst_cfg_hmac_vap->pst_net_device;
    if(NULL == pst_cfg_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_host_dev_config::pst_cfg_net_dev is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /***************************************************************************
    ????????wal??????
    ***************************************************************************/
    /* ???????? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, en_wid, 0);

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
             WAL_MSG_TYPE_WRITE,
             WAL_MSG_WRITE_MSG_HDR_LENGTH,
             (oal_uint8 *)&st_write_msg,
             OAL_TRUE,
             &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_host_dev_config::wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_host_dev_config::hmac cfg vap h2d fail,err code[%u]\r\n", ul_err_code);
        return -OAL_EINVAL;
    }

    return OAL_SUCC;
}


oal_int32  wal_host_dev_init_etc(oal_net_device_stru *pst_net_dev)
{
    return wal_host_dev_config(pst_net_dev, WLAN_CFGID_HOST_DEV_INIT);
}


OAL_STATIC oal_int32  wal_host_dev_exit(oal_net_device_stru *pst_net_dev)
{
    return wal_host_dev_config(pst_net_dev, WLAN_CFGID_HOST_DEV_EXIT);
}
#endif


#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE

OAL_STATIC oal_uint32 hwifi_force_refresh_rf_params(oal_net_device_stru* pst_net_dev)
{
    /* update params */
    if (hwifi_config_init_etc(CUS_TAG_NV))
    {
        return OAL_FAIL;
    }

    /* send data to device */
    return hwifi_config_init_nvram_main_etc(pst_net_dev);
}

oal_void hwifi_config_host_global_11ax_ini_param(oal_void)
{
    oal_int32                       l_val = 0;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_11AX_SWITCH);
    g_pst_mac_device_capability[0].en_11ax_switch = (((oal_uint32)l_val & 0x0F) & BIT0) ? OAL_TRUE : OAL_FALSE;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_MULTI_BSSID_SWITCH);
    g_pst_mac_device_capability[0].bit_multi_bssid_switch = (((oal_uint32)l_val & 0x0F) & BIT0) ? OAL_TRUE : OAL_FALSE;

}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

OAL_STATIC oal_int32 hwifi_set_voe_custom_param(oal_void)
{
    oal_uint32                       ul_val = 0;

    ul_val = (oal_uint32)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_VOE_SWITCH);
    g_st_mac_voe_custom_param.en_11k = (ul_val & BIT0) ? OAL_TRUE : OAL_FALSE;
    g_st_mac_voe_custom_param.en_11v = (ul_val & BIT1) ? OAL_TRUE : OAL_FALSE;
    g_st_mac_voe_custom_param.en_11r = (ul_val & BIT2) ? OAL_TRUE : OAL_FALSE;
    g_st_mac_voe_custom_param.en_11r_ds = (ul_val & BIT3) ? OAL_TRUE : OAL_FALSE;
    g_st_mac_voe_custom_param.en_adaptive11r = (ul_val & BIT4) ? OAL_TRUE : OAL_FALSE;
    g_st_mac_voe_custom_param.en_nb_rpt_11k = (ul_val & BIT5) ? OAL_TRUE : OAL_FALSE;

    return OAL_SUCC;
}
#endif


oal_int32 hwifi_config_host_global_ini_param(oal_void)
{
//#ifdef _PRE_WLAN_FEATURE_ROAM
    oal_int32                       l_val = 0;
//#endif /* #ifdef _PRE_WLAN_FEATURE_ROAM */
#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ

    oal_uint32                      cfg_id;
    oal_uint32                      ul_val;
    oal_int32                       l_cfg_value;
    oal_int8*                       pc_tmp;
    host_speed_freq_level_stru      ast_host_speed_freq_level_tmp[4];
    device_speed_freq_level_stru    ast_device_speed_freq_level_tmp[4];
    oal_uint8                       uc_flag = OAL_FALSE;
    oal_uint8                       uc_index;
    oal_int32                       l_ret = EOK;
#endif /* #ifdef _PRE_WLAN_FEATURE_AUTO_FREQ */
    /******************************************** ???? ********************************************/
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_AMPDU_TX_MAX_NUM);
    wlan_customize_etc.ul_ampdu_tx_max_num = (WLAN_AMPDU_TX_MAX_NUM >= l_val && 1 <= l_val) ? (oal_uint32)l_val : wlan_customize_etc.ul_ampdu_tx_max_num;
    OAL_IO_PRINT("hwifi_config_host_global_ini_param::ampdu_tx_max_num:%d", wlan_customize_etc.ul_ampdu_tx_max_num);
#ifdef _PRE_WLAN_FEATURE_ROAM
    /******************************************** ???? ********************************************/
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_ROAM_SWITCH);
    wlan_customize_etc.uc_roam_switch          = (0 == l_val || 1 == l_val) ? (oal_uint8)l_val : wlan_customize_etc.uc_roam_switch;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_SCAN_ORTHOGONAL);
    wlan_customize_etc.uc_roam_scan_orthogonal = (1 <= l_val) ? (oal_uint8)l_val : wlan_customize_etc.uc_roam_scan_orthogonal;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TRIGGER_B);
    wlan_customize_etc.c_roam_trigger_b = (oal_int8)l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TRIGGER_A);
    wlan_customize_etc.c_roam_trigger_a = (oal_int8)l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_B);
    wlan_customize_etc.c_roam_delta_b = (oal_int8)l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_A);
    wlan_customize_etc.c_roam_delta_a = (oal_int8)l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DENSE_ENV_TRIGGER_B);
    wlan_customize_etc.c_dense_env_roam_trigger_b = (oal_int8)l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DENSE_ENV_TRIGGER_A);
    wlan_customize_etc.c_dense_env_roam_trigger_a = (oal_int8)l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_SCENARIO_ENABLE);
    wlan_customize_etc.uc_scenario_enable = (oal_uint8)l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_CANDIDATE_GOOD_RSSI);
    wlan_customize_etc.c_candidate_good_rssi = (oal_int8)l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_CANDIDATE_GOOD_NUM);
    wlan_customize_etc.uc_candidate_good_num = (oal_uint8)l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_CANDIDATE_WEAK_NUM);
    wlan_customize_etc.uc_candidate_weak_num = (oal_uint8)l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_INTERVAL_VARIABLE);
    wlan_customize_etc.us_roam_interval = (oal_uint16)l_val;
#endif /* #ifdef _PRE_WLAN_FEATURE_ROAM */

#ifdef _PRE_WLAN_FEATURE_AMPDU_TX_HW
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_HW_AMPDU);
    g_st_ampdu_hw.uc_ampdu_hw_en = (l_val > 0) ? OAL_TRUE : OAL_FALSE;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_HW_AMPDU_TH_HIGH);
    g_st_ampdu_hw.us_throughput_high = (l_val > 0) ? (oal_uint16)l_val : 300;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_HW_AMPDU_TH_LOW);
    g_st_ampdu_hw.us_throughput_low = (l_val > 0) ? (oal_uint16)l_val : 200;
    OAL_IO_PRINT("ampdu_hw enable[%d]H[%u]L[%u]\r\n", g_st_ampdu_hw.uc_ampdu_hw_en,g_st_ampdu_hw.us_throughput_high,g_st_ampdu_hw.us_throughput_low);
#endif


#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_IRQ_AFFINITY);
    g_freq_lock_control_etc.en_irq_affinity = (l_val > 0) ? OAL_TRUE : OAL_FALSE;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_IRQ_TH_HIGH);
    g_freq_lock_control_etc.us_throughput_irq_high = (l_val > 0) ? (oal_uint16)l_val : 200;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_IRQ_TH_LOW);
    g_freq_lock_control_etc.us_throughput_irq_low = (l_val > 0) ? (oal_uint16)l_val : 150;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_IRQ_PPS_TH_HIGH);
    g_freq_lock_control_etc.ul_irq_pps_high = (l_val > 0) ? (oal_uint32)l_val : 25000;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_IRQ_PPS_TH_LOW);
    g_freq_lock_control_etc.ul_irq_pps_low = (l_val > 0) ? (oal_uint32)l_val : 5000;
    OAL_IO_PRINT("irq affinity enable[%d]High_th[%u]Low_th[%u]\r\n", g_freq_lock_control_etc.en_irq_affinity,g_freq_lock_control_etc.us_throughput_irq_high,g_freq_lock_control_etc.us_throughput_irq_low);
#endif
#ifdef _PRE_WLAN_FEATURE_MULTI_NETBUF_AMSDU
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_AMPDU_AMSDU_SKB);
    g_st_tx_large_amsdu.uc_host_large_amsdu_en = (l_val > 0) ? OAL_TRUE : OAL_FALSE;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_AMSDU_AMPDU_TH_HIGH);
    g_st_tx_large_amsdu.us_amsdu_throughput_high = (l_val > 0) ? (oal_uint16)l_val : 300;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_AMSDU_AMPDU_TH_LOW);
    g_st_tx_large_amsdu.us_amsdu_throughput_low = (l_val > 0) ? (oal_uint16)l_val : 200;
    OAL_IO_PRINT("ampdu+amsdu lareg skb en[%d],high[%d],low[%d]\r\n", g_st_tx_large_amsdu.uc_host_large_amsdu_en,g_st_tx_large_amsdu.us_amsdu_throughput_high,g_st_tx_large_amsdu.us_amsdu_throughput_low);
#endif
#ifdef _PRE_WLAN_TCP_OPT
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_FILTER);
    g_st_tcp_ack_filter.uc_tcp_ack_filter_en = (l_val > 0) ? OAL_TRUE : OAL_FALSE;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_FILTER_TH_HIGH);
    g_st_tcp_ack_filter.us_rx_filter_throughput_high = (l_val > 0) ? (oal_uint16)l_val : 50;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_FILTER_TH_LOW);
    g_st_tcp_ack_filter.us_rx_filter_throughput_low = (l_val > 0) ? (oal_uint16)l_val : 20;
    OAL_IO_PRINT("tcp ack filter en[%d],high[%d],low[%d]\r\n", g_st_tcp_ack_filter.uc_tcp_ack_filter_en, g_st_tcp_ack_filter.us_rx_filter_throughput_high, g_st_tcp_ack_filter.us_rx_filter_throughput_low);
#endif


#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RX_DYN_BYPASS_EXTLNA);
    g_st_rx_dyn_bypass_extlna_switch.uc_ini_en = (l_val > 0) ? OAL_TRUE : OAL_FALSE;
    g_st_rx_dyn_bypass_extlna_switch.uc_cur_status = OAL_TRUE;/* ?????????????? */
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RX_DYN_BYPASS_EXTLNA_HIGH);
    g_st_rx_dyn_bypass_extlna_switch.us_throughput_high = (l_val > 0) ? (oal_uint16)l_val : 100;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RX_DYN_BYPASS_EXTLNA_LOW);
    g_st_rx_dyn_bypass_extlna_switch.us_throughput_low = (l_val > 0) ? (oal_uint16)l_val : 50;
    OAL_IO_PRINT("DYN_BYPASS_EXTLNA SWITCH en[%d],high[%d],low[%d]\r\n", g_st_rx_dyn_bypass_extlna_switch.uc_ini_en,g_st_rx_dyn_bypass_extlna_switch.us_throughput_high,g_st_rx_dyn_bypass_extlna_switch.us_throughput_low);

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TX_SMALL_AMSDU);
    g_st_small_amsdu_switch.uc_ini_small_amsdu_en = (l_val > 0) ? OAL_TRUE : OAL_FALSE;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_SMALL_AMSDU_HIGH);
    g_st_small_amsdu_switch.us_small_amsdu_throughput_high = (l_val > 0) ? (oal_uint16)l_val : 300;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_SMALL_AMSDU_LOW);
    g_st_small_amsdu_switch.us_small_amsdu_throughput_low = (l_val > 0) ? (oal_uint16)l_val : 200;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_SMALL_AMSDU_PPS_HIGH);
    g_st_small_amsdu_switch.us_small_amsdu_pps_high = (l_val > 0) ? (oal_uint16)l_val : 25000;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_SMALL_AMSDU_PPS_LOW);
    g_st_small_amsdu_switch.us_small_amsdu_pps_low = (l_val > 0) ? (oal_uint16)l_val : 5000;
    OAL_IO_PRINT("SMALL AMSDU SWITCH en[%d],high[%d],low[%d]\r\n",  g_st_small_amsdu_switch.uc_ini_small_amsdu_en,g_st_small_amsdu_switch.us_small_amsdu_throughput_high,g_st_small_amsdu_switch.us_small_amsdu_throughput_low);



    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TX_TCP_ACK_BUF);
    g_st_tcp_ack_buf_switch.uc_ini_tcp_ack_buf_en= (l_val > 0) ? OAL_TRUE : OAL_FALSE;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_BUF_HIGH);
    g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high = (l_val > 0) ? (oal_uint16)l_val : 90;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_BUF_LOW);
    g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low = (l_val > 0) ? (oal_uint16)l_val : 30;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_BUF_HIGH_40M);
    g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high_40M = (l_val > 0) ? (oal_uint16)l_val : 300;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_BUF_LOW_40M);
    g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low_40M = (l_val > 0) ? (oal_uint16)l_val : 150;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_BUF_HIGH_80M);
    g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high_80M = (l_val > 0) ? (oal_uint16)l_val : 550;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_BUF_LOW_80M);
    g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low_80M = (l_val > 0) ? (oal_uint16)l_val : 450;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_BUF_HIGH_160M);
    g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high_160M = (l_val > 0) ? (oal_uint16)l_val : 800;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_TCP_ACK_BUF_LOW_160M);
    g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low_160M = (l_val > 0) ? (oal_uint16)l_val : 700;
    OAL_IO_PRINT("TCP ACK BUF en[%d],20M high[%d],low[%d],40M high[%d],low[%d], 80M high[%d],low[%d],160M high[%d],low[%d].\r\n",
                  g_st_tcp_ack_buf_switch.uc_ini_tcp_ack_buf_en,
                  g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high,
                  g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low,
                  g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high_40M,
                  g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low_40M,
                  g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high_80M,
                  g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low_80M,
                  g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_high_160M,
                  g_st_tcp_ack_buf_switch.us_tcp_ack_buf_throughput_low_160M);

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RX_AMPDU_AMSDU_SKB);
    g_uc_host_rx_ampdu_amsdu  = (l_val > 0) ? OAL_TRUE : OAL_FALSE;
    OAL_IO_PRINT("Rx:ampdu+amsdu skb en[%d]\r\n", g_uc_host_rx_ampdu_amsdu);

#endif
#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
    /******************************************** ???????? ********************************************/
    /* config g_host_speed_freq_level_etc */
    pc_tmp = (oal_int8*)&ast_host_speed_freq_level_tmp;

    for(cfg_id = WLAN_CFG_INIT_PSS_THRESHOLD_LEVEL_0; cfg_id <= WLAN_CFG_INIT_DDR_FREQ_LIMIT_LEVEL_3; ++cfg_id)
    {
        ul_val = hwifi_get_init_value_etc(CUS_TAG_INI, cfg_id);
        *(oal_uint32*)pc_tmp = ul_val;
        pc_tmp += 4;
    }

    /* config g_device_speed_freq_level_etc */
    pc_tmp = (oal_int8*)&ast_device_speed_freq_level_tmp;
    for(cfg_id = WLAN_CFG_INIT_DEVICE_TYPE_LEVEL_0; cfg_id <= WLAN_CFG_INIT_DEVICE_TYPE_LEVEL_3; ++cfg_id)
    {
        l_cfg_value = hwifi_get_init_value_etc(CUS_TAG_INI, cfg_id);
        if (OAL_VALUE_IN_VALID_RANGE(l_cfg_value, FREQ_IDLE, FREQ_HIGHEST))
        {
            *pc_tmp = l_cfg_value;
            pc_tmp += 4;
        }
        else
        {
            uc_flag = OAL_TRUE;
            break;
        }
    }

    if(!uc_flag)
    {
        l_ret += memcpy_s(&g_host_speed_freq_level_etc, OAL_SIZEOF(g_host_speed_freq_level_etc),
                          &ast_host_speed_freq_level_tmp, OAL_SIZEOF(g_host_speed_freq_level_etc));
        l_ret += memcpy_s(&g_device_speed_freq_level_etc, OAL_SIZEOF(g_device_speed_freq_level_etc),
                          &ast_device_speed_freq_level_tmp, OAL_SIZEOF(g_device_speed_freq_level_etc));
        if (l_ret != EOK) {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "hwifi_config_host_global_ini_param::memcpy fail!");
            return OAL_FAIL;
        }

        for(uc_index = 0;uc_index < 4;uc_index++)
        {
            OAM_WARNING_LOG4(0, OAM_SF_ANY, "{hwifi_config_host_global_ini_param::ul_speed_level = %d,ul_min_cpu_freq = %d,ul_min_ddr_freq = %d,uc_device_type = %d}\r\n",
                g_host_speed_freq_level_etc[uc_index].ul_speed_level,
                g_host_speed_freq_level_etc[uc_index].ul_min_cpu_freq,
                g_host_speed_freq_level_etc[uc_index].ul_min_ddr_freq,
                g_device_speed_freq_level_etc[uc_index].uc_device_type);
        }
    }
#endif /* #ifdef _PRE_WLAN_FEATURE_AUTO_FREQ */
    /******************************************** ???? ********************************************/
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RANDOM_MAC_ADDR_SCAN);
    wlan_customize_etc.uc_random_mac_addr_scan = !!l_val;

    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RANDOM_MAC_ADDR_CONNECT);
    wlan_customize_etc.uc_random_mac_addr_connect = !!l_val;
    /******************************************** CAPABILITY ********************************************/
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DISABLE_CAPAB_2GHT40);
    wlan_customize_etc.uc_disable_capab_2ght40 = (oal_uint8)!!l_val;
    /********************************************factory_lte_gpio_check ********************************************/
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_LTE_GPIO_CHECK_SWITCH);
    wlan_customize_etc.ul_lte_gpio_check_switch = (oal_uint32)!!l_val;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_ATCMDSRV_ISM_PRIORITY);
    wlan_customize_etc.ul_ism_priority = (oal_uint32)l_val;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_ATCMDSRV_LTE_RX);
    wlan_customize_etc.ul_lte_rx = (oal_uint32)l_val;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_ATCMDSRV_LTE_TX);
    wlan_customize_etc.ul_lte_tx = (oal_uint32)l_val;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_ATCMDSRV_LTE_INACT);
    wlan_customize_etc.ul_lte_inact = (oal_uint32)l_val;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_ATCMDSRV_ISM_RX_ACT);
    wlan_customize_etc.ul_ism_rx_act = (oal_uint32)l_val;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_ATCMDSRV_BANT_PRI);
    wlan_customize_etc.ul_bant_pri = (oal_uint32)l_val;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_ATCMDSRV_BANT_STATUS);
    wlan_customize_etc.ul_bant_status = (oal_uint32)l_val;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_ATCMDSRV_WANT_PRI);
    wlan_customize_etc.ul_want_pri = (oal_uint32)l_val;
    l_val = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_ATCMDSRV_WANT_STATUS);
    wlan_customize_etc.ul_want_status = (oal_uint32)l_val;
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hwifi_set_voe_custom_param();
#endif
#ifdef _PRE_WLAN_FEATURE_11AX
    hwifi_config_host_global_11ax_ini_param();
#endif
    return OAL_SUCC;
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

OAL_STATIC oal_void hwifi_config_init_ini_country(oal_net_device_stru *pst_cfg_net_dev)
{
    oal_int32                   l_ret;

    l_ret = (oal_int32)wal_hipriv_setcountry(pst_cfg_net_dev, hwifi_get_country_code_etc());

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{hwifi_config_init_ini_country::wal_send_cfg_event_etc return err code [%d]!}\r\n", l_ret);
    }

    /* ????wifi???????????????? */
    cust_country_code_ignore_flag.en_country_code_ingore_hipriv_flag = OAL_FALSE;
}
#endif

#ifdef _PRE_WLAN_FEATURE_SMARTANT
OAL_STATIC oal_void hwifi_config_init_ini_dual_antenna(oal_net_device_stru *pst_cfg_net_dev)
{
    wal_msg_write_stru          st_write_msg;
    oal_int32                   l_ret;
    oal_int32                   l_switch;
    oal_uint8                   *puc_param;
    l_switch = !!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DUAL_ANTENNA_ENABLE);
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DOUBLE_ANT_SW, OAL_SIZEOF(oal_int32));
    puc_param = (oal_uint8 *)(st_write_msg.auc_value);
    *puc_param = (oal_uint8)l_switch;
    *(puc_param + 1) = 1;
    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{hwifi_config_init_ini_dual_antenna::return err code [%d]!}\r\n", l_ret);
    }
}
#endif

OAL_STATIC oal_void hwifi_config_init_ini_log(oal_net_device_stru *pst_cfg_net_dev)
{
    wal_msg_write_stru          st_write_msg;
    oal_int32                   l_ret;
    oal_int32                   l_loglevel;

    /* log_level */
    l_loglevel = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LOGLEVEL);
    if (l_loglevel < OAM_LOG_LEVEL_ERROR ||
        l_loglevel > OAM_LOG_LEVEL_INFO)
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "{hwifi_config_init_ini_clock::loglevel[%d] out of range[%d,%d], check value in ini file!}\r\n",
                            l_loglevel, OAM_LOG_LEVEL_ERROR, OAM_LOG_LEVEL_INFO);
        return;
    }

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_ALL_LOG_LEVEL, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = l_loglevel;
    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{hwifi_config_init_ini_log::return err code[%d]!}\r\n", l_ret);
    }
}


#if defined(_PRE_PRODUCT_ID_HI110X_HOST)

oal_int8 hwifi_check_pwr_ref_delta(oal_int8 c_pwr_ref_delta)
{
    oal_int c_ret = 0;
    if (c_pwr_ref_delta > WAL_HIPRIV_PWR_REF_DELTA_HI)
    {
        c_ret = WAL_HIPRIV_PWR_REF_DELTA_HI;
    }
    else if (c_pwr_ref_delta < WAL_HIPRIV_PWR_REF_DELTA_LO)
    {
        c_ret = WAL_HIPRIV_PWR_REF_DELTA_LO;
    }
    else
    {
        c_ret = c_pwr_ref_delta;
    }

    return c_ret;
}


oal_void hwifi_cfg_pwr_ref_delta(mac_cfg_customize_rf *pst_customize_rf)
{
    oal_uint8 uc_rf_idx;
    WLAN_CFG_INIT cfg_id;
    oal_int32 l_pwr_ref_delta;

    for (uc_rf_idx = 0; uc_rf_idx < WLAN_RF_CHANNEL_NUMS; uc_rf_idx++)
    {
        /* 2G 20M/40M */
        cfg_id = (uc_rf_idx == WLAN_RF_CHANNEL_ZERO) ? WLAN_CFG_INIT_RF_PWR_REF_RSSI_2G_C0_MULT4 : WLAN_CFG_INIT_RF_PWR_REF_RSSI_2G_C1_MULT4;
        l_pwr_ref_delta = hwifi_get_init_value_etc(CUS_TAG_INI, cfg_id);
        pst_customize_rf->ast_delta_pwr_ref_cfg[uc_rf_idx].c_cfg_delta_pwr_ref_rssi_2g[0] = hwifi_check_pwr_ref_delta((oal_int8)CUS_GET_FIRST_BYTE(l_pwr_ref_delta));
        pst_customize_rf->ast_delta_pwr_ref_cfg[uc_rf_idx].c_cfg_delta_pwr_ref_rssi_2g[1] = hwifi_check_pwr_ref_delta((oal_int8)CUS_GET_SECOND_BYTE(l_pwr_ref_delta));
        pst_customize_rf->ast_delta_pwr_ref_cfg[uc_rf_idx].c_cfg_delta_pwr_ref_rssi_2g[2] = hwifi_check_pwr_ref_delta((oal_int8)CUS_GET_THIRD_BYTE(l_pwr_ref_delta));

        /* 5G 20M/40M/80M/160M */
        cfg_id = (uc_rf_idx == WLAN_RF_CHANNEL_ZERO) ? WLAN_CFG_INIT_RF_PWR_REF_RSSI_5G_C0_MULT4 : WLAN_CFG_INIT_RF_PWR_REF_RSSI_5G_C1_MULT4;
        l_pwr_ref_delta = hwifi_get_init_value_etc(CUS_TAG_INI, cfg_id);
        pst_customize_rf->ast_delta_pwr_ref_cfg[uc_rf_idx].c_cfg_delta_pwr_ref_rssi_5g[0] = hwifi_check_pwr_ref_delta((oal_int8)CUS_GET_FIRST_BYTE(l_pwr_ref_delta));
        pst_customize_rf->ast_delta_pwr_ref_cfg[uc_rf_idx].c_cfg_delta_pwr_ref_rssi_5g[1] = hwifi_check_pwr_ref_delta((oal_int8)CUS_GET_SECOND_BYTE(l_pwr_ref_delta));
        pst_customize_rf->ast_delta_pwr_ref_cfg[uc_rf_idx].c_cfg_delta_pwr_ref_rssi_5g[2] = hwifi_check_pwr_ref_delta((oal_int8)CUS_GET_THIRD_BYTE(l_pwr_ref_delta));
        pst_customize_rf->ast_delta_pwr_ref_cfg[uc_rf_idx].c_cfg_delta_pwr_ref_rssi_5g[3] = hwifi_check_pwr_ref_delta((oal_int8)CUS_GET_FOURTH_BYTE(l_pwr_ref_delta));
    }
}


OAL_STATIC OAL_INLINE oal_uint32 hwifi_cfg_front_end_value_range_check(mac_cfg_customize_rf *pst_customize_rf, oal_int32 l_wlan_band, oal_int32 l_rf_db_min)
{
    return ((pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ZERO].c_lna_bypass_gain_db < l_rf_db_min || pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ZERO].c_lna_bypass_gain_db > 0 ||
        pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ZERO].c_pa_gain_b0_db < l_rf_db_min || pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ZERO].c_pa_gain_b0_db > 0 ||
        pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ZERO].c_pa_gain_b1_db < l_rf_db_min || pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ZERO].c_pa_gain_b1_db > 0 ||
        pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ZERO].uc_pa_gain_lvl_num == 0 || pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ZERO].uc_pa_gain_lvl_num > MAC_EXT_PA_GAIN_MAX_LVL ||
        pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ZERO].c_lna_gain_db < LNA_GAIN_DB_MIN || pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ZERO].c_lna_gain_db > LNA_GAIN_DB_MAX) ||
        (pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ONE].c_lna_bypass_gain_db < l_rf_db_min || pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ONE].c_lna_bypass_gain_db > 0 ||
        pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ONE].c_pa_gain_b0_db < l_rf_db_min || pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ONE].c_pa_gain_b0_db > 0 ||
        pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ONE].c_pa_gain_b1_db < l_rf_db_min || pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ONE].c_pa_gain_b1_db > 0 ||
        pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ONE].uc_pa_gain_lvl_num == 0 || pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ONE].uc_pa_gain_lvl_num > MAC_EXT_PA_GAIN_MAX_LVL ||
        pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ONE].c_lna_gain_db < LNA_GAIN_DB_MIN || pst_customize_rf->ast_ext_rf[l_wlan_band][WLAN_RF_CHANNEL_ONE].c_lna_gain_db > LNA_GAIN_DB_MAX));
}


OAL_STATIC OAL_INLINE oal_uint32 hwifi_cfg_front_end_adjustment_range_check(oal_int8 c_delta_cca_ed_high_20th_2g,
                                                                                oal_int8 c_delta_cca_ed_high_40th_2g,
                                                                                oal_int8 c_delta_cca_ed_high_20th_5g,
                                                                                oal_int8 c_delta_cca_ed_high_40th_5g,
                                                                                oal_int8 c_delta_cca_ed_high_80th_5g)
{
    return (CUS_DELTA_CCA_ED_HIGH_TH_OUT_OF_RANGE(c_delta_cca_ed_high_20th_2g)
            || CUS_DELTA_CCA_ED_HIGH_TH_OUT_OF_RANGE(c_delta_cca_ed_high_40th_2g)
            || CUS_DELTA_CCA_ED_HIGH_TH_OUT_OF_RANGE(c_delta_cca_ed_high_20th_5g)
            || CUS_DELTA_CCA_ED_HIGH_TH_OUT_OF_RANGE(c_delta_cca_ed_high_40th_5g)
            || CUS_DELTA_CCA_ED_HIGH_TH_OUT_OF_RANGE(c_delta_cca_ed_high_80th_5g));
}

OAL_STATIC oal_uint32 hwifi_cfg_front_end(oal_uint8 *puc_param)
{
    mac_cfg_customize_rf       *pst_customize_rf;
    oal_uint8                   uc_idx;                 /*??????????????*/
    oal_int32                   l_mult4;
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
    oal_int8                    c_mult4_rf[2];
#endif //#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)

    pst_customize_rf = (mac_cfg_customize_rf *)puc_param;

    /* ????: 2g rf */
    for (uc_idx = 0; uc_idx < MAC_NUM_2G_BAND; ++uc_idx)
    {
        /* ??????2p4g ??band 0.25db??0.1db???????????? */
        l_mult4  = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_2G_BAND_START + uc_idx);
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
        /* rf0 */
        c_mult4_rf[0]  = (oal_int8)CUS_GET_FIRST_BYTE(l_mult4);
        /* rf1 */
        c_mult4_rf[1]  = (oal_int8)CUS_GET_SECOND_BYTE(l_mult4);
        if(CUS_VAL_VALID(c_mult4_rf[0], RF_LINE_TXRX_GAIN_DB_MAX, RF_LINE_TXRX_GAIN_DB_2G_MIN) &&
           CUS_VAL_VALID(c_mult4_rf[1], RF_LINE_TXRX_GAIN_DB_MAX, RF_LINE_TXRX_GAIN_DB_2G_MIN))
        {
            pst_customize_rf->ast_rf_gain_db_rf[0].ac_gain_db_2g[uc_idx].c_rf_gain_db_mult4  = c_mult4_rf[0];
            pst_customize_rf->ast_rf_gain_db_rf[1].ac_gain_db_2g[uc_idx].c_rf_gain_db_mult4  = c_mult4_rf[1];
        }
#else
        if(l_mult4  >= RF_LINE_TXRX_GAIN_DB_2G_MIN && l_mult4  <= 0)
        {
            pst_customize_rf->st_rf_gain_db_rf.ac_gain_db_2g[uc_idx].c_rf_gain_db_mult4  = (oal_int8)l_mult4;
        }
#endif //#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
        else
        {
            /* ?????????????? */
            OAM_ERROR_LOG2(0, OAM_SF_CFG, "{hwifi_cfg_front_end::ini_id[%d]value out of range, 2g mult4[0x%0x}!}",
                          WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_2G_BAND_START + uc_idx, l_mult4);
            return OAL_FAIL;
        }
    }

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
    hwifi_cfg_pwr_ref_delta(pst_customize_rf);

    /* ????radio cap */
    pst_customize_rf->uc_chn_radio_cap = (oal_uint8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_CHANN_RADIO_CAP);

    /*2g ????fem */
    /* RF0 */
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ZERO].c_lna_bypass_gain_db = (oal_int8)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LNA_BYPASS_GAIN_DB_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ZERO].c_lna_gain_db        = (oal_int8)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LNA_GAIN_DB_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ZERO].c_pa_gain_b0_db      = (oal_int8)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_DB_B0_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ZERO].c_pa_gain_b1_db      = (oal_int8)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_DB_B1_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ZERO].uc_pa_gain_lvl_num   = (oal_int8)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_LVL_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ZERO].uc_ext_switch_isexist = (oal_uint8)!!CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_SWITCH_ISEXIST_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ZERO].uc_ext_pa_isexist     = (oal_uint8)!!CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_PA_ISEXIST_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ZERO].uc_ext_lna_isexist    = (oal_uint8)!!CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_LNA_ISEXIST_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ZERO].us_lna_on2off_time_ns = (oal_uint16)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_ON2OFF_TIME_NS_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ZERO].us_lna_off2on_time_ns = (oal_uint16)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_OFF2ON_TIME_NS_2G));
    /* RF1 */
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].c_lna_bypass_gain_db = (oal_int8)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LNA_BYPASS_GAIN_DB_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].c_lna_gain_db        = (oal_int8)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LNA_GAIN_DB_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].c_pa_gain_b0_db      = (oal_int8)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_DB_B0_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].c_pa_gain_b1_db      = (oal_int8)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_DB_B1_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].uc_pa_gain_lvl_num   = (oal_int8)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_LVL_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].uc_ext_switch_isexist = (oal_uint8)!!CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_SWITCH_ISEXIST_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].uc_ext_pa_isexist     = (oal_uint8)!!CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_PA_ISEXIST_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].uc_ext_lna_isexist    = (oal_uint8)!!CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_LNA_ISEXIST_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].us_lna_on2off_time_ns = (oal_uint16)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_ON2OFF_TIME_NS_2G));
    pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].us_lna_off2on_time_ns = (oal_uint16)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_OFF2ON_TIME_NS_2G));

    if (hwifi_cfg_front_end_value_range_check(pst_customize_rf, WLAN_BAND_2G, RF_LINE_TXRX_GAIN_DB_2G_MIN))
    {
        /* ?????????????? */
        OAM_ERROR_LOG4(0, OAM_SF_CFG,
        "{hwifi_cfg_front_end::2g gain db out of range! rf0 lna_bypass[%d] pa_b0[%d] lna gain[%d] pa_b1[%d]}",
        pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].c_lna_bypass_gain_db,
        pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].c_pa_gain_b0_db,
        pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].c_lna_gain_db,
        pst_customize_rf->ast_ext_rf[WLAN_BAND_2G][WLAN_RF_CHANNEL_ONE].c_pa_gain_b1_db);

        return OAL_FAIL;
    }

    /* 2g??????RF????PA??????????  */
    for (uc_idx = 0; uc_idx < CUS_RF_PA_BIAS_REG_NUM; uc_idx++)
    {
        pst_customize_rf->aul_2g_pa_bias_rf_reg[uc_idx] = (oal_uint32)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_TX2G_PA_GATE_VCTL_REG236 + uc_idx);
    }
#endif //#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)

    if (OAL_TRUE == mac_device_check_5g_enable_per_chip())
    {
        /* ????: 5g rf */
        /* ????: fem?????????????????? */
        for (uc_idx = 0; uc_idx < MAC_NUM_5G_BAND; ++uc_idx)
        {
            /* ??????5g ??band 0.25db??0.1db???????????? */
            l_mult4  = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND_START + uc_idx);
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
            c_mult4_rf[0]  = (oal_int8)CUS_GET_FIRST_BYTE(l_mult4);
            c_mult4_rf[1]  = (oal_int8)CUS_GET_SECOND_BYTE(l_mult4);
            if (c_mult4_rf[0] <= RF_LINE_TXRX_GAIN_DB_MAX && c_mult4_rf[1] <= RF_LINE_TXRX_GAIN_DB_MAX)
            {
                pst_customize_rf->ast_rf_gain_db_rf[0].ac_gain_db_5g[uc_idx].c_rf_gain_db_mult4  = c_mult4_rf[0];
                pst_customize_rf->ast_rf_gain_db_rf[1].ac_gain_db_5g[uc_idx].c_rf_gain_db_mult4  = c_mult4_rf[1];
            }
#else
            if(l_mult4 <= 0)
            {
                pst_customize_rf->st_rf_gain_db_rf.ac_gain_db_5g[uc_idx].c_rf_gain_db_mult4  = (oal_int8)l_mult4;
            }
#endif //#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
            else
            {
                /* ?????????????? */
                OAM_ERROR_LOG2(0, OAM_SF_CFG, "{hwifi_cfg_front_end::ini_id[%d]value out of range, 5g mult4[0x%0x}!}\r\n",
                          WLAN_CFG_INIT_RF_RX_INSERTION_LOSS_5G_BAND_START + uc_idx, l_mult4);
                return OAL_FAIL;
            }
        }

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
        /*5g ????fem */
        /* RF0 */
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].c_lna_bypass_gain_db = (oal_int8)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LNA_BYPASS_GAIN_DB_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].c_lna_gain_db        = (oal_int8)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LNA_GAIN_DB_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].c_pa_gain_b0_db      = (oal_int8)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_DB_B0_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].c_pa_gain_b1_db      = (oal_int8)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_DB_B1_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].uc_pa_gain_lvl_num   = (oal_int8)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_LVL_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].uc_ext_switch_isexist = (oal_uint8)!!CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_SWITCH_ISEXIST_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].uc_ext_pa_isexist     = (oal_uint8)!!(CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_PA_ISEXIST_5G)) & EXT_PA_ISEXIST_5G_MASK);
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].en_fem_lp_enable      = (oal_fem_lp_state_enum_uint8)((CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_PA_ISEXIST_5G)) & EXT_FEM_LP_STATUS_MASK) >> EXT_FEM_LP_STATUS_OFFSET);
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].c_fem_spec_value      = (oal_int8)((CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_PA_ISEXIST_5G)) & EXT_FEM_FEM_SPEC_MASK) >> EXT_FEM_FEM_SPEC_OFFSET);
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].uc_ext_lna_isexist    = (oal_uint8)!!CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_LNA_ISEXIST_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].us_lna_on2off_time_ns = (oal_uint16)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_ON2OFF_TIME_NS_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ZERO].us_lna_off2on_time_ns = (oal_uint16)CUS_GET_LOW_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_OFF2ON_TIME_NS_5G));
        /* RF1 */
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].c_lna_bypass_gain_db = (oal_int8)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LNA_BYPASS_GAIN_DB_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].c_lna_gain_db        = (oal_int8)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LNA_GAIN_DB_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].c_pa_gain_b0_db      = (oal_int8)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_DB_B0_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].c_pa_gain_b1_db      = (oal_int8)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_DB_B1_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].uc_pa_gain_lvl_num   = (oal_int8)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_LVL_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].uc_ext_switch_isexist = (oal_uint8)!!CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_SWITCH_ISEXIST_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].uc_ext_pa_isexist     = (oal_uint8)!!(CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_PA_ISEXIST_5G)) & EXT_PA_ISEXIST_5G_MASK);
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].en_fem_lp_enable      = (oal_fem_lp_state_enum_uint8)((CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_PA_ISEXIST_5G)) & EXT_FEM_LP_STATUS_MASK) >> EXT_FEM_LP_STATUS_OFFSET);
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].c_fem_spec_value      = (oal_int8)((CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_PA_ISEXIST_5G)) & EXT_FEM_FEM_SPEC_MASK) >> EXT_FEM_FEM_SPEC_OFFSET);
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].uc_ext_lna_isexist    = (oal_uint8)!!CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_LNA_ISEXIST_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].us_lna_on2off_time_ns = (oal_uint16)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_ON2OFF_TIME_NS_5G));
        pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].us_lna_off2on_time_ns = (oal_uint16)CUS_GET_HIGH_16BITS(hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_OFF2ON_TIME_NS_5G));

        /* 5g upc mix_bf_gain_ctl for P10 */
        for (uc_idx = 0; uc_idx < MAC_NUM_5G_BAND; uc_idx++)
        {
            pst_customize_rf->aul_5g_upc_mix_gain_rf_reg[uc_idx] = (oal_uint32)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_TX5G_UPC_MIX_GAIN_CTRL_1 + uc_idx);
        }

        if (hwifi_cfg_front_end_value_range_check(pst_customize_rf, WLAN_BAND_5G, RF_LINE_TXRX_GAIN_DB_5G_MIN))
        {
            /* ?????????????? */
            OAM_ERROR_LOG4(0, OAM_SF_CFG,
            "{hwifi_cfg_front_end::2g gain db out of range! rf0 lna_bypass[%d] pa_b0[%d] lna gain[%d] pa_b1[%d]}",
            pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].c_lna_bypass_gain_db,
            pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].c_pa_gain_b0_db,
            pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].c_lna_gain_db,
            pst_customize_rf->ast_ext_rf[WLAN_BAND_5G][WLAN_RF_CHANNEL_ONE].c_pa_gain_b1_db);

            return OAL_FAIL;
        }
#else
        /*5g ????fem */
        pst_customize_rf->st_ext_rf.c_lna_bypass_gain_db    = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LNA_BYPASS_GAIN_DB_5G);
        pst_customize_rf->st_ext_rf.c_lna_gain_db           = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LNA_GAIN_DB_5G);
        pst_customize_rf->st_ext_rf.c_pa_gain_b0_db         = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_PA_GAIN_DB_B0_5G);
        pst_customize_rf->st_ext_rf.uc_ext_switch_isexist   = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_SWITCH_ISEXIST_5G);
        pst_customize_rf->st_ext_rf.uc_ext_pa_isexist       = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_PA_ISEXIST_5G);
        pst_customize_rf->st_ext_rf.uc_ext_lna_isexist      = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_LNA_ISEXIST_5G);
        pst_customize_rf->st_ext_rf.us_lna_on2off_time_ns   = (oal_uint16)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_ON2OFF_TIME_NS_5G);
        pst_customize_rf->st_ext_rf.us_lna_off2on_time_ns   = (oal_uint16)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_OFF2ON_TIME_NS_5G);

        if (CUS_VAL_INVALID(pst_customize_rf->st_ext_rf.c_lna_bypass_gain_db, 0, RF_LINE_TXRX_GAIN_DB_5G_MIN) ||
            CUS_VAL_INVALID(pst_customize_rf->st_ext_rf.c_pa_gain_b0_db, 0, RF_LINE_TXRX_GAIN_DB_5G_MIN) ||
            CUS_VAL_INVALID(pst_customize_rf->st_ext_rf.c_lna_gain_db, LNA_GAIN_DB_MAX, LNA_GAIN_DB_MIN))
        {
            /* ?????????????? */
            OAM_ERROR_LOG0(0, OAM_SF_CFG, "{hwifi_cfg_front_end::gain db rx or tx 5g lna gain db or out of range!}");
            return OAL_FAIL;
        }
#endif //#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
    }

    pst_customize_rf->uc_far_dist_pow_gain_switch = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_FAR_DIST_POW_GAIN_SWITCH);
    pst_customize_rf->uc_far_dist_dsss_scale_promote_switch = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_FAR_DIST_DSSS_SCALE_PROMOTE_SWITCH);

    /* ????: cca?????????????? */
    {
        oal_int8 c_delta_cca_ed_high_20th_2g = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_20TH_2G);
        oal_int8 c_delta_cca_ed_high_40th_2g = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_40TH_2G);
        oal_int8 c_delta_cca_ed_high_20th_5g = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_20TH_5G);
        oal_int8 c_delta_cca_ed_high_40th_5g = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_40TH_5G);
        oal_int8 c_delta_cca_ed_high_80th_5g = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_80TH_5G);

        /* ???????????????????????????????????? */
        if (hwifi_cfg_front_end_adjustment_range_check(c_delta_cca_ed_high_20th_2g,
                                                       c_delta_cca_ed_high_40th_2g,
                                                       c_delta_cca_ed_high_20th_5g,
                                                       c_delta_cca_ed_high_40th_5g,
                                                       c_delta_cca_ed_high_80th_5g))
        {
            OAM_ERROR_LOG4(0, OAM_SF_ANY, "{hwifi_cfg_front_end::one or more delta cca ed high threshold out of range \
                 [delta_20th_2g=%d, delta_40th_2g=%d, delta_20th_5g=%d, delta_40th_5g=%d], please check the value!}",
                                            c_delta_cca_ed_high_20th_2g,
                                            c_delta_cca_ed_high_40th_2g,
                                            c_delta_cca_ed_high_20th_5g,
                                            c_delta_cca_ed_high_40th_5g);
            /* set 0 */
            pst_customize_rf->c_delta_cca_ed_high_20th_2g = 0;
            pst_customize_rf->c_delta_cca_ed_high_40th_2g = 0;
            pst_customize_rf->c_delta_cca_ed_high_20th_5g = 0;
            pst_customize_rf->c_delta_cca_ed_high_40th_5g = 0;
            pst_customize_rf->c_delta_cca_ed_high_80th_5g = 0;
        }
        else
        {
            pst_customize_rf->c_delta_cca_ed_high_20th_2g = c_delta_cca_ed_high_20th_2g;
            pst_customize_rf->c_delta_cca_ed_high_40th_2g = c_delta_cca_ed_high_40th_2g;
            pst_customize_rf->c_delta_cca_ed_high_20th_5g = c_delta_cca_ed_high_20th_5g;
            pst_customize_rf->c_delta_cca_ed_high_40th_5g = c_delta_cca_ed_high_40th_5g;
            pst_customize_rf->c_delta_cca_ed_high_80th_5g = c_delta_cca_ed_high_80th_5g;
        }
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FIT_BASED_REALTIME_CALI

OAL_STATIC oal_uint32 hwifi_cfg_init_cus_dyn_cali(mac_cus_dy_cali_param_stru *puc_dyn_cali_param)
{
    oal_int32      l_val;
    oal_uint8      uc_idx    = 0;
    oal_uint8      uc_rf_idx;
    oal_uint8      uc_dy_cal_param_idx;
    oal_uint8      uc_cfg_id        = WLAN_CFG_DTS_2G_CORE0_DPN_CH1;
    oal_uint8      uc_dpn_2g_nv_id  = WLAN_CFG_DTS_NVRAM_MUFREQ_2GCCK_C0;
    oal_uint8      uc_dpn_5g_nv_id  = WLAN_CFG_DTS_NVRAM_MUFREQ_5G160_C0;
    oal_int8       ac_dpn_nv[HWIFI_CFG_DYN_PWR_CALI_2G_SNGL_MODE_CW][MAC_2G_CHANNEL_NUM];
    oal_int8       ac_dpn_5g_nv[OAL_5G_160M_CHANNEL_NUM];
    oal_uint8      uc_num_idx;
    oal_uint8     *puc_cust_nvram_info;
    oal_uint8     *pc_end = ";";
    oal_uint8     *pc_sep = ",";
    oal_int8      *pc_ctx;
    oal_int8      *pc_token;
    oal_uint8      auc_nv_pa_params[CUS_PARAMS_LEN_MAX] = {0};
    oal_int32      l_ret;

    for (uc_rf_idx = 0; uc_rf_idx < WLAN_RF_CHANNEL_NUMS; uc_rf_idx++)
    {
        puc_dyn_cali_param->uc_rf_id = uc_rf_idx;

        /* ?????????????????????????? */
        for (uc_dy_cal_param_idx = 0; uc_dy_cal_param_idx < DY_CALI_PARAMS_NUM; uc_dy_cal_param_idx++)
        {
            if (!pro_line_params[uc_rf_idx][uc_dy_cal_param_idx].l_pow_par2)
            {
                OAM_ERROR_LOG1(0, OAM_SF_CUSTOM, "{hwifi_cfg_init_cus_dyn_cali::unexpected val[%d] s_pow_par2[0] check nv and ini file!}", uc_dy_cal_param_idx);
                return OAL_FAIL;
            }
        }
        l_ret = memcpy_s(puc_dyn_cali_param->al_dy_cali_base_ratio_params,
                         OAL_SIZEOF(puc_dyn_cali_param->al_dy_cali_base_ratio_params),
                         pro_line_params[uc_rf_idx],
                         OAL_SIZEOF(puc_dyn_cali_param->al_dy_cali_base_ratio_params));

        l_ret += memcpy_s(puc_dyn_cali_param->al_dy_cali_base_ratio_ppa_params,
                          OAL_SIZEOF(puc_dyn_cali_param->al_dy_cali_base_ratio_ppa_params),
                          &pro_line_params[uc_rf_idx][CUS_DY_CALI_PARAMS_NUM],
                          OAL_SIZEOF(puc_dyn_cali_param->al_dy_cali_base_ratio_ppa_params));

        l_ret += memcpy_s(puc_dyn_cali_param->as_extre_point_val,
                          OAL_SIZEOF(puc_dyn_cali_param->as_extre_point_val),
                          gs_extre_point_vals[uc_rf_idx],
                          OAL_SIZEOF(puc_dyn_cali_param->as_extre_point_val));

#ifdef _PRE_WLAN_DPINIT_CALI
        /* DP INIT*/
        if (OAL_FALSE == en_nv_dp_init_is_null)
        {
            puc_cust_nvram_info = hwifi_get_nvram_param(uc_rf_idx);
            if (EOK != memcpy_s(auc_nv_pa_params, OAL_SIZEOF(auc_nv_pa_params),
                                puc_cust_nvram_info, OAL_STRLEN(puc_cust_nvram_info))) {
                OAM_ERROR_LOG0(0, OAM_SF_CUSTOM, "hwifi_cfg_init_cus_dyn_cali::memcpy fail!");
                return OAL_FAIL;
            }
            pc_token = oal_strtok(auc_nv_pa_params, pc_end, &pc_ctx);
            pc_token = oal_strtok(pc_token, pc_sep, &pc_ctx);
            uc_idx = 0;
            /* ?????????????? */
            while ((OAL_PTR_NULL != pc_token) && (uc_idx < MAC_2G_CHANNEL_NUM))
            {
                puc_dyn_cali_param->ac_dp_init[uc_idx] = (oal_int8)oal_strtol(pc_token, OAL_PTR_NULL, 10);
                pc_token = oal_strtok(OAL_PTR_NULL, pc_sep, &pc_ctx);
                uc_idx++;
            }
        }
        else
        {
            memset_s(puc_dyn_cali_param->ac_dp_init, MAC_2G_CHANNEL_NUM * OAL_SIZEOF(oal_int8),
                     0, MAC_2G_CHANNEL_NUM * OAL_SIZEOF(oal_int8));
        }
#endif //#ifdef _PRE_WLAN_DPINIT_CALI

        /* DPN */
        for (uc_idx = 0; uc_idx < MAC_2G_CHANNEL_NUM; uc_idx++)
        {
            l_val = hwifi_get_init_value_etc(CUS_TAG_DTS, uc_cfg_id + uc_idx);
            l_ret += memcpy_s(puc_dyn_cali_param->ac_dy_cali_2g_dpn_params[uc_idx],
                              CUS_DY_CALI_DPN_PARAMS_NUM * OAL_SIZEOF(oal_int8),
                              &l_val, CUS_DY_CALI_DPN_PARAMS_NUM * OAL_SIZEOF(oal_int8));
        }
        uc_cfg_id += MAC_2G_CHANNEL_NUM;

        for (uc_idx = HWIFI_CFG_DYN_PWR_CALI_2G_SNGL_MODE_11B; uc_idx  <= HWIFI_CFG_DYN_PWR_CALI_2G_SNGL_MODE_OFDM40; uc_idx++)
        {
            /* ????????????DPN?????? */
            puc_cust_nvram_info = hwifi_get_nvram_param(uc_dpn_2g_nv_id);
            uc_dpn_2g_nv_id++;

            if (0 == OAL_STRLEN(puc_cust_nvram_info))
            {
                continue;
            }

            memset_s(auc_nv_pa_params, OAL_SIZEOF(auc_nv_pa_params), 0, OAL_SIZEOF(auc_nv_pa_params));
            l_ret += memcpy_s(auc_nv_pa_params, OAL_SIZEOF(auc_nv_pa_params),
                              puc_cust_nvram_info, OAL_STRLEN(puc_cust_nvram_info));
            pc_token = oal_strtok(auc_nv_pa_params, pc_end, &pc_ctx);
            pc_token = oal_strtok(pc_token, pc_sep, &pc_ctx);
            uc_num_idx = 0;
            while ((OAL_PTR_NULL != pc_token))
            {
                if (uc_num_idx >= MAC_2G_CHANNEL_NUM)
                {
                    uc_num_idx++;
                    break;
                }
                l_val = oal_strtol(pc_token, OAL_PTR_NULL, 10) / 10;
                pc_token = oal_strtok(OAL_PTR_NULL, pc_sep, &pc_ctx);
                if (OAL_VALUE_NOT_IN_VALID_RANGE(l_val, CUS_DY_CALI_2G_VAL_DPN_MIN, CUS_DY_CALI_2G_VAL_DPN_MAX))
                {
                    OAM_ERROR_LOG3(0, OAM_SF_CUSTOM, "{hwifi_cfg_init_cus_dyn_cali::nvram 2g dpn val[%d] is unexpected uc_idx[%d] uc_num_idx[%d}!}",
                                   l_val, uc_idx, uc_num_idx);
                    l_val = 0;
                }
                ac_dpn_nv[uc_idx][uc_num_idx] = (oal_int8)l_val;
                uc_num_idx++;
            }

            if (MAC_2G_CHANNEL_NUM != uc_num_idx)
            {
                OAM_ERROR_LOG2(0, OAM_SF_CUSTOM, "{hwifi_cfg_init_cus_dyn_cali::nvram 2g dpn num is unexpected uc_id[%d] rf[%d}!}",
                               uc_idx, uc_rf_idx);
                continue;
            }

            for (uc_num_idx = 0; uc_num_idx < MAC_2G_CHANNEL_NUM; uc_num_idx++)
            {
                puc_dyn_cali_param->ac_dy_cali_2g_dpn_params[uc_num_idx][uc_idx] += ac_dpn_nv[uc_idx][uc_num_idx];
            }
        }

        for (uc_idx = 0; uc_idx < CUS_DY_CALI_NUM_5G_BAND; uc_idx++)
        {
            l_val = hwifi_get_init_value_etc(CUS_TAG_DTS, uc_cfg_id + uc_idx);
            l_ret += memcpy_s(puc_dyn_cali_param->ac_dy_cali_5g_dpn_params[uc_idx],
                              CUS_DY_CALI_DPN_PARAMS_NUM * OAL_SIZEOF(oal_int8),
                              &l_val, CUS_DY_CALI_DPN_PARAMS_NUM * OAL_SIZEOF(oal_int8));
        }
        uc_cfg_id += DY_CALI_NUM_5G_BAND;

        /* 5G 160M DPN */
        puc_cust_nvram_info = hwifi_get_nvram_param(uc_dpn_5g_nv_id);
        uc_dpn_5g_nv_id++;
        if (OAL_STRLEN(puc_cust_nvram_info))
        {
            memset_s(auc_nv_pa_params, OAL_SIZEOF(auc_nv_pa_params), 0, OAL_SIZEOF(auc_nv_pa_params));
            l_ret += memcpy_s(auc_nv_pa_params, OAL_SIZEOF(auc_nv_pa_params),
                              puc_cust_nvram_info, OAL_STRLEN(puc_cust_nvram_info));
            pc_token = oal_strtok(auc_nv_pa_params, pc_end, &pc_ctx);
            pc_token = oal_strtok(pc_token, pc_sep, &pc_ctx);
            uc_num_idx = 0;
            while ((OAL_PTR_NULL != pc_token))
            {
                if (uc_num_idx >= OAL_5G_160M_CHANNEL_NUM)
                {
                    uc_num_idx++;
                    break;
                }
                l_val = oal_strtol(pc_token, OAL_PTR_NULL, 10) / 10;
                pc_token = oal_strtok(OAL_PTR_NULL, pc_sep, &pc_ctx);
                if (OAL_VALUE_NOT_IN_VALID_RANGE(l_val, CUS_DY_CALI_5G_VAL_DPN_MIN, CUS_DY_CALI_5G_VAL_DPN_MAX))
                {
                    OAM_ERROR_LOG3(0, OAM_SF_CUSTOM, "{hwifi_cfg_init_cus_dyn_cali::nvram 2g dpn val[%d] is unexpected uc_idx[%d] uc_num_idx[%d}!}",
                                   l_val, uc_idx, uc_num_idx);
                    l_val = 0;
                }
                ac_dpn_5g_nv[uc_num_idx] = (oal_int8)l_val;
                uc_num_idx++;
            }

            if (OAL_5G_160M_CHANNEL_NUM != uc_num_idx)
            {
                OAM_ERROR_LOG2(0, OAM_SF_CUSTOM, "{hwifi_cfg_init_cus_dyn_cali::nvram 2g dpn num is unexpected uc_id[%d] rf[%d}!}",
                               uc_idx, uc_rf_idx);
                continue;
            }
            /* 5250  5570 */
            for (uc_num_idx = 0; uc_num_idx < OAL_5G_160M_CHANNEL_NUM; uc_num_idx++)
            {
                puc_dyn_cali_param->ac_dy_cali_5g_dpn_params[uc_num_idx+1][WLAN_BW_CAP_160M] += ac_dpn_5g_nv[uc_num_idx];
            }
        }

        puc_dyn_cali_param++;
    }
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_CUSTOM, "hwifi_cfg_init_cus_dyn_cali::memcpy fail!");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}
#endif

#else  //#if defined(_PRE_PRODUCT_ID_HI110X_HOST)

OAL_STATIC oal_uint32 hwifi_cfg_front_end(oal_uint8 *puc_param)
{
    mac_cfg_customize_rf*       pst_customize_rf;
    oal_uint8                   uc_idx;                 /*??????????????*/
    oal_int32                   l_mult4;
    oal_int32                   l_mult10;

    pst_customize_rf = (mac_cfg_customize_rf *)puc_param;

    /* ????: 2g rf */
    for (uc_idx = 0; uc_idx < MAC_NUM_2G_BAND; ++uc_idx)
    {
        /* ??????2p4g ??band 0.25db??0.1db???????????? */
        l_mult4  = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LINE_TXRX_GAIN_DB_2G_BAND_START + 2*uc_idx);
        l_mult10 = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LINE_TXRX_GAIN_DB_2G_BAND_START + 2*uc_idx + 1);

        if(l_mult4  >= RF_LINE_TXRX_GAIN_DB_2G_MIN && l_mult4  <= 0 &&
           l_mult10 >= RF_LINE_TXRX_GAIN_DB_2G_MIN && l_mult10 <= 0)
        {
            pst_customize_rf->st_rf_gain_db_rf.ac_gain_db_2g[uc_idx].c_rf_gain_db_mult4  = (oal_int8)l_mult4;
            pst_customize_rf->st_rf_gain_db_rf.ac_gain_db_2g[uc_idx].c_rf_gain_db_mult10 = (oal_int8)l_mult10;
        }
        else
        {
            /* ?????????????? */
            OAM_ERROR_LOG3(0, OAM_SF_CFG, "{hwifi_cfg_front_end::ini_id[%d]value out of range, 2g mult4[0x%0x}mult10[0x%0x]!}\r\n",
                          WLAN_CFG_INIT_RF_LINE_TXRX_GAIN_DB_2G_BAND_START + 2*uc_idx, l_mult4, l_mult10);
            return OAL_FAIL;
        }
    }

    if (OAL_TRUE == mac_device_check_5g_enable_per_chip())
    {
        /* ????: 5g rf */
        /* ????: fem?????????????????? */
        for (uc_idx = 0; uc_idx < MAC_NUM_5G_BAND; ++uc_idx)
        {
            /* ??????5g ??band 0.25db??0.1db???????????? */
            l_mult4  = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LINE_TXRX_GAIN_DB_5G_BAND_START + 2*uc_idx);
            l_mult10 = hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LINE_TXRX_GAIN_DB_5G_BAND_START + 2*uc_idx + 1);

            if(l_mult4 <= 0 && l_mult10 <= 0)
            {
                pst_customize_rf->st_rf_gain_db_rf.ac_gain_db_5g[uc_idx].c_rf_gain_db_mult4  = (oal_int8)l_mult4;
                pst_customize_rf->st_rf_gain_db_rf.ac_gain_db_5g[uc_idx].c_rf_gain_db_mult10 = (oal_int8)l_mult10;
            }
            else
            {
                /* ?????????????? */
                OAM_ERROR_LOG3(0, OAM_SF_CFG, "{hwifi_cfg_front_end::ini_id[%d]value out of range, 5g mult4[0x%0x}mult10[0x%0x]!}\r\n",
                          WLAN_CFG_INIT_RF_LINE_TXRX_GAIN_DB_5G_BAND_START + 2*uc_idx, l_mult4, l_mult10);
                return OAL_FAIL;
            }
        }


        /*5g ????fem */
        pst_customize_rf->st_ext_rf.c_lna_bypass_gain_db    = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LINE_RX_GAIN_DB_5G);
        pst_customize_rf->st_ext_rf.c_lna_gain_db           = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_GAIN_DB_5G);
        pst_customize_rf->st_ext_rf.c_pa_gain_b0_db    = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_RF_LINE_TX_GAIN_DB_5G);
        pst_customize_rf->st_ext_rf.uc_ext_switch_isexist   = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_SWITCH_ISEXIST_5G);
        pst_customize_rf->st_ext_rf.uc_ext_pa_isexist       = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_PA_ISEXIST_5G);
        pst_customize_rf->st_ext_rf.uc_ext_lna_isexist      = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_EXT_LNA_ISEXIST_5G);
        pst_customize_rf->st_ext_rf.us_lna_on2off_time_ns   = (oal_uint16)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_ON2OFF_TIME_NS_5G);
        pst_customize_rf->st_ext_rf.us_lna_off2on_time_ns   = (oal_uint16)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_LNA_OFF2ON_TIME_NS_5G);
        if (pst_customize_rf->st_ext_rf.c_lna_bypass_gain_db < RF_LINE_TXRX_GAIN_DB_5G_MIN ||
            pst_customize_rf->st_ext_rf.c_lna_bypass_gain_db > 0 ||
            pst_customize_rf->st_ext_rf.c_pa_gain_b0_db < RF_LINE_TXRX_GAIN_DB_5G_MIN ||
            pst_customize_rf->st_ext_rf.c_pa_gain_b0_db > 0 ||
            pst_customize_rf->st_ext_rf.c_lna_gain_db < LNA_GAIN_DB_MIN ||
            pst_customize_rf->st_ext_rf.c_lna_gain_db > LNA_GAIN_DB_MAX)
        {
            /* ?????????????? */
            OAM_ERROR_LOG0(0, OAM_SF_CFG, "{hwifi_cfg_front_end::gain db rx or tx 5g lna gain db or out of range!}\r\n");
            return OAL_FAIL;
        }
    }

    pst_customize_rf->uc_far_dist_pow_gain_switch = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_FAR_DIST_POW_GAIN_SWITCH);
    pst_customize_rf->uc_far_dist_dsss_scale_promote_switch = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_FAR_DIST_DSSS_SCALE_PROMOTE_SWITCH);

    /* ????: cca?????????????? */
    {
        oal_int8 c_delta_cca_ed_high_20th_2g = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_20TH_2G);
        oal_int8 c_delta_cca_ed_high_40th_2g = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_40TH_2G);
        oal_int8 c_delta_cca_ed_high_20th_5g = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_20TH_5G);
        oal_int8 c_delta_cca_ed_high_40th_5g = (oal_int8)hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_DELTA_CCA_ED_HIGH_40TH_5G);

        /* ???????????????????????????????????? */
        if (CUS_DELTA_CCA_ED_HIGH_TH_OUT_OF_RANGE(c_delta_cca_ed_high_20th_2g)
           || CUS_DELTA_CCA_ED_HIGH_TH_OUT_OF_RANGE(c_delta_cca_ed_high_40th_2g)
           || CUS_DELTA_CCA_ED_HIGH_TH_OUT_OF_RANGE(c_delta_cca_ed_high_20th_5g)
           || CUS_DELTA_CCA_ED_HIGH_TH_OUT_OF_RANGE(c_delta_cca_ed_high_40th_5g))
        {
            OAM_ERROR_LOG4(0, OAM_SF_ANY, "{hwifi_cfg_front_end::one or more delta cca ed high threshold out of range \
                 [delta_20th_2g=%d, delta_40th_2g=%d, delta_20th_5g=%d, delta_40th_5g=%d], please check the value!}",
                                            c_delta_cca_ed_high_20th_2g,
                                            c_delta_cca_ed_high_40th_2g,
                                            c_delta_cca_ed_high_20th_5g,
                                            c_delta_cca_ed_high_40th_5g);//lint !e571
            /* set 0 */
            pst_customize_rf->c_delta_cca_ed_high_20th_2g = 0;
            pst_customize_rf->c_delta_cca_ed_high_40th_2g = 0;
            pst_customize_rf->c_delta_cca_ed_high_20th_5g = 0;
            pst_customize_rf->c_delta_cca_ed_high_40th_5g = 0;
        }
        else
        {
            pst_customize_rf->c_delta_cca_ed_high_20th_2g = c_delta_cca_ed_high_20th_2g;
            pst_customize_rf->c_delta_cca_ed_high_40th_2g = c_delta_cca_ed_high_40th_2g;
            pst_customize_rf->c_delta_cca_ed_high_20th_5g = c_delta_cca_ed_high_20th_5g;
            pst_customize_rf->c_delta_cca_ed_high_40th_5g = c_delta_cca_ed_high_40th_5g;
        }
    }

    return OAL_SUCC;
}
#endif //#if defined(_PRE_PRODUCT_ID_HI110X_HOST)


OAL_STATIC oal_void hwifi_config_init_ini_rf(oal_net_device_stru *pst_cfg_net_dev)
{
    wal_msg_write_stru          st_write_msg;
    oal_uint32                  ul_ret;
    oal_uint16                  us_event_len = OAL_SIZEOF(mac_cfg_customize_rf);

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_CUS_RF, us_event_len);

    /*lint -e774*/
    /* ???????????????????????????? */
    if (us_event_len > WAL_MSG_WRITE_MAX_LEN)
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{hwifi_config_init_ini_rf::event size[%d] larger than msg size[%d]!}\r\n",
                            us_event_len, WAL_MSG_WRITE_MAX_LEN);
        return;
    }
    /*lint +e774*/

    /*  */
    ul_ret = hwifi_cfg_front_end(st_write_msg.auc_value);
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hwifi_config_init_ini_rf::front end rf wrong value, not send cfg!}\r\n");
        return;
    }

    /* ???????????????????????????????????????? */
    ul_ret = (oal_uint32)wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_event_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{hwifi_config_init_ini_rf::EVENT[wal_send_cfg_event_etc] failed, return err code [%d]!}\r\n", ul_ret);
    }
}


extern oal_bool_enum_uint8 en_fact_cali_completed;

OAL_STATIC oal_uint32 hwifi_cfg_init_dts_cus_cali(oal_uint8 *puc_param, oal_uint8 uc_5g_Band_enable)
{
    oal_int32                    l_val;
    oal_int16                    s_ref_val_ch1;
    oal_int16                    s_ref_val_ch0;
    oal_uint8                    uc_idx;             /* ?????????????? */
    mac_cus_dts_cali_stru       *pst_cus_cali;
    oal_uint8                    uc_gm_opt;

    pst_cus_cali = (mac_cus_dts_cali_stru *)puc_param;
    /** ????: TXPWR_PA_DC_REF **/
    /*2G REF: ??13??????*/
    for (uc_idx = 0; uc_idx < 13; uc_idx++)
    {
        l_val = hwifi_get_init_value_etc(CUS_TAG_DTS, WLAN_CFG_DTS_CALI_TXPWR_PA_DC_REF_2G_START + uc_idx);
        s_ref_val_ch1 = (oal_int16)CUS_GET_HIGH_16BITS(l_val);
        s_ref_val_ch0 = (oal_int16)CUS_GET_LOW_16BITS(l_val);

        /* 2G??????????????????<0, ????RF????????, TBD */
        if (s_ref_val_ch0 <= CALI_TXPWR_PA_DC_REF_MAX)
        {
            pst_cus_cali->ast_cali[0].aus_cali_txpwr_pa_dc_ref_2g_val_chan[uc_idx] = s_ref_val_ch0;
        }
        else
        {
            /* ?????????????? */
            OAM_ERROR_LOG2(0, OAM_SF_ANY, "{hwifi_cfg_init_dts_cus_cali::dts 2g ref id[%d]value[%d] out of range!}\r\n",
                            WLAN_CFG_DTS_CALI_TXPWR_PA_DC_REF_2G_START + uc_idx, s_ref_val_ch0);//lint !e571
            return OAL_FAIL;
        }
        /* 02???????????????? */
        if (s_ref_val_ch1 <= CALI_TXPWR_PA_DC_REF_MAX)
        {
                pst_cus_cali->ast_cali[1].aus_cali_txpwr_pa_dc_ref_2g_val_chan[uc_idx] = s_ref_val_ch1;
        }
        else
        {
            /* ?????????????? */
            OAM_ERROR_LOG2(0, OAM_SF_ANY, "{hwifi_cfg_init_dts_cus_cali::dts ch1 2g ref id[%d]value[%d] out of range!}\r\n",
                             WLAN_CFG_DTS_CALI_TXPWR_PA_DC_REF_2G_START + uc_idx, s_ref_val_ch1);//lint !e571
            return OAL_FAIL;
        }


#if (_PRE_TARGET_PRODUCT_TYPE_E5 == _PRE_CONFIG_TARGET_PRODUCT)
        /* E5-5885 B40???????? */
        l_val = hwifi_get_init_value_etc(CUS_TAG_DTS, WLAN_CFG_DTS_CALI_TXPWR_PA_DC_REF_2G_B40_START+uc_idx);
        s_ref_val_ch1 = (oal_int16)CUS_GET_HIGH_16BITS(l_val);
        s_ref_val_ch0 = (oal_int16)CUS_GET_LOW_16BITS(l_val);
        if (s_ref_val_ch1 >= 0 && s_ref_val_ch1 <= CALI_TXPWR_PA_DC_REF_MAX
            && s_ref_val_ch0 >= 0 && s_ref_val_ch0 <= CALI_TXPWR_PA_DC_REF_MAX)
        {
            pst_cus_cali->ast_cali[0].aus_cali_txpwr_pa_dc_ref_2g_b40_val[uc_idx] = s_ref_val_ch0;
            pst_cus_cali->ast_cali[1].aus_cali_txpwr_pa_dc_ref_2g_b40_val[uc_idx] = s_ref_val_ch1;
        }
        else
        {
            /* ?????????????? */
            OAM_ERROR_LOG3(0, OAM_SF_ANY, "{hwifi_cfg_init_dts_cus_cali::dts 2g ref id[%d]value ch0[%d]ch1[%d] out of range!}\r\n",
                             WLAN_CFG_DTS_CALI_TXPWR_PA_DC_REF_2G_B40_START + uc_idx, s_ref_val_ch0, s_ref_val_ch1);
            return OAL_FAIL;
        }
#endif //#if (_PRE_TARGET_PRODUCT_TYPE_E5 == _PRE_CONFIG_TARGET_PRODUCT)
    }

    /*5G REF: ??7??band*/
    if (uc_5g_Band_enable)
    {
        for (uc_idx = 0; uc_idx < 7; ++uc_idx)
        {
            l_val = hwifi_get_init_value_etc(CUS_TAG_DTS, WLAN_CFG_DTS_CALI_TXPWR_PA_DC_REF_5G_START + uc_idx);
            s_ref_val_ch1 = (oal_int16)CUS_GET_HIGH_16BITS(l_val);
            s_ref_val_ch0 = (oal_int16)CUS_GET_LOW_16BITS(l_val);

            if (s_ref_val_ch0 >= 0 && s_ref_val_ch0 <= CALI_TXPWR_PA_DC_REF_MAX)
            {
                pst_cus_cali->ast_cali[0].aus_cali_txpwr_pa_dc_ref_5g_val_band[uc_idx] = s_ref_val_ch0;
            }
            else
            {
                /* ?????????????? */
                OAM_ERROR_LOG2(0, OAM_SF_ANY, "{hwifi_cfg_init_dts_cus_cali::dts 5g ref id[%d]value[%d] out of range!}\r\n",
                                WLAN_CFG_DTS_CALI_TXPWR_PA_DC_REF_5G_START + uc_idx, s_ref_val_ch0);//lint !e571
                return OAL_FAIL;
            }
            if (s_ref_val_ch1 >= 0 && s_ref_val_ch1 <= CALI_TXPWR_PA_DC_REF_MAX)
            {
                    pst_cus_cali->ast_cali[1].aus_cali_txpwr_pa_dc_ref_5g_val_band[uc_idx] = s_ref_val_ch1;
            }
            else
            {
                /* ?????????????? */
                OAM_ERROR_LOG2(0, OAM_SF_ANY, "{hwifi_cfg_init_dts_cus_cali::dts ch1 5g ref id[%d]value[%d] out of range!}\r\n",
                                WLAN_CFG_DTS_CALI_TXPWR_PA_DC_REF_5G_START + uc_idx, s_ref_val_ch1);//lint !e571
                return OAL_FAIL;
            }
        }
    }

    /* ????BAND 5G ENABLE */
    pst_cus_cali->uc_band_5g_enable = !!uc_5g_Band_enable;

    /* ???????????????? */
    pst_cus_cali->uc_tone_amp_grade = (oal_uint8)hwifi_get_init_value_etc(CUS_TAG_DTS, WLAN_CFG_DTS_CALI_TONE_AMP_GRADE);

    /* ????DPD???????? */
#ifdef _PRE_WLAN_ONLINE_DPD
    for (uc_idx = 0; uc_idx < MAC_DPD_CALI_CUS_PARAMS_NUM; uc_idx++)
    {
        /* ????0 */
        l_val = hwifi_get_init_value_etc(CUS_TAG_DTS, WLAN_CFG_DTS_DPD_CALI_START + uc_idx);
        pst_cus_cali->ast_dpd_cali_para[0].aul_dpd_cali_cus_dts[uc_idx] = l_val;
        /* ????1 */
        l_val = hwifi_get_init_value_etc(CUS_TAG_DTS, WLAN_CFG_DTS_DPD_CALI_START + uc_idx + MAC_DPD_CALI_CUS_PARAMS_NUM);
        pst_cus_cali->ast_dpd_cali_para[1].aul_dpd_cali_cus_dts[uc_idx] = l_val;
    }
#endif

    /* ???????????????? */
#ifdef _PRE_WLAN_FIT_BASED_REALTIME_CALI
    l_val = hwifi_get_init_value_etc(CUS_TAG_DTS, WLAN_CFG_DTS_DYN_CALI_DSCR_ITERVL);
    pst_cus_cali->aus_dyn_cali_dscr_interval[WLAN_BAND_2G] = (oal_uint16)((oal_uint32)l_val & 0x0000FFFF);

    if (uc_5g_Band_enable)
    {
        pst_cus_cali->aus_dyn_cali_dscr_interval[WLAN_BAND_5G] = (oal_uint16)(((oal_uint32)l_val & 0xFFFF0000) >> 16);
    }

    l_val = hwifi_get_init_value_etc(CUS_TAG_DTS, WLAN_CFG_DTS_DYN_CALI_OPT_SWITCH);
    uc_gm_opt = ((oal_uint32)l_val & BIT2) >> NUM_1_BITS;

    if (((oal_uint32)l_val & 0x3) >> 1)
    {
        /* ?????????? */
        l_val = !en_fact_cali_completed;
    }
    else
    {
        l_val = (oal_int32)((oal_uint32)l_val & BIT0);
    }

    pst_cus_cali->en_dyn_cali_opt_switch = (oal_uint32)l_val | uc_gm_opt;

    l_val = hwifi_get_init_value_etc(CUS_TAG_DTS, WLAN_CFG_DTS_DYN_CALI_GM0_DB10_AMEND);
    pst_cus_cali->as_gm0_dB10_amend[WLAN_RF_CHANNEL_ZERO] = (oal_int16)CUS_GET_LOW_16BITS(l_val);
    pst_cus_cali->as_gm0_dB10_amend[WLAN_RF_CHANNEL_ONE]  = (oal_int16)CUS_GET_HIGH_16BITS(l_val);
#endif //#ifdef _PRE_WLAN_FIT_BASED_REALTIME_CALI

    return OAL_SUCC;
}


OAL_STATIC oal_uint32 hwifi_config_init_dts_cali(oal_net_device_stru *pst_cfg_net_dev)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_ret;
    mac_cus_dts_cali_stru           st_cus_cali;
    oal_uint32                      ul_offset = 0;
    oal_bool_enum                   en_5g_Band_enable;    /* mac device????????5g???? */

    if (OAL_WARN_ON(OAL_PTR_NULL == pst_cfg_net_dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_CFG, "{hwifi_config_init_dts_cali::pst_cfg_net_dev is null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ????????????????????5g */
    en_5g_Band_enable = mac_device_check_5g_enable_per_chip();

    /* ????????????TXPWR_PA_DC_REF */
    ul_ret = hwifi_cfg_init_dts_cus_cali((oal_uint8 *)&st_cus_cali, en_5g_Band_enable);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_ERROR_LOG1(0, OAM_SF_CFG, "{hwifi_config_init_dts_cali::init dts cus cali failed ret[%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ???????????????????????????????????????? */
    if (EOK != memcpy_s(st_write_msg.auc_value, OAL_SIZEOF(mac_cus_dts_cali_stru),
                        (oal_int8*)&st_cus_cali, OAL_SIZEOF(mac_cus_dts_cali_stru))) {
        OAM_ERROR_LOG0(0, OAM_SF_CFG, "hwifi_config_init_dts_cali::memcpy fail!");
        return OAL_FAIL;
    }
    ul_offset += OAL_SIZEOF(mac_cus_dts_cali_stru);

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_CUS_DTS_CALI, ul_offset);
    ul_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + ul_offset,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{hwifi_config_init_dts_cali::wal_send_cfg_event_etc failed, ret[%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    OAM_WARNING_LOG0(0, OAM_SF_CFG, "{hwifi_config_init_dts_cali::wal_send_cfg_event send succ}");

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FIT_BASED_REALTIME_CALI

OAL_STATIC oal_uint32 hwifi_config_init_cus_dyn_cali(oal_net_device_stru *pst_cfg_net_dev)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_ret;
    oal_uint32                      ul_offset = 0;
    mac_cus_dy_cali_param_stru      st_dy_cus_cali[WLAN_RF_CHANNEL_NUMS];
    oal_uint8                       uc_rf_id;
    mac_cus_dy_cali_param_stru      *pst_dy_cus_cali;
    wal_msg_stru                    *pst_rsp_msg;
    wal_msg_write_rsp_stru          *pst_write_rsp_msg;
    oal_int32                        l_ret;

    if (OAL_WARN_ON(OAL_PTR_NULL == pst_cfg_net_dev))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ????????????????TXPWR_PA_DC_REF */
    memset_s(st_dy_cus_cali, OAL_SIZEOF(mac_cus_dy_cali_param_stru) * WLAN_RF_CHANNEL_NUMS,
             0, OAL_SIZEOF(mac_cus_dy_cali_param_stru) * WLAN_RF_CHANNEL_NUMS);
    ul_ret = hwifi_cfg_init_cus_dyn_cali(st_dy_cus_cali);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{hwifi_config_init_cus_dyn_cali::init cus dyn cali failed ret[%d]!}", ul_ret);
        return ul_ret;
    }

    for (uc_rf_id = 0; uc_rf_id < WLAN_RF_CHANNEL_NUMS; uc_rf_id++)
    {
        pst_dy_cus_cali = &st_dy_cus_cali[uc_rf_id];
        pst_rsp_msg     = OAL_PTR_NULL;

        /* ???????????????????????????????????????? */
        l_ret = memcpy_s(st_write_msg.auc_value, OAL_SIZEOF(mac_cus_dy_cali_param_stru),
                         (oal_int8*)pst_dy_cus_cali, OAL_SIZEOF(mac_cus_dy_cali_param_stru));
        if (l_ret != EOK) {
            OAM_ERROR_LOG0(0, OAM_SF_CFG, "hwifi_config_init_cus_dyn_cali::memcpy fail!");
            return OAL_FAIL;
        }

        ul_offset = OAL_SIZEOF(mac_cus_dy_cali_param_stru);
        WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_CUS_DYN_CALI_PARAM, ul_offset);
        ul_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                                   WAL_MSG_TYPE_WRITE,
                                   WAL_MSG_WRITE_MSG_HDR_LENGTH + ul_offset,
                                   (oal_uint8 *)&st_write_msg,
                                   OAL_TRUE,
                                   &pst_rsp_msg);

        if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{hwifi_config_init_cus_dyn_cali::wal_send_cfg_event_etc failed, ret[%d]!}", ul_ret);
            return ul_ret;
        }

        if(OAL_PTR_NULL != pst_rsp_msg)
        {
            pst_write_rsp_msg = (wal_msg_write_rsp_stru *)(pst_rsp_msg->auc_msg_data);

            if (OAL_SUCC != pst_write_rsp_msg->ul_err_code)
            {
                OAM_ERROR_LOG2(0, OAM_SF_SCAN, "{wal_check_and_release_msg_resp_etc::detect err code:[%u],wid:[%u]}",
                                pst_write_rsp_msg->ul_err_code, pst_write_rsp_msg->en_wid);
                oal_free(pst_rsp_msg);
                return OAL_FAIL;
            }

            oal_free(pst_rsp_msg);
        }
    }

    return OAL_SUCC;
}


oal_uint32 hwifi_config_init_nvram_main_etc(oal_net_device_stru *pst_cfg_net_dev)
{
    wal_msg_write_stru  st_write_msg;
    oal_int32           l_ret;
    oal_uint16          us_offset = OAL_SIZEOF(wlan_cust_nvram_params); /* ????4?????????? */

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_CUS_NVRAM_PARAM, us_offset);
    l_ret = memcpy_s(st_write_msg.auc_value, us_offset, hwifi_get_nvram_params_etc(), us_offset);
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "hwifi_config_init_nvram_main_etc::memcpy fail!");
        return OAL_FAIL;
    }

    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_offset,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{hwifi_config_init_nvram_main_etc::return err code [%d]!}\r\n", l_ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

#endif //#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)


OAL_STATIC oal_void hwifi_config_init_ini_main(oal_net_device_stru *pst_cfg_net_dev)
{
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /* ?????? */
    hwifi_config_init_ini_country(pst_cfg_net_dev);
#endif
    /* ???????? */
    hwifi_config_init_ini_log(pst_cfg_net_dev);
    /* RF */
    hwifi_config_init_ini_rf(pst_cfg_net_dev);
#ifdef _PRE_WLAN_FEATURE_SMARTANT
    hwifi_config_init_ini_dual_antenna(pst_cfg_net_dev);
#endif
}


oal_uint32 hwifi_config_init_dts_main_etc(oal_net_device_stru *pst_cfg_net_dev)
{
    oal_uint32 ul_ret = OAL_SUCC;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_FIT_BASED_REALTIME_CALI
    /* ???????????????? */
    hwifi_config_init_cus_dyn_cali(pst_cfg_net_dev);
#endif
#endif

    /* ???? */
    if (OAL_SUCC != hwifi_config_init_dts_cali(pst_cfg_net_dev))
    {
        return OAL_FAIL;
    }
    /* ?????????????????? */
    return ul_ret;
}


OAL_STATIC oal_int32 hwifi_config_init_ini_wlan(oal_net_device_stru *pst_net_dev)
{
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_P2P

OAL_STATIC oal_int32 hwifi_config_init_ini_p2p(oal_net_device_stru *pst_net_dev)
{
    return OAL_SUCC;
}
#endif


oal_int32 hwifi_config_init_ini_etc(oal_net_device_stru *pst_net_dev)
{
    oal_net_device_stru        *pst_cfg_net_dev = OAL_PTR_NULL;
    mac_vap_stru               *pst_mac_vap = OAL_PTR_NULL;
    oal_wireless_dev_stru      *pst_wdev = OAL_PTR_NULL;
    mac_wiphy_priv_stru        *pst_wiphy_priv = OAL_PTR_NULL;
    hmac_vap_stru              *pst_cfg_hmac_vap = OAL_PTR_NULL;
    mac_device_stru            *pst_mac_device = OAL_PTR_NULL;
    oal_int8                    ac_wlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];
    oal_int8                    ac_p2p_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];

    if(OAL_PTR_NULL== pst_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hwifi_config_init_ini_etc::pst_net_dev is null!}\r\n");
        return -OAL_EINVAL;
    }

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(OAL_PTR_NULL== pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hwifi_config_init_ini_etc::pst_mac_vap is null}\r\n");
        return -OAL_EINVAL;
    }

    pst_wdev = OAL_NETDEVICE_WDEV(pst_net_dev);
    if(OAL_PTR_NULL == pst_wdev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hwifi_config_init_ini_etc::pst_wdev is null!}\r\n");
        return -OAL_EFAUL;
    }

    pst_wiphy_priv  = (mac_wiphy_priv_stru *)oal_wiphy_priv(pst_wdev->wiphy);
    if (OAL_PTR_NULL == pst_wiphy_priv)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hwifi_config_init_ini_etc::pst_wiphy_priv is null!}\r\n");
        return -OAL_EFAUL;
    }
    pst_mac_device  = pst_wiphy_priv->pst_mac_device;
    if(OAL_PTR_NULL == pst_mac_device)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hwifi_config_init_ini_etc::pst_mac_device is null!}\r\n");
        return -OAL_EFAUL;
    }

    pst_cfg_hmac_vap= (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_device->uc_cfg_vap_id);
    if (OAL_PTR_NULL == pst_cfg_hmac_vap)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hwifi_config_init_ini_etc::pst_cfg_hmac_vap is null, vap_id:%d!}\r\n",pst_mac_device->uc_cfg_vap_id);
        return -OAL_EFAUL;
    }

    pst_cfg_net_dev = pst_cfg_hmac_vap->pst_net_device;

    if(OAL_PTR_NULL == pst_cfg_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hwifi_config_init_ini_etc::pst_cfg_net_dev is null!}\r\n");
        return -OAL_EFAUL;
    }

    snprintf_s(ac_wlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "wlan%d", pst_mac_device->uc_device_id);

    snprintf_s(ac_p2p_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "p2p%d", pst_mac_device->uc_device_id);

#ifdef _PRE_WLAN_FEATURE_P2P
    if ((pst_wdev->iftype == NL80211_IFTYPE_STATION) || (pst_wdev->iftype == NL80211_IFTYPE_P2P_DEVICE) || (pst_wdev->iftype == NL80211_IFTYPE_AP))
#else
    if ((pst_wdev->iftype == NL80211_IFTYPE_STATION) || (pst_wdev->iftype == NL80211_IFTYPE_AP))
#endif
    {
        if(!g_uc_cfg_once_flag)
        {
            hwifi_config_init_nvram_main_etc(pst_cfg_net_dev);
            hwifi_config_init_ini_main(pst_cfg_net_dev);
            g_uc_cfg_once_flag = OAL_TRUE;
        }
        if(0 == (oal_strcmp(ac_wlan_netdev_name, pst_net_dev->name)))
        {
            hwifi_config_init_ini_wlan(pst_net_dev);
        }
#ifdef _PRE_WLAN_FEATURE_P2P
        else if(0 == (oal_strcmp(ac_p2p_netdev_name, pst_net_dev->name)))
        {
            hwifi_config_init_ini_p2p(pst_net_dev);
        }
#endif
        else
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hwifi_config_init_ini_etc::net_dev is not wlan0 or p2p0!}\r\n");
            return OAL_SUCC;
        }
    }

    return OAL_SUCC;
}

oal_void hwifi_config_init_force_etc(oal_void)
{
    /* ??????????????FALSE */
    g_uc_cfg_once_flag = OAL_FALSE;

    hwifi_config_host_global_ini_param();
}


OAL_STATIC oal_uint32  wal_hipriv_load_ini_power_gain(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;
    oal_uint16                      us_len;

    l_ret = memcpy_s(st_write_msg.auc_value, OAL_STRLEN(pc_param), pc_param, OAL_STRLEN(pc_param));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_load_ini_power_gain::memcpy fail!");
        return OAL_FAIL;
    }

    st_write_msg.auc_value[OAL_STRLEN(pc_param)] = '\0';

    us_len = (oal_uint16)(OAL_STRLEN(pc_param) + 1);

    /* ????txpwr??dbb scale */
    if (hwifi_force_refresh_rf_params(pst_net_dev) != OAL_SUCC)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_refresh_ini_power_gain::refresh rf(max_txpwr & dbb scale) params failed!}");
        return OAL_FAIL;
    }

    /* ????DTS???? */
    if (hwifi_config_init_etc(CUS_TAG_DTS))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_refresh_ini_power_gain::refresh CUS_TAG_DTS params failed!}");
        return OAL_FAIL;
    }


    /***************************************************************************
                              ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_LOAD_INI_PWR_GAIN, us_len);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                             WAL_MSG_TYPE_WRITE,
                             WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                             (oal_uint8 *)&st_write_msg,
                             OAL_FALSE,
                             OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_refresh_ini_power_gain::wal_send_cfg_event_etc return err_code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}

#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

/* E5 SPE module ralation */
#if (defined(CONFIG_BALONG_SPE) && defined(_PRE_WLAN_SPE_SUPPORT))


OAL_STATIC oal_int32 wal_finish_spe_td(oal_int32 l_port_num, oal_netbuf_stru *pst_buf, oal_uint32 ul_flags)
{
    if(OAL_WARN_ON(!spe_hook.port_netdev))
    {
         oal_print_hi11xx_log(HI11XX_LOG_ERR,"%s error: spe_hook.port_netdev is null",__FUNCTION__);
         return OAL_FAIL;
    };

    oal_dma_unmap_single(NULL, pst_buf->dma, pst_buf->len, OAL_TO_DEVICE);

    //if (OAL_UNLIKELY(NULL != pst_net_dev) && OAL_UNLIKELY(NULL != pst_buf))
    if (OAL_UNLIKELY(NULL != pst_buf))
    {
        dev_kfree_skb_any(pst_buf);
    }

    return OAL_SUCC;
}


OAL_STATIC oal_int32 wal_finish_spe_rd(oal_int32 l_port_num, oal_int32 l_src_port_num,
    oal_netbuf_stru *pst_buf, oal_uint32 ul_dma, oal_uint32 ul_flags)
{
    oal_net_device_stru       *pst_net_dev;

    if(OAL_WARN_ON(!spe_hook.port_netdev))
    {
         oal_print_hi11xx_log(HI11XX_LOG_ERR,"%s error: spe_hook.port_netdev is null",__FUNCTION__);
         return OAL_NETDEV_TX_BUSY;
    };

    pst_net_dev = spe_hook.port_netdev(l_port_num);

    oal_dma_unmap_single(NULL, ul_dma, pst_buf->len, OAL_FROM_DEVICE);

    return wal_vap_start_xmit(pst_buf, pst_net_dev);
}


OAL_STATIC oal_int32 wal_netdev_spe_init(oal_net_device_stru *pst_net_dev)
{
    oal_int32             l_ret = 0;
    oal_int32             l_port_num = 0;
    mac_vap_stru          *pst_mac_vap;

    struct spe_port_attr  st_spe_port_attr;

    pst_mac_vap = (mac_vap_stru*)OAL_NET_DEV_PRIV(pst_net_dev);

    memset_s(&st_spe_port_attr, sizeof(struct spe_port_attr), 0, sizeof(struct spe_port_attr));

    /* ????SPE???????? */
    st_spe_port_attr.desc_ops.finish_rd =  wal_finish_spe_rd;
    st_spe_port_attr.desc_ops.finish_td =  wal_finish_spe_td;
    st_spe_port_attr.rd_depth = WAL_MAX_SPE_PKT_NUM;
    st_spe_port_attr.td_depth = WAL_MAX_SPE_PKT_NUM;
    st_spe_port_attr.attach_brg = spe_attach_brg_normal;
    st_spe_port_attr.net = pst_net_dev;
    st_spe_port_attr.rd_skb_num = st_spe_port_attr.rd_depth;
    st_spe_port_attr.rd_skb_size = 2048;
    st_spe_port_attr.enc_type = spe_enc_none;
    st_spe_port_attr.stick_mode = 0;

    /* ????SPE???? */
    l_port_num = wifi_spe_port_alloc(pst_net_dev, &st_spe_port_attr);

    if(0 > l_port_num)
    {
        OAL_IO_PRINT("wal_netdev_spe_init::spe port alloc failed;\n");
        return -OAL_FAIL;
    }
    pst_mac_vap->ul_spe_portnum = (oal_uint32)l_port_num;

    OAL_IO_PRINT("wal_netdev_spe_init::vap_id::%d;port_num::%d\n", pst_mac_vap->uc_vap_id, pst_mac_vap->ul_spe_portnum);

    return l_ret;
}

OAL_STATIC oal_void wal_netdev_spe_exit(oal_net_device_stru *pst_net_dev)
{
    mac_vap_stru          *pst_mac_vap;

    pst_mac_vap = (mac_vap_stru*)OAL_NET_DEV_PRIV(pst_net_dev);

    wifi_spe_port_free(pst_mac_vap->ul_spe_portnum);

    OAL_IO_PRINT("wal_netdev_spe_exit::vap_id::%d, spe port(%d) free.\n", pst_mac_vap->uc_vap_id, pst_mac_vap->ul_spe_portnum);
}
#endif /* defined(CONFIG_BALONG_SPE) && _PRE_WLAN_SPE_SUPPORT */

mac_device_stru *wal_get_macdev_by_netdev(oal_net_device_stru *pst_net_dev)
{
    oal_wireless_dev_stru    *pst_wdev;
    mac_wiphy_priv_stru      *pst_wiphy_priv = OAL_PTR_NULL;

    pst_wdev = OAL_NETDEVICE_WDEV(pst_net_dev);
    if(OAL_PTR_NULL == pst_wdev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_get_macdev_by_netdev::pst_wdev is null!}\r\n");
        return OAL_PTR_NULL;
    }
    pst_wiphy_priv = (mac_wiphy_priv_stru *)(oal_wiphy_priv(pst_wdev->wiphy));
    return pst_wiphy_priv->pst_mac_device;

}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifdef _PRE_WLAN_FEATURE_DFR
OAL_STATIC oal_bool_enum_uint8 wal_dfr_recovery_check(oal_net_device_stru *pst_net_dev)
{
    oal_wireless_dev_stru      *pst_wdev;
    mac_wiphy_priv_stru        *pst_wiphy_priv;
    mac_device_stru            *pst_mac_device;

    if(OAL_PTR_NULL == pst_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_dfr_recovery_check::pst_net_dev is null!}\r\n");
        return OAL_FALSE;
    }

    pst_wdev = OAL_NETDEVICE_WDEV(pst_net_dev);
    if(OAL_PTR_NULL == pst_wdev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_dfr_recovery_check::pst_wdev is null!}\r\n");
        return OAL_FALSE;
    }

    pst_wiphy_priv  = (mac_wiphy_priv_stru *)oal_wiphy_priv(pst_wdev->wiphy);
    if(OAL_PTR_NULL == pst_wiphy_priv)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_dfr_recovery_check::pst_wiphy_priv is null!}\r\n");
        return OAL_FALSE;
    }

    pst_mac_device  = pst_wiphy_priv->pst_mac_device;
    if(OAL_PTR_NULL == pst_mac_device)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_dfr_recovery_check::pst_mac_device is null!}\r\n");
        return OAL_FALSE;
    }

    OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_dfr_recovery_check::recovery_flag:%d, uc_vap_num:%d.}\r\n" ,
                     g_st_dfr_info_etc.bit_ready_to_recovery_flag, pst_mac_device->uc_vap_num);

    if((OAL_TRUE == g_st_dfr_info_etc.bit_ready_to_recovery_flag)
       && (!pst_mac_device->uc_vap_num))
    {
        /* DFR????,??????????VAP????????????????,?????????? */
        return OAL_TRUE;
    }

    return OAL_FALSE;
}
#endif /* #ifdef _PRE_WLAN_FEATURE_DFR */
#endif


OAL_STATIC oal_int32  _wal_netdev_open(oal_net_device_stru *pst_net_dev,oal_uint8 uc_entry_flag, oal_int32 pm_ret)
{
    wal_msg_write_stru      st_write_msg;
    oal_int32               l_ret;
    wal_msg_stru           *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32              ul_err_code;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8 en_p2p_mode;
    oal_wireless_dev_stru   *pst_wdev;
#endif

#if (defined(_PRE_PRODUCT_ID_HI110X_HOST)&&(_PRE_OS_VERSION_WIN32 != _PRE_OS_VERSION))
    oal_int32                ul_check_hw_status = 0;
#endif

#if !defined(_PRE_PRODUCT_ID_HI110X_HOST)
    mac_vap_stru               *pst_mac_vap = OAL_PTR_NULL;
    hmac_device_stru           *pst_hmac_device = OAL_PTR_NULL;
#endif
    oal_netdev_priv_stru       *pst_netdev_priv = OAL_PTR_NULL;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_net_dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_netdev_open_etc::pst_net_dev is null ptr!}\r\n");
        return -OAL_EFAUL;
    }

    OAL_IO_PRINT("wal_netdev_open_etc,dev_name is:%s\n", pst_net_dev->name);
    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_netdev_open_etc::iftype:%d.!}\r\n", pst_net_dev->ieee80211_ptr->iftype);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    netdev_is_open_etc = OAL_TRUE;

    if(OAL_ERR_CODE_ALREADY_OPEN != pm_ret)
    {
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
        /* ??????????????FALSE */
        hwifi_config_init_force_etc();
#endif

        /* ????host device_stru??????*/
        l_ret = wal_host_dev_init_etc(pst_net_dev);
        if(OAL_SUCC != l_ret)
        {
            OAL_IO_PRINT("wal_host_dev_init_etc FAIL %d \r\n",l_ret);
            return -OAL_EFAIL;
        }
    }
#ifdef _PRE_WLAN_FEATURE_DFR
    /* dfr ????????wlan1 */
    else if (wal_dfr_recovery_check(pst_net_dev))
    {
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
        custom_cali_done_etc = OAL_TRUE;
        wal_custom_cali_etc();
        hwifi_config_init_force_etc();
#endif
    }
#endif  /* #ifdef _PRE_WLAN_FEATURE_DFR */

    // ????vap??????,????????????????????????????????device
    l_ret = wal_cfg_vap_h2d_event_etc(pst_net_dev);
    if(OAL_SUCC != l_ret)
    {
        OAL_IO_PRINT("wal_cfg_vap_h2d_event_etc FAIL %d \r\n",l_ret);
        return -OAL_EFAIL;
    }
    OAL_IO_PRINT("wal_cfg_vap_h2d_event_etc succ \r\n");
#endif

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    if(OAL_TRUE == g_st_ap_config_info.l_ap_power_flag)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_netdev_open_etc::power state is on,in ap mode, start vap later.}\r\n");

        /* ?????????????????????? framework??????????????????????up???? */
        g_st_ap_config_info.l_ap_power_flag = OAL_FALSE;
        oal_net_tx_wake_all_queues(pst_net_dev);/*???????????? */
        return OAL_SUCC;
    }

    if (OAL_VALUE_EQ_ANY2(pst_net_dev->ieee80211_ptr->iftype, NL80211_IFTYPE_STATION, NL80211_IFTYPE_P2P_DEVICE))/*????????--????name????*/
    {
        l_ret = wal_init_wlan_vap_etc(pst_net_dev);
        if(OAL_SUCC != l_ret)
        {
            CHR_EXCEPTION_REPORT(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI, CHR_LAYER_DRV, CHR_WIFI_DRV_EVENT_OPEN, CHR_WIFI_DRV_ERROR_POWER_ON);
            return -OAL_EFAIL;
        }
    }
    else if(NL80211_IFTYPE_AP == pst_net_dev->ieee80211_ptr->iftype)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_netdev_open_etc::ap mode,no need to start vap.!}\r\n");
        oal_net_tx_wake_all_queues(pst_net_dev);/*???????????? */
        return OAL_SUCC;
    }
#else
    if((NL80211_IFTYPE_AP == pst_net_dev->ieee80211_ptr->iftype)&&(OAL_TRUE == uc_entry_flag))
    {
        pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
        if(OAL_UNLIKELY(NULL == pst_mac_vap))
        {
            OAM_ERROR_LOG0(0, OAM_SF_CFG, "{_wal_netdev_open::can't get mac vap from netdevice priv data!}");
            return OAL_ERR_CODE_PTR_NULL;
        }

        pst_hmac_device = hmac_res_get_mac_dev_etc(pst_mac_vap->uc_device_id);
        if(OAL_UNLIKELY(NULL == pst_hmac_device))
        {
            OAM_ERROR_LOG0(0, OAM_SF_CFG, "{_wal_netdev_open::hmac device is NULL!}");
            return OAL_ERR_CODE_PTR_NULL;
        }

        if(!pst_hmac_device->en_start_via_priv)
        {
           OAL_NETDEVICE_FLAGS(pst_net_dev) |= OAL_IFF_RUNNING;
           return OAL_SUCC;
        }
    }
#endif

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#if ((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE))
    /* ???????????????? */
    (OAL_TRUE == wlan_pm_switch_etc) ? wlan_pm_enable_etc() : wlan_pm_disable_etc();
#endif // #if ((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE))

    /* ????GCGO MAC ???? */
    if ((wlan_customize_etc.uc_random_mac_addr_connect) &&
        (oal_strncmp("p2p-p2p0", pst_net_dev->name, OAL_STRLEN("p2p-p2p0")) == 0))
    {
        wal_set_random_mac_to_mib_etc(pst_net_dev);
    }

#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    /* ???????? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_START_VAP, OAL_SIZEOF(mac_cfg_start_vap_param_stru));
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->pst_net_dev = pst_net_dev;
#ifdef _PRE_WLAN_FEATURE_P2P
    pst_wdev    = pst_net_dev->ieee80211_ptr;
    en_p2p_mode = wal_wireless_iftype_to_mac_p2p_mode_etc(pst_wdev->iftype);
    if (WLAN_P2P_BUTT == en_p2p_mode)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_netdev_open_etc::wal_wireless_iftype_to_mac_p2p_mode_etc return BUFF}\r\n");
        CHR_EXCEPTION_REPORT(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI, CHR_LAYER_DRV, CHR_WIFI_DRV_EVENT_OPEN, CHR_WIFI_DRV_ERROR_POWER_ON);
        return -OAL_EINVAL;
    }
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->en_p2p_mode = en_p2p_mode;
#endif
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->en_mgmt_rate_init_flag = OAL_TRUE;

    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->l_ifindex = pst_net_dev->ifindex;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_start_vap_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_netdev_open_etc::wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_netdev_open_etc::hmac start vap fail,err code[%u]!}\r\n", ul_err_code);
        return -OAL_EINVAL;
    }

    OAL_NETDEVICE_FLAGS(pst_net_dev) |= OAL_IFF_RUNNING;

    pst_netdev_priv = (oal_netdev_priv_stru *)OAL_NET_DEV_WIRELESS_PRIV(pst_net_dev);
    if (pst_netdev_priv->uc_napi_enable
        && (!pst_netdev_priv->uc_state)
        && (OAL_VALUE_EQ_ANY3(pst_net_dev->ieee80211_ptr->iftype,NL80211_IFTYPE_STATION,NL80211_IFTYPE_P2P_DEVICE,NL80211_IFTYPE_P2P_CLIENT)))
    {
        oal_napi_enable(&pst_netdev_priv->st_napi);
        pst_netdev_priv->uc_state = 1;
    }


    oal_net_tx_wake_all_queues(pst_net_dev);/*???????????? */
#if (defined(_PRE_PRODUCT_ID_HI110X_HOST)&&(_PRE_OS_VERSION_WIN32 != _PRE_OS_VERSION))
    /*1102????fem??DEVICE????HOST????lna????????,????wlan0??????*/
    if((NL80211_IFTYPE_STATION == pst_net_dev->ieee80211_ptr->iftype)
        && (0 == (oal_strcmp("wlan0", pst_net_dev->name))))
    {
        wal_atcmsrv_ioctl_get_fem_pa_status_etc(pst_net_dev,&ul_check_hw_status);
    }
#endif

    return OAL_SUCC;
}

oal_int32  wal_netdev_open_ext(oal_net_device_stru *pst_net_dev)
{
    return  wal_netdev_open_etc(pst_net_dev,OAL_TRUE);
}


oal_int32  wal_netdev_open_etc(oal_net_device_stru *pst_net_dev,oal_uint8 uc_entry_flag)
{
    oal_int32 ret;
    oal_int32 pm_ret;

    if (OAL_NETDEVICE_FLAGS(pst_net_dev) & OAL_IFF_RUNNING)
    {
        return OAL_SUCC;
    }

    pm_ret = wlan_pm_open_etc();
    if (OAL_FAIL == pm_ret)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_netdev_open_etc::wlan_pm_open_etc Fail!}\r\n");
        CHR_EXCEPTION_REPORT(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI, CHR_LAYER_DRV, CHR_WIFI_DRV_EVENT_OPEN, CHR_WIFI_DRV_ERROR_POWER_ON);
        return -OAL_EFAIL;
    }

#ifdef _PRE_WLAN_FEATURE_DFR
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_lock(&g_st_dfr_info_etc.wifi_excp_mutex);
#endif
#endif

    wal_wake_lock();
    ret = _wal_netdev_open(pst_net_dev, uc_entry_flag, pm_ret);
    wal_wake_unlock();

#ifdef _PRE_WLAN_FEATURE_DFR
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_unlock(&g_st_dfr_info_etc.wifi_excp_mutex);
#endif
#endif

#if (defined(_PRE_E5_722_PLATFORM) || defined(_PRE_CPE_711_PLATFORM) || defined(_PRE_CPE_722_PLATFORM))
    wlan_set_driver_lock(true);
#endif
#if (defined(_PRE_PRODUCT_ID_HI110X_HOST) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))) && (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /*????wlan0 ????????*/
    if (oal_strncmp("wlan0", pst_net_dev->name, OAL_STRLEN("wlan0")) == 0)
    {
        g_st_wifi_radio_stat_etc.ull_wifi_on_time_stamp = OAL_TIME_JIFFY;
    }
#endif

    return ret;
}
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE


oal_uint32 wal_custom_cali_etc(oal_void)
{
    oal_net_device_stru *pst_net_dev;
    oal_uint32           ul_ret = 0;

    pst_net_dev = oal_dev_get_by_name("Hisilicon0");  //????cfg vap0????c0 c1????
    if (OAL_WARN_ON(OAL_PTR_NULL == pst_net_dev))
    {
        return OAL_ERR_CODE_PTR_NULL;
    }
    else
    {
        /* ????oal_dev_get_by_name????????????oal_dev_put??net_dev?????????????? */
        oal_dev_put(pst_net_dev);
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_custom_cali_etc::the net_device is already exist!}\r\n");
    }

    hwifi_config_init_nvram_main_etc(pst_net_dev);
    hwifi_config_init_ini_main(pst_net_dev);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (custom_cali_done_etc == OAL_TRUE)
    {
        /* ???????????? */
        wal_send_cali_data_etc(pst_net_dev);
    }
    else
    {
        custom_cali_done_etc = OAL_TRUE;
    }

    wal_send_cali_matrix_data(pst_net_dev);
#endif

    /* ???????? */
    ul_ret = hwifi_config_init_dts_main_etc(pst_net_dev);

    return ul_ret;
}


oal_int32 wal_set_custom_process_func_etc(oal_void)
{
#if (_PRE_PRODUCT_ID != _PRE_PRODUCT_ID_HI1151)
    struct custom_process_func_handler* pst_custom_process_func_handler;
    pst_custom_process_func_handler = oal_get_custom_process_func_etc();
    if(OAL_PTR_NULL == pst_custom_process_func_handler)
    {
        OAM_ERROR_LOG0(0,OAM_SF_ANY,"{hmac_set_auto_freq_process_func get handler failed!}");
    }
    else
    {
        pst_custom_process_func_handler->p_custom_cali_func = wal_custom_cali_etc;
    }
#else
    wal_custom_cali_etc();
#endif
    return OAL_SUCC;
}
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */


OAL_STATIC oal_int32  _wal_netdev_stop(oal_net_device_stru *pst_net_dev)
{
    wal_msg_write_stru          st_write_msg;
    oal_uint32                  ul_err_code;
    wal_msg_stru               *pst_rsp_msg = OAL_PTR_NULL;
    oal_int32                   l_ret;
    mac_vap_stru               *pst_mac_vap = OAL_PTR_NULL;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8    en_p2p_mode;
    oal_wireless_dev_stru      *pst_wdev;
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    mac_device_stru            *pst_mac_device;
#endif
#endif
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    mac_device_stru          *pst_mac_dev;
    oal_int8                 ac_wlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH]  = {0};
    oal_int8                 ac_p2p_netdev_name[MAC_NET_DEVICE_NAME_LENGTH]   = {0};
    oal_int8                 ac_p2p_group_name[MAC_NET_DEVICE_NAME_LENGTH]    = {0};
    oal_int8                 ac_hwlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH] = {0};
    oal_netdev_priv_stru     *pst_netdev_priv;
#endif

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_net_dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_netdev_stop_etc::pst_net_dev is null ptr!}\r\n");
        return -OAL_EFAUL;
    }

    /*stop the netdev's queues*/
    oal_net_tx_stop_all_queues(pst_net_dev);/* ???????????? */
    wal_force_scan_complete_etc(pst_net_dev, OAL_TRUE);

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    OAL_IO_PRINT("wal_netdev_stop_etc,dev_name is:%s\n", pst_net_dev->name);
    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_netdev_stop_etc::iftype:%d.!}", pst_net_dev->ieee80211_ptr->iftype);

    /* AP??????,????????????down?????? vap */
    if(NL80211_IFTYPE_AP == pst_net_dev->ieee80211_ptr->iftype)
    {
        l_ret = wal_netdev_stop_ap_etc(pst_net_dev);
        return l_ret;
    }
#endif

    /* ????netdev????running???????????????????? */
    if (0 == (OAL_NETDEVICE_FLAGS(pst_net_dev) & OAL_IFF_RUNNING))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{_wal_netdev_stop::vap is already down!}");
        return OAL_SUCC;
    }

    /* ????netdev??mac vap???????????????????????? */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{_wal_netdev_stop:: mac vap of netdevice is down!iftype:[%d]}", pst_net_dev->ieee80211_ptr->iftype);
        return OAL_SUCC;
    }

    if (MAC_VAP_STATE_INIT == pst_mac_vap->en_vap_state)
    {
        OAM_WARNING_LOG3(0, OAM_SF_ANY, "{_wal_netdev_stop::vap is already down!iftype:[%d] vap mode[%d] p2p mode[%d]!}",
            pst_net_dev->ieee80211_ptr->iftype, pst_mac_vap->en_vap_mode, pst_mac_vap->en_p2p_mode);
        if(WLAN_P2P_DEV_MODE != pst_mac_vap->en_p2p_mode)
        {
            return OAL_SUCC;
        }
    }

    /***************************************************************************
                           ????????wal??????
    ***************************************************************************/
    /* ????WID???? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DOWN_VAP, OAL_SIZEOF(mac_cfg_down_vap_param_stru));
    ((mac_cfg_down_vap_param_stru *)st_write_msg.auc_value)->pst_net_dev = pst_net_dev;
#ifdef _PRE_WLAN_FEATURE_P2P
    pst_wdev    = pst_net_dev->ieee80211_ptr;
    en_p2p_mode = wal_wireless_iftype_to_mac_p2p_mode_etc(pst_wdev->iftype);
    if (WLAN_P2P_BUTT == en_p2p_mode)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_netdev_stop_etc::wal_wireless_iftype_to_mac_p2p_mode_etc return BUFF}\r\n");
        return -OAL_EINVAL;
    }
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->en_p2p_mode = en_p2p_mode;
#endif

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_down_vap_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        if (OAL_PTR_NULL == OAL_NET_DEV_PRIV(pst_net_dev))
        {
            /* ????net_device????????????vap ??null??????flags running???? */
            OAL_NETDEVICE_FLAGS(pst_net_dev) &= (~OAL_IFF_RUNNING);
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_netdev_stop_etc::net_device's vap is null, set flag not running, if_idx:%d}", pst_net_dev->ifindex);
        }
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_netdev_stop_etc::wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_netdev_stop_etc::hmac stop vap fail!err code [%d]}\r\n", ul_err_code);
        return -OAL_EFAIL;
    }

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    pst_mac_dev = wal_get_macdev_by_netdev(pst_net_dev);
    if(OAL_PTR_NULL == pst_mac_dev)
    {
        OAL_IO_PRINT("wal_deinit_wlan_vap_etc::wal_get_macdev_by_netdev FAIL\r\n");
        return -OAL_EFAIL;
    }
        /* ????device id????netdev???? */
    snprintf_s(ac_wlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
                       "wlan%d", pst_mac_dev->uc_device_id);

    snprintf_s(ac_p2p_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
                       "p2p%d",  pst_mac_dev->uc_device_id);

    snprintf_s(ac_p2p_group_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
                       "p2p-p2p%d", pst_mac_dev->uc_device_id);

    snprintf_s(ac_hwlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
                       "hwlan%d",  pst_mac_dev->uc_device_id);

    if ((NL80211_IFTYPE_STATION == pst_net_dev->ieee80211_ptr->iftype)
        && ((0 == oal_strcmp(ac_wlan_netdev_name, pst_net_dev->name)) || (0 == oal_strcmp(ac_hwlan_netdev_name, pst_net_dev->name))))
    {
        l_ret = wal_deinit_wlan_vap_etc(pst_net_dev);
        if (OAL_SUCC != l_ret)
        {
            return l_ret;
        }
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    /* p2p0 down?? ????VAP */
    //????????????p2p group(GC or GO)????????wal_del_p2p_group_etc????p2p group??vap??????work queue????GC or GO netdevice
    //??????????????????p2p group????????????????p2p device??????????p2p group????????????wlan0??????????p2p group
    if ((NL80211_IFTYPE_STATION == pst_net_dev->ieee80211_ptr->iftype)
        && (0 == oal_strcmp(ac_p2p_netdev_name, pst_net_dev->name)))
        //|| ((NL80211_IFTYPE_P2P_CLIENT == pst_net_dev->ieee80211_ptr->iftype || NL80211_IFTYPE_P2P_GO == pst_net_dev->ieee80211_ptr->iftype)
        //&& (0 == oal_strncmp(ac_p2p_group_name, pst_net_dev->name, sizeof("p2p-p2p0") - 1))))
    {
        /* ????????p2p???? */
        pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
        if(OAL_PTR_NULL == pst_mac_vap)
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_netdev_stop_etc::pst_mac_vap is null, netdev released.}\r\n");
            return OAL_SUCC;
        }
        pst_mac_device = mac_res_get_dev_etc(pst_mac_vap->uc_device_id);
        if(OAL_PTR_NULL != pst_mac_device)
        {
            wal_del_p2p_group_etc(pst_mac_device);
        }
    }

    if (((pst_net_dev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION) || (pst_net_dev->ieee80211_ptr->iftype == NL80211_IFTYPE_P2P_DEVICE)) &&
        (0 == oal_strcmp(ac_p2p_netdev_name, pst_net_dev->name)))
    {
        l_ret = wal_deinit_wlan_vap_etc(pst_net_dev);
        if (OAL_SUCC != l_ret)
        {
            return l_ret;
        }
    }
#endif
#endif

    pst_netdev_priv = (oal_netdev_priv_stru *)OAL_NET_DEV_WIRELESS_PRIV(pst_net_dev);
    if (pst_netdev_priv->uc_napi_enable &&
        ((NL80211_IFTYPE_STATION == pst_net_dev->ieee80211_ptr->iftype) ||
        (NL80211_IFTYPE_P2P_DEVICE == pst_net_dev->ieee80211_ptr->iftype) ||
        (NL80211_IFTYPE_P2P_CLIENT == pst_net_dev->ieee80211_ptr->iftype)))
    {
    #ifndef WIN32
        oal_netbuf_list_purge(&pst_netdev_priv->st_rx_netbuf_queue);
    #endif
        oal_napi_disable(&pst_netdev_priv->st_napi);
        pst_netdev_priv->uc_state = 0;
    }

    return OAL_SUCC;
}

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)

OAL_STATIC oal_int32  wal_set_power_on(oal_net_device_stru *pst_net_dev, oal_int32 power_flag)
{
    oal_int32    l_ret = 0;

    //ap????????????VAP
    if (0 == power_flag)//????
    {
        /* ????host device_stru????????*/
        wal_host_dev_exit(pst_net_dev);

        wal_wake_lock();
        wlan_pm_close_etc();
        wal_wake_unlock();

        g_st_ap_config_info.l_ap_power_flag = OAL_FALSE;
    }
    else if (1 == power_flag) //????
    {
        g_st_ap_config_info.l_ap_power_flag = OAL_TRUE;

        wal_wake_lock();
        l_ret = wlan_pm_open_etc();
        wal_wake_unlock();
        if (OAL_FAIL==l_ret)
        {
             OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_set_power_on::wlan_pm_open_etc Fail!}\r\n");
             return -OAL_EFAIL;
        }
        else if(OAL_ERR_CODE_ALREADY_OPEN != l_ret)
        {
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
            /* ??????????????FALSE */
            hwifi_config_init_force_etc();
#endif
            // ??????????????????????VAP
            l_ret = wal_cfg_vap_h2d_event_etc(pst_net_dev);
            if(OAL_SUCC!= l_ret)
            {
                return -OAL_EFAIL;
            }
        }

        /* ????host device_stru??????*/
        l_ret = wal_host_dev_init_etc(pst_net_dev);
        if(OAL_SUCC != l_ret)
        {
            OAL_IO_PRINT("wal_set_power_on FAIL %d \r\n",l_ret);
            return -OAL_EFAIL;
        }
    }
    else
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_set_power_on::pupower_flag:%d error.}\r\n", power_flag);
        return -OAL_EFAIL;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_void wal_set_power_mgmt_on(oal_uint power_mgmt_flag)
{
    struct wlan_pm_s  *pst_wlan_pm;
    pst_wlan_pm = wlan_pm_get_drv_etc();
    if(NULL != pst_wlan_pm)
    {
        /* ap????????????????????????,1:????,0:?????? */
        pst_wlan_pm->ul_apmode_allow_pm_flag = power_mgmt_flag;
    }
    else
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_set_power_mgmt_on::wlan_pm_get_drv_etc return null.");
    }

}


oal_int32 wal_netdev_stop_ap_etc(oal_net_device_stru *pst_net_dev)
{
    oal_int32          l_ret;

    if (NL80211_IFTYPE_AP != pst_net_dev->ieee80211_ptr->iftype)
    {
        return OAL_SUCC;
    }

    /* ????????,??????20/40M??????????????AP */
    wal_force_scan_complete_etc(pst_net_dev, OAL_TRUE);

    /* AP??????????STA????,????????vap */
    if(OAL_SUCC != wal_stop_vap_etc(pst_net_dev))
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_netdev_stop_ap_etc::wal_stop_vap_etc enter a error.}");
    }
    l_ret = wal_deinit_wlan_vap_etc(pst_net_dev);
    if(OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_netdev_stop_ap_etc::wal_deinit_wlan_vap_etc enter a error.}");
        return l_ret;
    }

    /* Del aput??????????netdev iftype??????station */
    pst_net_dev->ieee80211_ptr->iftype = NL80211_IFTYPE_STATION;

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    /* aput???? */
    wal_set_power_mgmt_on(OAL_TRUE);
    l_ret = wal_set_power_on(pst_net_dev, OAL_FALSE);
    if (OAL_SUCC != l_ret)
    {
        OAM_ERROR_LOG1(0, OAM_SF_CFG, "{wal_netdev_stop_ap_etc::wal_set_power_on fail [%d]!}", l_ret);
        return l_ret;
    }
#endif /* #if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */
    return OAL_SUCC;

}
#endif

oal_int32  wal_netdev_stop_etc(oal_net_device_stru *pst_net_dev)
{
    oal_int32 ret;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_net_dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_netdev_stop_etc::pst_net_dev is null ptr!}\r\n");
        return -OAL_EFAUL;
    }

#ifdef _PRE_WLAN_FEATURE_DFR
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_lock(&g_st_dfr_info_etc.wifi_excp_mutex);
#endif
#endif

    wal_wake_lock();
    ret = _wal_netdev_stop(pst_net_dev);
    wal_wake_unlock();

    if(OAL_SUCC == ret)
    {
        OAL_NETDEVICE_FLAGS(pst_net_dev) &= ~OAL_IFF_RUNNING;
    }

#ifdef _PRE_WLAN_FEATURE_DFR
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    mutex_unlock(&g_st_dfr_info_etc.wifi_excp_mutex);
#endif
#endif

#if (defined(_PRE_E5_722_PLATFORM) || defined(_PRE_CPE_711_PLATFORM) || defined(_PRE_CPE_722_PLATFORM))
    wlan_set_driver_lock(false);
#endif

    return ret;
}


OAL_STATIC oal_net_device_stats_stru*  wal_netdev_get_stats(oal_net_device_stru *pst_net_dev)
{
    oal_net_device_stats_stru  *pst_stats = &(pst_net_dev->stats);
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    mac_vap_stru               *pst_mac_vap;
    oam_stat_info_stru         *pst_oam_stat;

    oam_vap_stat_info_stru     *pst_oam_vap_stat;

    pst_mac_vap  = OAL_NET_DEV_PRIV(pst_net_dev);
    pst_oam_stat = OAM_STAT_GET_STAT_ALL();

    if(NULL == pst_mac_vap)
    {
        return pst_stats;
    }

    if(pst_mac_vap->uc_vap_id >= WLAN_VAP_SUPPORT_MAX_NUM_LIMIT)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "wal_netdev_get_stats error vap id %u", pst_mac_vap->uc_vap_id);
        return pst_stats;
    }

    pst_oam_vap_stat = &(pst_oam_stat->ast_vap_stat_info[pst_mac_vap->uc_vap_id]);

    /* ??????????????net_device */
    pst_stats->rx_packets   = pst_oam_vap_stat->ul_rx_pkt_to_lan;
    pst_stats->rx_bytes     = pst_oam_vap_stat->ul_rx_bytes_to_lan;


    pst_stats->tx_packets   = pst_oam_vap_stat->ul_tx_pkt_num_from_lan;
    pst_stats->tx_bytes     = pst_oam_vap_stat->ul_tx_bytes_from_lan;
#endif
    return pst_stats;
}

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE

OAL_STATIC oal_uint32  wal_netdev_update_mac_addr(oal_net_device_stru *pst_net_dev, oal_sockaddr_stru *pst_mac_addr)
{
    oal_wireless_dev_stru    *pst_wifidev = (oal_wireless_dev_stru *)pst_net_dev->ieee80211_ptr;
    enum nl80211_iftype       en_iftype = NL80211_IFTYPE_MAX;

    if(wlan_customize_etc.uc_random_mac_addr_connect)
    {
        if(oal_strncmp("wlan0", pst_net_dev->name, OAL_STRLEN("wlan0")) == 0)
        {
            if(NL80211_IFTYPE_STATION == pst_wifidev->iftype)
            {
                oal_set_mac_addr(g_auc_wifistamac_etc, (oal_uint8 *)(pst_mac_addr->sa_data));
                en_iftype = NL80211_IFTYPE_STATION;
            }
            else
            {
                oal_set_mac_addr(g_auc_wifiapmac_etc, (oal_uint8 *)(pst_mac_addr->sa_data));
                en_iftype = NL80211_IFTYPE_AP;
            }
        }
        else if (oal_strncmp("p2p0", pst_net_dev->name, OAL_STRLEN("p2p0")) == 0)
        {
            oal_set_mac_addr(g_auc_wifip2p0mac_etc, (oal_uint8 *)(pst_mac_addr->sa_data));
            en_iftype = NL80211_IFTYPE_P2P_DEVICE;
        }
        else if (oal_strncmp("p2p-p2p0", pst_net_dev->name, OAL_STRLEN("p2p-p2p0")) == 0)
        {
            oal_set_mac_addr(g_auc_wifiGCGOmac_etc, (oal_uint8 *)(pst_mac_addr->sa_data));
            en_iftype = NL80211_IFTYPE_P2P_CLIENT;
        }
        else
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_netdev_update_mac_addr:: unknown netdevice!}\r\n");
            return OAL_FAIL;
        }
    }

    OAM_WARNING_LOG4(0, OAM_SF_ANY, "{wal_netdev_update_mac_addr:: update iftype = [%d],set macaddr: %02X:XX:XX:XX:%02X:%02X!}\r\n",
                                      en_iftype,pst_mac_addr->sa_data[0], pst_mac_addr->sa_data[4], pst_mac_addr->sa_data[5]);
    return OAL_SUCC;
}
#endif


OAL_STATIC oal_int32  _wal_netdev_set_mac_addr(oal_net_device_stru *pst_net_dev, void *p_addr)
{
    oal_uint32                    ul_ret = 0;
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    oal_sockaddr_stru            *pst_mac_addr = OAL_PTR_NULL;
#if defined(_PRE_WLAN_FEATURE_SMP_SUPPORT) || (defined(CONFIG_BALONG_SPE) && defined(_PRE_WLAN_SPE_SUPPORT))
    mac_vap_stru                 *pst_mac_vap = OAL_PTR_NULL;
#endif

#else
    oal_sockaddr_stru            *pst_mac_addr = OAL_PTR_NULL;
#if defined(_PRE_WLAN_FEATURE_SMP_SUPPORT) || (defined(CONFIG_BALONG_SPE) && defined(_PRE_WLAN_SPE_SUPPORT))
    mac_vap_stru                 *pst_mac_vap = OAL_PTR_NULL;
#endif
    frw_event_mem_stru           *pst_event_mem = OAL_PTR_NULL;
    wal_msg_stru                 *pst_cfg_msg = OAL_PTR_NULL;
    wal_msg_write_stru           *pst_write_msg = OAL_PTR_NULL;
    mac_cfg_staion_id_param_stru *pst_param = OAL_PTR_NULL;
#ifdef _PRE_WLAN_FEATURE_P2P
    oal_wireless_dev_stru        *pst_wdev = OAL_PTR_NULL; /* ????P2P ??????p2p0 ?? p2p-p2p0 MAC ??????wlan0 ???? */
#endif
#endif

    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR2(pst_net_dev,p_addr)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::pst_net_dev or p_addr null ptr error %x, %x!}\r\n", (uintptr_t)pst_net_dev, (uintptr_t)p_addr);

        return -OAL_EFAUL;
    }

    if (oal_netif_running(pst_net_dev))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::cannot set address; device running!}\r\n");

        return -OAL_EBUSY;
    }
    /*lint +e774*//*lint +e506*/

    pst_mac_addr = (oal_sockaddr_stru *)p_addr;

    if (ETHER_IS_MULTICAST(pst_mac_addr->sa_data))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::can not set group/broadcast addr!}\r\n");

        return -OAL_EINVAL;
    }

    oal_set_mac_addr((oal_uint8 *)(pst_net_dev->dev_addr), (oal_uint8 *)(pst_mac_addr->sa_data));

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    ul_ret = wal_netdev_update_mac_addr(pst_net_dev,pst_mac_addr);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        return -OAL_ENOMEM;
    }
#endif

#if (defined(CONFIG_BALONG_SPE) && defined(_PRE_WLAN_SPE_SUPPORT))
    /* mac??????????????????spe??????port?? ????spe?????????????? */
    if(spe_hook.is_enable && spe_hook.is_enable() && (spe_mode_normal == spe_hook.mode()))
    {
        pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_net_dev);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "{_wal_netdev_set_mac_addr::OAL_NET_DEV_PRIV(pst_net_dev) is null ptr.}\r\n");
            return -OAL_EINVAL;
        }
        spe_hook.port_ioctl(pst_mac_vap->ul_spe_portnum, spe_port_ioctl_set_mac, (int)pst_net_dev->dev_addr);
    }
#endif

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    return OAL_SUCC;
#else

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    ul_ret = wal_alloc_cfg_event_etc(pst_net_dev, &pst_event_mem, NULL, &pst_cfg_msg, (WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_staion_id_param_stru)));     /* ???????? */
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::wal_alloc_cfg_event_etc fail!err code[%u]}\r\n",ul_ret);
        return -OAL_ENOMEM;
    }

    /* ???????????? */
    WAL_CFG_MSG_HDR_INIT(&(pst_cfg_msg->st_msg_hdr),
                         WAL_MSG_TYPE_WRITE,
                         WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_staion_id_param_stru),
                         WAL_GET_MSG_SN());

    /* ????WID???? */
    pst_write_msg = (wal_msg_write_stru *)pst_cfg_msg->auc_msg_data;
    WAL_WRITE_MSG_HDR_INIT(pst_write_msg, WLAN_CFGID_STATION_ID, OAL_SIZEOF(mac_cfg_staion_id_param_stru));

    pst_param = (mac_cfg_staion_id_param_stru *)pst_write_msg->auc_value;   /* ????WID?????????? */
    oal_set_mac_addr(pst_param->auc_station_id, (oal_uint8 *)(pst_mac_addr->sa_data));
#ifdef _PRE_WLAN_FEATURE_P2P
    /* ????????net_device ????p2p ???? */
    pst_wdev = (oal_wireless_dev_stru *)pst_net_dev->ieee80211_ptr;
    pst_param->en_p2p_mode = wal_wireless_iftype_to_mac_p2p_mode_etc(pst_wdev->iftype);
    if (WLAN_P2P_BUTT == pst_param->en_p2p_mode)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_netdev_set_mac_addr::wal_wireless_iftype_to_mac_p2p_mode_etc return BUFF}\r\n");
        FRW_EVENT_FREE(pst_event_mem);
        return -OAL_EINVAL;
    }
#endif

#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
    pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{_wal_netdev_set_mac_addr::OAL_NET_DEV_PRIV(pst_net_dev) is null ptr.}\r\n");
        FRW_EVENT_FREE(pst_event_mem);
        return -OAL_EINVAL;
    }

    frw_event_post_event_etc(pst_event_mem, pst_mac_vap->ul_core_id);
#else
    frw_event_dispatch_event_etc(pst_event_mem);
#endif

    FRW_EVENT_FREE(pst_event_mem);

    return OAL_SUCC;
#endif
}

OAL_STATIC oal_int32  wal_netdev_set_mac_addr(oal_net_device_stru *pst_net_dev, void *p_addr)
{
    oal_int32 ret;
    wal_wake_lock();
    ret = _wal_netdev_set_mac_addr(pst_net_dev, p_addr);
    wal_wake_unlock();

    if(ret != OAL_SUCC)
    {
        CHR_EXCEPTION_REPORT(CHR_PLATFORM_EXCEPTION_EVENTID, CHR_SYSTEM_WIFI, CHR_LAYER_DRV, CHR_WIFI_DRV_EVENT_OPEN, CHR_WIFI_DRV_ERROR_POWER_ON_SET_MAC_ADDR);
    }

    return ret;
}


OAL_STATIC oal_int32 wal_ioctl_judge_input_param_length_etc(wal_wifi_priv_cmd_stru st_priv_cmd, oal_uint32 ul_cmd_length, oal_uint16 us_adjust_val)
{
    /* ????+1?? ?????????????????????????????????? */
    if (st_priv_cmd.ul_total_len < ul_cmd_length + 1 + us_adjust_val)
    {
        /* ??????????????????, ??????????pc_command????????????wal_vendor_priv_cmd_etc?????? */
        return -OAL_EFAIL;
    }
    return OAL_SUCC;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)

OAL_STATIC oal_int32 wal_ioctl_priv_cmd_tx_power_etc(oal_net_device_stru * pst_net_dev, oal_int8 * pc_command, wal_wifi_priv_cmd_stru st_priv_cmd)
{
    oal_uint32   ul_skip            = CMD_TX_POWER_LEN + 1;
    oal_uint16   us_txpwr;
    oal_int32    l_ret              = 0;

    l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_TX_POWER_LEN, 1);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_tx_power_etc::length is too short! at least need [%d]!}\r\n", CMD_TX_POWER_LEN + 2);
        return -OAL_EFAIL;
    }

    us_txpwr = (oal_uint16)oal_atoi(pc_command + ul_skip);
    l_ret = wal_ioctl_reduce_sar(pst_net_dev, us_txpwr);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_tx_power_etc::return err code [%d]!}\r\n", l_ret);
        /* ??????????????????????????????supplicant ????4?? ioctl????????wifi???????? */
        return OAL_SUCC;
    }
    return l_ret;

}

#endif

#ifdef _PRE_WLAN_FEATURE_LTECOEX

OAL_STATIC oal_int32 wal_ioctl_priv_cmd_ltecoex_mode_etc(oal_net_device_stru * pst_net_dev, oal_int8 * pc_command, wal_wifi_priv_cmd_stru st_priv_cmd)
{
    oal_int32      l_ret;
    oal_int8       ltecoex_mode;

    /* ????:LTECOEX_MODE 1 or LTECOEX_MODE 0 */
    l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_LTECOEX_MODE_LEN, 1);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_ltecoex_mode_etc::length is too short! at least need [%d]!}\r\n", CMD_LTECOEX_MODE_LEN + 2);
        return -OAL_EFAIL;
    }

    ltecoex_mode = oal_atoi(pc_command + CMD_LTECOEX_MODE_LEN + 1);

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_ltecoex_mode_etc::CMD_LTECOEX_MODE command,ltecoex mode:%d}\r\n", ltecoex_mode);

    l_ret = (oal_int32)wal_ioctl_ltecoex_mode_set(pst_net_dev, (oal_int8*)&ltecoex_mode);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_ltecoex_mode_etc::return err code [%d]!}\r\n", l_ret);
        return -OAL_EFAIL;
    }
    return l_ret;

}
#endif


OAL_STATIC oal_int32 wal_ioctl_priv_cmd_country_etc(oal_net_device_stru * pst_net_dev, oal_int8 * pc_command, wal_wifi_priv_cmd_stru st_priv_cmd)
{
#ifdef _PRE_WLAN_FEATURE_11D
    const oal_int8 *country_code = OAL_PTR_NULL;
    oal_int8        auc_country_code[3] = {0};
    oal_int32       l_ret;

    /* ????:COUNTRY CN */
    l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_COUNTRY_LEN, 2);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_country_etc::length is too short! at least need [%d]!}\r\n", CMD_COUNTRY_LEN + 3);
		return -OAL_EFAIL;
    }

    country_code = pc_command + CMD_COUNTRY_LEN + 1;
    if (EOK != memcpy_s(auc_country_code, OAL_SIZEOF(auc_country_code), country_code, 2)) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_ioctl_priv_cmd_country_etc::memcpy fail!");
        return -OAL_EFAIL;
    }

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    if (OAL_TRUE == cust_country_code_ignore_flag.en_country_code_ingore_ini_flag)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_country_etc::wlan_pm_set_country is ignore, ini[%d]",
                         cust_country_code_ignore_flag.en_country_code_ingore_ini_flag);

        return OAL_SUCC;
    }
#endif

    l_ret = wal_regdomain_update_for_dfs_etc(pst_net_dev, auc_country_code);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_country_etc::return err code [%d]!}\r\n", l_ret);

        return -OAL_EFAIL;
    }

    l_ret = wal_regdomain_update_etc(pst_net_dev, auc_country_code);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_country_etc::return err code [%d]!}\r\n", l_ret);

        return -OAL_EFAIL;
    }

#else
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_country_etc::_PRE_WLAN_FEATURE_11D is not define!}\r\n");
#endif

    return OAL_SUCC;
    }

OAL_STATIC oal_int32 wal_netdev_set_power_on(oal_net_device_stru *pst_net_dev, oal_int32 power_flag)
{
    oal_int32    l_ret    = 0;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_netdev_set_power_on::CMD_SET_POWER_ON command,power flag:%d}\r\n", power_flag);

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    //ap????????????VAP
    if (0 == power_flag)//????
    {
        /* ????host device_stru????????*/
        wal_host_dev_exit(pst_net_dev);

        wal_wake_lock();
        wlan_pm_close_etc();
        wal_wake_unlock();

        g_st_ap_config_info.l_ap_power_flag = OAL_FALSE;
    }
    else if (1 == power_flag) //????
    {
        g_st_ap_config_info.l_ap_power_flag = OAL_TRUE;

        wal_wake_lock();
        l_ret = wlan_pm_open_etc();
        wal_wake_unlock();
        if (OAL_FAIL==l_ret)
        {
             OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_netdev_set_power_on::wlan_pm_open_etc Fail!}\r\n");
             return -OAL_EFAIL;
        }
        else if(OAL_ERR_CODE_ALREADY_OPEN != l_ret)
        {
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
            /* ??????????????FALSE */
            hwifi_config_init_force_etc();
#endif
            // ??????????????????????VAP
            l_ret = wal_cfg_vap_h2d_event_etc(pst_net_dev);
            if(OAL_SUCC!= l_ret)
            {
                return -OAL_EFAIL;
            }

            /* ????host device_stru??????*/
            l_ret = wal_host_dev_init_etc(pst_net_dev);
            if(OAL_SUCC != l_ret)
            {
                OAL_IO_PRINT("wal_host_dev_init_etc FAIL %d \r\n",l_ret);
                return -OAL_EFAIL;
            }
        }
    }
    else
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_netdev_set_power_on::pupower_flag:%d error.}\r\n", power_flag);
        return -OAL_EFAIL;
    }
#endif

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_STA_PM
OAL_STATIC oal_int32 wal_ioctl_set_sta_pm_on(oal_net_device_stru *pst_net_dev,oal_uint8* pc_command, wal_wifi_priv_cmd_stru st_priv_cmd)
{
    wal_msg_write_stru                  st_write_msg;
    mac_cfg_ps_open_stru               *pst_sta_pm_open;
    oal_int32                           l_ret;

    l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_SET_STA_PM_ON_LEN, 1);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_ioctl_set_sta_pm_on::CMD_SET_STA_PM_ON puc_command len error.}\r\n");
        return -OAL_EFAIL;
    }

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_STA_PM_ON, OAL_SIZEOF(mac_cfg_ps_open_stru));

    /* ???????????????? */
    pst_sta_pm_open = (mac_cfg_ps_open_stru *)(st_write_msg.auc_value);
    /* MAC_STA_PM_SWITCH_ON / MAC_STA_PM_SWITCH_OFF */
    pst_sta_pm_open->uc_pm_enable      = *(pc_command + CMD_SET_STA_PM_ON_LEN + 1) - '0';
    pst_sta_pm_open->uc_pm_ctrl_type   = MAC_STA_PM_CTRL_TYPE_HOST;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_ps_open_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_set_sta_pm_on::CMD_SET_STA_PM_ON return err code [%d]!}\r\n", l_ret);
        return -OAL_EFAIL;
    }

    return OAL_SUCC;
}
#endif


OAL_STATIC oal_int32 wal_ioctl_priv_cmd_set_ap_wps_p2p_ie_etc(oal_net_device_stru * pst_net_dev, oal_int8 * pc_command, wal_wifi_priv_cmd_stru st_priv_cmd)
{
    oal_uint32 ul_skip = CMD_SET_AP_WPS_P2P_IE_LEN + 1;
    oal_app_ie_stru *pst_wps_p2p_ie = OAL_PTR_NULL;
    oal_int32    l_ret              = 0;

    /* ????????????????????????????????????????????oal_app_ie_stru?????????????? */
    l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_SET_AP_WPS_P2P_IE_LEN, (OAL_SIZEOF(oal_app_ie_stru) - (OAL_SIZEOF(oal_uint8) * WLAN_WPS_IE_MAX_SIZE)));
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_set_ap_wps_p2p_ie_etc::length is too short! at least need [%d]!}\r\n",
                            (ul_skip + OAL_SIZEOF(oal_app_ie_stru) - (OAL_SIZEOF(oal_uint8) * WLAN_WPS_IE_MAX_SIZE)));
        return -OAL_EFAIL;
    }

    pst_wps_p2p_ie = (oal_app_ie_stru *)(pc_command + ul_skip);

    /*lint -e413*/
    if((ul_skip + pst_wps_p2p_ie->ul_ie_len + OAL_OFFSET_OF(oal_app_ie_stru, auc_ie)) > (oal_uint32)st_priv_cmd.ul_total_len)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_ioctl_priv_cmd_set_ap_wps_p2p_ie_etc::SET_AP_WPS_P2P_IE param len is too short. need %d.}\r\n",(ul_skip + pst_wps_p2p_ie->ul_ie_len));
        return -OAL_EFAIL;
    }
    /*lint +e413*/
    l_ret = wal_ioctl_set_wps_p2p_ie_etc(pst_net_dev,
                                        pst_wps_p2p_ie->auc_ie,
                                        pst_wps_p2p_ie->ul_ie_len,
                                        pst_wps_p2p_ie->en_app_ie_type);

    return l_ret;
}


oal_int32 wal_vendor_get_ap_bandwidth_etc(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr)
{
    mac_vap_stru    *pst_mac_vap;
    oal_uint8        uc_current_bandwidth;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if ((OAL_PTR_NULL == pst_mac_vap) || (WLAN_VAP_MODE_BSS_AP != pst_mac_vap->en_vap_mode))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "wal_vendor_get_ap_bandwidth_etc::vap not find or is not ap mode");
        return -OAL_EFAIL;
    }

    if (WLAN_BAND_WIDTH_20M == pst_mac_vap->st_channel.en_bandwidth)
    {
        uc_current_bandwidth = WLAN_AP_BANDWIDTH_20M;
    }
    else if (WLAN_BAND_WIDTH_40MINUS >= pst_mac_vap->st_channel.en_bandwidth)
    {
        uc_current_bandwidth = WLAN_AP_BANDWIDTH_40M;
    }
    else if (WLAN_BAND_WIDTH_80MINUSMINUS >= pst_mac_vap->st_channel.en_bandwidth)
    {
        uc_current_bandwidth = WLAN_AP_BANDWIDTH_80M;
    }
#ifdef _PRE_WLAN_FEATURE_160M
    else if (WLAN_BAND_WIDTH_160MINUSMINUSMINUS >= pst_mac_vap->st_channel.en_bandwidth)
    {
        uc_current_bandwidth = WLAN_AP_BANDWIDTH_160M;
    }
#endif
    else
    {
        return -OAL_EFAIL;
    }

    oal_copy_to_user(pst_ifr->ifr_data + 8, &uc_current_bandwidth, OAL_SIZEOF(oal_uint8));

    return OAL_SUCC;
}


oal_int32 wal_vendor_get_160m_supported_etc(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr)
{
    oal_int32 l_priv_value = 0;
    oal_uint8 uc_val;

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    hwifi_get_init_priv_value(WLAN_CFG_APUT_160M_ENABLE, &l_priv_value);
#endif

    uc_val = (oal_uint8)l_priv_value;

    oal_copy_to_user(pst_ifr->ifr_data + 8, &uc_val, OAL_SIZEOF(oal_uint8));

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_NRCOEX
/*
 * ?? ?? ??  : wal_vendor_set_nrcoex_priority_etc
 * ????????  : NRCOEX??????????????????????
 */
oal_int32 wal_vendor_set_nrcoex_priority_etc(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr, oal_int8 *pc_command)
{
    oal_int32      l_ret;
    oal_int8       nrcoex_priority;

    /* ????:SET_CELLCOEX_PRIOR 0/1/2/3 */
    if(OAL_STRLEN(pc_command) < (OAL_STRLEN((oal_int8 *)CMD_SET_NRCOEX_PRIOR) + 2))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_set_nrcoex_priority_etc::CMD_SET_NRCOEX_PRIOR length is to short [%d].}\r\n", OAL_STRLEN(pc_command));

        return -OAL_EFAIL;
    }
    nrcoex_priority = oal_atoi(pc_command + OAL_STRLEN((oal_int8 *)CMD_SET_NRCOEX_PRIOR) + 1);

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_set_nrcoex_priority_etc::CMD_SET_NRCOEX_PRIOR command,nrcoex priority:%d}\r\n", nrcoex_priority);

    l_ret = (oal_int32)wal_ioctl_nrcoex_priority_set(pst_net_dev, (oal_int8*)&nrcoex_priority);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_set_nrcoex_priority_etc::return err code [%d]!}\r\n", l_ret);
        return -OAL_EFAIL;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32 wal_ioctl_get_nrcoex_stat(oal_net_device_stru *pst_net_dev, oal_int8 *puc_stat)
{
    wal_msg_write_stru           st_write_msg;
    oal_int32                    l_ret;
    mac_vap_stru                *pst_mac_vap;
    hmac_device_stru            *pst_hmac_device;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_get_nrcoex_stat: pst_mac_vap get from netdev is null.");
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_hmac_device = hmac_res_get_mac_dev_etc(pst_mac_vap->uc_device_id);
    if (OAL_PTR_NULL == pst_hmac_device)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_ioctl_get_nrcoex_stat: pst_hmac_device is null ptr. device id:%d", pst_mac_vap->uc_device_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    memset_s(&st_write_msg, OAL_SIZEOF(st_write_msg), 0, OAL_SIZEOF(st_write_msg));
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_QUERY_NRCOEX_STAT, 0);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_ioctl_get_nrcoex_stat: send  event failed ret:%d", l_ret);
    }

    pst_hmac_device->st_nrcoex_stat_query.en_query_completed_flag = OAL_FALSE;
    /*lint -e730 -e740 -e774*/
    l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_device->st_nrcoex_stat_query.st_wait_queue, (OAL_TRUE == pst_hmac_device->st_nrcoex_stat_query.en_query_completed_flag), 5 * OAL_TIME_HZ);
    /*lint +e730 +e740 +e774*/
    if (l_ret <= 0) /* ?????????????? */
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_ioctl_get_nrcoex_stat: timeout or err occur. ret:%d", l_ret);
        return OAL_FAIL;
    }

    if (puc_stat != OAL_PTR_NULL)
    {
        if (EOK != memcpy_s(puc_stat, OAL_SIZEOF(mac_nrcoex_stat_stru),
                            (oal_uint8*)&pst_hmac_device->st_nrcoex_stat_query.st_nrcoex_stat,
                            OAL_SIZEOF(mac_nrcoex_stat_stru))) {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_ioctl_get_nrcoex_stat::memcpy fail!");
            return OAL_FAIL;
        }
    }

    return OAL_SUCC;
}

#endif

OAL_STATIC oal_uint32  wal_vendor_fem_lowpower(oal_net_device_stru *pst_net_dev,
    oal_int8 *pc_fem_cmd, oal_uint8 uc_cmd_len)
{
    if (uc_cmd_len > WAL_HIPRIV_CMD_NAME_MAX_LEN) {
        return OAL_FAIL;
    }

    return wal_hipriv_fem_lowpower(pst_net_dev, pc_fem_cmd);
}


oal_int32 wal_vendor_priv_cmd_ext_etc(oal_net_device_stru *pst_net_dev,
                                                            oal_ifreq_stru *pst_ifr, oal_uint8 *pc_cmd)
{
    char fem_lower_cmd[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {"1"};
    oal_int32    l_ret              = 0;

    if (0 == oal_strncasecmp(pc_cmd, CMD_SET_VHT160_FEM_LOWER, CMD_SET_VHT160_FEM_LOWER_LEN))
    {
        /* ????fem?????? */
        return wal_vendor_fem_lowpower(pst_net_dev, fem_lower_cmd, sizeof(fem_lower_cmd));
    }
    else if (0 == oal_strncasecmp(pc_cmd, CMD_GET_VHT160_SUPPORTED, CMD_GET_VHT160_SUPPORTED_LEN))
    {
        return wal_vendor_get_160m_supported_etc(pst_net_dev, pst_ifr);
    }
    else if (0 == oal_strncasecmp(pc_cmd, CMD_GET_AP_BANDWIDTH, CMD_GET_AP_BANDWIDTH_LEN))
    {
        return wal_vendor_get_ap_bandwidth_etc(pst_net_dev, pst_ifr);
    } else if (0 == oal_strncasecmp(pc_cmd, CMD_SET_DC_STATE, CMD_SET_DC_STATE_LEN)) {
        oal_int32 l_dc_status = -1;
        if (OAL_STRLEN(pc_cmd) < (CMD_SET_DC_STATE_LEN + 2)) {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_ext_etc::dc status, check cmd len[%d].}", OAL_STRLEN(pc_cmd));
            return -OAL_EFAIL;
        }

        l_dc_status = oal_atoi(pc_cmd + CMD_SET_DC_STATE_LEN + 1);
        l_ret = wal_ioctl_set_dc_status(pst_net_dev, l_dc_status);
        if (l_ret != OAL_SUCC) {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "wal_vendor_priv_cmd_ext_etc:dc status syn fail!");
            return -OAL_EFAIL;
        }
    }
#ifdef _PRE_WLAN_FEATURE_PSM_FLT_STAT
    else if (0 == oal_strncasecmp(pc_cmd, CMD_GET_FAST_SLEEP_CNT, CMD_GET_FAST_SLEEP_CNT_LEN)) {
        l_ret = wal_ioctl_get_psm_stat(pst_net_dev, pc_cmd, pst_ifr);
        if (OAL_SUCC != l_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc:CMD_GET_FAST_SLEEP_CNT Failed to get psm stat ret[%d] !", l_ret);
            return -OAL_EFAIL;
        }

        return l_ret;
    }
#endif
#ifdef _PRE_WLAN_FEATURE_NRCOEX
    else if(0 == oal_strncasecmp(pc_cmd, CMD_SET_NRCOEX_PRIOR, OAL_STRLEN(CMD_SET_NRCOEX_PRIOR)))
    {
        return wal_vendor_set_nrcoex_priority_etc(pst_net_dev, pst_ifr, pc_cmd);
    }
    else if(0 == oal_strncasecmp(pc_cmd, CMD_GET_NRCOEX_INFO, OAL_STRLEN(CMD_GET_NRCOEX_INFO)))
    {
        l_ret = wal_ioctl_get_nrcoex_stat(pst_net_dev, pc_cmd);
        if (OAL_SUCC != l_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc:CMD_GET_NRCOEX_INFO Failed to get nrcoex stat ret[%d] !", l_ret);
            return -OAL_EFAIL;
        }

        if(oal_copy_to_user(pst_ifr->ifr_data+8, pc_cmd, 2*OAL_SIZEOF(oal_uint32)))
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc:CMD_GET_NRCOEX_INFO Failed to copy ioctl_data to user !");
            return -OAL_EFAIL;
        }
    }
#endif

    /* ????????????????????????????????????????wpa_supplicant????ioctl??????????????????wifi */
    return OAL_SUCC;
}



oal_int32 wal_vendor_priv_cmd_etc(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr, oal_int32 ul_cmd)
{
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    wal_wifi_priv_cmd_stru  st_priv_cmd;
    oal_int8    *pc_command         = OAL_PTR_NULL;
    oal_int32    l_ret              = 0;
    oal_int32    l_value;
    oal_int32   *pl_value;
#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
    oal_int32    l_param_1 = 0;
    oal_int32    l_param_2 = 0;
    oal_int8     ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {0};     /* ?????????????????????? */
    oal_uint32   ul_off_set;
    oal_int8    *pc_cmd_copy;
#endif
    oal_int32    l_memcpy_ret = EOK;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (!capable(CAP_NET_ADMIN))
    {
        return -EPERM;
    }
#endif

    if (OAL_PTR_NULL == pst_ifr->ifr_data)
    {
        l_ret = -OAL_EINVAL;
        return l_ret;
    }
#ifdef _PRE_WLAN_FEATURE_DFR
    if (g_st_dfr_info_etc.bit_device_reset_process_flag)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::dfr_process_status[%d]!}",
                g_st_dfr_info_etc.bit_device_reset_process_flag);
        return OAL_SUCC;
    }
#endif //#ifdef _PRE_WLAN_FEATURE_DFR
    if (oal_copy_from_user((oal_uint8 *)&st_priv_cmd, pst_ifr->ifr_data, sizeof(wal_wifi_priv_cmd_stru)))
    {
        l_ret = -OAL_EINVAL;
        return l_ret;
    }

    if (st_priv_cmd.ul_total_len > MAX_PRIV_CMD_SIZE || st_priv_cmd.ul_total_len < 0)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::too long or zero private command. len:%d. }\r\n", st_priv_cmd.ul_total_len);
        l_ret = -OAL_EINVAL;
        return l_ret;
    }

    /* ????????????wpa_supplicant ???????????????? */
    pc_command = oal_memalloc((oal_uint32)(st_priv_cmd.ul_total_len + 5));/* total len ??priv cmd ????buffer ???? */
    if (OAL_PTR_NULL == pc_command)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::mem alloc failed.}\r\n");

        l_ret = -OAL_ENOMEM;
        return l_ret;
    }

    /* ????wpa_supplicant ?????????????? */
    memset_s(pc_command, (oal_uint32)(st_priv_cmd.ul_total_len + 5), 0, (oal_uint32)(st_priv_cmd.ul_total_len + 5));

    l_ret = (oal_int32)oal_copy_from_user(pc_command, pst_ifr->ifr_data + 8, (oal_uint32)(st_priv_cmd.ul_total_len));
    if (l_ret != 0)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::oal_copy_from_user: -OAL_EFAIL }\r\n");
        l_ret = -OAL_EFAIL;
        oal_free(pc_command);
        return l_ret;
    }
    pc_command[st_priv_cmd.ul_total_len] = '\0';
    OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::vendor private cmd total_len:%d, used_len:%d.}\r\n",
                st_priv_cmd.ul_total_len, st_priv_cmd.ul_used_len);

    if (oal_strncasecmp(pc_command, CMD_SET_AP_WPS_P2P_IE, CMD_SET_AP_WPS_P2P_IE_LEN) == 0)
    {
        l_ret = wal_ioctl_priv_cmd_set_ap_wps_p2p_ie_etc(pst_net_dev, pc_command, st_priv_cmd);
    }
    else if (oal_strncasecmp(pc_command, CMD_SET_MLME_IE, CMD_SET_MLME_IE_LEN) == 0)
    {
        oal_uint32 skip = CMD_SET_MLME_IE_LEN + 1;
        /* ?????????? */
        oal_mlme_ie_stru *pst_mlme_ie;
        pst_mlme_ie = (oal_mlme_ie_stru *)(pc_command + skip);

        /*lint -e413*/
        if((skip + pst_mlme_ie->us_ie_len + OAL_OFFSET_OF(oal_mlme_ie_stru, auc_ie)) > (oal_uint32)st_priv_cmd.ul_total_len)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::SET_ASSOC_RSP_IE param len is too short. need %d.}\r\n", (skip + pst_mlme_ie->us_ie_len));
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
        /*lint +e413*/
        l_ret = wal_ioctl_set_mlme_ie_etc(pst_net_dev, pst_mlme_ie);
    }
#ifdef _PRE_WLAN_FEATURE_P2P
    else if (0 == oal_strncasecmp(pc_command, CMD_MIRACAST_START, CMD_MIRACAST_START_LEN))
    {
        OAM_WARNING_LOG0(0, OAM_SF_M2S, "{wal_vendor_priv_cmd_etc::Miracast start.}\r\n");
        l_ret = wal_ioctl_set_p2p_miracast_status(pst_net_dev, OAL_TRUE);
    }
    else if (0 == oal_strncasecmp(pc_command, CMD_MIRACAST_STOP, CMD_MIRACAST_STOP_LEN))
    {
        OAM_WARNING_LOG0(0, OAM_SF_M2S, "{wal_vendor_priv_cmd_etc::Miracast stop.}\r\n");
        l_ret = wal_ioctl_set_p2p_miracast_status(pst_net_dev, OAL_FALSE);
    }
    else if(oal_strncasecmp(pc_command, CMD_P2P_SET_NOA, CMD_P2P_SET_NOA_LEN) == 0)
    {
        oal_uint32 skip = CMD_P2P_SET_NOA_LEN + 1;
        mac_cfg_p2p_noa_param_stru  st_p2p_noa_param;
        l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_P2P_SET_NOA_LEN, OAL_SIZEOF(st_p2p_noa_param));
        if (OAL_SUCC != l_ret)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_P2P_SET_NOA param len is too short. need %d.}\r\n", skip + OAL_SIZEOF(st_p2p_noa_param));
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
        l_memcpy_ret += memcpy_s(&st_p2p_noa_param, OAL_SIZEOF(mac_cfg_p2p_noa_param_stru),
                                 pc_command + skip, OAL_SIZEOF(mac_cfg_p2p_noa_param_stru));

        l_ret = wal_ioctl_set_p2p_noa(pst_net_dev,&st_p2p_noa_param);
    }
    else if(oal_strncasecmp(pc_command, CMD_P2P_SET_PS, CMD_P2P_SET_PS_LEN) == 0)
    {
        oal_uint32 skip = CMD_P2P_SET_PS_LEN + 1;
        mac_cfg_p2p_ops_param_stru  st_p2p_ops_param;
        l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_P2P_SET_PS_LEN, OAL_SIZEOF(st_p2p_ops_param));
        if (OAL_SUCC != l_ret)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_P2P_SET_PS param len is too short.need %d.}\r\n", skip + OAL_SIZEOF(st_p2p_ops_param));
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
        l_memcpy_ret += memcpy_s(&st_p2p_ops_param, OAL_SIZEOF(mac_cfg_p2p_ops_param_stru),
                                 pc_command + skip, OAL_SIZEOF(mac_cfg_p2p_ops_param_stru));

        l_ret = wal_ioctl_set_p2p_ops(pst_net_dev,&st_p2p_ops_param);
    }
#endif

#ifdef _PRE_WLAN_FEATURE_HS20
    else if (0 == oal_strncasecmp(pc_command, CMD_SET_QOS_MAP, CMD_SET_QOS_MAP_LEN))
    {
        oal_uint32 skip = CMD_SET_QOS_MAP_LEN + 1;
        hmac_cfg_qos_map_param_stru st_qos_map_param;
        l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_SET_QOS_MAP_LEN, OAL_SIZEOF(st_qos_map_param));
        if (OAL_SUCC != l_ret)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_SET_QOS_MAP param len is too short.need %d.}\r\n", skip + OAL_SIZEOF(st_qos_map_param));
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
        l_memcpy_ret += memcpy_s(&st_qos_map_param, OAL_SIZEOF(hmac_cfg_qos_map_param_stru),
                                 pc_command + skip, OAL_SIZEOF(hmac_cfg_qos_map_param_stru));

        l_ret = wal_ioctl_set_qos_map(pst_net_dev,&st_qos_map_param);
    }
#endif
    else if(0 == oal_strncasecmp(pc_command, CMD_SET_POWER_ON, CMD_SET_POWER_ON_LEN))
    {
        oal_int32 power_flag = -1;
        oal_uint32 command_len = OAL_STRLEN(pc_command);
        /* ????:SET_POWER_ON 1 or SET_POWER_ON 0 */
        if(command_len < (CMD_SET_POWER_ON_LEN + 2))
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_SET_POWER_ON cmd len must equal or larger than 18. Now the cmd len:%d.}\r\n", command_len);

            oal_free(pc_command);
            return -OAL_EFAIL;
        }

        power_flag = oal_atoi(pc_command + CMD_SET_POWER_ON_LEN + 1);

        l_ret = wal_netdev_set_power_on(pst_net_dev, power_flag);
    }
    else if(0 == oal_strncasecmp(pc_command, CMD_SET_POWER_MGMT_ON, CMD_SET_POWER_MGMT_ON_LEN))
    {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        struct wlan_pm_s  *pst_wlan_pm = OAL_PTR_NULL;
#endif

        oal_uint power_mgmt_flag = OAL_TRUE;  /* AP????,???????????????????? */
        oal_uint32 command_len = OAL_STRLEN(pc_command);
        /* ????:CMD_SET_POWER_MGMT_ON 1 or CMD_SET_POWER_MGMT_ON 0 */
        if(command_len < (CMD_SET_POWER_MGMT_ON_LEN + 2))
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_SET_POWER_MGMT_ON cmd len:%d is error.}\r\n", command_len);

            oal_free(pc_command);
            return -OAL_EFAIL;
        }

        power_mgmt_flag = (oal_uint)oal_atoi(pc_command + CMD_SET_POWER_MGMT_ON_LEN + 1);

        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_SET_POWER_MGMT_ON command,power_mgmt flag:%u}\r\n", power_mgmt_flag);

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        pst_wlan_pm = wlan_pm_get_drv_etc();
        if(NULL != pst_wlan_pm)
        {
            pst_wlan_pm->ul_apmode_allow_pm_flag = power_mgmt_flag;
        }
        else
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::wlan_pm_get_drv_etc return null.");
        }
#endif

    }
    else if(0 == oal_strncasecmp(pc_command, CMD_COUNTRY, CMD_COUNTRY_LEN))
    {
        l_ret = wal_ioctl_priv_cmd_country_etc(pst_net_dev, pc_command, st_priv_cmd);
    }
    else if(0 == oal_strncasecmp(pc_command, CMD_GET_CAPA_DBDC, CMD_GET_CAPA_DBDC_LEN))
    {
        oal_int32      cmd_len = CMD_CAPA_DBDC_SUPP_LEN;
        oal_int32      ret_len = 0;

        if ((oal_uint32)st_priv_cmd.ul_total_len < CMD_CAPA_DBDC_SUPP_LEN)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_GET_CAPA_DBDC length is to short. need %d}\r\n", CMD_CAPA_DBDC_SUPP_LEN);
            oal_free(pc_command);
            return -OAL_EFAIL;
        }

        /* ??buf???? */
        ret_len = OAL_MAX(st_priv_cmd.ul_total_len, cmd_len);
        memset_s(pc_command, (oal_uint32)(ret_len + 1), 0, (oal_uint32)(ret_len + 1));
        pc_command[ret_len] = '\0';

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1103_HOST)
        /* hi1103 support DBDC */
        l_memcpy_ret += memcpy_s(pc_command, (oal_uint32)(ret_len + 1),
                                 CMD_CAPA_DBDC_SUPP, CMD_CAPA_DBDC_SUPP_LEN);
#else
        /* other don't support DBDC */
        l_memcpy_ret += memcpy_s(pc_command, (oal_uint32)(ret_len + 1),
                                 CMD_CAPA_DBDC_NOT_SUPP, CMD_CAPA_DBDC_NOT_SUPP_LEN);
#endif

        l_ret = oal_copy_to_user(pst_ifr->ifr_data+8, pc_command, ret_len);
        if (OAL_UNLIKELY(OAL_SUCC != l_ret))
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc:CMD_GET_CAPA_DBDC Failed to copy ioctl_data to user !");
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
        OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_GET_CAPA_DBDC reply len=%d, l_ret=%d}\r\n", OAL_STRLEN(pc_command), l_ret);
    }
#ifdef _PRE_WLAN_FEATURE_LTECOEX
    else if(0 == oal_strncasecmp(pc_command, CMD_LTECOEX_MODE, CMD_LTECOEX_MODE_LEN))
    {
        l_ret = wal_ioctl_priv_cmd_ltecoex_mode_etc(pc_command, pst_net_dev);
    }
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    else if (oal_strncasecmp(pc_command, CMD_TX_POWER, CMD_TX_POWER_LEN) == 0)
    {
        l_ret = wal_ioctl_priv_cmd_tx_power_etc(pst_net_dev, pc_command, st_priv_cmd);
    }
#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
    else if (oal_strncasecmp(pc_command, CMD_SET_MEMO_CHANGE, CMD_SET_MEMO_CHANGE_LEN) == 0)
    {
        if (OAL_ANY_TRUE_VALUE2(aen_tas_switch_en[WLAN_RF_CHANNEL_ZERO], aen_tas_switch_en[WLAN_RF_CHANNEL_ONE]))
        {
            l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_SET_MEMO_CHANGE_LEN, 1);
            if (l_ret != OAL_SUCC) {
                OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_SET_MEMO_CHANGE cmd len err.}");
                oal_free(pc_command);
                return -OAL_EFAIL;
            }

            /* 0:?????? 1:tas?? */
            l_param_1 = oal_atoi(pc_command + CMD_SET_MEMO_CHANGE_LEN + 1);
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_SET_MEMO_CHANGE antIndex[%d].}", l_param_1);
            l_ret = board_wifi_tas_set(l_param_1);
        }
    }
    else if (oal_strncasecmp(pc_command, CMD_TAS_GET_ANT, CMD_TAS_GET_ANT_LEN) == 0)
    {
        oal_free(pc_command);
        return board_get_wifi_tas_gpio_state();
    }
    else if (oal_strncasecmp(pc_command, CMD_MEASURE_TAS_RSSI, CMD_MEASURE_TAS_RSSI_LEN) == 0)
    {
        l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_MEASURE_TAS_RSSI_LEN, 1);
        if (l_ret != OAL_SUCC) {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_MEASURE_TAS_RSSI cmd len error}");
            oal_free(pc_command);
            return -OAL_EFAIL;
        }

        l_param_1 = !!oal_atoi(pc_command + CMD_MEASURE_TAS_RSSI_LEN + 1);
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_MEASURE_TAS_RSSI coreIndex[%d].}", l_param_1);
        /* ???????? */
        l_ret = wal_ioctl_tas_rssi_access(pst_net_dev, l_param_1);
    }
    else if (oal_strncasecmp(pc_command, CMD_SET_TAS_TXPOWER, CMD_SET_TAS_TXPOWER_LEN) == 0)
    {
        /* tas?????? */
        pc_cmd_copy = pc_command;
        pc_cmd_copy += CMD_SET_TAS_TXPOWER_LEN;
        l_value = wal_get_cmd_one_arg_etc(pc_cmd_copy, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != l_value)
        {
             OAM_WARNING_LOG1(0, OAM_SF_TPC, "{wal_vendor_priv_cmd_etc::CMD_SET_TAS_TXPOWER return err_code [%d]!}\r\n", l_ret);
             oal_free(pc_command);
             return OAL_SUCC;
        }
        l_param_1 = !!oal_atoi(ac_name);
        /* ????needImprove???? */
        pc_cmd_copy += ul_off_set;
        l_ret = wal_get_cmd_one_arg_etc(pc_cmd_copy, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC == l_ret)
        {
            l_param_2 = !!oal_atoi(ac_name);
            OAM_WARNING_LOG2(0, OAM_SF_TPC, "{wal_vendor_priv_cmd_etc::CMD_SET_TAS_TXPOWER coreIndex[%d] needImprove[%d].}",
                             l_param_1, l_param_2);
            /* TAS?????????? */
            l_ret = wal_ioctl_tas_pow_ctrl(pst_net_dev, l_param_1, l_param_2);
        }
        else
        {
             oal_free(pc_command);
             return OAL_SUCC;
        }
    }
#endif
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    else if (oal_strncasecmp(pc_command, CMD_WPAS_GET_CUST, CMD_WPAS_GET_CUST_LEN) == 0)
    {
        /* ??buf???? */
        memset_s(pc_command, st_priv_cmd.ul_total_len + 1, 0, st_priv_cmd.ul_total_len + 1);
        pc_command[st_priv_cmd.ul_total_len] = '\0';

        /* ??????????????????????????????disable_capab_ht40 */
        hwifi_config_host_global_ini_param();

        /* ????ht40?????? */
        *pc_command = wlan_customize_etc.uc_disable_capab_2ght40;

        if(oal_copy_to_user(pst_ifr->ifr_data+8, pc_command, (oal_uint32)(st_priv_cmd.ul_total_len)))
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc: Failed to copy ioctl_data to user !");
            oal_free(pc_command);
            /* ??????????????supplicant??????????supplicant???????????????? */
            return -OAL_EFAIL;
        }
    }
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_VOWIFI
    else if (oal_strncasecmp(pc_command, CMD_VOWIFI_SET_PARAM, CMD_VOWIFI_SET_PARAM_LEN) == 0)
    {
        l_ret = wal_ioctl_set_vowifi_param(pst_net_dev, pc_command, &st_priv_cmd);
        if (OAL_UNLIKELY(OAL_SUCC != l_ret))
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::VOWIFI_SET_PARAM return err code [%d]!}", l_ret);
            oal_free(pc_command);
            return -OAL_EFAIL;
        }

    }
    else if (oal_strncasecmp(pc_command, CMD_VOWIFI_GET_PARAM, CMD_VOWIFI_GET_PARAM_LEN) == 0)
    {
        l_value = 0;
        l_ret = wal_ioctl_get_vowifi_param(pst_net_dev, pc_command, &l_value);
        if (OAL_UNLIKELY(OAL_SUCC != l_ret))
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_VOWIFI_GET_MODE(%d) return err code [%d]!}", l_ret);
            oal_free(pc_command);
            return -OAL_EFAIL;
        }

        /* ??buf???? */
        memset_s(pc_command, (oal_uint32)(st_priv_cmd.ul_total_len + 1), 0, (oal_uint32)(st_priv_cmd.ul_total_len + 1));
        pc_command[st_priv_cmd.ul_total_len] = '\0';
        pl_value  = (oal_int32 *)pc_command;
        *pl_value = l_value;

        if(oal_copy_to_user(pst_ifr->ifr_data+8, pc_command, (oal_uint32)(st_priv_cmd.ul_total_len)))
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc:CMD_VOWIFi_GET_MODE Failed to copy ioctl_data to user !");
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
    }
#endif /* _PRE_WLAN_FEATURE_VOWIFI */
    else if(oal_strncasecmp(pc_command, CMD_GET_WIFI_PRIV_FEATURE_CAPABILITY, CMD_GET_WIFI_PRIV_FEATURE_CAPABILITY_LEN) == 0)
    {
        l_value = 0;
        l_ret = wal_ioctl_get_wifi_priv_feature_cap_param(pst_net_dev, pc_command, &l_value);
        if (OAL_UNLIKELY(OAL_SUCC != l_ret))
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_GET_WIFI_PRVI_FEATURE_CAPABILITY(%d) return err code [%d]!}", l_ret);
            oal_free(pc_command);
            return -OAL_EFAIL;
        }

        /* ??buf???? */
        memset_s(pc_command, (oal_uint32)(st_priv_cmd.ul_total_len + 1), 0, (oal_uint32)(st_priv_cmd.ul_total_len + 1));
        pc_command[st_priv_cmd.ul_total_len] = '\0';
        pl_value  = (oal_int32 *)pc_command;
        *pl_value = l_value;

        OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc::CMD_GET_WIFI_PRVI_FEATURE_CAPABILITY = [%x]!",*pl_value);

        if(oal_copy_to_user(pst_ifr->ifr_data+8, pc_command, (oal_uint32)(st_priv_cmd.ul_total_len)))
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc:CMD_GET_WIFI_PRVI_FEATURE_CAPABILITY Failed to copy ioctl_data to user !");
            oal_free(pc_command);
            return -OAL_EFAIL;
        }

    }
    else if (oal_strncasecmp(pc_command, CMD_VOWIFI_IS_SUPPORT, CMD_VOWIFI_IS_SUPPORT_LEN) == 0)
    {
        oal_int32      cmd_len = CMD_VOWIFI_IS_SUPPORT_REPLY_LEN;
        oal_int32      ret_len = 0;

        if ((oal_uint32)st_priv_cmd.ul_total_len < CMD_VOWIFI_IS_SUPPORT_REPLY_LEN)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_VOWIFI_IS_SUPPORT length is to short. need %d}\r\n", CMD_VOWIFI_IS_SUPPORT_REPLY_LEN);
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
        /* ??buf???? */
        ret_len = OAL_MAX(st_priv_cmd.ul_total_len, cmd_len);
        memset_s(pc_command, (oal_uint32)(ret_len + 1), 0, (oal_uint32)(ret_len + 1));
        pc_command[ret_len] = '\0';
        l_memcpy_ret += memcpy_s(pc_command, (oal_uint32)(ret_len + 1),
                                 CMD_VOWIFI_IS_SUPPORT_REPLY, CMD_VOWIFI_IS_SUPPORT_REPLY_LEN);
        if(oal_copy_to_user(pst_ifr->ifr_data+8, pc_command, ret_len))
        {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc:CMD_VOWIFI_IS_SUPPORT Failed to copy ioctl_data to user !");
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
    }
#ifdef _PRE_WLAN_FEATURE_IP_FILTER
    else if (oal_strncasecmp(pc_command, CMD_FILTER_SWITCH, CMD_FILTER_SWITCH_LEN) == 0)
    {
#ifdef CONFIG_DOZE_FILTER
        oal_int32 l_on;
        oal_uint32 command_len = OAL_STRLEN(pc_command);

        /* ????:FILTER 1 or FILTER 0 */
        l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_FILTER_SWITCH_LEN, 1);
        if (OAL_SUCC != l_ret)
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_FILTER_SWITCH cmd len must equal or larger than 8. Now the cmd len:%d.}\r\n", command_len);

            oal_free(pc_command);
            return -OAL_EFAIL;
        }

        l_on = oal_atoi(pc_command + CMD_FILTER_SWITCH_LEN + 1);

        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_FILTER_SWITCH %d.}", l_on);

        /* ???????????????? gWlanFilterOps.set_filter_enable */
        l_ret = hw_set_net_filter_enable(l_on);
        if (OAL_UNLIKELY(OAL_SUCC != l_ret))
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::CMD_FILTER_SWITCH return err code [%d]!}", l_ret);
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
#else
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc::Not support CMD_FILTER_SWITCH.}");
#endif
    }
#endif /* _PRE_WLAN_FEATURE_IP_FILTER */

#ifndef CONFIG_HAS_EARLYSUSPEND
    else if (0 == oal_strncasecmp(pc_command, CMD_SETSUSPENDMODE, CMD_SETSUSPENDMODE_LEN))
    {
        l_ret = wal_ioctl_judge_input_param_length_etc(st_priv_cmd, CMD_SETSUSPENDMODE_LEN, 1);
        if (OAL_SUCC != l_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_priv_cmd_etc:: CMD_SETSUSPENDMODE length is too short! at least need [%d]!}\r\n", (CMD_SETSUSPENDMODE_LEN + 2));
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
        l_ret = wal_ioctl_set_suspend_mode(pst_net_dev, *(pc_command + CMD_SETSUSPENDMODE_LEN + 1) - '0');
    }
#endif

#ifdef _PRE_WLAN_FEATURE_STA_PM
    else if(0 == oal_strncasecmp(pc_command, CMD_SET_STA_PM_ON, CMD_SET_STA_PM_ON_LEN))
    {
       l_ret = wal_ioctl_set_sta_pm_on(pst_net_dev, pc_command,st_priv_cmd);
    }
#endif
    else if(OAL_TRUE == wal_vendor_cmd_gather(pc_command))
    {
        l_ret = wal_vendor_cmd_gather_handler(pst_net_dev, pc_command);
    }
#ifdef  _PRE_WLAN_FEATURE_FORCE_STOP_FILTER
    else if (0 == oal_strncasecmp(pc_command, CMD_RXFILTER_START, CMD_RXFILTER_START_LEN))
    {
        wal_ioctl_force_stop_filter(pst_net_dev, OAL_FALSE);
    }
    else if (0 == oal_strncasecmp(pc_command, CMD_RXFILTER_STOP, CMD_RXFILTER_STOP_LEN))
    {
        wal_ioctl_force_stop_filter(pst_net_dev, OAL_TRUE);
    }
#endif
#ifdef _PRE_WLAN_FEATURE_PSM_FLT_STAT
    else if (0 == oal_strncasecmp(pc_command, CMD_GET_APF_PKTS_CNT, CMD_GET_APF_PKTS_CNT_LEN))
    {
        l_ret = wal_ioctl_get_psm_stat(pst_net_dev, pc_command, pst_ifr);
        if (OAL_SUCC != l_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc:CMD_GET_APF_PKTS_CNT Failed to get psm stat ret[%d] !", l_ret);
            oal_free(pc_command);
            return -OAL_EFAIL;
        }
    }
#endif
#ifdef _PRE_WLAN_FEATURE_STA_PM
    else if (0 == oal_strncasecmp(pc_command, CMD_SET_FASTSLEEP_SWITCH, CMD_SET_FASTSLEEP_SWITCH_LEN))
    {
        wal_ioctl_set_fastsleep_switch(pst_net_dev, pc_command+CMD_SET_FASTSLEEP_SWITCH_LEN);
    }
#endif
    else
    {
        /* ?????????????????????????????????? */
        l_ret = wal_vendor_priv_cmd_ext_etc(pst_net_dev, pst_ifr, pc_command);
    }
    if (l_memcpy_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_vendor_priv_cmd_etc::memcpy fail!");
        oal_free(pc_command);
        return -OAL_EFAIL;
    }


    oal_free(pc_command);
    return l_ret;
#endif
}


oal_int32 wal_net_device_ioctl(oal_net_device_stru *pst_net_dev, oal_ifreq_stru *pst_ifr, oal_int32 ul_cmd)
{
    oal_int32                           l_ret   = 0;

    if (OAL_ANY_NULL_PTR2(pst_net_dev,pst_ifr))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_net_device_ioctl::pst_dev %p, pst_ifr %p!}\r\n",
                       (uintptr_t)pst_net_dev, (uintptr_t)pst_ifr);
        return -OAL_EFAUL;
    }

    if (OAL_PTR_NULL == pst_ifr->ifr_data)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_net_device_ioctl::pst_ifr->ifr_data is NULL, ul_cmd[0x%x]!}\r\n", ul_cmd);
        return -OAL_EFAUL;
    }

    /* 1102 wpa_supplicant ????ioctl ???????? */
    if (WAL_SIOCDEVPRIVATE+1 == ul_cmd)
    {
        l_ret = wal_vendor_priv_cmd_etc(pst_net_dev, pst_ifr, ul_cmd);
        return l_ret;
    }
#if (_PRE_OS_VERSION_WIN32 != _PRE_OS_VERSION)

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    /*atcmdsrv ????ioctl????????*/
    else if ( (WAL_SIOCDEVPRIVATE + 2) == ul_cmd )
    {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        if (!capable(CAP_NET_ADMIN))
        {
            return -EPERM;
        }
#endif
        if (HI1XX_OS_BUILD_VARIANT_ROOT == hi11xx_get_os_build_variant())
        {
            wal_wake_lock();
            l_ret = wal_atcmdsrv_wifi_priv_cmd_etc(pst_net_dev, pst_ifr, ul_cmd);
            wal_wake_unlock();
        }

        return l_ret;
    }
#endif
#endif
    else
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_net_device_ioctl::unrecognised cmd[0x%x]!}\r\n", ul_cmd);
        return OAL_SUCC;
    }
}



OAL_STATIC oal_uint32  wal_hipriv_set_mode(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    oal_int8                    ac_mode_str[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {0};     /* ?????????????????????? */
    oal_uint8                   uc_prot_idx;
    mac_cfg_mode_param_stru    *pst_mode_param = OAL_PTR_NULL;
    wal_msg_write_stru          st_write_msg;
    oal_uint32                  ul_off_set;
    oal_uint32                  ul_ret;
    oal_int32                   l_ret;
    wal_msg_stru               *pst_rsp_msg = OAL_PTR_NULL;

    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR2(pst_net_dev,pc_param)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_hipriv_set_mode::pst_net_dev/p_param null ptr error %x!}\r\n", (uintptr_t)pst_net_dev, (uintptr_t)pc_param);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /* pc_param????????????????, ??????????????ac_mode_str?? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_mode_str, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mode::wal_get_cmd_one_arg_etc vap name return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }

    ac_mode_str[OAL_SIZEOF(ac_mode_str) - 1] = '\0';    /* ??????null???? */

    for (uc_prot_idx = 0; OAL_PTR_NULL != g_ast_mode_map_etc[uc_prot_idx].pc_name; uc_prot_idx++)
    {
        l_ret = oal_strcmp(g_ast_mode_map_etc[uc_prot_idx].pc_name, ac_mode_str);

        if (0 == l_ret)
        {
            break;
        }
    }

    if (OAL_PTR_NULL == g_ast_mode_map_etc[uc_prot_idx].pc_name)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_mode::unrecognized protocol string!}\r\n");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }


    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_MODE, OAL_SIZEOF(mac_cfg_mode_param_stru));

    pst_mode_param = (mac_cfg_mode_param_stru *)(st_write_msg.auc_value);
    pst_mode_param->en_protocol  = g_ast_mode_map_etc[uc_prot_idx].en_mode;
    pst_mode_param->en_band      = g_ast_mode_map_etc[uc_prot_idx].en_band;
    pst_mode_param->en_bandwidth = g_ast_mode_map_etc[uc_prot_idx].en_bandwidth;

    OAM_WARNING_LOG3(0, OAM_SF_CFG, "{wal_hipriv_set_mode::protocol[%d],band[%d],bandwidth[%d]!}\r\n",
                            pst_mode_param->en_protocol, pst_mode_param->en_band, pst_mode_param->en_bandwidth);

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                                WAL_MSG_TYPE_WRITE,
                                WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_mode_param_stru),
                                (oal_uint8 *)&st_write_msg,
                                OAL_TRUE,
                                &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mode::wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_set_mode fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44))

oal_uint32  wal_hipriv_set_essid_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    oal_uint8                       uc_ssid_len;
    oal_int32                       l_ret;
    wal_msg_write_stru              st_write_msg;
    mac_cfg_ssid_param_stru        *pst_param;
    mac_vap_stru                   *pst_mac_vap;
    oal_uint32                      ul_off_set;
    oal_int8                       *pc_ssid;
    oal_int8                        ac_ssid[WLAN_SSID_MAX_LEN] = {0};
    oal_uint32                      ul_ret;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_essid_etc::pst_mac_vap is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (WLAN_VAP_MODE_BSS_AP == pst_mac_vap->en_vap_mode)
    {
        /* ??????up????????AP??????????????????????down */
        if (0 != (OAL_IFF_RUNNING & OAL_NETDEVICE_FLAGS(pst_net_dev)))
        {
            OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_set_essid_etc::device is busy, please down it firste %d!}\r\n", OAL_NETDEVICE_FLAGS(pst_net_dev));
            return -OAL_EBUSY;
        }
    }

    /* pc_param????????????????, ??????????????ac_mode_str?? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_ssid, WLAN_SSID_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_essid_etc::wal_get_cmd_one_arg_etc vap name return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }

    pc_ssid       = ac_ssid;
    pc_ssid       = oal_strim(ac_ssid);                   /* ???????????????????????? */
    uc_ssid_len = (oal_uint8)OAL_STRLEN(pc_ssid);

    OAM_INFO_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_set_essid_etc:: ssid length %d!}\r\n", uc_ssid_len);

    if (uc_ssid_len > WLAN_SSID_MAX_LEN - 1)        /* -1??\0???????? */
    {
        uc_ssid_len = WLAN_SSID_MAX_LEN - 1;
    }

    OAM_INFO_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_set_essid_etc:: ssid length is %d!}\r\n", uc_ssid_len);
    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SSID, OAL_SIZEOF(mac_cfg_ssid_param_stru));

    /* ????WID?????????? */
    pst_param = (mac_cfg_ssid_param_stru *)(st_write_msg.auc_value);
    pst_param->uc_ssid_len = uc_ssid_len;
    if (EOK != memcpy_s(pst_param->ac_ssid, OAL_SIZEOF(pst_param->ac_ssid), pc_ssid, uc_ssid_len)) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_set_essid_etc::memcpy fail!");
        return OAL_FAIL;
    }

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_ssid_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_set_essid_etc:: wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif

/*
 * ?? ?? ??  : wal_octl_get_essid
 * ????????  : ????ssid
 */
OAL_STATIC int wal_ioctl_get_essid(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info,
                                        oal_iwreq_data_union *pst_data, char *pc_ssid)
{
    oal_int32 l_ret;
    wal_msg_query_stru st_query_msg;
    mac_cfg_ssid_param_stru *pst_ssid = OAL_PTR_NULL;
    wal_msg_stru *pst_rsp_msg = OAL_PTR_NULL;
    wal_msg_rsp_stru *pst_query_rsp_msg = OAL_PTR_NULL;
    oal_iw_point_stru *pst_essid = (oal_iw_point_stru *)pst_data;

    /* ????????wal?????? */
    st_query_msg.en_wid = WLAN_CFGID_SSID;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_QUERY,
                               WAL_MSG_WID_LENGTH,
                               (oal_uint8 *)&st_query_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (l_ret != OAL_SUCC || pst_rsp_msg == OAL_PTR_NULL) {
        if (pst_rsp_msg != OAL_PTR_NULL) {
            oal_free(pst_rsp_msg);
        }
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_get_essid:: wal_send_cfg_event return err code %d!}", l_ret);
        return -OAL_EFAIL;
    }

    /* ???????????? */
    pst_query_rsp_msg = (wal_msg_rsp_stru *)(pst_rsp_msg->auc_msg_data);

    /* ???????? */
    pst_ssid = (mac_cfg_ssid_param_stru *)(pst_query_rsp_msg->auc_value);
    pst_essid->flags = 1; /* ?????????????????? */
    pst_essid->length = OAL_MIN(pst_ssid->uc_ssid_len, OAL_IEEE80211_MAX_SSID_LEN);
    if (EOK != memcpy_s(pc_ssid, pst_essid->length, pst_ssid->ac_ssid, pst_essid->length)) {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_ioctl_get_essid::memcpy fail!",pst_essid->length);
        oal_free(pst_rsp_msg);
        return -OAL_EINVAL;
    }

    oal_free(pst_rsp_msg);
    return OAL_SUCC;
}


OAL_STATIC int wal_ioctl_get_apaddr(oal_net_device_stru *pst_net_dev,
                                    oal_iw_request_info_stru *pst_info,
                                    oal_iwreq_data_union *pst_wrqu,
                                    char *pc_extra)
{
    mac_vap_stru *pst_mac_vap = OAL_PTR_NULL;
    oal_sockaddr_stru *pst_addr = (oal_sockaddr_stru *)pst_wrqu;
    oal_uint8 auc_zero_addr[WLAN_MAC_ADDR_LEN] = {0};

    if ((pst_net_dev == OAL_PTR_NULL) || (pst_addr == OAL_PTR_NULL)) {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_ioctl_get_apaddr::param null, pst_net_dev = %p, pst_addr = %p.}",
                        (uintptr_t)pst_net_dev, (uintptr_t)pst_addr);
        return -OAL_EINVAL;
    }

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (pst_mac_vap == OAL_PTR_NULL) {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_ioctl_get_apaddr::pst_mac_vap is null!}\r\n");
        return -OAL_EFAUL;
    }

    if (pst_mac_vap->en_vap_state == MAC_VAP_STATE_UP) {
        oal_set_mac_addr((oal_uint8 *)pst_addr->sa_data, pst_mac_vap->auc_bssid);
    } else {
        oal_set_mac_addr((oal_uint8 *)pst_addr->sa_data, auc_zero_addr);
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_set_freq(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru          st_write_msg;
    oal_int32                   l_freq;
    oal_uint32                  ul_off_set;
    oal_int8                    ac_freq[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                  ul_ret;
    oal_int32                   l_ret;
    wal_msg_stru                *pst_rsp_msg = OAL_PTR_NULL;

    /* pc_param????????????net_device??name, ??????????????ac_name?? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_freq, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_freq::wal_get_cmd_one_arg_etc vap name return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }

    l_freq = oal_atoi(ac_freq);
    OAM_INFO_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_freq::l_freq = %d!}\r\n", l_freq);

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    /* ???????? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_CURRENT_CHANEL, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = l_freq;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_freq::return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

        /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_set_freq fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_11D

OAL_STATIC oal_bool_enum_uint8  wal_is_alpha_upper(oal_int8 c_letter)
{
    if (c_letter >= 'A' && c_letter <= 'Z')
    {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}


oal_uint8  wal_regdomain_get_band_etc(oal_uint32 ul_start_freq, oal_uint32 ul_end_freq)
{
    if (ul_start_freq > 2400 && ul_end_freq < 2500)
    {
        return MAC_RC_START_FREQ_2;
    }
    else if (ul_start_freq > 5000 && ul_end_freq < 5870)
    {
        return MAC_RC_START_FREQ_5;
    }
    else if (ul_start_freq > 4900 && ul_end_freq < 4999)
    {
        return MAC_RC_START_FREQ_5;
    }
    else
    {
        return MAC_RC_START_FREQ_BUTT;
    }
}


oal_uint8  wal_regdomain_get_bw_etc(oal_uint8 uc_bw)
{
    oal_uint8 uc_bw_map;

    switch (uc_bw)
    {
        case 80:
                uc_bw_map = MAC_CH_SPACING_80MHZ;
                break;
        case 40:
                uc_bw_map = MAC_CH_SPACING_40MHZ;
                break;
        case 20:
                uc_bw_map = MAC_CH_SPACING_20MHZ;
                break;
        default:
                uc_bw_map = MAC_CH_SPACING_BUTT;
                break;
    };

    return uc_bw_map;
}


oal_uint32  wal_regdomain_get_channel_2g_etc(oal_uint32 ul_start_freq, oal_uint32 ul_end_freq)
{
    oal_uint32 ul_freq;
    oal_uint32 ul_i;
    oal_uint32 ul_ch_bmap = 0;

    for (ul_freq = ul_start_freq + 10; ul_freq <= (ul_end_freq - 10); ul_freq++)
    {
        for (ul_i = 0; ul_i < MAC_CHANNEL_FREQ_2_BUTT; ul_i++)
        {
            if (ul_freq == g_ast_freq_map_2g_etc[ul_i].us_freq)
            {
                ul_ch_bmap |= (1 << ul_i);
            }
        }
    }

    return ul_ch_bmap;
}


oal_uint32  wal_regdomain_get_channel_5g_etc(oal_uint32 ul_start_freq, oal_uint32 ul_end_freq)
{
    oal_uint32 ul_freq;
    oal_uint32 ul_i;
    oal_uint32 ul_ch_bmap = 0;

    for (ul_freq = ul_start_freq + 10; ul_freq <= (ul_end_freq - 10); ul_freq += 5)
    {
        for (ul_i = 0; ul_i < MAC_CHANNEL_FREQ_5_BUTT; ul_i++)
        {
            if (ul_freq == g_ast_freq_map_5g_etc[ul_i].us_freq)
            {
                ul_ch_bmap |= (1 << ul_i);
            }
        }
    }

    return ul_ch_bmap;

}


oal_uint32  wal_regdomain_get_channel_etc(oal_uint8 uc_band, oal_uint32 ul_start_freq, oal_uint32 ul_end_freq)
{
    oal_uint32 ul_ch_bmap = 0;;

    switch (uc_band)
    {
        case MAC_RC_START_FREQ_2:
            ul_ch_bmap = wal_regdomain_get_channel_2g_etc(ul_start_freq, ul_end_freq);
            break;

        case MAC_RC_START_FREQ_5:
            ul_ch_bmap = wal_regdomain_get_channel_5g_etc(ul_start_freq, ul_end_freq);
            break;

        default:
            break;
    }

    return ul_ch_bmap;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,3,0))
extern oal_ieee80211_supported_band g_st_supported_band_2ghz_info;
oal_uint32 wal_linux_update_wiphy_channel_list_num_etc(oal_net_device_stru *pst_net_dev, oal_wiphy_stru *pst_wiphy)
{
    oal_uint16 us_len;
    oal_uint32 ul_ret;
    mac_vendor_cmd_channel_list_stru st_channel_list;
    mac_vap_stru                    *pst_mac_vap;

    if (OAL_ANY_NULL_PTR2(pst_wiphy,pst_net_dev))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_linux_update_wiphy_channel_list_num_etc::wiphy %p, net_dev %p}", (uintptr_t)pst_wiphy, (uintptr_t)pst_net_dev);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (pst_mac_vap == OAL_PTR_NULL)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_linux_update_wiphy_channel_list_num_etc::NET_DEV_PRIV is NULL.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = hmac_config_vendor_cmd_get_channel_list_etc(pst_mac_vap, &us_len, (oal_uint8 *)(&st_channel_list));
    if (ul_ret != OAL_SUCC)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_linux_update_wiphy_channel_list_num_etc::get_channel_list fail. %d}", ul_ret);
        return ul_ret;
    }

    /* ??????2G??????????5G????????????DFS ????????????????????????,?????????? */
    g_st_supported_band_2ghz_info.n_channels = st_channel_list.uc_channel_num_2g;

    OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_linux_update_wiphy_channel_list_num_etc::2g_channel_num = %d, 5g_channel_num = %d}",
                                st_channel_list.uc_channel_num_2g,
                                st_channel_list.uc_channel_num_5g);
    return OAL_SUCC;
}
#endif




OAL_STATIC OAL_INLINE oal_void  wal_get_dfs_domain(mac_regdomain_info_stru *pst_mac_regdom, OAL_CONST oal_int8 *pc_country)
{
    oal_uint32    u_idx;
    oal_uint32    ul_size = OAL_ARRAY_SIZE(g_ast_dfs_domain_table_etc);

    for (u_idx = 0; u_idx < ul_size; u_idx++)
    {
        if (0 == oal_strcmp(g_ast_dfs_domain_table_etc[u_idx].pc_country, pc_country))
        {
            pst_mac_regdom->en_dfs_domain = g_ast_dfs_domain_table_etc[u_idx].en_dfs_domain;

            return;
        }
    }

    pst_mac_regdom->en_dfs_domain = MAC_DFS_DOMAIN_NULL;
}


OAL_STATIC oal_void  wal_regdomain_fill_info(OAL_CONST oal_ieee80211_regdomain_stru *pst_regdom, mac_regdomain_info_stru *pst_mac_regdom)
{
    oal_uint32  ul_i;
    oal_uint32  ul_start;
    oal_uint32  ul_end;
    oal_uint8   uc_band;
    oal_uint8   uc_bw;

    /* ?????????????? */
    pst_mac_regdom->ac_country[0] = pst_regdom->alpha2[0];
    pst_mac_regdom->ac_country[1] = pst_regdom->alpha2[1];
    pst_mac_regdom->ac_country[2] = 0;

    /* ????DFS???????????? */
    wal_get_dfs_domain(pst_mac_regdom, pst_regdom->alpha2);

    /* ?????????????? */
    pst_mac_regdom->uc_regclass_num = (oal_uint8)pst_regdom->n_reg_rules;

    /* ?????????????? */
    for (ul_i = 0; ul_i < pst_regdom->n_reg_rules; ul_i++)
    {
        /* ????????????????(2.4G??5G) */
        ul_start = pst_regdom->reg_rules[ul_i].freq_range.start_freq_khz / 1000;
        ul_end   = pst_regdom->reg_rules[ul_i].freq_range.end_freq_khz / 1000;
        uc_band  = wal_regdomain_get_band_etc(ul_start, ul_end);
        pst_mac_regdom->ast_regclass[ul_i].en_start_freq = uc_band;

        /* ???????????????????????? */
        uc_bw = (oal_uint8)(pst_regdom->reg_rules[ul_i].freq_range.max_bandwidth_khz / 1000);
        pst_mac_regdom->ast_regclass[ul_i].en_ch_spacing = wal_regdomain_get_bw_etc(uc_bw);

        /* ?????????????????? */
        pst_mac_regdom->ast_regclass[ul_i].ul_channel_bmap = wal_regdomain_get_channel_etc(uc_band, ul_start, ul_end);

        /* ?????????????? */
        pst_mac_regdom->ast_regclass[ul_i].uc_behaviour_bmap = 0;

        if (pst_regdom->reg_rules[ul_i].flags & NL80211_RRF_DFS)
        {
            pst_mac_regdom->ast_regclass[ul_i].uc_behaviour_bmap |= MAC_RC_DFS;
        }

        if (pst_regdom->reg_rules[ul_i].flags & NL80211_RRF_NO_INDOOR)
        {
            pst_mac_regdom->ast_regclass[ul_i].uc_behaviour_bmap |= MAC_RC_NO_INDOOR;
        }

        if (pst_regdom->reg_rules[ul_i].flags & NL80211_RRF_NO_OUTDOOR)
        {
            pst_mac_regdom->ast_regclass[ul_i].uc_behaviour_bmap |= MAC_RC_NO_OUTDOOR;
        }

        /* ???????????????????????? */
        pst_mac_regdom->ast_regclass[ul_i].uc_coverage_class = 0;
        pst_mac_regdom->ast_regclass[ul_i].uc_max_reg_tx_pwr = (oal_uint8)(pst_regdom->reg_rules[ul_i].power_rule.max_eirp / 100);
        pst_mac_regdom->ast_regclass[ul_i].us_max_tx_pwr     = (oal_uint16)(pst_regdom->reg_rules[ul_i].power_rule.max_eirp / 10);

    }
}


oal_int32  wal_regdomain_update_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_country)
{
#ifndef _PRE_SUPPORT_ACS
    oal_uint8                               uc_dev_id;
    mac_device_stru                        *pst_device = OAL_PTR_NULL;
#endif
    OAL_CONST oal_ieee80211_regdomain_stru *pst_regdom;
    oal_uint16                              us_size;
    mac_regdomain_info_stru                *pst_mac_regdom = OAL_PTR_NULL;
    wal_msg_write_stru                      st_write_msg;
    mac_cfg_country_stru                   *pst_param = OAL_PTR_NULL;
    oal_int32                               l_ret;
    wal_msg_stru                           *pst_rsp_msg = OAL_PTR_NULL;
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    oal_int8 old_pc_country[COUNTRY_CODE_LEN] = {'9','9'};
#endif
#ifndef _PRE_SUPPORT_ACS
    mac_vap_stru                           *pst_mac_vap = OAL_PTR_NULL;
#endif

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    if (EOK != memcpy_s(old_pc_country, COUNTRY_CODE_LEN, hwifi_get_country_code_etc(), COUNTRY_CODE_LEN)) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_regdomain_update_etc::memcpy fail!");
        return -OAL_EINVAL;
    }
    hwifi_set_country_code_etc(pc_country, COUNTRY_CODE_LEN);

    /* ??????????????????????????????regdomain????????RF?????????????????? */
    if (OAL_TRUE == hwifi_is_regdomain_changed_etc((oal_uint8 *)old_pc_country, (oal_uint8 *)pc_country))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_regdomain_update_etc::regdomain changed, refresh rf params.!}\r\n");

        /* ?????????????????????????????????????????? */
        /* ?????????????????????????????????????? */
        if (hwifi_force_refresh_rf_params(pst_net_dev) != OAL_SUCC)
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY,
                "{wal_regdomain_update_etc::refresh rf(max_txpwr & dbb scale) params failed. Set country back.!}\r\n");
            hwifi_set_country_code_etc(old_pc_country, COUNTRY_CODE_LEN);
        }
    }
#endif


    if (!wal_is_alpha_upper(pc_country[0]) || !wal_is_alpha_upper(pc_country[1]))
    {
        if (('9' == pc_country[0]) && ('9' == pc_country[1]))
        {
            OAM_INFO_LOG0(0, OAM_SF_ANY, "{wal_regdomain_update_etc::set regdomain to 99!}\r\n");
        }
        else
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_regdomain_update_etc::country str is invalid!}\r\n");
            return -OAL_EINVAL;
        }
    }

    pst_regdom = wal_regdb_find_db_etc(pc_country);
    if (OAL_PTR_NULL == pst_regdom)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_regdomain_update_etc::no regdomain db was found!}\r\n");
        return -OAL_EINVAL;
    }

    us_size = (oal_uint16)(OAL_SIZEOF(mac_regclass_info_stru) * pst_regdom->n_reg_rules + MAC_RD_INFO_LEN);

    /* ??????????????????????????????????????????payload?????? */
    /* ????????????????????????????????(hmac_config_set_country_etc) */
    pst_mac_regdom = OAL_MEM_ALLOC(OAL_MEM_POOL_ID_LOCAL, us_size, OAL_TRUE);
    if (OAL_PTR_NULL == pst_mac_regdom)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_regdomain_update_etc::alloc regdom mem fail(size:%d), return null ptr!}\r\n",us_size);
        return -OAL_ENOMEM;
    }

    wal_regdomain_fill_info(pst_regdom, pst_mac_regdom);

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_COUNTRY, OAL_SIZEOF(mac_cfg_country_stru));

    /* ????WID?????????? */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    pst_mac_regdom->en_regdomain = hwifi_get_regdomain_from_country_code(pc_country);
#endif
    pst_param = (mac_cfg_country_stru *)(st_write_msg.auc_value);
    pst_param->p_mac_regdom = pst_mac_regdom;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_country_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_regdomain_update_etc::return err code %d!}\r\n", l_ret);
        if (OAL_VALUE_NE_ALL2(l_ret, -OAL_ETIMEDOUT, -OAL_EFAUL))
        {
            OAL_MEM_FREE(pst_mac_regdom, OAL_TRUE);
            pst_mac_regdom = OAL_PTR_NULL;
        }
        if (OAL_PTR_NULL != pst_rsp_msg)
        {
            oal_free(pst_rsp_msg);
        }
        return l_ret;
    }
    oal_free(pst_rsp_msg);

    /* ??????????ACS????????hostapd??????????; ????????????ACS????????????????????hostapd????????DFS???? */
#ifndef _PRE_SUPPORT_ACS
    pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_regdomain_update_etc::OAL_NET_DEV_PRIV(pst_net_dev) is null ptr.}");
        return -OAL_FAIL;
    }

    uc_dev_id = pst_mac_vap->uc_device_id;
    pst_device = mac_res_get_dev_etc(uc_dev_id);

    if((OAL_PTR_NULL != pst_device) && (OAL_PTR_NULL != pst_device->pst_wiphy))
    {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,3,0))
        
        wal_linux_update_wiphy_channel_list_num_etc(pst_net_dev, pst_device->pst_wiphy);
#endif

        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_regdomain_update_etc::update regdom to kernel.}\r\n");

        wal_cfg80211_reset_bands_etc(uc_dev_id);
        oal_wiphy_apply_custom_regulatory(pst_device->pst_wiphy, pst_regdom);
        
        wal_cfg80211_save_bands_etc(uc_dev_id);
    }
#endif

    return OAL_SUCC;
}

oal_int32  wal_regdomain_update_for_dfs_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_country)
{
    OAL_CONST oal_ieee80211_regdomain_stru *pst_regdom;
    oal_uint16                              us_size;
    mac_regdomain_info_stru                *pst_mac_regdom = OAL_PTR_NULL;
    wal_msg_write_stru                      st_write_msg;
    mac_dfs_domain_enum_uint8              *pst_param = OAL_PTR_NULL;
    oal_int32                               l_ret;

    if (!wal_is_alpha_upper(pc_country[0]) || !wal_is_alpha_upper(pc_country[1]))
    {
        if (('9' == pc_country[0]) && ('9' == pc_country[1]))
        {
            OAM_INFO_LOG0(0, OAM_SF_ANY, "{wal_regdomain_update_for_dfs_etc::set regdomain to 99!}\r\n");
        }
        else
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_regdomain_update_for_dfs_etc::country str is invalid!}\r\n");
            return -OAL_EINVAL;
        }
    }

    pst_regdom = wal_regdb_find_db_etc(pc_country);
    if (OAL_PTR_NULL == pst_regdom)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_regdomain_update_for_dfs_etc::no regdomain db was found!}\r\n");
        return -OAL_EINVAL;
    }

    us_size = (oal_uint16)(OAL_SIZEOF(mac_regclass_info_stru) * pst_regdom->n_reg_rules + MAC_RD_INFO_LEN);

    /* ??????????????????????,?????????????????? */
    pst_mac_regdom = OAL_MEM_ALLOC(OAL_MEM_POOL_ID_LOCAL, us_size, OAL_TRUE);
    if (OAL_PTR_NULL == pst_mac_regdom)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_regdomain_update_for_dfs_etc::alloc regdom mem fail, return null ptr!us_size[%d]}\r\n", us_size);
        return -OAL_ENOMEM;
    }

    wal_regdomain_fill_info(pst_regdom, pst_mac_regdom);

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_COUNTRY_FOR_DFS, OAL_SIZEOF(mac_dfs_domain_enum_uint8));

    /* ????WID?????????? */
    pst_param = (mac_dfs_domain_enum_uint8 *)(st_write_msg.auc_value);
    *pst_param = pst_mac_regdom->en_dfs_domain;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_dfs_domain_enum_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        /*pst_mac_regdom?????????????? */
        OAL_MEM_FREE(pst_mac_regdom, OAL_TRUE);
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_regdomain_update_for_dfs_etc::return err code %d!}\r\n", l_ret);
        return l_ret;
    }
    /*pst_mac_regdom?????????????? */
    OAL_MEM_FREE(pst_mac_regdom, OAL_TRUE);

    return OAL_SUCC;
}


oal_uint32  wal_regdomain_update_sta_etc(oal_uint8 uc_vap_id)
{
    oal_int8                      *pc_desired_country = OAL_PTR_NULL;

    oal_net_device_stru           *pst_net_dev = OAL_PTR_NULL;
    oal_int32                      l_ret;
    oal_bool_enum_uint8            us_updata_rd_by_ie_switch = OAL_FALSE;

    hmac_vap_get_updata_rd_by_ie_switch_etc(uc_vap_id, &us_updata_rd_by_ie_switch);

    if(OAL_TRUE == us_updata_rd_by_ie_switch)
    {
        pc_desired_country = hmac_vap_get_desired_country_etc(uc_vap_id);

        if (OAL_UNLIKELY(OAL_PTR_NULL == pc_desired_country))
        {
            OAM_ERROR_LOG0(uc_vap_id, OAM_SF_ANY, "{wal_regdomain_update_sta_etc::pc_desired_country is null ptr!}\r\n");
            return OAL_ERR_CODE_PTR_NULL;
        }


        /* ????????????????0??????????AP????????????????????sta???????????????? */
        if ((0 == pc_desired_country[0]) && (0 == pc_desired_country[1]))
        {
            OAM_INFO_LOG0(uc_vap_id, OAM_SF_ANY, "{wal_regdomain_update_sta_etc::ap does not have country ie, use default!}\r\n");
            return OAL_SUCC;
        }

        pst_net_dev = hmac_vap_get_net_device_etc(uc_vap_id);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_net_dev))
        {
            OAM_ERROR_LOG0(uc_vap_id, OAM_SF_ANY, "{wal_regdomain_update_sta_etc::pst_net_dev is null ptr!}\r\n");
            return OAL_ERR_CODE_PTR_NULL;
        }

        l_ret = wal_regdomain_update_for_dfs_etc(pst_net_dev, pc_desired_country);
        if (l_ret != OAL_SUCC)
        {
            OAM_WARNING_LOG1(uc_vap_id, OAM_SF_ANY, "{wal_regdomain_update_sta_etc::wal_regdomain_update_etc err code %d!}\r\n", l_ret);
            return OAL_FAIL;
        }

        l_ret = wal_regdomain_update_etc(pst_net_dev, pc_desired_country);
        if (l_ret != OAL_SUCC)
        {
            OAM_WARNING_LOG1(uc_vap_id, OAM_SF_ANY, "{wal_regdomain_update_sta_etc::wal_regdomain_update_etc err code %d!}\r\n", l_ret);
            return OAL_FAIL;
        }

    }
    else
    {
        OAM_INFO_LOG0(uc_vap_id, OAM_SF_ANY, "{wal_regdomain_update_sta_etc::us_updata_rd_by_ie_switch is OAL_FALSE!}\r\n");
    }
    return OAL_SUCC;
}


oal_int32 wal_regdomain_update_country_code_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_country)
{
    oal_int32   l_ret;

    if (pst_net_dev == OAL_PTR_NULL || pc_country == OAL_PTR_NULL)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_regdomain_update_country_code_etc::null ptr.net_dev %p!}",
                        (uintptr_t)pst_net_dev);
        return -OAL_EFAIL;
    }

    /* ????????????wifi ???? */
    l_ret = wal_regdomain_update_for_dfs_etc(pst_net_dev, pc_country);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_regdomain_update_country_code_etc::update_for_dfs return err code [%d]!}\r\n", l_ret);
        return -OAL_EFAIL;
    }

    l_ret = wal_regdomain_update_etc(pst_net_dev, pc_country);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_regdomain_update_country_code_etc::update return err code [%d]!}\r\n", l_ret);
        return -OAL_EFAIL;
    }
    return OAL_SUCC;
}

#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifndef _PRE_PC_LINT

OAL_STATIC oal_int32 wal_get_primary_mac_addr_etc(mac_device_stru *pst_mac_device, oal_uint8 *puc_primary_mac_addr, oal_wireless_dev_stru * pst_wdev)
{
    if(OAL_LIKELY(OAL_PTR_NULL != OAL_NETDEVICE_MAC_ADDR(pst_mac_device->st_p2p_info.pst_primary_net_device)))
    {
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
        if(wlan_customize_etc.uc_random_mac_addr_connect)
        {
            if(NL80211_IFTYPE_STATION == pst_wdev->iftype)
            {
                oal_set_mac_addr((oal_uint8 *)OAL_NETDEVICE_MAC_ADDR(pst_mac_device->st_p2p_info.pst_primary_net_device),g_auc_wifistamac_etc);
            }
            else if (NL80211_IFTYPE_AP == pst_wdev->iftype)
            {
                oal_set_mac_addr((oal_uint8 *)OAL_NETDEVICE_MAC_ADDR(pst_mac_device->st_p2p_info.pst_primary_net_device),g_auc_wifiapmac_etc);
            }
            else
            {
            }
        }
#endif
        if (EOK != memcpy_s(puc_primary_mac_addr,
                            WLAN_MAC_ADDR_LEN,
                            OAL_NETDEVICE_MAC_ADDR(pst_mac_device->st_p2p_info.pst_primary_net_device),
                            WLAN_MAC_ADDR_LEN)) {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_get_primary_mac_addr_etc: memcpy fail!}\r\n");
            return OAL_FAIL;
        }

        OAM_WARNING_LOG4(0, OAM_SF_ANY, "wal_get_primary_mac_addr_etc: iftype=[%d][2:STA 3:AP],mac is:%02X:XX:XX:XX:%02X:%02X\n",pst_wdev->iftype,puc_primary_mac_addr[0], puc_primary_mac_addr[4], puc_primary_mac_addr[5]);
        return OAL_SUCC;
    }
    else
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_get_primary_mac_addr_etc: pst_primary_net_device, addr is null}\r\n");
        return OAL_FAIL;
    }
}
#endif


OAL_STATIC oal_void wal_set_p2p_mac_addr_etc(mac_device_stru *pst_mac_device, oal_uint8 *puc_primary_mac_addr, oal_uint8  *auc_wlan_addr)
{
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    if(wlan_customize_etc.uc_random_mac_addr_connect)
    {
        if (EOK != memcpy_s(auc_wlan_addr,
                            WLAN_MAC_ADDR_LEN,
                            g_auc_wifiGCGOmac_etc,
                            WLAN_MAC_ADDR_LEN)) {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_set_p2p_mac_addr_etc::memcpy fail!");
            return;
        }
    }
    else
#endif
    {
        if (EOK != memcpy_s(auc_wlan_addr,
                            WLAN_MAC_ADDR_LEN,
                            puc_primary_mac_addr,
                            WLAN_MAC_ADDR_LEN)) {
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_set_p2p_mac_addr_etc::memcpy fail!");
            return;
        }
        auc_wlan_addr[0] |= 0x02;
        auc_wlan_addr[4] ^= 0x80;
    }

    OAM_WARNING_LOG3(0, OAM_SF_ANY, "wal_set_p2p_mac_addr_etc:set p2p GC/GO,mac is:%02X:XX:XX:XX:%02X:%02X\n",auc_wlan_addr[0], auc_wlan_addr[4], auc_wlan_addr[5]);
}

oal_int32  wal_set_random_mac_to_mib_etc(oal_net_device_stru *pst_net_dev)
{
    oal_uint32                    ul_ret;
    frw_event_mem_stru           *pst_event_mem = OAL_PTR_NULL;
    wal_msg_stru                 *pst_cfg_msg = OAL_PTR_NULL;
    wal_msg_write_stru           *pst_write_msg = OAL_PTR_NULL;
    mac_cfg_staion_id_param_stru *pst_param = OAL_PTR_NULL;
    mac_vap_stru                 *pst_mac_vap;
    oal_uint8                    *puc_mac_addr = OAL_PTR_NULL;
#ifdef _PRE_WLAN_FEATURE_P2P
    oal_wireless_dev_stru        *pst_wdev; /* ????P2P ??????p2p0 ?? p2p-p2p0 MAC ??????wlan0 ???? */
    mac_device_stru              *pst_mac_device;
    wlan_p2p_mode_enum_uint8      en_p2p_mode = WLAN_LEGACY_VAP_MODE;
    oal_uint8                     auc_primary_mac_addr[WLAN_MAC_ADDR_LEN] = {0};    /* MAC???? */
#endif
    oal_uint8                     *auc_wlan_addr = OAL_PTR_NULL;
#ifdef _PRE_WLAN_FEATURE_P2P
    oal_uint8                     *auc_p2p0_addr;
#endif
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    oal_int32                      l_ret = EOK;
#endif
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_set_random_mac_to_mib_etc::pst_mac_vap NULL}");
        return OAL_FAIL;
    }

    if(OAL_PTR_NULL == pst_mac_vap->pst_mib_info)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_random_mac_to_mib_etc::vap->mib_info is NULL !}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    auc_wlan_addr = mac_mib_get_StationID(pst_mac_vap);
#ifdef _PRE_WLAN_FEATURE_P2P
    auc_p2p0_addr = mac_mib_get_p2p0_dot11StationID(pst_mac_vap);
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
    /* ini ????,????wlan0 MAC ??????????p2p0/p2p-p2p0 MAC ???? */
    /* ini ????,????????????????????wlan0??p2p0??????p2p-p2p0 MAC ???? */
    pst_mac_device = (mac_device_stru *)mac_res_get_dev_etc(pst_mac_vap->uc_device_id);
    if (NULL == pst_mac_device)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_set_random_mac_to_mib_etc::pst_mac_device NULL, device_id:%d}",pst_mac_vap->uc_device_id);
        return OAL_FAIL;
    }
    pst_wdev = pst_net_dev->ieee80211_ptr;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_device->st_p2p_info.pst_primary_net_device))
    {
        /* random mac will be used. hi1102-cb (#include <linux/etherdevice.h>)    */
        oal_random_ether_addr(auc_primary_mac_addr);
        auc_primary_mac_addr[0] &= (~0x02);
        auc_primary_mac_addr[1] = 0x11;
        auc_primary_mac_addr[2] = 0x02;
    }
    else
    {
    #ifndef _PRE_PC_LINT
        ul_ret = wal_get_primary_mac_addr_etc(pst_mac_device, auc_primary_mac_addr, pst_wdev);
        if(ul_ret != OAL_SUCC)
        {
            return OAL_FAIL;
        }
    #endif
    }

    switch (pst_wdev->iftype)
    {
        case NL80211_IFTYPE_P2P_DEVICE:
           en_p2p_mode = WLAN_P2P_DEV_MODE;
        #if defined(_PRE_PRODUCT_ID_HI110X_HOST)
            /* ????P2P device MAC ????????????mac ????bit ??????1 */
            l_ret += memcpy_s(auc_p2p0_addr,
                              WLAN_MAC_ADDR_LEN,
                              OAL_NETDEVICE_MAC_ADDR(pst_net_dev),
                              WLAN_MAC_ADDR_LEN);

            OAM_WARNING_LOG3(0, OAM_SF_ANY, "wal_set_random_mac_to_mib_etc:p2p0 mac is:%02X:XX:XX:XX:%02X:%02X\n",auc_p2p0_addr[0], auc_p2p0_addr[4], auc_p2p0_addr[5]);
        #endif
            break;
        case NL80211_IFTYPE_P2P_CLIENT:
        case NL80211_IFTYPE_P2P_GO:
            en_p2p_mode = (NL80211_IFTYPE_P2P_CLIENT == pst_wdev->iftype) ? WLAN_P2P_CL_MODE : WLAN_P2P_GO_MODE;

            /* ????P2P interface MAC ???? */
            wal_set_p2p_mac_addr_etc(pst_mac_device, auc_primary_mac_addr, auc_wlan_addr);
            break;
        default:
        #if defined(_PRE_PRODUCT_ID_HI110X_HOST)
            if(0 == (oal_strcmp("p2p0", pst_net_dev->name)))
            {
                en_p2p_mode = WLAN_P2P_DEV_MODE;
                 /* ????P2P device MAC ????????????mac ????bit ??????1 */
                l_ret += memcpy_s(auc_p2p0_addr,
                                  WLAN_MAC_ADDR_LEN,
                                  OAL_NETDEVICE_MAC_ADDR(pst_net_dev),
                                  WLAN_MAC_ADDR_LEN);
                break;
            }

            l_ret += memcpy_s(auc_wlan_addr,
                              WLAN_MAC_ADDR_LEN,
                              OAL_NETDEVICE_MAC_ADDR(pst_net_dev),
                              WLAN_MAC_ADDR_LEN);
        #endif
            break;
    }
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_set_random_mac_to_mib_etc::memcpy fail!");
        return OAL_FAIL;
    }
#endif
#else
    /* random mac will be used. hi1102-cb (#include <linux/etherdevice.h>)    */
    oal_random_ether_addr(auc_wlan_addr);
    auc_wlan_addr[0]&=(~0x02);
    auc_wlan_addr[1]=0x11;
    auc_wlan_addr[2]=0x02;
#endif

    /* send the random mac to dmac */
    /***************************************************************************
        ????????wal??????   copy from wal_netdev_set_mac_addr()
        gong TBD : ??????????????config????
    ***************************************************************************/
    ul_ret = wal_alloc_cfg_event_etc(pst_net_dev, &pst_event_mem, NULL, &pst_cfg_msg, (WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_staion_id_param_stru)));     /* ???????? */
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_set_random_mac_to_mib_etc() fail; return %d!}\r\n", ul_ret);
        return -OAL_ENOMEM;
    }

    /* ???????????? */
    WAL_CFG_MSG_HDR_INIT(&(pst_cfg_msg->st_msg_hdr),
                         WAL_MSG_TYPE_WRITE,
                         WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_staion_id_param_stru),
                         WAL_GET_MSG_SN());

    /* ????WID???? */
    pst_write_msg = (wal_msg_write_stru *)pst_cfg_msg->auc_msg_data;
    WAL_WRITE_MSG_HDR_INIT(pst_write_msg, WLAN_CFGID_STATION_ID, OAL_SIZEOF(mac_cfg_staion_id_param_stru));

    pst_param = (mac_cfg_staion_id_param_stru *)pst_write_msg->auc_value;   /* ????WID?????????? */
#ifdef _PRE_WLAN_FEATURE_P2P
    /* ????????P2P????????netdevice ??????P2P ??????????????????????hmac ??dmac */
    /* ????????????????p2p0 ??p2p-p2p0 cl */
    pst_param->en_p2p_mode = en_p2p_mode;
    if (en_p2p_mode == WLAN_P2P_DEV_MODE)
    {
        puc_mac_addr = mac_mib_get_p2p0_dot11StationID(pst_mac_vap);
    }
    else
#endif
    {
        puc_mac_addr = mac_mib_get_StationID(pst_mac_vap);
    }
    oal_set_mac_addr(pst_param->auc_station_id, puc_mac_addr);

    frw_event_dispatch_event_etc(pst_event_mem);    /* ???????? */
    FRW_EVENT_FREE(pst_event_mem);

    return OAL_SUCC;
}
#endif


OAL_STATIC oal_uint32  wal_hipriv_setcountry(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
#ifdef _PRE_WLAN_FEATURE_11D
    oal_int32                        l_ret;
    oal_uint32                       ul_ret;
    oal_uint32                       ul_off_set;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int8                        *puc_para = OAL_PTR_NULL;

    /* ??????up??????????????????????down */
    if (0 != (OAL_IFF_RUNNING & OAL_NETDEVICE_FLAGS(pst_net_dev)))
    {
        OAM_INFO_LOG1(0, OAM_SF_ANY, "{wal_hipriv_setcountry::net_dev flags: %d!}\r\n", OAL_NETDEVICE_FLAGS(pst_net_dev));
        return OAL_EBUSY;
    }

    /* ???????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_setcountry::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }
    puc_para = &ac_arg[0];

    l_ret = wal_regdomain_update_for_dfs_etc(pst_net_dev, puc_para);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_setcountry::regdomain_update return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
    l_ret = wal_regdomain_update_etc(pst_net_dev, puc_para);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_setcountry::regdomain_update return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
#else
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_setcountry::_PRE_WLAN_FEATURE_11D is not define!}\r\n");
#endif
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    cust_country_code_ignore_flag.en_country_code_ingore_hipriv_flag = OAL_TRUE;
#endif

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_getcountry(oal_net_device_stru *pst_net_dev,oal_int8 *pc_param)
{
#ifdef _PRE_WLAN_FEATURE_11D
    wal_msg_stru                *pst_rsp_msg = OAL_PTR_NULL;
    wal_msg_rsp_stru            *pst_query_rsp_msg = OAL_PTR_NULL;
    wal_msg_query_stru           st_query_msg;
    oal_int8                     ac_tmp_buff[OAM_PRINT_FORMAT_LENGTH];
    oal_int32                    l_ret;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    st_query_msg.en_wid = WLAN_CFGID_COUNTRY;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_QUERY,
                               WAL_MSG_WID_LENGTH ,
                               (oal_uint8 *)&st_query_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        if (pst_rsp_msg != OAL_PTR_NULL)
        {
            oal_free(pst_rsp_msg);
        }

        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_getcountry::wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    /* ???????????? */
    pst_query_rsp_msg = (wal_msg_rsp_stru *)(pst_rsp_msg->auc_msg_data);

    l_ret = snprintf_s(ac_tmp_buff, sizeof(ac_tmp_buff), sizeof(ac_tmp_buff) - 1,
                       "getcountry code is : %c%c.\n",
                        pst_query_rsp_msg->auc_value[0],
                        pst_query_rsp_msg->auc_value[1]);
    if (l_ret < 0) {
        OAM_ERROR_LOG0(0, OAM_SF_CFG, "hmac_config_list_sta_etc::snprintf_s error!");
        oal_free(pst_rsp_msg);
        oam_print_etc(ac_tmp_buff);
        return OAL_FAIL;
    }

    OAM_WARNING_LOG3(0, OAM_SF_CFG, "{wal_hipriv_getcountry:: %c, %c, len %d}",
                pst_query_rsp_msg->auc_value[0],
                pst_query_rsp_msg->auc_value[1],
                pst_query_rsp_msg->us_len);

    oal_free(pst_rsp_msg);
    oam_print_etc(ac_tmp_buff);

#else
    OAM_INFO_LOG0(0, OAM_SF_ANY, "{wal_hipriv_getcountry::_PRE_WLAN_FEATURE_11D is not define!}\r\n");
#endif

    return OAL_SUCC;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,34))

OAL_STATIC oal_void *wal_sta_info_seq_start(struct seq_file *f, loff_t *pos)
{
    if (0 == *pos)
    {
        return f->private;
    }
    else
    {
        return NULL;
    }
}


OAL_STATIC oal_int32 wal_sta_info_seq_show(struct seq_file *f, void *v)
{
#define TID_STAT_TO_USER(_stat) ((_stat[0])+(_stat[1])+(_stat[2])+(_stat[3])+(_stat[4])+(_stat[5])+(_stat[6])+(_stat[7]))
#define BW_ENUM_TO_NUMBER(_bw)  ((_bw) == 0 ? 20 : (_bw) == 1 ? 40 : 80)

    mac_vap_stru                    *pst_mac_vap  = (mac_vap_stru *)v;
    hmac_vap_stru                   *pst_hmac_vap;
    oal_dlist_head_stru             *pst_entry;
    oal_dlist_head_stru             *pst_dlist_tmp;
    mac_user_stru                   *pst_user_tmp;
    hmac_user_stru                  *pst_hmac_user_tmp;
    oal_uint8                       *puc_addr;
    oal_uint16                       us_idx = 1;
    oam_stat_info_stru              *pst_oam_stat;
    oam_user_stat_info_stru         *pst_oam_user_stat;
    oal_uint32                       ul_curr_time;
    oal_int8                        *pac_protocol2string[] = {"11a", "11b", "11g", "11g", "11g", "11n", "11ac", "11n", "11ac", "11n","error"};
    wal_msg_write_stru               st_write_msg;
    oal_int32                        l_ret;
    mac_cfg_query_rssi_stru         *pst_query_rssi_param;
    mac_cfg_query_rate_stru         *pst_query_rate_param;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_sta_info_seq_show: hmac vap is null. vap id:%d", pst_mac_vap->uc_vap_id);
        return 0;
    }

    /* step1. ????????????dmac???? */
    OAL_DLIST_SEARCH_FOR_EACH_SAFE(pst_entry, pst_dlist_tmp, &(pst_mac_vap->st_mac_user_list_head))
    {
        pst_user_tmp = OAL_DLIST_GET_ENTRY(pst_entry, mac_user_stru, st_user_dlist);
        if (OAL_PTR_NULL == pst_user_tmp)
        {
            continue;
        }

        pst_hmac_user_tmp = mac_res_get_hmac_user_etc(pst_user_tmp->us_assoc_id);
        if (OAL_PTR_NULL == pst_hmac_user_tmp)
        {
            continue;
        }

        if (OAL_PTR_NULL == pst_dlist_tmp)
        {
            /* ??for????????????????????????????????????????????pst_dlist_tmp????????????????????????dmac???? */
            break;
        }

        /***********************************************************************/
        /*                  ????dmac user??RSSI????                            */
        /***********************************************************************/
        memset_s(&st_write_msg, OAL_SIZEOF(st_write_msg), 0, OAL_SIZEOF(st_write_msg));
        WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_QUERY_RSSI, OAL_SIZEOF(mac_cfg_query_rssi_stru));
        pst_query_rssi_param = (mac_cfg_query_rssi_stru *)st_write_msg.auc_value;

        pst_query_rssi_param->us_user_id = pst_user_tmp->us_assoc_id; /* ????????id?????? */

        l_ret = wal_send_cfg_event_etc(pst_hmac_vap->pst_net_device,
                                   WAL_MSG_TYPE_WRITE,
                                   WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_query_rssi_stru),
                                   (oal_uint8 *)&st_write_msg,
                                   OAL_FALSE,
                                   OAL_PTR_NULL);
        if (OAL_SUCC != l_ret)
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_sta_info_seq_show: send query rssi cfg event ret:%d", l_ret);
        }

        pst_hmac_vap->station_info_query_completed_flag = OAL_FALSE;
        /*lint -e730*/
        l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_vap->query_wait_q,(OAL_TRUE == pst_hmac_vap->station_info_query_completed_flag), 5*OAL_TIME_HZ);
        /*lint +e730*/
        if (l_ret <= 0) /* ?????????????? */
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_sta_info_seq_show: query rssi timeout. ret:%d", l_ret);
        }

        /***********************************************************************/
        /*                  ????dmac user??????????                            */
        /***********************************************************************/
        memset_s(&st_write_msg, OAL_SIZEOF(st_write_msg), 0, OAL_SIZEOF(st_write_msg));
        WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_QUERY_RATE, OAL_SIZEOF(mac_cfg_query_rate_stru));
        pst_query_rate_param = (mac_cfg_query_rate_stru *)st_write_msg.auc_value;

        pst_query_rate_param->us_user_id = pst_user_tmp->us_assoc_id; /* ????????id?????? */

        l_ret = wal_send_cfg_event_etc(pst_hmac_vap->pst_net_device,
                                   WAL_MSG_TYPE_WRITE,
                                   WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_query_rate_stru),
                                   (oal_uint8 *)&st_write_msg,
                                   OAL_FALSE,
                                   OAL_PTR_NULL);
        if (OAL_SUCC != l_ret)
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_sta_info_seq_show: send query rate cfg event ret:%d", l_ret);
        }

        pst_hmac_vap->station_info_query_completed_flag = OAL_FALSE;
        /*lint -e730*/
        l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_vap->query_wait_q, (OAL_TRUE == pst_hmac_vap->station_info_query_completed_flag), 5*OAL_TIME_HZ);
        /*lint +e730*/
        if (l_ret <= 0) /* ?????????????? */
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_sta_info_seq_show: query rate timeout. ret:%d", l_ret);
        }

    }

    /* step2. proc???????????????? */
    seq_printf(f, "Total user nums: %d\n", pst_mac_vap->us_user_nums);
    seq_printf(f, "-- STA info table --\n");

    pst_oam_stat = OAM_STAT_GET_STAT_ALL();
    ul_curr_time = (oal_uint32)OAL_TIME_GET_STAMP_MS();

    OAL_DLIST_SEARCH_FOR_EACH_SAFE(pst_entry, pst_dlist_tmp, &(pst_mac_vap->st_mac_user_list_head))
    {
        pst_user_tmp = OAL_DLIST_GET_ENTRY(pst_entry, mac_user_stru, st_user_dlist);
        if (OAL_PTR_NULL == pst_user_tmp)
        {
            continue;
        }

        pst_hmac_user_tmp = mac_res_get_hmac_user_etc(pst_user_tmp->us_assoc_id);
        if (OAL_PTR_NULL == pst_hmac_user_tmp)
        {
            continue;
        }

        pst_oam_user_stat = &(pst_oam_stat->ast_user_stat_info[pst_user_tmp->us_assoc_id]);
        puc_addr = pst_user_tmp->auc_user_mac_addr;

        seq_printf(f, "%2d: aid: %d\n"
                      "    MAC ADDR: %02X:%02X:%02X:%02X:%02X:%02X\n"
                      "    status: %d\n"
                      "    BW: %d\n"
                      "    NSS: %d\n"
                      "    RSSI: %d\n"
                      "    phy type: %s\n"
                      "    TX rate: %dkbps\n"
                      "    RX rate: %dkbps\n"
                      "    RX rate_min: %dkbps\n"
                      "    RX rate_max: %dkbps\n"
                      "    user online time: %us\n"
                      "    TX packets succ: %u\n"
                      "    TX packets fail: %u\n"
                      "    RX packets succ: %u\n"
                      "    RX packets fail: %u\n"
                      "    TX power: %ddBm\n"
                      "    TX bytes: %u\n"
                      "    RX bytes: %u\n"
                      "    TX retries: %u\n"
#ifdef _PRE_WLAN_DFT_STAT
                      "    Curr_rate PER: %u\n"
                      "    Best_rate PER: %u\n"
                      "    Tx Throughput: %u\n" /*??????*/
#endif
                      ,us_idx,
                      pst_user_tmp->us_assoc_id,
                      puc_addr[0],puc_addr[1],puc_addr[2],puc_addr[3],puc_addr[4],puc_addr[5],
                      pst_user_tmp->en_user_asoc_state,     /* status */
                      BW_ENUM_TO_NUMBER(pst_user_tmp->en_avail_bandwidth),
                      (pst_user_tmp->en_avail_num_spatial_stream+1),   /* NSS,??1????????????????????????????????0???????? */
                      pst_hmac_user_tmp->c_rssi,
                      pac_protocol2string[pst_user_tmp->en_avail_protocol_mode],
                      pst_hmac_user_tmp->ul_tx_rate,
                      pst_hmac_user_tmp->ul_rx_rate,
                      pst_hmac_user_tmp->ul_rx_rate_min,
                      pst_hmac_user_tmp->ul_rx_rate_max,
                      (oal_uint32)OAL_TIME_GET_RUNTIME(pst_hmac_user_tmp->ul_first_add_time, ul_curr_time)/1000,
                      TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_mpdu_succ_num)+TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_mpdu_in_ampdu),
                      TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_mpdu_fail_num)+TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_mpdu_fail_in_ampdu),
                      pst_oam_user_stat->ul_rx_mpdu_num,   /* RX packets succ */
                      0,
                      20,                                  /* TX power, ???????? ????????tpc????tx_power???? */
                      TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_mpdu_bytes)+TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_ampdu_bytes),
                      pst_oam_user_stat->ul_rx_mpdu_bytes, /* RX bytes */
                      pst_oam_user_stat->ul_tx_ppdu_retries/* TX retries */
#ifdef _PRE_WLAN_DFT_STAT
                      ,pst_hmac_user_tmp->uc_cur_per,
                      pst_hmac_user_tmp->uc_bestrate_per,
                      0
#endif
                      );

        us_idx++;

        if (OAL_PTR_NULL == pst_dlist_tmp)
        {
            break;
        }
    }

#undef TID_STAT_TO_USER
#undef BW_ENUM_TO_NUMBER
    return 0;
}


OAL_STATIC oal_void *wal_sta_info_seq_next(struct seq_file *f, void *v, loff_t *pos)
{
    return NULL;
}


OAL_STATIC oal_void wal_sta_info_seq_stop(struct seq_file *f, void *v)
{

}

/*****************************************************************************
    dmac_sta_info_seq_ops: ????seq_file ops
*****************************************************************************/
OAL_STATIC OAL_CONST struct seq_operations wal_sta_info_seq_ops = {
    .start = wal_sta_info_seq_start,
    .next  = wal_sta_info_seq_next,
    .stop  = wal_sta_info_seq_stop,
    .show  = wal_sta_info_seq_show
};


OAL_STATIC oal_int32 wal_sta_info_seq_open(struct inode *inode, struct file *filp)
{
    oal_int32               l_ret;
    struct seq_file        *pst_seq_file;
    struct proc_dir_entry  *pde = PDE(inode);

    l_ret = seq_open(filp, &wal_sta_info_seq_ops);
    if (OAL_SUCC == l_ret)
    {
        pst_seq_file = (struct seq_file *)filp->private_data;

        pst_seq_file->private = pde->data;
    }

    return l_ret;
}

/*****************************************************************************
    gst_sta_info_proc_fops: ????sta info proc fops
*****************************************************************************/
OAL_STATIC OAL_CONST struct file_operations gst_sta_info_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = wal_sta_info_seq_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = seq_release
};


OAL_STATIC int  wal_read_vap_info_proc(char *page, char **start, off_t off,
                   int count, int *eof, void *data)
{
    int                      len;
    mac_vap_stru            *pst_mac_vap = (mac_vap_stru *)data;
    oam_stat_info_stru      *pst_oam_stat;
    oam_vap_stat_info_stru  *pst_oam_vap_stat;

#ifdef _PRE_WLAN_DFT_STAT
    mac_cfg_query_ani_stru          *pst_query_ani_param;
    wal_msg_write_stru               st_write_msg;
    oal_int32                        l_ret;
    hmac_vap_stru                   *pst_hmac_vap;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_read_vap_info_proc: hmac vap is null. vap id:%d", pst_mac_vap->uc_vap_id);
        return 0;
    }
    /***********************************************************************/
    /*                  ????dmac vap??ANI????                            */
    /***********************************************************************/
    memset_s(&st_write_msg, OAL_SIZEOF(st_write_msg), 0, OAL_SIZEOF(st_write_msg));
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_QUERY_RSSI, OAL_SIZEOF(mac_cfg_query_ani_stru));
    pst_query_ani_param = (mac_cfg_query_ani_stru *)st_write_msg.auc_value;

    l_ret = wal_send_cfg_event_etc(pst_hmac_vap->pst_net_device,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_query_ani_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_read_vap_info_proc: send query ani cfg event ret:%d", l_ret);
    }

    pst_hmac_vap->station_info_query_completed_flag = OAL_FALSE;
    /*lint -e730*/
    l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_vap->query_wait_q,(OAL_TRUE == pst_hmac_vap->station_info_query_completed_flag), 5*OAL_TIME_HZ);
    /*lint +e730*/
    if (l_ret <= 0) /* ?????????????? */
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_read_vap_info_proc: query ani timeout. ret:%d", l_ret);
    }
#endif

    pst_oam_stat = OAM_STAT_GET_STAT_ALL();

    pst_oam_vap_stat = &(pst_oam_stat->ast_vap_stat_info[pst_mac_vap->uc_vap_id]);

    len = snprintf_s(page, PAGE_SIZE, PAGE_SIZE - 1,
                     "vap stats:\n"
                     "  TX bytes: %u\n"
                     "  TX packets: %u\n"
                     "  TX packets error: %u\n"
                     "  TX packets discard: %u\n"
                     "  TX unicast packets: %u\n"
                     "  TX multicast packets: %u\n"
                     "  TX broadcast packets: %u\n",
                     pst_oam_vap_stat->ul_tx_bytes_from_lan,
                     pst_oam_vap_stat->ul_tx_pkt_num_from_lan,
                     pst_oam_vap_stat->ul_tx_abnormal_msdu_dropped +
                     pst_oam_vap_stat->ul_tx_security_check_faild +
                     pst_oam_vap_stat->ul_tx_abnormal_mpdu_dropped,
                     pst_oam_vap_stat->ul_tx_uapsd_process_dropped +
                     pst_oam_vap_stat->ul_tx_psm_process_dropped +
                     pst_oam_vap_stat->ul_tx_alg_process_dropped,
                     pst_oam_vap_stat->ul_tx_pkt_num_from_lan - pst_oam_vap_stat->ul_tx_m2u_mcast_cnt,
                     0,
                     pst_oam_vap_stat->ul_tx_m2u_mcast_cnt);

    if (len >= PAGE_SIZE || len < 0)
    {
        OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_read_vap_info_proc:len: %d out of page size: %d", len, PAGE_SIZE);
        return (PAGE_SIZE - 1);
    }
    len += snprintf_s(page + len, PAGE_SIZE - len, PAGE_SIZE - len - 1,
                      "  RX bytes: %u\n"
                      "  RX packets: %u\n"
                      "  RX packets error: %u\n"
                      "  RX packets discard: %u\n"
                      "  RX unicast packets: %u\n"
                      "  RX multicast packets: %u\n"
                      "  RX broadcast packets: %u\n"
                      "  RX unhnown protocol packets: %u\n"
#ifdef _PRE_WLAN_DFT_STAT
                      "  Br_rate_num: %u\n"
                      "  Nbr_rate_num: %u\n"
                      "  Max_rate: %u\n"
                      "  Min_rate: %u\n"
                      "  Channel num: %d\n"
                      "  ANI:\n"
                      "    dmac_device_distance: %d\n"
                      "    cca_intf_state: %d\n"
                      "    co_intf_state: %d\n"
#endif
                      ,pst_oam_vap_stat->ul_rx_bytes_to_lan,
                      pst_oam_vap_stat->ul_rx_pkt_to_lan,
                      pst_oam_vap_stat->ul_rx_defrag_process_dropped +
                      pst_oam_vap_stat->ul_rx_alg_process_dropped +
                      pst_oam_vap_stat->ul_rx_abnormal_dropped,
                      pst_oam_vap_stat->ul_rx_no_buff_dropped +
                      pst_oam_vap_stat->ul_rx_ta_check_dropped +
                      pst_oam_vap_stat->ul_rx_da_check_dropped +
                      pst_oam_vap_stat->ul_rx_replay_fail_dropped +
                      pst_oam_vap_stat->ul_rx_key_search_fail_dropped,
                      pst_oam_vap_stat->ul_rx_pkt_to_lan - pst_oam_vap_stat->ul_rx_mcast_cnt,
                      0,
                      pst_oam_vap_stat->ul_rx_mcast_cnt,
                      0
#ifdef _PRE_WLAN_DFT_STAT
                      ,pst_mac_vap->st_curr_sup_rates.uc_br_rate_num,
                      pst_mac_vap->st_curr_sup_rates.uc_nbr_rate_num,
                      pst_mac_vap->st_curr_sup_rates.uc_max_rate,
                      pst_mac_vap->st_curr_sup_rates.uc_min_rate,
                      pst_mac_vap->st_channel.uc_chan_number,
                      pst_hmac_vap->uc_device_distance,
                      pst_hmac_vap->uc_intf_state_cca,
                      pst_hmac_vap->uc_intf_state_co
#endif
                      );

    if (len >= PAGE_SIZE)
    {
        OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_read_vap_info_proc:len: %d out of page size: %d", len, PAGE_SIZE);
        return (PAGE_SIZE - 1);
    }
    return len;
}


OAL_STATIC int  wal_read_rf_info_proc(char *page, char **start, off_t off,
                   int count, int *eof, void *data)
{
    int                  len;
    mac_vap_stru        *pst_mac_vap = (mac_vap_stru *)data;

    len = snprintf_s(page, PAGE_SIZE, PAGE_SIZE - 1, "rf info:\n  channel_num: %d\n",
                     pst_mac_vap->st_channel.uc_chan_number);
    if (len >= PAGE_SIZE)
    {
        len = PAGE_SIZE - 1;
    }
    return len;
}


OAL_STATIC oal_void wal_add_vap_proc_file(mac_vap_stru *pst_mac_vap, oal_int8 *pc_name)
{
    hmac_vap_stru                       *pst_hmac_vap;
    oal_proc_dir_entry_stru             *pst_proc_dir;
    oal_proc_dir_entry_stru             *pst_proc_vapinfo;
    oal_proc_dir_entry_stru             *pst_proc_stainfo;
    oal_proc_dir_entry_stru             *pst_proc_mibrf;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_add_vap_proc_file: pst_hmac_vap is null ptr!");
        return;
    }

    pst_proc_dir = proc_mkdir(pc_name, NULL);
    if (OAL_PTR_NULL == pst_proc_dir)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_add_vap_proc_file: proc_mkdir return null");
        return;
    }

    pst_proc_vapinfo = oal_create_proc_entry("ap_info", 420, pst_proc_dir);
    if (OAL_PTR_NULL == pst_proc_vapinfo)
    {
        oal_remove_proc_entry(pc_name, NULL);
        return;
    }

    pst_proc_stainfo = oal_create_proc_entry("sta_info", 420, pst_proc_dir);
    if (OAL_PTR_NULL == pst_proc_stainfo)
    {
        oal_remove_proc_entry("ap_info", pst_proc_dir);
        oal_remove_proc_entry(pc_name, NULL);
        return;
    }

    pst_proc_mibrf = oal_create_proc_entry("mib_rf", 420, pst_proc_dir);
    if (OAL_PTR_NULL == pst_proc_mibrf)
    {
        oal_remove_proc_entry("ap_info", pst_proc_dir);
        oal_remove_proc_entry("sta_info", pst_proc_dir);
        oal_remove_proc_entry(pc_name, NULL);
        return;
    }

    /* vap info */
    pst_proc_vapinfo->read_proc  = wal_read_vap_info_proc;
    pst_proc_vapinfo->data       = pst_mac_vap;

    /* sta info??????????????????proc file??????proc_fops?????????? */
    pst_proc_stainfo->data       = pst_mac_vap;
    pst_proc_stainfo->proc_fops  = &gst_sta_info_proc_fops;

    /* rf info */
    pst_proc_mibrf->read_proc  = wal_read_rf_info_proc;
    pst_proc_mibrf->data       = pst_mac_vap;

    pst_hmac_vap->pst_proc_dir = pst_proc_dir;
}


OAL_STATIC oal_void wal_del_vap_proc_file(oal_net_device_stru *pst_net_dev)
{
    mac_vap_stru   *pst_mac_vap;
    hmac_vap_stru  *pst_hmac_vap;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_del_vap_proc_file: pst_mac_vap is null ptr! pst_net_dev:%x", (uintptr_t)pst_net_dev);
        return;
    }

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "wal_del_vap_proc_file: pst_hmac_vap is null ptr. mac vap id:%d", pst_mac_vap->uc_vap_id);
        return;
    }

    if (pst_hmac_vap->pst_proc_dir)
    {
        oal_remove_proc_entry("mib_rf", pst_hmac_vap->pst_proc_dir);
        oal_remove_proc_entry("sta_info", pst_hmac_vap->pst_proc_dir);
        oal_remove_proc_entry("ap_info", pst_hmac_vap->pst_proc_dir);
        oal_remove_proc_entry(pst_hmac_vap->auc_name, NULL);
        pst_hmac_vap->pst_proc_dir = OAL_PTR_NULL;
    }
}
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,59))

OAL_STATIC oal_int32 wal_sta_info_seq_show_sec(struct seq_file *f, void *v)
{
#define TID_STAT_TO_USER(_stat) ((_stat[0])+(_stat[1])+(_stat[2])+(_stat[3])+(_stat[4])+(_stat[5])+(_stat[6])+(_stat[7]))
#define BW_ENUM_TO_NUMBER(_bw)  ((_bw) == 0 ? 20 : (_bw) == 1 ? 40 : 80)

    mac_vap_stru                    *pst_mac_vap  = (mac_vap_stru *)(f->private);
    hmac_vap_stru                   *pst_hmac_vap;
    oal_dlist_head_stru             *pst_entry;
    oal_dlist_head_stru             *pst_dlist_tmp;
    mac_user_stru                   *pst_user_tmp;
    hmac_user_stru                  *pst_hmac_user_tmp;
    oal_uint8                       *puc_addr;
    oal_uint16                       us_idx = 1;
    oam_stat_info_stru              *pst_oam_stat;
    oam_user_stat_info_stru         *pst_oam_user_stat;
    oal_uint32                       ul_curr_time;
    oal_int8                        *pac_protocol2string[] = {"11a", "11b", "11g", "11g", "11g", "11n", "11ac", "11n", "11ac", "11n","error"};
    wal_msg_write_stru               st_write_msg;
    oal_int32                        l_ret;
    mac_cfg_query_rssi_stru         *pst_query_rssi_param;
    mac_cfg_query_rate_stru         *pst_query_rate_param;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_sta_info_seq_show: hmac vap is null. vap id:%d", pst_mac_vap->uc_vap_id);
        return 0;
    }
    /* step1. ????????????dmac???? */
    OAL_DLIST_SEARCH_FOR_EACH_SAFE(pst_entry, pst_dlist_tmp, &(pst_mac_vap->st_mac_user_list_head))
    {
        pst_user_tmp = OAL_DLIST_GET_ENTRY(pst_entry, mac_user_stru, st_user_dlist);

        pst_hmac_user_tmp = mac_res_get_hmac_user_etc(pst_user_tmp->us_assoc_id);
        if (OAL_PTR_NULL == pst_hmac_user_tmp)
        {
            continue;
        }

        if (OAL_PTR_NULL == pst_dlist_tmp)
        {
            /* ??for????????????????????????????????????????????pst_dlist_tmp????????????????????????dmac???? */
            break;
        }

        /***********************************************************************/
        /*                  ????dmac user??RSSI????                            */
        /***********************************************************************/
        memset_s(&st_write_msg, OAL_SIZEOF(st_write_msg), 0, OAL_SIZEOF(st_write_msg));
        WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_QUERY_RSSI, OAL_SIZEOF(mac_cfg_query_rssi_stru));
        pst_query_rssi_param = (mac_cfg_query_rssi_stru *)st_write_msg.auc_value;

        pst_query_rssi_param->us_user_id = pst_user_tmp->us_assoc_id; /* ????????id?????? */

        l_ret = wal_send_cfg_event_etc(pst_hmac_vap->pst_net_device,
                                   WAL_MSG_TYPE_WRITE,
                                   WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_query_rssi_stru),
                                   (oal_uint8 *)&st_write_msg,
                                   OAL_FALSE,
                                   OAL_PTR_NULL);
        if (OAL_SUCC != l_ret)
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_sta_info_seq_show: send query rssi cfg event ret:%d", l_ret);
        }

        pst_hmac_vap->station_info_query_completed_flag = OAL_FALSE;
        /*lint -save -e774 */
        /*lint -e730*/
        l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_vap->query_wait_q,(OAL_TRUE == pst_hmac_vap->station_info_query_completed_flag), 5*OAL_TIME_HZ);
        /*lint +e730*/
        /*lint -restore */
        if (l_ret <= 0) /* ?????????????? */
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_sta_info_seq_show: query rssi timeout. ret:%d", l_ret);
        }

        /***********************************************************************/
        /*                  ????dmac user??????????                            */
        /***********************************************************************/
        memset_s(&st_write_msg, OAL_SIZEOF(st_write_msg), 0, OAL_SIZEOF(st_write_msg));
        WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_QUERY_RATE, OAL_SIZEOF(mac_cfg_query_rate_stru));
        pst_query_rate_param = (mac_cfg_query_rate_stru *)st_write_msg.auc_value;

        pst_query_rate_param->us_user_id = pst_user_tmp->us_assoc_id; /* ????????id?????? */

        l_ret = wal_send_cfg_event_etc(pst_hmac_vap->pst_net_device,
                                   WAL_MSG_TYPE_WRITE,
                                   WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_query_rate_stru),
                                   (oal_uint8 *)&st_write_msg,
                                   OAL_FALSE,
                                   OAL_PTR_NULL);
        if (OAL_SUCC != l_ret)
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_sta_info_seq_show: send query rate cfg event ret:%d", l_ret);
        }

        pst_hmac_vap->station_info_query_completed_flag = OAL_FALSE;
        /*lint -save -e774 */
        /*lint -e730*/
        l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_vap->query_wait_q, (OAL_TRUE == pst_hmac_vap->station_info_query_completed_flag), 5*OAL_TIME_HZ);
        /*lint +e730*/
        /*lint -restore */
        if (l_ret <= 0) /* ?????????????? */
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_sta_info_seq_show: query rate timeout. ret:%d", l_ret);
        }

    }

    /* step2. proc???????????????? */
    seq_printf(f, "Total user nums: %d\n", pst_mac_vap->us_user_nums);
    seq_printf(f, "-- STA info table --\n");

    pst_oam_stat = OAM_STAT_GET_STAT_ALL();
    ul_curr_time = (oal_uint32)OAL_TIME_GET_STAMP_MS();

    OAL_DLIST_SEARCH_FOR_EACH_SAFE(pst_entry, pst_dlist_tmp, &(pst_mac_vap->st_mac_user_list_head))
    {
        pst_user_tmp = OAL_DLIST_GET_ENTRY(pst_entry, mac_user_stru, st_user_dlist);

        pst_hmac_user_tmp = mac_res_get_hmac_user_etc(pst_user_tmp->us_assoc_id);
        if (OAL_PTR_NULL == pst_hmac_user_tmp)
        {
            continue;
        }

        pst_oam_user_stat = &(pst_oam_stat->ast_user_stat_info[pst_user_tmp->us_assoc_id]);
        puc_addr = pst_user_tmp->auc_user_mac_addr;

        seq_printf(f, "%2d: aid: %d\n"
                      "    MAC ADDR: %02X:%02X:%02X:%02X:%02X:%02X\n"
                      "    status: %d\n"
                      "    BW: %d\n"
                      "    NSS: %d\n"
                      "    RSSI: %d\n"
                      "    phy type: %s\n"
                      "    TX rate: %dkbps\n"
                      "    RX rate: %dkbps\n"
                      "    RX rate_min: %dkbps\n"
                      "    RX rate_max: %dkbps\n"
                      "    user online time: %us\n"
                      "    TX packets succ: %u\n"
                      "    TX packets fail: %u\n"
                      "    RX packets succ: %u\n"
                      "    RX packets fail: %u\n"
                      "    TX power: %ddBm\n"
                      "    TX bytes: %u\n"
                      "    RX bytes: %u\n"
                      "    TX retries: %u\n"
#ifdef _PRE_WLAN_DFT_STAT
                      "    Curr_rate PER: %u\n"
                      "    Best_rate PER: %u\n"
                      "    Tx Throughput: %u\n" /*??????*/
#endif
                      ,us_idx,
                      pst_user_tmp->us_assoc_id,
                      puc_addr[0],puc_addr[1],puc_addr[2],puc_addr[3],puc_addr[4],puc_addr[5],
                      pst_user_tmp->en_user_asoc_state,     /* status */
                      BW_ENUM_TO_NUMBER(pst_user_tmp->en_avail_bandwidth),
                      (pst_user_tmp->en_avail_num_spatial_stream+1),   /* NSS */
                      pst_hmac_user_tmp->c_rssi,
                      pac_protocol2string[pst_user_tmp->en_avail_protocol_mode],
                      pst_hmac_user_tmp->ul_tx_rate,
                      pst_hmac_user_tmp->ul_rx_rate,
                      pst_hmac_user_tmp->ul_rx_rate_min,
                      pst_hmac_user_tmp->ul_rx_rate_max,
                      (oal_uint32)OAL_TIME_GET_RUNTIME(pst_hmac_user_tmp->ul_first_add_time, ul_curr_time)/1000,
                      TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_mpdu_succ_num)+TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_mpdu_in_ampdu),
                      TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_mpdu_fail_num)+TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_mpdu_fail_in_ampdu),
                      pst_oam_user_stat->ul_rx_mpdu_num,   /* RX packets succ */
                      0,
                      20,                                  /* TX power, ???????? ????????tpc????tx_power???? */
                      TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_mpdu_bytes)+TID_STAT_TO_USER(pst_oam_user_stat->aul_tx_ampdu_bytes),
                      pst_oam_user_stat->ul_rx_mpdu_bytes, /* RX bytes */
                      pst_oam_user_stat->ul_tx_ppdu_retries/* TX retries */
#ifdef _PRE_WLAN_DFT_STAT
                      ,pst_hmac_user_tmp->uc_cur_per,
                      pst_hmac_user_tmp->uc_bestrate_per,
                      0
#endif
                      );

        us_idx++;

        if (OAL_PTR_NULL == pst_dlist_tmp)
        {
            break;
        }
    }

#undef TID_STAT_TO_USER
#undef BW_ENUM_TO_NUMBER
    return 0;
}


OAL_STATIC oal_int32 wal_vap_info_seq_show_sec(struct seq_file *f, void *v)
{
    mac_vap_stru                    *pst_mac_vap  = (mac_vap_stru *)(f->private);
    oam_stat_info_stru              *pst_oam_stat;
    oam_vap_stat_info_stru          *pst_oam_vap_stat;

#ifdef _PRE_WLAN_DFT_STAT
    mac_cfg_query_ani_stru          *pst_query_ani_param;
    wal_msg_write_stru               st_write_msg;
    oal_int32                        l_ret;
    hmac_vap_stru                   *pst_hmac_vap;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_read_vap_info_proc: hmac vap is null. vap id:%d", pst_mac_vap->uc_vap_id);
        return 0;
    }
    /***********************************************************************/
    /*                  ????dmac vap??ANI????                            */
    /***********************************************************************/
    memset_s(&st_write_msg, OAL_SIZEOF(st_write_msg), 0, OAL_SIZEOF(st_write_msg));
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_QUERY_RSSI, OAL_SIZEOF(mac_cfg_query_ani_stru));
    pst_query_ani_param = (mac_cfg_query_ani_stru *)st_write_msg.auc_value;

    l_ret = wal_send_cfg_event_etc(pst_hmac_vap->pst_net_device,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_query_ani_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_read_vap_info_proc: send query ani cfg event ret:%d", l_ret);
    }

    pst_hmac_vap->station_info_query_completed_flag = OAL_FALSE;
    /*lint -save -e774 */
    /*lint -e730*/
    l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_vap->query_wait_q,(OAL_TRUE == pst_hmac_vap->station_info_query_completed_flag), 5*OAL_TIME_HZ);
    /*lint +e730*/
    /*lint -restore */
    if (l_ret <= 0) /* ?????????????? */
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_read_vap_info_proc: query ani timeout. ret:%d", l_ret);
    }
#endif

    pst_oam_stat = OAM_STAT_GET_STAT_ALL();

    pst_oam_vap_stat = &(pst_oam_stat->ast_vap_stat_info[pst_mac_vap->uc_vap_id]);
    seq_printf(f, "vap stats:\n"
                        "  TX bytes: %u\n"
                        "  TX packets: %u\n"
                        "  TX packets error: %u\n"
                        "  TX packets discard: %u\n"
                        "  TX unicast packets: %u\n"
                        "  TX multicast packets: %u\n"
                        "  TX broadcast packets: %u\n",
                        pst_oam_vap_stat->ul_tx_bytes_from_lan,
                        pst_oam_vap_stat->ul_tx_pkt_num_from_lan,
                        pst_oam_vap_stat->ul_tx_abnormal_msdu_dropped+pst_oam_vap_stat->ul_tx_security_check_faild+pst_oam_vap_stat->ul_tx_abnormal_mpdu_dropped,
                        pst_oam_vap_stat->ul_tx_uapsd_process_dropped+pst_oam_vap_stat->ul_tx_psm_process_dropped+pst_oam_vap_stat->ul_tx_alg_process_dropped,
                        pst_oam_vap_stat->ul_tx_pkt_num_from_lan - pst_oam_vap_stat->ul_tx_m2u_mcast_cnt,
                        0,
                        pst_oam_vap_stat->ul_tx_m2u_mcast_cnt);

    seq_printf(f, "  RX bytes: %u\n"
                             "  RX packets: %u\n"
                             "  RX packets error: %u\n"
                             "  RX packets discard: %u\n"
                             "  RX unicast packets: %u\n"
                             "  RX multicast packets: %u\n"
                             "  RX broadcast packets: %u\n"
                             "  RX unhnown protocol packets: %u\n"
#ifdef _PRE_WLAN_DFT_STAT
                             "  Br_rate_num: %u\n"
                             "  Nbr_rate_num: %u\n"
                             "  Max_rate: %u\n"
                             "  Min_rate: %u\n"
                             "  Channel num: %d\n"
                             "  ANI:\n"
                             "    dmac_device_distance: %d\n"
                             "    cca_intf_state: %d\n"
                             "    co_intf_state: %d\n"
#endif
                            ,pst_oam_vap_stat->ul_rx_bytes_to_lan,
                             pst_oam_vap_stat->ul_rx_pkt_to_lan,
                             pst_oam_vap_stat->ul_rx_defrag_process_dropped+pst_oam_vap_stat->ul_rx_alg_process_dropped+pst_oam_vap_stat->ul_rx_abnormal_dropped,
                             pst_oam_vap_stat->ul_rx_no_buff_dropped+pst_oam_vap_stat->ul_rx_ta_check_dropped+pst_oam_vap_stat->ul_rx_da_check_dropped+pst_oam_vap_stat->ul_rx_replay_fail_dropped+pst_oam_vap_stat->ul_rx_key_search_fail_dropped,
                             pst_oam_vap_stat->ul_rx_pkt_to_lan - pst_oam_vap_stat->ul_rx_mcast_cnt,
                             0,
                             pst_oam_vap_stat->ul_rx_mcast_cnt,
                             0
#ifdef _PRE_WLAN_DFT_STAT
                             ,pst_mac_vap->st_curr_sup_rates.uc_br_rate_num,
                             pst_mac_vap->st_curr_sup_rates.uc_nbr_rate_num,
                             pst_mac_vap->st_curr_sup_rates.uc_max_rate,
                             pst_mac_vap->st_curr_sup_rates.uc_min_rate,
                             pst_mac_vap->st_channel.uc_chan_number,
                             pst_hmac_vap->uc_device_distance,
                             pst_hmac_vap->uc_intf_state_cca,
                             pst_hmac_vap->uc_intf_state_co
#endif
                            );

    return 0;

}

OAL_STATIC oal_int32 wal_sta_info_proc_open(struct inode *inode, struct file *filp)
{
    oal_int32               l_ret;
    struct proc_dir_entry  *pde = PDE(inode);
    l_ret = single_open(filp, wal_sta_info_seq_show_sec, pde->data);

    return l_ret;

}

OAL_STATIC oal_int32 wal_vap_info_proc_open(struct inode *inode, struct file *filp)
{
    oal_int32               l_ret;
    struct proc_dir_entry  *pde = PDE(inode);
    l_ret = single_open(filp, wal_vap_info_seq_show_sec, pde->data);

    return l_ret;

}

static ssize_t wal_sta_info_proc_write(struct file* filp, const char __user* buffer, size_t len, loff_t* off)
{
    char mode;
    if (len < 1)
    {
        return -EINVAL;
    }

    if (copy_from_user(&mode, buffer, sizeof(mode)))
    {
        return -EFAULT;
    }

    return len;
}

static ssize_t wal_vap_info_proc_write(struct file* filp, const char __user* buffer, size_t len, loff_t* off)
{
    char mode;
    if (len < 1)
    {
        return -EINVAL;
    }

    if (copy_from_user(&mode, buffer, sizeof(mode)))
    {
        return -EFAULT;
    }

    return len;
}

/*****************************************************************************
    gst_sta_info_proc_fops_sec: ????sta info proc fops
*****************************************************************************/
OAL_STATIC OAL_CONST struct file_operations gst_sta_info_proc_fops_sec = {
    .owner      = THIS_MODULE,
    .open       = wal_sta_info_proc_open,
    .read       = seq_read,
    .write      = wal_sta_info_proc_write,
    .llseek     = seq_lseek,
    .release    = single_release,
};

/*****************************************************************************
    gst_vap_info_proc_fops_sec: ????vap info proc fops
*****************************************************************************/
OAL_STATIC OAL_CONST struct file_operations gst_vap_info_proc_fops_sec = {
    .owner      = THIS_MODULE,
    .open       = wal_vap_info_proc_open,
    .read       = seq_read,
    .write      = wal_vap_info_proc_write,
    .llseek     = seq_lseek,
    .release    = single_release,
};


OAL_STATIC oal_void wal_add_vap_proc_file_sec(mac_vap_stru *pst_mac_vap, oal_int8 *pc_name)
{
    hmac_vap_stru                       *pst_hmac_vap;
    oal_proc_dir_entry_stru             *pst_proc_dir;
    oal_proc_dir_entry_stru             *pst_proc_vapinfo;
    oal_proc_dir_entry_stru             *pst_proc_stainfo;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_add_vap_proc_file: pst_hmac_vap is null ptr!");
        return;
    }

    pst_proc_dir = proc_mkdir(pc_name, NULL);
    if (OAL_PTR_NULL == pst_proc_dir)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_add_vap_proc_file: proc_mkdir return null");
        return;
    }

    pst_proc_vapinfo = proc_create("ap_info", 420, pst_proc_dir, &gst_vap_info_proc_fops_sec);
    if (OAL_PTR_NULL == pst_proc_vapinfo)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_add_vap_proc_file: proc_create return null");
        oal_remove_proc_entry(pc_name, NULL);
        return;
    }

    pst_proc_stainfo = proc_create("sta_info", 420, pst_proc_dir, &gst_sta_info_proc_fops_sec);
    if (OAL_PTR_NULL == pst_proc_stainfo)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_add_vap_proc_file: proc_create return null");
        oal_remove_proc_entry("ap_info", pst_proc_dir);
        oal_remove_proc_entry(pc_name, NULL);
        return;
    }

    pst_proc_vapinfo->data       = pst_mac_vap;
    pst_proc_stainfo->data       = pst_mac_vap;

    pst_hmac_vap->pst_proc_dir = pst_proc_dir;
}


OAL_STATIC oal_void wal_del_vap_proc_file_sec(oal_net_device_stru *pst_net_dev)
{
    mac_vap_stru   *pst_mac_vap;
    hmac_vap_stru  *pst_hmac_vap;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        return;
    }

    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "wal_del_vap_proc_file: pst_hmac_vap is null ptr. mac vap id:%d", pst_mac_vap->uc_vap_id);
        return;
    }

    if (pst_hmac_vap->pst_proc_dir)
    {
        oal_remove_proc_entry("sta_info", pst_hmac_vap->pst_proc_dir);
        oal_remove_proc_entry("ap_info", pst_hmac_vap->pst_proc_dir);
        oal_remove_proc_entry(pst_hmac_vap->auc_name, NULL);
        pst_hmac_vap->pst_proc_dir = OAL_PTR_NULL;
    }
}
#endif

#if (defined(CONFIG_ATP_FASTIP) && defined(_PRE_WLAN_FASTIP_SUPPORT))
extern int fastip_attach_wifi(struct net_device *dev);
extern void fastip_detach_wifi(unsigned int idx);
#endif  /* (defined(CONFIG_ATP_FASTIP) && defined(_PRE_WLAN_FASTIP_SUPPORT)) */


OAL_STATIC oal_uint32  wal_hipriv_add_vap(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param)
{
    oal_net_device_stru        *pst_net_dev = OAL_PTR_NULL;
    wal_msg_write_stru          st_write_msg;
    wal_msg_stru               *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                  ul_err_code;
    oal_uint32                  ul_ret;
    oal_int32                   l_ret;
    oal_uint32                  ul_off_set;
    oal_int8                    ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int8                    ac_mode[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    wlan_vap_mode_enum_uint8    en_mode;
    mac_vap_stru               *pst_mac_vap = OAL_PTR_NULL;

    oal_wireless_dev_stru      *pst_wdev = OAL_PTR_NULL;

    mac_vap_stru               *pst_cfg_mac_vap = OAL_PTR_NULL;
    mac_device_stru            *pst_mac_device = OAL_PTR_NULL;

#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8    en_p2p_mode = WLAN_LEGACY_VAP_MODE;
#endif
#if (defined(CONFIG_ATP_FASTIP) && defined(_PRE_WLAN_FASTIP_SUPPORT))
    oal_int32                   fastip_idx;
#endif  /* (defined(CONFIG_ATP_FASTIP) && defined(_PRE_WLAN_FASTIP_SUPPORT)) */


    /* pc_param????????????net_device??name, ??????????????ac_name?? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_add_vap::wal_get_cmd_one_arg_etc vap name return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ac_name length????????OAL_IF_NAME_SIZE */
    if (OAL_IF_NAME_SIZE <=  OAL_STRLEN(ac_name))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_add_vap:: vap name overlength is %d!}\r\n", OAL_STRLEN(ac_name));
        /* ??????????vap name???? */
        oal_print_hex_dump((oal_uint8 *)ac_name, OAL_IF_NAME_SIZE, 32, "vap name lengh is overlong:");
        return OAL_FAIL;
    }

    pc_param += ul_off_set;

    /* pc_param ????'ap|sta', ????????????ac_mode?? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_mode, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_add_vap::wal_get_cmd_one_arg_etc vap name return err_code %d!}\r\n", ul_ret);
        return (oal_uint32)ul_ret;
    }

    /* ????ac_mode???????????????? */
    if (0 == (oal_strcmp("ap", ac_mode)))
    {
        en_mode = WLAN_VAP_MODE_BSS_AP;
    }
    else if (0 == (oal_strcmp("sta", ac_mode)))
    {
        en_mode = WLAN_VAP_MODE_BSS_STA;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    /* ????P2P ????VAP */
    else if (0 == (oal_strcmp("p2p_device", ac_mode)))
    {
        en_mode = WLAN_VAP_MODE_BSS_STA;
        en_p2p_mode = WLAN_P2P_DEV_MODE;
    }
    else if (0 == (oal_strcmp("p2p_cl", ac_mode)))
    {
        en_mode = WLAN_VAP_MODE_BSS_STA;
        en_p2p_mode = WLAN_P2P_CL_MODE;
    }
    else if (0 == (oal_strcmp("p2p_go", ac_mode)))
    {
        en_mode = WLAN_VAP_MODE_BSS_AP;
        en_p2p_mode = WLAN_P2P_GO_MODE;
    }
#endif  /* _PRE_WLAN_FEATURE_P2P */
    else
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_add_vap::the mode param is invalid!}\r\n");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /* ??????????net device?????????????????? */
    /* ????dev_name????dev */
    pst_net_dev = oal_dev_get_by_name(ac_name);
    if (OAL_PTR_NULL != pst_net_dev)
    {
        /* ????oal_dev_get_by_name????????????oal_dev_put??net_dev?????????????? */
        oal_dev_put(pst_net_dev);

        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_add_vap::the net_device is already exist!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ????mac device */
    pst_cfg_mac_vap = OAL_NET_DEV_PRIV(pst_cfg_net_dev);
    pst_mac_device  = mac_res_get_dev_etc(pst_cfg_mac_vap->uc_device_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_device))
    {
        OAM_WARNING_LOG0(pst_cfg_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_add_vap::pst_mac_device is null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

#if defined(_PRE_WLAN_FEATURE_FLOWCTL)
    pst_net_dev = oal_net_alloc_netdev_mqs(OAL_SIZEOF(oal_netdev_priv_stru), ac_name, oal_ether_setup, WAL_NETDEV_SUBQUEUE_MAX_NUM, 1);    /* ??????????????????????????????????????????0 */
#elif defined(_PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL)
    pst_net_dev = oal_net_alloc_netdev_mqs(OAL_SIZEOF(oal_netdev_priv_stru), ac_name, oal_ether_setup, WLAN_NET_QUEUE_BUTT, 1);    /* ??????????????????????????????????????????0 */
#else
    pst_net_dev = oal_net_alloc_netdev(OAL_SIZEOF(oal_netdev_priv_stru), ac_name, oal_ether_setup);    /* ??????????????????????????????????????????0 */
#endif

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_net_dev))
    {
        OAM_WARNING_LOG0(pst_cfg_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_add_vap::pst_net_dev null ptr error!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_wdev = (oal_wireless_dev_stru *)OAL_MEM_ALLOC(OAL_MEM_POOL_ID_LOCAL,OAL_SIZEOF(oal_wireless_dev_stru), OAL_FALSE);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_wdev))
    {
        OAM_ERROR_LOG0(pst_cfg_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_add_vap::alloc mem, pst_wdev is null ptr!}\r\n");
        oal_net_free_netdev(pst_net_dev);
        return OAL_ERR_CODE_PTR_NULL;
    }

    memset_s(pst_wdev, OAL_SIZEOF(oal_wireless_dev_stru), 0, OAL_SIZEOF(oal_wireless_dev_stru));

    /* ??netdevice???????? */
#ifdef CONFIG_WIRELESS_EXT
    pst_net_dev->wireless_handlers             = &g_st_iw_handler_def_etc;
#endif /* CONFIG_WIRELESS_EXT */
    /* OAL_NETDEVICE_OPS(pst_net_dev)             = &g_st_wal_net_dev_ops_etc; */
    pst_net_dev->netdev_ops                    = &g_st_wal_net_dev_ops_etc;

    OAL_NETDEVICE_DESTRUCTOR(pst_net_dev)      = oal_net_free_netdev;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,44))
    OAL_NETDEVICE_MASTER(pst_net_dev)          = OAL_PTR_NULL;
#endif

    OAL_NETDEVICE_IFALIAS(pst_net_dev)         = OAL_PTR_NULL;
    OAL_NETDEVICE_WATCHDOG_TIMEO(pst_net_dev)  = 5;
    OAL_NETDEVICE_WDEV(pst_net_dev)            = pst_wdev;
    OAL_NETDEVICE_QDISC(pst_net_dev, OAL_PTR_NULL);
#ifdef _PRE_WLAN_FEATURE_SMP_SUPPORT
//    OAL_NETDEVICE_TX_QUEUE_LEN(pst_net_dev) = 0;
#endif

    pst_wdev->netdev = pst_net_dev;

    if (WLAN_VAP_MODE_BSS_AP == en_mode)
    {
        pst_wdev->iftype = NL80211_IFTYPE_AP;
    }
    else if (WLAN_VAP_MODE_BSS_STA == en_mode)
    {
        pst_wdev->iftype = NL80211_IFTYPE_STATION;
    }
#ifdef _PRE_WLAN_FEATURE_P2P
    if (WLAN_P2P_DEV_MODE == en_p2p_mode)
    {
        pst_wdev->iftype = NL80211_IFTYPE_P2P_DEVICE;
    }
    else if (WLAN_P2P_CL_MODE == en_p2p_mode)
    {
        pst_wdev->iftype = NL80211_IFTYPE_P2P_CLIENT;
    }
    else if (WLAN_P2P_GO_MODE == en_p2p_mode)
    {
        pst_wdev->iftype = NL80211_IFTYPE_P2P_GO;
    }
#endif  /* _PRE_WLAN_FEATURE_P2P */

    pst_wdev->wiphy = pst_mac_device->pst_wiphy;

    OAL_NETDEVICE_FLAGS(pst_net_dev) &= ~OAL_IFF_RUNNING;   /* ??net device??flag????down */

    /* st_write_msg??????????????????????????????????????????????????????????0??????????????????????P2P mode??????P2P?????????????????????????????????????? */
    memset_s(&st_write_msg, OAL_SIZEOF(wal_msg_write_stru), 0, OAL_SIZEOF(wal_msg_write_stru));

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    /* ???????? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_ADD_VAP, OAL_SIZEOF(mac_cfg_add_vap_param_stru));
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->pst_net_dev = pst_net_dev;
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->en_vap_mode  = en_mode;
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->uc_cfg_vap_indx = pst_cfg_mac_vap->uc_vap_id;

#ifdef _PRE_WLAN_FEATURE_P2P
   ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->en_p2p_mode  = en_p2p_mode;
#endif
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->bit_11ac2g_enable = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_11AC2G_ENABLE);
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->bit_disable_capab_2ght40 = wlan_customize_etc.uc_disable_capab_2ght40;
#endif
    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_add_vap_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAL_MEM_FREE(pst_wdev, OAL_FALSE);
        oal_net_free_netdev(pst_net_dev);
        OAM_WARNING_LOG1(pst_cfg_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_add_vap::return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    /* ???????????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(pst_cfg_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_add_vap::hmac add vap fail,err code[%u]!}\r\n", ul_err_code);
        /* ?????????????????? */
        OAL_MEM_FREE(pst_wdev, OAL_FALSE);
        oal_net_free_netdev(pst_net_dev);
        return ul_err_code;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

    if ((WLAN_LEGACY_VAP_MODE == en_p2p_mode) && (OAL_PTR_NULL == pst_mac_device->st_p2p_info.pst_primary_net_device))
    {
        /* ????????wlan0?? ??????wlan0 ????net_device,p2p0 ??p2p-p2p0 MAC ????????netdevice ???? */
        pst_mac_device->st_p2p_info.pst_primary_net_device = pst_net_dev;
    }

    if(OAL_SUCC != wal_set_random_mac_to_mib_etc(pst_net_dev))
    {
        /* ?????????????????? */
        /* ?????????????????? */
        OAL_MEM_FREE(pst_wdev, OAL_FALSE);
        oal_net_free_netdev(pst_net_dev);
        return OAL_ERR_CODE_PTR_NULL;
    } /* set random mac to mib ; for hi1102-cb */
#endif

    /* ????netdevice??MAC??????MAC??????HMAC????????????MIB?? */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
#ifdef _PRE_WLAN_FEATURE_P2P
    if (WLAN_P2P_CL_MODE != en_p2p_mode)
    {
        pst_mac_vap->en_vap_state = MAC_VAP_STATE_INIT;
    }
#else
    pst_mac_vap->en_vap_state = MAC_VAP_STATE_INIT;
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
    if (en_p2p_mode == WLAN_P2P_DEV_MODE)
    {
        oal_set_mac_addr((oal_uint8 *)OAL_NETDEVICE_MAC_ADDR(pst_net_dev), mac_mib_get_p2p0_dot11StationID(pst_mac_vap));

        pst_mac_device->st_p2p_info.uc_p2p0_vap_idx = pst_mac_vap->uc_vap_id;
    }
    else
#endif
    {
        oal_set_mac_addr((oal_uint8 *)OAL_NETDEVICE_MAC_ADDR(pst_net_dev), mac_mib_get_StationID(pst_mac_vap));
    }


    /* ????net_device */
    ul_ret = (oal_uint32)oal_net_register_netdev(pst_net_dev);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_add_vap::oal_net_register_netdev return error code %d!}\r\n", ul_ret);

        /* ?????????????????? */
        /* ??????vap????????????????vap  */
        WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DEL_VAP, OAL_SIZEOF(mac_cfg_del_vap_param_stru));

        l_ret = wal_send_cfg_event_etc(pst_net_dev,
                                   WAL_MSG_TYPE_WRITE,
                                   WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_del_vap_param_stru),
                                   (oal_uint8 *)&st_write_msg,
                                   OAL_TRUE,
                                   &pst_rsp_msg);

        if (OAL_SUCC != wal_check_and_release_msg_resp_etc(pst_rsp_msg))
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_add_vap::wal_check_and_release_msg_resp_etc fail.}");
        }
        if (OAL_SUCC != l_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_add_vap::wal_send_cfg_event_etc fail,err code %d!}\r\n",l_ret);
        }


        OAL_MEM_FREE(pst_wdev, OAL_FALSE);
        oal_net_free_netdev(pst_net_dev);
        return ul_ret;
    }

    /* E5 SPE module init */
#if (defined(CONFIG_BALONG_SPE) && defined(_PRE_WLAN_SPE_SUPPORT))
    if(spe_hook.is_enable && spe_hook.is_enable())
    {
        if(wal_netdev_spe_init(pst_net_dev))
        {
            OAL_IO_PRINT("wal_netdev_open_etc::spe init failed!!\n");
        }
    }
#endif

#if (defined(CONFIG_ATP_FASTIP) && defined(_PRE_WLAN_FASTIP_SUPPORT))
        fastip_idx = fastip_attach_wifi(pst_net_dev);
        if (-1 != fastip_idx)
        {
            pst_mac_vap->ul_fastip_idx = (oal_uint32)fastip_idx;
        }
#endif  /* (defined(CONFIG_ATP_FASTIP) && defined(_PRE_WLAN_FASTIP_SUPPORT)) */


    /* ????VAP??????proc???? */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,34))
    wal_add_vap_proc_file(pst_mac_vap, ac_name);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,59))
    wal_add_vap_proc_file_sec(pst_mac_vap, ac_name);
#endif

    return OAL_SUCC;
}



oal_uint32  wal_hipriv_del_vap_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru           st_write_msg;
    wal_msg_stru               *pst_rsp_msg = OAL_PTR_NULL;
    oal_int32                    l_ret;
    oal_wireless_dev_stru       *pst_wdev = OAL_PTR_NULL;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8     en_p2p_mode = WLAN_LEGACY_VAP_MODE;
#endif
    mac_vap_stru                *pst_mac_vap = OAL_PTR_NULL;
    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR2(pst_net_dev,pc_param)))
    {
        // ??????????????????????????????,????????????????????
        OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_hipriv_del_vap_etc::pst_net_dev or pc_param null ptr error %x, %x!}\r\n",
                       (uintptr_t)pst_net_dev, (uintptr_t)pc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ??????up??????????????????????down */
    if (OAL_UNLIKELY(0 != (OAL_IFF_RUNNING & OAL_NETDEVICE_FLAGS(pst_net_dev))))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_del_vap_etc::device is busy, please down it first %d!}\r\n", OAL_NETDEVICE_FLAGS(pst_net_dev));
        return OAL_ERR_CODE_CONFIG_BUSY;
    }

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(OAL_UNLIKELY(NULL == pst_mac_vap))
    {
        OAM_ERROR_LOG0(0, OAM_SF_CFG, "{wal_hipriv_del_vap_etc::can't get mac vap from netdevice priv data!}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    if(WLAN_VAP_MODE_CONFIG == pst_mac_vap->en_vap_mode)
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_del_vap_etc::invalid parameters, mac vap mode: %d}", pst_mac_vap->en_vap_mode);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /* ??????up??????????????????????down */
#ifdef _PRE_WLAN_FEATURE_P2P
    if ((pst_mac_vap->en_vap_state != MAC_VAP_STATE_INIT)
         &&(pst_mac_vap->en_p2p_mode != WLAN_P2P_CL_MODE))
#else
    if (pst_mac_vap->en_vap_state != MAC_VAP_STATE_INIT)
#endif
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_del_vap_etc::device is busy, please down it first %d!}\r\n", pst_mac_vap->en_vap_state);
        return OAL_ERR_CODE_CONFIG_BUSY;
    }

    /* E5 SPE module relation */
#if (defined(CONFIG_BALONG_SPE) && defined(_PRE_WLAN_SPE_SUPPORT))
    if(spe_hook.is_enable && spe_hook.is_enable())
    {
         wal_netdev_spe_exit(pst_net_dev);
    }
#endif
#if (defined(CONFIG_ATP_FASTIP) && defined(_PRE_WLAN_FASTIP_SUPPORT))
    fastip_detach_wifi(pst_mac_vap->ul_fastip_idx);
#endif  /* (defined(CONFIG_ATP_FASTIP) && defined(_PRE_WLAN_FASTIP_SUPPORT)) */


    /* ????vap??????proc???? */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,34))
    wal_del_vap_proc_file(pst_net_dev);
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,59))
    wal_del_vap_proc_file_sec(pst_net_dev);
#endif

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    //????vap ??????????????????
    /* st_write_msg??????????????????????????????????????????????????????????0??????????????????????P2P mode??????P2P?????????????????????????????????????? */
    memset_s(&st_write_msg, OAL_SIZEOF(wal_msg_write_stru), 0, OAL_SIZEOF(wal_msg_write_stru));

    ((mac_cfg_del_vap_param_stru *)st_write_msg.auc_value)->pst_net_dev = pst_net_dev;
    pst_wdev = pst_net_dev->ieee80211_ptr;
#ifdef _PRE_WLAN_FEATURE_P2P
    en_p2p_mode = wal_wireless_iftype_to_mac_p2p_mode_etc(pst_wdev->iftype);
    if (WLAN_P2P_BUTT == en_p2p_mode)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_hipriv_del_vap_etc::wal_wireless_iftype_to_mac_p2p_mode_etc return BUFF}\r\n");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }
    ((mac_cfg_del_vap_param_stru *)st_write_msg.auc_value)->en_p2p_mode = en_p2p_mode;

#endif


    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DEL_VAP, OAL_SIZEOF(mac_cfg_del_vap_param_stru));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_del_vap_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_SUCC != wal_check_and_release_msg_resp_etc(pst_rsp_msg))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_del_vap_etc::wal_check_and_release_msg_resp_etc fail}");
    }

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_del_vap_etc::return err code %d}\r\n", l_ret);
        /* ?????? */
        oal_net_unregister_netdev(pst_net_dev);
        OAL_MEM_FREE(pst_wdev, OAL_TRUE);
        return (oal_uint32)l_ret;
    }

    /* ?????? */
    oal_net_unregister_netdev(pst_net_dev);
    OAL_MEM_FREE(pst_wdev, OAL_TRUE);

    return OAL_SUCC;
}


oal_uint32  wal_hipriv_vap_info_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru           st_write_msg;
    oal_int32                    l_ret;

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_VAP_INFO, OAL_SIZEOF(oal_int32));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_vap_info_etc::return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;

}
#ifdef _PRE_WLAN_FEATURE_NEGTIVE_DET

oal_uint32  wal_hipriv_pk_mode_debug(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    /* pkmode?????????????????? hipriv "wlan0 pk_mode_debug 0/1(high/low) 0/1/2/3/4(BW) 0/1/2/3(protocol) ??????????"  */
    /*
        BW:20M     40M    80M   160M   80+80M

        protocol:lagency: HT: VHT: HE:
    */
    /*
    PK????????????:
    {(????Mbps)  20M     40M    80M   160M   80+80M
    lagency:    {valid, valid, valid, valid, valid},   (????????????????pk mode )
    HT:         {72, 150, valid, valid, valid},
    VHT:        {86, 200, 433, 866, 866},
    HE:         {valid, valid, valid, valid, valid},   (????????11ax??pk mode)
    };

    PK????????????:
    ??????????: g_st_pk_mode_high_th_table = PK???????????? * 70% *1024 *1024 /8  (????????)
    ??????????: g_st_pk_mode_low_th_table  = PK???????????? * 20% *1024 *1024 /8  (????????)

    */
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;
    oal_uint32                      ul_ret;
    oal_uint32                      ul_off_set = 0;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {0};
    oal_uint32                      auc_pk_mode_param[4] = {0};

    /* st_write_msg?????????? */
    memset_s(&st_write_msg, OAL_SIZEOF(wal_msg_write_stru), 0, OAL_SIZEOF(wal_msg_write_stru));

   /* 0.??????0??????: ??????/?????? */
    ul_ret = wal_get_cmd_one_arg_etc((oal_int8*)pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_hipriv_pk_mode_debug::wal_get_cmd_one_arg1 return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    auc_pk_mode_param[0] = (oal_uint32)oal_atoi(ac_name);
    if (auc_pk_mode_param[0] > 3)
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_hipriv_pk_mode_debug::high/low param ERROR [%d]!}", auc_pk_mode_param[0]);
        return OAL_FAIL;
    }


    pc_param = pc_param + ul_off_set;

    /* 1.??????????????: BW */
    ul_ret = wal_get_cmd_one_arg_etc((oal_int8*)pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_hipriv_pk_mode_debug::wal_get_cmd_one_arg1 return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    auc_pk_mode_param[1] = (oal_uint32)oal_atoi(ac_name);
    if (auc_pk_mode_param[1] >= WLAN_BW_CAP_BUTT)
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_hipriv_pk_mode_debug::BW param ERROR [%d]!}", auc_pk_mode_param[1]);
        return OAL_FAIL;
    }


    pc_param = pc_param + ul_off_set;

    /* 2.??????????????: protocol */
    ul_ret = wal_get_cmd_one_arg_etc((oal_int8*)pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_hipriv_pk_mode_debug::wal_get_cmd_one_arg1 return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    auc_pk_mode_param[2] = (oal_uint32)oal_atoi(ac_name);
    if (auc_pk_mode_param[2] >= WLAN_PROTOCOL_CAP_BUTT)
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_hipriv_pk_mode_debug::protocol param ERROR [%d]!}", auc_pk_mode_param[2]);
        return OAL_FAIL;
    }

    pc_param = pc_param + ul_off_set;


    /* 3.??????????????: ?????? */
    ul_ret = wal_get_cmd_one_arg_etc((oal_int8*)pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_hipriv_pk_mode_debug::wal_get_cmd_one_arg1 return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    auc_pk_mode_param[3] = (oal_uint32)oal_atoi(ac_name);

    pc_param = pc_param + ul_off_set;

    /*???????????????? */
    st_write_msg.auc_value[0] = auc_pk_mode_param[0];
    st_write_msg.auc_value[1] = auc_pk_mode_param[1];
    st_write_msg.auc_value[2] = auc_pk_mode_param[2];
    st_write_msg.auc_value[3] = auc_pk_mode_param[3];

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SYNC_PK_MODE, OAL_SIZEOF(auc_pk_mode_param));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(auc_pk_mode_param),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_pk_mode_debug::return err code %d!}", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;

}
#endif
#ifdef _PRE_WLAN_FEATURE_IP_FILTER
#define MAX_HIPRIV_IP_FILTER_BTABLE_SIZE 129

OAL_STATIC oal_uint32 wal_ipriv_ip_filter_items(wal_hw_wifi_filter_item *pst_items, oal_uint32 ul_item_size, oal_int32 l_count)
{
    if (ul_item_size > sizeof(wal_hw_wifi_filter_item) * MAX_HIPRIV_IP_FILTER_BTABLE_SIZE) {
        return OAL_FAIL;
    }

    return (oal_uint32)wal_add_ip_filter_items_etc(pst_items, l_count);
}



oal_uint32  wal_hipriv_set_ip_filter_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    oal_int32                l_items_cnt;
    oal_int32                l_items_idx;
    oal_int32                l_enable;
    oal_uint32               ul_ret;
    oal_uint32               ul_off_set;

    oal_int8                 ac_cmd[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {0};
    oal_int8                 ac_cmd_param[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {0};
    wal_hw_wifi_filter_item  ast_items[MAX_HIPRIV_IP_FILTER_BTABLE_SIZE];

    l_enable     = 0;
    l_items_cnt = 0;
    memset_s((oal_uint8 *)ast_items, OAL_SIZEOF(wal_hw_wifi_filter_item) * MAX_HIPRIV_IP_FILTER_BTABLE_SIZE,
             0, OAL_SIZEOF(wal_hw_wifi_filter_item) * MAX_HIPRIV_IP_FILTER_BTABLE_SIZE);


    /* ???????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_cmd, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_ip_filter_etc::wal_get_cmd_one_arg_etc vap name return err_code %d!}", ul_ret);
        return ul_ret;
    }

    if(0 == oal_strncmp(ac_cmd, CMD_CLEAR_RX_FILTERS, OAL_STRLEN(CMD_CLEAR_RX_FILTERS)))
    {
        /* ???????? */
        ul_ret = (oal_uint32)wal_clear_ip_filter_etc();
        return ul_ret;
    }

    pc_param += ul_off_set;
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_cmd_param, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_ip_filter_etc::get cmd_param return err_code %d!}", ul_ret);
        return ul_ret;
    }

    if(0 == oal_strncmp(ac_cmd, CMD_SET_RX_FILTER_ENABLE, OAL_STRLEN(CMD_SET_RX_FILTER_ENABLE)))
    {
        /* ????/???????? */
        l_enable = oal_atoi(ac_cmd_param);
        ul_ret = (oal_uint32)wal_set_ip_filter_enable_etc(l_enable);
        return ul_ret;

    }
    else if(0 == oal_strncmp(ac_cmd, CMD_ADD_RX_FILTER_ITEMS, OAL_STRLEN(CMD_ADD_RX_FILTER_ITEMS)))
    {
        /* ?????????? */
        /* ?????????????? */
        l_items_cnt = oal_atoi(ac_cmd_param);
        l_items_cnt = OAL_MIN(MAX_HIPRIV_IP_FILTER_BTABLE_SIZE, l_items_cnt);

        /* ???????????? */
        for (l_items_idx = 0; l_items_idx < l_items_cnt; l_items_idx++)
        {

            /* ????protocol X*/
            pc_param += ul_off_set;
            ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_cmd_param, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
            if (OAL_SUCC != ul_ret)
            {
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_ip_filter_etc::get item_params return err_code %d!}", ul_ret);
                return ul_ret;
            }
            ast_items[l_items_idx].protocol = (oal_uint8)oal_atoi(ac_cmd_param);


            /* ????portX*/
            pc_param += ul_off_set;
            ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_cmd_param, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
            if (OAL_SUCC != ul_ret)
            {
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_ip_filter_etc::get item_params return err_code %d!}", ul_ret);
                return ul_ret;
            }
            ast_items[l_items_idx].port = (oal_uint16)oal_atoi(ac_cmd_param);
        }

        ul_ret = wal_ipriv_ip_filter_items(ast_items, sizeof(ast_items), l_items_cnt);
        return ul_ret;
    }
    else
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_ip_filter_etc::cmd_one_arg no support!}");
        return OAL_FAIL;
    }

}

#endif //_PRE_WLAN_FEATURE_IP_FILTER


OAL_STATIC oal_uint32  wal_hipriv_set_2040_coext_support(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret;
    oal_int32                       l_ret;
    oal_uint8                       uc_csp;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_2040_coext_support::wal_get_cmd_one_arg_etc return err_code %d!}\r\n", ul_ret);
         return ul_ret;
    }

    if (0 == (oal_strcmp("0", ac_name)))
    {
        uc_csp = 0;
    }
    else if (0 == (oal_strcmp("1", ac_name)))
    {
        uc_csp = 1;
    }
    else
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_2040_coext_support::the 2040_coexistence command is erro %x!}\r\n", (uintptr_t)ac_name);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_2040_COEXISTENCE, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = uc_csp;  /* ???????????????? */

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_2040_coext_support::return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}



OAL_STATIC  oal_uint32  wal_hipriv_rx_fcs_info(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru          st_write_msg;
    oal_uint32                  ul_off_set;
    oal_int8                    ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int32                   l_ret;
    oal_uint32                  ul_ret;
    mac_cfg_rx_fcs_info_stru   *pst_rx_fcs_info = OAL_PTR_NULL;
    mac_cfg_rx_fcs_info_stru    st_rx_fcs_info;  /* ??????????????use?????? */

    /* ????????????FCS??????????????:sh hipriv.sh "vap0 rx_fcs_info 0/1 1-4" 0/1  0????????????1???????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_rx_fcs_info::wal_get_cmd_one_arg_etc return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }

    st_rx_fcs_info.ul_data_op = (oal_uint32)oal_atoi(ac_name);

    if (st_rx_fcs_info.ul_data_op > 1)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_rx_fcs_info::the ul_data_op command is error %x!}\r\n", (uintptr_t)ac_name);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /* ?????????????????? */
    pc_param = pc_param + ul_off_set;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_rx_fcs_info::wal_get_cmd_one_arg_etc return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }

    st_rx_fcs_info.ul_print_info = (oal_uint32)oal_atoi(ac_name);

    if (st_rx_fcs_info.ul_print_info > 4)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_rx_fcs_info::the ul_print_info command is error %x!}\r\n", (uintptr_t)ac_name);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_RX_FCS_INFO, OAL_SIZEOF(mac_cfg_rx_fcs_info_stru));

    /* ???????????????? */
    pst_rx_fcs_info = (mac_cfg_rx_fcs_info_stru *)(st_write_msg.auc_value);
    pst_rx_fcs_info->ul_data_op    = st_rx_fcs_info.ul_data_op;
    pst_rx_fcs_info->ul_print_info = st_rx_fcs_info.ul_print_info;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_rx_fcs_info_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_rx_fcs_info::return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_vap_log_level(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    mac_vap_stru               *pst_mac_vap = OAL_PTR_NULL;
    oam_log_level_enum_uint8    en_level_val;
    oal_uint32                  ul_off_set;
    oal_int8                    ac_param[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                  ul_ret;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    wal_msg_write_stru          st_write_msg;
#endif

    /* OAM log????????????????: hipriv "Hisilicon0[vapx] log_level {1/2}"
       1-2(error??warning)??????????vap????????????
    */

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_hipriv_vap_log_level::null pointer.}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ???????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_param, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        return ul_ret;
    }

    en_level_val = (oam_log_level_enum_uint8)oal_atoi(ac_param);
    if ((en_level_val<OAM_LOG_LEVEL_ERROR) || (en_level_val>OAM_LOG_LEVEL_INFO))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_feature_log_level::invalid switch value[%d].}", en_level_val);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /*hipriv "Hisilicon0 log_level 1|2|3" ????????vip id??log*/
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    if(0 == oal_strcmp("Hisilicon0", pst_net_dev->name))
    {
        hmac_config_set_all_log_level_etc(pst_mac_vap, OAL_SIZEOF(oam_log_level_enum_uint8), (oal_uint8*)&en_level_val);
        return OAL_SUCC;
    }
#endif

    ul_ret = oam_log_set_vap_level_etc(pst_mac_vap->uc_vap_id, en_level_val);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
// ????????02 device ????log ?????? ??????????????????
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_LOG_LEVEL, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = en_level_val;
    ul_ret |= (oal_uint32)wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_vap_log_level::return err code[%d]!}\r\n", ul_ret);
        return ul_ret;
    }

#endif
    return ul_ret;
}

#ifdef _PRE_WLAN_FEATURE_GREEN_AP

OAL_STATIC oal_uint32  wal_hipriv_green_ap_en(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru          st_write_msg;
    oal_int32                   l_tmp;
    oal_uint32                  ul_off_set;
    oal_int8                    ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int32                   l_ret;
    oal_uint32                  ul_ret;

    /* OAM event????????????????: hipriv "wlan0 green_ap_en 0 | 1"
        ????????????"1"??"0"????ac_name
    */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_green_ap_en::wal_get_cmd_one_arg_etc return err_code[%d]}\r\n", ul_ret);
        return ul_ret;
    }

    /* ????????????????????????event?????????????????? */

    l_tmp = (oal_uint8)oal_atoi(ac_name);

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_GREEN_AP_EN, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = l_tmp;  /* ???????????????? */

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_green_ap_en::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_LTECOEX

OAL_STATIC oal_uint32 wal_ioctl_ltecoex_mode_set(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_LTECOEX_MODE_SET, OAL_SIZEOF(oal_uint32));

    /* ???????????????? */
    *((oal_uint8 *)(st_write_msg.auc_value)) = *pc_param;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_ltecoex_mode_set::return err code[%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_NRCOEX
/*
 * ?? ?? ??  : wal_ioctl_nrcoex_priority_set
 * ????????  : ????NR????WiFi??????
 */
OAL_STATIC oal_uint32 wal_ioctl_nrcoex_priority_set(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;

    /* ????????wal?????? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_NRCOEX_PRIORITY_SET, OAL_SIZEOF(oal_uint32));

    /* ???????????????? */
    *((oal_uint8 *)(st_write_msg.auc_value)) = *pc_param;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_nrcoex_priority_set::return err code[%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif


#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

OAL_STATIC oal_uint32  wal_hipriv_aifsn_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    mac_edca_cfg_stru               st_edca_cfg;
    oal_int32                       l_ret;
    oal_uint32                      ul_ret;

    memset_s(&st_edca_cfg, OAL_SIZEOF(st_edca_cfg), 0, OAL_SIZEOF(st_edca_cfg));

    /* ???????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_aifsn_cfg::get wfa switch fail, return err_code[%d]!}", ul_ret);
        return ul_ret;
    }
    pc_param += ul_off_set;
    st_edca_cfg.en_switch = (oal_bool_enum_uint8)oal_atoi(ac_name);

    /* ????ac */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_aifsn_cfg::get wfa ac fail, return err_code[%d]!}", ul_ret);
        return ul_ret;
    }
    pc_param += ul_off_set;
    st_edca_cfg.en_ac = (wlan_wme_ac_type_enum_uint8)oal_atoi(ac_name);

    if (OAL_TRUE == st_edca_cfg.en_switch)
    {
        /* ?????????? */
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_aifsn_cfg::get wfa val fail, return err_code[%d]!}", ul_ret);
            return ul_ret;
        }
        pc_param += ul_off_set;
        st_edca_cfg.us_val = (oal_uint16)oal_atoi(ac_name);
    }
    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_WFA_CFG_AIFSN, OAL_SIZEOF(st_edca_cfg));

    /* ???????????????? */
    if (EOK != memcpy_s(st_write_msg.auc_value,
                        OAL_SIZEOF(st_edca_cfg),
                        (const oal_void *)&st_edca_cfg,
                        OAL_SIZEOF(st_edca_cfg))) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_aifsn_cfg::memcpy fail!");
        return OAL_FAIL;
    }

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(st_edca_cfg),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_aifsn_cfg::return err code[%d]!}", ul_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_cw_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    mac_edca_cfg_stru               st_edca_cfg;
    oal_int32                       l_ret;
    oal_uint32                      ul_ret;

    memset_s(&st_edca_cfg, OAL_SIZEOF(st_edca_cfg), 0, OAL_SIZEOF(st_edca_cfg));

    /* ???????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_cw_cfg::get wfa switch fail, return err_code[%d]!}", ul_ret);
        return ul_ret;
    }
    pc_param += ul_off_set;
    st_edca_cfg.en_switch = (oal_bool_enum_uint8)oal_atoi(ac_name);

    /* ????ac */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_cw_cfg::get wfa ac fail, return err_code[%d]!}", ul_ret);
        return ul_ret;
    }
    pc_param += ul_off_set;
    st_edca_cfg.en_ac = (wlan_wme_ac_type_enum_uint8)oal_atoi(ac_name);

    if (OAL_TRUE == st_edca_cfg.en_switch)
    {
        /* ?????????? */
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_cw_cfg::get wfa val fail, return err_code[%d]!}", ul_ret);
            return ul_ret;
        }
        pc_param += ul_off_set;
        st_edca_cfg.us_val = (oal_uint16)oal_strtol(ac_name, OAL_PTR_NULL, 0);
    }
    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_WFA_CFG_CW, OAL_SIZEOF(st_edca_cfg));

    /* ???????????????? */
    if (EOK != memcpy_s(st_write_msg.auc_value,
                        OAL_SIZEOF(st_edca_cfg),
                        (const oal_void *)&st_edca_cfg,
                        OAL_SIZEOF(st_edca_cfg))) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_aifsn_cfg::memcpy fail!");
        return OAL_FAIL;
    }

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(st_edca_cfg),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_aifsn_cfg::return err code[%d]!}", ul_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif

OAL_STATIC oal_uint32  wal_hipriv_set_random_mac_addr_scan(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret;
    oal_int32                       l_ret;
    oal_uint8                       uc_rand_mac_addr_scan_switch;

    /* sh hipriv.sh "Hisilicon0 random_mac_addr_scan 0|1(????)" */

    /* ?????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_random_mac_addr_scan::get switch return err_code[%d]!}", ul_ret);
        return ul_ret;
    }
    pc_param += ul_off_set;
    uc_rand_mac_addr_scan_switch = (oal_uint8)oal_atoi(ac_name);

    /* ????????????????0|1,???????????????? */
    if (uc_rand_mac_addr_scan_switch > 1)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_random_mac_addr_scan::param is error, switch_value[%d]!}",
                         uc_rand_mac_addr_scan_switch);
        return OAL_FAIL;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_RANDOM_MAC_ADDR_SCAN, OAL_SIZEOF(oal_uint32));
    *((oal_int32 *)(st_write_msg.auc_value)) = (oal_uint32)uc_rand_mac_addr_scan_switch;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_random_mac_addr_scan::return err code[%d]!}", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_SMPS
OAL_STATIC oal_uint32  wal_hipriv_set_smps_vap_mode(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru          st_write_msg;
    oal_int32                    l_tmp;
    oal_uint32                  ul_off_set;
    oal_int8                    ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int32                   l_ret;
    oal_uint32                  ul_ret;

    /* ????????????"1"??"2"??"3"????ac_name */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_SMPS, "{wal_hipriv_set_smps_vap_mode::wal_get_cmd_one_arg_etc return err_code[%d]}\r\n", ul_ret);
        return ul_ret;
    }

    /* ??????????????????????????????SMPS???? */
    if (0 == (oal_strcmp("1", ac_name)))
    {
        l_tmp = 1;
    }
    else if (0 == (oal_strcmp("2", ac_name)))
    {
        l_tmp = 2;
    }
    else if (0 == (oal_strcmp("3", ac_name)))
    {
        l_tmp = 3;
    }
    else
    {
        OAM_ERROR_LOG1(0, OAM_SF_SMPS, "{wal_hipriv_set_smps_vap_mode::error cmd[%x],input 1/2/3!}", (uintptr_t)ac_name);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SMPS_VAP_MODE, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = l_tmp;  /* ???????????????? */

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_SMPS, "{wal_hipriv_set_smps_vap_mode::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;


}



OAL_STATIC oal_uint32  wal_hipriv_set_smps_mode(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru          st_write_msg;
    oal_int32                    l_tmp;
    oal_uint32                  ul_off_set;
    oal_int8                    ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int32                   l_ret;
    oal_uint32                  ul_ret;

    /* ????????????"1"??"2"??"3"????ac_name */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_SMPS, "{wal_hipriv_set_smps_mode::wal_get_cmd_one_arg_etc return err_code[%d]}\r\n", ul_ret);
        return ul_ret;
    }

    /* ??????????????????????????????SMPS???? */
    if (0 == (oal_strcmp("1", ac_name)))
    {
        l_tmp = 1;
    }
    else if (0 == (oal_strcmp("2", ac_name)))
    {
        l_tmp = 2;
    }
    else if (0 == (oal_strcmp("3", ac_name)))
    {
        l_tmp = 3;
    }
    else
    {
        OAM_ERROR_LOG1(0, OAM_SF_SMPS, "{wal_hipriv_set_smps_mode::error cmd[%x],input 1/2/3!}", (uintptr_t)ac_name);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SMPS_MODE, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = l_tmp;  /* ???????????????? */

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_SMPS, "{wal_hipriv_set_smps_mode::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_UAPSD

OAL_STATIC oal_uint32  wal_hipriv_set_uapsd_cap(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru          st_write_msg;
    oal_int32                   l_tmp;
    oal_uint32                  ul_off_set;
    oal_int8                    ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int32                   l_ret;
    oal_uint32                  ul_ret;

    /* ????????????"1"??"0"????ac_name */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_uapsd_cap::wal_get_cmd_one_arg_etc return err_code[%d]}\r\n", ul_ret);
        return ul_ret;
    }

    /* ????????????????????????UAPSD?????????????????? */
    if (0 == (oal_strcmp("0", ac_name)))
    {
        l_tmp = 0;
    }
    else if (0 == (oal_strcmp("1", ac_name)))
    {
        l_tmp = 1;
    }
    else
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_uapsd_cap::the log switch command is error [%x]!}\r\n", (uintptr_t)ac_name);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_UAPSD_EN, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = l_tmp;  /* ???????????????? */

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_event_switch::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif


OAL_STATIC oal_uint32  wal_hipriv_add_user(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret;
    oal_int32                       l_ret;
    mac_cfg_add_user_param_stru    *pst_add_user_param = OAL_PTR_NULL;
    mac_cfg_add_user_param_stru     st_add_user_param;  /* ??????????????use?????? */
    oal_uint32                      ul_get_addr_idx;

    /*
        ??????????????????????: hipriv "vap0 add_user xx xx xx xx xx xx(mac????) 0 | 1(HT??????) "
        ????????????????VAP
    */

    /* ????mac???? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_add_user::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    memset_s((oal_uint8*)&st_add_user_param, OAL_SIZEOF(st_add_user_param), 0, OAL_SIZEOF(st_add_user_param));
    oal_strtoaddr(ac_name, OAL_SIZEOF(ac_name), st_add_user_param.auc_mac_addr, WLAN_MAC_ADDR_LEN);
    /* ?????????????????? */
    pc_param = pc_param + ul_off_set;

    /* ??????????HT???? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_add_user::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ????????????????????????user??HT?????????????????? */
    if (0 == (oal_strcmp("0", ac_name)))
    {
        st_add_user_param.en_ht_cap = 0;
    }
    else if (0 == (oal_strcmp("1", ac_name)))
    {
        st_add_user_param.en_ht_cap = 1;
    }
    else
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_add_user::the mod switch command is error [%x]!}\r\n", (uintptr_t)ac_name);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_ADD_USER, OAL_SIZEOF(mac_cfg_add_user_param_stru));

    /* ???????????????? */
    pst_add_user_param = (mac_cfg_add_user_param_stru *)(st_write_msg.auc_value);
    for (ul_get_addr_idx = 0; ul_get_addr_idx < WLAN_MAC_ADDR_LEN; ul_get_addr_idx++)
    {
        pst_add_user_param->auc_mac_addr[ul_get_addr_idx] = st_add_user_param.auc_mac_addr[ul_get_addr_idx];
    }
    pst_add_user_param->en_ht_cap = st_add_user_param.en_ht_cap;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_add_user_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_add_user::return err code[%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_del_user(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret;
    oal_int32                       l_ret;
    mac_cfg_del_user_param_stru    *pst_del_user_param = OAL_PTR_NULL;
    mac_cfg_del_user_param_stru     st_del_user_param;  /* ??????????????use?????? */
    oal_uint32                      ul_get_addr_idx;

    /*
        ??????????????????????: hipriv "vap0 del_user xx xx xx xx xx xx(mac????)"
        ????????????????VAP
    */

    /* ????mac???? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_del_user::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    memset_s((oal_uint8*)&st_del_user_param, OAL_SIZEOF(st_del_user_param), 0, OAL_SIZEOF(st_del_user_param));
    oal_strtoaddr(ac_name, OAL_SIZEOF(ac_name), st_del_user_param.auc_mac_addr, WLAN_MAC_ADDR_LEN);

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DEL_USER, OAL_SIZEOF(mac_cfg_add_user_param_stru));

    /* ???????????????? */
    pst_del_user_param = (mac_cfg_add_user_param_stru *)(st_write_msg.auc_value);
    for (ul_get_addr_idx = 0; ul_get_addr_idx < WLAN_MAC_ADDR_LEN; ul_get_addr_idx++)
    {
        pst_del_user_param->auc_mac_addr[ul_get_addr_idx] = st_del_user_param.auc_mac_addr[ul_get_addr_idx];
    }

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_add_user_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_del_user::return err code[%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_user_info(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    mac_vap_stru                    *pst_mac_vap = OAL_PTR_NULL;
    wal_msg_write_stru              st_write_msg;
    oal_int32                      l_ret;
    mac_cfg_user_info_param_stru    *pst_user_info_param = OAL_PTR_NULL;
    oal_uint8                       auc_mac_addr[6] = {0};    /* ??????????????use??mac???????? */
    oal_uint8                       uc_char_index;
    oal_uint16                      us_user_idx;

    /* ???????????????? */
    pc_param++;

    /* ????mac????,16???????? */
    for (uc_char_index = 0; uc_char_index < 12; uc_char_index++)
    {
        if (':' == *pc_param)
        {
            pc_param++;
            if (0 != uc_char_index)
            {
                uc_char_index--;
            }

            continue;
        }

        auc_mac_addr[uc_char_index/2] =
        (oal_uint8)(auc_mac_addr[uc_char_index/2] * 16 * (uc_char_index % 2) +
                                        oal_strtohex(pc_param));
        pc_param++;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_USER_INFO, OAL_SIZEOF(mac_cfg_user_info_param_stru));

    /* ????mac?????????? */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);

    l_ret = (oal_int32)mac_vap_find_user_by_macaddr_etc(pst_mac_vap, auc_mac_addr, &us_user_idx);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_user_info::no such user!}\r\n");
        return OAL_FAIL;
    }

    /* ???????????????? */
    pst_user_info_param              = (mac_cfg_user_info_param_stru *)(st_write_msg.auc_value);
    pst_user_info_param->us_user_idx = us_user_idx;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_user_info_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_user_info::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_set_mcast_data_dscr_param(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_set_dscr_param_stru     *pst_set_dscr_param = OAL_PTR_NULL;
    wal_dscr_param_enum_uint8        en_param_index;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_DSCR, OAL_SIZEOF(mac_cfg_set_dscr_param_stru));

    /* ?????????????????????? */
    pst_set_dscr_param = (mac_cfg_set_dscr_param_stru *)(st_write_msg.auc_value);

    /* ???????????????????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcast_data_dscr_param::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }
    pc_param += ul_off_set;

    /* ???????????????????? */
    for (en_param_index = 0; en_param_index < WAL_DSCR_PARAM_BUTT; en_param_index++)
    {
        if(!oal_strcmp(pauc_tx_dscr_param_name_etc[en_param_index], ac_arg))
        {
            break;
        }
    }

    /* ???????????????? */
    if (WAL_DSCR_PARAM_BUTT == en_param_index)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_mcast_data_dscr_param::no such param for tx dscr!}\r\n");
        return OAL_FAIL;
    }

    pst_set_dscr_param->uc_function_index = en_param_index;

    /*????????????????????????*/
    if(en_param_index >= WAL_DSCR_PARAM_RATE && en_param_index <= WAL_DSCR_PARAM_BW)
    {
        ul_ret = wal_hipriv_process_rate_params(pst_net_dev, pc_param, pst_set_dscr_param);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcast_data_dscr_param::wal_hipriv_process_ucast_params return err_code [%d]!}\r\n", ul_ret);
            return ul_ret;
        }
    }
    else
    {
        /* ???????????????????? */
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcast_data_dscr_param::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
            return ul_ret;
        }
        pc_param += ul_off_set;
        pst_set_dscr_param->l_value = oal_strtol(ac_arg, OAL_PTR_NULL, 0);
    }

    /* ???????????????????? tpye = MAC_VAP_CONFIG_MCAST_DATA */
    pst_set_dscr_param->en_type = MAC_VAP_CONFIG_MCAST_DATA;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_set_dscr_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcast_data_dscr_param::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;

}



oal_uint32  wal_hipriv_set_rate_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
//#ifdef _PRE_WLAN_CHIP_TEST
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_non_ht_rate_stru         *pst_set_rate_param = OAL_PTR_NULL;
    wlan_legacy_rate_value_enum_uint8  en_rate_index;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    wal_msg_stru                *pst_rsp_msg = OAL_PTR_NULL;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_RATE, OAL_SIZEOF(mac_cfg_non_ht_rate_stru));

    /* ?????????????????????? */
    pst_set_rate_param = (mac_cfg_non_ht_rate_stru *)(st_write_msg.auc_value);

    /* ???????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_rate_etc::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ?????????????????????? */
    for (en_rate_index = 0; en_rate_index < WLAN_LEGACY_RATE_VALUE_BUTT; en_rate_index++)
    {
        if(!oal_strcmp(pauc_non_ht_rate_tbl[en_rate_index], ac_arg))
        {
            break;
        }
    }

    /* ???????????????????? */
    pst_set_rate_param->en_rate = en_rate_index;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_non_ht_rate_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_rate_etc::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
        /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_set_rate_etc fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }
//#endif  /* _PRE_WLAN_CHIP_TEST */
    return OAL_SUCC;
}


oal_uint32  wal_hipriv_set_mcs_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
//#ifdef _PRE_WLAN_CHIP_TEST
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_tx_comp_stru             *pst_set_mcs_param = OAL_PTR_NULL;
    oal_int32                        l_mcs;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int32                        l_idx = 0;
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_MCS, OAL_SIZEOF(mac_cfg_tx_comp_stru));

    /* ?????????????????????? */
    pst_set_mcs_param = (mac_cfg_tx_comp_stru *)(st_write_msg.auc_value);

    /* ???????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcs_etc::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ?????????????????? */
    while ('\0' != ac_arg[l_idx])
    {
        if (isdigit(ac_arg[l_idx]))
        {
            l_idx++;
            continue;
        }
        else
        {
            l_idx++;
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_mcs_etc::input illegal!}\r\n");
            return OAL_ERR_CODE_INVALID_CONFIG;
        }
    }

    /* ???????????????????? */
    l_mcs = oal_atoi(ac_arg);

    if (l_mcs < WAL_HIPRIV_HT_MCS_MIN || l_mcs > WAL_HIPRIV_HT_MCS_MAX)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcs_etc::input val out of range [%d]!}\r\n", l_mcs);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    pst_set_mcs_param->uc_param = (oal_uint8)l_mcs;
    //pst_set_mcs_param->en_protocol_mode = WLAN_HT_PHY_PROTOCOL_MODE;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_tx_comp_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcs_etc::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
        /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_set_mcs_etc fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }
//#endif  /* _PRE_WLAN_CHIP_TEST */
    return OAL_SUCC;
}


oal_uint32  wal_hipriv_set_mcsac_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
//#ifdef _PRE_WLAN_CHIP_TEST
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_tx_comp_stru             *pst_set_mcs_param = OAL_PTR_NULL;
    oal_int32                        l_mcs;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int32                        l_idx = 0;
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_MCSAC, OAL_SIZEOF(mac_cfg_tx_comp_stru));

    /* ?????????????????????? */
    pst_set_mcs_param = (mac_cfg_tx_comp_stru *)(st_write_msg.auc_value);

    /* ???????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcsac_etc::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ?????????????????? */
    while ('\0' != ac_arg[l_idx])
    {
        if (isdigit(ac_arg[l_idx]))
        {
            l_idx++;
            continue;
        }
        else
        {
            l_idx++;
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_mcsac_etc::input illegal!}\r\n");
            return OAL_ERR_CODE_INVALID_CONFIG;
        }
    }

    /* ???????????????????? */
    l_mcs = oal_atoi(ac_arg);

    if (l_mcs < WAL_HIPRIV_VHT_MCS_MIN || l_mcs > WAL_HIPRIV_VHT_MCS_MAX)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcs_etc::input val out of range [%d]!}\r\n", l_mcs);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    pst_set_mcs_param->uc_param = (oal_uint8)l_mcs;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_tx_comp_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcsac_etc::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
        /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_set_mcsac_etc fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

//#endif  /* _PRE_WLAN_CHIP_TEST */
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_11AX

oal_uint32  wal_hipriv_set_mcsax(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_tx_comp_stru             *pst_set_mcs_param;
    oal_int32                        l_mcs;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int32                        l_idx = 0;
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_MCSAX, OAL_SIZEOF(mac_cfg_tx_comp_stru));

    /* ?????????????????????? */
    pst_set_mcs_param = (mac_cfg_tx_comp_stru *)(st_write_msg.auc_value);

    /* ???????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcsax::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ?????????????????? */
    while ('\0' != ac_arg[l_idx])
    {
        if (isdigit(ac_arg[l_idx]))
        {
            l_idx++;
            continue;
        }
        else
        {
            l_idx++;
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_mcsax::input illegal!}\r\n");
            return OAL_ERR_CODE_INVALID_CONFIG;
        }
    }

    /* ???????????????????? */
    l_mcs = oal_atoi(ac_arg);

    if (l_mcs < WAL_HIPRIV_HE_MCS_MIN || l_mcs > WAL_HIPRIV_HE_MCS_MAX)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcsax::input val out of range [%d]!}\r\n", l_mcs);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    pst_set_mcs_param->uc_param = (oal_uint8)l_mcs;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_tx_comp_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcsax::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
        /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_set_mcsax fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}
#ifdef _PRE_WLAN_FEATURE_11AX_ER_SU

oal_uint32  wal_hipriv_set_mcsax_er(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_tx_comp_stru             *pst_set_mcs_param;
    oal_int32                        l_mcs;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int32                        l_idx = 0;
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_MCSAX_ER, OAL_SIZEOF(mac_cfg_tx_comp_stru));

    /* ?????????????????????? */
    pst_set_mcs_param = (mac_cfg_tx_comp_stru *)(st_write_msg.auc_value);

    /* ???????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcsax_er::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ?????????????????? */
    while ('\0' != ac_arg[l_idx])
    {
        if (isdigit(ac_arg[l_idx]))
        {
            l_idx++;
            continue;
        }
        else
        {
            l_idx++;
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_mcsax_er::input illegal!}\r\n");
            return OAL_ERR_CODE_INVALID_CONFIG;
        }
    }

    /* ???????????????????? */
    l_mcs = oal_atoi(ac_arg);

    if (l_mcs < WAL_HIPRIV_HE_ER_MCS_MIN || l_mcs > WAL_HIPRIV_HE_ER_MCS_MAX)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcsax_er::input val out of range [%d]!}\r\n", l_mcs);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    pst_set_mcs_param->uc_param = (oal_uint8)l_mcs;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_tx_comp_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_mcsax_er::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
        /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_set_mcsax_er fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}
#endif
#endif


OAL_STATIC oal_uint32  wal_hipriv_set_bw(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
//#ifdef _PRE_WLAN_CHIP_TEST
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_tx_comp_stru             *pst_set_bw_param = OAL_PTR_NULL;
    wlan_bandwith_cap_enum_uint8     en_bw_index;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_BW, OAL_SIZEOF(mac_cfg_tx_comp_stru));

    /* ?????????????????????? */
    pst_set_bw_param = (mac_cfg_tx_comp_stru *)(st_write_msg.auc_value);

    /* ???????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_bw::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ???????????????????? */
    for (en_bw_index = 0; en_bw_index < WLAN_BANDWITH_CAP_BUTT; en_bw_index++)
    {
        if(!oal_strcmp(pauc_bw_tbl[en_bw_index], ac_arg))
        {
            break;
        }
    }

    /* ???????????????? */
    if (en_bw_index >= WLAN_BANDWITH_CAP_BUTT)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_bw::not support this bandwidth!}\r\n");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    pst_set_bw_param->uc_param = (oal_uint8)(en_bw_index);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_tx_comp_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_bw::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
    /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_set_bw fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }
//#endif  /* _PRE_WLAN_CHIP_TEST */
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX

OAL_STATIC oal_uint32  wal_hipriv_always_tx(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_tx_comp_stru             *pst_set_bcast_param;
    oal_int8                         ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint8                        en_tx_flag = HAL_ALWAYS_TX_DISABLE;
    mac_rf_payload_enum_uint8        en_payload_flag = RF_PAYLOAD_RAND;
    oal_uint32                       ul_len = 2000;
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;
    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_ALWAYS_TX, OAL_SIZEOF(mac_cfg_tx_comp_stru));

    /* ?????????????????????? */
    pst_set_bcast_param = (mac_cfg_tx_comp_stru *)(st_write_msg.auc_value);

    /* ???????????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }
    en_tx_flag = (oal_uint8)oal_atoi(ac_name);

    if (en_tx_flag >= HAL_ALWAYS_TX_MPDU)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_always_tx::input should be 0 or 1.}\r\n");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }
    /* ???????????????????????????????? */
    if (HAL_ALWAYS_TX_DISABLE != en_tx_flag)
    {
        /* ack_policy???????????? */

        /* ????payload_flag???? */
        pc_param = pc_param + ul_off_set;
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        /* ??????payload_flag??len??????????????????????RF_PAYLOAD_RAND 2000 */
        if (OAL_SUCC == ul_ret)
        {
            en_payload_flag = (oal_uint8)oal_atoi(ac_name);
            if(en_payload_flag >= RF_PAYLOAD_BUTT)
            {
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx::payload flag err[%d]!}\r\n", en_payload_flag);
                return OAL_ERR_CODE_INVALID_CONFIG;
            }

            /* ????len???? */
            pc_param = pc_param + ul_off_set;
            ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
            if (OAL_SUCC != ul_ret)
            {
                 OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
                 return ul_ret;
            }
            ul_len = (oal_uint16)oal_atoi(ac_name);
            if(ul_len > 65535)
            {
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx::len [%u] overflow!}\r\n", ul_len);
                return OAL_ERR_CODE_INVALID_CONFIG;
            }
        }
    }


    pst_set_bcast_param->en_payload_flag = en_payload_flag;
    pst_set_bcast_param->ul_payload_len = ul_len;
    pst_set_bcast_param->uc_param = en_tx_flag;
    OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_hipriv_always_tx::tx_flag[%d],len[%d]!}\r\n", en_tx_flag,ul_len);
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_tx_comp_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_always_tx fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}



OAL_STATIC oal_uint32  wal_hipriv_always_tx_aggr_num(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                      *pul_num;
    oal_uint32                       ul_ret;
    oal_int8                         ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                       ul_off_set;
    oal_int32                        l_ret;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_ALWAYS_TX_AGGR_NUM, OAL_SIZEOF(oal_uint32));

    /* ???????????? */
    pul_num = (oal_uint32 *)(st_write_msg.auc_value);

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_aggr_num::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }
    *pul_num = (oal_uint32)oal_atoi(ac_name);
    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_aggr_num::altx aggr num[%d]!}\r\n", *pul_num);
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_aggr_num::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;

}

OAL_STATIC oal_uint32  wal_hipriv_always_tx_num(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                      *pul_num;
    oal_uint32                       ul_ret;
    oal_int8                         ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                       ul_off_set;
    oal_int32                        l_ret;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_ALWAYS_TX_NUM, OAL_SIZEOF(oal_uint32));

    /* ???????????? */
    pul_num = (oal_uint32 *)(st_write_msg.auc_value);

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_packet_xmit::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }
    *pul_num = (oal_uint32)oal_atoi(ac_name);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;

}


OAL_STATIC oal_uint32  wal_hipriv_always_tx_hw_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_al_tx_hw_cfg_stru       *pst_cfg;
    oal_int8                         ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int8                        *pc_end;
    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_ALWAYS_TX_HW_CFG, OAL_SIZEOF(mac_cfg_al_tx_hw_cfg_stru));

    /* ?????????????????????? */
    pst_cfg = (mac_cfg_al_tx_hw_cfg_stru *)(st_write_msg.auc_value);

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw_cfg::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }
    pc_param = pc_param + ul_off_set;
    pst_cfg->ul_devid = (oal_uint32)oal_strtol(ac_name, &pc_end, 16);

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw_cfg::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }
    pc_param = pc_param + ul_off_set;
    pst_cfg->ul_mode = (oal_uint32)oal_strtol(ac_name, &pc_end, 16);

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw_cfg::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }
    pc_param = pc_param + ul_off_set;
    pst_cfg->ul_rate = (oal_uint32)oal_strtol(ac_name, &pc_end, 16);


    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_al_tx_hw_cfg_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw_cfg::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}




OAL_STATIC oal_uint32  wal_hipriv_always_tx_hw(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_al_tx_hw_stru           *pst_al_hw_tx;
    oal_int8                         ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint8                        uc_value;
    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_ALWAYS_TX_HW, OAL_SIZEOF(mac_cfg_al_tx_hw_stru));

    /* ?????????????????????? */
    pst_al_hw_tx = (mac_cfg_al_tx_hw_stru *)(st_write_msg.auc_value);

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    uc_value = (oal_uint8)oal_atoi(ac_name);
    pc_param = pc_param + ul_off_set;
    //uc_dev_id = (pst_al_hw_tx->bit_switch & 0x2);
    pst_al_hw_tx->bit_switch = uc_value & 0x01;
    pst_al_hw_tx->bit_dev_id = uc_value & 0x02;
    pst_al_hw_tx->bit_flag   = (uc_value & 0x0c) >> 2; /* 0b1100 */
    if (OAL_SWITCH_ON == pst_al_hw_tx->bit_switch)
    {

        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
             OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
             return ul_ret;
        }
        pc_param = pc_param + ul_off_set;
        pst_al_hw_tx->uc_content = (oal_uint8)oal_atoi(ac_name);


        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
             OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
             return ul_ret;
        }
        pc_param = pc_param + ul_off_set;
        pst_al_hw_tx->ul_len = (oal_uint32)oal_atoi(ac_name);
        if(65535 < pst_al_hw_tx->ul_len)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw::len [%u] overflow!}\r\n", pst_al_hw_tx->ul_len);
            return OAL_ERR_CODE_INVALID_CONFIG;
        }

        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
             OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
             return ul_ret;
        }
        pc_param += ul_off_set;
        pst_al_hw_tx->ul_times = (oal_uint32)oal_atoi(ac_name);

        /* ?????????????? */
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
             OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
             return ul_ret;
        }
        pst_al_hw_tx->ul_ifs = (oal_uint32)oal_atoi(ac_name);
        if(pst_al_hw_tx->ul_ifs > 65535)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw::al_tx ifs [%u] overflow!}\r\n", pst_al_hw_tx->ul_ifs);
            return OAL_ERR_CODE_INVALID_CONFIG;
        }
    }


    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_al_tx_hw_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_tx_hw::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif


OAL_STATIC oal_uint32  wal_hipriv_always_rx(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint8                        uc_rx_flag;
    oal_int32                        l_idx = 0;
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;

    /* ???????????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_rx::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ?????????????????? */
    while ('\0' != ac_arg[l_idx])
    {
        if (isdigit(ac_arg[l_idx]))
        {
            l_idx++;
            continue;
        }
        else
        {
            l_idx++;
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_always_rx::input illegal!}\r\n");
            return OAL_ERR_CODE_INVALID_CONFIG;
        }
    }

    /* ???????????????????????????? */
    uc_rx_flag = (oal_uint8)oal_atoi(ac_arg);

    if ( uc_rx_flag > HAL_ALWAYS_RX_RESERVED)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_always_rx::input should be 0 or 1.}\r\n");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    *(oal_uint8 *)(st_write_msg.auc_value) = uc_rx_flag;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_ALWAYS_RX, OAL_SIZEOF(oal_uint8));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_always_rx::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
        /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_always_rx fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}
#ifdef _PRE_WLAN_FEATURE_MONITOR

OAL_STATIC oal_uint32  wal_hipriv_set_sniffer(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint8                        uc_sniffer_mode;
    oal_int32                        l_idx = 0;
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;

    /* ????sniffer???????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_sniffer::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ?????????????????? */
    while ('\0' != ac_arg[l_idx])
    {
        if (isdigit(ac_arg[l_idx]))
        {
            l_idx++;
            continue;
        }
        else
        {
            l_idx++;
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_sniffer::input illegal!}\r\n");
            return OAL_ERR_CODE_INVALID_CONFIG;
        }
    }

    /* ???????????????????????????? */
    uc_sniffer_mode = (oal_uint8)oal_atoi(ac_arg);

    if (uc_sniffer_mode > WLAN_SNIFFER_STATE_BUTT)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_sniffer::input should be 0 or 1.}\r\n");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    *(oal_uint8 *)(st_write_msg.auc_value) = uc_sniffer_mode;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_sniffer::uc_sniffer_mode [%d].}\r\n", uc_sniffer_mode);

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SNIFFER, OAL_SIZEOF(oal_uint8));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_sniffer::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
        /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_set_sniffer fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_enable_monitor_mode(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint8                        uc_sniffer_mode;
    oal_int32                        l_idx = 0;
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;

    /* ????monitor mode???????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_enable_monitor_mode::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ?????????????????? */
    while ('\0' != ac_arg[l_idx])
    {
        if (isdigit(ac_arg[l_idx]))
        {
            l_idx++;
            continue;
        }
        else
        {
            l_idx++;
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_enable_monitor_mode::input illegal!}\r\n");
            return OAL_ERR_CODE_INVALID_CONFIG;
        }
    }

    /* ???????????????????????????? */
    uc_sniffer_mode = (oal_uint8)oal_atoi(ac_arg);

    if (uc_sniffer_mode > WLAN_SNIFFER_STATE_BUTT)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_enable_monitor_mode::input should be 0 or 1.}\r\n");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    *(oal_uint8 *)(st_write_msg.auc_value) = uc_sniffer_mode;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_enable_monitor_mode::uc_sniffer_mode [%d].}\r\n", uc_sniffer_mode);

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_MONITOR_MODE, OAL_SIZEOF(oal_uint8));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_enable_monitor_mode::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
        /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_enable_monitor_mode fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}
#endif


OAL_STATIC oal_uint32  wal_hipriv_pcie_pm_level(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru          st_write_msg;
    oal_int8                    ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_int32                   l_ret;
    oal_uint16                  us_len;
    oal_uint32                  ul_ret;
    oal_uint32                  ul_off_set;
    mac_cfg_pcie_pm_level_stru     *pst_pcie_pm_level;

    /* ????????: hipriv "Hisilicon0 pcie_pm_level level(0/1/2/3/4)" */
    pst_pcie_pm_level = (mac_cfg_pcie_pm_level_stru *)st_write_msg.auc_value;

    /* ppm */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_pcie_pm_level::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    pst_pcie_pm_level->uc_pcie_pm_level = (oal_uint8)oal_atoi(ac_arg);
    if (pst_pcie_pm_level->uc_pcie_pm_level > 4)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_pcie_pm_level::pcie pm level must in set(0/1/2/3/4);\r\n", pst_pcie_pm_level->uc_pcie_pm_level);
        return ul_ret;
    }

    us_len = OAL_SIZEOF(mac_cfg_pcie_pm_level_stru);
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_PCIE_PM_LEVEL, us_len);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                             WAL_MSG_TYPE_WRITE,
                             WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                             (oal_uint8 *)&st_write_msg,
                             OAL_FALSE,
                             OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_pcie_pm_level::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}


OAL_STATIC int wal_ioctl_get_iwname(oal_net_device_stru *pst_net_dev,
                                    oal_iw_request_info_stru *pst_info,
                                    oal_iwreq_data_union *pst_wrqu,
                                    char *pc_extra)
{
    oal_int8 ac_iwname[] = "IEEE 802.11";

    if ((pst_net_dev == OAL_PTR_NULL) || (pst_wrqu == OAL_PTR_NULL)) {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_ioctl_get_iwname::param null, pst_net_dev = %p, pc_name = %p.}",
                        (uintptr_t)pst_net_dev, (uintptr_t)pst_wrqu);
        return -OAL_EINVAL;
    }

    if (EOK != memcpy_s(pst_wrqu->name, OAL_IF_NAME_SIZE, ac_iwname, OAL_SIZEOF(ac_iwname))) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_ioctl_get_iwname::memcpy fail!");
        return -OAL_EINVAL;
    }
    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_find_cmd(oal_int8 *pc_cmd_name, oal_uint8 uc_cmd_name_len, wal_hipriv_cmd_entry_stru **pst_cmd_id)
{
    oal_uint32                en_cmd_idx;
    int                       l_ret;
    *pst_cmd_id               = NULL;

    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR2(pc_cmd_name,pst_cmd_id)))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_hipriv_find_cmd::pc_cmd_name/puc_cmd_id null ptr error [%x] [%x]!}\r\n", (uintptr_t)pc_cmd_name, (uintptr_t)pst_cmd_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (uc_cmd_name_len > WAL_HIPRIV_CMD_NAME_MAX_LEN) {
        return OAL_FAIL;
    }

    for (en_cmd_idx = 0; en_cmd_idx < OAL_ARRAY_SIZE(g_ast_hipriv_cmd); en_cmd_idx++)
    {
        l_ret = oal_strcmp(g_ast_hipriv_cmd[en_cmd_idx].pc_cmd_name, pc_cmd_name);

        if (0 == l_ret)
        {
            *pst_cmd_id = (wal_hipriv_cmd_entry_stru*)&g_ast_hipriv_cmd[en_cmd_idx];
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_find_cmd::wal_hipriv_find_cmd en_cmd_idx = %d.}\r\n", en_cmd_idx);

            return OAL_SUCC;
        }
    }

#ifdef _PRE_WLAN_CFGID_DEBUG
    for (en_cmd_idx = 0; en_cmd_idx < wal_hipriv_get_debug_cmd_size_etc(); en_cmd_idx++)
    {
        l_ret = oal_strcmp(g_ast_hipriv_cmd_debug_etc[en_cmd_idx].pc_cmd_name, pc_cmd_name);

        if (0 == l_ret)
        {
            *pst_cmd_id = (wal_hipriv_cmd_entry_stru*)&g_ast_hipriv_cmd_debug_etc[en_cmd_idx];
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_find_cmd::wal_hipriv_find_cmd en_cmd_idx = %d.}\r\n", en_cmd_idx);

            return OAL_SUCC;
        }
    }
#endif

    OAM_IO_PRINTK("cmd name[%s] is not exist. \r\n", pc_cmd_name);
    return OAL_FAIL;
}


OAL_STATIC oal_uint32  wal_hipriv_get_cmd_net_dev(oal_int8 *pc_cmd, oal_net_device_stru **ppst_net_dev, oal_uint32 *pul_off_set)
{
    oal_net_device_stru  *pst_net_dev = OAL_PTR_NULL;
    oal_int8              ac_dev_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32            ul_ret;

    if (OAL_ANY_NULL_PTR3(pc_cmd,ppst_net_dev,pul_off_set))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "{wal_hipriv_get_cmd_net_dev::pc_cmd/ppst_net_dev/pul_off_set null ptr error [%x] [%x] [%x]!}\r\n", (uintptr_t)pc_cmd, (uintptr_t)ppst_net_dev, (uintptr_t)pul_off_set);
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = wal_get_cmd_one_arg_etc(pc_cmd, ac_dev_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, pul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_get_cmd_net_dev::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ????dev_name????dev */
    pst_net_dev = oal_dev_get_by_name(ac_dev_name);
    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_get_cmd_net_dev::oal_dev_get_by_name return null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ????oal_dev_get_by_name????????????oal_dev_put??net_dev?????????????? */
    oal_dev_put(pst_net_dev);

    *ppst_net_dev = pst_net_dev;

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_get_cmd_id(oal_int8 *pc_cmd, wal_hipriv_cmd_entry_stru **pst_cmd_id, oal_uint32 *pul_off_set)
{
    oal_int8                    ac_cmd_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                  ul_ret;

    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR3(pc_cmd,pst_cmd_id,pul_off_set)))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "{wal_hipriv_get_cmd_id::pc_cmd/puc_cmd_id/pul_off_set null ptr error [%x] [%x] [%x]!}\r\n", (uintptr_t)pc_cmd, (uintptr_t)pst_cmd_id, (uintptr_t)pul_off_set);
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = wal_get_cmd_one_arg_etc(pc_cmd, ac_cmd_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, pul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_get_cmd_id::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    /* ?????????????????????? */
    ul_ret = wal_hipriv_find_cmd(ac_cmd_name, sizeof(ac_cmd_name), pst_cmd_id);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_get_cmd_id::wal_hipriv_find_cmd return error cod [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}


oal_uint32  wal_hipriv_parse_cmd_etc(oal_int8 *pc_cmd)
{
    oal_net_device_stru        *pst_net_dev = OAL_PTR_NULL;
    wal_hipriv_cmd_entry_stru  *pst_hipriv_cmd_entry = NULL;
    oal_uint32                  ul_off_set = 0;
    oal_uint32                  ul_ret;
    if (OAL_UNLIKELY(OAL_PTR_NULL == pc_cmd))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_hipriv_parse_cmd_etc::pc_cmd null ptr error!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /***************************************************************************
        cmd????????
        ?????????? ????      ????   Hisilicon0 create vap0
        1~15Byte   1~15Byte
    **************************** ***********************************************/

    ul_ret = wal_hipriv_get_cmd_net_dev(pc_cmd, &pst_net_dev, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_parse_cmd_etc::wal_hipriv_get_cmd_net_dev return error code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    pc_cmd += ul_off_set;
    ul_ret  = wal_hipriv_get_cmd_id(pc_cmd, &pst_hipriv_cmd_entry, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_parse_cmd_etc::wal_hipriv_get_cmd_id return error code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    pc_cmd += ul_off_set;
    /* ?????????????????? */
    ul_ret = pst_hipriv_cmd_entry->p_func(pst_net_dev, pc_cmd);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_parse_cmd_etc::g_ast_hipriv_cmd return error code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT

OAL_STATIC oal_ssize_t  wal_hipriv_sys_write(struct kobject *dev, struct kobj_attribute *attr, const char *pc_buffer, oal_size_t count)
{
    oal_int8  *pc_cmd;
    oal_uint32 ul_ret;
    oal_uint32 ul_len = (oal_uint32)count;

    if (ul_len > WAL_HIPRIV_CMD_MAX_LEN)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_sys_write::ul_len>WAL_HIPRIV_CMD_MAX_LEN, ul_len [%d]!}\r\n", ul_len);
        return -OAL_EINVAL;
    }

    pc_cmd = OAL_MEM_ALLOC(OAL_MEM_POOL_ID_LOCAL, WAL_HIPRIV_CMD_MAX_LEN, OAL_TRUE);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pc_cmd))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_proc_write::alloc mem return null ptr!}\r\n");
        return -OAL_ENOMEM;
    }

    memset_s(pc_cmd, WAL_HIPRIV_CMD_MAX_LEN, 0, WAL_HIPRIV_CMD_MAX_LEN);

    if (EOK != memcpy_s(pc_cmd, WAL_HIPRIV_CMD_MAX_LEN, pc_buffer, ul_len)) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_sys_write::memcpy fail!");
        OAL_MEM_FREE(pc_cmd, OAL_TRUE);
        return -OAL_EINVAL;
    }

    pc_cmd[ul_len - 1] = '\0';

    ul_ret = wal_hipriv_parse_cmd_etc(pc_cmd);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_proc_write::parse cmd return err code[%d]!}\r\n", ul_ret);
    }

    OAL_MEM_FREE(pc_cmd, OAL_TRUE);

    return (oal_int32)ul_len;
}


#define SYS_READ_MAX_STRING_LEN (4096-40)   /* ????????????????20????????????40???????????? */
OAL_STATIC oal_ssize_t  wal_hipriv_sys_read(struct kobject *dev, struct kobj_attribute *attr, char *pc_buffer)
{
    oal_uint32              ul_cmd_idx;
    oal_uint32              buff_index = 0;

    for (ul_cmd_idx = 0; ul_cmd_idx < OAL_ARRAY_SIZE(g_ast_hipriv_cmd); ul_cmd_idx++)
    {
        buff_index += snprintf_s(pc_buffer + buff_index,(SYS_READ_MAX_STRING_LEN - buff_index),
                                (SYS_READ_MAX_STRING_LEN - buff_index) - 1,
                                "\t%s\n", g_ast_hipriv_cmd[ul_cmd_idx].pc_cmd_name);

        if(buff_index > SYS_READ_MAX_STRING_LEN){
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_sys_read::snprintf_s error!");
            return buff_index;
        }
    }
#ifdef _PRE_WLAN_CFGID_DEBUG
    for (ul_cmd_idx = 0; ul_cmd_idx < wal_hipriv_get_debug_cmd_size_etc(); ul_cmd_idx++)
    {
        buff_index += snprintf_s(pc_buffer + buff_index, (SYS_READ_MAX_STRING_LEN - buff_index),
                                 (SYS_READ_MAX_STRING_LEN - buff_index) - 1,
                                 "\t%s\n",g_ast_hipriv_cmd_debug_etc[ul_cmd_idx].pc_cmd_name);

        if(buff_index > SYS_READ_MAX_STRING_LEN){
            OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_sys_read::snprintf_s error!");
            break;
        }
    }
#endif

    return buff_index;
}

#endif  /* _PRE_OS_VERSION_LINUX */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44))
    // use sys filesystem instead
#else

OAL_STATIC oal_int32  wal_hipriv_proc_write(oal_file_stru *pst_file, const oal_int8 *pc_buffer, oal_uint32 ul_len, oal_void *p_data)
{
    oal_int8                    *pc_cmd = OAL_PTR_NULL;
    oal_uint32                  ul_ret;

    if (ul_len > WAL_HIPRIV_CMD_MAX_LEN)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_hipriv_proc_write::ul_len>WAL_HIPRIV_CMD_MAX_LEN, ul_len [%d]!}\r\n", ul_len);
        return -OAL_EINVAL;
    }

    pc_cmd = OAL_MEM_ALLOC(OAL_MEM_POOL_ID_LOCAL, WAL_HIPRIV_CMD_MAX_LEN, OAL_TRUE);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pc_cmd))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_proc_write::alloc mem return null ptr!}\r\n");
        return -OAL_ENOMEM;
    }

    memset_s(pc_cmd, WAL_HIPRIV_CMD_MAX_LEN, 0, WAL_HIPRIV_CMD_MAX_LEN);

    ul_ret = oal_copy_from_user((oal_void *)pc_cmd, pc_buffer, ul_len);

    /* copy_from_user??????????????????????????????????????????????????????????????????????????????0 */
    if (ul_ret > 0)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_proc_write::oal_copy_from_user return ul_ret[%d]!}\r\n", ul_ret);
        OAL_MEM_FREE(pc_cmd, OAL_TRUE);

        return -OAL_EFAUL;
    }

    pc_cmd[ul_len - 1] = '\0';

    ul_ret = wal_hipriv_parse_cmd_etc(pc_cmd);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_proc_write::parse cmd return err code[%d]!}\r\n", ul_ret);
    }

    OAL_MEM_FREE(pc_cmd, OAL_TRUE);

    return (oal_int32)ul_len;

}
#endif

oal_uint32  wal_hipriv_create_proc_etc(oal_void *p_proc_arg)
{
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
    oal_uint32           ul_ret;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,44))
        /* TBD */
        g_pst_proc_entry = OAL_PTR_NULL;
#else

    /* 420??????????????????0644 linux???????? S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); */
    /* S_IRUSR??????????????????????, S_IWUSR??????????????????????, S_IRGRP??????????????????, S_IROTH???????????????????? */
    g_pst_proc_entry = oal_create_proc_entry(WAL_HIPRIV_PROC_ENTRY_NAME, 420, NULL);
    if (OAL_PTR_NULL == g_pst_proc_entry)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_create_proc_etc::oal_create_proc_entry return null ptr!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    g_pst_proc_entry->data  = p_proc_arg;
    g_pst_proc_entry->nlink = 1;        /* linux????proc?????? */
    g_pst_proc_entry->read_proc  = OAL_PTR_NULL;

    g_pst_proc_entry->write_proc = (write_proc_t *)wal_hipriv_proc_write;
#endif

    /* hi1102-cb add sys for 51/02 */
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
    gp_sys_kobject_etc = oal_get_sysfs_root_object_etc();
    if(NULL == gp_sys_kobject_etc)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_hipriv_create_proc_etc::get sysfs root object failed!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_ret = (oal_uint32)oal_debug_sysfs_create_group(gp_sys_kobject_etc,&hipriv_attribute_group);
    if (ul_ret)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_hipriv_create_proc_etc::hipriv_attribute_group create failed!}");
        ul_ret = OAL_ERR_CODE_PTR_NULL;
        return ul_ret;
    }
#endif

    return OAL_SUCC;
}


oal_uint32  wal_hipriv_remove_proc_etc(void)
{
/* ??????????sysfs */

#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
    if(NULL != gp_sys_kobject_etc)
    {
        oal_debug_sysfs_remove_group(gp_sys_kobject_etc, &hipriv_attribute_group);
        kobject_del(gp_sys_kobject_etc);
        gp_sys_kobject_etc = NULL;
    }
    oal_conn_sysfs_root_obj_exit_etc();
    oal_conn_sysfs_root_boot_obj_exit_etc();
#endif

    if (g_pst_proc_entry)
    {
        oal_remove_proc_entry(WAL_HIPRIV_PROC_ENTRY_NAME, NULL);
        g_pst_proc_entry = OAL_PTR_NULL;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_reg_info(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;
    oal_uint16                      us_len;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    l_ret = memcpy_s(st_write_msg.auc_value, OAL_STRLEN(pc_param), pc_param, OAL_STRLEN(pc_param));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_reg_info::memcpy fail!");
        return OAL_FAIL;
    }

    st_write_msg.auc_value[OAL_STRLEN(pc_param)] = '\0';

    us_len = (oal_uint16)(OAL_STRLEN(pc_param) + 1);

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_REG_INFO, us_len);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_reg_info::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}

#if (defined(_PRE_PRODUCT_ID_HI110X_DEV) || defined(_PRE_PRODUCT_ID_HI110X_HOST))

OAL_STATIC oal_uint32  wal_hipriv_sdio_flowctrl(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;
    oal_uint16                      us_len;

    if (OAL_UNLIKELY(WAL_MSG_WRITE_MAX_LEN <= OAL_STRLEN(pc_param)))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_sdio_flowctrl:: pc_param overlength is %d}\n",OAL_STRLEN(pc_param));
        return OAL_FAIL;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    l_ret = memcpy_s(st_write_msg.auc_value, OAL_STRLEN(pc_param), pc_param, OAL_STRLEN(pc_param));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_sdio_flowctrl::memcpy fail!");
        return OAL_FAIL;
    }

    st_write_msg.auc_value[OAL_STRLEN(pc_param)] = '\0';

    us_len = (oal_uint16)(OAL_STRLEN(pc_param) + 1);

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SDIO_FLOWCTRL, us_len);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_sdio_flowctrl::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif


OAL_STATIC oal_uint32 wal_hipriv_set_regdomain_pwr(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    oal_uint32                  ul_off_set;
    oal_int8                    ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                  ul_ret;
    oal_int32                   l_ret;
    oal_int32                   l_pwr;
    wal_msg_write_stru          st_write_msg;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "wal_hipriv_set_regdomain_pwr, get arg return err %d", ul_ret);
        return ul_ret;
    }

    l_pwr = oal_atoi(ac_name);
    if (l_pwr <= 0 || l_pwr > 100)
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "invalid value, %d", l_pwr);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_REGDOMAIN_PWR, OAL_SIZEOF(mac_cfg_regdomain_max_pwr_stru));

    ((mac_cfg_regdomain_max_pwr_stru *)st_write_msg.auc_value)->uc_pwr        = (oal_uint8)l_pwr;
    ((mac_cfg_regdomain_max_pwr_stru *)st_write_msg.auc_value)->en_exceed_reg = OAL_FALSE;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                       WAL_MSG_TYPE_WRITE,
                       WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                       (oal_uint8 *)&st_write_msg,
                       OAL_FALSE,
                       OAL_PTR_NULL);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0,OAM_SF_CFG,"{wal_hipriv_set_regdomain_pwr::wal_send_cfg_event_etc fail.return err code %d}",l_ret);
    }

    return (oal_uint32)l_ret;
}


OAL_STATIC oal_uint32  wal_hipriv_dump_all_rx_dscr(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru           st_write_msg;
    oal_int32                    l_ret;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DUMP_ALL_RX_DSCR, OAL_SIZEOF(oal_int32));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_dump_all_rx_dscr::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}



OAL_STATIC oal_uint32  wal_hipriv_reg_write(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{

    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;
    oal_uint16                      us_len;

    /***************************************************************************
                              ????????wal??????
    ***************************************************************************/
    l_ret = memcpy_s(st_write_msg.auc_value, OAL_STRLEN(pc_param), pc_param, OAL_STRLEN(pc_param));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_reg_write::memcpy fail!");
        return OAL_FAIL;
    }

    st_write_msg.auc_value[OAL_STRLEN(pc_param)] = '\0';

    us_len = (oal_uint16)(OAL_STRLEN(pc_param) + 1);

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_REG_WRITE, us_len);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                             WAL_MSG_TYPE_WRITE,
                             WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                             (oal_uint8 *)&st_write_msg,
                             OAL_FALSE,
                             OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_reg_write::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}



#ifdef _PRE_WLAN_FEATURE_DFS

OAL_STATIC oal_uint32  wal_hipriv_dfs_radartool(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru    st_write_msg;
    oal_uint16            us_len;
    oal_int32             l_ret;

    if (OAL_UNLIKELY(WAL_MSG_WRITE_MAX_LEN <= OAL_STRLEN(pc_param)))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_dfs_radartool:: pc_param overlength is %d}\n", OAL_STRLEN(pc_param));
        oal_print_hex_dump((oal_uint8 *)pc_param, WAL_MSG_WRITE_MAX_LEN, 32, "wal_hipriv_dfs_radartool: param is overlong:");
        return OAL_FAIL;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    l_ret = memcpy_s(st_write_msg.auc_value, OAL_STRLEN(pc_param), pc_param, OAL_STRLEN(pc_param));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "hwifi_config_init_fcc_ce_txpwr_nvram::memcpy fail!");
        return OAL_FAIL;
    }

    st_write_msg.auc_value[OAL_STRLEN(pc_param)] = '\0';

    us_len = (oal_uint16)(OAL_STRLEN(pc_param) + 1);

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_RADARTOOL, us_len);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_dfs_radartool::return err code [%d]!}\r\n", l_ret);

        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif   /* end of _PRE_WLAN_FEATURE_DFS */


oal_uint32  wal_hipriv_alg_cfg_etc(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret;
    mac_ioctl_alg_param_stru       *pst_alg_param = OAL_PTR_NULL;
    wal_ioctl_alg_cfg_stru          st_alg_cfg;
    oal_uint8                       uc_map_index = 0;
    oal_int32                       l_ret;
    wal_msg_stru                *pst_rsp_msg = OAL_PTR_NULL;

    pst_alg_param = (mac_ioctl_alg_param_stru *)(st_write_msg.auc_value);

    /* ???????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_alg_cfg_etc::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    /* ?????????????? */
    st_alg_cfg = g_ast_alg_cfg_map_etc[0];
    while(OAL_PTR_NULL != st_alg_cfg.pc_name)
    {
        if (0 == oal_strcmp(st_alg_cfg.pc_name, ac_name))
        {
            break;
        }
        st_alg_cfg = g_ast_alg_cfg_map_etc[++uc_map_index];
    }

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_alg_cfg:: alg_cfg command[%d]!}\r\n", st_alg_cfg.en_alg_cfg);

    /* ?????????????????????????? */
    if( OAL_PTR_NULL == st_alg_cfg.pc_name)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_alg_cfg_etc::invalid alg_cfg command!}\r\n");
        return OAL_FAIL;
    }

    /* ???????????????????? */
    pst_alg_param->en_alg_cfg = g_ast_alg_cfg_map_etc[uc_map_index].en_alg_cfg;

    /* ?????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param + ul_off_set, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_alg_cfg_etc::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    /* ?????????????? */
    pst_alg_param->ul_value = (oal_uint32)oal_atoi(ac_name);

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_ALG_PARAM, OAL_SIZEOF(mac_ioctl_alg_param_stru));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_ioctl_alg_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        return (oal_uint32)l_ret;
    }
    /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_alg_cfg_etc fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;

}



OAL_STATIC oal_uint32  wal_hipriv_tpc_log(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru                      st_write_msg;
    oal_uint32                              ul_off_set;
    oal_int8                                ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                              ul_ret;
    oal_int32                               l_ret;
    mac_ioctl_alg_tpc_log_param_stru       *pst_alg_tpc_log_param = OAL_PTR_NULL;
    wal_ioctl_alg_cfg_stru                  st_alg_cfg;
    oal_uint8                               uc_map_index = 0;
    oal_bool_enum_uint8                     en_stop_flag = OAL_FALSE;

    pst_alg_tpc_log_param = (mac_ioctl_alg_tpc_log_param_stru *)(st_write_msg.auc_value);

    /* ???????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_tpc_log::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }
    pc_param = pc_param + ul_off_set;

    /* ?????????????? */
    st_alg_cfg = g_ast_alg_cfg_map_etc[0];
    while(OAL_PTR_NULL != st_alg_cfg.pc_name)
    {
        if (0 == oal_strcmp(st_alg_cfg.pc_name, ac_name))
        {
            break;
        }
        st_alg_cfg = g_ast_alg_cfg_map_etc[++uc_map_index];
    }

    /* ?????????????????????????? */
    if( OAL_PTR_NULL == st_alg_cfg.pc_name)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_tpc_log::invalid alg_cfg command!}\r\n");
        return OAL_FAIL;
    }

    /* ???????????????????? */
    pst_alg_tpc_log_param->en_alg_cfg = g_ast_alg_cfg_map_etc[uc_map_index].en_alg_cfg;

    /* ????????????????????????????????????:?????????????????????? */
    if (MAC_ALG_CFG_TPC_GET_FRAME_POW == pst_alg_tpc_log_param->en_alg_cfg)
    {
        /* ???????????????? */
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_tpc_log::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
            return ul_ret;
        }
        /* ???????????????????? */
        pst_alg_tpc_log_param->pc_frame_name = ac_name;
    }
    else
    {
        ul_ret = wal_hipriv_get_mac_addr_etc(pc_param, pst_alg_tpc_log_param->auc_mac_addr, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_tpc_log::wal_hipriv_get_mac_addr_etc failed!}\r\n");
            return ul_ret;
        }
        pc_param += ul_off_set;

        while ((' ' == *pc_param) || ('\0' == *pc_param))
        {
            if ('\0' == *pc_param)
            {
                en_stop_flag = OAL_TRUE;
                break;
            }
            ++ pc_param;
        }

        /* ?????????????? */
        if (OAL_TRUE != en_stop_flag)
        {
            ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
            if (OAL_SUCC != ul_ret)
            {
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_tpc_log::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
                return ul_ret;
            }

            pst_alg_tpc_log_param->uc_ac_no = (oal_uint8)oal_atoi(ac_name);
            pc_param = pc_param + ul_off_set;

            en_stop_flag = OAL_FALSE;
            while ((' ' == *pc_param) || ('\0' == *pc_param))
            {
                if ('\0' == *pc_param)
                {
                    en_stop_flag = OAL_TRUE;
                    break;
                }
                ++ pc_param;
            }

            if (OAL_TRUE != en_stop_flag)
            {
                /* ?????????????? */
                ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
                if (OAL_SUCC != ul_ret)
                {
                    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_tpc_log::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
                    return ul_ret;
                }

                /* ?????????????? */
                pst_alg_tpc_log_param->us_value = (oal_uint16)oal_atoi(ac_name);
            }
        }
    }

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_ALG_PARAM, OAL_SIZEOF(mac_ioctl_alg_tpc_log_param_stru));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_ioctl_alg_tpc_log_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;

}


oal_int32 wal_ioctl_set_mlme_ie_etc(oal_net_device_stru *pst_net_dev, oal_mlme_ie_stru *pst_mlme_ie)
{
    wal_msg_write_stru               st_write_msg;
    oal_mlme_ie_stru                 st_mlme_ie;
    oal_w2h_mlme_ie_stru            *pst_w2h_mlme_ie = OAL_PTR_NULL;
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                       ul_err_code;
    oal_int32                        l_ret = 0;

    memset_s(&st_mlme_ie, sizeof(oal_mlme_ie_stru), 0, sizeof(oal_mlme_ie_stru));

    if (pst_mlme_ie->us_ie_len > WLAN_WPS_IE_MAX_SIZE)
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_ioctl_set_mlme_ie_etc::en_mlme_type[%d], uc_ie_len[%u] beyond range !}",
                       st_mlme_ie.en_mlme_type,
                       st_mlme_ie.us_ie_len);
        return -OAL_EFAIL;
    }
    l_ret = memcpy_s(&st_mlme_ie, sizeof(oal_mlme_ie_stru), pst_mlme_ie, sizeof(oal_mlme_ie_stru));

    OAM_WARNING_LOG3(0, OAM_SF_ANY, "{wal_ioctl_set_mlme_ie_etc::en_mlme_type[%d], uc_status=%u, uc_ie_len[%u]!}",
                     st_mlme_ie.en_mlme_type,
                     st_mlme_ie.us_status,
                     st_mlme_ie.us_ie_len);

    pst_w2h_mlme_ie = (oal_w2h_mlme_ie_stru *)(st_write_msg.auc_value);
    l_ret += memcpy_s(pst_w2h_mlme_ie, (sizeof(oal_mlme_ie_stru) - WLAN_WPS_IE_MAX_SIZE),
                      &st_mlme_ie, (sizeof(oal_mlme_ie_stru) - WLAN_WPS_IE_MAX_SIZE));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_ioctl_set_mlme_ie_etc::memcpy fail!");
        return -OAL_EFAIL;
    }

    pst_w2h_mlme_ie->puc_data_ie = st_mlme_ie.auc_ie;

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_MLME, OAL_SIZEOF(oal_w2h_mlme_ie_stru));

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_w2h_mlme_ie_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if ((OAL_SUCC != l_ret)  || (OAL_PTR_NULL == pst_rsp_msg))
    {
        OAM_ERROR_LOG1(0, OAM_SF_P2P, "{wal_ioctl_set_mlme:: wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_P2P, "{wal_ioctl_set_mlme::wal_send_cfg_event_etc return err code:[%x]!}\r\n",
                        ul_err_code);
        return -OAL_EFAIL;
    }

    return OAL_SUCC;

}


#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

static oal_uint32 wal_ioctl_parse_wps_p2p_ie(oal_app_ie_stru *pst_app_ie, oal_uint8 *puc_src, oal_uint32 ul_src_len)
{
    oal_uint8                      *puc_ie             = OAL_PTR_NULL;
    oal_uint32                      ul_ie_len;
    oal_uint8                      *puc_buf_remain = OAL_PTR_NULL;
    oal_uint32                      ul_len_remain;

    if (OAL_ANY_NULL_PTR2(pst_app_ie,puc_src))
    {
        OAM_ERROR_LOG2(0, OAM_SF_CFG, "{wal_ioctl_parse_wps_p2p_ie::param is NULL, pst_app_ie=[%p], puc_src=[%p]!}\r\n",(uintptr_t)pst_app_ie, (uintptr_t)puc_src);
        return OAL_ERR_CODE_PTR_NULL;
    }

    if(0 == ul_src_len || WLAN_WPS_IE_MAX_SIZE < ul_src_len)
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_ioctl_parse_wps_p2p_ie::ul_src_len=[%d] is invailid!}\r\n",ul_src_len);
        return OAL_FAIL;
    }

    pst_app_ie->ul_ie_len    = 0;
    puc_buf_remain           = puc_src;
    ul_len_remain            = ul_src_len;

    while(ul_len_remain > MAC_IE_HDR_LEN)
    {
        /*MAC_EID_WPS,MAC_EID_P2P ID????221 */
        puc_ie = mac_find_ie_etc(MAC_EID_P2P, puc_buf_remain, (oal_int32)ul_len_remain);
        if(OAL_PTR_NULL != puc_ie)
        {
            ul_ie_len = (oal_uint8)puc_ie[1] + MAC_IE_HDR_LEN;
            if((ul_ie_len > (WLAN_WPS_IE_MAX_SIZE - pst_app_ie->ul_ie_len)) ||
               (ul_src_len < ((oal_uint16)(puc_ie - puc_src) + ul_ie_len)))
            {
                OAM_WARNING_LOG3(0, OAM_SF_CFG, "{wal_ioctl_parse_wps_p2p_ie::uc_ie_len=[%d], left buffer sieze=[%d], src_end_len=[%d],param invalid!}\r\n",
                ul_ie_len, WLAN_WPS_IE_MAX_SIZE - pst_app_ie->ul_ie_len, puc_ie - puc_src + ul_ie_len);
                return OAL_FAIL;
            }
            if (EOK != memcpy_s(&(pst_app_ie->auc_ie[pst_app_ie->ul_ie_len]), ul_ie_len, puc_ie, ul_ie_len)) {
                OAM_ERROR_LOG0(0, OAM_SF_CFG, "wal_ioctl_parse_wps_p2p_ie::memcpy fail!");
                return OAL_FAIL;
            }
            pst_app_ie->ul_ie_len  += ul_ie_len;
            puc_buf_remain          = puc_ie + ul_ie_len;
            ul_len_remain           = ul_src_len -(oal_uint32)(puc_buf_remain - puc_src);
        }
        else
        {
            break;
        }
    }

    if(pst_app_ie->ul_ie_len > 0)
    {
        return OAL_SUCC;
    }

    return OAL_FAIL;
}


oal_int32 wal_ioctl_set_wps_p2p_ie_etc(oal_net_device_stru  *pst_net_dev,
                                    oal_uint8           *puc_buf,
                                    oal_uint32           ul_len,
                                    en_app_ie_type_uint8 en_type)
{
    wal_msg_write_stru              st_write_msg;
    oal_app_ie_stru                 st_app_ie;
    oal_uint32                      ul_err_code;
    oal_int32                       l_ret              = 0;
    wal_msg_stru                   *pst_rsp_msg        = OAL_PTR_NULL;
    oal_w2h_app_ie_stru            *pst_w2h_wps_p2p_ie = OAL_PTR_NULL;
    mac_vap_stru                   *pst_mac_vap = OAL_PTR_NULL;

    if (WLAN_WPS_IE_MAX_SIZE < ul_len)
    {
        OAM_ERROR_LOG1(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie_etc:: wrong ul_len: [%u]!}\r\n",
                            ul_len);
        return -OAL_EFAIL;
    }

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie_etc::pst_mac_vap is null}");
        return -OAL_EINVAL;
    }

    memset_s(&st_app_ie, OAL_SIZEOF(oal_app_ie_stru), 0, OAL_SIZEOF(oal_app_ie_stru));
    switch (en_type) {
        case OAL_APP_BEACON_IE:
        case OAL_APP_PROBE_RSP_IE:
        case OAL_APP_ASSOC_RSP_IE:
            st_app_ie.en_app_ie_type = en_type;
            break;
        default:
            OAM_ERROR_LOG1(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie_etc:: wrong type: [%x]!}\r\n",
                            en_type);
            return -OAL_EFAIL;
    }

    if(OAL_FALSE == IS_LEGACY_VAP(pst_mac_vap))
    {
        if(OAL_SUCC != wal_ioctl_parse_wps_p2p_ie(&st_app_ie, puc_buf , ul_len))
        {
            OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie_etc::Type=[%d], parse p2p ie fail,!}\r\n", en_type);
            return -OAL_EFAIL;
        }
    }
    else
    {
        if (EOK != memcpy_s(st_app_ie.auc_ie, WLAN_WPS_IE_MAX_SIZE, puc_buf, ul_len)) {
            OAM_ERROR_LOG0(0, OAM_SF_CFG, "wal_ioctl_set_wps_p2p_ie_etc::memcpy fail!");
            return -OAL_EFAIL;
        }
        st_app_ie.ul_ie_len = ul_len;
    }

    OAM_WARNING_LOG3(0, OAM_SF_CFG, "{wal_ioctl_set_wps_p2p_ie_etc::p2p_ie_type=[%d], ul_len=[%d], st_app_ie.ul_ie_len=[%d]!}\r\n",
    en_type, ul_len, st_app_ie.ul_ie_len);

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    pst_w2h_wps_p2p_ie                 = (oal_w2h_app_ie_stru *)st_write_msg.auc_value;
    pst_w2h_wps_p2p_ie->en_app_ie_type = st_app_ie.en_app_ie_type;
    pst_w2h_wps_p2p_ie->ul_ie_len      = st_app_ie.ul_ie_len;
    pst_w2h_wps_p2p_ie->puc_data_ie    = st_app_ie.auc_ie;

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_WPS_P2P_IE, OAL_SIZEOF(oal_w2h_app_ie_stru));

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_w2h_app_ie_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if ((OAL_SUCC != l_ret)  || (OAL_PTR_NULL == pst_rsp_msg))
    {
        OAM_ERROR_LOG1(0, OAM_SF_P2P, "{wal_ioctl_set_wps_p2p_ie_etc:: wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_P2P, "{wal_ioctl_set_wps_p2p_ie_etc::wal_check_and_release_msg_resp_etc fail return err code: [%d]!}\r\n",
                        ul_err_code);
        return -OAL_EFAIL;
    }

    return OAL_SUCC;
}
#endif

OAL_STATIC oal_int32 wal_ioctl_set_dc_status(oal_net_device_stru *pst_net_dev, oal_int32 dc_param)
{
    wal_msg_write_stru           st_write_msg;
    oal_int32                    l_ret;

    if (dc_param < 0) {
        OAM_ERROR_LOG1(0, OAM_SF_CFG, "{wal_ioctl_set_dc_status::check input[%d]!}", dc_param);
        return -OAL_EFAIL;
    }

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DC_STATUS, OAL_SIZEOF(oal_uint8));
    st_write_msg.auc_value[0] = (oal_uint8)(dc_param ? OAL_TRUE : OAL_FALSE);

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(l_ret != OAL_SUCC)) {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_ioctl_set_dc_status::return err code [%d]!}", l_ret);
        return (oal_uint32)l_ret;
    }

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_set_dc_status::dc_param[%d].}", dc_param);
    return OAL_SUCC;
}


#ifdef _PRE_WLAN_FEATURE_P2P
OAL_STATIC oal_int32  wal_ioctl_set_p2p_miracast_status(oal_net_device_stru *pst_net_dev,oal_uint8 uc_param)
{
    wal_msg_write_stru           st_write_msg;
    oal_int32                    l_ret;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_P2P_MIRACAST_STATUS, OAL_SIZEOF(oal_uint8));

    st_write_msg.auc_value[0] = uc_param;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_ioctl_set_miracast_status::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_set_miracast_status::uc_param[%d].}", uc_param);
    return OAL_SUCC;
}


OAL_STATIC oal_int32  wal_ioctl_set_p2p_noa(oal_net_device_stru *pst_net_dev, mac_cfg_p2p_noa_param_stru *pst_p2p_noa_param)
{
    wal_msg_write_stru              st_write_msg;
    wal_msg_stru                   *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                      ul_err_code;
    oal_int32                       l_ret = 0;

    l_ret = memcpy_s(st_write_msg.auc_value, OAL_SIZEOF(mac_cfg_p2p_noa_param_stru),
                     pst_p2p_noa_param, OAL_SIZEOF(mac_cfg_p2p_noa_param_stru));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_P2P, "wal_ioctl_set_p2p_noa::memcpy fail!");
        return -OAL_EFAIL;
    }

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_P2P_PS_NOA, OAL_SIZEOF(mac_cfg_p2p_noa_param_stru));

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_p2p_noa_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if ((OAL_SUCC != l_ret)  || (OAL_PTR_NULL == pst_rsp_msg))
    {
        OAM_ERROR_LOG1(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_noa:: wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_noa::wal_check_and_release_msg_resp_etc fail return err code:  [%d]!}\r\n",
                           ul_err_code);
        return -OAL_EFAIL;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_int32  wal_ioctl_set_p2p_ops(oal_net_device_stru *pst_net_dev, mac_cfg_p2p_ops_param_stru *pst_p2p_ops_param)
{
    wal_msg_write_stru              st_write_msg;
    wal_msg_stru                   *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                      ul_err_code;
    oal_int32                       l_ret = 0;

    l_ret = memcpy_s(st_write_msg.auc_value, OAL_SIZEOF(mac_cfg_p2p_ops_param_stru),
                     pst_p2p_ops_param, OAL_SIZEOF(mac_cfg_p2p_ops_param_stru));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_P2P, "wal_ioctl_set_p2p_ops::memcpy fail!");
        return -OAL_EFAIL;
    }

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_P2P_PS_OPS, OAL_SIZEOF(mac_cfg_p2p_ops_param_stru));

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_p2p_ops_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if ((OAL_SUCC != l_ret)  || (OAL_PTR_NULL == pst_rsp_msg))
    {
        OAM_ERROR_LOG1(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_ops:: wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_P2P, "{wal_ioctl_set_p2p_ops::wal_check_and_release_msg_resp_etc fail return err code:[%d]!}\r\n",
                        ul_err_code);
        return -OAL_EFAIL;
    }

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_HS20

OAL_STATIC oal_int32 wal_ioctl_set_qos_map(oal_net_device_stru *pst_net_dev, hmac_cfg_qos_map_param_stru *pst_qos_map_param)
{
    wal_msg_write_stru              st_write_msg;
    wal_msg_stru                   *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                      ul_err_code;
    oal_int32                       l_ret = 0;

    l_ret = memcpy_s(st_write_msg.auc_value, OAL_SIZEOF(hmac_cfg_qos_map_param_stru),
                     pst_qos_map_param, OAL_SIZEOF(hmac_cfg_qos_map_param_stru));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_HS20, "wal_ioctl_set_qos_map::memcpy fail!");
        return -OAL_EFAIL;
    }

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_QOS_MAP, OAL_SIZEOF(hmac_cfg_qos_map_param_stru));

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(hmac_cfg_qos_map_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);
    if ((OAL_SUCC != l_ret)  || (OAL_PTR_NULL == pst_rsp_msg))
    {
        OAM_ERROR_LOG1(0, OAM_SF_HS20, "{wal_ioctl_set_qos_map:: wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_HS20, "{wal_ioctl_set_qos_map::wal_check_and_release_msg_resp_etc return err code: [%x!}\r\n",
                        ul_err_code);
        return -OAL_EFAIL;
    }
    return OAL_SUCC;
}
#endif // _PRE_WLAN_FEATURE_HS20

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

OAL_STATIC oal_int32 wal_ioctl_reduce_sar(oal_net_device_stru *pst_net_dev, oal_uint16 ul_tx_power)
{
#ifdef _PRE_WLAN_FEATURE_TPC_OPT
    oal_uint8                   uc_lvl_idx = 0;
    oal_int32                   l_ret;
    wal_msg_write_stru          st_write_msg;
    oal_uint8                   auc_sar_ctrl_params[CUS_NUM_OF_SAR_PARAMS][WLAN_RF_CHANNEL_NUMS];
    oal_uint8                  *puc_sar_ctrl_params;

    OAM_WARNING_LOG1(0, OAM_SF_TPC, "wal_ioctl_reduce_sar::supplicant set tx_power[%d] for reduce SAR purpose.\r\n", ul_tx_power);

    /***************************************************************************
        ????10XX????????????????SAR??????
        ????????????????"????WiFi??????SAR sensor??
        ??????????WiFi??????WiFi??Modem????????"????????
        ??????1001~1020????
        ????        ????          ????0        ????1    ????2??RPC??   ????3(Ant????SAR sensor????)
                              ??????????????                           Ant1 Ant3
        Head SAR    ????1001        N           CE??    receiver on     NA  NA
                    ????1002        Y           CE??    receiver on     NA  NA
                    ????1003        N           FCC??   receiver on     NA  NA
                    ????1004        Y           FCC??   receiver on     NA  NA
        -------------------------------------------------------------------------
        Body SAR    ????1005        N           CE??    receiver off    0   0
                    ????1006        N           CE??    receiver off    0   1
                    ????1007        N           CE??    receiver off    1   0
                    ????1008        N           CE??    receiver off    1   1
                    ????1009        Y           CE??    receiver off    0   0
                    ????1010        Y           CE??    receiver off    0   1
                    ????1011        Y           CE??    receiver off    1   0
                    ????1012        Y           CE??    receiver off    1   1
                    ????1013        N           FCC??   receiver off    0   0
                    ????1014        N           FCC??   receiver off    0   1
                    ????1015        N           FCC??   receiver off    1   0
                    ????1016        N           FCC??   receiver off    1   1
                    ????1017        Y           FCC??   receiver off    0   0
                    ????1018        Y           FCC??   receiver off    0   1
                    ????1019        Y           FCC??   receiver off    1   0
                    ????1020        Y           FCC??   receiver off    1   1
    ***************************************************************************/

    if ((ul_tx_power >= 1001) && (ul_tx_power <= 1020))
    {
        uc_lvl_idx = ul_tx_power - 1000;
    }
    puc_sar_ctrl_params = wal_get_reduce_sar_ctrl_params(uc_lvl_idx);
    if (OAL_PTR_NULL == puc_sar_ctrl_params)
    {
        memset_s(auc_sar_ctrl_params, OAL_SIZEOF(auc_sar_ctrl_params), 0xFF, OAL_SIZEOF(auc_sar_ctrl_params));
        puc_sar_ctrl_params = &auc_sar_ctrl_params[0][0];
    }

    /* vap????????????????supplicant???? */
    if (OAL_PTR_NULL == OAL_NET_DEV_PRIV(pst_net_dev))
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_reduce_sar::vap not created yet, ignore the cmd!");
        return -OAL_EINVAL;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_REDUCE_SAR, OAL_SIZEOF(auc_sar_ctrl_params));
    if (EOK != memcpy_s(st_write_msg.auc_value, OAL_SIZEOF(auc_sar_ctrl_params),
                        puc_sar_ctrl_params, OAL_SIZEOF(auc_sar_ctrl_params))) {
        OAM_ERROR_LOG0(0, OAM_SF_CFG, "wal_ioctl_reduce_sar::memcpy fail!");
        return -OAL_EINVAL;
    }
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                       WAL_MSG_TYPE_WRITE,
                       WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(auc_sar_ctrl_params),
                       (oal_uint8 *)&st_write_msg,
                       OAL_FALSE,
                       OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "wal_ioctl_reduce_sar::wal_send_cfg_event_etc failed, error no[%d]!\r\n", l_ret);
        return l_ret;
    }
#endif
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH

OAL_STATIC oal_int32 wal_ioctl_tas_pow_ctrl(oal_net_device_stru *pst_net_dev, oal_uint8 uc_coreindex,
                                                                     oal_bool_enum_uint8 en_needImprove)
{
#ifdef _PRE_WLAN_FEATURE_TPC_OPT
    oal_int32                   l_ret;
    wal_msg_write_stru          st_write_msg;
    mac_cfg_tas_pwr_ctrl_stru   st_tas_pow_ctrl_params;
    mac_device_stru             *pst_mac_device;

    if (OAL_FALSE == aen_tas_switch_en[uc_coreindex])
    {
        /* ??????????????TAS???????? */
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "wal_ioctl_tas_pow_ctrl::core[%d] is not supported!", uc_coreindex);
        return OAL_SUCC;
    }

    memset_s(&st_tas_pow_ctrl_params, OAL_SIZEOF(mac_cfg_tas_pwr_ctrl_stru), 0, OAL_SIZEOF(mac_cfg_tas_pwr_ctrl_stru));
    st_tas_pow_ctrl_params.en_need_improved = en_needImprove;
    st_tas_pow_ctrl_params.uc_core_idx      = uc_coreindex;

    /* vap????????????????supplicant???? */
    if (OAL_PTR_NULL == OAL_NET_DEV_PRIV(pst_net_dev))
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_tas_pow_ctrl::vap not created yet, ignore the cmd!");
        return -OAL_EINVAL;
    }

    pst_mac_device = mac_res_get_dev_etc(0);
    /* ????????VAP,???????? */
    if (1 != mac_device_calc_up_vap_num_etc(pst_mac_device))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "wal_ioctl_tas_pow_ctrl::abort for more than 1 vap");
        return OAL_SUCC;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_TAS_PWR_CTRL, OAL_SIZEOF(mac_cfg_tas_pwr_ctrl_stru));
    if (EOK != memcpy_s(st_write_msg.auc_value, OAL_SIZEOF(mac_cfg_tas_pwr_ctrl_stru),
                        &st_tas_pow_ctrl_params, OAL_SIZEOF(mac_cfg_tas_pwr_ctrl_stru))) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_ioctl_tas_pow_ctrl::memcpy fail!");
        return OAL_FAIL;
    }
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                       WAL_MSG_TYPE_WRITE,
                       WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_tas_pwr_ctrl_stru),
                       (oal_uint8 *)&st_write_msg,
                       OAL_FALSE,
                       OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "wal_ioctl_tas_pow_ctrl::wal_send_cfg_event_etc failed, error no[%d]!\r\n", l_ret);
        return l_ret;
    }
#endif
    return OAL_SUCC;
}


 
 OAL_STATIC oal_int32 wal_ioctl_tas_rssi_access(oal_net_device_stru *pst_net_dev, oal_uint8 uc_coreindex)
 {
     oal_int32                   l_ret;
     wal_msg_write_stru          st_write_msg;

     /* vap????????????????supplicant???? */
     if (OAL_PTR_NULL == OAL_NET_DEV_PRIV(pst_net_dev))
     {
         OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_tas_rssi_access::vap not created yet, ignore the cmd!");
         return -OAL_EINVAL;
     }

     if (OAL_FALSE == aen_tas_switch_en[uc_coreindex])
     {
         /* ??????????????TAS???????? */
         OAM_ERROR_LOG1(0, OAM_SF_ANY, "wal_ioctl_tas_rssi_access::core[%d] is not supported!", uc_coreindex);
         return OAL_SUCC;
     }

     /***************************************************************************
                                 ????????wal??????
     ***************************************************************************/
     WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_TAS_RSSI_ACCESS, OAL_SIZEOF(oal_uint8));
     st_write_msg.auc_value[0] = uc_coreindex;
     l_ret = wal_send_cfg_event_etc(pst_net_dev,
                        WAL_MSG_TYPE_WRITE,
                        WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                        (oal_uint8 *)&st_write_msg,
                        OAL_FALSE,
                        OAL_PTR_NULL);
     if (OAL_UNLIKELY(OAL_SUCC != l_ret))
     {
         OAM_ERROR_LOG1(0, OAM_SF_ANY, "wal_ioctl_tas_rssi_access::wal_send_cfg_event_etc failed, error no[%d]!\r\n", l_ret);
         return l_ret;
     }

     return OAL_SUCC;
 }
#endif

#endif
#endif

OAL_STATIC oal_uint32 wal_get_parameter_from_cmd(oal_int8 *pc_cmd, oal_int8 *pc_arg, OAL_CONST oal_int8 *puc_token, oal_uint32 *pul_cmd_offset, oal_uint32 ul_param_max_len)
{
    oal_int8   *pc_cmd_copy = OAL_PTR_NULL;
    oal_int8    ac_cmd_copy[WAL_IOCTL_PRIV_SUBCMD_MAX_LEN];
    oal_uint32  ul_pos = 0;
    oal_uint32  ul_arg_len;
    oal_int8   *pc_cmd_tmp = OAL_PTR_NULL;

    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR3(pc_cmd,pc_arg,pul_cmd_offset)))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "{wal_get_parameter_from_cmd::pc_cmd/pc_arg/pul_cmd_offset null ptr error %x, %x, %x, %x!}\r\n", (uintptr_t)pc_cmd, (uintptr_t)pc_arg, (uintptr_t)pul_cmd_offset);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pc_cmd_copy = pc_cmd;

    /* ???????????????????? */
    while (',' == *pc_cmd_copy)
    {
        ++pc_cmd_copy;
    }
    /* ?????????????????? */
    while ((',' != *pc_cmd_copy) && ('\0' != *pc_cmd_copy))
    {
        ac_cmd_copy[ul_pos] = *pc_cmd_copy;
        ++ul_pos;
        ++pc_cmd_copy;

        if (OAL_UNLIKELY(ul_pos >= ul_param_max_len))
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_get_parameter_from_cmd::ul_pos >= WAL_HIPRIV_CMD_NAME_MAX_LEN, ul_pos %d!}\r\n", ul_pos);
            return OAL_ERR_CODE_ARRAY_OVERFLOW;
        }
    }
    ac_cmd_copy[ul_pos]  = '\0';
    /* ???????????????????????? */
    if (0 == ul_pos)
    {
        OAM_INFO_LOG0(0, OAM_SF_ANY, "{wal_get_parameter_from_cmd::return param pc_arg is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }
    *pul_cmd_offset = (oal_uint32)(pc_cmd_copy - pc_cmd);

    /* ????????????????????????????????????*/
    pc_cmd_tmp = &ac_cmd_copy[0];
    if (0 != oal_memcmp(pc_cmd_tmp, puc_token, OAL_STRLEN(puc_token)))
    {
        return OAL_FAIL;
    }
    else
    {
        /* ??????????????????????????*/
        ul_arg_len = OAL_STRLEN(ac_cmd_copy) - OAL_STRLEN(puc_token);
        if (EOK != memcpy_s(pc_arg, ul_param_max_len, ac_cmd_copy + OAL_STRLEN(puc_token), ul_arg_len)) {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "wal_get_parameter_from_cmd::memcpy fail!");
        }
        pc_arg[ul_arg_len]  = '\0';
    }
    return OAL_SUCC;
}


OAL_STATIC oal_int32 wal_set_ap_max_user(oal_net_device_stru *pst_net_dev, oal_uint32 ul_ap_max_user)
{
    wal_msg_write_stru          st_write_msg;
    wal_msg_stru               *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                  ul_err_code;
    oal_int32                   l_ret;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_set_ap_max_user:: ap_max_user is : %u.}\r\n", ul_ap_max_user);

    if(ul_ap_max_user == 0)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_set_ap_max_user::invalid ap max user(%u),ignore this set.}\r\n", ul_ap_max_user);
        return OAL_SUCC;
    }

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_MAX_USER, OAL_SIZEOF(ul_ap_max_user));
     *((oal_uint32 *)st_write_msg.auc_value) = ul_ap_max_user;
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                                WAL_MSG_TYPE_WRITE,
                                WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(ul_ap_max_user),
                                (oal_uint8 *)&st_write_msg,
                                OAL_TRUE,
                                &pst_rsp_msg);

    if ((OAL_SUCC != l_ret) || (OAL_PTR_NULL == pst_rsp_msg))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_set_ap_max_user:: wal_send_cfg_event_etc return err code %d!}\r\n", l_ret);

        return l_ret;
    }

    /* ???????????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_set_ap_max_user::wal_check_and_release_msg_resp_etc return err code: [%d]!}\r\n",
                           ul_err_code);
        return -OAL_EFAIL;
    }
    /* ????????????????????????????????????????0 **/
    //g_st_ap_config_info.ul_ap_max_user = 0;

    return l_ret;
}


OAL_STATIC oal_int32 wal_config_mac_filter(oal_net_device_stru *pst_net_dev, oal_int8 *pc_command)
{
    oal_int8                    ac_parsed_command[WAL_IOCTL_PRIV_SUBCMD_MAX_LEN];
    oal_int8                   *pc_parse_command = OAL_PTR_NULL;
    oal_uint32                  ul_mac_mode;
    oal_uint32                  ul_mac_cnt = 0;
    oal_uint32                  ul_i;
#if (_PRE_WLAN_FEATURE_BLACKLIST_LEVEL != _PRE_WLAN_FEATURE_BLACKLIST_NONE)
    wal_msg_write_stru          st_write_msg;
    oal_uint16                  us_len;
    wal_msg_stru               *pst_rsp_msg = OAL_PTR_NULL;
   oal_uint32                   ul_err_code;
    oal_int32                   l_ret = 0;
    mac_blacklist_stru         *pst_blklst = OAL_PTR_NULL;
#endif
    oal_uint32                  ul_ret = 0;
    oal_uint32                  ul_off_set;


    if (OAL_PTR_NULL == pc_command)
    {
        return -OAL_EINVAL;
    }
    pc_parse_command = pc_command;

    /* ????MAC_MODE*/
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parsed_command, "MAC_MODE=", &ul_off_set, WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_config_mac_filter::wal_get_parameter_from_cmd return err_code %u.}\r\n", ul_ret);
        return -OAL_EINVAL;
    }
    /* ???????????????? 0,1,2*/
    ul_mac_mode = (oal_uint32)oal_atoi(ac_parsed_command);
    if(ul_mac_mode > 2)
    {
        OAM_WARNING_LOG4(0, OAM_SF_ANY, "{wal_config_mac_filter::invalid MAC_MODE[%c%c%c%c]!}\r\n",
                    (oal_uint8)ac_parsed_command[0],
                    (oal_uint8)ac_parsed_command[1],
                    (oal_uint8)ac_parsed_command[2],
                    (oal_uint8)ac_parsed_command[3]);
        return -OAL_EINVAL;
    }

    /* ????????????*/
#if (_PRE_WLAN_FEATURE_BLACKLIST_LEVEL != _PRE_WLAN_FEATURE_BLACKLIST_NONE)
    ul_ret = wal_hipriv_send_cfg_uint32_data_etc(pst_net_dev,ac_parsed_command, WLAN_CFGID_BLACKLIST_MODE);
    if(OAL_SUCC != ul_ret)
    {
        return (oal_int32)ul_ret;
    }
#endif
    /* ????MAC_CNT*/
    pc_parse_command += ul_off_set;
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parsed_command, "MAC_CNT=", &ul_off_set, WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_config_mac_filter::wal_get_parameter_from_cmd return err_code [%u]!}\r\n", ul_ret);
        return -OAL_EINVAL;
    }
    ul_mac_cnt = (oal_uint32)oal_atoi(ac_parsed_command);

    for (ul_i = 0; ul_i < ul_mac_cnt; ul_i++)
    {
        pc_parse_command += ul_off_set;
        ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parsed_command, "MAC=", &ul_off_set, WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
        if(OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_config_mac_filter::wal_get_parameter_from_cmd return err_code [%u]!}\r\n", ul_ret);
            return -OAL_EINVAL;
        }
        /* 5.1  ????????????????MAC????*/
        if(WLAN_MAC_ADDR_LEN * 2 != OAL_STRLEN(ac_parsed_command))
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_config_mac_filter::invalid MAC format}\r\n");
            return -OAL_EINVAL;
        }
        /*6. ????????????*/
#if (_PRE_WLAN_FEATURE_BLACKLIST_LEVEL != _PRE_WLAN_FEATURE_BLACKLIST_NONE)
        /***************************************************************************
                             ????????wal??????
        ***************************************************************************/
        memset_s((oal_uint8*)&st_write_msg, OAL_SIZEOF(st_write_msg), 0, OAL_SIZEOF(st_write_msg));
        pst_blklst = (mac_blacklist_stru*)(st_write_msg.auc_value);
        oal_strtoaddr(ac_parsed_command, OAL_SIZEOF(ac_parsed_command), pst_blklst->auc_mac_addr, WLAN_MAC_ADDR_LEN); /* ?????? ac_name ?????????? mac_add[6] */

        us_len = OAL_SIZEOF(mac_blacklist_stru);

        if(ul_i == (ul_mac_cnt - 1))
        {
            /* ????????mac?????????????????????????????????????????????????? */
            WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_ADD_BLACK_LIST, us_len);
        }
        else
        {
            WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_ADD_BLACK_LIST_ONLY, us_len);
        }

        /* 6.1  ????????*/
        l_ret = wal_send_cfg_event_etc(pst_net_dev,
                                    WAL_MSG_TYPE_WRITE,
                                    WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                                    (oal_uint8 *)&st_write_msg,
                                    OAL_TRUE,
                                    &pst_rsp_msg);

        if ((OAL_SUCC != l_ret)  || (OAL_PTR_NULL == pst_rsp_msg))
        {
            OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_config_mac_filter:: wal_send_cfg_event_etc return err code %d!}\r\n", l_ret);
            return l_ret;
        }

        /* 6.2  ???????????????? */
        ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
        if(OAL_SUCC != ul_err_code)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_config_mac_filter::wal_check_and_release_msg_resp_etc return err code:[%x]!}\r\n",
                               ul_err_code);
            return -OAL_EFAIL;
        }
#endif
    }

    /* ????????????mac?????????????????????????? */


    return OAL_SUCC;
}



OAL_STATIC oal_int32 wal_kick_sta(oal_net_device_stru *pst_net_dev, oal_uint8 *auc_mac_addr, oal_uint8 uc_mac_addr_len, oal_uint16 us_reason_code)
{
#if (_PRE_WLAN_FEATURE_BLACKLIST_LEVEL != _PRE_WLAN_FEATURE_BLACKLIST_NONE)
    wal_msg_write_stru          st_write_msg;
    wal_msg_stru               *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                  ul_err_code;
    mac_cfg_kick_user_param_stru   *pst_kick_user_param = OAL_PTR_NULL;
    oal_int32                       l_ret = 0;
#endif


    if ((NULL == auc_mac_addr) || (uc_mac_addr_len != WLAN_MAC_ADDR_LEN))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_kick_sta::check para! .\n");
        return -OAL_EFAIL;
    }

#if (_PRE_WLAN_FEATURE_BLACKLIST_LEVEL != _PRE_WLAN_FEATURE_BLACKLIST_NONE)

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_KICK_USER, OAL_SIZEOF(mac_cfg_kick_user_param_stru));

    pst_kick_user_param = (mac_cfg_kick_user_param_stru *)(st_write_msg.auc_value);
    oal_set_mac_addr(pst_kick_user_param->auc_mac_addr, auc_mac_addr);

    pst_kick_user_param->us_reason_code = us_reason_code;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_kick_user_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if ((OAL_SUCC != l_ret)  || (OAL_PTR_NULL == pst_rsp_msg))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_kick_sta:: wal_send_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* 4.4  ???????????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_kick_sta::wal_check_and_release_msg_resp_etc return err code: [%x]!}\r\n",
                               ul_err_code);
        return -OAL_EFAIL;
    }
#endif

    return OAL_SUCC;
}


OAL_STATIC int wal_ioctl_set_ap_config(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info, oal_iwreq_data_union *pst_wrqu, char *pc_extra)
{
    oal_int8                   *pc_command        = OAL_PTR_NULL;
    oal_int8                   *pc_parse_command  = OAL_PTR_NULL;
    oal_int32                   l_ret             = OAL_SUCC;
    oal_uint32                  ul_ret            = OAL_SUCC;
    oal_int8                    ac_parse_command[WAL_IOCTL_PRIV_SUBCMD_MAX_LEN];
    oal_uint32                  ul_off_set;

    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR2(pst_net_dev,pst_wrqu)))
    {
        OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_ioctl_set_ap_config:: param is OAL_PTR_NULL , pst_net_dev = %p, pst_wrqu = %p}",(uintptr_t)pst_net_dev,(uintptr_t)pst_wrqu);
        return -OAL_EFAIL;
    }

    /* 1. ????????????netd ???????????????? */
    pc_command = oal_memalloc((oal_int32)(pst_wrqu->data.length + 1));
    if (OAL_PTR_NULL == pc_command)
    {
        return -OAL_ENOMEM;
    }
    /* 2. ????netd ?????????????? */
    memset_s(pc_command, (oal_uint32)(pst_wrqu->data.length + 1), 0, (oal_uint32)(pst_wrqu->data.length + 1));
    ul_ret = oal_copy_from_user(pc_command, pst_wrqu->data.pointer , (oal_uint32)(pst_wrqu->data.length));
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_ioctl_set_ap_config::oal_copy_from_user: -OAL_EFAIL }\r\n");
        oal_free(pc_command);
        return -OAL_EFAIL;
    }
    pc_command[pst_wrqu->data.length] = '\0';

    OAL_IO_PRINT("wal_ioctl_set_ap_config,data len:%u \n", (oal_uint32)pst_wrqu->data.length);

    pc_parse_command = pc_command;
    /* 3.   ???????? */
    /* 3.1  ????ASCII_CMD*/
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parse_command, "ASCII_CMD=", &ul_off_set, WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_set_ap_config::wal_get_parameter_from_cmd ASCII_CMD return err_code [%u]!}\r\n", ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }

    if ((0 != oal_strcmp("AP_CFG", ac_parse_command)))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_ioctl_set_ap_config::sub_command != 'AP_CFG' }");
        OAL_IO_PRINT("{wal_ioctl_set_ap_config::sub_command %6s...!= 'AP_CFG' }",ac_parse_command);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }

    /* 3.2  ????CHANNEL????????????netd??????channel????*/
    pc_parse_command += ul_off_set;
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parse_command, "CHANNEL=", &ul_off_set, WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_set_ap_config::wal_get_parameter_from_cmd CHANNEL return err_code [%u]!}\r\n", ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }

    /* 3.3  ????MAX_SCB*/
    pc_parse_command += ul_off_set;
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parse_command, "MAX_SCB=", &ul_off_set, WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_set_ap_config::wal_get_parameter_from_cmd MAX_SCB return err_code [%u]!}\r\n", ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }

    g_st_ap_config_info.ul_ap_max_user = (oal_uint32)oal_atoi(ac_parse_command);

    if (OAL_PTR_NULL != OAL_NET_DEV_PRIV(pst_net_dev))
    {
        l_ret = wal_set_ap_max_user(pst_net_dev, (oal_uint32)oal_atoi(ac_parse_command));
    }

    /* 5. ????????????*/
    oal_free(pc_command);
    return l_ret;
}


OAL_STATIC int  wal_ioctl_get_assoc_list(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info, oal_iwreq_data_union *pst_wrqu, char *pc_extra)
{
    oal_int32                       l_ret;
    oal_int32                       l_memcpy_ret = EOK;
    wal_msg_query_stru              st_query_msg;
    wal_msg_stru                   *pst_rsp_msg = OAL_PTR_NULL;
    wal_msg_rsp_stru               *pst_query_rsp_msg = OAL_PTR_NULL;
    oal_int8                       *pc_sta_list = OAL_PTR_NULL;
    oal_netbuf_stru                *pst_response_netbuf = OAL_PTR_NULL;

    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR4(pst_net_dev,pst_info,pst_wrqu,pc_extra)))
    {
        OAM_WARNING_LOG4(0, OAM_SF_ANY, "{wal_ioctl_get_assoc_list:: param is OAL_PTR_NULL ,psy_dev=%p, pst_info = %p , pst_wrqu = %p , pc_extra = %p}\n",
        (uintptr_t)pst_net_dev, (uintptr_t)pst_info, (uintptr_t)pst_wrqu, (uintptr_t)pc_extra);
        return -OAL_EFAIL;
    }

    /* ??????????????????????????????????????????????netdev???????????????? */
    if (OAL_UNLIKELY(OAL_PTR_NULL == OAL_NET_DEV_PRIV(pst_net_dev)))
    {
        return -OAL_EFAIL;
    }

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    st_query_msg.en_wid = WLAN_CFGID_GET_STA_LIST;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_QUERY,
                               WAL_MSG_WID_LENGTH,
                               (oal_uint8 *)&st_query_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if ((OAL_SUCC != l_ret)  || (OAL_PTR_NULL == pst_rsp_msg))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_get_assoc_list:: wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????? */
    pst_query_rsp_msg = (wal_msg_rsp_stru *)(pst_rsp_msg->auc_msg_data);
    /* ???????? */
    if(pst_query_rsp_msg->us_len >= OAL_SIZEOF(oal_netbuf_stru *))
    {
        /* ????hmac??????netbuf???? */
        l_memcpy_ret = memcpy_s(&pst_response_netbuf, OAL_SIZEOF(oal_netbuf_stru *),
                                pst_query_rsp_msg->auc_value, OAL_SIZEOF(oal_netbuf_stru *));
        if(NULL != pst_response_netbuf)
        {
            /* ????ap??????sta???????? */
            pc_sta_list = (oal_int8*)OAL_NETBUF_DATA(pst_response_netbuf);
            pst_wrqu->data.length = (oal_uint16)(OAL_NETBUF_LEN(pst_response_netbuf) + 1);
            l_memcpy_ret += memcpy_s(pc_extra, pst_wrqu->data.length, pc_sta_list, pst_wrqu->data.length);
            pc_extra[OAL_NETBUF_LEN(pst_response_netbuf)] = '\0';
            oal_netbuf_free(pst_response_netbuf);
        }
        else
        {
            l_ret = -OAL_ENOMEM;
        }
    }
    else
    {
        oal_print_hex_dump((oal_uint8 * )pst_rsp_msg->auc_msg_data, pst_query_rsp_msg->us_len, 32, "query msg: ");
        l_ret = -OAL_EINVAL;
    }
    if (l_memcpy_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_ioctl_get_assoc_list::memcpy fail!");
        oal_free(pst_rsp_msg);
        return -OAL_EINVAL;
    }

    if(OAL_SUCC != l_ret)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_ioctl_get_assoc_list::process failed,ret=%d}", l_ret);
    }

    oal_free(pst_rsp_msg);
    return l_ret;

}


OAL_STATIC int  wal_ioctl_set_mac_filters(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info, oal_iwreq_data_union *pst_wrqu, char *pc_extra)
{
    mac_vap_stru               *pst_vap           = OAL_PTR_NULL;
    oal_int8                   *pc_command        = OAL_PTR_NULL;
    oal_int32                   l_ret             = 0;
    oal_uint32                  ul_ret            = 0;
    oal_int8                    ac_parsed_command[WAL_IOCTL_PRIV_SUBCMD_MAX_LEN];
    oal_int8                   *pc_parse_command = OAL_PTR_NULL;
    oal_uint32                  ul_mac_mode;
    oal_uint32                  ul_mac_cnt = 0;
    oal_uint8                   auc_mac_addr[WLAN_MAC_ADDR_LEN];
    oal_uint32                  ul_off_set;

    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR4(pst_net_dev,pst_info,pst_wrqu,pc_extra)))
    {
        OAM_WARNING_LOG4(0, OAM_SF_ANY, "{wal_ioctl_set_mac_filters:: param is OAL_PTR_NULL ,pst_net_dev=%p, pst_info = %p , pst_wrqu = %p , pc_extra = %p}\n",
        (uintptr_t)pst_net_dev, (uintptr_t)pst_info, (uintptr_t)pst_wrqu, (uintptr_t)pc_extra);
        return -OAL_EFAIL;
    }

    /* 1. ????????????netd ???????????????? */
    pc_command = oal_memalloc((oal_int32)(pst_wrqu->data.length + 1));
    if (OAL_PTR_NULL == pc_command)
    {
        return -OAL_ENOMEM;
    }

    /* 2. ????netd ?????????????? */
    memset_s(pc_command, (oal_uint32)(pst_wrqu->data.length + 1), 0, (oal_uint32)(pst_wrqu->data.length + 1));
    ul_ret = oal_copy_from_user(pc_command, pst_wrqu->data.pointer , (oal_uint32)(pst_wrqu->data.length));
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_ioctl_set_mac_filters::oal_copy_from_user: -OAL_EFAIL }\r\n");
        oal_free(pc_command);
        return -OAL_EFAIL;
    }
    pc_command[pst_wrqu->data.length] = '\0';

    OAL_IO_PRINT("wal_ioctl_set_mac_filters,data len:%d \n", pst_wrqu->data.length);

    pc_parse_command = pc_command;

    memset_s(g_st_ap_config_info.ac_ap_mac_filter_mode,
             OAL_SIZEOF(g_st_ap_config_info.ac_ap_mac_filter_mode), 0,
             OAL_SIZEOF(g_st_ap_config_info.ac_ap_mac_filter_mode));
    strncpy_s(g_st_ap_config_info.ac_ap_mac_filter_mode, OAL_SIZEOF(g_st_ap_config_info.ac_ap_mac_filter_mode) ,
              pc_command, OAL_SIZEOF(g_st_ap_config_info.ac_ap_mac_filter_mode) - 1);

    pst_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_ioctl_set_mac_filters::netdevice vap is null,just save it.}\r\n");
        oal_free(pc_command);
        return OAL_SUCC;
    }

    /* 3  ????MAC_MODE*/
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parsed_command, "MAC_MODE=", &ul_off_set, WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_set_mac_filters::wal_get_parameter_from_cmd return err_code [%u]!}\r\n", ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }
    /* 3.1 ???????????????? 0,1,2*/
    ul_mac_mode = (oal_uint32)oal_atoi(ac_parsed_command);
    if(ul_mac_mode > 2)
    {
        OAM_WARNING_LOG4(0, OAM_SF_ANY, "{wal_ioctl_set_mac_filters::invalid MAC_MODE[%c%c%c%c]!}",
                (oal_uint8)ac_parsed_command[0],
                (oal_uint8)ac_parsed_command[1],
                (oal_uint8)ac_parsed_command[2],
                (oal_uint8)ac_parsed_command[3]);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }

    /* 5 ????MAC_CNT*/
    pc_parse_command += ul_off_set;
    ul_ret = wal_get_parameter_from_cmd(pc_parse_command, ac_parsed_command, "MAC_CNT=", &ul_off_set, WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_set_mac_filters::wal_get_parameter_from_cmd return err_code [%u]!}\r\n", ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }
    ul_mac_cnt = (oal_uint32)oal_atoi(ac_parsed_command);

    wal_config_mac_filter(pst_net_dev, pc_command);

    /* ????????????????????????????MAC????????????????????????????????????????????????????????STA */
    if((0 == ul_mac_cnt) && (2 == ul_mac_mode))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_ioctl_set_mac_filters::delete all user!}");

        memset_s(auc_mac_addr, sizeof(auc_mac_addr), 0xff, OAL_ETH_ALEN);
        l_ret = wal_kick_sta(pst_net_dev, auc_mac_addr, sizeof(auc_mac_addr), MAC_AUTH_NOT_VALID);
    }

    oal_free(pc_command);
    return l_ret;
}


OAL_STATIC int  wal_ioctl_set_ap_sta_disassoc(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info, oal_iwreq_data_union *pst_wrqu, char *pc_extra)
{
    oal_int8                       *pc_command        = OAL_PTR_NULL;
    oal_int32                       l_ret             = 0;
    oal_uint32                      ul_ret            = 0;
    oal_int8                        ac_parsed_command[WAL_IOCTL_PRIV_SUBCMD_MAX_LEN] = {0};
    oal_uint8                       auc_mac_addr[WLAN_MAC_ADDR_LEN] = {0};
    oal_uint32                      ul_off_set;

    if (OAL_UNLIKELY(OAL_ANY_NULL_PTR2(pst_net_dev,pst_wrqu)))
    {
        OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_ioctl_set_ap_sta_disassoc:: param is OAL_PTR_NULL , pst_net_dev = %p, pst_wrqu = %p}",
        (uintptr_t)pst_net_dev,(uintptr_t)pst_wrqu);
        return -OAL_EFAIL;
    }

    /* 1. ????????????netd ???????????????? */
    pc_command = oal_memalloc((oal_int32)(pst_wrqu->data.length + 1));
    if (OAL_PTR_NULL == pc_command)
    {
        return -OAL_ENOMEM;
    }

    /* 2. ????netd ?????????????? */
    memset_s(pc_command, (oal_uint32)(pst_wrqu->data.length + 1), 0, (oal_uint32)(pst_wrqu->data.length + 1));
    ul_ret = oal_copy_from_user(pc_command, pst_wrqu->data.pointer , (oal_uint32)(pst_wrqu->data.length));
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_ioctl_set_ap_sta_disassoc::oal_copy_from_user: -OAL_EFAIL }\r\n");
        oal_free(pc_command);
        return -OAL_EFAIL;
    }
    pc_command[pst_wrqu->data.length] = '\0';

    /* 3. ????????????MAC */
    ul_ret = wal_get_parameter_from_cmd(pc_command, ac_parsed_command, "MAC=", &ul_off_set, WAL_IOCTL_PRIV_SUBCMD_MAX_LEN);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_set_ap_sta_disassoc::wal_get_parameter_from_cmd MAC return err_code [%u]!}\r\n", ul_ret);
        oal_free(pc_command);
        return -OAL_EINVAL;
    }
    /* 3.1  ????????????????MAC????*/
    if(WLAN_MAC_ADDR_LEN * 2 != OAL_STRLEN(ac_parsed_command))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_ioctl_set_ap_sta_disassoc::invalid MAC format}\r\n");
        oal_free(pc_command);
        return -OAL_EINVAL;
    }
    oal_strtoaddr(ac_parsed_command, OAL_SIZEOF(ac_parsed_command), auc_mac_addr, WLAN_MAC_ADDR_LEN); /* ?????? ac_name ?????????? mac_add[6] */

    OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_ioctl_set_ap_sta_disassoc::Geting CMD from APP to DISASSOC!!}");
    l_ret = wal_kick_sta(pst_net_dev, auc_mac_addr, sizeof(auc_mac_addr), MAC_AUTH_NOT_VALID);

    /* 5. ????????????*/
    oal_free(pc_command);
    return l_ret;

}

#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_DFT_EVENT

OAL_STATIC oal_void  wal_event_report_to_sdt(wal_msg_type_enum_uint8   en_msg_type,
                                       oal_uint8                *puc_param,
                                       wal_msg_stru             *pst_cfg_msg)
{
    oam_event_type_enum_uint16   en_event_type = OAM_EVENT_TYPE_BUTT;
    oal_uint8                    auc_event[50] = {0};
    oal_int32                    l_ret;

    if (WAL_MSG_TYPE_QUERY == en_msg_type)
    {
        en_event_type = OAM_EVENT_WID_QUERY;
    }
    else if (WAL_MSG_TYPE_WRITE == en_msg_type)
    {
        en_event_type = OAM_EVENT_WID_WRITE;
    }

    /* ????WID,??????????????????WID */
    l_ret = memcpy_s((oal_void *)auc_event, OAL_SIZEOF(auc_event),
                     (const oal_void *)puc_param, OAL_SIZEOF(oal_uint16));

    /* ?????????? */
    l_ret += memcpy_s((oal_void *)&auc_event[2], OAL_SIZEOF(auc_event) - 2,
                      (const oal_void *)&(pst_cfg_msg->st_msg_hdr), OAL_SIZEOF(wal_msg_hdr_stru));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_event_report_to_sdt::memcpy fail!");
        return;
    }

    WAL_EVENT_WID(BROADCAST_MACADDR, 0, en_event_type, auc_event,
                    OAL_SIZEOF(oal_uint16) + OAL_SIZEOF(wal_msg_hdr_stru));
}
#endif
#endif


oal_uint32  wal_hipriv_get_mac_addr_etc(oal_int8 *pc_param, oal_uint8 auc_mac_addr[], oal_uint32 *pul_total_offset)
{
    oal_uint32                      ul_off_set      = 0;
    oal_uint32                      ul_ret          = OAL_SUCC;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];

    /* ????mac???? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_get_mac_addr_etc::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }
    oal_strtoaddr(ac_name, OAL_SIZEOF(ac_name), auc_mac_addr, WLAN_MAC_ADDR_LEN);

    *pul_total_offset = ul_off_set;

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP

OAL_STATIC oal_uint32  wal_hipriv_set_edca_opt_switch_sta(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru  st_write_msg;
    oal_uint8        uc_flag         = 0;
    oal_uint8       *puc_value       = 0;
    oal_uint32       ul_ret          = OAL_SUCC;
    oal_uint32       ul_off_set      = 0;
    oal_int32        l_ret           = OAL_SUCC;
    mac_vap_stru    *pst_mac_vap     = OAL_PTR_NULL;
    oal_int8         ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];

    // sh hipriv.sh "vap0 set_edca_switch_sta 1/0"

    /* ????mac_vap */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (WLAN_VAP_MODE_BSS_STA != pst_mac_vap->en_vap_mode)
    {
       OAM_WARNING_LOG0(0, OAM_SF_EDCA, "{wal_hipriv_set_edca_opt_switch_sta:: only STA_MODE support}");
       return OAL_FAIL;
    }

    /* ???????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_EDCA, "{wal_hipriv_set_edca_opt_switch_sta::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    uc_flag = (oal_uint8)oal_atoi(ac_name);

    /* ???????????? */
    if (uc_flag > 1)
    {
        OAM_WARNING_LOG0(0, OAM_SF_EDCA, "wal_hipriv_set_edca_opt_switch_sta, invalid config, should be 0 or 1");
        return OAL_SUCC;
    }

    /* ???????????? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_EDCA_OPT_SWITCH_STA, OAL_SIZEOF(oal_uint8));
    puc_value = (oal_uint8 *)(st_write_msg.auc_value);
    *puc_value = uc_flag;

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_EDCA, "{wal_hipriv_set_edca_opt_switch_sta:: return err_code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif


oal_uint32  wal_hipriv_send_cfg_uint32_data_etc(oal_net_device_stru *pst_net_dev,
    oal_int8 *pc_param, wlan_cfgid_enum_uint16 cfgid)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;
    oal_uint16                      us_len;
    oal_uint32                      ul_ret = OAL_SUCC;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN] = {0};
    oal_uint32                      ul_off_set = 0;
    oal_uint32                      set_value = 0;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    memset_s(&st_write_msg, OAL_SIZEOF(wal_msg_write_stru), 0, OAL_SIZEOF(wal_msg_write_stru));
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_send_cfg_uint32_data_etc:wal_get_cmd_one_arg_etc fail!}\r\n");
        return ul_ret;
    }

    pc_param += ul_off_set;
    set_value = (oal_uint32)oal_atoi((const oal_int8 *)ac_name);

    us_len = 4; /* OAL_SIZEOF(oal_uint32) */
    *(oal_uint32 *)(st_write_msg.auc_value) = set_value;

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, cfgid, us_len);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                              WAL_MSG_TYPE_WRITE,
                              WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                              (oal_uint8 *)&st_write_msg,
                              OAL_FALSE,
                              OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_send_cfg_uint32_data_etc:wal_send_cfg_event_etc return [%d].}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_bgscan_enable(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    oal_uint32                           ul_off_set;
    oal_int8                             ac_stop[2];
    oal_uint32                           ul_ret;
    oal_int32                            l_ret;
    wal_msg_write_stru                   st_write_msg;
    oal_uint8                 *pen_bgscan_enable_flag = OAL_PTR_NULL;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_stop, OAL_SIZEOF(ac_stop), &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG0(0, OAM_SF_SCAN, "wal_hipriv_bgscan_enable: get first arg fail.");
        return OAL_FAIL;
    }

    /***************************************************************************
                            ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFIGD_BGSCAN_ENABLE, OAL_SIZEOF(oal_bool_enum_uint8));

    /* ???????????????? */
    pen_bgscan_enable_flag = (oal_uint8 *)(st_write_msg.auc_value);
   *pen_bgscan_enable_flag = (oal_uint8)oal_atoi(ac_stop);

    OAM_WARNING_LOG1(0, OAM_SF_SCAN, "wal_hipriv_bgscan_enable:: bgscan_enable_flag=%d.", *pen_bgscan_enable_flag);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                       WAL_MSG_TYPE_WRITE,
                       WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_bool_enum_uint8),
                       (oal_uint8 *)&st_write_msg,
                       OAL_FALSE,
                       OAL_PTR_NULL);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_bgscan_enable::wal_send_cfg_event_etc fail.return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_DHCP_REQ_DISABLE

OAL_STATIC oal_uint32  wal_hipriv_dhcp_req_disable_switch(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    oal_uint32                           ul_off_set;
    oal_int8                             ac_stop[2];
    oal_uint32                           ul_ret;
    oal_int32                            l_ret;
    wal_msg_write_stru                   st_write_msg;
    oal_bool_enum_uint8                 *pen_dhcp_req_disable_flag;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_stop, OAL_SIZEOF(ac_stop), &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "wal_hipriv_dhcp_req_disable_switch: get first arg fail.");
        return OAL_FAIL;
    }

    /***************************************************************************
                            ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DHCP_REQ_DISABLE_SWITCH, OAL_SIZEOF(oal_bool_enum_uint8));

    /* ???????????????? */
    pen_dhcp_req_disable_flag = (oal_bool_enum_uint8 *)(st_write_msg.auc_value);
   *pen_dhcp_req_disable_flag = (oal_bool_enum_uint8)oal_atoi(ac_stop);

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "wal_hipriv_dhcp_req_disable_switch:: pen_dhcp_req_disable_flag= %d.", *pen_dhcp_req_disable_flag);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                       WAL_MSG_TYPE_WRITE,
                       WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_bool_enum_uint8),
                       (oal_uint8 *)&st_write_msg,
                       OAL_FALSE,
                       OAL_PTR_NULL);
    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_dhcp_req_disable_switch::wal_send_cfg_event_etc fail.return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif
#ifdef _PRE_WLAN_FEATURE_STA_PM

OAL_STATIC oal_uint32  wal_ioctl_set_sta_ps_mode_etc(oal_net_device_stru *pst_cfg_net_dev, oal_int8 uc_ps_mode)
{
    wal_msg_write_stru               st_write_msg;
    oal_int32                        l_ret;
    mac_cfg_ps_mode_param_stru       *pst_ps_mode_param;

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_PS_MODE, OAL_SIZEOF(mac_cfg_ps_mode_param_stru));

    /* ???????????????? */
    pst_ps_mode_param = (mac_cfg_ps_mode_param_stru *)(st_write_msg.auc_value);
    pst_ps_mode_param->uc_vap_ps_mode   = uc_ps_mode;

    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_ps_mode_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_PWR, "{wal_hipriv_sta_ps_enable::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_sta_ps_mode(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param)
{
    oal_uint32                       ul_off_set;
    oal_int8                         ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                       ul_ret;
    oal_uint8                        uc_vap_ps_mode;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_sta_ps_enable::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    uc_vap_ps_mode = (oal_uint8)oal_atoi(ac_name);
    pc_param = pc_param + ul_off_set;

    return wal_ioctl_set_sta_ps_mode_etc(pst_cfg_net_dev, uc_vap_ps_mode);
}

#ifdef _PRE_PSM_DEBUG_MODE

OAL_STATIC oal_uint32  wal_hipriv_sta_ps_info(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_int8                         ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    oal_uint8                        uc_psm_info_enable;
    oal_uint8                        uc_psm_debug_mode;
    mac_cfg_ps_info_stru            *pst_ps_info;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_sta_ps_info::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    uc_psm_info_enable = (oal_uint8)oal_atoi(ac_name);
    pc_param = pc_param + ul_off_set;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_sta_ps_info::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    uc_psm_debug_mode = (oal_uint8)oal_atoi(ac_name);
    pc_param = pc_param + ul_off_set;

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SHOW_PS_INFO, OAL_SIZEOF(mac_cfg_ps_info_stru));

    /* ???????????????? */
    pst_ps_info = (mac_cfg_ps_info_stru *)(st_write_msg.auc_value);
    pst_ps_info->uc_psm_info_enable   = uc_psm_info_enable;
    pst_ps_info->uc_psm_debug_mode    = uc_psm_debug_mode;

    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_ps_info_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_PWR, "{wal_hipriv_sta_ps_info::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}

#endif

OAL_STATIC oal_uint32 wal_ioctl_set_fast_sleep_para_etc(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_int32                        l_ret;

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_FASTSLEEP_PARA, 4*OAL_SIZEOF(oal_uint8));

    /* ???????????????? */
    if (EOK != memcpy_s(st_write_msg.auc_value, 4*OAL_SIZEOF(oal_uint8), pc_param, 4*OAL_SIZEOF(oal_uint8))) {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "wal_ioctl_set_fast_sleep_para_etc::memcpy fail!");
        return OAL_FAIL;
    }

    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + 4*OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_PWR, "{wal_ioctl_set_fast_sleep_para_etc::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}

OAL_STATIC oal_uint32  wal_hipriv_set_fasts_sleep_para(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param)
{
    oal_uint32 ul_para_cnt;
    oal_uint8   auc_para_val[4];
    oal_uint8   auc_para_str_tmp[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32  pul_cmd_offset;
    oal_uint32 ul_ret;

    /* ??????????4??????<min listen????><max listen????><??????????????><??????????????> */
    for (ul_para_cnt = 0; ul_para_cnt < 4; ul_para_cnt++)
    {
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, auc_para_str_tmp, WAL_HIPRIV_CMD_NAME_MAX_LEN, &pul_cmd_offset);
        if (OAL_SUCC != ul_ret)
        {
             OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_set_fasts_sleep_para::get para fail, return err_code [%d]!}\r\n", ul_ret);
             return ul_ret;
        }
        auc_para_val[ul_para_cnt] = (oal_uint8)oal_atoi(auc_para_str_tmp);
        pc_param += pul_cmd_offset;
    }

    return wal_ioctl_set_fast_sleep_para_etc(pst_cfg_net_dev, auc_para_val);
}

#endif

#ifdef _PRE_WLAN_FEATURE_STA_UAPSD

OAL_STATIC oal_uint32  wal_hipriv_set_uapsd_para(oal_net_device_stru *pst_cfg_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_off_set;
    oal_int8                         ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    mac_cfg_uapsd_sta_stru          *pst_uapsd_param;
    oal_uint8                       uc_max_sp_len;
    oal_uint8                       uc_ac;
    oal_uint8                       uc_delivery_enabled[WLAN_WME_AC_BUTT];
    oal_uint8                       uc_trigger_enabled[WLAN_WME_AC_BUTT];

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_uapsd_para::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    uc_max_sp_len = (oal_uint8)oal_atoi(ac_name);

    for (uc_ac = 0; uc_ac < WLAN_WME_AC_BUTT; uc_ac++)
    {
        pc_param = pc_param + ul_off_set;
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
             OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_uapsd_para::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
             return ul_ret;
        }

        /* delivery_enabled?????????? */
        uc_delivery_enabled[uc_ac] = (oal_uint8)oal_atoi(ac_name);

        /* trigger_enabled ?????????? */
        uc_trigger_enabled[uc_ac] = (oal_uint8)oal_atoi(ac_name);
    }
    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_UAPSD_PARA, OAL_SIZEOF(mac_cfg_uapsd_sta_stru));

    /* ???????????????? */
    pst_uapsd_param = (mac_cfg_uapsd_sta_stru *)(st_write_msg.auc_value);
    pst_uapsd_param->uc_max_sp_len   = uc_max_sp_len;
    for (uc_ac = 0; uc_ac < WLAN_WME_AC_BUTT; uc_ac++)
    {
        pst_uapsd_param->uc_delivery_enabled[uc_ac] = uc_delivery_enabled[uc_ac];
        pst_uapsd_param->uc_trigger_enabled[uc_ac]  = uc_trigger_enabled[uc_ac];
    }

    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_uapsd_sta_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_PWR, "{wal_hipriv_set_uapsd_para::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif

#if defined(_PRE_PRODUCT_ID_HI110X_HOST)

oal_int32 wal_start_vap_etc(oal_net_device_stru *pst_net_dev)
{
    wal_msg_write_stru      st_write_msg;
    oal_int32               l_ret;
    wal_msg_stru           *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32              ul_err_code;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8 en_p2p_mode;
    oal_wireless_dev_stru   *pst_wdev;
#endif

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_net_dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_start_vap_etc::pst_net_dev is null ptr!}\r\n");
        return -OAL_EFAUL;
    }

    OAL_IO_PRINT("wal_start_vap_etc,dev_name is:%s\n", pst_net_dev->name);

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    /* ???????? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_START_VAP, OAL_SIZEOF(mac_cfg_start_vap_param_stru));
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->pst_net_dev = pst_net_dev;
#ifdef _PRE_WLAN_FEATURE_P2P
    pst_wdev    = pst_net_dev->ieee80211_ptr;
    en_p2p_mode = wal_wireless_iftype_to_mac_p2p_mode_etc(pst_wdev->iftype);
    if (WLAN_P2P_BUTT == en_p2p_mode)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_start_vap_etc::wal_wireless_iftype_to_mac_p2p_mode_etc return BUFF}\r\n");
        return -OAL_EINVAL;
    }
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->en_p2p_mode = en_p2p_mode;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_start_vap_etc::en_p2p_mode:%d}\r\n", en_p2p_mode);
#endif
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->en_mgmt_rate_init_flag = OAL_TRUE;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_start_vap_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_start_vap_etc::wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    /* ???????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_start_vap_etc::hmac start vap fail, err code[%d]!}\r\n", ul_err_code);
        return -OAL_EINVAL;
    }


    if (0 == (OAL_NETDEVICE_FLAGS(pst_net_dev) & OAL_IFF_RUNNING))
    {
        OAL_NETDEVICE_FLAGS(pst_net_dev) |= OAL_IFF_RUNNING;
    }

    /* AP????,????VAP??,???????????? */
    oal_net_tx_wake_all_queues(pst_net_dev);/*???????????? */

    return OAL_SUCC;
}


oal_int32  wal_stop_vap_etc(oal_net_device_stru *pst_net_dev)
{
    wal_msg_write_stru      st_write_msg;
    wal_msg_stru           *pst_rsp_msg = OAL_PTR_NULL;
    oal_int32               l_ret;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8 en_p2p_mode;
    oal_wireless_dev_stru   *pst_wdev;
#endif

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_net_dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_stop_vap_etc::pst_net_dev is null ptr!}\r\n");
        return -OAL_EFAUL;
    }

    if (0 == (OAL_NETDEVICE_FLAGS(pst_net_dev) & OAL_IFF_RUNNING))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_stop_vap::vap is already down,continue to reset hmac vap state.}\r\n");
    }

    OAL_IO_PRINT("wal_stop_vap_etc,dev_name is:%s\n", pst_net_dev->name);

    /***************************************************************************
                           ????????wal??????
    ***************************************************************************/
    /* ????WID???? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DOWN_VAP, OAL_SIZEOF(mac_cfg_down_vap_param_stru));
    ((mac_cfg_down_vap_param_stru *)st_write_msg.auc_value)->pst_net_dev = pst_net_dev;
#ifdef _PRE_WLAN_FEATURE_P2P
    pst_wdev    = pst_net_dev->ieee80211_ptr;
    en_p2p_mode = wal_wireless_iftype_to_mac_p2p_mode_etc(pst_wdev->iftype);
    if (WLAN_P2P_BUTT == en_p2p_mode)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_stop_vap_etc::wal_wireless_iftype_to_mac_p2p_mode_etc return BUFF}\r\n");
        return -OAL_EINVAL;
    }
    ((mac_cfg_start_vap_param_stru *)st_write_msg.auc_value)->en_p2p_mode = en_p2p_mode;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_stop_vap_etc::en_p2p_mode:%d}\r\n", en_p2p_mode);
#endif

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_down_vap_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_stop_vap_etc::wal_alloc_cfg_event_etc return err code %d!}\r\n", l_ret);
        return l_ret;
    }

    if (OAL_SUCC != wal_check_and_release_msg_resp_etc(pst_rsp_msg))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "wal_stop_vap_etc::wal_check_and_release_msg_resp_etc fail");
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
oal_int32 wal_init_WiTAS_state(oal_void)
{
#ifdef _PRE_CONFIG_USE_DTS
    oal_int32 wifi_tas_state = 0;
    oal_int32 ret;
    struct device_node *np = NULL;

    if ((aen_tas_switch_en[WLAN_RF_CHANNEL_ZERO] == OAL_FALSE) && (aen_tas_switch_en[WLAN_RF_CHANNEL_ONE] == OAL_FALSE)) {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{not support WiTAS}\r\n");
        return BOARD_SUCC;
    }
    ret = get_board_dts_node_etc(&np, DTS_NODE_HI110X_WIFI);
    if (ret != BOARD_SUCC) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{DTS read node hisi_wifi fail!}\r\n");
        return BOARD_FAIL;
    }
    ret = of_property_read_u32(np, DTS_PROP_HI110X_WIFI_TAS_STATE, &wifi_tas_state);
    if (ret) {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{read prop gpio_wifi_tas_state fail!}\r\n");
        return BOARD_SUCC;
    }
    if (board_get_wifi_tas_gpio_state() != wifi_tas_state) {
        return board_wifi_tas_set(wifi_tas_state);
    } else {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{current WiTAS state is right, no need to set again!}\r\n");
        return BOARD_SUCC;
    }
#else
    return BOARD_SUCC;
#endif
}
#endif


oal_int32 wal_init_wlan_vap_etc(oal_net_device_stru *pst_net_dev)
{
    oal_net_device_stru        *pst_cfg_net_dev;
    wal_msg_write_stru          st_write_msg;
    wal_msg_stru               *pst_rsp_msg = OAL_PTR_NULL;
    oal_uint32                  ul_err_code;
    mac_vap_stru               *pst_mac_vap;
    oal_wireless_dev_stru      *pst_wdev;
    mac_wiphy_priv_stru        *pst_wiphy_priv;
    mac_vap_stru               *pst_cfg_mac_vap;
    hmac_vap_stru              *pst_cfg_hmac_vap;
    mac_device_stru            *pst_mac_device;
    oal_int8                    ac_wlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];
    oal_int8                    ac_p2p_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];
    oal_int32                   l_ret;
    oal_int8                    ac_hwlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];
    wlan_vap_mode_enum_uint8    en_vap_mode;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8    en_p2p_mode = WLAN_LEGACY_VAP_MODE;
#endif

    if(NULL == pst_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::pst_net_dev is null!}\r\n");
        return -OAL_EINVAL;
    }

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(NULL != pst_mac_vap)
    {
        if (MAC_VAP_STATE_BUTT != pst_mac_vap->en_vap_state)
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::pst_mac_vap is already exist}\r\n");
            return OAL_SUCC;
        }
        /* netdev????vap?????????????????????????????? */
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_init_wlan_vap_etc::pst_mac_vap is already free, need creat again!!}");
        OAL_NET_DEV_PRIV(pst_net_dev) = OAL_PTR_NULL;
    }

    pst_wdev = OAL_NETDEVICE_WDEV(pst_net_dev);
    if(NULL == pst_wdev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::pst_wdev is null!}\r\n");
        return -OAL_EFAUL;
    }

    pst_wiphy_priv  = (mac_wiphy_priv_stru *)oal_wiphy_priv(pst_wdev->wiphy);
    pst_mac_device  = pst_wiphy_priv->pst_mac_device;
    if(NULL == pst_mac_device)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::pst_mac_device is null!}\r\n");
        return -OAL_EFAUL;
    }

    /* ????device id????wlanX?? p2pX??netdev?? */
    snprintf_s(ac_wlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "wlan%d", pst_mac_device->uc_device_id);
    snprintf_s(ac_p2p_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "p2p%d",  pst_mac_device->uc_device_id);
    snprintf_s(ac_hwlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "hwlan%d", pst_mac_device->uc_device_id);

    pst_cfg_mac_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_mac_device->uc_cfg_vap_id);
    if (NULL == pst_cfg_mac_vap)
    {
        OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::pst_cfg_mac_vap is null! vap_id:%d,deviceid[%d]}\r\n",pst_mac_device->uc_cfg_vap_id,pst_mac_device->uc_device_id);
        return -OAL_EFAUL;
    }
    pst_cfg_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_device->uc_cfg_vap_id);
    if (NULL == pst_cfg_hmac_vap)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::pst_cfg_hmac_vap is null! vap_id:%d}\r\n",pst_mac_device->uc_cfg_vap_id);
        return -OAL_EFAUL;
    }


    pst_cfg_net_dev = pst_cfg_hmac_vap->pst_net_device;
    if(NULL == pst_cfg_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::pst_cfg_net_dev is null!}\r\n");
        return -OAL_EFAUL;
    }

    /* ??????WIFI??AP??????????VAP */
    if((NL80211_IFTYPE_STATION == pst_wdev->iftype) || (NL80211_IFTYPE_P2P_DEVICE == pst_wdev->iftype))
    {
        if(OAL_ANY_ZERO_VALUE2(oal_strcmp(ac_wlan_netdev_name, pst_net_dev->name), oal_strcmp(ac_hwlan_netdev_name, pst_net_dev->name)))
        {
            en_vap_mode = WLAN_VAP_MODE_BSS_STA;
#ifdef _PRE_WLAN_FEATURE_TAS_ANT_SWITCH
            l_ret = wal_init_WiTAS_state();
            if (l_ret != BOARD_SUCC) {
                OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_WiTAS_state ini WiTAS state fail!}\r\n");
            }
#endif
        }
#ifdef _PRE_WLAN_FEATURE_P2P
        else if(0 == (oal_strcmp(ac_p2p_netdev_name, pst_net_dev->name)))
        {
            en_vap_mode = WLAN_VAP_MODE_BSS_STA;
            en_p2p_mode= WLAN_P2P_DEV_MODE;
        }
#endif
        else
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::net_dev is not wlan or p2p[%d]!}\r\n",pst_mac_device->uc_device_id);
            return OAL_SUCC;
        }
    }
    else if(NL80211_IFTYPE_AP == pst_wdev->iftype)
    {
        en_vap_mode = WLAN_VAP_MODE_BSS_AP;
    }
    else
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::net_dev is not wlan0 or p2p0!}\r\n");
        return OAL_SUCC;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::en_vap_mode:%d,en_p2p_mode:%d}\r\n",
                     en_vap_mode, en_p2p_mode);
#endif

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    /* ???????? */
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_ADD_VAP, OAL_SIZEOF(mac_cfg_add_vap_param_stru));
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->pst_net_dev     = pst_net_dev;
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->en_vap_mode     = en_vap_mode;
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->uc_cfg_vap_indx = pst_cfg_mac_vap->uc_vap_id;
#ifdef _PRE_WLAN_FEATURE_P2P
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->en_p2p_mode      = en_p2p_mode;
#endif
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->bit_11ac2g_enable = (oal_uint8)!!hwifi_get_init_value_etc(CUS_TAG_INI, WLAN_CFG_INIT_11AC2G_ENABLE);
    ((mac_cfg_add_vap_param_stru *)st_write_msg.auc_value)->bit_disable_capab_2ght40 = wlan_customize_etc.uc_disable_capab_2ght40;
#endif
    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_cfg_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_add_vap_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(pst_cfg_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_init_wlan_vap_etc::return err code %d!}\r\n", l_ret);
        return -OAL_EFAIL;
    }

    /* ???????????????? */
    ul_err_code = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_err_code)
    {
        OAM_WARNING_LOG1(pst_cfg_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_init_wlan_vap_etc::hmac add vap fail, err code[%u]!}\r\n", ul_err_code);
        return -OAL_EFAIL;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    if(OAL_SUCC != wal_set_random_mac_to_mib_etc(pst_net_dev))
    {
        OAM_WARNING_LOG0(pst_cfg_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_init_wlan_vap_etc::wal_set_random_mac_to_mib_etc fail!}\r\n");
        return -OAL_EFAUL;
    }
#endif

    /* ????netdevice??MAC??????MAC??????HMAC????????????MIB?? */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_vap_etc::OAL_NET_DEV_PRIV(pst_net_dev) is null ptr.}");
        return -OAL_EINVAL;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    if (en_p2p_mode == WLAN_P2P_DEV_MODE)
    {
        pst_mac_device->st_p2p_info.uc_p2p0_vap_idx = pst_mac_vap->uc_vap_id;
    }
#endif

    if(NL80211_IFTYPE_AP == pst_wdev->iftype)
    {
        /* AP??????????????????????????????????mac???????????? */
        if(g_st_ap_config_info.ul_ap_max_user > 0)
        {
            wal_set_ap_max_user(pst_net_dev, g_st_ap_config_info.ul_ap_max_user);
        }

        if(OAL_STRLEN(g_st_ap_config_info.ac_ap_mac_filter_mode) > 0)
        {
            wal_config_mac_filter(pst_net_dev, g_st_ap_config_info.ac_ap_mac_filter_mode);
        }
    }

    return OAL_SUCC;
}



oal_int32 wal_deinit_wlan_vap_etc(oal_net_device_stru *pst_net_dev)
{
    wal_msg_write_stru           st_write_msg;
    wal_msg_stru                *pst_rsp_msg = OAL_PTR_NULL;
    mac_vap_stru                *pst_mac_vap;
    oal_int32                    l_ret;
    oal_int32                    l_del_vap_flag = OAL_TRUE;
    oal_int8                     ac_wlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];
    oal_int8                     ac_p2p_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];
    oal_int8                     ac_hwlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];
    mac_device_stru             *pst_mac_dev;
#ifdef _PRE_WLAN_FEATURE_P2P
    wlan_p2p_mode_enum_uint8    en_p2p_mode = WLAN_LEGACY_VAP_MODE;
#endif

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_net_dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_deinit_wlan_vap_etc::pst_del_vap_param null ptr !}\r\n");
        return -OAL_EINVAL;
    }


    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_deinit_wlan_vap_etc::pst_mac_vap is already null}\r\n");
        return OAL_SUCC;
    }

    pst_mac_dev = mac_res_get_dev_etc(pst_mac_vap->uc_device_id);
    if(OAL_PTR_NULL == pst_mac_dev)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY,"wal_deinit_wlan_vap_etc::mac_res_get_dev_etc id[%d] FAIL",pst_mac_vap->uc_device_id);
        return -OAL_EFAIL;
    }
        /* ????device id????netdev???? */
    snprintf_s(ac_wlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "wlan%d", pst_mac_dev->uc_device_id);
    snprintf_s(ac_p2p_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "p2p%d",  pst_mac_dev->uc_device_id);
    snprintf_s(ac_hwlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "hwlan%d",  pst_mac_dev->uc_device_id);

    /* ??????WIFI??AP??????????VAP */
    if ((0 != (oal_strcmp(ac_wlan_netdev_name, pst_net_dev->name)))
        && (0 != (oal_strcmp(ac_p2p_netdev_name, pst_net_dev->name)))
        && (0 != (oal_strcmp(ac_hwlan_netdev_name, pst_net_dev->name))))
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_deinit_wlan_vap_etc::net_dev is not wlan or p2p!}\r\n");
        return OAL_SUCC;
    }

#ifdef _PRE_WLAN_FEATURE_P2P
    if (0 == oal_strcmp(ac_p2p_netdev_name, pst_net_dev->name))
    {
        en_p2p_mode = WLAN_P2P_DEV_MODE;
    }
#endif

#ifdef _PRE_WLAN_FEATURE_P2P
    OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_deinit_wlan_vap_etc::en_p2p_mode:%d}\r\n", en_p2p_mode);
#endif

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    //????vap ??????????????????
    /* st_write_msg??????????????????????????????????????????????????????????0??????????????????????vap mode???????????? */
    memset_s(&st_write_msg, OAL_SIZEOF(wal_msg_write_stru), 0, OAL_SIZEOF(wal_msg_write_stru));

    ((mac_cfg_del_vap_param_stru *)st_write_msg.auc_value)->pst_net_dev = pst_net_dev;
#ifdef _PRE_WLAN_FEATURE_P2P
    ((mac_cfg_del_vap_param_stru *)st_write_msg.auc_value)->en_p2p_mode = en_p2p_mode;
#endif

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_DEL_VAP, OAL_SIZEOF(mac_cfg_del_vap_param_stru));
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_del_vap_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_TRUE,
                               &pst_rsp_msg);

    if (OAL_SUCC != wal_check_and_release_msg_resp_etc(pst_rsp_msg))
    {
        OAM_WARNING_LOG0(0,OAM_SF_ANY,"wal_deinit_wlan_vap_etc::wal_check_and_release_msg_resp_etc fail.");
        /*can't set net dev's vap ptr to null when
          del vap wid process failed!*/
        l_del_vap_flag = OAL_FALSE;
    }

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_deinit_wlan_vap_etc::return error code %d}\r\n", l_ret);
        if(-OAL_ENOMEM == l_ret || -OAL_EFAIL == l_ret)
        {
            /*wid had't processed*/
           l_del_vap_flag = OAL_FALSE;
        }
    }

    if(OAL_TRUE == l_del_vap_flag)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_deinit_wlan_vap_etc::clear netdev priv.}\r\n");
        OAL_NET_DEV_PRIV(pst_net_dev) = NULL;
    }

    return l_ret;
}



OAL_STATIC oal_int32 wal_set_mac_addr(oal_net_device_stru *pst_net_dev)
{
    oal_uint8                     auc_primary_mac_addr[WLAN_MAC_ADDR_LEN] = {0};    /* MAC???? */
    oal_wireless_dev_stru        *pst_wdev;
    mac_wiphy_priv_stru          *pst_wiphy_priv;
    mac_device_stru              *pst_mac_device;
    oal_int8                    ac_hwlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];

    pst_wdev = OAL_NETDEVICE_WDEV(pst_net_dev);
    pst_wiphy_priv = (mac_wiphy_priv_stru *)(oal_wiphy_priv(pst_wdev->wiphy));
    pst_mac_device = pst_wiphy_priv->pst_mac_device;

#ifdef _PRE_WLAN_FEATURE_P2P
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_device->st_p2p_info.pst_primary_net_device))
    {
        /* random mac will be used. hi1102-cb (#include <linux/etherdevice.h>)    */
        oal_random_ether_addr(auc_primary_mac_addr);
        auc_primary_mac_addr[0] &= (~0x02);
        auc_primary_mac_addr[1] = 0x11;
        auc_primary_mac_addr[2] = 0x02;
    }
    else
    {
#ifndef _PRE_PC_LINT
        if(OAL_LIKELY(OAL_PTR_NULL != OAL_NETDEVICE_MAC_ADDR(pst_mac_device->st_p2p_info.pst_primary_net_device)))
        {
            if (EOK != memcpy_s(auc_primary_mac_addr, WLAN_MAC_ADDR_LEN,
                                OAL_NETDEVICE_MAC_ADDR(pst_mac_device->st_p2p_info.pst_primary_net_device), WLAN_MAC_ADDR_LEN)) {
                OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_set_mac_addr::memcpy fail!");
                return OAL_FAIL;
            }
        }
        else
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_get_mac_addr() pst_primary_net_device; addr is null}\r\n");
            return OAL_FAIL;
        }
#endif
    }

    switch (pst_wdev->iftype)
    {
        case NL80211_IFTYPE_P2P_DEVICE:
        {
            /* ??wlan0(NV)????P2P0 ??GCGO ??????MAC ????????????mac ????bit ??????1 */
            auc_primary_mac_addr[0] |= 0x02;
            oal_set_mac_addr((oal_uint8 *)OAL_NETDEVICE_MAC_ADDR(pst_net_dev), auc_primary_mac_addr);
            oal_set_mac_addr(g_auc_wifip2p0mac_etc, auc_primary_mac_addr);
            auc_primary_mac_addr[4] ^= 0x80;
            oal_set_mac_addr(g_auc_wifiGCGOmac_etc, auc_primary_mac_addr);
            break;
        }
        default:
        {
            /*????????--add ????hwlan name????*/
            snprintf_s(ac_hwlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
                       "hwlan%d",  pst_mac_device->uc_device_id);
            if(0 == (oal_strcmp(ac_hwlan_netdev_name, pst_net_dev->name)))
            {
                auc_primary_mac_addr[0] |= 0x02;
                auc_primary_mac_addr[5] += 1;
            }
            else
            {

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
                hwifi_get_mac_addr_etc(auc_primary_mac_addr);
                auc_primary_mac_addr[0]&=(~0x02);
#else
                oal_random_ether_addr(auc_primary_mac_addr);
                auc_primary_mac_addr[0]&=(~0x02);
                auc_primary_mac_addr[1]=0x11;
                auc_primary_mac_addr[2]=0x02;
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */
            }
            oal_set_mac_addr((oal_uint8 *)OAL_NETDEVICE_MAC_ADDR(pst_net_dev), auc_primary_mac_addr);

            /* NV??????MAC??????????sta ap MAC???????????? */
            oal_set_mac_addr(g_auc_wifistamac_etc, auc_primary_mac_addr);
            oal_set_mac_addr(g_auc_wifiapmac_etc, auc_primary_mac_addr);
            oal_set_mac_addr(g_auc_wifinvmac_etc, auc_primary_mac_addr);
            OAM_WARNING_LOG3(0, OAM_SF_ANY, "{wal_set_mac_addr:: init wlan0 mac use NV,set macaddr: %02X:XX:XX:XX:%02X:%02X!}\r\n",
                                      auc_primary_mac_addr[0], auc_primary_mac_addr[4], auc_primary_mac_addr[5]);
            break;
        }
    }
#else
            /*????????--add ????hwlan name????*/
            oal_random_ether_addr(auc_primary_mac_addr);
            snprintf_s(ac_hwlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
                       "hwlan%d",  pst_mac_device->uc_device_id);
            if(0 == (oal_strcmp(ac_hwlan_netdev_name, pst_net_dev->name)))
            {
                auc_primary_mac_addr[0] |= 0x02;
                auc_primary_mac_addr[5] += 1;
            }
            else
            {
                auc_primary_mac_addr[0]&=(~0x02);
                auc_primary_mac_addr[1]=0x11;
                auc_primary_mac_addr[2]=0x02;
            }
            oal_set_mac_addr((oal_uint8 *)OAL_NETDEVICE_MAC_ADDR(pst_net_dev), auc_primary_mac_addr);
#endif

    return OAL_SUCC;
}


oal_int32 wal_init_wlan_netdev_etc(oal_wiphy_stru *pst_wiphy, const char *dev_name)
{
    mac_device_stru            *pst_mac_device;
    oal_int8                    ac_wlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];
    oal_int8                    ac_p2p_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];
    oal_net_device_stru        *pst_net_dev;
    oal_wireless_dev_stru      *pst_wdev;
    mac_wiphy_priv_stru        *pst_wiphy_priv;
    enum nl80211_iftype         en_type;
    oal_int32                   l_ret;
    oal_int8                    ac_hwlan_netdev_name[MAC_NET_DEVICE_NAME_LENGTH];
    oal_netdev_priv_stru        *pst_netdev_priv;

    if((NULL == pst_wiphy) || (NULL == dev_name))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_netdev_etc::pst_wiphy or dev_name is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_wiphy_priv  = (mac_wiphy_priv_stru *)(oal_wiphy_priv(pst_wiphy));
    if (OAL_PTR_NULL == pst_wiphy_priv)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_netdev_etc::pst_wiphy_priv is null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_device = pst_wiphy_priv->pst_mac_device;
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_netdev_etc::pst_wiphy_priv is null!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ????device id????netdev???? */
    snprintf_s(ac_wlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "wlan%d", pst_mac_device->uc_device_id);
    snprintf_s(ac_p2p_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "p2p%d",  pst_mac_device->uc_device_id);
    /*????????-add*/
    snprintf_s(ac_hwlan_netdev_name, MAC_NET_DEVICE_NAME_LENGTH, MAC_NET_DEVICE_NAME_LENGTH - 1,
               "hwlan%d",  pst_mac_device->uc_device_id);

    /*????????--????hwlan name????*/
    if (0 == (oal_strcmp(ac_wlan_netdev_name, dev_name)) ||0 == (oal_strcmp(ac_hwlan_netdev_name, dev_name)))
    {
        en_type = NL80211_IFTYPE_STATION;
    }
    else if (0 == (oal_strcmp(ac_p2p_netdev_name, dev_name)))
    {
        en_type = NL80211_IFTYPE_P2P_DEVICE;
    }
    else
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_netdev_etc::dev name is not wlan or p2p}\r\n");
        return OAL_SUCC;
    }
    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_init_wlan_netdev_etc::en_type is %d}\r\n", en_type);

    /* ??????????net device?????????????????? */
    /* ????dev_name????dev */
    pst_net_dev = oal_dev_get_by_name(dev_name);
    if (OAL_PTR_NULL != pst_net_dev)
    {
        /* ????oal_dev_get_by_name????????????oal_dev_put??net_dev?????????????? */
        oal_dev_put(pst_net_dev);

        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_netdev_etc::the net_device is already exist!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

#if defined(_PRE_WLAN_FEATURE_FLOWCTL)
    pst_net_dev = oal_net_alloc_netdev_mqs(OAL_SIZEOF(oal_netdev_priv_stru), dev_name, oal_ether_setup, WAL_NETDEV_SUBQUEUE_MAX_NUM, 1);    /* ??????????????????????????????????????????0 */
#elif defined(_PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL)
    pst_net_dev = oal_net_alloc_netdev_mqs(OAL_SIZEOF(oal_netdev_priv_stru), dev_name, oal_ether_setup, WLAN_NET_QUEUE_BUTT, 1);    /* ??????????????????????????????????????????0 */
#else
    pst_net_dev = oal_net_alloc_netdev(OAL_SIZEOF(oal_netdev_priv_stru), dev_name, oal_ether_setup);    /* ??????????????????????????????????????????0 */
#endif

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_net_dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_netdev_etc::oal_net_alloc_netdev return null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_wdev = (oal_wireless_dev_stru *)OAL_MEM_ALLOC(OAL_MEM_POOL_ID_LOCAL,OAL_SIZEOF(oal_wireless_dev_stru), OAL_FALSE);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_wdev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_netdev_etc::alloc mem, pst_wdev is null ptr!}\r\n");
        oal_net_free_netdev(pst_net_dev);
        return OAL_ERR_CODE_PTR_NULL;
    }

    memset_s(pst_wdev, OAL_SIZEOF(oal_wireless_dev_stru), 0, OAL_SIZEOF(oal_wireless_dev_stru));

#ifdef _PRE_WLAN_FEATURE_GSO
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_init_wlan_netdev::[GSO] add NETIF_F_SG}");
    pst_net_dev->features |= NETIF_F_SG;
    pst_net_dev->hw_features |= NETIF_F_SG;
#endif

    /* ??netdevice???????? */
#ifdef CONFIG_WIRELESS_EXT
    pst_net_dev->wireless_handlers             = &g_st_iw_handler_def_etc;
#endif
    pst_net_dev->netdev_ops                    = &g_st_wal_net_dev_ops_etc;

    pst_net_dev->ethtool_ops                   = &g_st_wal_ethtool_ops_etc;

    OAL_NETDEVICE_DESTRUCTOR(pst_net_dev)      = oal_net_free_netdev;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,44))
    OAL_NETDEVICE_MASTER(pst_net_dev)          = OAL_PTR_NULL;
#endif

    OAL_NETDEVICE_IFALIAS(pst_net_dev)         = OAL_PTR_NULL;
    OAL_NETDEVICE_WATCHDOG_TIMEO(pst_net_dev)  = 5;
    OAL_NETDEVICE_WDEV(pst_net_dev)            = pst_wdev;
    OAL_NETDEVICE_QDISC(pst_net_dev, OAL_PTR_NULL);

    pst_wdev->netdev = pst_net_dev;
    pst_wdev->iftype = en_type;
    pst_wdev->wiphy = pst_wiphy;
    pst_wiphy_priv = (mac_wiphy_priv_stru *)(oal_wiphy_priv(pst_wiphy));

#ifdef _PRE_WLAN_FEATURE_P2P
    /*????????--add wlan name ????*/
    if ((NL80211_IFTYPE_STATION == en_type) && (0 == (oal_strcmp(ac_wlan_netdev_name, dev_name))))
    {
        /* ????????wlan0?? ??????wlan0 ????net_device,p2p0 ??p2p-p2p0 MAC ????????netdevice ???? */
        pst_wiphy_priv->pst_mac_device->st_p2p_info.pst_primary_net_device = pst_net_dev;
    }
    /*????????--add hwlan name ????*/
    else if((NL80211_IFTYPE_STATION == en_type) && (0 == (oal_strcmp(ac_hwlan_netdev_name, dev_name))))
    {
        /*netdevice ????????????????????*/
        pst_wiphy_priv->pst_mac_device->st_p2p_info.pst_second_net_device = pst_net_dev;
    }
    else if(NL80211_IFTYPE_P2P_DEVICE == en_type)
    {
        pst_wiphy_priv->pst_mac_device->st_p2p_info.pst_p2p_net_device = pst_net_dev;
    }
#endif
    OAL_NETDEVICE_FLAGS(pst_net_dev) &= ~OAL_IFF_RUNNING;   /* ??net device??flag????down */

    wal_set_mac_addr(pst_net_dev);

    pst_netdev_priv = (oal_netdev_priv_stru *)OAL_NET_DEV_WIRELESS_PRIV(pst_net_dev);
    pst_netdev_priv->uc_napi_enable     = OAL_TRUE;
    pst_netdev_priv->uc_gro_enable      = OAL_TRUE;
    pst_netdev_priv->uc_napi_weight     = NAPI_POLL_WEIGHT_LEV1;
    pst_netdev_priv->ul_queue_len_max   = NAPI_NETDEV_PRIV_QUEUE_LEN_MAX;
    pst_netdev_priv->uc_state           = 0;
    pst_netdev_priv->ul_period_pkts     = 0;
    pst_netdev_priv->ul_period_start    = 0;

    oal_netbuf_list_head_init(&pst_netdev_priv->st_rx_netbuf_queue);
    oal_netif_napi_add(pst_net_dev, &pst_netdev_priv->st_napi, hmac_rxdata_polling, NAPI_POLL_WEIGHT_LEV1);

    /* ????net_device */
    l_ret = oal_net_register_netdev(pst_net_dev);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {

        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_init_wlan_netdev_etc::oal_net_register_netdev return error code %d!}\r\n", l_ret);

        OAL_MEM_FREE(pst_wdev, OAL_FALSE);
        oal_net_free_netdev(pst_net_dev);

        return l_ret;
    }

    return OAL_SUCC;
}


oal_int32  wal_setup_ap_etc(oal_net_device_stru *pst_net_dev)
{
    oal_int32 l_ret;
    mac_vap_stru                *pst_mac_vap;
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
    wal_set_power_mgmt_on(OAL_FALSE);
    l_ret = wal_set_power_on(pst_net_dev, OAL_TRUE);
    if (OAL_SUCC != l_ret)
    {
        OAM_ERROR_LOG1(0, OAM_SF_CFG, "{wal_setup_ap_etc::wal_set_power_on fail [%d]!}", l_ret);
        return l_ret;
    }
#endif /* #if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */

    if (OAL_NETDEVICE_FLAGS(pst_net_dev) & OAL_IFF_RUNNING)
    {
        /* ??????AP??????????????????UP????????????down wlan0???????? */
        OAL_IO_PRINT("wal_setup_ap:stop netdevice:%.16s", pst_net_dev->name);
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_setup_ap_etc:: oal_iff_running! now, stop netdevice}");
        wal_netdev_stop_etc(pst_net_dev);
    }

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(pst_mac_vap)
    {

        if (pst_mac_vap->en_vap_state != MAC_VAP_STATE_INIT)
        {
            /* ??????AP??????????????????UP????????????down wlan0???????? */
            OAL_IO_PRINT("wal_setup_ap_etc:stop netdevice:%s", pst_net_dev->name);
            wal_netdev_stop_etc(pst_net_dev);
        }
    }

    pst_net_dev->ieee80211_ptr->iftype = NL80211_IFTYPE_AP;

    l_ret = wal_init_wlan_vap_etc(pst_net_dev);

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
    if(OAL_SUCC == l_ret)
    {
#if ((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE))
        /* ???????????????? */
        (OAL_TRUE == wlan_pm_switch_etc) ? wlan_pm_enable_etc() : wlan_pm_disable_etc();
#endif // #if ((_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE))
    }
#endif /* #ifdef _PRE_PLAT_FEATURE_CUSTOMIZE */
    return l_ret;
}

#endif

#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD

oal_uint32 wal_hipriv_register_inetaddr_notifier_etc(oal_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (0 == register_inetaddr_notifier(&wal_hipriv_notifier))
    {
        return OAL_SUCC;
    }

    OAM_ERROR_LOG0(0, OAM_SF_PWR, "{wal_hipriv_register_inetaddr_notifier_etc::register inetaddr notifier failed.}");
    return OAL_FAIL;

#else
    return OAL_SUCC;
#endif
}


oal_uint32 wal_hipriv_unregister_inetaddr_notifier_etc(oal_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (0 == unregister_inetaddr_notifier(&wal_hipriv_notifier))
    {
        return OAL_SUCC;
    }

    OAM_ERROR_LOG0(0, OAM_SF_PWR, "{wal_hipriv_unregister_inetaddr_notifier_etc::hmac_unregister inetaddr notifier failed.}");
    return OAL_FAIL;

#else
    return OAL_SUCC;
#endif
}


oal_uint32 wal_hipriv_register_inet6addr_notifier_etc(oal_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (0 == register_inet6addr_notifier(&wal_hipriv_notifier_ipv6))
    {
        return OAL_SUCC;
    }

    OAM_ERROR_LOG0(0, OAM_SF_PWR, "{wal_hipriv_register_inet6addr_notifier_etc::register inetaddr6 notifier failed.}");
    return OAL_FAIL;

#else
    return OAL_SUCC;
#endif
}


oal_uint32 wal_hipriv_unregister_inet6addr_notifier_etc(oal_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    if (0 == unregister_inet6addr_notifier(&wal_hipriv_notifier_ipv6))
    {
        return OAL_SUCC;
    }

    OAM_ERROR_LOG0(0, OAM_SF_PWR, "{wal_hipriv_unregister_inet6addr_notifier_etc::hmac_unregister inetaddr6 notifier failed.}");
    return OAL_FAIL;

#else
    return OAL_SUCC;
#endif
}


#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)

oal_int32 wal_hipriv_inetaddr_notifier_call_etc(struct notifier_block *this, oal_uint event, oal_void *ptr)
{
    /*
     * Notification mechanism from kernel to our driver. This function is called by the Linux kernel
     * whenever there is an event related to an IP address.
     * ptr : kernel provided pointer to IP address that has changed
     */
    struct in_ifaddr    *pst_ifa       = (struct in_ifaddr *)ptr;
    mac_vap_stru        *pst_mac_vap   = OAL_PTR_NULL;
    hmac_vap_stru       *pst_hmac_vap  = OAL_PTR_NULL;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_ifa))
    {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "{wal_hipriv_inetaddr_notifier_call_etc::pst_ifa is NULL.}");
        return NOTIFY_DONE;
    }
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_ifa->ifa_dev->dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "{wal_hipriv_inet6addr_notifier_call_etc::pst_ifa->idev->dev is NULL.}");
        return NOTIFY_DONE;
     }

    /* Filter notifications meant for non Hislicon devices */
    if (pst_ifa->ifa_dev->dev->netdev_ops != &g_st_wal_net_dev_ops_etc)
    {
        return NOTIFY_DONE;
    }

    pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_ifa->ifa_dev->dev);
    if (OAL_UNLIKELY(NULL == pst_mac_vap))
    {
        OAM_WARNING_LOG1(0, OAM_SF_PWR, "{wal_hipriv_inetaddr_notifier_call_etc::Get mac vap failed, when %d(UP:1 DOWN:2 UNKNOWN:others) ipv4 address.}", event);
        return NOTIFY_DONE;
    }

    if (ipv4_is_linklocal_169(pst_ifa->ifa_address))
    {
        OAM_WARNING_LOG3(pst_mac_vap->uc_vap_id, OAM_SF_PWR, "{wal_hipriv_inetaddr_notifier_call_etc::Invalid IPv4[%d.X.X.%d], MASK[0x%08X].}",
                         ((oal_uint8 *)&(pst_ifa->ifa_address))[0],
                         ((oal_uint8 *)&(pst_ifa->ifa_address))[3],
                         pst_ifa->ifa_mask);
        return NOTIFY_DONE;
    }

    wal_wake_lock();

    switch (event)
    {
        case NETDEV_UP:
        {
            OAM_WARNING_LOG3(pst_mac_vap->uc_vap_id, OAM_SF_PWR, "{wal_hipriv_inetaddr_notifier_call_etc::Up IPv4[%d.X.X.%d], MASK[0x%08X].}",
                             ((oal_uint8 *)&(pst_ifa->ifa_address))[0],
                             ((oal_uint8 *)&(pst_ifa->ifa_address))[3],
                             pst_ifa->ifa_mask);
            hmac_arp_offload_set_ip_addr_etc(pst_mac_vap, DMAC_CONFIG_IPV4, DMAC_IP_ADDR_ADD, &(pst_ifa->ifa_address), &(pst_ifa->ifa_mask));

            if (WLAN_VAP_MODE_BSS_STA == pst_mac_vap->en_vap_mode)
            {
                pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
                if (OAL_PTR_NULL == pst_hmac_vap)
                {
                    OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_hipriv_inetaddr_notifier_call_etc:: pst_hmac_vap null.uc_vap_id[%d]}",
                        pst_mac_vap->uc_vap_id);

                    wal_wake_unlock();

                    return NOTIFY_DONE;
                }
                /* ??????IP???????????????????? */
            #if _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE
                if((MAX_FAST_PS==pst_hmac_vap->uc_ps_mode)||(AUTO_FAST_PS==pst_hmac_vap->uc_ps_mode))
                {
                    wlan_pm_set_timeout_etc((wlan_min_fast_ps_idle>1)?(wlan_min_fast_ps_idle-1): wlan_min_fast_ps_idle);
                }
                else
                {
                    wlan_pm_set_timeout_etc(WLAN_SLEEP_DEFAULT_CHECK_CNT);
                }
            #endif

                /* ??????IP?????????????????????? */
            #ifdef _PRE_WLAN_FEATURE_ROAM
                hmac_roam_wpas_connect_state_notify_etc(pst_hmac_vap, WPAS_CONNECT_STATE_IPADDR_OBTAINED);
            #endif

            }
            break;
        }


        case NETDEV_DOWN:
        {
#if _PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE
            wlan_pm_set_timeout_etc(WLAN_SLEEP_LONG_CHECK_CNT);
#endif
            OAM_WARNING_LOG3(pst_mac_vap->uc_vap_id, OAM_SF_PWR, "{wal_hipriv_inetaddr_notifier_call_etc::Down IPv4[%d.X.X.%d], MASK[0x%08X]..}",
                             ((oal_uint8 *)&(pst_ifa->ifa_address))[0],
                             ((oal_uint8 *)&(pst_ifa->ifa_address))[3],
                             pst_ifa->ifa_mask);
            hmac_arp_offload_set_ip_addr_etc(pst_mac_vap, DMAC_CONFIG_IPV4, DMAC_IP_ADDR_DEL, &(pst_ifa->ifa_address), &(pst_ifa->ifa_mask));

            if (WLAN_VAP_MODE_BSS_STA == pst_mac_vap->en_vap_mode)
            {
                /* ??????IP?????????????????????? */
            #ifdef _PRE_WLAN_FEATURE_ROAM
                pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
                if (OAL_PTR_NULL == pst_hmac_vap)
                {
                    OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{wal_hipriv_inetaddr_notifier_call_etc:: pst_hmac_vap null.uc_vap_id[%d]}",
                        pst_mac_vap->uc_vap_id);

                    wal_wake_unlock();

                    return NOTIFY_DONE;
                }
                hmac_roam_wpas_connect_state_notify_etc(pst_hmac_vap, WPAS_CONNECT_STATE_IPADDR_REMOVED);
            #endif
            }
            break;
        }

        default:
        {
            OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_PWR, "{wal_hipriv_inetaddr_notifier_call_etc::Unknown notifier event[%d].}", event);
            break;
        }
    }
    wal_wake_unlock();

    return NOTIFY_DONE;
}



oal_int32 wal_hipriv_inet6addr_notifier_call_etc(struct notifier_block *this, oal_uint event, oal_void *ptr)
{
    /*
     * Notification mechanism from kernel to our driver. This function is called by the Linux kernel
     * whenever there is an event related to an IP address.
     * ptr : kernel provided pointer to IP address that has changed
     */
    struct inet6_ifaddr    *pst_ifa       = (struct inet6_ifaddr *)ptr;
    mac_vap_stru           *pst_mac_vap;

    if (OAL_UNLIKELY(NULL == pst_ifa))
    {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "{wal_hipriv_inet6addr_notifier_call_etc::pst_ifa is NULL.}");
        return NOTIFY_DONE;
    }
    if (OAL_UNLIKELY(NULL == pst_ifa->idev->dev))
    {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "{wal_hipriv_inet6addr_notifier_call_etc::pst_ifa->idev->dev is NULL.}");
        return NOTIFY_DONE;
     }


    /* Filter notifications meant for non Hislicon devices */
    if (pst_ifa->idev->dev->netdev_ops != &g_st_wal_net_dev_ops_etc)
    {
        return NOTIFY_DONE;
    }

    pst_mac_vap = (mac_vap_stru *)OAL_NET_DEV_PRIV(pst_ifa->idev->dev);
    if (OAL_UNLIKELY(NULL == pst_mac_vap))
    {
        OAM_WARNING_LOG1(0, OAM_SF_PWR, "{wal_hipriv_inet6addr_notifier_call_etc::Get mac vap failed, when %d(UP:1 DOWN:2 UNKNOWN:others) ipv6 address.}", event);
        return NOTIFY_DONE;
    }

    switch (event)
    {
        case NETDEV_UP:
        {
            OAM_WARNING_LOG4(pst_mac_vap->uc_vap_id, OAM_SF_PWR, "{wal_hipriv_inet6addr_notifier_call_etc::UP IPv6[%04x:%04x:XXXX:XXXX:XXXX:XXXX:%04x:%04x].}",
                             OAL_NET2HOST_SHORT((pst_ifa->addr.s6_addr16)[0]),
                             OAL_NET2HOST_SHORT((pst_ifa->addr.s6_addr16)[1]),
                             OAL_NET2HOST_SHORT((pst_ifa->addr.s6_addr16)[6]),
                             OAL_NET2HOST_SHORT((pst_ifa->addr.s6_addr16)[7]));
            hmac_arp_offload_set_ip_addr_etc(pst_mac_vap, DMAC_CONFIG_IPV6, DMAC_IP_ADDR_ADD, &(pst_ifa->addr), &(pst_ifa->addr));
            break;
        }

        case NETDEV_DOWN:
        {
            OAM_WARNING_LOG4(pst_mac_vap->uc_vap_id, OAM_SF_PWR, "{wal_hipriv_inet6addr_notifier_call_etc::DOWN IPv6[%04x:%04x:XXXX:XXXX:XXXX:XXXX:%04x:%04x].}",
                             OAL_NET2HOST_SHORT((pst_ifa->addr.s6_addr16)[0]),
                             OAL_NET2HOST_SHORT((pst_ifa->addr.s6_addr16)[1]),
                             OAL_NET2HOST_SHORT((pst_ifa->addr.s6_addr16)[6]),
                             OAL_NET2HOST_SHORT((pst_ifa->addr.s6_addr16)[7]));
            hmac_arp_offload_set_ip_addr_etc(pst_mac_vap, DMAC_CONFIG_IPV6, DMAC_IP_ADDR_DEL, &(pst_ifa->addr), &(pst_ifa->addr));
            break;
        }

        default:
        {
            OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_PWR, "{wal_hipriv_inet6addr_notifier_call_etc::Unknown notifier event[%d].}", event);
            break;
        }
    }

    return NOTIFY_DONE;
}
#endif /* #if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) */
#endif /* #ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD */
#ifdef _PRE_WLAN_FEATURE_11K

OAL_STATIC oal_uint32  wal_hipriv_send_neighbor_req(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_offset = 0;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    mac_cfg_ssid_param_stru          *pst_ssid;
    oal_uint8                        uc_str_len;

    uc_str_len = OS_STR_LEN(pc_param);
    uc_str_len = (uc_str_len > 1)?uc_str_len-1:uc_str_len;

    /* ????SSID?????? */
    if (0 != uc_str_len)
    {
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_offset);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_send_neighbor_req::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
            return ul_ret;
        }
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SEND_NEIGHBOR_REQ, OAL_SIZEOF(mac_cfg_ssid_param_stru));
    pst_ssid = (mac_cfg_ssid_param_stru *)st_write_msg.auc_value;
    pst_ssid->uc_ssid_len = uc_str_len;
    l_ret = memcpy_s(pst_ssid->ac_ssid, WLAN_SSID_MAX_LEN, ac_arg, pst_ssid->uc_ssid_len);
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_send_neighbor_req::memcpy fail!");
        return OAL_FAIL;
    }

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_ssid_param_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_send_neighbor_req::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
    return OAL_SUCC;
}

OAL_STATIC oal_uint32  wal_hipriv_beacon_req_table_switch(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_offset = 0;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint8                        uc_switch;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_offset);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_beacon_req_table_switch::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    uc_switch = (oal_uint8)oal_atoi(ac_arg);

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_BCN_TABLE_SWITCH, OAL_SIZEOF(oal_uint8));
    st_write_msg.auc_value[0] = uc_switch;
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_beacon_req_table_switch::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
    return OAL_SUCC;
}
#endif //_PRE_WLAN_FEATURE_11K


OAL_STATIC oal_uint32  wal_hipriv_voe_enable(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru               st_write_msg;
    oal_uint32                       ul_offset = 0;
    oal_uint32                       ul_ret;
    oal_int32                        l_ret;
    oal_int8                         ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint16                       us_switch;
    oal_uint16                      *ps_value = OAL_PTR_NULL;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_offset);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_voe_enable::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    us_switch = (oal_uint16)oal_atoi(ac_arg);
    us_switch = us_switch & 0xFFFF;

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_VOE_ENABLE, OAL_SIZEOF(oal_uint16));
    ps_value = (oal_uint16 *)st_write_msg.auc_value;
    *ps_value  = us_switch;
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint16),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_voe_enable::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_VOWIFI

OAL_STATIC oal_int32 wal_ioctl_set_vowifi_param(oal_net_device_stru *pst_net_dev, oal_int8* puc_command,
    wal_wifi_priv_cmd_stru *pst_priv_cmd)
{

    oal_int32                   l_ret;
    oal_uint16                  us_len;
    wal_msg_write_stru          st_write_msg;
    mac_cfg_vowifi_stru        *pst_cfg_vowifi;
    mac_vowifi_cmd_enum_uint8   en_vowifi_cmd_id;
    oal_uint8                   uc_param;
    oal_uint8                   uc_cfg_id;

    /* vap????????????????supplicant???? */
    if (OAL_PTR_NULL == OAL_NET_DEV_PRIV(pst_net_dev))
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_set_vowifi_param::vap not created yet, ignore the cmd!");
        return -OAL_EINVAL;
    }

    for (uc_cfg_id = VOWIFI_SET_MODE; uc_cfg_id < VOWIFI_CMD_BUTT; uc_cfg_id++) {
        if ((0 == oal_strncmp(puc_command, g_ast_vowifi_cmd_table[uc_cfg_id].pc_priv_cmd,
            OAL_STRLEN(g_ast_vowifi_cmd_table[uc_cfg_id].pc_priv_cmd))) &&
            (pst_priv_cmd->ul_total_len > (OAL_STRLEN(g_ast_vowifi_cmd_table[uc_cfg_id].pc_priv_cmd) + 1))) {
            uc_param = (oal_uint8)oal_atoi((oal_int8*)puc_command + OAL_STRLEN((oal_uint8 *)g_ast_vowifi_cmd_table[uc_cfg_id].pc_priv_cmd) + 1);
            en_vowifi_cmd_id = (oal_uint8)g_ast_vowifi_cmd_table[uc_cfg_id].ul_case_entry;
            break;
        }
    }

    if (uc_cfg_id >= VOWIFI_CMD_BUTT) {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_set_vowifi_param::invalid cmd!");
        return -OAL_EINVAL;
    }

    OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_ioctl_set_vowifi_param::supplicant set VoWiFi_param cmd(%d), value[%d] }", en_vowifi_cmd_id, uc_param);

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    us_len         = OAL_SIZEOF(mac_cfg_vowifi_stru);
    pst_cfg_vowifi = (mac_cfg_vowifi_stru *)(st_write_msg.auc_value);
    pst_cfg_vowifi->en_vowifi_cfg_cmd = en_vowifi_cmd_id;
    pst_cfg_vowifi->uc_value          = uc_param;

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_VOWIFI_INFO, us_len);
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                       WAL_MSG_TYPE_WRITE,
                       WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                       (oal_uint8 *)&st_write_msg,
                       OAL_FALSE,
                       OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "wal_ioctl_set_vowifi_param::wal_send_cfg_event_etc failed, error no[%d]!\r\n", l_ret);
        return l_ret;
    }
    return OAL_SUCC;
}


OAL_STATIC oal_int32 wal_ioctl_get_vowifi_param(oal_net_device_stru *pst_net_dev, oal_int8 *puc_command, oal_int32 *pl_value)
{
    mac_vap_stru *pst_mac_vap;

    /* vap????????????????supplicant???? */
    if (OAL_PTR_NULL == OAL_NET_DEV_PRIV(pst_net_dev))
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_get_vowifi_param::vap not created yet, ignore the cmd!");
        return -OAL_EINVAL;
    }

    /* ????mac_vap */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap->pst_vowifi_cfg_param)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_ioctl_get_vowifi_param::pst_vowifi_cfg_param is null.}");
        return OAL_SUCC;
    }

    if (oal_strncmp(puc_command, CMD_VOWIFI_GET_MODE, OAL_STRLEN(CMD_VOWIFI_GET_MODE)) == 0)
    {
        *pl_value  = (oal_int)pst_mac_vap->pst_vowifi_cfg_param->en_vowifi_mode;
    }
    else if (oal_strncmp(puc_command, CMD_VOWIFI_GET_PERIOD, OAL_STRLEN(CMD_VOWIFI_GET_PERIOD)) == 0)
    {
        *pl_value  = (oal_int)pst_mac_vap->pst_vowifi_cfg_param->us_rssi_period_ms/1000;
    }
    else if (oal_strncmp(puc_command, CMD_VOWIFI_GET_LOW_THRESHOLD, OAL_STRLEN(CMD_VOWIFI_GET_LOW_THRESHOLD)) == 0)
    {
        *pl_value  = (oal_int)pst_mac_vap->pst_vowifi_cfg_param->c_rssi_low_thres;
    }
    else if (oal_strncmp(puc_command, CMD_VOWIFI_GET_HIGH_THRESHOLD, OAL_STRLEN(CMD_VOWIFI_GET_HIGH_THRESHOLD)) == 0)
    {
        *pl_value  = (oal_int)pst_mac_vap->pst_vowifi_cfg_param->c_rssi_high_thres;
    }
    else if (oal_strncmp(puc_command, CMD_VOWIFI_GET_TRIGGER_COUNT, OAL_STRLEN(CMD_VOWIFI_GET_TRIGGER_COUNT)) == 0)
    {
        *pl_value  = (oal_int)pst_mac_vap->pst_vowifi_cfg_param->uc_trigger_count_thres;
    }
    else
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_get_vowifi_param::invalid cmd!");
        *pl_value  = 0xffffffff;
        return -OAL_EINVAL;
    }

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_get_vowifi_param::supplicant get VoWiFi_param value[%d] }",*pl_value);

    return OAL_SUCC;
}

#endif /* _PRE_WLAN_FEATURE_VOWIFI */

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

OAL_STATIC oal_int32 wal_ioctl_get_wifi_priv_feature_cap_param(oal_net_device_stru *pst_net_dev, oal_int8 *puc_command, oal_int32 *pl_value)
{
    mac_vap_stru                    *pst_mac_vap = OAL_PTR_NULL;
    hmac_vap_stru                   *pst_hmac_vap = OAL_PTR_NULL;
    oal_uint32                       ul_value = 0;

    /* vap????????????????supplicant???? */
    if (OAL_PTR_NULL == OAL_NET_DEV_PRIV(pst_net_dev))
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_get_wifi_priv_feature_cap_param::vap not created yet, ignore the cmd!");
        return -OAL_EINVAL;
    }

    /* ????mac_vap */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_ioctl_get_wifi_priv_feature_cap_param::netdevice->mac_vap is null.}");
        return OAL_SUCC;
    }

    *pl_value    = 0;
    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_ioctl_get_wifi_priv_feature_cap_param::pst_hmac_vap is null.}");
        return OAL_SUCC;
    }

#if defined(_PRE_WLAN_FEATURE_11K) || defined(_PRE_WLAN_FEATURE_11R) || defined(_PRE_WLAN_FEATURE_11K_EXTERN)
    /*11k????*/
    if(OAL_TRUE == pst_hmac_vap->bit_11k_enable)
    {
        ul_value |=  BIT(WAL_WIFI_FEATURE_SUPPORT_11K);
    }

    /*11v????*/
    if(OAL_TRUE == pst_hmac_vap->bit_11v_enable)
    {
        ul_value |= BIT(WAL_WIFI_FEATURE_SUPPORT_11V);
    }

    if(OAL_TRUE == pst_hmac_vap->bit_11r_enable)
    {
        ul_value |= BIT(WAL_WIFI_FEATURE_SUPPORT_11R);
    }
#endif
    ul_value     |= BIT(WAL_WIFI_FEATURE_SUPPORT_VOWIFI_NAT_KEEP_ALIVE);

    *pl_value = ul_value;

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_IP_FILTER

oal_int32 wal_set_assigned_filter_enable_etc(oal_int32 l_filter_id,oal_int32 l_on)
{
    oal_uint16   us_len;
    oal_int32    l_ret;
    mac_vap_stru               *pst_mac_vap = OAL_PTR_NULL;
    oal_net_device_stru        *pst_net_dev = OAL_PTR_NULL;
    wal_msg_write_stru          st_write_msg;
    mac_assigned_filter_cmd_stru      st_assigned_filter_cmd;

#ifdef _PRE_WLAN_FEATURE_DFR
    if (g_st_dfr_info_etc.bit_device_reset_process_flag)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_set_assigned_filter_enable_etc:: dfr_process_status[%d]!}",
            g_st_dfr_info_etc.bit_device_reset_process_flag);
        return -OAL_EFAIL;
    }
#endif //#ifdef _PRE_WLAN_FEATURE_DFR

    if (l_on < 0)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_set_assigned_filter_enable_etc::Invalid input parameter, on/off %d!}", l_on);
        return -OAL_EINVAL;
    }

    pst_net_dev = oal_dev_get_by_name("wlan0");
    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_set_assigned_filter_enable_etc::wlan0 not exist!}");
        return -OAL_EINVAL;
    }
    /* ????oal_dev_get_by_name????????????oal_dev_put??net_dev?????????????? */
    oal_dev_put(pst_net_dev);

    /* vap?????????????????????????? */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_set_assigned_filter_enable_etc::vap not created yet, ignore the cmd!}");
        return -OAL_EINVAL;
    }

    if (OAL_TRUE != pst_mac_vap->st_cap_flag.bit_icmp_filter)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_assigned_filter_enable_etc::Func not enable, ignore the cmd!}");
        return -OAL_EINVAL;
    }

    /* ???????????? */
    us_len = OAL_SIZEOF(st_assigned_filter_cmd);
    memset_s((oal_uint8 *)&st_assigned_filter_cmd, us_len, 0, us_len);
    st_assigned_filter_cmd.en_filter_id  = (mac_assigned_filter_id_enum)l_filter_id;
    st_assigned_filter_cmd.en_enable     = (l_on > 0)? OAL_TRUE : OAL_FALSE;

    OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_assigned_filter_enable_etc::assigned_filter on/off(%d).}",
                        st_assigned_filter_cmd.en_enable);

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/

    /* ???? msg ??????*/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_ASSIGNED_FILTER, us_len);

    /* ????????netbuf????????????msg???????? */
    l_ret = memcpy_s(st_write_msg.auc_value, us_len, (oal_uint8 *)&st_assigned_filter_cmd, us_len);
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_set_assigned_filter_enable_etc::memcpy fail!");
        return OAL_FAIL;
    }

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_assigned_filter_enable_etc::wal_send_cfg_event_etc failed, error no[%d]!}", l_ret);
        return l_ret;
    }

    return OAL_SUCC;
}


oal_int32 wal_set_ip_filter_enable_etc(oal_int32 l_on)
{
    oal_uint16   us_len;
    oal_int32    l_ret;
    oal_uint32   ul_netbuf_len;
    oal_netbuf_stru            *pst_netbuf = OAL_PTR_NULL;
    mac_vap_stru               *pst_mac_vap = OAL_PTR_NULL;
    oal_net_device_stru        *pst_net_dev = OAL_PTR_NULL;
    wal_msg_write_stru          st_write_msg;
    mac_ip_filter_cmd_stru      st_ip_filter_cmd;
    mac_ip_filter_cmd_stru     *pst_cmd_info = OAL_PTR_NULL;

#ifdef _PRE_WLAN_FEATURE_DFR
    if (g_st_dfr_info_etc.bit_device_reset_process_flag)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_set_ip_filter_enable_etc:: dfr_process_status[%d]!}",
            g_st_dfr_info_etc.bit_device_reset_process_flag);
        return -OAL_EFAIL;
    }
#endif //#ifdef _PRE_WLAN_FEATURE_DFR

    if (l_on < 0)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{wal_set_ip_filter_enable_etc::Invalid input parameter, on/off %d!}", l_on);
        return -OAL_EINVAL;
    }

    pst_net_dev = oal_dev_get_by_name("wlan0");
    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_set_ip_filter_enable_etc::wlan0 not exist!}");
        return -OAL_EINVAL;
    }
    /* ????oal_dev_get_by_name????????????oal_dev_put??net_dev?????????????? */
    oal_dev_put(pst_net_dev);

    /* vap?????????????????????????? */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_set_ip_filter_enable_etc::vap not created yet, ignore the cmd!}");
        return -OAL_EINVAL;
    }

    if (OAL_TRUE != pst_mac_vap->st_cap_flag.bit_ip_filter)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_ip_filter_enable_etc::Func not enable, ignore the cmd!}");
        return -OAL_EINVAL;
    }

    /* ???????????? */
    ul_netbuf_len = OAL_SIZEOF(st_ip_filter_cmd);
    memset_s((oal_uint8 *)&st_ip_filter_cmd, ul_netbuf_len, 0, ul_netbuf_len);
    st_ip_filter_cmd.en_cmd        = MAC_IP_FILTER_ENABLE;
    st_ip_filter_cmd.en_enable     = (l_on > 0)? OAL_TRUE : OAL_FALSE;

    OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_ip_filter_enable_etc::IP_filter on/off(%d).}",
                        st_ip_filter_cmd.en_enable);

    /* ???????? ???????????? */
    pst_netbuf = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF,ul_netbuf_len, OAL_NETBUF_PRIORITY_MID);
    if (OAL_PTR_NULL == pst_netbuf)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_ip_filter_enable_etc::netbuf alloc null,size %d.}", ul_netbuf_len);
        return -OAL_EINVAL;
    }
    memset_s(((oal_uint8 *)OAL_NETBUF_DATA(pst_netbuf)), ul_netbuf_len, 0, ul_netbuf_len);
    pst_cmd_info = (mac_ip_filter_cmd_stru *)OAL_NETBUF_DATA(pst_netbuf);

    /* ???????????? */
    l_ret = memcpy_s((oal_uint8 *)pst_cmd_info, ul_netbuf_len, (oal_uint8 *)(&st_ip_filter_cmd), ul_netbuf_len);
    oal_netbuf_put(pst_netbuf, ul_netbuf_len);

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    us_len = OAL_SIZEOF(oal_netbuf_stru *);

    /* ???? msg ??????*/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_IP_FILTER, us_len);

    /* ????????netbuf????????????msg???????? */
    l_ret += memcpy_s(st_write_msg.auc_value, us_len, (oal_uint8 *)&pst_netbuf, us_len);
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_set_ip_filter_enable_etc::memcpy fail!");
        oal_netbuf_free(pst_netbuf);
        return -OAL_EINVAL;
    }

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_set_ip_filter_enable_etc::wal_send_cfg_event_etc failed, error no[%d]!}", l_ret);
        oal_netbuf_free(pst_netbuf);
        return l_ret;
    }

    return OAL_SUCC;
}

oal_int32 wal_add_ip_filter_items_etc(wal_hw_wifi_filter_item *pst_items, oal_int32 l_count)
{
    oal_uint16   us_len;
    oal_int32    l_ret;
    oal_uint32   ul_netbuf_len;
    oal_uint32   ul_items_idx;
    oal_netbuf_stru            *pst_netbuf = OAL_PTR_NULL;
    mac_vap_stru               *pst_mac_vap = OAL_PTR_NULL;
    oal_net_device_stru        *pst_net_dev = OAL_PTR_NULL;
    wal_msg_write_stru          st_write_msg;
    mac_ip_filter_cmd_stru      st_ip_filter_cmd;
    mac_ip_filter_cmd_stru      *pst_cmd_info = OAL_PTR_NULL;

#ifdef _PRE_WLAN_FEATURE_DFR
    if (g_st_dfr_info_etc.bit_device_reset_process_flag)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_add_ip_filter_items_etc:: dfr_process_status[%d]!}",
            g_st_dfr_info_etc.bit_device_reset_process_flag);
        return -OAL_EFAIL;
    }
#endif //#ifdef _PRE_WLAN_FEATURE_DFR

    if ((OAL_PTR_NULL == pst_items)||(l_count <= 0))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{wal_add_ip_filter_items_etc::Invalid input parameter, pst_items %p, l_count %d!}", (uintptr_t)pst_items,l_count);
        return -OAL_EINVAL;
    }

    pst_net_dev = oal_dev_get_by_name("wlan0");
    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_add_ip_filter_items_etc::wlan0 not exist!}");
        return -OAL_EINVAL;
    }
    /* ????oal_dev_get_by_name????????????oal_dev_put??net_dev?????????????? */
    oal_dev_put(pst_net_dev);

    /* vap?????????????????????????? */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_add_ip_filter_items_etc::vap not created yet, ignore the cmd!.}");
        return -OAL_EINVAL;
    }

    if (OAL_TRUE != pst_mac_vap->st_cap_flag.bit_ip_filter)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_add_ip_filter_items_etc::Func not enable, ignore the cmd!.}");
        return -OAL_EINVAL;
    }

    /* ???????????? */
    memset_s((oal_uint8 *)&st_ip_filter_cmd, OAL_SIZEOF(st_ip_filter_cmd), 0, OAL_SIZEOF(st_ip_filter_cmd));
    st_ip_filter_cmd.en_cmd    = MAC_IP_FILTER_UPDATE_BTABLE;

    /* ???????????????????????????????????????????????? */
    st_ip_filter_cmd.uc_item_count = OAL_MIN((MAC_MAX_IP_FILTER_BTABLE_SIZE / OAL_SIZEOF(mac_ip_filter_item_stru)), l_count);
    if (st_ip_filter_cmd.uc_item_count < l_count)
    {
       OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_add_ip_filter_items_etc::Btable(%d) is too small to store %d items!}",
                        st_ip_filter_cmd.uc_item_count,
                        l_count);
    }

    OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_add_ip_filter_items_etc::Start updating btable, items_cnt(%d).}",
                        st_ip_filter_cmd.uc_item_count);

    /* ?????????????????????? */
    ul_netbuf_len = (st_ip_filter_cmd.uc_item_count * OAL_SIZEOF(mac_ip_filter_item_stru)) + OAL_SIZEOF(st_ip_filter_cmd);


    /* ???????? ???????????? */
    pst_netbuf = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF,ul_netbuf_len, OAL_NETBUF_PRIORITY_MID);
    if (OAL_PTR_NULL == pst_netbuf)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_add_ip_filter_items_etc::netbuf alloc null,size %d.}", ul_netbuf_len);
        return -OAL_EINVAL;
    }
    memset_s(((oal_uint8 *)OAL_NETBUF_DATA(pst_netbuf)), ul_netbuf_len, 0, ul_netbuf_len);
    pst_cmd_info = (mac_ip_filter_cmd_stru *)OAL_NETBUF_DATA(pst_netbuf);

    /* ???????????? */
    l_ret = memcpy_s((oal_uint8 *)pst_cmd_info, ul_netbuf_len,
                     (oal_uint8 *)(&st_ip_filter_cmd), OAL_SIZEOF(st_ip_filter_cmd));
    oal_netbuf_put(pst_netbuf, ul_netbuf_len);

    for(ul_items_idx = 0; ul_items_idx < st_ip_filter_cmd.uc_item_count; ul_items_idx++)
    {
        pst_cmd_info->ast_filter_items[ul_items_idx].uc_protocol = (oal_uint8)pst_items[ul_items_idx].protocol;
        pst_cmd_info->ast_filter_items[ul_items_idx].us_port     = (oal_uint16)pst_items[ul_items_idx].port;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    us_len = OAL_SIZEOF(oal_netbuf_stru *);

    /* ???? msg ??????*/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_IP_FILTER, us_len);

    /* ????????netbuf????????????msg???????? */
    l_ret += memcpy_s(st_write_msg.auc_value, us_len, (oal_uint8 *)&pst_netbuf, us_len);
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "hwifi_config_init_fcc_ce_txpwr_nvram::memcpy fail!");
        oal_netbuf_free(pst_netbuf);
        return -OAL_EINVAL;
    }

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_add_ip_filter_items_etc::wal_send_cfg_event_etc failed, error no[%d]!}", l_ret);
        oal_netbuf_free(pst_netbuf);
        return l_ret;
    }


    return OAL_SUCC;

}

oal_int32 wal_clear_ip_filter_etc()
{
    oal_uint16   us_len;
    oal_int32    l_ret;
    oal_uint32   ul_netbuf_len;
    oal_netbuf_stru            *pst_netbuf = OAL_PTR_NULL;
    mac_vap_stru               *pst_mac_vap = OAL_PTR_NULL;
    oal_net_device_stru        *pst_net_dev;
    wal_msg_write_stru          st_write_msg;
    mac_ip_filter_cmd_stru      st_ip_filter_cmd;
    mac_ip_filter_cmd_stru     *pst_cmd_info = OAL_PTR_NULL;

#ifdef _PRE_WLAN_FEATURE_DFR
    if (g_st_dfr_info_etc.bit_device_reset_process_flag)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_clear_ip_filter_etc:: dfr_process_status[%d]!}",
            g_st_dfr_info_etc.bit_device_reset_process_flag);
        return -OAL_EFAIL;
    }
#endif //#ifdef _PRE_WLAN_FEATURE_DFR

    pst_net_dev = oal_dev_get_by_name("wlan0");
    if (OAL_PTR_NULL == pst_net_dev)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_clear_ip_filter_etc::wlan0 not exist!}");
        return -OAL_EINVAL;
    }

    /* ????oal_dev_get_by_name????????????oal_dev_put??net_dev?????????????? */
    oal_dev_put(pst_net_dev);

    /* vap?????????????????????????? */
    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_clear_ip_filter_etc::vap not created yet, ignore the cmd!.}");
        return -OAL_EINVAL;
    }

    if (OAL_TRUE != pst_mac_vap->st_cap_flag.bit_ip_filter)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_clear_ip_filter_etc::Func not enable, ignore the cmd!.}");
        return -OAL_EINVAL;
    }

    /* ?????????? */
    memset_s((oal_uint8 *)&st_ip_filter_cmd, OAL_SIZEOF(st_ip_filter_cmd), 0, OAL_SIZEOF(st_ip_filter_cmd));
    st_ip_filter_cmd.en_cmd        = MAC_IP_FILTER_CLEAR;

    OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_clear_ip_filter_etc::Now start clearing the list.}");

    /* ?????????????????????? */
    ul_netbuf_len = OAL_SIZEOF(st_ip_filter_cmd);

    /* ???????? ???????????? */
    pst_netbuf = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF,ul_netbuf_len, OAL_NETBUF_PRIORITY_MID);
    if (OAL_PTR_NULL == pst_netbuf)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_clear_ip_filter_etc::netbuf alloc null,size %d.}", ul_netbuf_len);
        return -OAL_EINVAL;
    }
    memset_s(((oal_uint8 *)OAL_NETBUF_DATA(pst_netbuf)), ul_netbuf_len, 0, ul_netbuf_len);
    pst_cmd_info = (mac_ip_filter_cmd_stru *)OAL_NETBUF_DATA(pst_netbuf);

    /* ???????????? */
    l_ret = memcpy_s((oal_uint8 *)pst_cmd_info, ul_netbuf_len, (oal_uint8 *)(&st_ip_filter_cmd), ul_netbuf_len);
    oal_netbuf_put(pst_netbuf, ul_netbuf_len);

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    us_len = OAL_SIZEOF(oal_netbuf_stru *);

    /* ???? msg ??????*/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_IP_FILTER, us_len);

    /* ????????netbuf????????????msg???????? */
    l_ret += memcpy_s(st_write_msg.auc_value, us_len, (oal_uint8 *)&pst_netbuf, us_len);
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_clear_ip_filter_etc::memcpy fail!");
        oal_netbuf_free(pst_netbuf);
        return -OAL_EINVAL;
    }

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + us_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{wal_clear_ip_filter_etc::wal_send_cfg_event_etc failed, error no[%d]!}", l_ret);
        oal_netbuf_free(pst_netbuf);
        return l_ret;
    }

    return OAL_SUCC;
}
oal_int32 wal_register_ip_filter_etc(wal_hw_wlan_filter_ops *pst_ip_filter_ops)
{
#ifdef CONFIG_DOZE_FILTER
    if (OAL_PTR_NULL == pst_ip_filter_ops)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_register_ip_filter_etc::pg_st_ip_filter_ops is null !}");
        return -OAL_EINVAL;
    }
    hw_register_wlan_filter(pst_ip_filter_ops);
#else
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_register_ip_filter_etc:: Not support CONFIG_DOZE_FILTER!}");
#endif
    return OAL_SUCC;
}

oal_int32 wal_unregister_ip_filter_etc()
{
#ifdef CONFIG_DOZE_FILTER
    hw_unregister_wlan_filter();
#else
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_unregister_ip_filter_etc:: Not support CONFIG_DOZE_FILTER!}");
#endif
    return OAL_SUCC;
}

#else
oal_int32 wal_set_assigned_filter_enable_etc(oal_int32 l_filter_id,oal_int32 l_on)
{
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_set_assigned_filter_enable_etc::Assigned_filter not support!}");
    return -OAL_EFAIL;
}

oal_int32 wal_set_ip_filter_enable_etc(oal_int32 l_on)
{
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_set_ip_filter_enable_etc::Ip_filter not support!}");
    return -OAL_EFAIL;
}
oal_int32 wal_add_ip_filter_items_etc(wal_hw_wifi_filter_item *pst_items, oal_int32 l_count)
{
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_add_ip_filter_items_etc::Ip_filter not support!}");
    return -OAL_EFAIL;
}

oal_int32 wal_clear_ip_filter_etc()
{
    OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_clear_ip_filter_etc::Ip_filter not support!}");

    return -OAL_EFAIL;
}


#endif //_PRE_WLAN_FEATURE_IP_FILTER

/*lint -e19*/
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,44))
oal_module_symbol(wal_hipriv_proc_write);
#endif
oal_module_symbol(wal_hipriv_get_mac_addr_etc);
/*lint +e19*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#if (defined _PRE_WLAN_RF_CALI) || (defined _PRE_WLAN_RF_CALI_1151V2)

OAL_STATIC oal_uint32  wal_hipriv_auto_cali(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;
    //oal_uint16                      us_len;
    oal_int8                        ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint8                       uc_cali_type;
    oal_uint32                      ul_ret;
    oal_uint32                      ul_off_set;
    wal_msg_stru                    *pst_rsp_msg = OAL_PTR_NULL;
    mac_vap_stru                    *pst_mac_vap;
    mac_device_stru                 *pst_mac_device;
    oal_uint8                       uc_vap_idx;
    mac_vap_stru                    *pst_vap;

    if (OAL_UNLIKELY(WAL_MSG_WRITE_MAX_LEN <= OAL_STRLEN(pc_param)))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_auto_cali:: pc_param overlength is %d}\n", OAL_STRLEN(pc_param));
        return OAL_FAIL;
    }

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(OAL_UNLIKELY(NULL == pst_mac_vap))
    {
        OAM_ERROR_LOG0(0, OAM_SF_CFG, "{wal_hipriv_auto_cali::can't get mac vap from netdevice priv data!}");
        return OAL_EINVAL;
    }

    pst_mac_device = mac_res_get_dev_etc(pst_mac_vap->uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_SCAN, "{wal_hipriv_auto_cali::mac_res_get_dev_etc fail.device_id = %u}",
                           pst_mac_vap->uc_device_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ????device??????vap */
    for (uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++)
    {
        pst_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
        if (OAL_PTR_NULL == pst_vap)
        {
            OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_SCAN, "{wal_hipriv_auto_cali::pst_mac_vap(%d) null.}",
                           pst_mac_device->auc_vap_id[uc_vap_idx]);
            continue;
        }
        /* ??????up??????????????????????down */
        if (pst_vap->en_vap_state != MAC_VAP_STATE_INIT)
        {
            OAM_ERROR_LOG2(pst_vap->uc_vap_id, OAM_SF_ANY, "{wal_hipriv_auto_cali::device is busy, please down it first, vap id: %d, state: %d!}\r\n",
                pst_vap->uc_vap_id, pst_vap->en_vap_state);
            return OAL_EBUSY;
        }
    }

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        uc_cali_type = 0xff;
    }
    else
    {
        uc_cali_type = (oal_uint8)oal_atoi(ac_arg);
    }
    *(st_write_msg.auc_value) = uc_cali_type;
    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_auto_cali::cali type = %x!}\r\n", uc_cali_type);

    /***************************************************************************
                              ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_AUTO_CALI, OAL_SIZEOF(uc_cali_type));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                             WAL_MSG_TYPE_WRITE,
                             WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(uc_cali_type),
                             (oal_uint8 *)&st_write_msg,
                             OAL_TRUE,
                             &pst_rsp_msg);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_auto_cali::return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

        /* ???????????????? */
    ul_ret = wal_check_and_release_msg_resp_etc(pst_rsp_msg);
    if(OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_DFR, "{wal_hipriv_auto_cali fail, err code[%u]!}\r\n", ul_ret);
        return ul_ret;
    }

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_auto_cali::wal_send_cfg_event_etc return err_code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}


OAL_STATIC oal_uint32  wal_hipriv_set_cali_vref(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru          st_write_msg;
    oal_int32                   l_ret;
    oal_uint32                  ul_off_set;
    oal_int8                    ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                  ul_ret;
    oal_uint8                   uc_chain_idx;
    oal_uint8                   uc_band_idx;
    oal_uint16                  us_vref_value;
    mac_cfg_set_cali_vref_stru *pst_cali_vref;

    /* ???????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_cali_ref::wal_get_cmd_one_arg_etc vap name return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }
    uc_chain_idx = (oal_uint8)oal_atoi(ac_arg);

    /* ????????vref idx */
    pc_param = pc_param + ul_off_set;
    ul_ret   = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_cali_ref::wal_get_cmd_one_arg_etc vap name return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }
    uc_band_idx = (oal_uint8)oal_atoi(ac_arg);

    /* ????????vref?? */
    pc_param = pc_param + ul_off_set;
    ul_ret   = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_cali_ref::wal_get_cmd_one_arg_etc vap name return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }

    us_vref_value = (oal_uint16)oal_atoi(ac_arg);

    OAL_IO_PRINT("chain(%d):us_cali_ref(%d) = %u.\r\n",uc_chain_idx,uc_band_idx, us_vref_value);

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_CALI_VREF, OAL_SIZEOF(mac_cfg_set_cali_vref_stru));

    /* ???????????????? */
    pst_cali_vref = (mac_cfg_set_cali_vref_stru *)(st_write_msg.auc_value);
    pst_cali_vref->uc_chain_idx = uc_chain_idx;
    pst_cali_vref->uc_band_idx  = uc_band_idx;
    pst_cali_vref->us_vref_value = us_vref_value;

    /***************************************************************************
                                    ????????wal??????
    ***************************************************************************/

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_set_cali_vref_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_cali_ref::return err code %d!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    OAL_IO_PRINT("wal_hipriv_set_cali_ref:OAL_SUCC.\r\n");

    return OAL_SUCC;
}
#endif
#ifdef _PRE_WLAN_FEATURE_SINGLE_CHIP_DUAL_BAND

OAL_STATIC oal_uint32  wal_hipriv_set_restrict_band(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru   st_write_msg;
    oal_int32            l_ret;
    oal_int8             ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32           ul_offset;
    oal_uint32           ul_ret;
    oal_uint8            uc_val;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_offset);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_restrict_band::need argument}");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    uc_val = (oal_uint8)oal_atoi(ac_arg);

    if ((uc_val != WLAN_BAND_2G) && (uc_val != WLAN_BAND_5G) && (uc_val != WLAN_BAND_BUTT))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_restrict_band::invalid argument=%d", uc_val);
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    st_write_msg.auc_value[0] = uc_val;

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_RESTRICT_BAND, OAL_SIZEOF(oal_uint8));
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_restrict_band::wal_send_cfg_event_etc failed}");
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif
#if defined(_PRE_WLAN_FEATURE_DBAC) && defined(_PRE_WLAN_FEATRUE_DBAC_DOUBLE_AP_MODE)

OAL_STATIC oal_uint32  wal_hipriv_set_omit_acs(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru   st_write_msg;
    oal_int32            l_ret;
    oal_int8             ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32           ul_offset;
    oal_uint8            uc_val;
    oal_uint32           ul_ret;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_offset);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_omit_acs::need argument}");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    uc_val = (oal_uint8)oal_atoi(ac_arg);

    if ((uc_val != OAL_TRUE) && (uc_val != OAL_FALSE))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_set_omit_acs::invalid argument=%d", uc_val);

        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    st_write_msg.auc_value[0] = uc_val;

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_OMIT_ACS, OAL_SIZEOF(oal_uint8));
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_hipriv_set_omit_acs::wal_send_cfg_event_etc failed}");
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_WDS

OAL_STATIC oal_uint32  wal_hipriv_wds_vap_mode(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret = OAL_SUCC;
    oal_uint8                       uc_wds_vap_mode = 0;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_wds_vap_mode::wal_get_cmd_one_arg_etc return err_code %d!}\r\n", ul_ret);
         return ul_ret;
    }

    uc_wds_vap_mode = (oal_uint8)oal_atoi(ac_name);

    if ( WDS_MODE_BUTT <= uc_wds_vap_mode )
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_hipriv_wds_vap_mode::invalid parameter.}\r\n");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_WDS_VAP_MODE, OAL_SIZEOF(oal_uint8));
    *((oal_uint8 *)(st_write_msg.auc_value)) = uc_wds_vap_mode;  /* ????WDS???? */

    ul_ret = (oal_uint32)wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_wds_vap_mode::return err code %d!}\r\n", ul_ret);
    }

    return ul_ret;
}


OAL_STATIC oal_uint32  wal_hipriv_wds_vap_show(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_ret = OAL_SUCC;


    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_WDS_VAP_SHOW, OAL_SIZEOF(oal_uint8));

    ul_ret = (oal_uint32)wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_wds_vap_show::return err code %d!}\r\n", ul_ret);
    }

    return ul_ret;
}


OAL_STATIC oal_uint32  wal_hipriv_wds_sta_add(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret = OAL_SUCC;
    mac_cfg_wds_sta_stru            *pst_wds_sta = OAL_PTR_NULL;
    mac_cfg_wds_sta_stru            st_wds_sta;      /* ??????????????use?????? */

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_wds_sta_add::wal_get_cmd_one_arg_etc 1 return err_code %d!}\r\n", ul_ret);
         return ul_ret;
    }
    oal_strtoaddr(ac_name, OAL_SIZEOF(ac_name), st_wds_sta.auc_sta_mac, WLAN_MAC_ADDR_LEN);

    /* ?????????????????? */
    pc_param = pc_param + ul_off_set;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_wds_sta_add::wal_get_cmd_one_arg_etc 2 return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }

    oal_strtoaddr(ac_name, OAL_SIZEOF(ac_name), st_wds_sta.auc_node_mac, WLAN_MAC_ADDR_LEN);

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_WDS_STA_ADD, OAL_SIZEOF(mac_cfg_wds_sta_stru));

    /* ???????????????? */
    pst_wds_sta     = (mac_cfg_wds_sta_stru *)(st_write_msg.auc_value);
    oal_set_mac_addr(pst_wds_sta->auc_sta_mac, st_wds_sta.auc_sta_mac);
    oal_set_mac_addr(pst_wds_sta->auc_node_mac, st_wds_sta.auc_node_mac);

    ul_ret = (oal_uint32)wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_wds_sta_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_wds_sta_add::return err code %d!}\r\n", ul_ret);
    }

    return ul_ret;
}


OAL_STATIC oal_uint32  wal_hipriv_wds_sta_del(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret = OAL_SUCC;
    mac_cfg_wds_sta_stru            *pst_wds_sta = OAL_PTR_NULL;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_wds_sta_del::wal_get_cmd_one_arg_etc return err_code %d!}\r\n", ul_ret);
         return ul_ret;
    }

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_WDS_STA_DEL, OAL_SIZEOF(mac_cfg_wds_sta_stru));

    /* ???????????????? */
    pst_wds_sta     = (mac_cfg_wds_sta_stru *)(st_write_msg.auc_value);
    oal_strtoaddr(ac_name, OAL_SIZEOF(ac_name), pst_wds_sta->auc_sta_mac, WLAN_MAC_ADDR_LEN);
    oal_set_mac_addr_zero(pst_wds_sta->auc_node_mac);

    ul_ret = (oal_uint32)wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_wds_sta_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_wds_sta_del::return err code %d!}\r\n", ul_ret);
    }

    return ul_ret;
}


OAL_STATIC oal_uint32  wal_hipriv_wds_sta_age(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret = OAL_SUCC;
    oal_uint32                      ul_sta_aging = 0;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_wds_sta_age::wal_get_cmd_one_arg_etc return err_code %d!}\r\n", ul_ret);
         return ul_ret;
    }

    ul_sta_aging = (oal_uint32)oal_atoi(ac_name);

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_WDS_STA_AGE, OAL_SIZEOF(oal_uint8));
    *((oal_uint32 *)(st_write_msg.auc_value)) = ul_sta_aging;  /* ????WDS???? */

    ul_ret = (oal_uint32)wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_wds_sta_age::return err code %d!}\r\n", ul_ret);
    }

    return ul_ret;
}

#endif


OAL_STATIC oal_uint32  wal_hipriv_rssi_limit(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret;
    oal_int32                       l_ret;
    mac_cfg_rssi_limit_stru        *pst_rssi_limit;
    oal_uint8                       uc_idx;

    /* ????????????????,???????? */
    pst_rssi_limit = (mac_cfg_rssi_limit_stru *)(st_write_msg.auc_value);
    memset_s((oal_void *)pst_rssi_limit, OAL_SIZEOF(mac_cfg_rssi_limit_stru),
             0, OAL_SIZEOF(mac_cfg_rssi_limit_stru));

    //????rssi_limit ????
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    pc_param = pc_param + ul_off_set;
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_rssi_limit::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    //??????????????????????????????????????
    for (uc_idx = 0; uc_idx < MAC_RSSI_LIMIT_TYPE_BUTT; uc_idx++)
    {
        if(0 == oal_strcmp(ac_name, g_ast_mac_rssi_config_table[uc_idx].puc_car_name))
        {
            break;
        }
    }
    if (MAC_RSSI_LIMIT_TYPE_BUTT == uc_idx)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CAR, "{wal_hipriv_rssi_limit:: parameter error !\r\n");
        return uc_idx;
    }
    pst_rssi_limit->en_rssi_limit_type = g_ast_mac_rssi_config_table[uc_idx].en_rssi_cfg_id;

    //????????????????????
    if (MAC_RSSI_LIMIT_SHOW_INFO != pst_rssi_limit->en_rssi_limit_type)
    {
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        pc_param = pc_param + ul_off_set;
        if (OAL_SUCC != ul_ret)
        {
             OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_rssi_limit::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
             return ul_ret;
        }
    }

    switch (pst_rssi_limit->en_rssi_limit_type)
    {
        case MAC_RSSI_LIMIT_SHOW_INFO: break;
        case MAC_RSSI_LIMIT_DELTA:
            pst_rssi_limit->c_rssi_delta = (oal_int8)oal_atoi(ac_name); break;
        case MAC_RSSI_LIMIT_ENABLE:
            pst_rssi_limit->en_rssi_limit_enable_flag = (oal_bool_enum_uint8)oal_atoi(ac_name); break;
        case MAC_RSSI_LIMIT_THRESHOLD:
            pst_rssi_limit->c_rssi = (oal_int8)oal_atoi(ac_name);
            /*????????rssi??????????????????????????????*/
            if ((pst_rssi_limit->c_rssi < OAL_RSSI_SIGNAL_MIN) || (pst_rssi_limit->c_rssi > OAL_RSSI_SIGNAL_MAX))
            {
                OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_rssi_limit::rssi limit error data[%d]!}\r\n", (oal_int32)pst_rssi_limit->c_rssi);
                return OAL_FAIL;
            }

            if (pst_rssi_limit->c_rssi > WLAN_FAR_DISTANCE_RSSI)
            {
                OAM_WARNING_LOG2(0, OAM_SF_ANY, "{wal_hipriv_rssi_limit::rssi_limit exceed WLAN_FAR_DISTANCE_RSSI! s_rssi[%d], WLAN_FAR_DISTANCE_RSSI[%d]}\r\n",
                    (oal_int32)pst_rssi_limit->c_rssi, WLAN_FAR_DISTANCE_RSSI);
            }
            break;
        default:
            break;
    }

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_RSSI_LIMIT_CFG, OAL_SIZEOF(mac_cfg_rssi_limit_stru));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_rssi_limit_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_rssi_limit::return err code [%d]!}\r\n", ul_ret);
        return (oal_uint32)ul_ret;
    }

    return OAL_SUCC;

}

#ifdef _PRE_WLAN_REPORT_PRODUCT_LOG

OAL_STATIC oal_uint32  wal_hipriv_report_product_log_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret;
    oal_uint8                       uc_flag;
    oal_uint8                      *pst_product_log_param;
    oal_int32                       l_ret;

    /* ???????????????? */
    pst_product_log_param = (oal_uint8 *)(st_write_msg.auc_value);
    memset_s((oal_void *)pst_product_log_param, OAL_SIZEOF(oal_uint8), 0, OAL_SIZEOF(oal_uint8));

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    pc_param = pc_param + ul_off_set;
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_report_product_log_cfg::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    uc_flag = (oal_uint8)oal_atoi(ac_name);

    // 0 ??1 ????????; 2 ??????????
    if ((0 != uc_flag ) && (1 != uc_flag ) && (2 != uc_flag ))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_report_product_log_cfg:: input invalid[%d]}\r\n",uc_flag);
        return OAL_FAIL;
    }

    *pst_product_log_param = uc_flag;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_report_product_log_cfg:: param=%d}\r\n", *pst_product_log_param);

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_PRODUCT_LOG, OAL_SIZEOF(oal_uint8));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_report_product_log_cfg::return err code [%d]!}\r\n", ul_ret);
        return (oal_uint32)ul_ret;
    }

    return OAL_SUCC;

}
#endif

#ifdef _PRE_WLAN_FEATURE_TCP_ACK_BUFFER
OAL_STATIC oal_uint32  wal_hipriv_tcp_ack_buf_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret;
    oal_int32                       l_ret;
    oal_int32                       l_tmp;
    oal_uint8                       uc_idx;
    mac_cfg_tcp_ack_buf_stru        *pst_tcp_ack_param;

    pst_tcp_ack_param = (mac_cfg_tcp_ack_buf_stru *)(st_write_msg.auc_value);
    memset_s((oal_void *)pst_tcp_ack_param, OAL_SIZEOF(mac_cfg_tcp_ack_buf_stru),
             0, OAL_SIZEOF(mac_cfg_tcp_ack_buf_stru));

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_tcp_ack_buf_cfg::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }
    pc_param = pc_param + ul_off_set;

    for (uc_idx = 0; uc_idx < MAC_TCP_ACK_BUF_TYPE_BUTT; uc_idx++)
    {
        if(0 == oal_strcmp(ac_name, g_ast_hmac_tcp_ack_buf_cfg_table[uc_idx].puc_string))
        {
            break;
        }
    }
    if (MAC_TCP_ACK_BUF_TYPE_BUTT == uc_idx)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_hipriv_tcp_ack_buf_cfg:: parameter error !\r\n");
        return uc_idx;
    }
    pst_tcp_ack_param->en_cmd = g_ast_hmac_tcp_ack_buf_cfg_table[uc_idx].en_tcp_ack_buf_cfg_id;

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_tcp_ack_buf_cfg::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    l_tmp = (oal_int32)oal_atoi(ac_name);
    if ((l_tmp < 0) || (l_tmp > 10*1000))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_tcp_ack_buf_cfg::  car param[%d] invalid! }\r\n", oal_atoi(ac_name));
        return OAL_FAIL;
    }

    if (MAC_TCP_ACK_BUF_ENABLE == pst_tcp_ack_param->en_cmd)
    {
        if ((OAL_FALSE == (oal_uint8)l_tmp) || (OAL_TRUE == (oal_uint8)l_tmp))
        {
            pst_tcp_ack_param->en_enable = (oal_uint8)l_tmp;
        }
        else
        {
            OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_tcp_ack_buf_cfg:: en_fast_aging_flag[%d] must be 0 or 1.}", (oal_uint8)l_tmp);
            return OAL_FAIL;
        }

    }
    if (MAC_TCP_ACK_BUF_TIMEOUT == pst_tcp_ack_param->en_cmd)
    {
        if (0 == (oal_uint8)l_tmp)
        {
            OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_hipriv_tcp_ack_buf_cfg:: timer_ms shoule not be 0.}");
            return OAL_FAIL;
        }

        pst_tcp_ack_param->uc_timeout_ms = (oal_uint8)l_tmp;
    }
    if (MAC_TCP_ACK_BUF_MAX == pst_tcp_ack_param->en_cmd)
    {
        if (0 == (oal_uint8)l_tmp)
        {
            OAM_WARNING_LOG0(0, OAM_SF_CFG, "{wal_hipriv_tcp_ack_buf_cfg:: uc_count_limit shoule not be 0.}");
            return OAL_FAIL;
        }

        pst_tcp_ack_param->uc_count_limit = (oal_uint8)l_tmp;
    }

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_TCP_ACK_BUF, OAL_SIZEOF(mac_cfg_tcp_ack_buf_stru));

    OAM_WARNING_LOG4(0, OAM_SF_CFG, "{wal_hipriv_tcp_ack_buf_cfg::en_cmd[%d], en_enable[%d], uc_timeout_ms[%d] uc_count_limit[%d]!}\r\n",
        pst_tcp_ack_param->en_cmd,
        pst_tcp_ack_param->en_enable,
        pst_tcp_ack_param->uc_timeout_ms,
        pst_tcp_ack_param->uc_count_limit);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_tcp_ack_buf_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_tcp_ack_buf_cfg::return err code [%d]!}\r\n", ul_ret);
        return (oal_uint32)ul_ret;
    }

    return OAL_SUCC;


}
#endif


#ifdef _PRE_FEATURE_FAST_AGING
OAL_STATIC oal_uint32  wal_hipriv_fast_aging_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret;
    oal_int32                       l_ret;
    oal_int32                       l_tmp;
    oal_uint8                       uc_idx;
    mac_cfg_fast_aging_stru        *pst_aging_cfg_param;

    /* ???????????????? */
    pst_aging_cfg_param = (mac_cfg_fast_aging_stru *)(st_write_msg.auc_value);
    memset_s((oal_void *)pst_aging_cfg_param, OAL_SIZEOF(mac_cfg_fast_aging_stru),
             0, OAL_SIZEOF(mac_cfg_fast_aging_stru));

    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    pc_param = pc_param + ul_off_set;

    //??????????????????????????????????
    for (uc_idx = 0; uc_idx < MAC_FAST_AGING_TYPE_BUTT; uc_idx++)
    {
        if(0 == oal_strcmp(ac_name, g_ast_dmac_fast_aging_config_table[uc_idx].puc_string))
        {
            break;
        }
    }
    if (MAC_FAST_AGING_TYPE_BUTT == uc_idx)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CAR, "{wal_hipriv_fast_aging_cfg:: parameter error !\r\n");
        return uc_idx;
    }
    pst_aging_cfg_param->en_cmd = g_ast_dmac_fast_aging_config_table[uc_idx].en_fast_aging_cfg_id;


    //????????????
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    pc_param = pc_param + ul_off_set;
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_fast_aging_cfg::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    l_tmp = (oal_int32)oal_atoi(ac_name);
    //???????? >= 0 ??< 10*1000
    if ((l_tmp < 0) || (l_tmp > 10*1000))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_fast_aging_cfg::  car param[%d] invalid! }\r\n", oal_atoi(ac_name));
        return OAL_FAIL;
    }

    if (MAC_FAST_AGING_ENABLE == pst_aging_cfg_param->en_cmd)
    {
        if ((OAL_FALSE == (oal_uint8)l_tmp) || (OAL_TRUE == (oal_uint8)l_tmp))
        {
            pst_aging_cfg_param->en_enable = (oal_uint8)l_tmp;
        }
        else
        {
            OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_fast_aging_cfg:: en_fast_aging_flag[%d] must be 0 or 1.}", (oal_uint8)l_tmp);
            return OAL_FAIL;
        }

    }
    if (MAC_FAST_AGING_TIMEOUT == pst_aging_cfg_param->en_cmd)
    {
        if (0 == (oal_uint16)l_tmp)
        {
            OAM_WARNING_LOG0(0, OAM_SF_CAR, "{wal_hipriv_fast_aging_cfg:: timer_ms shoule not be 0.}");
            return OAL_FAIL;
        }

        pst_aging_cfg_param->us_timeout_ms = (oal_uint16)l_tmp;
    }
    if (MAC_FAST_AGING_COUNT == pst_aging_cfg_param->en_cmd)
    {
        if (0 == (oal_uint8)l_tmp)
        {
            OAM_WARNING_LOG0(0, OAM_SF_CAR, "{wal_hipriv_fast_aging_cfg:: uc_count_limit shoule not be 0.}");
            return OAL_FAIL;
        }

        pst_aging_cfg_param->uc_count_limit = (oal_uint8)l_tmp;
    }

/***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_FAST_AGING, OAL_SIZEOF(mac_cfg_fast_aging_stru));

    OAM_WARNING_LOG4(0, OAM_SF_CAR, "{wal_hipriv_fast_aging_cfg::en_cmd[%d], en_fast_aging[%d], timer[%d] uc_count_limit[%d]!}\r\n",
        pst_aging_cfg_param->en_cmd,
        pst_aging_cfg_param->en_enable,
        pst_aging_cfg_param->us_timeout_ms,
        pst_aging_cfg_param->uc_count_limit);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_fast_aging_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_fast_aging_cfg::return err code [%d]!}\r\n", ul_ret);
        return (oal_uint32)ul_ret;
    }

    return OAL_SUCC;


}

#endif

#ifdef _PRE_WLAN_FEATURE_CAR

OAL_STATIC oal_uint32  wal_hipriv_car_cfg(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{

    wal_msg_write_stru              st_write_msg;
    oal_uint32                      ul_off_set;
    oal_int8                        ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32                      ul_ret;
    oal_int32                       l_ret;
    oal_uint32                      ul_tmp;
    oal_uint8                       uc_idx;
    mac_cfg_car_stru               *pst_car_cfg_param;

    /* ???????????????? */
    pst_car_cfg_param = (mac_cfg_car_stru *)(st_write_msg.auc_value);
    memset_s((oal_void *)pst_car_cfg_param, OAL_SIZEOF(mac_cfg_car_stru),
             0, OAL_SIZEOF(mac_cfg_car_stru));

    //????device ???? vap ???? user ???? timer ???? enable
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    pc_param = pc_param + ul_off_set;
    if (OAL_SUCC != ul_ret)
    {
         OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_car_cfg::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
         return ul_ret;
    }

    //??????????????????????????????????
    for (uc_idx = 0; uc_idx < MAC_CAR_TYPE_BUTT; uc_idx++)
    {
        if(0 == oal_strcmp(ac_name, g_ast_hmac_car_config_table[uc_idx].puc_car_name))
        {
            break;
        }
    }
    if (MAC_CAR_TYPE_BUTT == uc_idx)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CAR, "{wal_hipriv_car_cfg:: parameter error !\r\n");
        return uc_idx;
    }
    pst_car_cfg_param->en_car_flag = g_ast_hmac_car_config_table[uc_idx].en_car_cfg_id;

    //??????device_bw??vap_bw??user_bw??????????on????down
    if ((MAC_CAR_DEVICE_LIMIT == pst_car_cfg_param->en_car_flag) || (MAC_CAR_VAP_LIMIT == pst_car_cfg_param->en_car_flag)|| (MAC_CAR_USER_LIMIT == pst_car_cfg_param->en_car_flag))
    {
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        pc_param = pc_param + ul_off_set;
        if(0 == oal_strcmp(ac_name, "up"))
        {
            pst_car_cfg_param->uc_car_up_down_type = HMAC_CAR_UPLINK ;
        }
        else if (0 == oal_strcmp(ac_name, "down"))
        {
            pst_car_cfg_param->uc_car_up_down_type = 1 ;
        }
        else
        {
            return OAL_FAIL;
        }
    }
    //??????user????????????mac????
    if (MAC_CAR_USER_LIMIT == pst_car_cfg_param->en_car_flag)
    {
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        pc_param = pc_param + ul_off_set;
        if (OAL_SUCC != ul_ret)
        {
             OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_car_cfg::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
             return ul_ret;
        }
        /* ????mac???? */
        oal_strtoaddr(ac_name, OAL_SIZEOF(ac_name), pst_car_cfg_param->auc_user_macaddr, WLAN_MAC_ADDR_LEN);
    }

    //????show_info????????limit_kbps ????timer_cycle ????car_enable
    if ((MAC_CAR_SHOW_INFO != pst_car_cfg_param->en_car_flag))
    {
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        pc_param = pc_param + ul_off_set;
        if (OAL_SUCC != ul_ret)
        {
             OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_car_cfg::wal_get_cmd_one_arg_etc return err_code [%d]!}\r\n", ul_ret);
             return ul_ret;
        }

        //???????? >= 0 ??????????????????1000*1024kbps
        if ((0 > oal_atoi(ac_name)) || (1000*1024 < oal_atoi(ac_name)))
        {
            OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_car_cfg::  car param[%d] invalid! }\r\n", oal_atoi(ac_name));
            return OAL_FAIL;
        }

        ul_tmp = (oal_uint32)oal_atoi(ac_name);
        if (MAC_CAR_TIMER_CYCLE_MS == pst_car_cfg_param->en_car_flag)
        {
            if (0 == (oal_uint16)ul_tmp)
            {
                OAM_WARNING_LOG0(0, OAM_SF_CAR, "{wal_hipriv_car_cfg:: timer_ms shoule not be 0.}");
                return OAL_FAIL;
            }

            pst_car_cfg_param->us_car_timer_cycle_ms = (oal_uint16)ul_tmp;
        }
        else if (MAC_CAR_ENABLE == pst_car_cfg_param->en_car_flag)
        {
            if ((OAL_FALSE == (oal_uint8)ul_tmp) || (OAL_TRUE == (oal_uint8)ul_tmp))
            {
                pst_car_cfg_param->en_car_enable_flag = (oal_uint8)ul_tmp;
            }
            else
            {
                OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_car_cfg:: car_flag[%d] must be 0 or 1.}", (oal_uint8)ul_tmp);
                return OAL_FAIL;
            }
        }
        else if (MAC_CAR_MULTICAST_PPS == pst_car_cfg_param->en_car_flag)
        {
            pst_car_cfg_param->ul_car_multicast_pps_num = ul_tmp;
        }
        else
        {
            pst_car_cfg_param->ul_bw_limit_kbps = ul_tmp;
        }
    }

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_CAR_CFG, OAL_SIZEOF(mac_cfg_car_stru));

    OAM_WARNING_LOG4(0, OAM_SF_CAR, "{wal_hipriv_car_cfg::en_car_flag[%d], uc_car_up_down_type[%d], timer[%d] kbps[%d]!}\r\n",
        pst_car_cfg_param->en_car_flag,
        pst_car_cfg_param->uc_car_up_down_type,
        pst_car_cfg_param->us_car_timer_cycle_ms,
        pst_car_cfg_param->ul_bw_limit_kbps);
    OAM_WARNING_LOG2(0, OAM_SF_CAR, "{wal_hipriv_car_cfg::uc_car_enable[%d], multicast_pps_num[%d]!}\r\n", pst_car_cfg_param->en_car_enable_flag, pst_car_cfg_param->ul_car_multicast_pps_num);

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_cfg_car_stru),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CAR, "{wal_hipriv_car_cfg::return err code [%d]!}\r\n", ul_ret);
        return (oal_uint32)ul_ret;
    }

    return OAL_SUCC;

}
#endif

#ifdef _PRE_FEATURE_FAST_AGING
oal_int32  wal_ioctl_get_fast_aging(oal_net_device_stru *pst_net_dev, oal_iw_request_info_stru *pst_info, oal_void *p_param, oal_int8 *pc_extra)
{
    oal_int32                       l_ret;
    oal_iw_point_stru               *pst_w = (oal_iw_point_stru *)p_param;
    wal_msg_write_stru              st_query_msg;
    mac_cfg_fast_aging_stru        *pst_aging_cfg_param;
    mac_vap_stru                   *pst_mac_vap;
    hmac_vap_stru                  *pst_hmac_vap;

    /***************************************************************************
                                ????????wal??????
        ***************************************************************************/
    st_query_msg.en_wid = (oal_uint16)WLAN_CFGID_GET_FAST_AGING;

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_QUERY,
                               WAL_MSG_WID_LENGTH,
                               (oal_uint8 *)&st_query_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_SUCC != l_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_get_ppm::return err code %d!}\r\n", l_ret);
        return -l_ret;
    }

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if(NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_ioctl_get_param_char::pst_mac_vap is null!}\r\n");
        return -OAL_EINVAL;
    }
    pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_ioctl_get_param_char: hmac vap is null. vap id:%d", pst_mac_vap->uc_vap_id);
        return 0;
    }

    pst_hmac_vap->auc_query_flag[QUERY_ID_FAST_AGING] = OAL_FALSE;
    /*lint -e730*/
    l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT((pst_hmac_vap->query_wait_q), (OAL_TRUE == pst_hmac_vap->auc_query_flag[QUERY_ID_FAST_AGING]), (5*OAL_TIME_HZ));
    /*lint +e730*/
    if (l_ret <= 0) /* ?????????????? */
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "wal_ioctl_get_param_char: query rssi timeout. ret:%d", l_ret);
    }

    pst_aging_cfg_param = (mac_cfg_fast_aging_stru*)pst_hmac_vap->st_param_char.auc_buff;


    snprintf_s(pc_extra, WLAN_IWPRIV_MAX_BUFF_LEN, WLAN_IWPRIV_MAX_BUFF_LEN - 1,
                "fast aging: enable=%d, timeout=%d, count=%d\nsuccess\n",
                pst_aging_cfg_param->en_enable, pst_aging_cfg_param->us_timeout_ms, pst_aging_cfg_param->uc_count_limit);
    pst_w->length = (oal_uint16)OAL_STRLEN(pc_extra);




    return OAL_SUCC;
}

#endif

#ifdef _PRE_WLAN_FEATURE_M2S_MSS

OAL_STATIC oal_uint32  wal_ioctl_set_m2s_blacklist(oal_net_device_stru *pst_net_dev, oal_uint8 *puc_buf, oal_uint32 ul_buf_len, oal_uint8 uc_m2s_blacklist_cnt)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;
    mac_m2s_ie_stru                *pst_m2s_ie;

    /* st_write_msg?????????? */
    memset_s(&st_write_msg, OAL_SIZEOF(wal_msg_write_stru), 0, OAL_SIZEOF(wal_msg_write_stru));

    if (uc_m2s_blacklist_cnt > WLAN_M2S_BLACKLIST_MAX_NUM)
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_ioctl_set_m2s_blacklist::blacklist[%d] is beyond scope.}", uc_m2s_blacklist_cnt);
        return OAL_FAIL;
    }

    if (ul_buf_len < uc_m2s_blacklist_cnt * OAL_SIZEOF(wlan_m2s_mgr_vap_stru))
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_ioctl_set_m2s_blacklist::src buff len[%d] not enough.}", ul_buf_len);
        return OAL_FAIL;
    }

    /***************************************************************************
          ????????wal??????
      ***************************************************************************/
    pst_m2s_ie = (mac_m2s_ie_stru *)st_write_msg.auc_value;

    pst_m2s_ie->uc_blacklist_cnt = uc_m2s_blacklist_cnt;

    l_ret = memcpy_s(pst_m2s_ie->ast_m2s_blacklist, WLAN_M2S_BLACKLIST_MAX_NUM * OAL_SIZEOF(wlan_m2s_mgr_vap_stru),
                     puc_buf, uc_m2s_blacklist_cnt * OAL_SIZEOF(wlan_m2s_mgr_vap_stru));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_M2S, "wal_ioctl_set_m2s_blacklist::memcpy fail!");
        return OAL_FAIL;
    }

    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_M2S_BLACKLIST, OAL_SIZEOF(mac_m2s_ie_stru));

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                             WAL_MSG_TYPE_WRITE,
                             WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(mac_m2s_ie_stru),
                             (oal_uint8 *)&st_write_msg,
                             OAL_FALSE,
                             OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_ioctl_set_m2s_blacklist::wal_send_cfg_event_etc return err code = [%d].}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}


oal_uint32  wal_ioctl_set_m2s_mss(oal_net_device_stru *pst_net_dev, oal_uint8 uc_m2s_mode)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret;

    /* st_write_msg?????????? */
    memset_s(&st_write_msg, OAL_SIZEOF(wal_msg_write_stru), 0, OAL_SIZEOF(wal_msg_write_stru));

    if (uc_m2s_mode > MAC_M2S_COMMAND_MODE_GET_STATE)
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_ioctl_set_m2s_mss::uc_m2s_mode[%d] is beyond scope.}", uc_m2s_mode);
        return OAL_FAIL;
    }

    /***************************************************************************
          ????????wal??????
      ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_M2S_MSS, OAL_SIZEOF(oal_int32));
    *((oal_int32 *)(st_write_msg.auc_value)) = uc_m2s_mode;  /* ???????????????? */

    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                             WAL_MSG_TYPE_WRITE,
                             WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_int32),
                             (oal_uint8 *)&st_write_msg,
                             OAL_FALSE,
                             OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_ioctl_set_m2s_mss::wal_send_cfg_event_etc return err code = [%d].}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}

#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifndef CONFIG_HAS_EARLYSUSPEND

OAL_STATIC oal_int32  wal_ioctl_set_suspend_mode(oal_net_device_stru *pst_net_dev, oal_uint8 uc_suspend)
{
    wal_msg_write_stru              st_write_msg;
    oal_int32                       l_ret = 0;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_net_dev)))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{wal_ioctl_set_suspend_mode::pst_net_dev null ptr error!}");
        return -OAL_EFAUL;
    }

    st_write_msg.auc_value[0] = uc_suspend;

    /***************************************************************************
        ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_SUSPEND_MODE, OAL_SIZEOF(uc_suspend));

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(uc_suspend),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    return l_ret;
}
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_APF

OAL_STATIC oal_uint32  wal_hipriv_apf_filter_list(oal_net_device_stru *pst_net_dev,oal_int8 *pc_param)
{
    wal_msg_write_stru           st_write_msg;
    oal_int32                    l_ret;
    mac_apf_filter_cmd_stru      st_apf_filter_cmd;

    memset_s(&st_apf_filter_cmd, OAL_SIZEOF(mac_apf_filter_cmd_stru), 0, OAL_SIZEOF(mac_apf_filter_cmd_stru));
    st_apf_filter_cmd.en_cmd_type = APF_GET_FILTER_CMD;
    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SET_APF_FILTER, OAL_SIZEOF(st_apf_filter_cmd));
    l_ret = memcpy_s(st_write_msg.auc_value, OAL_SIZEOF(st_apf_filter_cmd),
                     &st_apf_filter_cmd, OAL_SIZEOF(st_apf_filter_cmd));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "wal_hipriv_apf_filter_list::memcpy fail!");
        return OAL_FAIL;
    }

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(st_apf_filter_cmd),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
       OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_80m_rts_debug::return err code [%d]!}\r\n", l_ret);
       return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}
#endif


OAL_STATIC oal_uint32  wal_hipriv_remove_app_ie(oal_net_device_stru *pst_net_dev,oal_int8 *pc_param)
{
    wal_msg_write_stru           st_write_msg;
    oal_int32                    l_ret;
    oal_uint32                   ul_ret;
    oal_uint32                   ul_off_set;
    oal_int8                     ac_name[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    const oal_uint8              uc_remove_ie_len = 2;  /* 0/1 eid*/

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_REMOVE_APP_IE, uc_remove_ie_len);

    /* ????????????????????????????0/1 ???????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_remove_app_ie::wal_get_cmd_one_arg_etc type return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }
    st_write_msg.auc_value[0] = (oal_uint8)oal_atoi(ac_name);
    /* ??????????EID */
    pc_param += ul_off_set;
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_name, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_remove_app_ie::wal_get_cmd_one_arg_etc eid return err_code %d!}\r\n", ul_ret);
        return ul_ret;
    }
    st_write_msg.auc_value[1] = (oal_uint8)oal_atoi(ac_name);

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + uc_remove_ie_len,
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_remove_app_ie::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_DELAY_STATISTIC

OAL_STATIC oal_uint32 wal_hipriv_pkt_time_switch(oal_net_device_stru *pst_net_dev, oal_int8 *pc_param)
{
    oal_uint32          ul_off_set;
    oal_uint32          ul_ret;
    oal_int8            ac_arg[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    wal_msg_write_stru  st_write_msg;
    user_delay_switch_stru st_switch_cmd;
    oal_int32              l_ret;

    /* ?????????????? */
    ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_pkt_time_switch::wal_get_cmd_one_arg return err_code of 1st parameter [%d]!}\r\n", ul_ret);
        return ul_ret;
    }

    memset_s(&st_switch_cmd, OAL_SIZEOF(user_delay_switch_stru), 0, OAL_SIZEOF(user_delay_switch_stru));
    if (0 == (oal_strcmp("on", ac_arg)))
    {
        st_switch_cmd.dmac_stat_enable = 1;

        /*????????????????????????????*/
        pc_param += ul_off_set;
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_pkt_time_switch::wal_get_cmd_one_arg return er_code of 2nd parameter [%d]!}\r\n", ul_ret);
            return ul_ret;
        }
        st_switch_cmd.dmac_packet_count_num = (oal_uint32)oal_atoi(ac_arg);

        /*????????????????????*/
        pc_param += ul_off_set;
        ul_ret = wal_get_cmd_one_arg_etc(pc_param, ac_arg, WAL_HIPRIV_CMD_NAME_MAX_LEN, &ul_off_set);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_hipriv_pkt_time_switch::wal_get_cmd_one_arg return err_code of 3rd parameter [%d]!}\r\n", ul_ret);
            return ul_ret;
        }
        st_switch_cmd.dmac_report_interval = (oal_uint32)oal_atoi(ac_arg);

    }
    else if (0 == (oal_strcmp("off", ac_arg)))
    {
        st_switch_cmd.dmac_stat_enable = 0;
        st_switch_cmd.dmac_packet_count_num = 0;
        st_switch_cmd.dmac_report_interval = 0;
    }
    else
    {
        OAM_ERROR_LOG0(0, OAM_SF_CFG, "{wal_hipriv_pkt_time_switch::invalid parameter!}");
        return OAL_FAIL;
    }
    l_ret = memcpy_s(st_write_msg.auc_value, OAL_SIZEOF(st_switch_cmd), &st_switch_cmd,  OAL_SIZEOF(st_switch_cmd));
    if (l_ret != EOK) {
        OAM_ERROR_LOG0(0, OAM_SF_CFG, "wal_hipriv_pkt_time_switch::memcpy fail!");
        return OAL_FAIL;
    }

    /***************************************************************************
                             ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_PKT_TIME_SWITCH, OAL_SIZEOF(st_switch_cmd));
    ul_ret = (oal_uint32)wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(st_switch_cmd),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);

    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_hipriv_pkt_time_switch::return err code %d!}\r\n", ul_ret);
    }
    return ul_ret;
}
#endif

#ifdef  _PRE_WLAN_FEATURE_FORCE_STOP_FILTER

OAL_STATIC oal_uint32  wal_ioctl_force_stop_filter(oal_net_device_stru *pst_net_dev,oal_uint8 uc_param)
{
    wal_msg_write_stru           st_write_msg;
    oal_int32                    l_ret;
    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_FORCE_STOP_FILTER, OAL_SIZEOF(oal_uint8));

    st_write_msg.auc_value[0] = uc_param;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_ioctl_force_stop_filter::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_force_stop_filter::uc_param[%d].}", uc_param);
    return OAL_SUCC;
}
#endif

oal_uint32  wal_ioctl_set_ap_mode(oal_net_device_stru *pst_net_dev,oal_uint8 uc_param)
{
    wal_msg_write_stru           st_write_msg;
    oal_int32                    l_ret;
    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_SOFTAP_MIMO_MODE, OAL_SIZEOF(oal_uint8));

    st_write_msg.auc_value[0] = uc_param;

    /* ???????? */
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                               WAL_MSG_TYPE_WRITE,
                               WAL_MSG_WRITE_MSG_HDR_LENGTH + OAL_SIZEOF(oal_uint8),
                               (oal_uint8 *)&st_write_msg,
                               OAL_FALSE,
                               OAL_PTR_NULL);
    if (OAL_UNLIKELY(OAL_SUCC != l_ret))
    {
        OAM_WARNING_LOG1(0, OAM_SF_CFG, "{wal_ioctl_set_ap_mode::return err code [%d]!}\r\n", l_ret);
        return (oal_uint32)l_ret;
    }

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_ioctl_set_ap_mode::uc_param[%d].}", uc_param);
    return OAL_SUCC;
}


oal_uint32  wal_vendor_cmd_gather_handler(oal_net_device_stru *pst_net_dev, oal_int8 *pc_command)
{
    oal_int32    l_ret              = 0;

    if (0 == oal_strncasecmp(pc_command, CMD_SET_SOFTAP_MIMOMODE, OAL_STRLEN(CMD_SET_SOFTAP_MIMOMODE)))
    {
        if (OAL_STRLEN(pc_command) < OAL_STRLEN((oal_int8 *)CMD_SET_SOFTAP_MIMOMODE))
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{wal_vendor_cmd_gather_handler::CMD_SET_SOFTAP_MIMOMODE puc_command len error.}\r\n");

            return -OAL_EFAIL;
        }

        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_cmd_gather_handler::CMD_SET_SOFTAP_MIMOMODE %d.}", OAL_TRUE);

        l_ret = wal_ioctl_set_ap_mode(pst_net_dev, OAL_TRUE);

        if (OAL_UNLIKELY(OAL_SUCC != l_ret))
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{wal_vendor_cmd_gather_handler::CMD_SET_SOFTAP_MIMOMODE return err code [%d]!}\r\n", l_ret);
            return -OAL_EFAIL;
        }
    }
#ifdef _PRE_WLAN_FEATURE_M2S
    else if(0 == oal_strncasecmp(pc_command, CMD_SET_M2S_SWITCH, OAL_STRLEN(CMD_SET_M2S_SWITCH)))
    {
        oal_uint8 uc_m2s_switch_on;

        if (OAL_STRLEN(pc_command) < (OAL_STRLEN((oal_int8 *)CMD_SET_M2S_SWITCH) + 2))
        {
            OAM_WARNING_LOG0(0, OAM_SF_M2S, "{wal_vendor_cmd_gather_handler::CMD_SET_M2S_SWITCH puc_command len error.}\r\n");

            return -OAL_EFAIL;
        }

        uc_m2s_switch_on = (oal_uint8)oal_atoi(pc_command + OAL_STRLEN((oal_int8 *)CMD_SET_M2S_SWITCH) + 1);

        OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_vendor_cmd_gather_handler::CMD_SET_M2S_SWITCH %d.}", uc_m2s_switch_on);

    #ifdef _PRE_WLAN_FEATURE_M2S_MSS
        l_ret = wal_ioctl_set_m2s_mss(pst_net_dev, uc_m2s_switch_on);

        if (OAL_UNLIKELY(OAL_SUCC != l_ret))
        {
            OAM_WARNING_LOG1(0, OAM_SF_M2S, "{wal_vendor_cmd_gather_handler::CMD_SET_M2S_SWITCH return err code [%d]!}\r\n", l_ret);
            return -OAL_EFAIL;
        }
    #endif
    }
#endif
    return l_ret;
}


oal_uint8  wal_vendor_cmd_gather(oal_int8 *pc_command)
{
    if ((0 == oal_strncasecmp(pc_command, CMD_SET_SOFTAP_MIMOMODE, OAL_STRLEN(CMD_SET_SOFTAP_MIMOMODE)))
#ifdef _PRE_WLAN_FEATURE_M2S
        || (0 == oal_strncasecmp(pc_command, CMD_SET_M2S_SWITCH, OAL_STRLEN(CMD_SET_M2S_SWITCH)))
#endif
        )
    {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}

#ifdef _PRE_WLAN_FEATURE_PSM_FLT_STAT
OAL_STATIC mac_psm_query_type_enum_uint8 wal_psm_query_cmd2type(oal_int8 *puc_query_cmd)
{
    if (0 == oal_strncasecmp(puc_query_cmd, CMD_GET_APF_PKTS_CNT, CMD_GET_APF_PKTS_CNT_LEN)) {
        return MAC_PSM_QUERY_FLT_STAT;
    } else if (0 == oal_strncasecmp(puc_query_cmd, CMD_GET_FAST_SLEEP_CNT, CMD_GET_FAST_SLEEP_CNT_LEN)) {
        return MAC_PSM_QUERY_FASTSLEEP_STAT;
    } else {
        return MAC_PSM_QUERY_TYPE_BUTT;
    }
}

OAL_STATIC oal_uint32 wal_psm_fill_flt_stat_info(hmac_psm_flt_stat_query_stru *pst_hmac_psm_query, oal_int8 *puc_query_buffer)
{
    oal_uint32 *pul_query_buff = (oal_uint32*)puc_query_buffer;

    pul_query_buff[0] = pst_hmac_psm_query->st_psm_flt_stat.ul_ao_drop_cnt;
    pul_query_buff[1] = pst_hmac_psm_query->st_psm_flt_stat.ul_ao_send_rsp_cnt;
    pul_query_buff[2] = pst_hmac_psm_query->st_psm_flt_stat.ul_apf_flt_drop_cnt;
    pul_query_buff[3] = pst_hmac_psm_query->st_psm_flt_stat.ul_icmp_flt_drop_cnt;

    OAM_WARNING_LOG4(0, OAM_SF_CFG, "wal_psm_fill_flt_stat_info: apf drop:%d, ao drop%d, ao send rsp%d, icmp %d",
                       pst_hmac_psm_query->st_psm_flt_stat.ul_apf_flt_drop_cnt,
                       pst_hmac_psm_query->st_psm_flt_stat.ul_ao_drop_cnt,
                       pst_hmac_psm_query->st_psm_flt_stat.ul_ao_send_rsp_cnt,
                       pst_hmac_psm_query->st_psm_flt_stat.ul_icmp_flt_drop_cnt);
    return OAL_SIZEOF(oal_uint32)*4;
}

OAL_STATIC oal_uint32 wal_psm_fill_fastsleep_stat_info(hmac_psm_flt_stat_query_stru *pst_hmac_psm_query, oal_int8 *puc_query_buffer)
{
    oal_uint32 *pul_query_buff = (oal_uint32*)puc_query_buffer;

    pul_query_buff[0] = pst_hmac_psm_query->st_psm_fastsleep_stat.ul_short_sleep_cnt;
    pul_query_buff[1] = pst_hmac_psm_query->st_psm_fastsleep_stat.ul_long_sleep_cnt;

    OAM_WARNING_LOG2(0, OAM_SF_CFG, "wal_psm_fill_fastsleep_stat_info: ul_short_sleep_cnt:%d, ul_long_sleep_cnt:%d",
                    pst_hmac_psm_query->st_psm_fastsleep_stat.ul_short_sleep_cnt,
                    pst_hmac_psm_query->st_psm_fastsleep_stat.ul_long_sleep_cnt);

    return OAL_SIZEOF(oal_uint32)*2;
}

OAL_STATIC oal_uint32 wal_psm_fill_query_info(hmac_psm_flt_stat_query_stru *pst_hmac_psm_query,
                                            mac_psm_query_type_enum_uint8 en_query_type,
                                            oal_int8 *puc_query_buffer)
{
    oal_uint32 ul_ret_len = 0;
    switch (en_query_type) {
        case MAC_PSM_QUERY_FLT_STAT:
            ul_ret_len = wal_psm_fill_flt_stat_info(pst_hmac_psm_query, puc_query_buffer);
        break;

        case MAC_PSM_QUERY_FASTSLEEP_STAT:
            ul_ret_len = wal_psm_fill_fastsleep_stat_info(pst_hmac_psm_query, puc_query_buffer);
        break;

        default:
            OAM_ERROR_LOG1(0, OAM_SF_CFG, "wal_psm_fill_query_info::bad query type[%d]", en_query_type);
        break;
    }

    return ul_ret_len;
}

OAL_STATIC oal_int32 wal_psm_query_wait_complete(hmac_psm_flt_stat_query_stru *pst_hmac_psm_query,
                                                 mac_psm_query_type_enum_uint8 en_query_type)
{
    oal_int32 l_ret = 0;

    switch (en_query_type) {
        case MAC_PSM_QUERY_FLT_STAT:
            pst_hmac_psm_query->st_psm_flt_stat.en_query_completed_flag = OAL_FALSE;
            /*lint -e730 -e740 -e774*/
            l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_psm_query->st_wait_queue,
                     (OAL_TRUE == pst_hmac_psm_query->st_psm_flt_stat.en_query_completed_flag),
                     5 * OAL_TIME_HZ);
             /*lint +e730 +e740 +e774*/
        break;

        case MAC_PSM_QUERY_FASTSLEEP_STAT:
            pst_hmac_psm_query->st_psm_fastsleep_stat.en_query_completed_flag = OAL_FALSE;
            /*lint -e730 -e740 -e774*/
            l_ret = OAL_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(pst_hmac_psm_query->st_wait_queue,
                     (OAL_TRUE == pst_hmac_psm_query->st_psm_fastsleep_stat.en_query_completed_flag),
                     5 * OAL_TIME_HZ);
             /*lint +e730 +e740 +e774*/
        break;

        default:
            OAM_ERROR_LOG1(0, OAM_SF_CFG, "wal_psm_query_wait_complete::bad query type[%d]", en_query_type);
        break;
    }

    return l_ret;
}

oal_uint32 wal_ioctl_get_psm_stat(oal_net_device_stru *pst_net_dev, oal_int8 *puc_stat, oal_ifreq_stru *pst_ifr)
{
    wal_msg_write_stru st_write_msg;
    oal_int32 l_ret;
    mac_vap_stru *pst_mac_vap;
    hmac_device_stru *pst_hmac_device;
    hmac_psm_flt_stat_query_stru *pst_hmac_psm_query;
    mac_psm_query_type_enum_uint8 en_query_type;
    mac_psm_query_msg *pst_psm_query_msg;
    oal_uint32 ul_stat_len;

    pst_mac_vap = OAL_NET_DEV_PRIV(pst_net_dev);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_get_psm_stat: pst_mac_vap get from netdev is null.");
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_hmac_device = hmac_res_get_mac_dev_etc(pst_mac_vap->uc_device_id);
    if (OAL_PTR_NULL == pst_hmac_device)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_ioctl_get_psm_stat: pst_hmac_device is null ptr. device id:%d", pst_mac_vap->uc_device_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /***************************************************************************
                                ????????wal??????
    ***************************************************************************/
    memset_s(&st_write_msg, OAL_SIZEOF(st_write_msg), 0, OAL_SIZEOF(st_write_msg));
    WAL_WRITE_MSG_HDR_INIT(&st_write_msg, WLAN_CFGID_QUERY_PSM_STAT, OAL_SIZEOF(mac_psm_query_msg));
    en_query_type = wal_psm_query_cmd2type(puc_stat);
    if (en_query_type == MAC_PSM_QUERY_TYPE_BUTT) {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_ioctl_get_psm_stat: query type is not supported");
        return OAL_FAIL;
    }
    pst_psm_query_msg = (mac_psm_query_msg*)(st_write_msg.auc_value);
    pst_psm_query_msg->en_query_type = en_query_type;
    l_ret = wal_send_cfg_event_etc(pst_net_dev,
                                   WAL_MSG_TYPE_WRITE,
                                   WAL_MSG_WRITE_MSG_HDR_LENGTH+OAL_SIZEOF(mac_psm_query_msg),
                                   (oal_uint8 *)&st_write_msg,
                                   OAL_FALSE,
                                   OAL_PTR_NULL);
    if (l_ret != OAL_SUCC) {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_ioctl_get_psm_stat: send  event failed ret:%d", l_ret);
    }

    pst_hmac_psm_query = &pst_hmac_device->st_psm_flt_stat_query;
    /* ???????????? */
    l_ret = wal_psm_query_wait_complete(pst_hmac_psm_query, en_query_type);
    if (l_ret <= 0) { /* ?????????????? */
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "wal_ioctl_get_psm_stat: timeout or err occur. ret:%d", l_ret);
        return OAL_FAIL;
    }

    /* ??????????????buffer?? */
    ul_stat_len = wal_psm_fill_query_info(pst_hmac_psm_query, en_query_type, puc_stat);
    if (ul_stat_len == 0) {
        OAM_ERROR_LOG0(0, OAM_SF_CFG, "wal_ioctl_get_psm_stat: stat len is 0!");
        return OAL_FAIL;
    }
    if (oal_copy_to_user(pst_ifr->ifr_data+8, puc_stat, ul_stat_len)) {
        OAM_ERROR_LOG0(0, OAM_SF_CFG, "wal_ioctl_get_psm_stat: Failed to copy ioctl_data to user !");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}
#endif

#ifdef _PRE_WLAN_FEATURE_STA_PM

OAL_STATIC oal_uint32 wal_ioctl_set_fastsleep_switch(oal_net_device_stru *pst_net_dev,oal_int8 *puc_para_str)
{
    oal_uint32  ul_ret;
    oal_uint8   auc_para_val[4];
    oal_uint8   auc_para_str_tmp[WAL_HIPRIV_CMD_NAME_MAX_LEN];
    oal_uint32  pul_cmd_offset;
    oal_uint32  ul_para_cnt;

    if (!oal_strncasecmp(puc_para_str, "0", 1))
    {
        /* ????fastsleep */
        wal_ioctl_set_sta_ps_mode_etc(pst_net_dev, MIN_FAST_PS);
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_set_fastsleep_switch:disable fast sleep!");
    }
    else if(!oal_strncasecmp(puc_para_str, "1", 1))
    {
        puc_para_str += 1;
        /* ??????????4??????<min listen????><max listen????><??????????????><??????????????> */
        for (ul_para_cnt = 0;ul_para_cnt < 4;ul_para_cnt++)
        {
            ul_ret = wal_get_cmd_one_arg_etc(puc_para_str, auc_para_str_tmp, WAL_HIPRIV_CMD_NAME_MAX_LEN, &pul_cmd_offset);
            if (OAL_SUCC != ul_ret)
            {
                 OAM_WARNING_LOG2(0, OAM_SF_CFG, "{wal_ioctl_set_fastsleep_switch::get para[%d] fail, return err_code [%d]!}\r\n", ul_para_cnt, ul_ret);
                 return ul_ret;
            }
            auc_para_val[ul_para_cnt] = (oal_uint8)oal_atoi(auc_para_str_tmp);
            puc_para_str += pul_cmd_offset;
        }

#ifndef WIN32
        auc_para_val[0] /= WLAN_SLEEP_TIMER_PERIOD;
        auc_para_val[1] /= WLAN_SLEEP_TIMER_PERIOD;
        /* ??????host???????? */
        wlan_min_fast_ps_idle = auc_para_val[0];
        wlan_max_fast_ps_idle = auc_para_val[1];
        wlan_auto_ps_thresh_screen_on = auc_para_val[2];
        wlan_auto_ps_thresh_screen_off = auc_para_val[3];

        OAM_WARNING_LOG4(0, OAM_SF_CFG, "{wal_ioctl_set_fastsleep_switch::wlan_min_fast_ps_idle[%d], wlan_max_fast_ps_idle[%d], wlan_auto_ps_thresh_screen_on[%d], wlan_auto_ps_thresh_screen_off!}\r\n",
                                      wlan_min_fast_ps_idle, wlan_max_fast_ps_idle, wlan_auto_ps_thresh_screen_on, wlan_auto_ps_thresh_screen_off);
#endif
        /* ???????? */
        wal_ioctl_set_fast_sleep_para_etc(pst_net_dev, auc_para_val);
        /* ????ps mode */
        wal_ioctl_set_sta_ps_mode_etc(pst_net_dev, AUTO_FAST_PS);
    }
    else
    {
        OAM_WARNING_LOG0(0, OAM_SF_CFG, "wal_ioctl_set_fastsleep_switch:invalid cmd str!");
    }

    return OAL_SUCC;
}
#endif

