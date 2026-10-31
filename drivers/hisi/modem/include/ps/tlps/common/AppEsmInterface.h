/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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

#ifndef __APPESMINTERFACE_H__
#define __APPESMINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "AppNasComm.h"
#include "AppRrcInterface.h"
#include "NasCommPacketSer.h"


#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 Macro
*****************************************************************************/

/*IP??????????????*/
#define APP_MAX_IPV4_ADDR_LEN                               4
#define APP_MAX_IPV6_ADDR_LEN                               16
#define APP_ESM_PCSCF_ADDR_MAX_NUM                          (8)

/* Added 2016-06-20,Begin */
#define APP_MAX_IPV6_PREFIX_LEN                             8
/* Added 2016-06-20,End */

#define APP_ESM_MAX_IPV6_PREFIX_NUM     (6)
#define APP_ESM_IPV6_ADDR_LEN           (16)


#define APP_ESM_MAX_LENGTH_OF_APN       (100)

#define APP_MAX_APN_LEN                                     99
#define APP_ESM_MAX_EPSB_NUM                                (11)                /*??????????*/
#define APP_ESM_MAX_ACCESS_NUM_LEN                          32

/* ??????at????????AT^AUTHDATA??????????????????????127 */
#define APP_ESM_MAX_USER_NAME_LEN                           100
#define APP_ESM_MAX_PASSWORD_LEN                            100
/* PCO??????????????253??????????????????127????????PCO??????????????GU??AUTH????????????????????????????????99,????GU??????????????99 */
#define APP_ESM_MAX_USER_NAME_ENCODE_LEN                    99
#define APP_ESM_MAX_PASSWORD_ENCODE_LEN                     99



#define APP_ESM_MAX_SDF_PF_NUM                              16


/*APP->SM Command??????3???????????? APP->SM*/
#define APP_ESM_MSG_ID_HEADER                               PS_MSG_ID_APP_TO_ESM_BASE

/*SM->APP Command??????3???????????? SM->APP*/
#define ESM_APP_MSG_ID_HEADER                               PS_MSG_ID_ESM_TO_APP_BASE

#define APP_ESM_PPP_CHAP_CHALLNGE_LEN       (16)
#define APP_ESM_PPP_CHAP_CHALLNGE_NAME_LEN  (16)
#define APP_ESM_PPP_CHAP_RESPONSE_LEN       (16)


#define APP_ESM_PPP_MAX_USERNAME_LEN    (99)
#define APP_ESM_PPP_MAX_PASSWORD_LEN    (99)

#define APP_ESM_TFT_MAX_PF_NUM                              16

/* DT begin */
#define APP_ESM_DT_REPORT_STATUS_OPEN                       1
#define APP_ESM_DT_REPORT_STATUS_CLOSE                      0
/* DT end */

/* moded for verizon band13 pco requirement 2017-03-25, begin */
#define APP_ESM_MAX_CUSTOM_PCO_CONTAINER_NUM                (3)
#define APP_ESM_MAX_CUSTOM_PCO_CONTAINER_CONTENT_LEN        (53)
/* moded for verizon band13 pco requirement 2017-03-25, end */
/* added for Band13 R1 PHASE1, 2017-10-20, begin */
#define APP_ESM_MAX_VZW_APN_INFO_NUM                        (8)
/* added for Band13 R1 PHASE1, 2017-10-20, end */
/* added for R1 DATA DEVICE PHASE1, 2017-12-13, begin */
#define  APP_ESM_MAX_IPV4_EPDG_NUM                          (2)
#define  APP_ESM_MAX_IPV6_EPDG_NUM                          (2)
/* added for R1 DATA DEVICE PHASE1, 2017-12-13, end */


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
/*****************************************************************************
 ??????    : APP_ESM_MSG_TYPE_ENUM
 ????????  : APP??SM????????
*****************************************************************************/
enum APP_ESM_MSG_TYPE_ENUM
{
    /*????????????*/
    ID_APP_ESM_SET_TFT_REQ              = 0x01+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_TFT_REQ_STRU*/
    ID_APP_ESM_SET_TFT_CNF              = 0x02+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_TFT_CNF_STRU*/

    ID_APP_ESM_SET_QOS_REQ              = 0x03+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_QOS_REQ_STRU*/
    ID_APP_ESM_SET_QOS_CNF              = 0x04+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_QOS_CNF_STRU*/

    ID_APP_ESM_SET_EPS_QOS_REQ          = 0x05+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_EPS_QOS_REQ_STRU*/
    ID_APP_ESM_SET_EPS_QOS_CNF          = 0x06+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_EPS_QOS_CNF_STRU*/

    ID_APP_ESM_SET_APN_REQ              = 0x07+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_APN_REQ_STRU*/
    ID_APP_ESM_SET_APN_CNF              = 0x08+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_APN_CNF_STRU*/

    ID_APP_ESM_SET_PCO_REQ              = 0x09+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_PCO_REQ_STRU*/
    ID_APP_ESM_SET_PCO_CNF              = 0x0A+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_PCO_CNF_STRU*/

    ID_APP_ESM_SET_PDN_TYPE_REQ         = 0x0B+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_PDN_TYPE_REQ_STRU*/
    ID_APP_ESM_SET_PDN_TYPE_CNF         = 0x0C+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_PDN_TYPE_CNF_STRU*/

    ID_APP_ESM_SET_BEARER_TYPE_REQ      = 0x0D+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_BEARER_TYPE_REQ_STRU*/
    ID_APP_ESM_SET_BEARER_TYPE_CNF      = ESM_APP_MSG_ID_HEADER + 0x0E,/*_H2ASN_MsgChoice  APP_ESM_SET_BEARER_TYPE_CNF_STRU*/

    ID_APP_ESM_SET_PDP_MANAGER_TYPE_REQ = 0x0F+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_PDP_MANAGER_TYPE_REQ_STRU*/
    ID_APP_ESM_SET_PDP_MANAGER_TYPE_CNF = 0x10+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_PDP_MANAGER_TYPE_CNF_STRU*/

    ID_APP_ESM_SET_GW_AUTH_REQ          = 0x11+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_GW_AUTH_REQ_STRU*/
    ID_APP_ESM_SET_GW_AUTH_CNF          = 0x11+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_GW_AUTH_CNF_STRU*/
    /* modify begin */
    ID_APP_ESM_IPV6_INFO_NOTIFY         = 0x12+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_IPV6_INFO_NOTIFY_STRU*/

    ID_APP_ESM_PROCEDURE_ABORT_NOTIFY   = 0x13+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PROCEDURE_ABORT_NOTIFY_STRU*/
    /* modify end */

    /* begin 2013-05-29 Modify L4A */
    ID_APP_ESM_NDISCONN_REQ             = 0x14+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_NDISCONN_REQ_STRU*/
    ID_APP_ESM_NDISCONN_CNF             = 0x12+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_NDISCONN_CNF_STRU*/

    ID_APP_ESM_SET_CGDCONT_REQ          = 0x15+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_CGDCONT_REQ_STRU*/
    ID_APP_ESM_SET_CGDCONT_CNF          = 0x13+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_CGDCONT_CNF_STRU*/
    /* end 2013-05-29 Modify L4A */
    /* added for Band13 R1 PHASE1, 2017-10-20, begin */
    ID_APP_ESM_APN_ENABLE_INFO_CHANGE_NTF  = 0x16+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_APN_ENABLE_INFO_CHANGE_NTF_STRU*/
    /* added for Band13 R1 PHASE1, 2017-10-20, end */
    /* Added for R1 e911, 2017-12-6, begin */
    ID_APP_ESM_SET_APN_THROT_INFO_REQ= 0x17+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_APN_THROT_INFO_REQ_STRU*/
    ID_APP_ESM_SET_APN_THROT_INFO_CNF          = 0x14+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_SET_APN_THROT_INFO_CNF_STRU*/
    /* Added for R1 e911, 2017-12-6, end */
    /*????????????*/
    ID_APP_ESM_INQ_TFT_REQ              = 0x31+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_TFT_REQ_STRU*/
    ID_APP_ESM_INQ_TFT_CNF              = 0x32+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_TFT_CNF_STRU*/

    ID_APP_ESM_INQ_QOS_REQ              = 0x33+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_QOS_REQ_STRU*/
    ID_APP_ESM_INQ_QOS_CNF              = 0x34+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_QOS_CNF_STRU*/

    ID_APP_ESM_INQ_EPS_QOS_REQ          = 0x35+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_EPS_QOS_REQ_STRU*/
    ID_APP_ESM_INQ_EPS_QOS_CNF          = 0x36+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_EPS_QOS_CNF_STRU*/

    ID_APP_ESM_INQ_APN_REQ              = 0x37+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_APN_REQ_STRU*/
    ID_APP_ESM_INQ_APN_CNF              = 0x38+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_APN_CNF_STRU*/

    ID_APP_ESM_INQ_PCO_REQ              = 0x39+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_PCO_REQ_STRU*/
    ID_APP_ESM_INQ_PCO_CNF              = 0x3A+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_PCO_CNF_STRU*/

    ID_APP_ESM_INQ_PDN_TYPE_REQ         = 0x3B+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_PDN_TYPE_REQ_STRU*/
    ID_APP_ESM_INQ_PDN_TYPE_CNF         = 0x3C+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_PDN_TYPE_CNF_STRU*/

    ID_APP_ESM_INQ_BEARER_TYPE_REQ      = 0x3D+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_BEARER_TYPE_REQ_STRU*/
    ID_APP_ESM_INQ_BEARER_TYPE_CNF      = ESM_APP_MSG_ID_HEADER + 0x3E,/*_H2ASN_MsgChoice  APP_ESM_INQ_BEARER_TYPE_CNF_STRU*/

    ID_APP_ESM_INQ_PDP_MANAGER_TYPE_REQ= 0x3F+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_PDP_MANAGER_TYPE_REQ_STRU*/
    ID_APP_ESM_INQ_PDP_MANAGER_TYPE_CNF= 0x40+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_PDP_MANAGER_TYPE_CNF_STRU*/

    ID_APP_ESM_INQ_BEAER_QOS_REQ        =  0x41 + APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_BEAER_QOS_REQ_STRU*/
    ID_APP_ESM_INQ_BEAER_QOS_CNF        =  0x42 + ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_BEAER_QOS_CNF_STRU*/
    ID_APP_ESM_INQ_BEAER_QOS_IND        =  0x43 + ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_BEAER_QOS_IND_STRU*/

    ID_APP_ESM_INQ_DYNAMIC_PDP_CONT_REQ = 0x44+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_DYNAMIC_PDP_CONT_REQ_STRU*/
    ID_APP_ESM_INQ_DYNAMIC_PDP_CONT_CNF = 0x45+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_DYNAMIC_PDP_CONT_CNF_STRU*/

    ID_APP_ESM_INQ_DYNAMIC_EPS_QOS_REQ  = 0x46+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_DYNAMIC_EPS_QOS_REQ_STRU*/
    ID_APP_ESM_INQ_DYNAMIC_EPS_QOS_CNF  = 0x47+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_DYNAMIC_EPS_QOS_CNF_STRU*/

    ID_APP_ESM_INQ_DYNAMIC_TFT_REQ      = 0x48+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_DYNAMIC_TFT_REQ_STRU*/
    ID_APP_ESM_INQ_DYNAMIC_TFT_CNF      = 0x49+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_DYNAMIC_TFT_CNF_STRU*/

    ID_APP_ESM_INQ_GW_AUTH_REQ          = 0x4A+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_GW_AUTH_REQ_STRU*/
    ID_APP_ESM_INQ_GW_AUTH_CNF          = 0x4A+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_GW_AUTH_CNF_STRU*/

    /* V7R2-DT PDP   2014/03/21  start*/
    /* DT begin */
    ID_APP_ESM_DT_INQ_PDP_INFO_REQ      = 0x4B+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_PDP_INFO_REQ_STRU*/
    ID_APP_ESM_DT_INQ_PDP_INFO_CNF      = 0x4B+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_PDP_INFO_CNF_STRU*/
    ID_APP_ESM_DT_INQ_PDP_INFO_IND      = 0x4C+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_INQ_PDP_INFO_IND_STRU*/
    /* DT end */
    /* V7R2-DT PDP   2014/03/21  end*/

    ID_ESM_DSM_NOTIFICATION_IND         = 0x4D + APP_ESM_MSG_ID_HEADER,/* _H2ASN_MsgChoice ESM_DSM_NOTIFICATION_IND_STRU */
    ID_ESM_DSM_REG_CID_IND              = 0x4E + ESM_APP_MSG_ID_HEADER,/* _H2ASN_MsgChoice ESM_DSM_REG_CID_IND_STRU */
    /*????????????????????????????????*/
    ID_APP_ESM_PDP_SETUP_REQ         = 0x61+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_SETUP_REQ_STRU*/
    ID_APP_ESM_PDP_SETUP_CNF         = 0x62+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_SETUP_CNF_STRU*/
    ID_APP_ESM_PDP_SETUP_IND         = 0x63+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_SETUP_IND_STRU*/

    ID_APP_ESM_PDP_MODIFY_REQ        = 0x64+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_MODIFY_REQ_STRU*/
    ID_APP_ESM_PDP_MODIFY_CNF        = 0x65+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_MODIFY_CNF_STRU*/
    ID_APP_ESM_PDP_MODIFY_IND        = 0x66+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_MODIFY_IND_STRU*/

    ID_APP_ESM_PDP_RELEASE_REQ       = 0x67+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_RELEASE_REQ_STRU*/
    ID_APP_ESM_PDP_RELEASE_CNF       = 0x68+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_RELEASE_CNF_STRU*/
    ID_APP_ESM_PDP_RELEASE_IND       = 0x69+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_RELEASE_IND_STRU*/

