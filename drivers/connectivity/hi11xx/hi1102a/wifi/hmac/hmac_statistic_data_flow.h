

#ifndef __MAC_STATISTIC_DATA_FLOW_H__
#define __MAC_STATISTIC_DATA_FLOW_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/*****************************************************************************
  1 ??????????????
*****************************************************************************/
#include "hmac_main.h"
#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#include "hisi_customize_wifi.h"
#endif
/*****************************************************************************
  2 ??????
*****************************************************************************/
#define WLAN_STATIS_DATA_TIMER_PERIOD    (100)          /*??????100ms????*/
#define WLAN_THROUGHPUT_STA_PERIOD        (20)
#define WLAN_THROUGHPUT_LOAD_LOW          (10)           /* ??????10M */
/* WIFI?????????????????????????????????? */
#define WLAN_IRQ_AFFINITY_IDLE_CPU   0
#define WLAN_IRQ_AFFINITY_BUSY_CPU   4
#define PM_QOS_THOUGHTPUT_VALUE      60

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
typedef struct {
    oal_uint32          ul_tx_bytes;            /* WIFI ?????????????? */
    oal_uint32          ul_rx_bytes;            /* WIFI ?????????????? */
    oal_uint32          ul_tx_large_pkt;
    oal_uint32          ul_rx_large_pkt;
    oal_uint32          ul_tx_small_pkt;
    oal_uint32          ul_rx_small_pkt;
    oal_uint32          ul_rx_amsdu_pkt;
}wifi_statis_pkgs_info_stru;

typedef struct{
    wifi_statis_pkgs_info_stru  st_pkgs_info;
    oal_bool_enum_uint8         en_wifi_rx_busy;
    oal_uint8                   uc_timer_cycles;        /* ???????????? */
    oal_uint32                  ul_pre_time;            /* ?????????????? */
    frw_timeout_stru            st_statis_data_timer;   /* ???????????????????????????????? */
} wifi_txrx_pkt_statis_stru;

typedef struct
{
    oal_uint32 ul_rx_throughput_mbps;
    oal_uint32 ul_tx_throughput_mbps;
    oal_uint32 ul_tx_large_pps;
    oal_uint32 ul_rx_large_pps;
    oal_uint32 ul_tx_small_pps;
    oal_uint32 ul_rx_small_pps;
    oal_uint32 ul_rx_amsdu_pps;
} wifi_txrx_calculated_stat_stru;

#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ

enum
{
    /* frequency lock disable mode */
    H2D_FREQ_MODE_DISABLE                 = 0,
    /* frequency lock enable mode */
    H2D_FREQ_MODE_ENABLE                  = 1,
};
typedef oal_uint8 oal_freq_mode_enum_uint8;

typedef enum
{
    CMD_SET_DEVICE_FREQ_ENDABLE, //cmd type 0
    CMD_SET_DEVICE_FREQ_VALUE,
    CMD_SET_AUTO_BYPASS_DEVICE_AUTO_FREQ,
    CMD_GET_DEVICE_AUTO_FREQ,
    CMD_AUTO_FREQ_BUTT,
} oal_h2d_freq_cmd_enum;
typedef oal_uint8 oal_h2d_freq_cmd_enum_uint8;

typedef struct
{
    oal_uint8  uc_device_type;   /*device????????*/
    oal_uint8  uc_reserve[3];   /*????????*/
} device_speed_freq_level_stru;

typedef struct
{
    oal_uint32  ul_speed_level;    /*??????????*/
    oal_uint32  ul_min_cpu_freq;  /*CPU????????*/
    oal_uint32  ul_min_ddr_freq;   /*DDR????????*/
} host_speed_freq_level_stru;

typedef struct
{
    oal_uint16           us_throughput_pps_irq_high;
    oal_uint16           us_throughput_pps_irq_low;
    oal_uint16           us_btcoex_throughput_pps_irq_high;
    oal_uint16           us_btcoex_throughput_pps_irq_low;
    oal_bool_enum_uint8  en_irq_cpu_state;            /* ???????? */
    oal_bool_enum_uint8  en_irq_bindcpu;            /* ?????????? */
    oal_uint8            uc_cpumask;
    oal_bool_enum_uint8  en_btcoex_flag;  /* ????BT???????????????????? */
} thread_bindcpu_stru;

extern host_speed_freq_level_stru g_host_speed_freq_level[];
extern device_speed_freq_level_stru g_device_speed_freq_level[];
extern thread_bindcpu_stru g_st_thread_bindcpu;

#endif
/*****************************************************************************
  8 UNION????
*****************************************************************************/


/*****************************************************************************
  9 OTHERS????
*****************************************************************************/


/*****************************************************************************
  10 ????????
*****************************************************************************/
extern oal_void hmac_rx_statistics_update_hmac_vap(hmac_vap_stru *pst_hmac_vap);
extern oal_void hmac_wifi_statistic_rx_amsdu(oal_uint32 ul_amsdu_tcp_ack_cnt);
extern oal_void hmac_wifi_statistic_rx_tcp_ack(oal_uint32 ul_small_pkt_cnt,
                                                    oal_uint32 ul_large_pkt_cnt,
                                                    oal_uint32 ul_filtered_rx_bytes);
extern oal_void hmac_wifi_statistic_rx_packets(oal_uint32 ul_pkt_bytes);
extern oal_void hmac_wifi_statistic_tx_packets(oal_uint32 ul_pkt_bytes);
extern oal_bool_enum_uint8 hmac_wifi_rx_is_busy(oal_void);
extern oal_void hmac_wifi_calculate_throughput(oal_void);
extern oal_void hmac_wifi_statis_data_timer_init(oal_void);
extern oal_void hmac_wifi_statis_data_timer_deinit(oal_void);
extern oal_void hmac_wifi_pm_state_notify(oal_bool_enum_uint8 en_wake_up);
extern oal_void hmac_wifi_state_notify(oal_bool_enum_uint8 en_wifi_on);


#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif




