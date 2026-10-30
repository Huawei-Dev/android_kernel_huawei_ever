/*
* Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
* foss@huawei.com
*
* If distributed as part of the Linux kernel, the following license terms
* apply:
*
* * This program is free software; you can redistribute it and/or modify
* * it under the terms of the GNU General Public License version 2 and
* * only version 2 as published by the Free Software Foundation.
* *
* * This program is distributed in the hope that it will be useful,
* * but WITHOUT ANY WARRANTY; without even the implied warranty of
* * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* * GNU General Public License for more details.
* *
* * You should have received a copy of the GNU General Public License
* * along with this program; if not, write to the Free Software
* * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
*
* Otherwise, the following license terms apply:
*
* * Redistribution and use in source and binary forms, with or without
* * modification, are permitted provided that the following conditions
* * are met:
* * 1) Redistributions of source code must retain the above copyright
* *    notice, this list of conditions and the following disclaimer.
* * 2) Redistributions in binary form must reproduce the above copyright
* *    notice, this list of conditions and the following disclaimer in the
* *    documentation and/or other materials provided with the distribution.
* * 3) Neither the name of Huawei nor the names of its contributors may
* *    be used to endorse or promote products derived from this software
* *    without specific prior written permission.
*
* * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*/


#ifndef __AT_DATA_PROC_H__
#define __AT_DATA_PROC_H__


/*****************************************************************************
  1 ??????????????
*****************************************************************************/
#include "mdrv.h"
#include "AtCtx.h"
#include "AtInputProc.h"
#include "AtNdisInterface.h"
#include "AtRnicInterface.h"
#include "AdsInterface.h"
#include "AtPppInterface.h"
#include "FcInterface.h"
#include "PppInterface.h"
#include "PsIfaceGlobalDef.h"
#include "TafPsApi.h"
#include "AtCmdMsgProc.h"
#include "AtInternalMsg.h"
#include "acore_nv_stru_gucttf.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 ??????
*****************************************************************************/

/*lint -e778 */
/*lint -e572 */
#ifndef VOS_NTOHL                   /* ??????????????*/
#if VOS_BYTE_ORDER==VOS_BIG_ENDIAN
#define VOS_NTOHL(x)    (x)
#define VOS_HTONL(x)    (x)
#define VOS_NTOHS(x)    (x)
#define VOS_HTONS(x)    (x)
#else
#define VOS_NTOHL(x)    ((((x) & 0x000000ffU) << 24) | \
             (((x) & 0x0000ff00U) <<  8) | \
             (((x) & 0x00ff0000U) >>  8) | \
             (((x) & 0xff000000U) >> 24))

#define VOS_HTONL(x)    ((((x) & 0x000000ffU) << 24) | \
             (((x) & 0x0000ff00U) <<  8) | \
             (((x) & 0x00ff0000U) >>  8) | \
             (((x) & 0xff000000U) >> 24))

#define VOS_NTOHS(x)    ((((x) & 0x00ff) << 8) | \
             (((x) & 0xff00) >> 8))

#define VOS_HTONS(x)    ((((x) & 0x00ff) << 8) | \
             (((x) & 0xff00) >> 8))
#endif  /* _BYTE_ORDER==_LITTLE_ENDIAN */
#endif
/*lint -e572 */
/*lint -e778 */

#define AT_PPP_CODE_REQ                 (1)
#define AT_PPP_CODE_ACK                 (2)
#define AT_PPP_DEF_ID                   (1)

#define AT_AP_PPP_CODE_CHALLENGE        (1)
#define AT_AP_PPP_CODE_RESPONSE         (2)

#define AT_AP_PPP_PRIMARY_DNS           (129)
#define AT_AP_PPP_PRIMARY_WINNS         (130)
#define AT_AP_PPP_SECONDARY_DNS         (131)
#define AT_AP_PPP_SECONDARY_WINNS       (132)

#define AT_AP_PPP_CHAP_CHALLENGE_LEN    (16)
#define AT_AP_PPP_CHAP_RESPONSE_LEN     (16)
#define AT_AP_MAX_IPV4_ADDR_LEN         (15)

#define AT_AP_AUTH_NO_PROTO             (0)
#define AT_AP_AUTH_PAP_PROTO            (1)
#define AT_AP_AUTH_CHAP_PROTO           (2)

#define AT_DEF_DISPLAY_SPEED            (81920000)                              /* 10 M*/

#define AT_MAX_IPV4_DNS_LEN             (16)
#define AT_MAX_IPV4_NBNS_LEN            (16)
#define AT_MAX_IPV6_DNS_LEN             (16)
#define AT_MAX_IPV6_NBNS_LEN            (16)

#define AT_MAX_IPV6_STR_DOT_NUM         (3)
#define AT_MAX_IPV4V6_STR_COLON_NUM     (6)
#define AT_MAX_IPV6_STR_COLON_NUM       (7)

#define AT_IPV6_STR_MAX_TOKENS          (16)                            /* IPV6?????????????????????????????????? */
#define AT_IPV4_STR_DELIMITER           '.'                             /* RFC??????????IPV4???????????????????????? */
#define AT_IPV6_STR_DELIMITER           ':'                             /* RFC2373??????IPV6???????????????????????? */

#if (FEATURE_ON == FEATURE_IPV6)
#define AT_GetIpv6Capability()          (AT_GetCommPsCtxAddr()->ucIpv6Capability)
#endif

#define AT_PS_GET_SHARE_PDP_FLG()       (AT_GetCommPsCtxAddr()->ucSharePdpFlag)

/* ????APP??????ID */
#define AT_APP_GET_CLIENT_ID()          (gastAtClientTab[AT_CLIENT_TAB_APP_INDEX].usClientId)

/*----------------------------------------------------------------------
   ^NDISSTAT: <stat>[,<err_code>[,<wx_stat>[,<PDP_type]]]
   <err_code> value defined as follows:
   0      - Unknown error/Unspecified error
   others - (E)SM Cause
            SM Cause is defined in TS 24.008 section 10.5.6.6
            ESM Cause is defined in TS 24.301 section 9.9.4.4
*---------------------------------------------------------------------*/
#define AT_NDISSTAT_ERR_UNKNOWN         (0)

/* NDIS????????NDISCONN??NDISDUP???????????? index */
#define AT_NDIS_CID_IDX                 (0)
#define AT_NDIS_CONN_IDX                (1)
#define AT_NDIS_APN_IDX                 (2)
#define AT_NDIS_NAME_IDX                (3)
#define AT_NDIS_PWD_IDX                 (4)
#define AT_NDIS_AUTH_IDX                (5)

/* ????NDIS??????ID */
#define AT_NDIS_GET_CLIENT_ID()         (gastAtClientTab[AT_CLIENT_TAB_NDIS_INDEX].usClientId)

#define AT_APS_IP6_ADDR_PREFIX_BYTE_LEN (8)

#define AT_MAC_LEN                      (6)
#define AT_IPV6_ADDR_PREFIX_BYTE_LEN    (8)
#define AT_IPV6_ADDR_PREFIX_BIT_LEN     (64)

#define AT_REG_FC                       (1)
#define AT_DEREG_FC                     (0)

/* QOS_TRAFFIC_CLASS */
#define AT_QOS_TRAFFIC_CLASS_SUBSCRIBE      (0)
#define AT_QOS_TRAFFIC_CLASS_CONVERSATIONAL (1)
#define AT_QOS_TRAFFIC_CLASS_STREAMING      (2)
#define AT_QOS_TRAFFIC_CLASS_INTERACTIVE    (3)
#define AT_QOS_TRAFFIC_CLASS_BACKGROUND     (4)

#define AT_IPV6_STR_RFC2373_TOKENS      (8)                 /* ???????????????????? */

#define AT_IPPROTO_UDP                  (0x11)              /* IP??????UDP??????*/
#define AT_IP_VERSION                   (4)                 /* IP??????IP V4?????? */
#define AT_IP_DEF_TTL                   (0xFF)              /* IP??????IP TTL?????? */
#define AT_IP_RAND_ID                   (61468)             /* IP????ID??????????*/
#define AT_IP_HDR_LEN                   (20)                /* IP ???????? */
#define AT_UDP_HDR_LEN                  (8)                 /* UDP ???????? */

/* ??????????CID??????????????????1??2??3 */
#define AT_PS_USER_CID_1                (1)
#define AT_PS_USER_CID_2                (2)
#define AT_PS_USER_CID_3                (3)

/* PS??????????CID */
#define AT_PS_CALL_INVALID_CID          (0xFF)
#define AT_PS_CALL_INVALID_CALLID       (0xFF)