    ID_APP_ESM_PDP_MANAGER_IND       = 0x6A+ESM_APP_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_MANAGER_IND_STRU*/
    ID_APP_ESM_PDP_MANAGER_RSP       = 0x6B+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_PDP_MANAGER_RSP_STRU*/

    ID_APP_ESM_NR_HANDOVER_TO_LTE_IND = 0x6C+APP_ESM_MSG_ID_HEADER,/*_H2ASN_MsgChoice  APP_ESM_NR_HANDOVER_TO_LTE_IND_STRU*/

    ID_APP_ESM_MSG_TYPE_BUTT
};
typedef VOS_UINT32 APP_ESM_MSG_TYPE_ENUM_UINT32;


/*****************************************************************************
 ??????    : APP_ESM_PARA_SET_ENUM
 ????????  : ????????????????
*****************************************************************************/
enum APP_ESM_PARA_SET_ENUM
{
    APP_ESM_PARA_SETTING                = 0x00,
    APP_ESM_PARA_DELETE                 = 0x01,
    APP_ESM_PARA_BUTT
};
typedef VOS_UINT32 APP_ESM_PARA_SET_ENUM_UINT32;

/*****************************************************************************
 ??????    : APP_ESM_PARA_SET_RESULT_ENUM
 ????????  : ????????????????
*****************************************************************************/
enum APP_ESM_PARA_SET_RESULT_ENUM
{
    APP_ESM_PARA_SET_SUCCESS            = 0x00,
    APP_ESM_PARA_SET_FAIL               = 0x01,
    APP_ESM_PARA_SET_BUTT
};
typedef VOS_UINT32 APP_ESM_PARA_SET_RESULT_ENUM_UINT32;

/*****************************************************************************
 ??????    : APP_ESM_BEARER_STATE_ENUM
 ????????  : ????????????
*****************************************************************************/
enum APP_ESM_BEARER_STATE_ENUM
{
    APP_ESM_BEARER_STATE_INACTIVE       = 0x00,
    APP_ESM_BEARER_STATE_ACTIVATED      = 0x01,
    APP_ESM_BEARER_STATE_BUTT
};
typedef VOS_UINT32 APP_ESM_BEARER_STATE_ENUM_UINT32;

/*****************************************************************************
 ??????    : APP_ESM_CAUSE_ENUM_UINT32
 ????????  : SM????????
*****************************************************************************/

typedef VOS_UINT32 APP_ESM_CAUSE_ENUM_UINT32;

/*****************************************************************************
 ??????    : APP_ESM_BEARER_TYPE_ENUM
 ????????  : ????????????
*****************************************************************************/
enum APP_ESM_BEARER_TYPE_ENUM
{
    APP_ESM_BEARER_TYPE_DEFAULT         = 0x00,
    APP_ESM_BEARER_TYPE_DEDICATED       = 0x01,
    APP_ESM_BEARER_TYPE_EMERGENCY       = 0x02,
    APP_ESM_BEARER_TYPE_BUTT
};
typedef VOS_UINT32 APP_ESM_BEARER_TYPE_ENUM_UINT32;

/*****************************************************************************
 ??????    : APP_ESM_BEARER_MODIFY_ENUM
 ????????  : ????????????????
*****************************************************************************/
enum APP_ESM_BEARER_MODIFY_ENUM
{
    APP_ESM_BEARER_MODIFY_TFT           = 0x00,
    APP_ESM_BEARER_MODIFY_QOS           = 0x01,
    APP_ESM_BEARER_MODIFY_TFT_QOS       = 0x02,
    APP_ESM_BEARER_MODIFY_BUTT
};
typedef VOS_UINT32 APP_ESM_BEARER_MODIFY_ENUM_UINT32;

/*****************************************************************************
 ??????    : APP_ESM_BEARER_ACT_TYPE_ENUM
 ????????  : ????????????????
*****************************************************************************/
enum APP_ESM_BEARER_ACT_TYPE_ENUM
{
    APP_ESM_BEARER_ACT_TYPE_ACTIVE      = 0x00,
    APP_ESM_BEARER_ACT_TYPE_MODIFY      = 0x01,
    APP_ESM_BEARER_ACT_TYPE_BUTT
};
typedef VOS_UINT32 APP_ESM_BEARER_ACT_TYPE_ENUM_UINT32;

/*****************************************************************************
 ??????    : APP_ESM_BEARER_ACT_RESULT_ENUM
 ????????  : ????????????????
*****************************************************************************/
enum APP_ESM_BEARER_ACT_RESULT_ENUM
{
    APP_ESM_BEARER_ACT_ACCEPT           = 0x00,
    APP_ESM_BEARER_ACT_REJ              = 0x01,
    APP_ESM_BEARER_ACT_BUTT
};
typedef VOS_UINT32 APP_ESM_BEARER_ACT_RESULT_ENUM_UINT32;

/*****************************************************************************
 ??????    : APP_ESM_ANSWER_MODE_ENUM
 ????????  : ????????????????
*****************************************************************************/
enum APP_ESM_ANSWER_MODE_ENUM
{
    APP_ESM_ANSWER_MODE_AUTO            = 0x00,
    APP_ESM_ANSWER_MODE_MANI            = 0x01,
    APP_ESM_ANSWER_MODE_BUTT
};
typedef VOS_UINT32 APP_ESM_ANSWER_MODE_ENUM_UINT32;

/*****************************************************************************
 ??????    : APP_ESM_ANSWER_RESULT_ENUM
 ????????  : ????????????????
*****************************************************************************/
enum APP_ESM_ANSWER_RESULT_ENUM
{
    APP_ESM_ANSWER_ACCEPT               = 0x00,
    APP_ESM_ANSWER_REJ                  = 0x01,
    APP_ESM_ANSWER_RESULT_BUTT
};
typedef VOS_UINT32 APP_ESM_ANSWER_RESULT_ENUM_UINT32;

/*****************************************************************************
 ??????    : APP_ESM_PDN_TYPE_ENUM
 ????????  : PDN????????
*****************************************************************************/
enum APP_ESM_PDN_TYPE_ENUM
{
    APP_ESM_PDN_TYPE_IPV4               = 0x01,             /*????????24301 9.9.4.10????*/
    APP_ESM_PDN_TYPE_IPV6               = 0x02,
    APP_ESM_PDN_TYPE_IPV4_IPV6          = 0x03,
    APP_ESM_PDN_TYPE_BUTT
};
typedef VOS_UINT32 APP_ESM_PDN_TYPE_ENUM_UINT32;

/*Modify for CL multimode 2014-01-09 begin*/
/*****************************************************************************
 ??????    : APP_ESM_PDN_REQUEST_TYPE_ENUM
 ????????  : PDN???????????? 24008 10.5.6.17
*****************************************************************************/
enum APP_ESM_PDN_REQUEST_TYPE_ENUM
{
    APP_ESM_PDN_REQUEST_TYPE_INITIAL    = 0x01,             /*????????????PDN????*/
    APP_ESM_PDN_REQUEST_TYPE_HANDOVER   = 0x02,             /*????non-3GPP????????PDN????*/
    APP_ESM_PDN_REQUEST_TYPE_BUTT
};
typedef VOS_UINT32 APP_ESM_PDN_REQUEST_TYPE_ENUM_UINT32;
/*Modify for CL multimode 2014-01-09 end*/


/*****************************************************************************
 ??????    : APP_ESM_CID_TYPE_ENUM
 ????????  : CID????????
*****************************************************************************/
enum APP_ESM_CID_TYPE_ENUM
{
    APP_ESM_CID_TYPE_DEFAULT             = 0x00,
    APP_ESM_CID_TYPE_DEDICATED           = 0x01,
    APP_ESM_CID_TYPE_EMERGENCY           = 0x02,
    APP_ESM_CID_TYPE_BUTT
};
typedef VOS_UINT32   APP_ESM_CID_TYPE_ENUM_UINT32;

enum APP_ESM_TRANSFER_DIRECTION_ENUM
{
    APP_ESM_TRANSFER_DIRECTION_PRE_REL7_TFT_FILTER          = 0x00,
    APP_ESM_TRANSFER_DIRECTION_DOWNLINK                     = 0x01,
    APP_ESM_TRANSFER_DIRECTION_UPLINK                       = 0x02,
    APP_ESM_TRANSFER_DIRECTION_UPLINK_DOWNLINK              = 0x03,
    APP_ESM_TRANSFER_DIRECTION_BUTT
};
typedef VOS_UINT8 APP_ESM_TRANSFER_DIRECTION_ENUM_UINT8;

enum APP_ESM_BEARER_OPERATE_TYPE_ENUM
{
    APP_ESM_BEARER_OPERATE_TYPE_DEF_ACT      = 0x00,
    APP_ESM_BEARER_OPERATE_TYPE_DEDI_ACT     = 0x01,
    APP_ESM_BEARER_OPERATE_TYPE_MODIFY       = 0x02,
    APP_ESM_BEARER_OPERATE_TYPE_BUTT
};
typedef VOS_UINT32 APP_ESM_BEARER_OPERATE_TYPE_ENUM_UINT32;

enum APP_ESM_IPV4_ADDR_ALLOC_TYPE_ENUM
{
   APP_ESM_IPV4_ADDR_ALLOC_TYPE_NAS_SIGNALING   = 0x00,
   APP_ESM_IPV4_ADDR_ALLOC_TYPE_DHCP            = 0x01,

   APP_ESM_IPV4_ADDR_ALLOC_TYPE_BUTT
};
typedef VOS_UINT32 APP_ESM_IPV4_ADDR_ALLOC_TYPE_ENUM_UINT32;

enum APP_ESM_PDP_SETUP_TYPE_ENUM
{
    APP_ESM_PDP_SETUP_TYPE_NORMAL       = 0x00,
    APP_ESM_PDP_SETUP_TYPE_PPP          = 0x01,

    APP_ESM_PDP_SETUP_TYPE_BUTT         = 0xFF
};

typedef VOS_UINT32 APP_ESM_PDP_SETUP_TYPE_ENUM_UINT32;
/* Added for DATA RETRY PHASEIII, 2016-6-23, begin */
/* ????EAB??????????APP???????????????? */
enum APP_ESM_BEARER_PRIO_ENUM
{
    APP_ESM_BEARER_PRIO_NORMAL          = 0x00,
    APP_ESM_BEARER_PRIO_LOW             = 0x01,

    APP_ESM_BEARER_PRIO_BUTT
};

typedef VOS_UINT32 APP_ESM_BEARER_PRIO_ENUM_UINT32;
/* Added for DATA RETRY PHASEIII, 2016-6-23, end */


/*****************************************************************************
 ????????: APP_ESM_AUTH_TYPE_ENUM
 ????????: Authentication Type
*****************************************************************************/
enum APP_ESM_AUTH_TYPE_ENUM
{
    APP_ESM_AUTH_TYPE_NONE              = 0x00,
    APP_ESM_AUTH_TYPE_PAP               = 0x01,
    APP_ESM_AUTH_TYPE_CHAP              = 0x02,

    APP_ESM_AUTH_TYPE_BUTT              = 0xFF
};

typedef VOS_UINT8 APP_ESM_AUTH_TYPE_ENUM_UINT8;

enum APP_ESM_BEARER_MANAGE_TYPE_ENUM
{
    APP_ESM_BEARER_MANAGE_TYPE_ACCEPT = 0,                  /*????*/
    APP_ESM_BEARER_MANAGE_TYPE_REJ,                         /*????*/

    APP_ESM_BEARER_MANANGE_TYPE_BUTT
};
typedef VOS_UINT8  APP_ESM_BEARER_MANAGE_TYPE_ENUM_UINT8;

enum APP_ESM_BEARER_MANAGE_MODE_ENUM
{
    APP_ESM_BEARER_MANAGE_MODE_AUTO = 0,                    /*????*/
    APP_ESM_BEARER_MANAGE_MODE_MANUAL,                      /*????*/

    APP_ESM_BEARER_MANAGE_MODE_BUTT
};
typedef VOS_UINT8  APP_ESM_BEARER_MANAGE_MODE_ENUM_UINT8;

/*DT begin */

/*****************************************************************************
 ??????    : NAS_ESM_DT_BEARER_TYPE_ENUM
 ????????  : ????????????????????
*****************************************************************************/

enum NAS_ESM_DT_BEARER_TYPE_ENUM
{
    NAS_ESM_DT_BEARER_TYPE_DEFAULT = 0,                /*????????*/
    NAS_ESM_DT_BEARER_TYPE_DEDICATED,                  /*????????*/
    /*V7R2-DT ,2014/5/6, begin*/
    NAS_ESM_DT_BEARER_TYPE_EMERGENCY,                  /*????????*/
    /*V7R2-DT ,2014/5/6, end*/

    NAS_ESM_DT_BEARER_TYPE_BUTT
};/*??????????*/
typedef VOS_UINT8  NAS_ESM_DT_BEARER_TYPE_ENUM_UINT8;
/*DT end */

/* ims begin */
enum APP_ESM_PCSCF_DISCOVERY_ENUM
{
    APP_ESM_PCSCF_DISCOVERY_NOT_INFLUENCED  = 0x00,
    APP_ESM_PCSCF_DISCOVERY_THROUGH_NAS_SIG = 0x01,
    APP_ESM_PCSCF_DISCOVERY_THROUGH_DHCP    = 0x02,

    APP_ESM_PCSCF_DISCOVERY_BUTT
};
typedef VOS_UINT32 APP_ESM_PCSCF_DISCOVERY_ENUM_UINT32;

