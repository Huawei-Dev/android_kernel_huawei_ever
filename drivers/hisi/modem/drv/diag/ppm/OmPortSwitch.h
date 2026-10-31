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


#ifndef __OM_PORT_SWITCH_H__
#define __OM_PORT_SWITCH_H__

/*****************************************************************************
  1 ??????????????
*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#pragma pack(push)
#pragma pack(4)

/*****************************************************************************
 ??????    : PPM_PORT_SWITCH_NV_INFO
 ????????  : ??????????NV??ACORE??????CCORE????????ICC????????????
*****************************************************************************/
typedef struct
{
    u32 msgid;
    u32 sn;
    u32 ret; //????????????0 ??????????C????????????
    u32 len;
    DIAG_CHANNLE_PORT_CFG_STRU data;
}PPM_PORT_SWITCH_NV_INFO;

typedef struct
{
    u32                          ulPortTypeErr;
    u32                          ulSwitchFail;
    u32                          ulSwitchSucc;
    u32                          ulStartSlice;
    u32                          ulEndSlice;
}PPM_PORT_CFG_INFO_STRU;

enum
{
    PPM_MSGID_PORT_SWITCH_NV_A2C = 1,   /* PPM????????NV??A????C????NV???? */
    PPM_MSGID_PORT_SWITCH_NV_C2A = 2,   /* PPM????????NV??????C??????A?? */
    PPM_MSGID_BUTT
};//PPM_????ID????

#define AP_CP_PORT_SWITCH_TIMEOUT_MS        (10)    /* 100ms */

typedef struct
{
    u32 ulMsgId;    /* ????ID */
    u32 ulLen;      /* ???????? */
    u32 ulPortNum;  /* ?????? */
}AP_CP_MSG_PORT_STRU;

typedef struct
{
    u32 ulMsgId;    /* ????ID */
    u32 ulLen;      /* ???????? */
    u32 ulRet;      /* ?????? */
}AP_CP_MSG_RET_STRU;
/*****************************************************************************
  4 ????????
*****************************************************************************/
u32 PPM_DisconnectTLPort(void);
void PPM_DisconnectAllPort(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel);
void PPM_RegDisconnectCb(PPM_DisconnectTLPortFuc cb);
u32 PPM_LogPortSwitch(u32  ulPhyPort, bool ulEffect);
int PPM_PortSwtichInit(void);
void ppm_cp_port_switch_read_cb(void* context);
void ppm_ap_port_switch_read_cb(void* context);
u32 PPM_PhyPortSwitch(u32 ulPhyPort);
u32 PPM_SetIndMode(void);
u32 PPM_WritePortNv(void);


#pragma pack(pop)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of OmPortSwitch.h*/