/* IPV6??????8?????????????????????????? */
#define AT_PS_IS_IPV6_ADDR_IID_VALID(aucIpv6Addr)\
            !((aucIpv6Addr[8] == 0x00) && (aucIpv6Addr[9] == 0x00)\
             && (aucIpv6Addr[10] == 0x00) && (aucIpv6Addr[11] == 0x00)\
             && (aucIpv6Addr[12] == 0x00) && (aucIpv6Addr[13] == 0x00)\
             && (aucIpv6Addr[14] == 0x00) && (aucIpv6Addr[15] == 0x00))

#if (FEATURE_ON == FEATURE_IPV6)
/* IPv6 ????ID, ??????64bit */
#define AT_PS_IPV6_IID_LEN              (8)
#define AT_PS_IPV6_IID_OFFSET           (8)
#endif

#define AT_PS_INVALID_RMNET_ID          (0xFFFF)
#define AT_PS_INVALID_IFACE_ID          (0xFF)

#if (FEATURE_ON == FEATURE_IPV6)
#define AT_PS_IS_PDP_TYPE_SUPPORT(pdptype)  \
            ( (TAF_PDP_IPV4 == (pdptype))   \
             || (TAF_PDP_IPV6 == (pdptype)) \
             || (TAF_PDP_IPV4V6 == (pdptype)))
#else
#define AT_PS_IS_PDP_TYPE_SUPPORT(pdptype)  \
            (TAF_PDP_IPV4 == (pdptype))
#endif

#define AT_PS_IS_RABID_VALID(ucRabId)\
    (((ucRabId) >= AT_PS_MIN_RABID) && ((ucRabId) <= AT_PS_MAX_RABID))

#define AT_PS_GET_CURR_CMD_OPT(index)   (gastAtClientTab[index].CmdCurrentOpt)
#define AT_PS_GET_CURR_DATA_MODE(index) (gastAtClientTab[index].DataMode)
#define AT_PS_GET_PPPID(index)          (gastAtClientTab[index].usPppId)

#define AT_PS_GET_RPT_CONN_RSLT_FUNC_TBL_PTR()      (g_astAtRptConnectedResultTab)
#define AT_PS_GET_RPT_CONN_RSLT_FUNC_TBL_SIZE()     (AT_ARRAY_SIZE(g_astAtRptConnectedResultTab))

#define AT_PS_GET_RPT_END_RSLT_FUNC_TBL_PTR()       (g_astAtRptEndedResultTab)
#define AT_PS_GET_RPT_END_RSLT_FUNC_TBL_SIZE()      (AT_ARRAY_SIZE(g_astAtRptEndedResultTab))

#define AT_PS_GET_SND_PDP_ACT_IND_FUNC_TBL_PTR()    (g_astAtSndPdpActIndTab)
#define AT_PS_GET_SND_PDP_ACT_IND_FUNC_TBL_SIZE()   (AT_ARRAY_SIZE(g_astAtSndPdpActIndTab))

#define AT_PS_GET_SND_PDP_DEACT_IND_FUNC_TBL_PTR()  (g_astAtSndPdpDeActIndTab)
#define AT_PS_GET_SND_PDP_DEACT_IND_FUNC_TBL_SIZE() (AT_ARRAY_SIZE(g_astAtSndPdpDeActIndTab))

#define AT_PS_GET_CHDATA_RNIC_RMNET_ID_TBL_PTR()    (g_astAtChdataRnicRmNetIdTab)
#define AT_PS_GET_CHDATA_RNIC_RMNET_ID_TBL_SIZE()   (AT_ARRAY_SIZE(g_astAtChdataRnicRmNetIdTab))

#define AT_PS_GET_APP_CALL_RNIC_IFACE_ID_TBL_PTR()  (g_astAtPsAppCallRnicIFaceIdTab)
#define AT_PS_GET_APP_CALL_RNIC_IFACE_ID_TBL_SIZE() (AT_ARRAY_SIZE(g_astAtPsAppCallRnicIFaceIdTab))

#define AT_PS_GET_NDIS_CALL_IFACE_ID_TBL_PTR()      (g_astAtPsNdisCallIFaceIdTab)
#define AT_PS_GET_NDIS_CALL_IFACE_ID_TBL_SIZE()     (AT_ARRAY_SIZE(g_astAtPsNdisCallIFaceIdTab))

#define AT_PS_GET_FC_IFACE_ID_TBL_PTR()             (g_astAtPsFcIFaceIdTab)
#define AT_PS_GET_FC_IFACE_ID_TBL_SIZE()            (AT_ARRAY_SIZE(g_astAtPsFcIFaceIdTab))

#define AT_PS_DIAL_RAT_TYPE_NO_ASTRICT              (0)
#define AT_PS_DIAL_RAT_TYPE_1X_OR_HRPD              (24)
#define AT_PS_DIAL_RAT_TYPE_LTE_OR_EHRPD            (36)

#define IMS_PCSCF_ADDR_PORT_MAX         (65535)

#define AT_BUILD_EXRABID(i,j)           (((i << 6) & 0xC0) | (j & 0x3F))
#define AT_GET_RABID_FROM_EXRABID(i)    (i & 0x3F)

#define AT_IPV6_ADDR_MASK_FORMAT_STR_LEN        (256)
#define AT_IPV6_ADDR_DEC_FORMAT_STR_LEN         (128)
#define AT_IPV6_ADDR_COLON_FORMAT_STR_LEN       (40)
#define AT_IPV6_ADDR_DEC_TOKEN_NUM              (16)
#define AT_IPV6_ADDR_HEX_TOKEN_NUM              (8)
#define AT_IP_STR_DOT_DELIMITER                 '.'
#define AT_IP_STR_COLON_DELIMITER               ':'

#define AT_WAIT_WLAN_ACT_PDN_CNF_TIMER_LEN       (25 * 1000)                    /* AT????Wlan ????active cnf ?????????? */

/* AT_WAIT_WLAN_ACT_PDN_CNF_TIMER?????????? */
#define AT_SET_WLAN_ACT_PDN_CNF_TMR_NAME(ulTmrName)\
            (ulTmrName)  = AT_WAIT_WLAN_ACT_PDN_CNF_TIMER;\
            (ulTmrName) |= AT_INTERNAL_PROCESS_TYPE

/* AT_WAIT_WLAN_ACT_PDN_CNF_TIMER?????????? */
#define AT_SET_WLAN_ACT_PDN_CNF_TMR_PARAM(ulTmrParam, ucIndex, ucCallId)\
            ((ulTmrParam) = ((ucCallId) << 8) | (ucIndex))

/* ??AT_WAIT_WLAN_ACT_PDN_CNF_TIMER????????????????????CALLID */
#define AT_GET_WLAN_ACT_PDN_CNF_CALLID_FROM_TMR_PARAM(ulTmrParam)\
            ((VOS_UINT8)(((ulTmrParam) & 0x0000FF00) >> 8))

/* ??AT_WAIT_WLAN_ACT_PDN_CNF_TIMER????????????????????????ID */
#define AT_GET_WLAN_ACT_PDN_CNF_CLIENTID_FROM_TMR_PARAM(ulTmrParam)\
            ((VOS_UINT8)((ulTmrParam) & 0x000000FF))

#define AT_WAIT_WLAN_DEACT_PDN_CNF_TIMER_LEN       (7 * 1000)                   /* AT????Wlan ????deactive cnf ?????????? */

/* AT_WAIT_WLAN_DEACT_PDN_CNF_TIMER?????????? */
#define AT_SET_WLAN_DEACT_PDN_CNF_TMR_NAME(ulTmrName)\
            (ulTmrName)  = AT_WAIT_WLAN_DEACT_PDN_CNF_TIMER;\
            (ulTmrName) |= AT_INTERNAL_PROCESS_TYPE

/* AT_WAIT_WLAN_DEACT_PDN_CNF_TIMER?????????? */
#define AT_SET_WLAN_DEACT_PDN_CNF_TMR_PARAM(ulTmrParam, ucIndex, ucCallId)\
            ((ulTmrParam) = ((ucCallId) << 8) | (ucIndex))

/* ??AT_WAIT_WLAN_DEACT_PDN_CNF_TIMER????????????????????CALLID */
#define AT_GET_WLAN_DEACT_PDN_CNF_CALLID_FROM_TMR_PARAM(ulTmrParam)\
            ((VOS_UINT8)(((ulTmrParam) & 0x0000FF00) >> 8))

/* ??AT_WAIT_WLAN_DEACT_PDN_CNF_TIMER????????????????????????ID */
#define AT_GET_WLAN_DEACT_PDN_CNF_CLIENTID_FROM_TMR_PARAM(ulTmrParam)\
            ((VOS_UINT8)((ulTmrParam) & 0x000000FF))


/* AT_PROTECT_PDN_IN_DATA_SYS_TIMER?????????? */
#define AT_SET_PROTECT_PDN_IN_DATA_SYS_TMR_NAME(ulTmrName)\
            (ulTmrName)  = AT_PROTECT_PDN_IN_DATA_SYS_TIMER;\
            (ulTmrName) |= AT_INTERNAL_PROCESS_TYPE

/* AT_PROTECT_PDN_IN_DATA_SYS_TIMER?????????? */
#define AT_SET_PROTECT_PDN_IN_DATA_SYS_TMR_PARAM(ulTmrParam, ucIndex, ucCallId)\
            ((ulTmrParam) = ((ucCallId) << 8) | (ucIndex))

/* ??AT_PROTECT_PDN_IN_DATA_SYS_TIMER????????????????????CALLID */
#define AT_GET_PROTECT_PDN_IN_DATA_SYS_CALLID_FROM_TMR_PARAM(ulTmrParam)\
            ((VOS_UINT8)(((ulTmrParam) & 0x0000FF00) >> 8))

/* ??AT_PROTECT_PDN_IN_DATA_SYS_TIMER????????????????????????ID */
#define AT_GET_PROTECT_PDN_IN_DATA_SYS_CLIENTID_FROM_TMR_PARAM(ulTmrParam)\
            ((VOS_UINT8)((ulTmrParam) & 0x000000FF))

/* ??CID????SERVICE TYPE */
#define AT_PS_CALL_GET_SERVICE_TYPE_FROM_CID(ucCid)\
            ((VOS_INT8)(ucCid + 0x20))

/* ??SERVICE TYPE????CID */
#define AT_PS_CALL_GET_CID_FROM_SERVICE_TYPE(cServiceType)\
            ((VOS_UINT8)(cServiceType - 0x20))

/* SERVICE TYPE???????? */
#define AT_PS_IS_SERVICE_TYPE_VALID(cServiceType) \
            (cServiceType > 0x20)

/* A??????0.0.0.0 ~ 127.255.255.255 */
#define AT_IPV4_CLASS_A_BEGIN           (0x00000000)
#define AT_IPV4_CLASS_A_END             (0x7FFFFFFF)

/* B??????128.0.0.0 ~ 191.255.255.255 */
#define AT_IPV4_CLASS_B_BEGIN           (0x80000000)
#define AT_IPV4_CLASS_B_END             (0xBFFFFFFF)

/* C??????192.0.0.0 ~ 223.255.255.255 */
#define AT_IPV4_CLASS_C_BEGIN           (0xC0000000)
#define AT_IPV4_CLASS_C_END             (0xDFFFFFFF)

/* IPV4 ??????A?????? */
#define AT_IS_CLASS_A_IPV4_ADDR(ulIpv4Addr)  \
            (ulIpv4Addr <= AT_IPV4_CLASS_A_END)

/* IPV4 ??????B?????? */
#define AT_IS_CLASS_B_IPV4_ADDR(ulIpv4Addr)  \
            ((ulIpv4Addr >= AT_IPV4_CLASS_B_BEGIN) && (ulIpv4Addr <= AT_IPV4_CLASS_B_END))

/* A?????????????????? 255.0.0.0 */
#define AT_IPV4_CLASS_A_SUBNET_MASK     (0xFF000000)

/* B?????????????????? 255.255.0.0 */
#define AT_IPV4_CLASS_B_SUBNET_MASK     (0xFFFF0000)

/* C?????????????????? 255.255.255.0 */
#define AT_IPV4_CLASS_C_SUBNET_MASK     (0xFFFFFF00)


/*****************************************************************************
  3 ????????
*****************************************************************************/

enum AT_PDP_STATUS_ENUM
{
    AT_PDP_STATUS_DEACT                    = 0,
    AT_PDP_STATUS_ACT                      = 1,
    AT_PDP_STATUS_BUTT
};
typedef VOS_UINT32 AT_PDP_STATUS_ENUM_UINT32;

/*****************************************************************************
 ????????   : AT_PDP_TYPE_ENUM_ENUM
 ????????   :
 ASN.1 ???? :
 ????????   : PDP????
*****************************************************************************/
enum AT_PDP_TYPE_ENUM
{
    AT_PDP_IPV4                         = 1,
    AT_PDP_IPV6                         = 2,
    AT_PDP_IPV4V6                       = 3,
    AT_PDP_PPP                          = 4,
    AT_PDP_TYPE_BUTT
};
typedef VOS_UINT8 AT_PDP_TYPE_ENUM_ENUM_U8;

/*****************************************************************************
 ????????   : AT_IPV6_STR_TYPE_ENUM
 ????????   :
 ASN.1 ???? :
 ????????   : IPV6 String????????
              HEX??RFC2373????????':'??????????
              DEX??RFC????????'.'??????????
*****************************************************************************/
enum AT_IPV6_STR_TYPE_ENUM
{
    AT_IPV6_STR_TYPE_HEX                = 0x01,
    AT_IPV6_STR_TYPE_DEC                = 0x02,

    AT_IPV6_STR_TYPE_BUTT               = 0xFF
};
typedef VOS_UINT8 AT_IPV6_STR_TYPE_ENUM_UINT8;

/*****************************************************************************
 ????????   : AT_IP_ADDR_TYPE_ENUM
 ????????   :
 ASN.1 ???? :
 ????????   : ??????SOURCE????LOCAL??IP ADDR
*****************************************************************************/
enum AT_IP_ADDR_TYPE_ENUM
{
    AT_IP_ADDR_TYPE_SOURCE              = 0x01,
    AT_IP_ADDR_TYPE_LOCAL               = 0x02,

    AT_IP_ADDR_TYPE_BUTT                = 0xFF
};
typedef VOS_UINT8 AT_IP_ADDR_TYPE_ENUM_UINT8;

/*****************************************************************************
 ????????   : AT_HILINK_MODE_ENUM
 ????????   :
 ASN.1 ???? :
 ????????   : HiLink????
*****************************************************************************/
enum AT_HILINK_MODE_ENUM
{
    AT_HILINK_NORMAL_MODE,
    AT_HILINK_GATEWAY_MODE,
    AT_HILINK_MODE_BUTT
};
typedef VOS_UINT8 AT_HILINK_MODE_ENUM_U8;

/* APP????????*/
/*****************************************************************************
 ????????   : AT_APP_CONN_STATE_ENUM
 ????????   :
 ASN.1 ???? :
 ????????   : APP????????
*****************************************************************************/
enum AT_APP_CONN_STATE_ENUM
{
    AT_APP_DIALING,                     /*0: indicates is connecting*/
    AT_APP_DIALED,                      /*1: indicates connected*/
    AT_APP_UNDIALED,                    /*2: indicates disconnected*/
    AT_APP_BUTT
};
typedef VOS_UINT32 AT_APP_CONN_STATE_ENUM_U32;

/*****************************************************************************
 PPP????????????????????????????????????:
 1.2G??????????????????????????????????GSM/GPRS/EDGE????????????????????,????????:
     GSM          - 9600
     GPRS         - 85600
     GPRS Class33 - 107800
     EDGE         - 236800
     EDGE Class33 - 296000
     ???? -
 2.3g??????????????RRC??????HSDPA??category??????????????????????????????????:
     RRC??????R99   - 384000
     RRC????????R99 - ????HSDPA??category????
                   6  - 3600000
                   8  - 7200000
                   9  - 10200000
                   10 - 14400000
                   12 - 1800000
                   14 - 21600000
                   18 - 28800000
                   24 - 43200000
                   26 - 57600000
                   28 - 86400000
     ????????category?????????? - 21600000
     ????????category?????????? - 7200000
*****************************************************************************/
enum PPP_RATE_DISPLAY_ENUM
{
    PPP_RATE_DISPLAY_GSM = 0,
    PPP_RATE_DISPLAY_GPRS,
    PPP_RATE_DISPLAY_GPRS_CALSS33,
    PPP_RATE_DISPLAY_EDGE,
    PPP_RATE_DISPLAY_EDGE_CALSS33,

