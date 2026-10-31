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

#include <mdrv.h>
#include <securec.h>
#include <bsp_slice.h>
#include <osl_spinlock.h>
#include <osl_sem.h>
#include <osl_list.h>
#include <osl_malloc.h>
#include <osl_sem.h>
#include "msp_service.h"
#include "soc_socp_adapter.h"
#include "bsp_diag.h"
#include "bsp_diag_frame.h"
#include "diag_system_debug.h"
#include "scm_ind_src.h"
#include "scm_cnf_src.h"
#include "diag_service.h"


#define  THIS_MODU mod_diag

DIAG_DFR_INFO_STRU  g_stDFRcnf = {};
spinlock_t          g_stSrvIndSrcBuffSpinLock;
spinlock_t          g_stSrvCnfSrcBuffSpinLock;

typedef struct
{
    osl_sem_id                 ListSem;
    struct list_head           ListHeader;
} DIAG_SRVC_MAIN_STRU;


#define     DIAG_PKT_TIMEOUT_LEN            (32768*3)     /* 3s */

typedef struct
{
    diag_service_head_stru  stHead;
    u32              ulSlice;        /* ?????????????????????????????? */
    u32              ulFrameDataLen;
    u32              ulFrameOffset;
    struct list_head FrameList;
    diag_frame_head_stru    *pFrame;
} DIAG_SRVC_FRAME_NODE_STRU;

DRV_DIAG_SERVICE_FUNC g_fnService = NULL;
DIAG_DFR_INFO_STRU g_stDFRreq = {};

DIAG_SRVC_MAIN_STRU  g_stDiagSrvc = {};

DIAG_SRV_CTRL g_SrvCtrl = {
    SOCP_CODER_SRC_PS_IND,
    SOCP_CODER_SRC_CNF,
};

void diag_SvcFillHeader(DIAG_SRV_HEADER_STRU *pstSrvHeader)
{
    u8 auctime[8];
    u32 ulTimeStampLen = sizeof(pstSrvHeader->frame_header.stService.aucTimeStamp);

    /* ?????????????????? */
    pstSrvHeader->socp_header.ulHisiMagic = DIAG_SOCP_HEAD_MAGIC;
    pstSrvHeader->socp_header.ulDataLen = 0;

    pstSrvHeader->frame_header.stService.sid8b = DIAG_FRAME_MSP_SID_DIAG_SERVICE;
    pstSrvHeader->frame_header.stService.mdmid3b = 0;
    pstSrvHeader->frame_header.stService.rsv1b = 0;
    pstSrvHeader->frame_header.stService.ssid4b = DIAG_FRAME_SSID_APP_CPU;
    pstSrvHeader->frame_header.stService.sessionid8b = MSP_SERVICE_SESSION_ID;
    /*??????????????*/
    pstSrvHeader->frame_header.stService.mt2b = DIAG_FRAME_MT_IND;
    pstSrvHeader->frame_header.stService.index4b = 0;
    pstSrvHeader->frame_header.stService.eof1b = 0;
    pstSrvHeader->frame_header.stService.ff1b = 0;
    pstSrvHeader->frame_header.stService.MsgTransId = 0;

    (void)mdrv_timer_get_accuracy_timestamp((u32*)&(auctime[4]), (u32*)&(auctime[0]));
    if(memcpy_s(pstSrvHeader->frame_header.stService.aucTimeStamp, sizeof(pstSrvHeader->frame_header.stService.aucTimeStamp), \
                auctime, ulTimeStampLen))
    {
        diag_error("memcpy fail\n");
    }

}
/*****************************************************************************
 Function Name   : diag_PktTimeoutClear
 Description     : ????????????????????????????????????????
*****************************************************************************/
void diag_PktTimeoutClear(void)
{
    DIAG_SRVC_FRAME_NODE_STRU *p_curr_node = NULL;
    DIAG_SRVC_FRAME_NODE_STRU *p_next_node = NULL;
    u32 ulCurSlice = bsp_get_slice_value();

    osl_sem_down(&g_stDiagSrvc.ListSem);

    list_for_each_entry_safe(p_curr_node, p_next_node, &g_stDiagSrvc.ListHeader, FrameList)
    {
        if((ulCurSlice > p_curr_node->ulSlice) && ((ulCurSlice - p_curr_node->ulSlice) > DIAG_PKT_TIMEOUT_LEN))
        {
            if((NULL != p_curr_node->FrameList.next) && (NULL != p_curr_node->FrameList.prev))
            {
                list_del(&(p_curr_node->FrameList));
            }

            diag_crit("diag_PktTimeoutClear delete node.\n");

            if(NULL != p_curr_node->pFrame)
            {
                osl_free(p_curr_node->pFrame);
            }

            osl_free(p_curr_node);
        }
    }

    (void)osl_sem_up(&(g_stDiagSrvc.ListSem));
}