enum APP_ESM_IMS_CN_SIG_FLAG_ENUM
{
    APP_ESM_PDP_NOT_FOR_IMS_CN_SIG_ONLY     = 0x00,
    APP_ESM_PDP_FOR_IMS_CN_SIG_ONLY         = 0x01,

    APP_ESM_IMS_CN_SIG_FLAG_BUTT
};
typedef VOS_UINT32 APP_ESM_IMS_CN_SIG_FLAG_ENUM_UINT32;
/* ims end */

/* modify for 2014-05-09 begin */
enum APP_ESM_PDP_EMC_IND_ENUM
{
    APP_ESM_PDP_NOT_FOR_EMC             = 0x00,
    APP_ESM_PDP_FOR_EMC                 = 0x01,

    APP_ESM_PDP_EMC_IND_BUTT
};
typedef VOS_UINT32 APP_ESM_PDP_EMC_IND_ENUM_UINT32;
/* modify for 2014-05-09 end */

/* Added for DATA RETRY PHASEI, 2016-03-21, Begin */
/*****************************************************************************
 ??????    : APP_ESM_PDP_RELEASE_CAUSE_ENUM
 ????????  : PDP????????
*****************************************************************************/
enum APP_ESM_PDP_RELEASE_CAUSE_ENUM
{
    APP_ESM_PDP_RELEASE_CAUSE_NORMAL                  = 0,
    APP_ESM_PDP_RELEASE_CAUSE_IPV6_ADDR_ALLOC_FAIL    = 1,
        /* Added for DATA RETRY PHASEIV, 2016-7-25, begin */
    APP_ESM_PDP_RELEASE_CAUSE_IPV6_ADDR_TIME_OUT      = 2,
    /* Added for DATA RETRY PHASEIV, 2016-7-25, end */
    APP_ESM_PDP_RELEASE_CAUSE_PCSCF_ADDR_ALLOC_FAIL   = 3,
    /* added for Band13 R1 PHASE1, 2017-10-20, begin */
    /* del reattach type */
    /* added for Band13 R1 PHASE1, 2017-10-20, end */


    /* Added for DCM DATA OFF, 2017-10-28, begin */
    APP_ESM_PDP_RELEASE_CAUSE_DATA_OFF                = 4,
    /* Added for DCM DATA OFF, 2017-10-28, end */
    /* Added for 2018-1-8, begin */
    APP_ESM_PDP_RELEASE_CAUSE_CELLULAR2W_HO           = 5,
    /* Added for 2018-1-8, end */

    APP_ESM_PDP_RELEASE_CAUSE_BUTT
};
typedef VOS_UINT8 APP_ESM_PDP_RELEASE_CAUSE_ENUM_UINT8;
/* Added for DATA RETRY PHASEI, 2016-03-21, End */

/* Added for DATA RETRY PHASEIV, 2016-7-25, begin */
/* added for Band13 R1 PHASE1, 2017-10-20, begin */
/*****************************************************************************
 ??????    : APP_ESM_APN_TYPE_ENUM_UINT8
 ????????  : ??????????APN ????
*****************************************************************************/
enum APP_ESM_APN_TYPE_ENUM
{
    APP_ESM_APN_TYPE_INVALID = 0x00,
    APP_ESM_APN_TYPE_CLASS1  = 0x01,
    APP_ESM_APN_TYPE_CLASS2  = 0x02,
    APP_ESM_APN_TYPE_CLASS3  = 0x03,
    APP_ESM_APN_TYPE_CLASS4  = 0x04,
    APP_ESM_APN_TYPE_CLASS5  = 0x05,
    APP_ESM_APN_TYPE_CLASS6  = 0x06,
    APP_ESM_APN_TYPE_CLASS7  = 0x07,
    APP_ESM_APN_TYPE_CLASS8  = 0x08,

    APP_ESM_APN_TYPE_BUTT
};
typedef VOS_UINT8 APP_ESM_APN_TYPE_ENUM_UINT8;
/* added for Band13 R1 PHASE1, 2017-10-20, end */
/* Added for DATA RETRY PHASEIV, 2016-7-25, begin */

/*****************************************************************************
 ??????    : ESM_DSM_NOTIFICATION_IND_ENUM_UINT32
 ????????  : SRVCC????????
*****************************************************************************/
enum ESM_DSM_NOTIFICATION_IND_ENUM
{
    ESM_DSM_NOTIFICATION_IND_SRVCC_HO_CANCELLED         = 0,

    ESM_DSM_NOTIFICATION_IND_BUTT
};
typedef VOS_UINT32 ESM_DSM_NOTIFICATION_IND_ENUM_UINT32;

/*****************************************************************************
 ????????   : APP_ESM_BCM_ENUM_UINT8
 ????????   :
 ASN.1 ???? :
 ????????   : BEARER CONTROL MODE????
*****************************************************************************/
enum APP_ESM_BCM_ENUM
{
    APP_ESM_BCM_MS_ONLY                  = 0x01,
    APP_ESM_BCM_MS_NW                    = 0x02,

    APP_ESM_BCM_BUTT
};
typedef VOS_UINT8 APP_ESM_BCM_ENUM_UINT8;

/*****************************************************************************
 ??????    : APP_ESM_HANDOVER_RESULT_ENUM_UINT32
 ????????  : ????????????
*****************************************************************************/
enum APP_ESM_HANDOVER_RESULT_ENUM
{
    APP_ESM_HANDOVER_RESULT_SUCC        = 0x00,
    APP_ESM_HANDOVER_RESULT_FAIL        = 0x01,
    APP_ESM_HANDOVER_RESULT_BUTT
};
typedef VOS_UINT32 APP_ESM_HANDOVER_RESULT_ENUM_UINT32;

/*****************************************************************************
  5 STRUCT
*****************************************************************************/
/*****************************************************************************
*                                                                            *
*                           ????????????????                                 *
*                                                                            *
******************************************************************************/
/* Added for DATA RETRY PHASEIV, 2016-7-25, begin */

/* Added for 5G Iteration 5, 2018-1-22, begin */
/*****************************************************************************
 ??????    : ESM_DSM_NOTIFICATION_IND_STRU
 ????????  : SRVCC????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                              ulMsgId;
    ESM_DSM_NOTIFICATION_IND_ENUM_UINT32    enNotificationIndicator;
}ESM_DSM_NOTIFICATION_IND_STRU;


/*****************************************************************************
 ????????   : ESM_DSM_REG_CID_IND_STRU
 ????????   : ESM -> DSM
              ??????????????SDF????CID??????DSM
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32                          ulMsgId;
    VOS_UINT32                          ulRegCid;
}ESM_DSM_REG_CID_IND_STRU;

/* moded for Band13 R1 PHASE1, 2017-10-16, begin */
/*****************************************************************************
 ??????    : APP_ESM_APN_CLASS_INFO_STRU
 ????????  : ????APN????????
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulCid;  /*CID [0,11]*/
    APP_ESM_APN_TYPE_ENUM_UINT8         enApnType;    /*apn class*/
    VOS_UINT8                           ucDisableFlag;/*apn disable flag*/
    VOS_UINT8                           aucReserved[2];
    VOS_UINT32                          ulInactivityTimerValue;/*Inactivity Timer Value in second*/
}APP_ESM_APN_CLASS_INFO_STRU;
/* moded for Band13 R1 PHASE1, 2017-10-16, end */
/* Added for DATA RETRY PHASEIV, 2016-7-25, end */

/*****************************************************************************
 ??????    : APP_ESM_PARA_SET_CNF_STRU
 ????????  : ??????????????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;             /*????????????*/
    VOS_UINT32                          ulCid;              /*????????????:0~31   */
    VOS_UINT32                          ulSetRslt;          /*????????????:0:????,1:????   */
}APP_ESM_PARA_SET_CNF_STRU;

/* begin for r11 2014-09-18 */
/*****************************************************************************
 ??????    : APP_ESM_TFT_INFO_STRU
 ????????  : TFT  ????
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpRmtIpv4AddrAndMask        : 1;
    VOS_UINT32                          bitOpRmtIpv6AddrAndMask        : 1;
    VOS_UINT32                          bitOpProtocolId                : 1;
    VOS_UINT32                          bitOpSingleLocalPort           : 1;
    VOS_UINT32                          bitOpLocalPortRange            : 1;
    VOS_UINT32                          bitOpSingleRemotePort          : 1;
    VOS_UINT32                          bitOpRemotePortRange           : 1;
    VOS_UINT32                          bitOpSecuParaIndex             : 1;
    VOS_UINT32                          bitOpTypeOfService             : 1;
    VOS_UINT32                          bitOpFlowLabelType             : 1;
    /* begin for r11 2014-09-18 */
    VOS_UINT32                          bitOpLocalIpv4AddrAndMask      : 1;
    VOS_UINT32                          bitOpLocalIpv6AddrAndMask      : 1;
    /* end for r11 2014-09-18 */
    VOS_UINT32                          bitOpSpare                     : 20;

    VOS_UINT8                           ucPacketFilterId;
    VOS_UINT8                           ucNwPacketFilterId;
    APP_ESM_TRANSFER_DIRECTION_ENUM_UINT8 enDirection;
    VOS_UINT8                           ucPrecedence;             /*packet filter evaluation precedence*/


    VOS_UINT32                          ulSecuParaIndex;          /*SPI*/
    VOS_UINT16                          usSingleLcPort;
    VOS_UINT16                          usLcPortHighLimit;
    VOS_UINT16                          usLcPortLowLimit;
    VOS_UINT16                          usSingleRmtPort;
    VOS_UINT16                          usRmtPortHighLimit;
    VOS_UINT16                          usRmtPortLowLimit;
    VOS_UINT8                           ucProtocolId;             /*??????*/
    VOS_UINT8                           ucTypeOfService;          /*TOS*/

    VOS_UINT8                           ucTypeOfServiceMask;      /*TOS Mask*/
    VOS_UINT8                           aucReserved[1];

    VOS_UINT8                           aucRmtIpv4Address[APP_MAX_IPV4_ADDR_LEN];
                                                                  /*ucSourceIpAddress[0]??IP????????????
                                                                  ucSourceIpAddress[3]??????????*/
    VOS_UINT8                           aucRmtIpv4Mask[APP_MAX_IPV4_ADDR_LEN];
                                                                  /*ucSourceIpMask[0]??IP???????????? ,
                                                                  ucSourceIpMask[3]??????????*/
    VOS_UINT8                           aucRmtIpv6Address[APP_MAX_IPV6_ADDR_LEN];
                                                                  /*ucRmtIpv6Address[0]??IPv6????????????????
                                                                    ucRmtIpv6Address[7]??IPv6????????????????*/
    VOS_UINT8                           aucRmtIpv6Mask[APP_MAX_IPV6_ADDR_LEN];
                                                                  /*ucRmtIpv6Mask[0]??????????
                                                                    ucRmtIpv6Mask[7]??????????*/

    VOS_UINT32                          ulFlowLabelType;          /*FlowLabelType*/
    /* begin for r11 2014-09-18 */
    VOS_UINT8                           aucLocalIpv4Addr[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucLocalIpv4Mask[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucLocalIpv6Addr[APP_MAX_IPV6_ADDR_LEN];
    VOS_UINT8                           ucLocalIpv6Prefix;
    VOS_UINT8                           aucReserved2[3];
    /* end for r11 2014-09-18 */
}APP_ESM_TFT_INFO_STRU;
/* end for r11 2014-09-18 */
/*****************************************************************************
 ??????    : APP_ESM_SET_TFT_REQ_STRU
 ????????  : TFT  ????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;             /*????????????*/
    VOS_UINT32                          ulCid;              /*????????????:0~31       */
    APP_ESM_PARA_SET_ENUM_UINT32        enSetType;          /*????????:0:????,1:????*/
    APP_ESM_TFT_INFO_STRU               stAppTftInfo;       /*TFT????*/
} APP_ESM_SET_TFT_REQ_STRU;

typedef  APP_ESM_PARA_SET_CNF_STRU APP_ESM_SET_TFT_CNF_STRU;

/*****************************************************************************
 ??????    : APP_ESM_QOS_INFO_STRU
 ????????  : QOS  ????
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucDelayClass;
    VOS_UINT8                           ucReliabClass;
    VOS_UINT8                           ucPeakThrough;
    VOS_UINT8                           ucPrecedenceClass;
    VOS_UINT8                           ucMeanThrough;
    VOS_UINT8                           ucTrafficClass;
    VOS_UINT8                           ucDeliverOrder;
    VOS_UINT8                           ucDeliverOfErrSdu;
    VOS_UINT8                           ucMaximSduSize;
    VOS_UINT8                           ucMaxBitRateForUp;
    VOS_UINT8                           ucMaxBitRateForDown;
    VOS_UINT8                           ucResidualBer;
    VOS_UINT8                           ucSduErrRatio;
    VOS_UINT8                           ucTransDelay;
    VOS_UINT8                           ucTraffHandlPrior;
    VOS_UINT8                           ucGuarantBitRateForUp;
    VOS_UINT8                           ucGuarantBitRateForDown;
    VOS_UINT8                           ucSrcStatisticsDescriptor;
    VOS_UINT8                           ucSignallingIndication;
    VOS_UINT8                           ucMaxBitRateForDownExt;
    VOS_UINT8                           ucGuarantBitRateForDownExt;
    VOS_UINT8                           ucMaxBitRateForUpExt;
    VOS_UINT8                           ucGuarantBitRateForUpExt;
    /* begin for r11 2014-09-02 */
    VOS_UINT8                           ucMaxBitRateForDownExt_2;
    VOS_UINT8                           ucGuarantBitRateForDownExt_2;
    VOS_UINT8                           ucMaxBitRateForUpExt_2;
    VOS_UINT8                           ucGuarantBitRateForUpExt_2;
    /* end for r11 2014-09-02 */

    VOS_UINT8                           ucReserved[1];
}APP_ESM_QOS_INFO_STRU;