    PPP_RATE_DISPLAY_R99,
    PPP_RATE_DISPLAY_DPA_CATEGORY_6,
    PPP_RATE_DISPLAY_DPA_CATEGORY_8,
    PPP_RATE_DISPLAY_DPA_CATEGORY_9,
    PPP_RATE_DISPLAY_DPA_CATEGORY_10,
    PPP_RATE_DISPLAY_DPA_CATEGORY_12,
    PPP_RATE_DISPLAY_DPA_CATEGORY_14,
    PPP_RATE_DISPLAY_DPA_CATEGORY_18,
    PPP_RATE_DISPLAY_DPA_CATEGORY_24,
    PPP_RATE_DISPLAY_DPA_CATEGORY_26,
    PPP_RATE_DISPLAY_DPA_CATEGORY_28,

    PPP_RATE_DISPLAY_BUTT
};
typedef VOS_UINT32 PPP_RATE_DISPLAY_ENUM_UINT32;

/*****************************************************************************
 ??????    : AT_CH_DATA_CHANNEL_ENUM
 ????????  : AT^CHDATA??????????<datachannelid>??????
*****************************************************************************/
enum AT_CH_DATA_CHANNEL_ENUM
{
    AT_CH_DATA_CHANNEL_ID_1             = 1,
    AT_CH_DATA_CHANNEL_ID_2,
    AT_CH_DATA_CHANNEL_ID_3,
    AT_CH_DATA_CHANNEL_ID_4,
    AT_CH_DATA_CHANNEL_ID_5,
    AT_CH_DATA_CHANNEL_ID_6,
    AT_CH_DATA_CHANNEL_ID_7,

    AT_CH_DATA_CHANNEL_BUTT
};
typedef VOS_UINT32 AT_CH_DATA_CHANNEL_ENUM_UINT32;


enum AT_PS_DATA_SYS_ENUM
{
    AT_PS_DATA_SYS_NONE                      = 0,
    AT_PS_DATA_SYS_CELLULAR                  = 1,
    AT_PS_DATA_SYS_WLAN                      = 2,

    AT_PS_DATA_SYS_BUTT
};
typedef VOS_UINT8 AT_PS_DATA_SYS_ENUM_UINT32;


/*****************************************************************************
  4 ??????????
*****************************************************************************/

/*****************************************************************************
  5 ????????
*****************************************************************************/

/*****************************************************************************
  6 STRUCT????
*****************************************************************************/

typedef struct
{
    VOS_UINT32                          ulDLEnqueuedPkts;                       /*????????????????*/
    VOS_UINT32                          ulDLDequeuedPkts;                       /*????????????*/
    VOS_UINT32                          ulDLDroppedPkts;                        /*????????????*/
    VOS_UINT32                          ulDLMaxBlkPkts;                         /*????????????????????*/
    VOS_UINT32                          ulDLMaxDequeueOnce;                     /*????????????????????????*/
} AT_AP_DL_DATA_Q_STAT_ST;


/* DHCP??????????????*/
typedef struct
{
    VOS_UINT32                          ulIPAddr;                               /* IP ??????????????*/
    VOS_UINT32                          ulSubNetMask;                           /* ??????????????????IP????????*/
    VOS_UINT32                          ulGateWay;                              /* ????????????????DHCP Server??????*/
    VOS_UINT32                          ulPrimDNS;                              /* ?? DNS??????????????*/
    VOS_UINT32                          ulSndDNS;                               /* ?? DNS??????????????*/
}AT_DHCP_CONFIG_STRU;


typedef struct
{
    VOS_UINT16                          usClientID;                             /* Client ID*/
    VOS_UINT8                           ucRabID;                                /* Rab ID*/
    VOS_UINT8                           ucCid;                                  /* CID*/
    AT_PDP_STATE_ENUM_U8                enState;                                /* State*/
    VOS_UINT8                           aucReserved[3];
    VOS_UINT32                          ulFlowCtrlState;                        /* Flow Ctrl State ; 1: flow ctrl ,0: no flow ctrl*/
    VOS_UINT32                          ulSpeed;                                /* Um Speed*/

    VOS_UINT32                          ulDLMaxRate;                            /* ????????????????*/
    VOS_UINT32                          ulULMaxRate;                            /* ????????????????*/
    VOS_UINT32                          ulDLCurrentRate;                        /* ????????????*/
    VOS_UINT32                          ulULCurrentRate;                        /* ????????????*/
    VOS_UINT32                          ulRateCalcPeriod;                       /* ????????????*/
    AT_DHCP_CONFIG_STRU                 stDhcpCfg;
}AT_CTRL_ENTITY_STRU;

typedef struct AT_DHCP_PARA
{
    VOS_UINT16                          usClientID;                             /* Client ID*/
    VOS_UINT8                           ucRabID;                                /* Rab ID*/
    VOS_UINT8                           ucCid;                                  /* CID*/
    VOS_UINT32                          ulSpeed;                                /* Um Speed*/
    VOS_UINT32                          ulDLMaxRate;                            /* ????????????????*/
    VOS_UINT32                          ulULMaxRate;                            /* ????????????????*/
    AT_DHCP_CONFIG_STRU                 stDhcpCfg;
}AT_DHCP_PARA_STRU;

typedef struct
{
  VOS_UINT8                             ucCode;                                 /* Request code */
  VOS_UINT8                             ucId;                                   /* Identification */
  VOS_UINT16                            usLength;                               /* Length of packet */
}AT_PPP_FRAME_HEAD_STRU;

/* IP???????? U32????????????????????*/
typedef struct
{
    VOS_UINT32                          ulIpAddr;                               /*U32????????????????????*/
}IN_ADDR_ST;


typedef struct
{
    VOS_UINT8                           ucType;
    VOS_UINT8                           ucCode;
    VOS_UINT16                          usCheckSum;
    VOS_UINT16                          usId;
    VOS_UINT16                          usSeq;
}ICMP_ECHO_HEAD_ST;

typedef struct
{
    VOS_UINT32                          ulIPAddr;                               /* IP ????*/
    VOS_UINT32                          ulPrimDNS;                              /* ?? DNS????*/
    VOS_UINT32                          ulSndDNS;                               /* ?? DNS????*/
    VOS_UINT32                          ulPrimWINNS;                            /* ?WINNS*/
    VOS_UINT32                          ulSncWINNS;                             /* ??WINNS */
}AT_DHCP_SETUP_PARAM_ST;


/* DHCP ??????????????*/
typedef struct
{
    AT_DHCP_CONFIG_STRU                 stDHCPConfig;                           /* DHCP??????????*/
    VOS_UINT32                          ulDHCPServerOn;                         /* DHCP????????????*/
}AT_DHCP_ENTITY_ST;

/* PDP????????????????????????*/
typedef struct
{
    VOS_UINT32                          ulspeed;                                /* ??????????????bps */
    AT_PDP_STATUS_ENUM_UINT32           enActiveSatus;                          /* ??????????0?????????????????? */
}AT_NDIS_PRO_STRU;

/*****************************************************************************
 ????????   : AT_PDP_SAVE_STATE_STRU
 ????????   :
 ASN.1 ???? :
 ????????   : ????????????
*****************************************************************************/
typedef struct
{

    AT_PDP_STATE_ENUM_U8                enIpv4State;
    AT_PDP_STATE_ENUM_U8                enIpv6State;
    AT_PDP_STATE_ENUM_U8                enIpv4v6State;

} AT_PDP_SAVE_STATE_INFO_STRU;

/*****************************************************************************
 ??????    : AT_CLINTID_RABID_MAP_STRU
 ????????  :
 ASN.1???? :
 ????????  :
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulUsed;   /* ????FCID????????????????????VOS_TRUE:??????VOS_FALSE:???? */
    VOS_UINT32                          ulRabIdMask;
    MODEM_ID_ENUM_UINT16                enModemId;
    FC_PRI_ENUM_UINT8                   enFcPri;
    VOS_UINT8                           aucReserve[1];                          /* ???? */
} AT_FCID_MAP_STRU;

/*****************************************************************************
 ??????     : AT_IPHDR_STRU
 ????????   :
 ASN.1????  :
 ????????   : IPv4 packet header ????
*****************************************************************************/
typedef struct
{
#if (VOS_LITTLE_ENDIAN == VOS_BYTE_ORDER)                   /* ??????????*/
    VOS_UINT8                           ucIpHdrLen  :4;     /* IP???????? */
    VOS_UINT8                           ucIpVer     :4;     /* IP??????*/
#elif (VOS_BIG_ENDIAN == VOS_BYTE_ORDER)                    /* ??????????*/
    VOS_UINT8                           ucIpVer     :4;     /* IP??????*/
    VOS_UINT8                           ucIpHdrLen  :4;     /* IP???????? */
#else
#error  "Please fix Macro VOS_BYTE_ORDER"                   /* VOS_BYTE_ORDER??????*/
#endif
    VOS_UINT8                           ucServiceType;      /* IP TOS????*/
    VOS_UINT16                          usTotalLen;         /* IP????????????*/
    VOS_UINT16                          usIdentification;   /* IP??????ID*/
    VOS_UINT16                          usOffset;           /* IP??????????*/
    VOS_UINT8                           ucTTL;              /* IPTTL*/
    VOS_UINT8                           ucProtocol;         /* IP????????????????*/
    VOS_UINT16                          usCheckSum;         /* IP??????????*/
    VOS_UINT32                          ulSrcAddr;          /* ??IP????*/
    VOS_UINT32                          ulDstAddr;          /* ????IP????*/
} AT_IPHDR_STRU;

