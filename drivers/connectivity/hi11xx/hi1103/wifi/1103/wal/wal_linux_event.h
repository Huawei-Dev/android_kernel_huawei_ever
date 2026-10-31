

#ifndef __WAL_LINUX_EVENT_H__
#define __WAL_LINUX_EVENT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ??????????????
*****************************************************************************/
#include "oal_ext_if.h"
#include "frw_ext_if.h"
#include "mac_vap.h"
#include "mac_device.h"
#include "hmac_ext_if.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_WAL_LINUX_EVENT_H
/*****************************************************************************
  2 ??????
*****************************************************************************/


/*****************************************************************************
  3 ????????
*****************************************************************************/


/*****************************************************************************
  4 ????????????
*****************************************************************************/


/*****************************************************************************
  5 ??????????
*****************************************************************************/


/*****************************************************************************
  6 ????????
*****************************************************************************/


/*****************************************************************************
  7 STRUCT????
*****************************************************************************/

/*****************************************************************************
  8 UNION????
*****************************************************************************/


/*****************************************************************************
  9 OTHERS????
*****************************************************************************/


/*****************************************************************************
  10 ????????
*****************************************************************************/
extern oal_uint32  wal_cfg80211_start_scan_etc(oal_net_device_stru *pst_net_dev, mac_cfg80211_scan_param_stru *pst_scan_param);
extern oal_uint32  wal_cfg80211_start_sched_scan_etc(oal_net_device_stru *pst_net_dev, mac_pno_scan_stru *pst_pno_scan_info);
extern oal_int32  wal_cfg80211_start_connect_etc(oal_net_device_stru *pst_net_dev, mac_conn_param_stru *pst_connect_param);
extern oal_int32  wal_cfg80211_start_disconnect_etc(oal_net_device_stru *pst_net_dev, mac_cfg_kick_user_param_stru *pst_disconnect_param);

#ifdef _PRE_WLAN_FEATURE_AP_PM
extern oal_uint32 wal_config_sta_scan_connect_event(oal_net_device_stru * pst_net_dev,  oal_uint8 *puc_param);
#endif
#ifdef _PRE_WLAN_FEATURE_SAE
oal_uint32 wal_cfg80211_do_external_auth(oal_net_device_stru * pst_netdev, hmac_external_auth_req_stru *pst_ext_auth);
#endif /* _PRE_WLAN_FEATURE_SAE */

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of wal_linux_event.h */