/*****************************************************************************
 ??????    : APP_ESM_SET_QOS_REQ_STRU
 ????????  : QOS  ????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;              /*????????????:0~31       */
    APP_ESM_PARA_SET_ENUM_UINT32        enSetType;          /*????????:0:????,1:????*/
    APP_ESM_QOS_INFO_STRU               stAppQoSInfo;       /*APP_ESM_QOS_INFO_STRU????????*/
}APP_ESM_SET_QOS_REQ_STRU;

typedef  APP_ESM_PARA_SET_CNF_STRU APP_ESM_SET_QOS_CNF_STRU;

/*****************************************************************************
 ??????    : APP_ESM_EPS_QOS_INFO_STRU
 ????????  : EPS_QOS  ????
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucQCI;
    VOS_UINT8                           aucRsv[3];
    VOS_UINT32                          ulULMaxRate;    /* ??????kbps */
    VOS_UINT32                          ulDLMaxRate;    /* ??????kbps */
    VOS_UINT32                          ulULGMaxRate;   /* ??????kbps */
    VOS_UINT32                          ulDLGMaxRate;   /* ??????kbps */
}APP_ESM_EPS_QOS_INFO_STRU;

typedef struct
{
    VOS_UINT32                           ulEpsbId;
    APP_ESM_EPS_QOS_INFO_STRU            stQos;
} APP_EPS_QOS_STRU;
/*****************************************************************************
 ??????    : APP_ESM_SET_EPS_QOS_REQ_STRU
 ????????  : SDF QOS  ????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;

    APP_ESM_PARA_SET_ENUM_UINT32        enSetType;          /*0:SET;1:DEL*/
    APP_ESM_EPS_QOS_INFO_STRU           stSdfQosInfo;

}APP_ESM_SET_EPS_QOS_REQ_STRU;


typedef  APP_ESM_PARA_SET_CNF_STRU APP_ESM_SET_EPS_QOS_CNF_STRU;


/*****************************************************************************
 ??????    : APP_ESM_APN_INFO_STRU
 ????????  : APN  ????
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucApnLen;
    VOS_UINT8                           aucApnName[APP_MAX_APN_LEN];
}APP_ESM_APN_INFO_STRU;

/*****************************************************************************
 ????????   : APP_ESM_APN_STRU
 ????????   :
 ASN.1 ???? :
 ????????   : ????TS 24.008 section 10.5.6.1 APN??????????(??????)
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucLength;           /* APN????    */
    VOS_UINT8                           aucReserved[3];     /* ?????????? */
    VOS_UINT8                           aucValue[APP_ESM_MAX_LENGTH_OF_APN];
} APP_ESM_APN_STRU;

/*****************************************************************************
 ??????    : APP_ESM_SET_APN_REQ_STRU
 ????????  : APN  ????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;

    APP_ESM_PARA_SET_ENUM_UINT32        enSetType;          /*0:SET;1:DEL*/
    APP_ESM_APN_INFO_STRU               stApnInfo;
}APP_ESM_SET_APN_REQ_STRU;

typedef  APP_ESM_PARA_SET_CNF_STRU APP_ESM_SET_APN_CNF_STRU;
/* Added for R1 e911, 2017-12-6, begin */
/*****************************************************************************
 ??????    : APP_ESM_SET_EXCESSIVE_PDN_CTRL_PARA_REQ_STRU
 ????????  : APN  ????????
*****************************************************************************/
typedef struct
{
    APP_ESM_APN_INFO_STRU               stApnInfo;

    VOS_UINT32                          ulWaitTime;            /* ??PDN??????????????waittime????????????PDN??????????????:s*/
    VOS_UINT32                          ulPdnMaxConnTime;      /* ????????????????????????PDN??????????????????????????????????:s*/
    VOS_UINT32                          ulPdnMaxConnCount;     /* ??????????????????????????????????????PDN??????????????*/

    VOS_UINT8                           ucCid;
    APP_ESM_APN_TYPE_ENUM_UINT8         enApnClassType;
    VOS_UINT8                           aucRsv[2];

}APP_ESM_EXCESSIVE_PDN_CTRL_PARA_STRU;

/*****************************************************************************
 ??????    : APP_ESM_SET_APN_THROT_INFO_REQ_STRU
 ????????  : APN  ????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                                      ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER

    APP_ESM_EXCESSIVE_PDN_CTRL_PARA_STRU    stExcessivePdnCtrlPara;
}APP_ESM_SET_APN_THROT_INFO_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_SET_EXCESSIVE_PDN_CTRL_PARA_CNF_STRU
 ????????  : APN  ????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER

    VOS_UINT32                          ulRslt;

}APP_ESM_SET_APN_THROT_INFO_CNF_STRU;



/* Added for R1 e911, 2017-12-6, end */
/*****************************************************************************
 ??????    : APP_ESM_APN_AMBR_INFO_STRU
 ????????  : APN AMBR ????
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulDLApnAmbr;/* ??????????,????KBPS */
    VOS_UINT32                          ulULApnAmbr;/* ??????????,????KBPS */

}APP_ESM_APN_AMBR_INFO_STRU;

/*****************************************************************************
 ??????    : APP_ESM_PCO_INFO_STRU
 ????????  : PCO  ????
*****************************************************************************/
typedef struct
{

    VOS_UINT32                          bitOpAPL                          : 1;
    VOS_UINT32                          bitOpCPOL                         : 1;
    VOS_UINT32                          bitOpPCSCFAddReq                  : 1;
    VOS_UINT32                          bitOpIMCNFlag                     : 1;
    VOS_UINT32                          bitOpDNSAddrReq                   : 1;
    VOS_UINT32                          bitOpMSBearCtrlID                 : 1;
    VOS_UINT32                          bitOpPrefBearCtrlMode             : 1;

    VOS_UINT32                          bitOpSpare                        : 25;

    VOS_UINT8                           ucConfigProtocol;
    VOS_UINT8                           ucBearMode;       /*container identifier??Preferred Bearer Control Mode????????*/
    VOS_UINT8                           ucReserved[2];
    VOS_UINT16                          usAPLCid;          /*Additional parameters list??container identifier*/
    VOS_UINT16                          usCPOLPid;         /*Configuration protocol options list??protocol identifier*/
    VOS_UINT8                           aucPPPPacket[248];
}APP_ESM_PCO_INFO_STRU;



/*****************************************************************************
 ??????    : APP_ESM_SET_PCO_REQ_STRU
 ????????  : PCO ????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;             /*????????????*/
    VOS_UINT32                          ulCid;              /*????????????:0~31       */

    APP_ESM_PARA_SET_ENUM_UINT32        enSetType;          /*????????:0:????,1:????*/
    APP_ESM_PCO_INFO_STRU               stAppPcoInfo;       /*PCO????*/
} APP_ESM_SET_PCO_REQ_STRU;

typedef  APP_ESM_PARA_SET_CNF_STRU APP_ESM_SET_PCO_CNF_STRU;

/*****************************************************************************
 ??????    : APP_ESM_SET_PDN_TYPE_REQ_STRU
 ????????  : PDN_TYPE ????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;

    VOS_UINT32                          bitOpIpv4AddrAllocType    :1;
    VOS_UINT32                          bitOpSpare              :31;

    APP_ESM_PARA_SET_ENUM_UINT32        enSetType;          /*0:SET;1:DEL*/
    APP_ESM_PDN_TYPE_ENUM_UINT32        enPdnType;      /*1:IPV4,2:IPV6,3:BOTH*/
    APP_ESM_IPV4_ADDR_ALLOC_TYPE_ENUM_UINT32 enIpv4AddrAllocType;
}APP_ESM_SET_PDN_TYPE_REQ_STRU;
typedef  APP_ESM_PARA_SET_CNF_STRU APP_ESM_SET_PDN_TYPE_CNF_STRU;

/*****************************************************************************
 ??????    : APP_ESM_SET_BEARER_TYPE_REQ_STRU
 ????????  : BEARER_TYPE  ????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;

    VOS_UINT32                          bitOpLinkdCid       :1;
    VOS_UINT32                          bitOpSpare          :31;

    APP_ESM_PARA_SET_ENUM_UINT32        enSetType;
    APP_ESM_CID_TYPE_ENUM_UINT32        enBearCidType;

    VOS_UINT32                          ulLinkdCid;

}APP_ESM_SET_BEARER_TYPE_REQ_STRU;

typedef  APP_ESM_PARA_SET_CNF_STRU APP_ESM_SET_BEARER_TYPE_CNF_STRU;


/*****************************************************************************
 ??????    : APP_ESM_SET_PDP_MANAGER_TYPE_REQ_STRU
 ????????  : ????APP????????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;

    APP_ESM_PARA_SET_ENUM_UINT32        enSetType;
    APP_ESM_ANSWER_MODE_ENUM_UINT32     enAnsMode;          /*APP????????:0:????,1:????*/
    APP_ESM_ANSWER_RESULT_ENUM_UINT32   enAnsType;          /*APP????????:0:????,1:????,????????????????????*/

} APP_ESM_SET_PDP_MANAGER_TYPE_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER                              /*_H2ASN_Skip*/
    VOS_UINT32              ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32              ulOpId;
    VOS_UINT32              ulSetRslt;
} APP_ESM_SET_PDP_MANAGER_TYPE_CNF_STRU;

/*****************************************************************************
 ??????    : APP_ESM_SET_GW_AUTH_REQ_STRU
 ????????  : ????????????????????
*****************************************************************************/
typedef struct
{
    APP_ESM_AUTH_TYPE_ENUM_UINT8        enGwAuthType;
    VOS_UINT8                           ucAccNumLen;
    VOS_UINT8                           aucRsv1[2];
    VOS_UINT8                           auAccessNum[APP_ESM_MAX_ACCESS_NUM_LEN];/*??????????????????????*/
    VOS_UINT8                           ucUserNameLen;
    VOS_UINT8                           aucRsv2[3];
    VOS_UINT8                           aucUserName[APP_ESM_MAX_USER_NAME_LEN];
    VOS_UINT8                           ucPwdLen;
    VOS_UINT8                           aucRsv3[3];
    VOS_UINT8                           aucPwd[APP_ESM_MAX_PASSWORD_LEN];
}APP_ESM_GW_AUTH_INFO_STRU;

/*****************************************************************************
 ??????    : APP_ESM_SDF_PARA_STRU
 ????????  : APP??ESM ??????SDF??????????????
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpPdnType         : 1;
    VOS_UINT32                          bitOpApn             : 1;
    VOS_UINT32                          bitOpSdfQos          : 1;
    VOS_UINT32                          bitOpLinkdCId        : 1;
    VOS_UINT32                          bitIpv4AddrAllocType : 1;
    VOS_UINT32                          bitOpGwAuthInfo      : 1;
    /* ims begin */
    VOS_UINT32                          bitOpPcscfDiscovery  : 1;
    VOS_UINT32                          bitOpImsCnSignalFlag : 1;
    VOS_UINT32                          bitOpSpare           : 24;
    /* ims end */

    APP_ESM_PDN_TYPE_ENUM_UINT32        enPdnType;
    APP_ESM_IPV4_ADDR_ALLOC_TYPE_ENUM_UINT32 enIpv4AddrAllocType;
    APP_ESM_BEARER_TYPE_ENUM_UINT32      enBearerCntxtType;  /* SM???????????????????? */

    VOS_UINT32                          ulCId;              /* ??????????ID */
    VOS_UINT32                          ulLinkdCId;         /*????CID*/

    VOS_UINT32                          ulPfNum;
    APP_ESM_EPS_QOS_INFO_STRU           stSdfQosInfo;
    APP_ESM_APN_INFO_STRU               stApnInfo;
    APP_ESM_GW_AUTH_INFO_STRU           stGwAuthInfo;
    APP_ESM_TFT_INFO_STRU               astCntxtTftInfo[APP_ESM_MAX_SDF_PF_NUM];
    /* ims begin */
    APP_ESM_PCSCF_DISCOVERY_ENUM_UINT32 enPcscfDiscovery;
    APP_ESM_IMS_CN_SIG_FLAG_ENUM_UINT32 enImsCnSignalFlag;
    /* ims end */
}APP_ESM_SDF_PARA_STRU;

/*lint -save -e959*/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;

    APP_ESM_PARA_SET_ENUM_UINT32        enSetType;
    APP_ESM_GW_AUTH_INFO_STRU           stGwAuthInfo;
}APP_ESM_SET_GW_AUTH_REQ_STRU;
/*lint -restore*/

typedef  APP_ESM_PARA_SET_CNF_STRU APP_ESM_SET_GW_AUTH_CNF_STRU;


/*****************************************************************************
*                                                                            *
*                            ????????????????                                *
*                                                                            *
******************************************************************************/

/*****************************************************************************
 ??????    : APP_ESM_INQ_PARA_REQ_STRU
 ????????  : ????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;
}APP_ESM_INQ_PARA_REQ_STRU;

typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_TFT_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_TFT_CNF_STRU
 ????????  : ????????:TFT
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                  ulOpId;
    VOS_UINT32                  ulCid;
    VOS_UINT32                  ulRslt;
    VOS_UINT32                  ulPfNum;           /*??????PF??*/
    APP_ESM_TFT_INFO_STRU       astPfInfo[APP_ESM_TFT_MAX_PF_NUM];
}APP_ESM_INQ_TFT_CNF_STRU;

typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_QOS_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_QOS_CNF_STRU
 ????????  : ????????:QOS
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;
    VOS_UINT32                          ulRslt;
    APP_ESM_QOS_INFO_STRU               stAppQoSInfo;       /*APP_ESM_QOS_INFO_STRU????????*/
}APP_ESM_INQ_QOS_CNF_STRU;


typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_PCO_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_PCO_CNF_STRU
 ????????  : ????????:PCO
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;
    VOS_UINT32                          ulRslt;
    APP_ESM_PCO_INFO_STRU               stAppPcoInfo;       /*PCO????*/
} APP_ESM_INQ_PCO_CNF_STRU;


typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_BEARER_TYPE_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_BEARER_TYPE_CNF_STRU
 ????????  : ????????:BEARER_TYPE
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;
    VOS_UINT32                          bitOpLinkdCid       :1;
    VOS_UINT32                          bitOpSpare          :31;

    VOS_UINT32                          ulRslt;
    APP_ESM_CID_TYPE_ENUM_UINT32        enBearCidType;
    VOS_UINT32                          ulLinkdCid;         /*??????????CID*/

} APP_ESM_INQ_BEARER_TYPE_CNF_STRU;


typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_PDN_TYPE_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_PDN_TYPE_CNF_STRU
 ????????  : ????????:PDN_TYPE
*****************************************************************************/

typedef struct
{
    VOS_MSG_HEADER                                                  /*_H2ASN_Skip*/
    VOS_UINT32                                  ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                                  ulOpId;
    VOS_UINT32                                  ulCid;

    VOS_UINT32                                  bitOpIpv4AddrAllocType    :1;
    VOS_UINT32                                  bitOpSpare                :31;

    VOS_UINT32                                  ulRslt;
    APP_ESM_PDN_TYPE_ENUM_UINT32                enPdnType;          /*1:IPV4,2:IPV6,3:BOTH*/
    APP_ESM_IPV4_ADDR_ALLOC_TYPE_ENUM_UINT32    enIpv4AddrAllocType;
} APP_ESM_INQ_PDN_TYPE_CNF_STRU;

typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_APN_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_APN_TYPE_CNF_STRU
 ????????  : ????????:APN
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;

    VOS_UINT32                          ulRslt;
    APP_ESM_APN_INFO_STRU               stApnInfo;
} APP_ESM_INQ_APN_CNF_STRU;

typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_EPS_QOS_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_EPS_QOS_CNF_STRU
 ????????  : ????????:SDF QOS
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;
    VOS_UINT32                          ulRslt;

    APP_ESM_EPS_QOS_INFO_STRU           stSdfQosInfo;
} APP_ESM_INQ_EPS_QOS_CNF_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_PDP_MANAGER_TYPE_REQ_STRU
 ????????  : ????????:BEARER_MANAGER_TYPE
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32               ulMsgId;                       /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32               ulOpId;
} APP_ESM_INQ_PDP_MANAGER_TYPE_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_PDP_MANAGER_TYPE_CNF_STRU
 ????????  : ????????:BEARER_MANAGER_TYPE
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;

    VOS_UINT32                          ulRslt;
    APP_ESM_ANSWER_MODE_ENUM_UINT32     enAnsMode;          /*APP????????:0:????,1:????*/
    APP_ESM_ANSWER_RESULT_ENUM_UINT32   enAnsType;          /*APP????????:0:????,1:????,????????????????????*/
} APP_ESM_INQ_PDP_MANAGER_TYPE_CNF_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_BEAER_QOS_REQ_STRU
 ????????  : ????????:BEARER_QOS
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                  /*_H2ASN_Skip*/
    VOS_UINT32 ulMsgId;    /*_H2ASN_Skip*/

    APP_MSG_HEADER
    VOS_UINT32                             ulEpsbId;
} APP_ESM_INQ_BEAER_QOS_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_BEAER_QOS_CNF_STRU
 ????????  : ????????:BEARER_QOS
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                  /*_H2ASN_Skip*/
    VOS_UINT32 ulMsgId;    /*_H2ASN_Skip*/

    APP_MSG_HEADER
    VOS_UINT32                             ulEpsbIdNum;
    APP_EPS_QOS_STRU                       astQosInfo[APP_ESM_MAX_EPSB_NUM];
    APP_ESM_ANSWER_RESULT_ENUM_UINT32      enAnsType;
}APP_ESM_INQ_BEAER_QOS_CNF_STRU;

typedef struct
{
    VOS_MSG_HEADER                                  /*_H2ASN_Skip*/
    VOS_UINT32 ulMsgId;    /*_H2ASN_Skip*/

    APP_MSG_HEADER
    VOS_UINT32                             ulEpsbId;
    APP_ESM_EPS_QOS_INFO_STRU              stQosInfo;
}APP_ESM_INQ_BEAER_QOS_IND_STRU;

typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_DYNAMIC_PDP_CONT_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_IP_ADDR_STRU
 ????????  : IP????????????
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucIpType;
    VOS_UINT8                           ucReserved[3];
    VOS_UINT8                           aucIpV4Addr[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucIpV6Addr[APP_MAX_IPV6_ADDR_LEN];
}APP_ESM_IP_ADDR_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_DYNAMIC_PDP_CONT_CNF_STRU
 ????????  : ????????:????PDP??????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;

    VOS_UINT32                          bitOpPdnAddr         :1;
    VOS_UINT32                          bitOpApn             :1;
    VOS_UINT32                          bitOpLinkCid         :1;
    VOS_UINT32                          bitOpDnsPrim         :1;
    VOS_UINT32                          bitOpDnsSec          :1;
    VOS_UINT32                          bitOpPCscfPrim       :1;
    VOS_UINT32                          bitOpPCscfSec        :1;
    VOS_UINT32                          bitOpGateWayAddrInfo :1;
    /* ims begin */
    VOS_UINT32                          bitOpImsCnSignalFlag :1;
    /* ims end */
    /* Modified for IPV4 MTU, 2016-11-15, Begin */
    VOS_UINT32                          bitOpIpv4Mtu         : 1;
    VOS_UINT32                          bitOpSpare           :22;
    /* Modified for IPV4 MTU, 2016-11-15, End */

    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulEpsbId;
    APP_ESM_IP_ADDR_STRU                stPDNAddrInfo;
    APP_ESM_IP_ADDR_STRU                stSubnetMask;
    APP_ESM_IP_ADDR_STRU                stGateWayAddrInfo;
    APP_ESM_IP_ADDR_STRU                stDnsPrimAddrInfo;  /*??DNS???? */
    APP_ESM_IP_ADDR_STRU                stDnsSecAddrInfo;   /*??DNS???? */
    APP_ESM_IP_ADDR_STRU                stPCscfPrimAddrInfo;/*??P-CSCF???? */
    APP_ESM_IP_ADDR_STRU                stPCscfSecAddrInfo; /* ??P-CSCF???? */
    APP_ESM_APN_INFO_STRU               stApnInfo;
    VOS_UINT32                          ulLinkCid;
    /* ims begin */
    APP_ESM_IMS_CN_SIG_FLAG_ENUM_UINT32 enImsCnSignalFlag;
    /* ims end */
    /* Added for IPV4 MTU, 2016-11-15, Begin */
    VOS_UINT16                          usIpv4Mtu;
    VOS_UINT8                           aucReserved[2];
    /* Added for IPV4 MTU, 2016-11-15, End */
} APP_ESM_INQ_DYNAMIC_PDP_CONT_CNF_STRU;

typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_DYNAMIC_EPS_QOS_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_DYNAMIC_EPS_QOS_CNF_STRU
 ????????  : ????????:????EPS QoS????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                           /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;             /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;

    VOS_UINT32                          ulRslt;
    APP_ESM_EPS_QOS_INFO_STRU           stSdfQosInfo;
} APP_ESM_INQ_DYNAMIC_EPS_QOS_CNF_STRU;

typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_DYNAMIC_TFT_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_DYNAMIC_TFT_CNF_STRU
 ????????  : ????????:????TFT????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                           /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;             /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;

    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulPfNum;             /*??????PF??*/
    APP_ESM_TFT_INFO_STRU               astPfInfo[APP_ESM_TFT_MAX_PF_NUM];
} APP_ESM_INQ_DYNAMIC_TFT_CNF_STRU;


typedef APP_ESM_INQ_PARA_REQ_STRU APP_ESM_INQ_GW_AUTH_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_INQ_GW_AUTH_CNF_STRU
 ????????  : ????????:????????????
*****************************************************************************/
/*lint -save -e959*/
typedef struct
{
    VOS_MSG_HEADER                                           /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;             /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;
    VOS_UINT32                          ulRslt;
    APP_ESM_GW_AUTH_INFO_STRU           stGwAuthInfo;
} APP_ESM_INQ_GW_AUTH_CNF_STRU;
/*lint -restore*/

/*****************************************************************************
*                                                                            *
*                            ??????????????????                              *
*                                                                            *
******************************************************************************/
/*****************************************************************************
 ????????: APP_ESM_CHAP_PARAM_STRU
 ????????: The struct contains the parameter of CHAP.
           ulChallengeLen     - Length of username
           aucChallenge       - Array of challenge
           ulChallengeNameLen - Length of challenge name
           aucChallengeName   - Array of challenge name
           ulResponseLen      - Length of response
           aucResponse        - Array of response
           ulResponseNameLen  - Length of response name (username)
           aucResponseName    - Array of response name (username)
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulChallengeLen;
    VOS_UINT8                           aucChallenge[APP_ESM_PPP_CHAP_CHALLNGE_LEN];
    VOS_UINT32                          ulChallengeNameLen;
    VOS_UINT8                           aucChallengeName[APP_ESM_PPP_CHAP_CHALLNGE_NAME_LEN];
    VOS_UINT32                          ulResponseLen;
    VOS_UINT8                           aucResponse[APP_ESM_PPP_CHAP_RESPONSE_LEN];
    VOS_UINT8                           ucResponseNameLen;
    VOS_UINT8                           aucResponseName[APP_ESM_PPP_MAX_USERNAME_LEN];

} APP_ESM_CHAP_PARAM_STRU;

/*****************************************************************************
 ????????: APP_ESM_PAP_PARAM_STRU
 ????????: The struct contains the parameter of PAP.
           ucUsernameLen - Length of username
           aucUsername   - Array of username
           ucPasswordLen - Length of password
           aucPassword   - Array of password
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucUsernameLen;
    VOS_UINT8                           aucUsername[APP_ESM_PPP_MAX_USERNAME_LEN];
    VOS_UINT8                           ucPasswordLen;
    VOS_UINT8                           aucPassword[APP_ESM_PPP_MAX_PASSWORD_LEN];

} APP_ESM_PAP_PARAM_STRU;

/*****************************************************************************
 ????????: APP_ESM_AUTH_INFO_STRU
 ????????: The struct contains the authentication information.
           enAuthType  - Auth Type (NONE/PAP/CHAP)
           stChapParam - Parameters of CHAP
           stPapParam  - Parameters of PAP
*****************************************************************************/
typedef struct
{
    APP_ESM_AUTH_TYPE_ENUM_UINT8        enAuthType;
    VOS_UINT8                           ucAuthId;
    VOS_UINT8                           aucRsv[2];
    union
    {
        APP_ESM_CHAP_PARAM_STRU         stChapParam;
        APP_ESM_PAP_PARAM_STRU          stPapParam;
    } unAuthParam;

} APP_ESM_AUTH_INFO_STRU;

/*****************************************************************************
 ????????: APP_ESM_IPCP_INFO_STRU
 ????????: The struct contains the IPCP information.
           aucPrimDns  - Primary DNS address
           aucSecDns   - Secondary DNS address
           aucPrimNbns - Primary NBNS address
           aucSecNbns  - Secondary NBNS address
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpIpAddr         :1;
    VOS_UINT32                          bitOpPrimDns        :1;
    VOS_UINT32                          bitOpSecDns         :1;
    VOS_UINT32                          bitOpPrimNbns       :1;
    VOS_UINT32                          bitOpSecNbns        :1;
    VOS_UINT32                          bitOpSpare          :27;

    VOS_UINT8                           aucIpAddr[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucPrimDns[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecDns[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucPrimNbns[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecNbns[APP_MAX_IPV4_ADDR_LEN];

} APP_ESM_IPCP_INFO_STRU;


/*****************************************************************************
 ????????: APP_ESM_PPP_INFO_STRU
 ????????: The struct contains the PPP information.
           stAuthInfo - Authentication information
           stIpcpInfo - IPCP information
*****************************************************************************/
typedef struct
{
    APP_ESM_AUTH_INFO_STRU              stAuthInfo;       /*PPP????????????*/
    APP_ESM_IPCP_INFO_STRU              stIpcpInfo;       /*PPP????IPCP??????ESM?????? */

} APP_ESM_PPP_INFO_STRU;
typedef struct
{
    VOS_UINT32                           ulEpsbId;
    NAS_ESM_DT_BEARER_TYPE_ENUM_UINT8    enBearerType; /*????????*/
    VOS_UINT8                            ucRsv[3];
 	APP_ESM_APN_INFO_STRU                stApn;         /*APN???? */
    APP_ESM_IP_ADDR_STRU                 stIpAddr;      /*UE IP???????? */
    APP_ESM_EPS_QOS_INFO_STRU            stSdfQosInfo;  /*QoS???? */
} NAS_OM_EPSB_INFO_STRU;