/*****************************************************************************
 ??????     : AT_UDP_HDR_STRU
 ????????   :
 ASN.1????  :
 ????????   : UDP header ????
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usSrcPort;          /* ?????? */
    VOS_UINT16                          usDstPort;          /* ???????? */
    VOS_UINT16                          usLen;              /* UDP?????? */
    VOS_UINT16                          usChecksum;         /* UDP?????? */
} AT_UDP_HDR_STRU;

/*****************************************************************************
 ??????     : AT_UDP_PACKET_FORMAT_STRU
 ????????   :
 ASN.1????  :
 ????????   : UDP packet ????
*****************************************************************************/
typedef struct
{

    AT_IPHDR_STRU                       stIpHdr;            /* IP?? */
    AT_UDP_HDR_STRU                     stUdpHdr;           /* UDP?? */
    VOS_UINT32                          ulBody;
} AT_UDP_PACKET_FORMAT_STRU;

/*****************************************************************************
 ??????    : AT_PS_RMNET_ID_TAB
 ????????  : PS????????????ModemId,cid,FcId????????
*****************************************************************************/
typedef struct
{
    MODEM_ID_ENUM_UINT16                enModemId;
    FC_ID_ENUM_UINT8                    enFcId;
    VOS_UINT8                           ucUsrCid;
}AT_PS_RMNET_ID_TAB;

/*****************************************************************************
 ??????    : AT_CHDATA_RNIC_RMNET_ID_STRU
 ????????  : AT^CHDATA??RNIC??????????????????
*****************************************************************************/
typedef struct
{
    AT_CH_DATA_CHANNEL_ENUM_UINT32  enChdataValue;
    RNIC_DEV_ID_ENUM_UINT8          enRnicRmNetId;
    PS_IFACE_ID_ENUM_UINT8          enIfaceId;
    VOS_UINT8                       aucReserved[2];

}AT_CHDATA_RNIC_RMNET_ID_STRU;

/*****************************************************************************
 ??????    : AT_PS_APP_CALL_RNIC_IFACE_ID_STRU
 ????????  : WAN PS APP CALL??RNIC??????????????????
*****************************************************************************/
typedef struct
{
    VOS_UINT8                       ucCallIndex;
    RNIC_DEV_ID_ENUM_UINT8          enRnicRmNetId;
    PS_IFACE_ID_ENUM_UINT8          enIfaceId;
    VOS_UINT8                       ucReserved;
}AT_PS_APP_CALL_RNIC_IFACE_ID_STRU;

/*****************************************************************************
 ??????    : AT_PS_NDIS_CALL_IFACE_ID_STRU
 ????????  : WAN PS NDIS CALL??IFACE??????????????
*****************************************************************************/
typedef struct
{
    VOS_UINT8                       ucCallIndex;
    PS_IFACE_ID_ENUM_UINT8          enIfaceId;
    VOS_UINT8                       ucReserved1;
    VOS_UINT8                       ucReserved2;
}AT_PS_NDIS_CALL_IFACE_ID_STRU;

/*****************************************************************************
 ??????    : AT_PS_FC_IFACE_ID_STRU
 ????????  : FC??IFACE ID??????????????
*****************************************************************************/
typedef struct
{
    FC_ID_ENUM_UINT8                enFcId;
    PS_IFACE_ID_ENUM_UINT8          enIfaceId;
    VOS_UINT8                       ucReserved1;
    VOS_UINT8                       ucReserved2;
}AT_PS_FC_IFACE_ID_STRU;

/*****************************************************************************
 ??????    : AT_DISPLAY_RATE_STRU
 ????????  : ??????????????????????????
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucDlSpeed[AT_AP_SPEED_STRLEN + 1];
    VOS_UINT8                           ucUlSpeed[AT_AP_SPEED_STRLEN + 1];
    VOS_UINT8                           aucReserved[2];
}AT_DISPLAY_RATE_STRU;

/*****************************************************************************
 ??????  : AT_PS_WLAN_PDN_ACT_ERR_CODE_MAP_STRU
 ????????: PS????????WLAN PDN ACT??????????????
*****************************************************************************/
typedef struct
{
    TAF_PS_CAUSE_ENUM_UINT32            enPsCause;
    VOS_UINT32                          ulWlanCause;
} AT_PS_WLAN_PDN_ACT_ERR_CODE_MAP_STRU;

/*****************************************************************************
 ??????  : AT_PS_WLAN_PDN_DEACT_ERR_CODE_MAP_STRU
 ????????: PS????????WLAN PDN DEACT??????????????
*****************************************************************************/
typedef struct
{
    TAF_PS_CAUSE_ENUM_UINT32            enPsCause;
    VOS_UINT32                          ulWlanCause;
} AT_PS_WLAN_PDN_DEACT_ERR_CODE_MAP_STRU;


/*****************************************************************************
  7 UNION????
*****************************************************************************/

/*****************************************************************************
  8 OTHERS????
*****************************************************************************/

/*****************************************************************************
  9 ????????????
*****************************************************************************/

/*?????????????????????????????????????????????? */
extern VOS_UINT32                       g_ulLcStartTime;

extern AT_HILINK_MODE_ENUM_U8           g_enHiLinkMode;

extern AT_FCID_MAP_STRU                 g_stFcIdMaptoFcPri[];

extern CONST AT_PS_REPORT_CONN_RESULT_STRU    g_astAtRptConnectedResultTab[];

extern CONST AT_PS_REPORT_END_RESULT_STRU     g_astAtRptEndedResultTab[];

extern CONST AT_CHDATA_RNIC_RMNET_ID_STRU     g_astAtChdataRnicRmNetIdTab[];


/*****************************************************************************
  10 ????????
*****************************************************************************/

/*****************************************************************************
 ?? ?? ??  : AT_GetDhcpPara
 ????????  : ????DHCP????(DHCP????????????)
 ????????  : pstConfig                  - DHCP????(??????)
 ????????  : ??
 ?? ?? ??  : VOS_VOID
 ????????  :
 ????????  :
*****************************************************************************/
VOS_VOID AT_GetDhcpPara(
    AT_DHCP_PARA_STRU                  *pstConfig,
    AT_IPV4_DHCP_PARAM_STRU            *pstIpv4Dhcp
);

/******************************************************************************
 ?? ?? ??  : AT_GetDisplayRate
 ????????  : ????????????????????NAS????????????????????????????
 ????????  : *pstSpeed     ----    ??????????????
 ????????  : ??
 ?? ?? ??  : ??
 ????????  :
 ????????  :
******************************************************************************/
VOS_UINT32 AT_GetDisplayRate(
    VOS_UINT16                          usClientId,
    AT_DISPLAY_RATE_STRU               *pstSpeed
);

/******************************************************************************
 Function:      AT_CtrlGetPDPAuthType
 Description:    ????PC??????PDP??????????????????????
 Calls:
 Data Accessed:
 Data Updated:
 Input:
                 usTotalLen     PDP??????????????
 Output:
 Return:        0   no auth
                1   pap
                2   chap
******************************************************************************/
PPP_AUTH_TYPE_ENUM_UINT8 AT_CtrlGetPDPAuthType(
    VOS_UINT32                          Value,
    VOS_UINT16                          usTotalLen
);

TAF_PDP_AUTH_TYPE_ENUM_UINT8 AT_ClGetPdpAuthType(
    VOS_UINT32                          Value,
    VOS_UINT16                          usTotalLen
);


VOS_UINT32 AT_Ipv4AddrAtoi(VOS_CHAR *pcString, VOS_UINT8 *pucNumber);


VOS_UINT32 AT_Ipv4AddrItoa(VOS_CHAR *pcString, VOS_UINT8 *pucNumber);

/*****************************************************************************
 ?? ?? ??  : AT_Ipv4Addr2Str
 ????????  : IPV4??????????????????????????
 ????????  : pucNumber      - IPV4????
 ????????  : pcString       - ??????
 ?? ?? ??  : VOS_UINT32

*****************************************************************************/
VOS_UINT32 AT_Ipv4Addr2Str(
    VOS_CHAR                           *pcString,
    VOS_UINT8                          *pucNumber
);

VOS_UINT32 AT_Ipv6AddrAtoi(VOS_CHAR *pcString, VOS_UINT8 *pucNumber);

VOS_VOID AT_PcscfIpv4Addr2Str(
    VOS_CHAR                           *pcString,
    VOS_UINT8                          *pucNumber
);

VOS_UINT32  AT_PortAtoI(
    VOS_CHAR                         *pcString,
    VOS_UINT32                       *pulValue
);

VOS_UINT32  AT_Ipv6PcscfDataToAddr(
    VOS_UINT8                          *pucStr,
    VOS_UINT8                          *pucIpAddr,
    VOS_UINT32                         *pulPortExistFlg,
    VOS_UINT32                         *pulPortNum

);

/*****************************************************************************
 ?? ?? ??  : AT_Ipv6AddrToStr
 ????????  : ??IPV6????????????????????????
 ????????  : aucIpAddr[]    - IPV6????(????????)
             enIpStrType    - IPV6??????????????????
 ????????  : aucAddrStr[]   - IPV6????(??????????)
 ?? ?? ??  : VOS_OK         - ????????
             VOS_ERR        - ????????
 ????????  :
 ????????  :
*****************************************************************************/
VOS_UINT32 AT_Ipv6AddrToStr(
    VOS_UINT8                           aucAddrStr[],
    VOS_UINT8                           aucIpAddr[],
    AT_IPV6_STR_TYPE_ENUM_UINT8         enIpStrType
);

/*****************************************************************************
 ?? ?? ??  : AT_Itoa
 ????????  : ????????????(10??16), ????????????ASCII??, ??????????????????
 ????????  : usValue    - ????????ASCII????????
             pcStr      - ????????????????
             usRadix    - ????????
 ????????  : ??
 ?? ?? ??  : VOS_CHAR*
 ????????  :
 ????????  :
*****************************************************************************/
VOS_CHAR* AT_Itoa(
    VOS_UINT16                          usValue,
    VOS_CHAR                           *pcStr,
    VOS_UINT16                          usRadix,
    VOS_UINT32                          ulLength
);
/******************************************************************************
 ?? ?? ??  : AT_AtoI
 ????????  : ????????????
 ????????  : pString ??????
 ????????  : ??
 ?? ?? ??  : ???? IP????
 ????????  :
 ????????  :
******************************************************************************/
VOS_UINT64  AT_AtoI(
    VOS_UINT8                         *pString
);


/******************************************************************************
 ?? ?? ??  : AT_AtoI
 ????????  : ????????????(??????????????)
 ????????  : pString ??????
 ????????  : ??
 ?? ?? ??  : ???? IP????
 ????????  :
 ????????  :
******************************************************************************/
VOS_INT32  AT_AtoInt(
    VOS_UINT8                          *pString,
    VOS_INT32                          *pOut
);

/*****************************************************************************
 ?? ?? ??  : AT_ConvertIpv6AddrToCompressedStr
 ????????  : ??IPV6????????????????????????????
 ????????  : aucIpAddr[]    - IPV6????(????????)
             ucTokensNum    - ??????????
 ????????  : aucAddrStr[]   - IPV6????(??????????)
 ?? ?? ??  : VOS_OK         - ????????
             VOS_ERR        - ????????
 ????????  :
 ????????  :
*****************************************************************************/
VOS_UINT32 AT_ConvertIpv6AddrToCompressedStr(
    VOS_UINT8                           aucAddrStr[],
    VOS_UINT8                           aucIpAddr[],
    VOS_UINT8                           ucTokensNum
);

/*****************************************************************************
 ?? ?? ??  : AT_BuildUdpHdr
 ????????  : ????IP??UDP????????(??????????????????UDP??)
 ????????  : pstUdpPkt  - UDP??????
             usLen      - UDP??????
             ulSrcAddr  - ??IP????
             ulDstAddr  - ????IP????
             usSrcPort  - ????????
             usDstPort  - ??????????
 ????????  : ??
 ?? ?? ??  : VOS_OK     - ????????
             VOS_ERR    - ????????
 ????????  :
 ????????  :
*****************************************************************************/
VOS_UINT32 AT_BuildUdpHdr(
    AT_UDP_PACKET_FORMAT_STRU          *pstUdpPkt,
    VOS_UINT16                          usLen,
    VOS_UINT32                          ulSrcAddr,
    VOS_UINT32                          ulDstAddr,
    VOS_UINT16                          usSrcPort,
    VOS_UINT16                          usDstPort
);


/*****************************************************************************
 ?? ?? ??  : AT_NidsGet3gppSmCauseByPsCause
 ????????  : ??PS??????????????????3GPP??????????(E)SM Cause, ??3GPP????????
             ??????????????????0(Unknown error/Unspecified error)
 ????????  : enCause - PS????????????
 ????????  : ??
 ?? ?? ??  : VOS_UINT16
 ????????  :
 ????????  :
*****************************************************************************/
VOS_UINT32 AT_Get3gppSmCauseByPsCause(
    TAF_PS_CAUSE_ENUM_UINT32            enCause
);

VOS_VOID AT_NDIS_ConnStatusChgProc(NCM_IOCTL_CONNECT_STUS_E enStatus);

#if (FEATURE_ON == FEATURE_IPV6)

VOS_VOID AT_PS_ProcIpv6RaFail(
    TAF_PS_IPV6_INFO_IND_STRU          *pstRaInfoNotifyInd,
    VOS_UINT8                           ucCallId
);


VOS_VOID AT_PS_ProcHoIpv6RaInfo(
    TAF_PS_IPV6_INFO_IND_STRU          *pstRaInfoNotifyInd,
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity,
    VOS_UINT8                           ucCallId
);


VOS_VOID AT_PS_FillIpv6RaInfo(
    TAF_PS_IPV6_INFO_IND_STRU          *pstRaInfoNotifyInd,
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity
);
#endif


VOS_VOID  AT_ModemPsRspPdpActEvtRejProc(
    VOS_UINT8                           ucIndex,
    TAF_PS_CALL_PDP_ACTIVATE_REJ_STRU  *pEvent
);



VOS_VOID  AT_ModemPsRspPdpActEvtCnfProc(
    VOS_UINT8                           ucIndex,
    TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU  *pEvent
);


VOS_VOID  AT_ModemPsRspPdpDeactEvtCnfProc(
    VOS_UINT8                           ucIndex,
    TAF_PS_CALL_PDP_DEACTIVATE_CNF_STRU *pEvent
);

/*****************************************************************************
 ?? ?? ??  : AT_MODEM_ProcCallEndedEvent
 ????????  : ????PS_CALL_END_CNF????
 ????????  : ucIndex  - ????????
             pstEvent - ID_EVT_TAF_PS_CALL_END_CNF????????
 ????????  : ??
 ?? ?? ??  : VOS_VOID
*****************************************************************************/
VOS_VOID AT_MODEM_ProcCallEndCnfEvent(
    VOS_UINT8                           ucIndex,
    TAF_PS_CALL_END_CNF_STRU           *pstEvent
);


/*****************************************************************************
 ?? ?? ??  : AT_MODEM_HangupCall
 ????????  : ????PPP????????
 ????????  : ucIndex - ????????
 ????????  : ??
 ?? ?? ??  : AT_XXX  - ATC??????
*****************************************************************************/
VOS_UINT32 AT_MODEM_HangupCall(VOS_UINT8 ucIndex);


VOS_VOID  AT_AnswerPdpActInd(
    VOS_UINT8                           ucIndex,
    TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU  *pstEvent
);


VOS_VOID  AT_ModemPsRspPdpDeactivatedEvtProc(
    VOS_UINT8                           ucIndex,
    TAF_PS_CALL_PDP_DEACTIVATE_IND_STRU *pEvent
);

/*****************************************************************************
 ?? ?? ??  : AT_DeRegModemPsDataFCPoint
 ????????  : ??????Modem????????????
 ????????  : MN_CLIENT_ID_T                      usClientID,
             VOS_UINT8                           ucRabId
 ????????  : ??
 ?? ?? ??  : VOS_UINT32
 ????????  :
 ????????  :
*****************************************************************************/
extern VOS_UINT32 AT_DeRegModemPsDataFCPoint(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucRabId
);

