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


#ifndef __BSP_DIAG_H__
#define __BSP_DIAG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(push)
#pragma pack(4)


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include <product_config.h>
#include <stdarg.h>
#include "osl_types.h"
#include "bsp_diag_frame.h"
/*****************************************************************************
  2 macro
*****************************************************************************/
/* ???????????? */
#define DIAG_FRAME_MAX_LEN      (4*1024)
/* ???????????????? */
#define DIAG_FRMAE_MAX_CNT      (16)
/* ???????????? */
#define DIAG_FRAME_SUM_LEN      (DIAG_FRAME_MAX_LEN * DIAG_FRMAE_MAX_CNT)

/*4????????*/
#define ALIGN_DDR_WITH_4BYTE(len)       (((len) + 3)&(~3))
/*8????????*/
#define ALIGN_DDR_WITH_8BYTE(len)       (((len) + 7)&(~7))

/*****************************************************************************
  3 Enum
*****************************************************************************/
typedef struct
{
    u32                  ulHisiMagic;   /*"HISI"*/
    u32                  ulDataLen;      /*????????*/
}DIAG_SRV_SOCP_HEADER_STRU;

typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
}DIAG_SRV_HEADER_STRU;

/* ==============????????????????????====================================== */

/* diag event report???????????????? */
typedef struct
{
    u32 ulNo;      /* ????*/
    u32 ulId;      /* ????????????ID,????????????,????,????,????????????????????????*/
    u32 ulModule;  /* ??????????????????ID */
    s8   aucDta[0]; /* ??????????????*/    /*lint !e43 */
} DIAG_CMD_LOG_EVENT_IND_STRU;

/* diag air report???????????????? */
typedef struct
{
    u32 ulModule;     /* ??????ID*/
    u32 ulSide;       /* 1: NET-->UE, 2: UE-->NET*/
    u32 ulNo;         /* ????*/
    u32 ulId;         /* ID*/
    s8 aucDta[0];     /* ??????????????*/  /*lint !e43 */
} DIAG_CMD_LOG_AIR_IND_STRU;

/* diag volte report???????????????? */
typedef struct
{
    u32 ulModule;     /* ??????ID*/
    u32 ulSide;       /* 1: NET-->UE, 2: UE-->NET*/
    u32 ulNo;         /* ????*/
    u32 ulId;         /* ID*/
    s8   aucDta[0];    /* ??????????????*/     /*lint !e43 */
} DIAG_CMD_LOG_VOLTE_IND_STRU;


/* diag userplane report???????????????? */
typedef struct
{
    u32 ulModule;     /* ??????ID*/
    u32 ulNo;         /* ????*/
    u32 ulId;         /* ID*/
    s8 aucDta[0];     /* ??????????????*/  /*lint !e43 */
} DIAG_CMD_LOG_USERPLANE_IND_STRU;


/* diag ?????????????????????????? */
typedef struct
{
    u32 ulModule;     /* ??????ID*/
    u32 ulMsgId;      /* ID*/
    u32 ulNo;         /* ????*/
    s8   aucDta[0];    /* ??????????????*/     /*lint !e43 */
} DIAG_CMD_TRANS_IND_STRU;

/* diag drive??????????????????????TRANS???? */
typedef struct
{
    u32 ulModule;     /* ??????ID*/
    u32 ulMsgId;      /* ID*/
    u32 ulNo;         /* ????*/
    s8  aucDta[0];    /* ??????????????*/     /*lint !e43 */
} DIAG_CMD_DT_IND_STRU;

/* diag trace report???????????????? */
typedef struct
{
    u32 ulModule;     /* ??????ID*/
    u32 ulDestMod;    /* ????????ID*/
    u32 ulNo;         /* ????*/
    u32 ulId;         /* ID*/
    s8 aucDta[0];     /* ??????????????*/  /*lint !e43 */
} DIAG_CMD_LOG_LAYER_IND_STRU;

/* ======================================================================== */

/* CNF?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
}DIAG_API_CNF_HEADER_STRU;

/* ?????????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
}DIAG_SRV_LOG_ID_HEADER_STRU;

/* ?????????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
}DIAG_SRV_LOG_HEADER_STRU;

/* trans ?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
    DIAG_CMD_TRANS_IND_STRU         trans_header;
}DIAG_SRV_TRANS_HEADER_STRU;

/* trans ?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
    DIAG_CMD_DT_IND_STRU            dt_header;
}DIAG_SRV_DT_HEADER_STRU;

/* event ?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
    DIAG_CMD_LOG_EVENT_IND_STRU     event_header;
}DIAG_SRV_EVENT_HEADER_STRU;

/* air ?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
    DIAG_CMD_LOG_AIR_IND_STRU       air_header;
}DIAG_SRV_AIR_HEADER_STRU;

/* volte ?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
    DIAG_CMD_LOG_VOLTE_IND_STRU     volte_header;
}DIAG_SRV_VOLTE_HEADER_STRU;

/* trace ?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
    DIAG_CMD_LOG_LAYER_IND_STRU     trace_header;
}DIAG_SRV_TRACE_HEADER_STRU;

/* layer ?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
    DIAG_CMD_LOG_LAYER_IND_STRU     layer_header;
}DIAG_SRV_LAYER_HEADER_STRU;

/* user ?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
    DIAG_CMD_LOG_USERPLANE_IND_STRU user_header;
}DIAG_SRV_USER_HEADER_STRU;

/* drv log?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
    diag_print_head_stru            print_head;
}DIAG_DRV_STRING_HEADER_STRU;

/* cnf ?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
}DIAG_SRV_CNF_HEADER_STRU;


/* trans ?????????? */
typedef struct
{
    DIAG_SRV_SOCP_HEADER_STRU       socp_header;
    diag_frame_head_stru            frame_header;
    DIAG_CMD_TRANS_IND_STRU          comm_header;
}DIAG_SRV_COMM_HEADER_STRU;

/* ======================================================================== */

/* ??????????OSA??MsgBlock?????????????????? */
#pragma pack(1)
typedef struct
{
    u32    ulSenderCpuId;
    u32    ulSenderPid;
    u32    ulReceiverCpuid;
    u32    ulReceiverPid;
    u32    ulLength;
    u32    ulMsgId;
    u8     aucValue[0];   /*lint !e43 */
}DIAG_OSA_MSG_STRU;
#pragma pack()
/*************serviec start*********************/
typedef struct
{
    u32          ulHeaderSize;   /* ???????????? */
    void            *pHeaderData;   /* ?????? */

    u32          ulDataSize;     /* ucData?????? */
    void            *pData;         /* ???? */
}DIAG_MSG_REPORT_HEAD_STRU;



/*****************************************************************************
  4 UNION
*****************************************************************************/

/*****************************************************************************
  5 struct
*****************************************************************************/

/*****************************************************************************
  6 Extern Global Variable
*****************************************************************************/

/*****************************************************************************
  7 Fuction Extern
*****************************************************************************/
unsigned int bsp_diag_report_drv_log(unsigned int level, char* fmt, va_list arg);

#pragma pack(pop)

#ifdef __cplusplus
    #if __cplusplus
            }
    #endif
#endif

#endif