typedef struct
{
    VOS_UINT32                ulActEpsbNum;                /*?????????????? */
    NAS_OM_EPSB_INFO_STRU     stEpsbInfo[APP_ESM_MAX_EPSB_NUM];
}NAS_OM_ACT_PDP_INFO_STRU;


typedef struct
{
    VOS_MSG_HEADER                                  /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;    /*_H2ASN_Skip*/

    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;

    DT_CMD_ENUM_UINT8                   enCmd;
    VOS_UINT8                           ucRsv[3];
    VOS_UINT32                          ulRptPeriod;
}APP_ESM_DT_INQ_CMD_REQ_STRU;

typedef   APP_ESM_DT_INQ_CMD_REQ_STRU APP_ESM_INQ_PDP_INFO_REQ_STRU;

/*V7R2-DT ,2014/4/24, CNF ????????????????IND????,begin*/
typedef struct
{
    VOS_MSG_HEADER                                           /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;             /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulRslt;
} APP_ESM_INQ_PDP_INFO_CNF_STRU;
/*V7R2-DT ,2014/4/24, CNF ????????????????IND????,end*/

typedef struct
{
    VOS_MSG_HEADER                                           /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;             /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;

    NAS_OM_ACT_PDP_INFO_STRU            stActPdpInfo;
} APP_ESM_INQ_PDP_INFO_IND_STRU;


/* DT end */

/*****************************************************************************
*                                                                            *
*                            ??????????????????                              *
*                                                                            *
******************************************************************************/

/* moded for verizon band13 pco requirement 2017-03-25, begin */
/*****************************************************************************
 ????????   : APP_ESM_CUSTOM_PCO_CONTAINER_STRU
 ????????   : PCO????????????24008-10.5.6.3
              PCO-FF00H????????:
              Container Identifier     -FF00H(2 bytes)
              Container Content Length -MAX 50 octets(1 bytes)
              Container Content Format -MCCMNC(VZW-311480, 3 octets),
                                        ACTION(value:0-5;  6-255:reserved)
              4????????
 ASN.1 ???? :
 ????????   : ESM -> APS
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usContainerId;
    VOS_UINT8                           ucContentLen;
    VOS_UINT8                           aucContents[APP_ESM_MAX_CUSTOM_PCO_CONTAINER_CONTENT_LEN];
}APP_ESM_CUSTOM_PCO_CONTAINER_STRU;

/*****************************************************************************
 ????????   : APP_ESM_CUSTOM_PCO_INFO_STRU
 ????????   : PCO??????????????????????PCO????????????24008-10.5.6.3??
              ??VERIZON????????,????????????????????????????
              ????????????3??????????????FF00H
 ASN.1 ???? :
 ????????   : ESM -> APS
*****************************************************************************/
typedef struct
{
    VOS_UINT32                                  ulContainerNum;
    APP_ESM_CUSTOM_PCO_CONTAINER_STRU           astContainerList[APP_ESM_MAX_CUSTOM_PCO_CONTAINER_NUM];
}APP_ESM_CUSTOM_PCO_INFO_STRU;
/* moded for verizon band13 pco requirement 2017-03-25, end */

/* added for R1 DATA DEVICE PHASE1, 2017-12-13, begin */
typedef struct
{
    VOS_UINT8                           aucIpV4Addr[APP_MAX_IPV4_ADDR_LEN];
}APP_ESM_IPV4_EPDG_STRU;

typedef struct
{
    VOS_UINT8                           aucIpV6Addr[APP_MAX_IPV6_ADDR_LEN];
}APP_ESM_IPV6_EPDG_STRU;

/*****************************************************************************
 ????????   : APP_ESM_EPDG_INFO_STRU
 ????????   : R1????EPDG????????
 ASN.1 ???? :
 ????????   : ESM -> TAF
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usIpv4EpdgNum;
    VOS_UINT16                          usIpv6EpdgNum;
    APP_ESM_IPV4_EPDG_STRU              astIpv4EpdgList[APP_ESM_MAX_IPV4_EPDG_NUM];
    APP_ESM_IPV6_EPDG_STRU              astIpv6EpdgList[APP_ESM_MAX_IPV6_EPDG_NUM];
}APP_ESM_EPDG_INFO_STRU;
/* added for R1 DATA DEVICE PHASE1, 2017-12-13, end */


/*****************************************************************************
??????    : APP_ESM_PDP_SETUP_REQ_STRU
????????  : APP????????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;             /*??????????????*/

    APP_ESM_PDP_SETUP_TYPE_ENUM_UINT32  enSetupType;        /* ????PDP????????PPP???????? */

    VOS_UINT32                          ulCid;              /*??????????????????:0~31*/
    VOS_UINT8                           ucPduSessionId;
    VOS_UINT8                           aucRsv[3];
    APP_ESM_PPP_INFO_STRU               stPppInfo;          /* PPP????????????*/
    /*Added for DATA RETRY PHASEIII 2016-06-21 start*/
    APP_ESM_BEARER_PRIO_ENUM_UINT32     enBearerPrio;       /* NAS signalling low priority???? */
    /*Added for DATA RETRY PHASEIII 2016-06-21 end*/
    VOS_UINT32                          ulPsCallId;         /*????????ID??????:0~31*/
}APP_ESM_PDP_SETUP_REQ_STRU;


typedef struct
{
    VOS_UINT8                           aucPlmnId[3];
    VOS_UINT8                           ucRsv;
}APP_ESM_PLMN_ID_STRU;


typedef struct
{
    PS_S_NSSAI_STRU                     stSnssai;
    APP_ESM_PLMN_ID_STRU                stPlmnId;
}APP_ESM_S_NSSAI_STRU;

/*****************************************************************************
 ??????    : APP_ESM_PDP_SETUP_CNF_STRU
 ????????  : ????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;

    VOS_UINT32                          bitOpLinkCid         :1;
    VOS_UINT32                          bitOpBearerState     :1;
    VOS_UINT32                          bitOpBearerType      :1;
    VOS_UINT32                          bitOpPdnAddr         :1;
    VOS_UINT32                          bitOpRabId           :1;
    VOS_UINT32                          bitOpApn             :1;
    VOS_UINT32                          bitOpSdfQos          :1;
    VOS_UINT32                          bitOpDnsPrim         :1;
    VOS_UINT32                          bitOpDnsSec          :1;
    VOS_UINT32                          bitOpPCscfPrim       :1;
    VOS_UINT32                          bitOpPCscfSec        :1;
    VOS_UINT32                          bitOpGateWayAddrInfo :1;
    /* added for verizon band13 pco requirement 2017-03-25, begin */
    VOS_UINT32                          bitOpCustomPco       :1; /* ????VERRIZON??????????,??FF00H??????APS??,????FF00H?????????????? 1:???? 0:?????? */
    /* added for verizon band13 pco requirement 2017-03-25, end */
    /* moded for R1 DATA DEVICE PHASE1, 2017-12-13, begin */
    VOS_UINT32                          bitOpEpdgInfo        : 1; /* ????R1??????????,??EPDG??????????APS??,????????????????IMSA?? 1:???? 0:?????? */
    /* moded for R1 DATA DEVICE PHASE1, 2017-12-13, end */
    VOS_UINT32                          bitOpSscMode         : 1;
    VOS_UINT32                          bitOpSNssai          : 1;
    VOS_UINT32                          bitOpSessionAmbr     : 1;
    VOS_UINT32                          bitOpSpare           : 15;

    VOS_UINT32                          ulRslt;       /*????????????;????????:????????3.1*/
    VOS_UINT32                          ulLinkCid;
    VOS_UINT32                          ulPsCallId;         /*????????ID??????:0~31*/
    APP_ESM_BEARER_STATE_ENUM_UINT32    enBearerState;      /*????????????:0~1*/
    APP_ESM_BEARER_TYPE_ENUM_UINT32     enBearerType;       /*????????*/
    APP_ESM_IP_ADDR_STRU                stPDNAddrInfo;
    APP_ESM_IP_ADDR_STRU                stSubnetMask;
    APP_ESM_IP_ADDR_STRU                stGateWayAddrInfo;  /*????????*/
    APP_ESM_IP_ADDR_STRU                stDnsPrimAddrInfo;  /*??DNS???? */
    APP_ESM_IP_ADDR_STRU                stDnsSecAddrInfo;   /*??DNS???? */
    APP_ESM_IP_ADDR_STRU                stPCscfPrimAddrInfo;/*??P-CSCF???? */
    APP_ESM_IP_ADDR_STRU                stPCscfSecAddrInfo; /* ??P-CSCF???? */
    VOS_UINT32                          ulRabId;            /*??????????????????????????????????????ulEpsBid*/
    APP_ESM_APN_INFO_STRU               stApnInfo;
    APP_ESM_EPS_QOS_INFO_STRU           stSdfQosInfo;

    /* add for active pdp timer expire, 2016-10-20, begin */
    VOS_UINT8                           ucExpiredCount;     /* ?????????????????? */
    /* add for active pdp timer expire, 2016-10-20, end */
    PS_SSC_MODE_ENUM_UINT8              enSscMode;      /* 24501-9.10.4.12 */
    VOS_UINT8                           ucPduSessionId;
    VOS_UINT8                           ucReserve;
    /* added for verizon band13 pco requirement 2017-03-25, begin */
    APP_ESM_CUSTOM_PCO_INFO_STRU        stCustomPcoInfo;
    /* added for verizon band13 pco requirement 2017-03-25, end */
    /* added for R1 DATA DEVICE PHASE1, 2017-12-13, begin */
    APP_ESM_EPDG_INFO_STRU              stEpdgInfo;
    /* added for R1 DATA DEVICE PHASE1, 2017-12-13, end */


#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    /* TODO:??????????NR???????????????????????????? */
    APP_ESM_S_NSSAI_STRU                stSnssaiInfo;       /* 5G S-NASSAI */
    PS_PDU_SESSION_AMBR_STRU            stPduSessionAmbr;   /* 5G SESSION AMBR */

    PS_EPS_MAP_QOS_FLOW_LIST_STRU       stMapQosFlowList;
    PS_EPS_MAP_QOS_RULE_LIST_STRU       stMapQosRuleList;
#endif

} APP_ESM_PDP_SETUP_CNF_STRU;


typedef APP_ESM_PDP_SETUP_CNF_STRU APP_ESM_PDP_SETUP_IND_STRU;

/*****************************************************************************
 ??????    : APP_ESM_PDP_MODIFY_REQ_STRU
 ????????  : APP????????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;             /*??????????????*/
    VOS_UINT32                          ulCid;              /*??????????????????:0~31*/
    /* Added for DATA RETRY PHASEIII, 2016-6-23, begin */
    APP_ESM_BEARER_PRIO_ENUM_UINT32     enBearerPrio;       /* NAS signalling priority???? */
    /* Added for DATA RETRY PHASEIII, 2016-6-23, end */
    /* add for PS CALL ID,2018-05-22,begin */
    VOS_UINT32                          ulPsCallId;         /*????????ID??????:0~31*/
    /* add for PS CALL ID,2018-05-22,end */
}APP_ESM_PDP_MODIFY_REQ_STRU;

typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;
    VOS_UINT32                          ulPsCallId;         /*????????ID??????:0~31*/
    APP_ESM_BEARER_MODIFY_ENUM_UINT32   enBearerModifyType;

    VOS_UINT32                          bitOpBearerState    :1;
    VOS_UINT32                          bitOpBearerType     :1;
    VOS_UINT32                          bitOpRabId          :1;
    VOS_UINT32                          bitOpSdfQos         :1;
    /* added for verizon band13 pco requirement 2017-03-25, begin */
    VOS_UINT32                          bitOpCustomPco      :1; /* ????VERRIZON??????????,??FF00H??????APS??,????FF00H?????????????? 1:???? 0:?????? */
    /* added for verizon band13 pco requirement 2017-03-25, end */
    VOS_UINT32                          bitOpSpare          :27;

    VOS_UINT32                          ulRslt ;            /*????????????;????????:????????3.1*/

    APP_ESM_BEARER_STATE_ENUM_UINT32    enBearerState;      /*????????????:0~1*/
    APP_ESM_BEARER_TYPE_ENUM_UINT32     enBearerType;       /*????????*/

    VOS_UINT32                          ulRabId;            /*??????????????????????????????????????ulEpsBid*/

    APP_ESM_EPS_QOS_INFO_STRU           stSdfQosInfo;

    /* added for verizon band13 pco requirement 2017-03-25, begin */
    APP_ESM_CUSTOM_PCO_INFO_STRU        stCustomPcoInfo;
    /* added for verizon band13 pco requirement 2017-03-25, end */
} APP_ESM_PDP_MODIFY_CNF_STRU;

typedef APP_ESM_PDP_MODIFY_CNF_STRU APP_ESM_PDP_MODIFY_IND_STRU;


/*****************************************************************************
 ??????    : APP_ESM_PDP_RELEASE_REQ_STRU
 ????????  : APP????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;
    VOS_UINT32                          ulPsCallId;         /*????????ID??????:0~31*/

    /* Added for DATA RETRY PHASEI, 2016-03-21, Begin */
    APP_ESM_PDP_RELEASE_CAUSE_ENUM_UINT8          enCause;       /* ???????? */
    VOS_UINT8                                     ucDetachInd;   /* ????????????DETACH??VOS_TRUE????VOS_FALS?? */
    VOS_UINT8                                     ucRsv[2];      /* ???? */
    /* Added for DATA RETRY PHASEI, 2016-03-21, End */

}APP_ESM_PDP_RELEASE_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_PDP_RELEASE_CNF_STRU
 ????????  : ????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;
    VOS_UINT32                          ulPsCallId;         /*????????ID??????:0~31*/

    VOS_UINT32                          bitOpLinkCid        :1;
    VOS_UINT32                          bitOpPdnAddr        :1;
    /* added for verizon band13 pco requirement 2017-03-25, begin */
    VOS_UINT32                          bitOpCustomPco      :1; /* ????VERRIZON??????????,??FF00H??????APS??,????FF00H?????????????? 1:???? 0:?????? */
    /* added for verizon band13 pco requirement 2017-03-25, end */
    VOS_UINT32                          bitOpSpare          :29;

    VOS_UINT32                          ulRslt;


    VOS_UINT32                          ulLinkCid;
    APP_ESM_IP_ADDR_STRU                stPDNAddrInfo;

    /* add for VIA CL MT-DETACH make CID mod problem begin */
    /* ??APP????RELEASE IND??(RELEASE CNF????);    */
    VOS_UINT32                          ulEpsbId;
    /* add for VIA CL MT-DETACH make CID mod problem end */

    /* added for verizon band13 pco requirement 2017-03-25, begin */
    APP_ESM_CUSTOM_PCO_INFO_STRU        stCustomPcoInfo;
    /* added for verizon band13 pco requirement 2017-03-25, end */
    VOS_UINT8                          ucPduSessionId;
    /* fix pclint, add begin, 2018-11-06 */
    VOS_UINT8                           ucReserved[3];
    /* fix pclint, add end, 2018-11-06 */
}APP_ESM_PDP_RELEASE_CNF_STRU;

typedef APP_ESM_PDP_RELEASE_CNF_STRU APP_ESM_PDP_RELEASE_IND_STRU;

/*****************************************************************************
 ??????    : APP_ESM_PDP_MANAGER_IND_STRU
 ????????  : ????????????APP????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                                  ulOpId;
    VOS_UINT32                                  ulCid;
    VOS_UINT32                                  ulPsCallId;         /*????????ID??????:0~31????????????????TAF_APS_GetUnusedPsCallId()????????*/
    VOS_UINT32                                  bitOpLinkCid      : 1;
    VOS_UINT32                                  bitOpModifyTpye   : 1;
    VOS_UINT32                                  bitOpSpare        : 30;

    VOS_UINT32                                  ulLinkCid;
    APP_ESM_BEARER_OPERATE_TYPE_ENUM_UINT32     enOperateType;
    APP_ESM_BEARER_MODIFY_ENUM_UINT32           enBearerModifyType;

}APP_ESM_PDP_MANAGER_IND_STRU;

/*****************************************************************************
 ??????    : APP_ESM_PDP_MANAGER_RSP_STRU
 ????????  : APP????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                              ulOpId;
    VOS_UINT32                              ulCid;
    VOS_UINT32                              ulPsCallId;        /*????????ID??????:0~31*/
    APP_ESM_BEARER_ACT_RESULT_ENUM_UINT32   ulSetupRslt;       /*????????:0:??????????????????,1:????????*/


}APP_ESM_PDP_MANAGER_RSP_STRU;

/*****************************************************************************
 ??????    : APP_ESM_IPV6_PREFIX_STRU
 ????????  : APS-->ESM IPv6 Prefix????
*****************************************************************************/

/* APS????????????, IPV6????*/
typedef struct
{
    VOS_UINT8                           ucPrefixLen;
    VOS_UINT8                           aucReserved[3];

    VOS_UINT8                           aucPrefix[APP_ESM_IPV6_ADDR_LEN];
}APP_ESM_IPV6_PREFIX_STRU;

/*****************************************************************************
 ??????    : APP_ESM_IPV6_INFO_NOTIFY_STRU
 ????????  : APS-->ESM IPv6 Notify
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                      /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;        /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT8                           ucEpsbId;
    VOS_UINT8                           ucIpv6PrefixNum;
    VOS_UINT8                           aucReserved[2];
    APP_ESM_IPV6_PREFIX_STRU            astIpv6PrefixArray[APP_ESM_MAX_IPV6_PREFIX_NUM];
}APP_ESM_IPV6_INFO_NOTIFY_STRU;
/*modify begin */
/*****************************************************************************
 ??????    : APP_ESM_PROCEDURE_ABORT_NOTIFY_STRU
 ????????  : APS-->ESM ????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                      /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;        /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;
    VOS_UINT32                          ulCid;
    VOS_UINT32                          ulPsCallId;     /*????????ID??????:0~31*/
}APP_ESM_PROCEDURE_ABORT_NOTIFY_STRU;
/*modify end */

/* by begin 2013-05-29 Modify L4A */

/* ims begin */
/*****************************************************************************
 ??????    : APP_ESM_NDISCONN_REQ_STRU
 ????????  : APS-->ESM ????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                                  ulMsgId;    /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                                  ulOpId;
    VOS_UINT32                                  ulCid;
    VOS_UINT32                                  ulPsCallId;        /*????????ID??????:0~31*/

    VOS_UINT8                                   ucPduSessionId;
    VOS_UINT8                                   aucRsv[3];

    VOS_UINT32                                  bitOpPdnType           :1;
    VOS_UINT32                                  bitOpApn               :1;
    VOS_UINT32                                  bitOpGwAuth            :1;
    /* modify 2014-05-09 begin */
    VOS_UINT32                                  bitOpIpv4AddrAlloc     :1;                /* Ipv4AddrAlloc*/
    VOS_UINT32                                  bitOpPcscfDiscovery    :1;                /* P-CSCF discovery */
    VOS_UINT32                                  bitOpImCnSignalFlg     :1;                /* IM CN Signalling Flag */
    VOS_UINT32                                  bitOpSpare             :26;
    /* modify 2014-05-09 end */

    APP_ESM_APN_INFO_STRU                       stApnInfo;
    APP_ESM_PDN_TYPE_ENUM_UINT32                enPdnType;
    APP_ESM_GW_AUTH_INFO_STRU                   stGwAuthInfo;
	/*Modify for CL multimode 2014-02-14 begin*/
    APP_ESM_PDN_REQUEST_TYPE_ENUM_UINT32        enPdnReqestType;
    /*Modify for CL multimode 2014-02-14 end*/
    /* 2014-05-09 begin */
    APP_ESM_PDP_EMC_IND_ENUM_UINT32             enEmergencyInd;
    APP_ESM_IPV4_ADDR_ALLOC_TYPE_ENUM_UINT32    enIpv4AddrAlloc;
    APP_ESM_PCSCF_DISCOVERY_ENUM_UINT32         enPcscfDiscovery;
    APP_ESM_IMS_CN_SIG_FLAG_ENUM_UINT32         enImCnSignalFlg;
    /* 2014-05-09 end */
    /* Added for DATA RETRY PHASEIII, 2016-6-23, begin */
    APP_ESM_BEARER_PRIO_ENUM_UINT32               enBearerPrio;
    /* Added for DATA RETRY PHASEIII, 2016-6-23, end */
} APP_ESM_NDISCONN_REQ_STRU;

/* ims end */

/*****************************************************************************
 ??????    : APP_ESM_NDISCONN_CNF_STRU
 ????????  : ????APS-->ESM ????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                                  ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                                  ulOpId;
    VOS_UINT32                                  ulCid;
    VOS_UINT32                                  ulPsCallId;        /*????????ID??????:0~31*/

    VOS_UINT32                                  bitOpBearerState    :1;
    VOS_UINT32                                  bitOpBearerType     :1;
    VOS_UINT32                                  bitOpPdnAddr        :1;
    VOS_UINT32                                  bitOpRabId          :1;
    VOS_UINT32                                  bitOpApn            :1;
    VOS_UINT32                                  bitOpSdfQos         :1;
    VOS_UINT32                                  bitOpDnsPrim        :1;
    VOS_UINT32                                  bitOpDnsSec         :1;
    VOS_UINT32                                  bitOpPCscfPrim      :1;
    VOS_UINT32                                  bitOpPCscfSec       :1;
    VOS_UINT32                                  bitOpGateWayAddrInfo    :1;
    /* added for verizon band13 pco requirement 2017-03-25, begin */
    VOS_UINT32                                  bitOpCustomPco      :1; /* ????VERRIZON??????????,??FF00H??????APS??,????FF00H?????????????? 1:???? 0:?????? */
    /* added for verizon band13 pco requirement 2017-03-25, end */
    /* moded for R1 DATA DEVICE PHASE1, 2017-12-13, begin */
    VOS_UINT32                                  bitOpEpdgInfo        : 1; /* ????R1??????????,??EPDG??????????APS??,??????????????IMSA?? 1:???? 0:?????? */
    /* moded for R1 DATA DEVICE PHASE1, 2017-12-13, end */
    VOS_UINT32                                  bitOpSscMode         : 1;
    VOS_UINT32                                  bitOpSNssai          : 1;
    VOS_UINT32                                  bitOpSessionAmbr     : 1;
    VOS_UINT32                                  bitOpSpare           : 16;

    VOS_UINT32                                  ulRslt;       /*????????:????????3.1*/

    APP_ESM_BEARER_STATE_ENUM_UINT32            enBearerState;      /*????????????:0~1*/
    APP_ESM_BEARER_TYPE_ENUM_UINT32             enBearerType;       /*????????*/

    APP_ESM_IP_ADDR_STRU                        stPDNAddrInfo;
    APP_ESM_IP_ADDR_STRU                        stSubnetMask;
    APP_ESM_IP_ADDR_STRU                        stGateWayAddrInfo;
    APP_ESM_IP_ADDR_STRU                        stDnsPrimAddrInfo;   /*??DNS????*/
    APP_ESM_IP_ADDR_STRU                        stDnsSecAddrInfo;    /*??DNS????*/
    APP_ESM_IP_ADDR_STRU                        stPCscfPrimAddrInfo; /*??P-CSCF????*/
    APP_ESM_IP_ADDR_STRU                        stPCscfSecAddrInfo;  /*??P-CSCF????*/
    VOS_UINT32                                  ulRabId;            /*??????????????????????????????????ulEpsbId*/
    APP_ESM_APN_INFO_STRU                       stApnInfo;
    APP_ESM_EPS_QOS_INFO_STRU                   stSdfQosInfo;

    /* add for active pdp timer expire, 2016-10-20, begin */
    VOS_UINT8                                   ucExpiredCount;     /* ?????????????????? */
    /* add for active pdp timer expire, 2016-10-20, end */
    PS_SSC_MODE_ENUM_UINT8                      enSscMode;      /* 24501-9.10.4.12 */
    VOS_UINT8                                   ucPduSessionId;
    VOS_UINT8                                   ucReserve;
    /* added for verizon band13 pco requirement 2017-03-25, begin */
    APP_ESM_CUSTOM_PCO_INFO_STRU                stCustomPcoInfo;
    /* added for verizon band13 pco requirement 2017-03-25, end */
    /* added for R1 DATA DEVICE PHASE1, 2017-12-13, begin */
    APP_ESM_EPDG_INFO_STRU                      stEpdgInfo;
    /* added for R1 DATA DEVICE PHASE1, 2017-12-13, end */

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    /* TODO:??????????NR???????????????????????????? */
    APP_ESM_S_NSSAI_STRU                        stSnssaiInfo;       /* 5G S-NASSAI */
    PS_PDU_SESSION_AMBR_STRU                    stPduSessionAmbr;   /* 5G SESSION AMBR */

    PS_EPS_MAP_QOS_FLOW_LIST_STRU               stMapQosFlowList;
    PS_EPS_MAP_QOS_RULE_LIST_STRU               stMapQosRuleList;
#endif

} APP_ESM_NDISCONN_CNF_STRU;
/*****************************************************************************
 ??????    : APP_ESM_SET_CGDCONT_REQ_STRU
 ????????  : CGDCONTAT??????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                                  ulMsgId;    /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                                  ulOpId;
    VOS_UINT32                                  ulCid;

    VOS_UINT32                                  bitOpBearType          :1;
    VOS_UINT32                                  bitOpPdnType           :1;
    VOS_UINT32                                  bitOpApn               :1;

    VOS_UINT32                                  bitOpIpv4AddrAllocType :1;
    /* ims begin */
    VOS_UINT32                                  bitOpPcscfDiscovery    :1;
    VOS_UINT32                                  bitOpImsCnSignalFlag   :1;
    /* ims end */
    VOS_UINT32                                  bitOpSpare             :26;


    APP_ESM_PARA_SET_ENUM_UINT32                enSetType;       /*0:SET;1:DEL*/
    APP_ESM_CID_TYPE_ENUM_UINT32                enBearCidType;
    APP_ESM_APN_INFO_STRU                       stApnInfo;
    APP_ESM_PDN_TYPE_ENUM_UINT32                enPdnType;
    APP_ESM_IPV4_ADDR_ALLOC_TYPE_ENUM_UINT32    enIpv4AddrAllocType;
    /* ims begin */
    APP_ESM_PCSCF_DISCOVERY_ENUM_UINT32         enPcscfDiscovery;
    APP_ESM_IMS_CN_SIG_FLAG_ENUM_UINT32         enImsCnSignalFlag;
    /* ims end */
} APP_ESM_SET_CGDCONT_REQ_STRU;

/*****************************************************************************
 ??????    : APP_ESM_SET_CGDCONT_CNF_STRU
 ????????  : ????CGDCONTAT??????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                                  ulMsgId;    /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                                  ulOpId;
    VOS_UINT32                                  ulCid;

    VOS_UINT32                                  ulRslt;

} APP_ESM_SET_CGDCONT_CNF_STRU;
/* end 2013-05-29 Modify L4A */