/*****************************************************************************
 Function Name   : diag_SrvcCreatePkt
 Description     : ??????????????????????????????

 History         :
    1.c64416         2015-03-18  Draft Enact

*****************************************************************************/
/*lint -save -e429*/
void diag_SrvcCreatePkt(diag_frame_head_stru *pFrame)
{
    struct list_head* me = NULL;
    DIAG_SRVC_FRAME_NODE_STRU *pTempNode;

    /*????????????????????????*/
    if(pFrame->u32MsgLen + sizeof(diag_frame_head_stru) > DIAG_FRAME_SUM_LEN)
    {
        diag_error("msg len too large, msglen = 0x%x\n", pFrame->u32MsgLen);
        return;
    }

    /* ????????????????????transid???????????????? */
    osl_sem_down(&g_stDiagSrvc.ListSem);

    list_for_each(me, &g_stDiagSrvc.ListHeader)
    {
        pTempNode = list_entry(me, DIAG_SRVC_FRAME_NODE_STRU, FrameList);

        pTempNode->stHead.index4b = pFrame->stService.index4b;
        pTempNode->stHead.eof1b   = pFrame->stService.eof1b;
        pTempNode->stHead.ff1b    = pFrame->stService.ff1b;
        if(memcmp(&pTempNode->stHead, &pFrame->stService, sizeof(diag_service_head_stru)))
        {
            osl_sem_up(&g_stDiagSrvc.ListSem);
            diag_error("there is a uniform packet in list.\n");
            return ;
        }
    }
    osl_sem_up(&g_stDiagSrvc.ListSem);

    /* ?????????????????? */
    pTempNode = (DIAG_SRVC_FRAME_NODE_STRU *)osl_malloc(sizeof(DIAG_SRVC_FRAME_NODE_STRU));
    if(NULL == pTempNode)
    {
        diag_error("malloc pTempNode fail\n");
        return ;
    }

    pTempNode->ulFrameDataLen = pFrame->u32MsgLen + sizeof(diag_frame_head_stru);
    pTempNode->ulFrameOffset = 0;
    pTempNode->pFrame = (diag_frame_head_stru*)osl_malloc(pTempNode->ulFrameDataLen);
    if(NULL == pTempNode->pFrame)
    {
        diag_error("malloc pFrame fail\n");
        osl_free(pTempNode);
        return ;
    }

    osl_sem_down(&g_stDiagSrvc.ListSem);

    memcpy_s(&pTempNode->stHead, (u32)sizeof(pTempNode->stHead), &pFrame->stService, sizeof(pFrame->stService));

    list_add_tail(&pTempNode->FrameList, &g_stDiagSrvc.ListHeader);

    pTempNode->ulSlice = mdrv_timer_get_normal_timestamp();

    osl_sem_up(&g_stDiagSrvc.ListSem);

    return ;

}
/*lint -restore*/

/*****************************************************************************
 Function Name   : diag_SrvcSavePkt
 Description     : ????????????????????????????????

 History         :
    1.c64416         2015-03-18  Draft Enact

*****************************************************************************/
diag_frame_head_stru * diag_SrvcSavePkt(diag_frame_head_stru *pFrame, u32 ulDataLen)
{
    struct list_head* me = NULL;
    DIAG_SRVC_FRAME_NODE_STRU *pTempNode;
    u32 ulLen = 0;
    u32 uloffset = 0;
    u32 ulLocalLen = 0;

    osl_sem_down(&g_stDiagSrvc.ListSem);

    list_for_each(me, &g_stDiagSrvc.ListHeader)
    {
        pTempNode = list_entry(me, DIAG_SRVC_FRAME_NODE_STRU, FrameList);
        if(NULL == pTempNode)
        {
            /* coverity[dead_error_begin] */
            osl_sem_up(&g_stDiagSrvc.ListSem);
            return NULL;
        }

        /* ????????stService??4G ??5G?????? */
        pTempNode->stHead.index4b = pFrame->stService.index4b;
        pTempNode->stHead.eof1b   = pFrame->stService.eof1b;
        pTempNode->stHead.ff1b    = pFrame->stService.ff1b;
        if(0 == memcmp(&pTempNode->stHead, &pFrame->stService, sizeof(diag_service_head_stru)))
        {
            if(0 == pFrame->stService.index4b)  /* ??0?? */
            {
                /* ??0??????????header, cmdid, meglen and data */
                if(memcpy_s(pTempNode->pFrame, pTempNode->ulFrameDataLen, pFrame, ulDataLen))
                {
                    diag_error("memcpy fail\n");
                }

                pTempNode->ulFrameOffset = ulDataLen;
                diag_SaveDFR(&g_stDFRreq, (u8*)pFrame, ulDataLen);
            }
            else if(pFrame->stService.eof1b)  /* ????1?? */
            {
                /* ?????????????????????????????? */
                ulLen = pTempNode->ulFrameOffset - sizeof(diag_frame_head_stru);
                ulLocalLen = ulDataLen - sizeof(diag_service_head_stru);

                if(     (NULL == pTempNode->pFrame)
                    ||  (pTempNode->ulFrameOffset + ulLocalLen > DIAG_FRAME_SUM_LEN)
                    ||  (pTempNode->pFrame->u32MsgLen != ulLen + ulLocalLen))
                {
                    diag_error("rev data len error, ulLen:0x%x ulLocalLen:0x%x\n", ulLen, ulLocalLen);
                    osl_sem_up(&g_stDiagSrvc.ListSem);
                    return NULL;
                }

                /* ???????????????? */
                ulLen = pTempNode->pFrame->u32MsgLen - ulLen;

                /* ???????????????? */
                uloffset = pTempNode->ulFrameOffset;

                /* ??????????????????????data */
                if(memcpy_s( ((u8*)pTempNode->pFrame) + uloffset, pTempNode->ulFrameDataLen - pTempNode->ulFrameOffset,
                            ((u8*)pFrame) + sizeof(diag_service_head_stru),  ulLen))
                {
                    diag_error("memcpy fail\n");
                }

                pTempNode->ulFrameOffset += ulLen;
                diag_SaveDFR(&g_stDFRreq, (u8*)pFrame, ulDataLen);
            }
            else
            {
                /* ???????????????? */
                uloffset = pTempNode->ulFrameOffset;
                ulLocalLen = ulDataLen - sizeof(diag_service_head_stru);

                if(     (NULL == pTempNode->pFrame)
                    ||  (uloffset + ulLocalLen > DIAG_FRAME_SUM_LEN)
                    ||  (pTempNode->pFrame->u32MsgLen < (uloffset - sizeof(diag_frame_head_stru) + ulLocalLen)))
                {
                    diag_error("msg len error, uloffset:0x%x ulLocallen:0x%x\n", uloffset, ulLocalLen);
                    osl_sem_up(&g_stDiagSrvc.ListSem);
                    return NULL;
                }

                /* ??????????????cmdid??????????????????data */
                if(memcpy_s( ((u8*)pTempNode->pFrame) + uloffset,
                            (u32)(pTempNode->ulFrameDataLen - uloffset),
                            ((u8*)pFrame) + sizeof(diag_service_head_stru),
                            ulLocalLen))
                 {
                      diag_error("memcpy fail\n");
                 }

                pTempNode->ulFrameOffset += ulLocalLen;
                diag_SaveDFR(&g_stDFRreq, (u8*)pFrame, ulDataLen);
            }

            osl_sem_up(&g_stDiagSrvc.ListSem);
            return pTempNode->pFrame;
        }
    }
    osl_sem_up(&g_stDiagSrvc.ListSem);

    return (diag_frame_head_stru *)NULL;
}


/*****************************************************************************
 Function Name   : diag_SrvcDestroyPkt
 Description     : ??????????????

 History         :
    1.c64416         2015-03-18  Draft Enact

*****************************************************************************/
void diag_SrvcDestroyPkt(diag_frame_head_stru *pFrame)
{
    struct list_head* me = NULL;
    DIAG_SRVC_FRAME_NODE_STRU *pTempNode;

    osl_sem_down(&g_stDiagSrvc.ListSem);

    list_for_each(me, &g_stDiagSrvc.ListHeader)
    {
        pTempNode = list_entry(me, DIAG_SRVC_FRAME_NODE_STRU, FrameList);

        if(NULL == pTempNode)
        {
            osl_sem_up(&g_stDiagSrvc.ListSem);
            return ;
        }

        /* ????????stService??4G ??5G?????? */
        pTempNode->stHead.index4b = pFrame->stService.index4b;
        pTempNode->stHead.eof1b   = pFrame->stService.eof1b;
        pTempNode->stHead.ff1b    = pFrame->stService.ff1b;
        if(0 == memcmp(&pTempNode->stHead, &pFrame->stService, sizeof(diag_service_head_stru)))
        {
            /*????????*/
            if((NULL != pTempNode->FrameList.next) && (NULL != pTempNode->FrameList.prev))
            {
                list_del(&pTempNode->FrameList);
            }

            if(NULL != pTempNode->pFrame)
            {
                osl_free(pTempNode->pFrame);
            }

            osl_free(pTempNode);
            break;
        }
    }

    osl_sem_up(&g_stDiagSrvc.ListSem);

    return ;
}


u32 diag_GetSrvData(diag_frame_head_stru *pHeader, u32 ulDatalen, diag_frame_head_stru **pProcHead)
{
    diag_frame_head_stru *pProcHeadTemp = NULL;

    if(pHeader->stService.ff1b)
    {
        /* ???????????????????????????????? */
        diag_PktTimeoutClear();

        /* index4b????????????16, ??????????????????????16,????index??????????16 */
        if(0 == pHeader->stService.index4b)
        {
            diag_SrvcCreatePkt(pHeader);
            (void)diag_SrvcSavePkt(pHeader, ulDatalen);
            return ERR_MSP_SUCCESS;
        }
        else if(pHeader->stService.eof1b)
        {
            pProcHeadTemp = diag_SrvcSavePkt(pHeader, ulDatalen);
            if(pProcHeadTemp == NULL)
            {
                return ERR_MSP_FAILURE;
            }
            /* 5G??????????????????????5G????,4G????????????????4G?????? */
            ulDatalen = pProcHeadTemp->u32MsgLen + sizeof(diag_frame_head_stru);

            *pProcHead = pProcHeadTemp;
            return SERVICE_MSG_PROC;
        }
        else
        {
            (void)diag_SrvcSavePkt(pHeader, ulDatalen);
            return ERR_MSP_SUCCESS;
        }
    }
    else
    {
        if(ulDatalen < pHeader->u32MsgLen + sizeof(diag_frame_head_stru))
        {
            diag_error("rev tools data len error, rev:0x%x except:0x%x\n", \
                ulDatalen, pHeader->u32MsgLen + (u32)sizeof(diag_frame_head_stru));
            return ERR_MSP_INVALID_PARAMETER;
        }
        diag_SaveDFR(&g_stDFRreq, (u8*)pHeader, (sizeof(diag_frame_head_stru) + pHeader->u32MsgLen));

        *pProcHead = pHeader;

        return SERVICE_MSG_PROC;
    }
}
/*****************************************************************************
 Function Name   : diag_ServiceProc
 Description     : DIAG service ????????

 History         :
    1.c64416         2014-11-18  Draft Enact

*****************************************************************************/
u32 diag_ServiceProc(void *pData, u32 ulDatalen)
{
    u32 ulRet = BSP_ERROR;
    diag_frame_head_stru *pHeader;
    diag_frame_head_stru *pProcHead = NULL;

    if((NULL == pData)||(ulDatalen < sizeof(diag_service_head_stru)))
    {
        diag_error("para error:ulDatalen=0x%x\n", ulDatalen);
        return BSP_ERROR;
    }

    pHeader = (diag_frame_head_stru*)pData;

    diag_PTR(EN_DIAG_PTR_SERVICE_IN, 1, pHeader->u32CmdId, 0);

    /* ??????DIAG???? */
    if(DIAG_FRAME_MSP_SID_DIAG_SERVICE == SERVICE_HEAD_SID(pData))
    {
        diag_PTR(EN_DIAG_PTR_SERVICE_1, 1, pHeader->u32CmdId, 0 );

        /* ???????????????????? */
        __pm_stay_awake(&g_SrvCtrl.stWakelock);

        ulRet = diag_GetSrvData(pHeader, ulDatalen, &pProcHead);
        if(ulRet != SERVICE_MSG_PROC)
        {
            __pm_relax(&g_SrvCtrl.stWakelock);
            return ulRet;
        }

        if(g_fnService && pProcHead)
        {
            /* ??????????N??cmdid */
            diag_DrvLNR(EN_DIAG_DRV_LNR_CMDID, pHeader->u32CmdId, bsp_get_slice_value());
            //diag_DumpDFInfo(pProcHead);
            ulRet = g_fnService((void *)pProcHead);
        }
        else
        {
            ulRet = ERR_MSP_NO_INITILIZATION;
            diag_PTR(EN_DIAG_PTR_DIAG_SERVICE_ERR1, 1, pHeader->u32CmdId, 0);
        }

        if(pHeader->stService.ff1b)
        {
            diag_SrvcDestroyPkt(pHeader);
        }

        /* ?????????????????? */
        __pm_relax(&g_SrvCtrl.stWakelock);
    }
    else
    {
        diag_PTR(EN_DIAG_PTR_DIAG_SERVICE_ERR2, 1, pHeader->u32CmdId, 0);
        ulRet = ERR_MSP_INVALID_PARAMETER;
    }

    return ulRet;
}


/*****************************************************************************
 Function Name   : diag_ServiceProcReg
 Description     : DIAG service????????????

 History         :
    1.c64416         2014-11-18  Draft Enact

*****************************************************************************/
void diag_ServiceProcReg(DRV_DIAG_SERVICE_FUNC pServiceFn)
{
    g_fnService = pServiceFn;
}

void diag_ServiceInit(void)
{
    u32 ret;

    spin_lock_init(&g_stSrvIndSrcBuffSpinLock);
    spin_lock_init(&g_stSrvCnfSrcBuffSpinLock);

    wakeup_source_init(&g_SrvCtrl.stWakelock, "diag_srv_lock");

    /* ??????????????????*/
    osl_sem_init(1, &g_stDiagSrvc.ListSem);

    /* ?????????????? */
    INIT_LIST_HEAD(&g_stDiagSrvc.ListHeader);

    msp_ServiceProcReg(DIAG_FRAME_MSP_SID_DIAG_SERVICE, diag_ServiceProc);

    /* coverity[alloc_arg] */
    ret = diag_CreateDFR("DFRReqA", DIAG_DFR_BUFFER_MAX, &g_stDFRreq);
    if(ret)
    {
        diag_error("diag_CreateDFR DFR_Req failed.\n");
    }

    /* coverity[alloc_arg] */
    ret = diag_CreateDFR("DFRCnfA", DIAG_DFR_BUFFER_MAX, &g_stDFRcnf);

    if(ret)
    {
        diag_error("diag_CreateDFR DFR_Cnf failed.\n");
    }
}