#if( FEATURE_ON == FEATURE_CSD )
/*****************************************************************************
 ?? ?? ??  : AT_RegModemVideoPhoneFCPoint
 ????????  : ????Modem????CST????????
 ????????  : FC_ID_ENUM_UINT8                    enFcId
 ????????  : ??
 ?? ?? ??  : VOS_UINT32
 ????????  :
 ????????  :
*****************************************************************************/
VOS_UINT32 AT_RegModemVideoPhoneFCPoint(
    VOS_UINT8                           ucIndex,
    FC_ID_ENUM_UINT8                    enFcId
);

/*****************************************************************************
 ?? ?? ??  : AT_DeRegModemVideoPhoneFCPoint
 ????????  : ??????Modem????CST????????
 ????????  :
 ????????  : ??
 ?? ?? ??  : VOS_UINT32
 ????????  :
 ????????  :
*****************************************************************************/
VOS_UINT32 AT_DeRegModemVideoPhoneFCPoint(VOS_UINT8 ucIndex);
#endif

VOS_VOID AT_PS_SendNdisIPv6PdnInfoCfgReq(
    MODEM_ID_ENUM_UINT16                 enModemId,
    AT_PS_CALL_ENTITY_STRU              *pstCallEntity,
    TAF_PS_IPV6_INFO_IND_STRU           *pIPv6RaNotify
);

#if(FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
VOS_VOID AT_PS_SendNdisIPv6IfaceUpCfgInd(
    AT_PS_CALL_ENTITY_STRU              *pstCallEntity,
    TAF_PS_IPV6_INFO_IND_STRU           *pIPv6RaNotify
);
#endif


VOS_VOID AT_NotifyFcWhenPdpModify(
    TAF_PS_CALL_PDP_MODIFY_CNF_STRU    *pstEvent,
    FC_ID_ENUM_UINT8                    enFcId
);

#if (FEATURE_ON == FEATURE_LTE)
/* AT??????FTM ???????????? */
VOS_UINT32 atSendFtmDataMsg(VOS_UINT32 TaskId, VOS_UINT32 MsgId, VOS_UINT32 ulClientId, VOS_VOID* pData, VOS_UINT32 uLen);
#endif

/*****************************************************************************
 ?? ?? ??  : AT_PS_AddIpAddrRabIdMap
 ????????  : ????????IP??RABID??????
 ????????  : usClientId - ??????ID
             pstEvent   - PS??????????
 ????????  : ??
 ?? ?? ??  : VOS_VOID
*****************************************************************************/
VOS_VOID AT_PS_AddIpAddrRabIdMap(
    VOS_UINT16                          usClientId,
    TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU  *pstEvent
);


/*****************************************************************************
 ?? ?? ??  : AT_PS_DeleteIpAddrRabIdMap
 ????????  : ????????IP??RABID??????
 ????????  : usClientId - ??????ID
             pstEvent   - PS??????????
 ????????  : ??
 ?? ?? ??  : VOS_VOID
*****************************************************************************/
VOS_VOID AT_PS_DeleteIpAddrRabIdMap(
    VOS_UINT16                          usClientId,
    TAF_PS_CALL_PDP_DEACTIVATE_CNF_STRU *pstEvent
);


/*****************************************************************************
 ?? ?? ??  : AT_PS_GetIpAddrByRabId
 ????????  : ????RABID????????IP????
 ????????  : usClientId - ??????ID
             ucRabId    - RABID [5,15]
 ????????  : ??
 ?? ?? ??  : ulIpAddr   - IP????(??????)
*****************************************************************************/
VOS_UINT32 AT_PS_GetIpAddrByRabId(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucRabId
);

VOS_VOID AT_PS_SetPsCallErrCause(
    VOS_UINT16                          usClientId,
    TAF_PS_CAUSE_ENUM_UINT32            enPsErrCause
);

TAF_PS_CAUSE_ENUM_UINT32 AT_PS_GetPsCallErrCause(
    VOS_UINT16                          usClientId
);

VOS_UINT32 AT_PS_GetChDataValueFromRnicRmNetId(
    RNIC_DEV_ID_ENUM_UINT8              enRnicRmNetId,
    AT_CH_DATA_CHANNEL_ENUM_UINT32     *penDataChannelId
);

CONST AT_CHDATA_RNIC_RMNET_ID_STRU *AT_PS_GetChDataCfgByChannelId(
    VOS_UINT8                           ucIndex,
    AT_CH_DATA_CHANNEL_ENUM_UINT32      enDataChannelId
);

AT_PS_CALL_ENTITY_STRU* AT_PS_GetCallEntity(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId
);

VOS_UINT8 AT_PS_TransCidToCallId(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCid
);

VOS_UINT32 AT_PS_IsCallIdValid(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId
);

VOS_VOID AT_PS_ReportDCONN(
    AT_PS_USER_INFO_STRU               *pstUsrInfo,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);

VOS_VOID AT_PS_ReportNdisStatConn(
    AT_PS_USER_INFO_STRU               *pstUsrInfo,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);

VOS_VOID AT_PS_ReportDEND(
    AT_PS_USER_INFO_STRU               *pstUsrInfo,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType,
    TAF_PS_CAUSE_ENUM_UINT32            enCause
);

VOS_VOID AT_PS_ReportNdisStatEnd(
    AT_PS_USER_INFO_STRU               *pstUsrInfo,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType,
    TAF_PS_CAUSE_ENUM_UINT32            enCause
);

AT_PS_CALL_TYPE_ENUM_U8 AT_PS_GetPsCallType(VOS_UINT8 ucIndex);

VOS_VOID  AT_PS_SndRnicPdpActInd(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity,
    TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU  *pstEvent,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);

VOS_VOID AT_PS_SndRnicPdpDeactInd(
    AT_PS_CALL_ENTITY_STRU              *pstCallEntity,
    TAF_PS_CALL_PDP_DEACTIVATE_CNF_STRU *pstEvent,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);

VOS_VOID AT_PS_ProcNdisPdpActInd(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity,
    TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU  *pstEvent,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);

VOS_VOID AT_PS_ProcNdisPdpDeActInd(
    AT_PS_CALL_ENTITY_STRU              *pstCallEntity,
    TAF_PS_CALL_PDP_DEACTIVATE_CNF_STRU *pstEvent,
    TAF_PDP_TYPE_ENUM_UINT8              enPdpType
);

#if (FEATURE_ON == FEATURE_IPV6)
VOS_UINT32 AT_PS_IsIpv6CapabilityValid(VOS_UINT8 ucCapability);

VOS_UINT32 AT_CheckIpv6Capability(
    VOS_UINT8                           ucPdpType
);

VOS_UINT32 AT_PS_GenIpv6LanAddrWithRadomIID(
    VOS_UINT8                          *pucPrefix,
    VOS_UINT32                          ulPrefixByteLen,
    VOS_UINT8                          *pucIpv6LanAddr
);

VOS_UINT8 AT_CalcIpv6PrefixLength(
    VOS_UINT8                           *pucLocalIpv6Mask
);

VOS_VOID AT_GetIpv6MaskByPrefixLength(
    VOS_UINT8                            ucLocalIpv6Prefix,
    VOS_UINT8                           *pucLocalIpv6Mask
);
#endif

VOS_VOID AT_PS_ProcCallConnectedEvent(
    TAF_PS_CALL_PDP_ACTIVATE_CNF_STRU  *pstEvent
);

VOS_VOID AT_PS_ProcCallRejectEvent(
    TAF_PS_CALL_PDP_ACTIVATE_REJ_STRU   *pstEvent
);

VOS_VOID AT_PS_ProcCallEndedEvent(
    TAF_PS_CALL_PDP_DEACTIVATE_CNF_STRU *pstEvent
);

VOS_VOID AT_PS_ProcCallOrigCnfEvent(TAF_PS_CALL_ORIG_CNF_STRU *pstCallOrigCnf);

VOS_VOID AT_PS_ProcCallEndCnfEvent(TAF_PS_CALL_END_CNF_STRU *pstCallEndCnf);

VOS_UINT32 AT_PS_ProcCallModifyEvent(
    VOS_UINT8                           ucIndex,
    TAF_PS_CALL_PDP_MODIFY_CNF_STRU    *pstEvent
);

VOS_VOID AT_PS_FreeCallEntity(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    TAF_PS_APN_DATA_SYS_ENUM_UINT8      enDataSys
);

VOS_UINT32 AT_PS_ValidateDialParam(
    VOS_UINT8                           ucIndex,
    AT_PS_CALL_TYPE_ENUM_U8             enPsCallType
);

VOS_VOID AT_PS_ParseUsrInfo(
    VOS_UINT8                           ucIndex,
    AT_PS_CALL_TYPE_ENUM_U8             enPsCallType,
    AT_PS_USER_INFO_STRU               *pstUserInfo
);