/* added for Band13 R1 PHASE1, 2017-10-16, begin */
/*****************************************************************************
 ??????    : APP_ESM_APN_ENABLE_INFO_CHANGE_NTF_STRU
 ????????  : APN enable??????????????????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                                  ulMsgId;    /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                                  ulOpId;

    VOS_UINT32                                  ulCid;          /* CID [0,11] */
    APP_ESM_APN_TYPE_ENUM_UINT8                 enApnType;      /* apn class */
    VOS_UINT8                                   ucEnableFlag;  /* apn enable flag */
    VOS_UINT8                                   aucReserved[2];
} APP_ESM_APN_ENABLE_INFO_CHANGE_NTF_STRU;
/* added for Band13 R1 PHASE1, 2017-10-16, end */


/*****************************************************************************
 ??????    : APP_ESM_PDP_MANAGE_INFO_STRU
 ????????  : PDP????????????????
*****************************************************************************/
typedef struct
{
    APP_ESM_BEARER_MANAGE_MODE_ENUM_UINT8    enMode;         /* ???????????? */
    APP_ESM_BEARER_MANAGE_TYPE_ENUM_UINT8    enType;         /* ???????????? */
    VOS_UINT8                                aucReserve1[2]; /* ???????????????? */
}APP_ESM_PDP_MANAGE_INFO_STRU;

/* Added for DCM DATA OFF, 2017-11-11, begin */
/*****************************************************************************
 ??????    : APP_ESM_DATA_SWITCH_INFO_STRU
 ????????  : ????????????????????
*****************************************************************************/
typedef struct
{
    VOS_UINT8                               ucDataSwitch;                       /* ???????????????? */
    VOS_UINT8                               ucDataRoamSwitch;                   /* ???????????????????? */

    VOS_UINT8                               aucResv[2];
}APP_ESM_DATA_SWITCH_INFO_STRU;
/* Added for DCM DATA OFF, 2017-11-11, end */

typedef struct
{
    APP_ESM_MSG_TYPE_ENUM_UINT32        enMsgId;            /*_H2ASN_MsgChoice_Export APP_ESM_MSG_TYPE_ENUM_UINT32*/
    VOS_UINT8                           aucMsg[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          APP_ESM_MSG_TYPE_ENUM_UINT32
    ****************************************************************************/
}APP_ESM_INTERFACE_MSG_DATA;
/*_H2ASN_Length UINT32*/


typedef struct
{
    VOS_MSG_HEADER
    APP_ESM_INTERFACE_MSG_DATA           stMsgData;
} AppEsmInterface_MSG;

/*****************************************************************************
 ????????   : APP_ESM_CID_PF_STRU
 ????????   :
 ASN.1 ???? :
 ????????   : CID????PF??????
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulCid;
    APP_ESM_TFT_INFO_STRU               stPf;
} APP_ESM_CID_PF_STRU;
/*****************************************************************************
 ????????   : SM_ESM_TFT_STRU
 ????????   :
 ASN.1 ???? :
 ????????   : TFT????
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulPfNum;
    APP_ESM_CID_PF_STRU                 astCidPf[PS_MAX_PF_NUM_IN_TFT];
} APP_ESM_TFT_STRU;

/*****************************************************************************
 ????????   : APP_ESM_TRANSACTION_ID_STRU
 ????????   :
 ASN.1 ???? :
 ????????   : Transaction ID ????
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucTiFlag;
    VOS_UINT8                           ucTiValue;

    VOS_UINT8                           aucReserved[2];

} APP_ESM_TRANSACTION_ID_STRU;


typedef struct
{
    VOS_UINT8                           aucPcscfAddr[APP_MAX_IPV4_ADDR_LEN];
} APP_ESM_IPV4_PCSCF_STRU;


typedef struct
{
    VOS_UINT8                           aucPcscfAddr[APP_MAX_IPV6_ADDR_LEN];
} APP_ESM_IPV6_PCSCF_STRU;


typedef struct
{
    VOS_UINT8                           ucIpv4PcscfAddrNum;                     /* IPV4??P-CSCF??????????????????[0,8] */
    VOS_UINT8                           aucRsv[3];                              /* ???? */

    APP_ESM_IPV4_PCSCF_STRU             astIpv4PcscfAddrList[APP_ESM_PCSCF_ADDR_MAX_NUM];
} APP_ESM_IPV4_PCSCF_LIST_STRU;


typedef struct
{
    VOS_UINT8                           ucIpv6PcscfAddrNum;                     /* IPV6??P-CSCF??????????????????[0,8] */
    VOS_UINT8                           aucRsv[3];                              /* ???? */

    APP_ESM_IPV6_PCSCF_STRU             astIpv6PcscfAddrList[APP_ESM_PCSCF_ADDR_MAX_NUM];
} APP_ESM_IPV6_PCSCF_LIST_STRU;

/*****************************************************************************
 ????????   : APP_ESM_PCO_IPV4_ITEM_STRU
 ????????   :
 ASN.1 ???? :
 ????????   : aucPriDns   - Primary DNS server Address
              aucSecDns   - Secondary DNS server Address
              aucGateWay  - Peer IP address
              aucPriNbns  - Primary WINS DNS address
              aucSecNbns  - Seocndary WINS DNS address
              aucPriPcscf - Primary P-CSCF address
              aucSecPcscf - Seocndary P-CSCF address

              aucGateWay[0]????????????????aucGateWay[3]??????????????
              aucPriNbns[0]????????????????aucPriNbns[3]??????????????
              aucSecNbns[0]????????????????aucSecNbns[3]??????????????
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpPriDns     : 1;
    VOS_UINT32                          bitOpSecDns     : 1;
    VOS_UINT32                          bitOpGateWay    : 1;
    VOS_UINT32                          bitOpPriNbns    : 1;
    VOS_UINT32                          bitOpSecNbns    : 1;
    VOS_UINT32                          bitOpIpv4Mtu    : 1;
    VOS_UINT32                          bitOpSpare      : 26;

    VOS_UINT8                           aucPriDns[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecDns[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucGateWay[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucPriNbns[APP_MAX_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecNbns[APP_MAX_IPV4_ADDR_LEN];

    APP_ESM_IPV4_PCSCF_LIST_STRU        stIpv4PcscfList;
    VOS_UINT16                          usIpv4Mtu;
    VOS_UINT8                           aucReserved[2];
} APP_ESM_PCO_IPV4_ITEM_STRU;

/*****************************************************************************
 ????????   : APP_ESM_PCO_IPV6_ITEM_STRU
 ????????   :
 ASN.1 ???? :
 ????????   : aucPriDns   - Primary DNS server Address
              aucSecDns   - Secondary DNS server Address
              aucPriPcscf - Primary P-CSCF address
              aucSecPcscf - Seocndary P-CSCF address

              aucPriDns[0]????????????????aucPriDns[15]??????????????
              aucSecDns[0]????????????????aucSecDns[15]??????????????
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpPriDns     : 1;
    VOS_UINT32                          bitOpSecDns     : 1;
    VOS_UINT32                          bitOpSpare      : 30;

    VOS_UINT8                           aucPriDns[APP_MAX_IPV6_ADDR_LEN];
    VOS_UINT8                           aucSecDns[APP_MAX_IPV6_ADDR_LEN];

    APP_ESM_IPV6_PCSCF_LIST_STRU        stIpv6PcscfList;
} APP_ESM_PCO_IPV6_ITEM_STRU;


/*****************************************************************************
 ????????   : APP_ESM_NR_HANDOVER_TO_LTE_IND_STRU
 ????????   :
 ASN.1 ???? :
 ????????   : ESM -> APS
              EPS Bearer??????????(Activate, Modify, Deactivate), ????EPS??????????
              ??????TI, ????????????????????. ????????????????TI, SM??????????????
              linked EPS bearer identity (if available) ?????? linked TI;
              PDN address and APN of the default EPS bearer context??????PDP address and APN of the default PDP context;
              TFT of the default EPS bearer context?????? the TFT of the default PDP context;
              TFTs of the dedicated EPS bearer contexts ??????TFTs of the secondary PDP contexts;
              ????????????SM_ESM_PDP_OPT_MODIFY????OP????0????????????????????OP????1??????????????????
              ??????????TFT??????????bitOpPf????1??ulPfNum????0??
              ????????DNS??NBNS,P-CSCF??????????????????OP??????1??????????????????0??
              ucLlcSapi??ucRadioPriority??ucPacketFlowId????????????????GU????SM????
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /* _H2ASN_Skip */
    VOS_UINT32                          ulMsgId;            /* _H2ASN_Skip */

    VOS_UINT32                          bitOpLinkedEpsbId   : 1;
    VOS_UINT32                          bitOpEpsQos         : 1;
    VOS_UINT32                          bitOpTft            : 1;
    VOS_UINT32                          bitOpApn            : 1;
    VOS_UINT32                          bitOpApnAmbr        : 1;
    VOS_UINT32                          bitOpTransId        : 1;
    VOS_UINT32                          bitOpNegQos         : 1;
    VOS_UINT32                          bitOpLlcSapi        : 1;
    VOS_UINT32                          bitOpRadioPriority  : 1;
    VOS_UINT32                          bitOpPacketFlowId   : 1;
    VOS_UINT32                          bitOpBcm            : 1;
    VOS_UINT32                          bitOpEmcInd         : 1;
    VOS_UINT32                          bitOpImCnSigalFlag  : 1;
    VOS_UINT32                          bitOpErrorCode      : 1;
    VOS_UINT32                          bitOpCustomPco      : 1;    /* ????VERRIZON??????????,??FF00H??????APS??,????FF00H?????????????? 1:???? 0:?????? */

    VOS_UINT32                          bitOpSpare          : 17;

    APP_ESM_HANDOVER_RESULT_ENUM_UINT32 enResult;
    VOS_UINT32                          ulEpsbId;
    VOS_UINT32                          ulBitCid;
    VOS_UINT32                          ulLinkedEpsbId;                         /* ??????????????EPS????ID */

    VOS_UINT8                           ucPsCallId;
    VOS_UINT8                           ucPduSessionId;
    PS_SSC_MODE_ENUM_UINT8              enSscMode;                              /* ??????SSC MODE1??????????24501-9.10.4.12 */
    VOS_UINT8                           aucRsv[1];

    APP_ESM_IP_ADDR_STRU                stPdnAddr;
    PS_EPS_APN_AMBR_STRU                stEpsApnAmbr;
    APP_ESM_EPS_QOS_INFO_STRU           stEpsQos;
    APP_ESM_TFT_STRU                    stTft;
    APP_ESM_APN_STRU                    stApn;
    APP_ESM_QOS_INFO_STRU               stNegQos;
    APP_ESM_PCO_IPV4_ITEM_STRU          stPcoIpv4Item;
    APP_ESM_PCO_IPV6_ITEM_STRU          stPcoIpv6Item;

    VOS_UINT8                           ucLlcSapi;
    VOS_UINT8                           ucRadioPriority;
    VOS_UINT8                           ucPacketFlowId;
    APP_ESM_BCM_ENUM_UINT8              enBcm;
    APP_ESM_TRANSACTION_ID_STRU         stTransId;

    APP_ESM_PDP_EMC_IND_ENUM_UINT32     enEmergencyInd;
    APP_ESM_IMS_CN_SIG_FLAG_ENUM_UINT32 enImCnSignalFlag;

    VOS_UINT32                          ulErrorCode;                        /* ????????????????????SM???????? */
    APP_ESM_CUSTOM_PCO_INFO_STRU        stCustomPcoInfo;
    APP_ESM_S_NSSAI_STRU                stSnssaiInfo;                       /* 5G S-NASSAI */
} APP_ESM_NR_HANDOVER_TO_LTE_IND_STRU;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_UINT32 APP_GetSdfParaByModemId
(
    MODEM_ID_ENUM_UINT16      enModemId,
    VOS_UINT32                         *pulSdfNum,
    APP_ESM_SDF_PARA_STRU              *pstSdfPara
);
extern VOS_UINT32 APP_GetPdpManageInfoByModemId
(
    MODEM_ID_ENUM_UINT16      enModemId,
    APP_ESM_PDP_MANAGE_INFO_STRU  *pstPdpManageInfo
);
extern VOS_UINT32 APP_GetCidImsSuppFlagByModemId
(
    MODEM_ID_ENUM_UINT16      enModemId,
    VOS_UINT8                           ucCid,
    VOS_UINT8                          *pucImsSuppFlag
);
/* added for Band13 R1 PHASE1, 2017-10-16, begin */

extern VOS_UINT32 APP_ESM_GetAllApnClassInfo
(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT32                         *pulApnNum,
    APP_ESM_APN_CLASS_INFO_STRU        *pstApnClassInfo
);
/* added for Band13 R1 PHASE1, 2017-10-16, end */

/* Added for DCM DATA OFF, 2017-11-1, begin */
extern VOS_UINT32 APP_GetDataSwitchInfoByModemId
(
    MODEM_ID_ENUM_UINT16                enModemId,
    APP_ESM_DATA_SWITCH_INFO_STRU      *pstDataSwitchInfo
);
/* Added for DCM DATA OFF, 2017-11-1, end */


extern VOS_UINT32 APP_AllocPsCallIdByModemId
(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                          *pucPsCallId
);


extern VOS_UINT32 APP_AllocPduSessionIdByModemId
(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                          *pucPduSessionId
);

extern VOS_VOID APP_FreePduSessionIdByModemId
(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucPduSessionId
);

/*****************************************************************************
  9 OTHERS
*****************************************************************************/










#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif





#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of AppEsmInterface.h */