/*****************************************************************************
* ?? ?? ??  :iqi_data_buffer_write
* ????????  : ????????????????????
* ????????  : id????????????????
* ????????  : ??
* ?? ?? ??  : ????????????
*****************************************************************************/
void diag_SrvcPackWrite(SOCP_BUFFER_RW_STRU *pRWBuffer, const void * pPayload, u32 u32DataLen )
{
    u32 ulTempLen;
    u32 ulTempLen1;

    if(0 == u32DataLen)
    {
        return;
    }

    if(pRWBuffer->u32Size > u32DataLen)
    {
        if(memcpy_s(((u8*)pRWBuffer->pBuffer), pRWBuffer->u32Size, pPayload, u32DataLen))
        {
            diag_error("memcpy fail\n");
        }

        pRWBuffer->pBuffer = pRWBuffer->pBuffer + u32DataLen;
        pRWBuffer->u32Size = pRWBuffer->u32Size - u32DataLen;
    }
    else
    {
        ulTempLen = pRWBuffer->u32Size;
        if(memcpy_s(((u8*)pRWBuffer->pBuffer), pRWBuffer->u32Size, pPayload,ulTempLen))
        {
            diag_error("memcpy fail\n");
        }

        ulTempLen1 = u32DataLen - pRWBuffer->u32Size;
        if(ulTempLen1)
        {
            if(memcpy_s(pRWBuffer->pRbBuffer, pRWBuffer->u32RbSize, ((u8 *)pPayload + ulTempLen) ,ulTempLen1))
            {
                diag_error("memcpy fail\n");
            }

        }
        pRWBuffer->pBuffer = pRWBuffer->pRbBuffer + ulTempLen1;
        pRWBuffer->u32Size = pRWBuffer->u32RbSize - ulTempLen1;
        pRWBuffer->pRbBuffer = NULL;
        pRWBuffer->u32RbSize = 0;
    }

    return;
}


/*****************************************************************************
 Function Name   : diag_SrvcPackFirst
 Description     : ??????????????????????????????????????

 History         :
    1.c64416         2015-03-12  Draft Enact

*****************************************************************************/
u32 diag_SrvcPackIndSend(DIAG_MSG_REPORT_HEAD_STRU *pData)
{
    SOCP_BUFFER_RW_STRU stSocpBuf = {NULL, NULL, 0, 0};
    DIAG_SRV_SOCP_HEADER_STRU *pstSocpHeader;
    u32 ulTmpLen =0;
    u32 ret = ERR_MSP_FAILURE;
    unsigned long  ulLockLevel;

    ulTmpLen = ALIGN_DDR_WITH_8BYTE(pData->ulHeaderSize + pData->ulDataSize);

    pstSocpHeader = &(((DIAG_SRV_HEADER_STRU *)(pData->pHeaderData))->socp_header);
    pstSocpHeader->ulDataLen = pData->ulHeaderSize + pData->ulDataSize - sizeof(DIAG_SRV_SOCP_HEADER_STRU);

    spin_lock_irqsave(&g_stSrvIndSrcBuffSpinLock, ulLockLevel);

    if(mdrv_socp_get_write_buff(g_SrvCtrl.ulIndChannelID, &stSocpBuf))
    {
        /* no print because it is a noraml way */
        spin_unlock_irqrestore(&g_stSrvIndSrcBuffSpinLock, ulLockLevel);
        return ERR_MSP_GET_WRITE_BUFF_FAIL;/* ???????? */
    }

    diag_DebugOverFlowRecord(stSocpBuf.u32Size + stSocpBuf.u32RbSize);

    /* ???????????? */
    if((stSocpBuf.u32Size + stSocpBuf.u32RbSize) >= (ulTmpLen))
    {
        stSocpBuf.pBuffer = (char *)scm_ind_src_phy_to_virt((u8*)stSocpBuf.pBuffer);
        stSocpBuf.pRbBuffer = (char *)scm_ind_src_phy_to_virt((u8*)stSocpBuf.pRbBuffer);
    }
    else
    {
        spin_unlock_irqrestore(&g_stSrvIndSrcBuffSpinLock, ulLockLevel);
        diag_debug_ind_src_lost(ulTmpLen);
        return ERR_MSP_NOT_FREEE_SPACE;
    }

    if(!(stSocpBuf.pBuffer) && (!stSocpBuf.pRbBuffer))
    {
        spin_unlock_irqrestore(&g_stSrvIndSrcBuffSpinLock, ulLockLevel);
        diag_error("get virt fail\n");
        return ERR_MSP_GET_VIRT_ADDR_FAIL;
    }

    diag_SrvcPackWrite(&stSocpBuf, pData->pHeaderData, pData->ulHeaderSize);
    diag_SrvcPackWrite(&stSocpBuf, pData->pData, pData->ulDataSize);

    ret = (u32)mdrv_socp_write_done(g_SrvCtrl.ulIndChannelID, (ulTmpLen));
    spin_unlock_irqrestore(&g_stSrvIndSrcBuffSpinLock, ulLockLevel);
    if(ret)
    {
        diag_error("write done fail,ret:0x%x\n", ret);
        return ERR_MSP_WRITE_DONE_FAIL;
    }

    return ERR_MSP_SUCCESS;
}