TAF_PDP_TYPE_ENUM_UINT8 AT_PS_GetCurrCallType(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId
);

VOS_UINT32 AT_PS_SetupCall(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    AT_DIAL_PARAM_STRU                 *pstCallDialParam
);

VOS_UINT32 AT_PS_HangupCall(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    TAF_PS_CALL_END_CAUSE_ENUM_UINT8    enCause
);

VOS_UINT32 AT_PS_ProcDialCmd(
    VOS_UINT8                           ucIndex,
    AT_PS_CALL_TYPE_ENUM_U8             enPsCallType
);

VOS_UINT32 AT_CheckApnFormat(
    VOS_UINT8                          *pucApn,
    VOS_UINT16                          usApnLen
);

/*****************************************************************************
 ?? ?? ??  : AT_PS_SndCallConnectedResult
 ????????  : ????????????????
 ????????  : ucCallId  - ????????????
             enPdpType - PDP????
 ????????  : ??
 ?? ?? ??  : VOS_VOID
*****************************************************************************/
VOS_VOID AT_PS_SndCallConnectedResult(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);

VOS_VOID AT_PS_ReportCustomPcoInfo(
    TAF_PS_CUSTOM_PCO_INFO_STRU        *pstPcoCustInfo,
    TAF_PS_PDN_OPERATE_TYPE_ENUM_UINT8  enOperateType,
    VOS_UINT8                           ucCid,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType,
    AT_CLIENT_TAB_INDEX_UINT8           enPortIndex
);


VOS_UINT32 AT_PS_IsLinkDown(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity
);


VOS_UINT32 AT_PS_IsNeedClearCurrCall(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity,
    TAF_PS_APN_DATA_SYS_ENUM_UINT8      enDataSys
);

/*****************************************************************************
 ?? ?? ??  : AT_PS_BuildExClientId
 ????????  : ??????????ClientId(ModemId + ClientId)
 ????????  : usClientId                 - ClientId
 ????????  : ??
 ?? ?? ??  : VOS_UINT16                 - ??????????ClientId
*****************************************************************************/
VOS_UINT16 AT_PS_BuildExClientId(VOS_UINT16 usClientId);

/*****************************************************************************
 ?? ?? ??  : AT_PS_BuildPsCallExClientId
 ????????  : ??????????ClientId(ModemId + ClientId)
 ????????  : usClientId                 - ClientId
 ????????  : ??
 ?? ?? ??  : VOS_UINT16                 - ??????????ClientId
*****************************************************************************/
VOS_UINT16 AT_PS_BuildPsCallExClientId(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId
);

/*****************************************************************************
 ?? ?? ??  : AT_PS_BuildNdisExClientId
 ????????  : ??????????ClientId(ModemId + ClientId)
 ????????  : usPortIndex                - ????????
             usClientId                 - ClientId
 ????????  : ??
 ?? ?? ??  : VOS_UINT16                 - ??????????ClientId
*****************************************************************************/
VOS_UINT16 AT_PS_BuildNdisExClientId(
    VOS_UINT16                          usPortIndex,
    VOS_UINT16                          usClientId
);

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
VOS_UINT32 AT_PS_CheckDialRatType(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucBitRatType
);
#endif

VOS_VOID AT_PS_ProcRabidChangedEvent(
    TAF_PS_CALL_PDP_RABID_CHANGE_IND_STRU  *pstEvent
);


VOS_UINT32 AT_Ipv6AddrMask2FormatString(
    VOS_CHAR                           *pcIpv6FormatStr,
    VOS_UINT8                           aucIpv6Addr[],
    VOS_UINT8                           aucIpv6Mask[]
);

VOS_VOID AT_PS_SetWlanCurrCallType(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);
VOS_VOID AT_PS_SetHoCallType(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);
VOS_VOID AT_PS_SetCallHandOverFlg(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    VOS_UINT8                           ucIsHandOver
);
VOS_VOID AT_PS_SetCallHandOverFailCause(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    TAF_PS_CAUSE_ENUM_UINT32            enHoFailCause
);
TAF_PS_CAUSE_ENUM_UINT32 AT_PS_GetCallHandOverFailCause(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity
);
AT_PS_DATA_SYS_ENUM_UINT32 AT_PS_GetPreDataSystem(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity,
    VOS_UINT8                           ucIndex
);
VOS_VOID AT_PS_SetPsCallCurrentDataSys(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity,
    TAF_PS_APN_DATA_SYS_ENUM_UINT8      enCurrentDataSys
);
TAF_PS_APN_DATA_SYS_ENUM_UINT8 AT_PS_GetPsCallCurrentDataSys(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity
);
VOS_UINT8 AT_PS_GetPsCallHandOverFlg(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity
);
VOS_VOID AT_PS_RegDataSysChgNtf(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity
);
VOS_VOID AT_PS_DeRegDataSysChgNtf(
    VOS_UINT16                              usClientId,
    VOS_UINT8                               ucCallId
);
VOS_UINT32 AT_PS_DialUpInWlanAllowed(
    VOS_UINT8                               ucBitDataSystem,
    TAF_PS_APN_DATA_SYS_POLICY_ENUM_UINT8   enDataSysPolicy
);
VOS_UINT32 AT_PS_DialUpInCellularAllowed(
    VOS_UINT8                               ucBitDataSystem,
    TAF_PS_APN_DATA_SYS_POLICY_ENUM_UINT8   enDataSysPolicy
);
VOS_UINT32 AT_PS_IsWlanThrotAllowed(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity
);
VOS_VOID AT_PS_SetWlanCallStateByType(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType,
    AT_PDP_STATE_ENUM_U8                enPdpState
);
VOS_UINT32 AT_PS_ProcMapconMsg (
    VOS_UINT16                          usClientId,
    AT_MAPCON_CTRL_MSG_STRU            *pstAtMapConMsgPara
);
VOS_UINT32 AT_PS_HangupWlanCall(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucCallId,
    VOS_INT8                            cIsLocal,
    VOS_INT8                            cIsHandover
);
VOS_VOID AT_PS_PreProcPsCallDataSysChgNtf(
    VOS_UINT16                   usClientId,
    VOS_UINT32                   ulBitDataSysSwitchCid
);
VOS_UINT32 AT_PS_SndWlanMsgPdnActivateReq(
    VOS_UINT8                           ucIndex,
    TAF_PDP_TYPE_ENUM_UINT8             enIpType,
    VOS_INT8                            cIsHandover,
    VOS_UINT8                           ucCallId
);
VOS_UINT32 AT_PS_SndWlanMsgPdnDeActivateReq(
    VOS_UINT8                           ucIndex,
    VOS_INT8                            cIsLocal,
    VOS_INT8                            cIsHandover,
    VOS_UINT8                           ucCallId
);
VOS_VOID AT_PS_ReportImsCtrlMsgu(
    VOS_UINT8                                       ucIndex,
    AT_IMS_CTRL_MSG_RECEIVE_MODULE_ENUM_UINT8       enModule,
    VOS_UINT32                                      ulMsgLen,
    VOS_UINT8                                      *pucDst
);
VOS_VOID AT_PS_SelectApnDataSysConfig(
    VOS_UINT8                           ucIndex,
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity
);
VOS_VOID AT_PS_ProcWlanPowerOff (
    VOS_UINT16                          usClientId
);
TAF_PS_APN_DATA_SYS_POLICY_INFO_STRU* AT_PS_GetApnDataSysPolicyInfo(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucIndex
);
TAF_PDP_TYPE_ENUM_UINT8 AT_PS_ConvertPdpType2Cellular(IMSA_WIFI_IP_TYPE_ENUM_INT8 enImsaPdpType);
VOS_VOID AT_PS_RcvTiWlanActPdnCnfExpired(REL_TIMER_MSG *pstTmrMsg);
VOS_VOID AT_PS_RcvTiWlanDeActPdnCnfExpired(REL_TIMER_MSG *pstTmrMsg);
VOS_VOID AT_PS_RcvTiProtectPdnInDataSysExpired(REL_TIMER_MSG *pstTmrMsg);

AT_PDP_STATE_ENUM_U8 AT_PS_GetCallStateByType(
    AT_PS_CALL_ENTITY_STRU             *pstCallEntity,
    TAF_PDP_TYPE_ENUM_UINT8             enPdpType
);

FC_ID_ENUM_UINT8 AT_PS_GetFcIdByIFaceId(
    VOS_UINT32                          ulIFaceId
);

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* __AT_DATA_PROC_H__ */