/*****************************************************************************
 Function Name   : diag_ServicePackData
 Description     : DIAG service??????????????????

 History         :
    1.c64416         2014-11-18  Draft Enact
    2.c64416         2015-03-14  ????????????????
                    ????????????????????????????????:
                    A. ????????ulCmdId??ulMsgLen??????????????????
                    B. ??????????????????????????????????????????????
                    C. transid??timestamp??????????????????????

*****************************************************************************/
u32 diag_ServicePackData(DIAG_MSG_REPORT_HEAD_STRU *pData)
{
    DIAG_MSG_REPORT_HEAD_STRU stReportInfo;
    diag_frame_head_stru      *pstFrameHeader;
    u32 ret = ERR_MSP_FAILURE;
    s32 lDataLen = 0;         /* ?????????? */
    u32 ulCurlen = 0;        /* ???????????????????? */
    u32 ulIndex = 0;
    u32 ulFrameHeaderSize = 0;
    u32 ulSendLen = 0;

    ulFrameHeaderSize = pData->ulHeaderSize - sizeof(DIAG_SRV_SOCP_HEADER_STRU);

    /* ???????????????????? */
    lDataLen = (s32)(ulFrameHeaderSize + pData->ulDataSize);
    if(lDataLen > (s32)(DIAG_FRAME_SUM_LEN - 15*sizeof(diag_frame_head_stru)))
    {
        diag_error("diag report length is %d.\n", lDataLen);
        return ERR_MSP_FAILURE;
    }

    diag_DebugPackageRecord(lDataLen);

    pstFrameHeader = &(((DIAG_SRV_HEADER_STRU *)(pData->pHeaderData))->frame_header);

    ulCurlen = (u32)lDataLen;
    if(lDataLen > DIAG_FRAME_MAX_LEN)
    {
        ulCurlen = DIAG_FRAME_MAX_LEN;
        pstFrameHeader->stService.ff1b = 1;
    }
    else
    {
        pstFrameHeader->stService.ff1b = 0;
    }

    stReportInfo.pHeaderData    = pData->pHeaderData;
    stReportInfo.ulHeaderSize   = pData->ulHeaderSize;
    stReportInfo.pData          = pData->pData;
    stReportInfo.ulDataSize     = ulCurlen  - ulFrameHeaderSize;
    ulSendLen += stReportInfo.ulDataSize;

    /* ????????????????????cmdid???????????????????????????????????????? */
    ret = diag_SrvcPackIndSend(&stReportInfo);
    if(ret)
    {
        return ERR_MSP_FAILURE;
    }

    /* ???????? */
    if(pstFrameHeader->stService.ff1b)
    {
        /* ?????????????????????????? */
        lDataLen = lDataLen - (s32)ulCurlen;

        while(lDataLen > 0)
        {
            pstFrameHeader->stService.index4b = ++ulIndex;
            if( (lDataLen + sizeof(diag_service_head_stru)) > DIAG_FRAME_MAX_LEN )
            {
                ulCurlen = DIAG_FRAME_MAX_LEN - sizeof(diag_service_head_stru);
            }
            else
            {
                ulCurlen = (u32)lDataLen;
                pstFrameHeader->stService.eof1b = 1;    /* ???????????????? */
            }
            stReportInfo.pHeaderData    = pData->pHeaderData;
            stReportInfo.ulHeaderSize   = sizeof(DIAG_SRV_SOCP_HEADER_STRU) + sizeof(diag_service_head_stru);
            stReportInfo.pData          = (u8*)(pData->pData) + ulSendLen;
            stReportInfo.ulDataSize     = ulCurlen;
            ulSendLen += stReportInfo.ulDataSize;

            ret = diag_SrvcPackIndSend(&stReportInfo);
            if(ret)
            {
                return ERR_MSP_FAILURE;
            }

            lDataLen -= (s32)ulCurlen;
        }
    }

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 Function Name   : diag_SrvcPackFirst
 Description     : ??????????????????????????????????????

 History         :
    1.c64416         2015-03-12  Draft Enact

*****************************************************************************/
u32 diag_SrvcPackCnfSend(DIAG_MSG_REPORT_HEAD_STRU *pData)
{
    SOCP_BUFFER_RW_STRU         stSocpBuf = {NULL, NULL, 0, 0};
    DIAG_SRV_SOCP_HEADER_STRU * pstSocpHeader;
    u32 ulTmpLen =0;
    u32 ret      = ERR_MSP_FAILURE;
    unsigned long  ulLockLevel;

    ulTmpLen = ALIGN_DDR_WITH_8BYTE((pData->ulHeaderSize + pData->ulDataSize));

    pstSocpHeader = &(((DIAG_SRV_HEADER_STRU *)(pData->pHeaderData))->socp_header);
    pstSocpHeader->ulDataLen = pData->ulHeaderSize + pData->ulDataSize - sizeof(DIAG_SRV_SOCP_HEADER_STRU);

    spin_lock_irqsave(&g_stSrvCnfSrcBuffSpinLock, ulLockLevel);

    if(mdrv_socp_get_write_buff(g_SrvCtrl.ulCnfChannelID, &stSocpBuf))
    {
        spin_unlock_irqrestore(&g_stSrvCnfSrcBuffSpinLock, ulLockLevel);
        return ERR_MSP_GET_WRITE_BUFF_FAIL;/* ???????? */
    }
    /* ???????????? */
    if((stSocpBuf.u32Size + stSocpBuf.u32RbSize) >= (ulTmpLen))
    {
        stSocpBuf.pBuffer = (char *)scm_cnf_src_phy_to_virt((u8*)stSocpBuf.pBuffer);
        stSocpBuf.pRbBuffer = (char *)scm_cnf_src_phy_to_virt((u8*)stSocpBuf.pRbBuffer);
    }
    else
    {
        spin_unlock_irqrestore(&g_stSrvCnfSrcBuffSpinLock, ulLockLevel);
        return ERR_MSP_NOT_FREEE_SPACE;
    }

    if(!(stSocpBuf.pBuffer) && (!stSocpBuf.pRbBuffer))
    {
        spin_unlock_irqrestore(&g_stSrvCnfSrcBuffSpinLock, ulLockLevel);
        diag_error("get virt fail\n");
        return ERR_MSP_GET_VIRT_ADDR_FAIL;
    }

    diag_SrvcPackWrite(&stSocpBuf, pData->pHeaderData, pData->ulHeaderSize);
    diag_SrvcPackWrite(&stSocpBuf, pData->pData, pData->ulDataSize);

    ret = (u32)mdrv_socp_write_done(g_SrvCtrl.ulCnfChannelID, (ulTmpLen));
    spin_unlock_irqrestore(&g_stSrvCnfSrcBuffSpinLock, ulLockLevel);
    if(ret)
    {
        diag_error("write done fail,ret:0x%x\n", ret);
        return ERR_MSP_WRITE_DONE_FAIL;
    }

    if(pData->pData)
    {
        diag_SaveDFR(&g_stDFRcnf, (u8 *)pData->pHeaderData, (u32)(pData->ulHeaderSize  - sizeof(DIAG_SRV_SOCP_HEADER_STRU)));
    }

    return ERR_MSP_SUCCESS;
}


/*****************************************************************************
 Function Name   : diag_ServicePackData
 Description     : DIAG service??????????????????

 History         :
    1.c64416         2014-11-18  Draft Enact
    2.c64416         2015-03-14  ????????????????
                    ????????????????????????????????:
                    A. ????????ulCmdId??ulMsgLen??????????????????
                    B. ??????????????????????????????????????????????
                    C. transid??timestamp??????????????????????

*****************************************************************************/
u32 diag_ServicePackCnfData(DIAG_MSG_REPORT_HEAD_STRU *pData)
{
    DIAG_MSG_REPORT_HEAD_STRU stReportInfo;
    diag_frame_head_stru      *pstFrameHeader;
    u32 ret = ERR_MSP_FAILURE;
    s32  lDataLen = 0;         /* ?????????? */
    u32 ulCurlen = 0;        /* ???????????????????? */
    u32 ulSplit = 0;
    u32 ulIndex = 0;
    u32 ulFrameHeaderSize = 0;
    u32 ulSendLen = 0;

    ulFrameHeaderSize = pData->ulHeaderSize - sizeof(DIAG_SRV_SOCP_HEADER_STRU);
    pstFrameHeader = &(((DIAG_SRV_HEADER_STRU *)(pData->pHeaderData))->frame_header);

    diag_PTR(EN_DIAG_PTR_SERVICE_PACKETDATA, 1, pstFrameHeader->u32CmdId, 0);

    /* ???????????????????? */
    lDataLen = (s32)(ulFrameHeaderSize + pData->ulDataSize);
    if(lDataLen > (s32)(DIAG_FRAME_SUM_LEN - 15*sizeof(diag_frame_head_stru)))
    {
        diag_error("diag report length is %d.\n", lDataLen);
        return ERR_MSP_FAILURE;
    }

    ulCurlen = (u32)lDataLen;
    if(lDataLen > DIAG_FRAME_MAX_LEN)
    {
        ulCurlen = DIAG_FRAME_MAX_LEN;
        ulSplit = 1;
    }
    else
    {
        ulSplit = 0;
    }

    /* ?????????? */
    pstFrameHeader = &(((DIAG_SRV_HEADER_STRU *)(pData->pHeaderData))->frame_header);
    pstFrameHeader->stService.mt2b = DIAG_FRAME_MT_CNF;
    pstFrameHeader->stService.ff1b = ulSplit;

    stReportInfo.pHeaderData    = pData->pHeaderData;
    stReportInfo.ulHeaderSize   = pData->ulHeaderSize;
    stReportInfo.pData          = pData->pData;
    stReportInfo.ulDataSize     = ulCurlen  - ulFrameHeaderSize;
    ulSendLen += stReportInfo.ulDataSize;

    /* ????????????????????cmdid???????????????????????????????????????? */
    ret = diag_SrvcPackCnfSend(&stReportInfo);
    if(ret)
    {
        return ERR_MSP_FAILURE;
    }

    /* ???????? */
    if(ulSplit)
    {
        /* ?????????????????????????? */
        lDataLen = lDataLen - (s32)ulCurlen;

        while(lDataLen > 0)
        {
            pstFrameHeader->stService.index4b = ++ulIndex;
            if( (lDataLen + sizeof(diag_service_head_stru)) > DIAG_FRAME_MAX_LEN )
            {
                ulCurlen = DIAG_FRAME_MAX_LEN - sizeof(diag_service_head_stru);
            }
            else
            {
                ulCurlen = (u32)lDataLen;
                pstFrameHeader->stService.eof1b = 1;    /* ???????????????? */
            }
            stReportInfo.pHeaderData    = pData->pHeaderData;
            stReportInfo.ulHeaderSize   = sizeof(DIAG_SRV_SOCP_HEADER_STRU) + sizeof(diag_service_head_stru);
            stReportInfo.pData          = (u8*)(pData->pData) + ulSendLen;
            stReportInfo.ulDataSize     = ulCurlen;
            ulSendLen += stReportInfo.ulDataSize;

            ret = diag_SrvcPackCnfSend(&stReportInfo);
            if(ret)
            {
                return ERR_MSP_FAILURE;
            }

            lDataLen -= (s32)ulCurlen;
        }
    }

    return ERR_MSP_SUCCESS;
}


u32 diag_ServicePacketResetData(DIAG_MSG_REPORT_HEAD_STRU *pData)
{
    DIAG_MSG_REPORT_HEAD_STRU   stReportInfo;
    diag_frame_head_stru*       pstFrameHeader;
    u32 ret = ERR_MSP_FAILURE;
    s32 lDataLen = 0;         /* ?????????? */
    u32 ulCurlen = 0;        /* ???????????????????? */

    pstFrameHeader = &(((DIAG_SRV_HEADER_STRU *)(pData->pHeaderData))->frame_header);

    /* ???????????????????? */
    lDataLen = (s32)(pData->ulHeaderSize + pData->ulDataSize);
    if(lDataLen - sizeof(DIAG_SRV_SOCP_HEADER_STRU) > (s32)(DIAG_FRAME_SUM_LEN - 15*sizeof(diag_frame_head_stru)))
    {
        diag_error("report length error(0x%x).\n", lDataLen);
        return ERR_MSP_FAILURE;
    }

    ulCurlen = (u32)lDataLen;
    if(lDataLen > DIAG_FRAME_MAX_LEN)
    {
        return ERR_MSP_INALID_LEN_ERROR;
    }

    /* ?????????? */
    pstFrameHeader = &(((DIAG_SRV_HEADER_STRU *)(pData->pHeaderData))->frame_header);
    pstFrameHeader->stService.mt2b = DIAG_FRAME_MT_IND;
    pstFrameHeader->stService.ff1b = 0;
    pstFrameHeader->u32MsgLen = (u32)lDataLen - sizeof(DIAG_SRV_HEADER_STRU);

    stReportInfo.pHeaderData    = pData->pHeaderData;
    stReportInfo.ulHeaderSize   = pData->ulHeaderSize;
    stReportInfo.pData          = pData->pData;
    stReportInfo.ulDataSize     = ulCurlen  - pData->ulHeaderSize;

    /* ????????????????????cmdid???????????????????????????????????????? */
    ret = diag_SrvcPackCnfSend(&stReportInfo);
    if(ret)
    {
        return ERR_MSP_FAILURE;
    }

    return ERR_MSP_SUCCESS;
}

void DIAG_DebugDFR(void)
{
    diag_error("%d, %s, %d.\n", g_stDFRreq.ulMagicNum, g_stDFRreq.name, g_stDFRreq.ulLen);
    diag_GetDFR(&g_stDFRreq);
    diag_error("%d, %s, %d.\n", g_stDFRcnf.ulMagicNum, g_stDFRcnf.name, g_stDFRcnf.ulLen);
    diag_GetDFR(&g_stDFRcnf);
}




