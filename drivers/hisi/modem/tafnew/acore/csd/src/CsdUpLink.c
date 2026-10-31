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

/*****************************************************************************
  1 ??????????
*****************************************************************************/
#include "CsdUpLink.h"
#include "CsdDebug.h"
#include "PsLib.h"

/*****************************************************************************
  1 ??????????????
*****************************************************************************/





/*****************************************************************************
    ??????????????????????.C??????????
*****************************************************************************/
#define    THIS_FILE_ID                 PS_FILE_ID_CSD_UP_LINK_C

#if(FEATURE_ON == FEATURE_CSD)

/*****************************************************************************
  2 ????????????
*****************************************************************************/

/*****************************************************************************
  3 ????????
*****************************************************************************/


VOS_VOID CSD_UL_SendData(IMM_ZC_STRU *pstULData)
{
    /*????????*/
    if (VOS_NULL_PTR == pstULData)
    {
        CSD_ERROR_LOG(ACPU_PID_CSD, "CSD_UL_SendData:: pstULData Is Null");

        return;
    }

    /*??????????????????*/
    if (VOS_NULL_PTR == pstULData->data)
    {
        CSD_ERROR_LOG(ACPU_PID_CSD, "CSD_UL_SendData:: pstULData->data Is Null");

        return;
    }

    /*????????????*/
    if (0 == pstULData->len)
    {
        CSD_ERROR_LOG(ACPU_PID_CSD, "CSD_UL_SendData:: pstULData->len Is Null");

        return;
    }

    CSD_DBG_UL_RECV_PKT_NUM(1);

    /*????????*/
    CSD_UL_InsertQueue(pstULData);

}

VOS_UINT32 CSD_UL_InsertQueue(IMM_ZC_STRU *pstULData)
{

    IMM_ZC_HEAD_STRU                   *pstULQueue;
    VOS_UINT32                          ulInQueueRst;
    VOS_SEM                             hULDataSem = VOS_NULL_PTR;

    hULDataSem       = CSD_GetUpLinkDataSem();

    pstULQueue       = CSD_UL_GetQueue();

    /*??????????????????????????????????????*/
    if (VOS_NULL_PTR == pstULQueue)
    {
        IMM_ZcFree(pstULData);
        CSD_DBG_UL_ENQUE_FAIL_NUM(1);
        CSD_ERROR_LOG(ACPU_PID_CSD, "CSD_UL_InsertQueue:: ULQueue Is Null");

        return VOS_ERR;
    }

    /*??????????????????????????????????????????????????????????????????????????
    ????*/
    VOS_TaskLock();

    ulInQueueRst     = CSD_UL_InsertQueueTail(pstULQueue,pstULData);

    VOS_TaskUnlock();

    if (VOS_ERR == ulInQueueRst)
    {
        IMM_ZcFree(pstULData);
        CSD_ERROR_LOG(ACPU_PID_CSD, "CSD_UL_InsertQueue:: ULQueue Is Null");

        return VOS_ERR;
    }

    /*????????????????????????????*/
    VOS_SmV(hULDataSem);

    CSD_DBG_UL_SAVE_BUFF_PKT_NUM(1);

    CSD_NORMAL_LOG(ACPU_PID_CSD,"CSD_UL_InsertQueue::Insert ULQueue Done!");

    return VOS_OK;

}


VOS_UINT32 CSD_UL_InitQueue(VOS_VOID)
{

    IMM_ZC_HEAD_STRU                    *pstULQueue;

    pstULQueue     = CSD_UL_GetQueue();

    /*??????????????????????????????????*/
    if (VOS_NULL_PTR ==  pstULQueue)
    {
        CSD_WARNING_LOG(ACPU_PID_CSD,
                        "CSD_UL_InitQueue:: ULQueue Is Null Need Alloc Mem !");

        /*??????????????*/
        pstULQueue = (IMM_ZC_HEAD_STRU *)PS_MEM_ALLOC(ACPU_PID_CSD, sizeof(IMM_ZC_HEAD_STRU));

        CSD_UL_SetQueue(pstULQueue);

        /*????????????????????????????????????????????????????????*/
        if (VOS_NULL_PTR == pstULQueue)
        {
            CSD_ERROR_LOG(ACPU_PID_CSD, "CSD_UL_InitQueue:: PS_MEM_ALLOC Fail");
            return VOS_ERR;
        }

        /*??????????*/
        IMM_ZcQueueHeadInit(pstULQueue);

    }

    CSD_NORMAL_LOG3(ACPU_PID_CSD,
                   "CSD_UL_InitQueue:: Queue len is , Pre is , Next is ",
                   pstULQueue->qlen,
                   pstULQueue->next,
                   pstULQueue->prev);

    return VOS_OK;
}

VOS_UINT32 CSD_UL_BuildDiccInsertData(
    IMM_ZC_HEAD_STRU                   *pstUlQueue,
    CSD_CST_DATA_REQ_STRU              *pstULData,
    DICC_INSERT_DSCP_BLK_STRU          *pstDiccInsertData
)
{
    IMM_ZC_STRU                        *pstULImmZcData;
    IMM_MEM_STRU                       *pstULImmData;

    /*??????????????*/
    pstULImmZcData                      = CSD_UL_GetQueueFrontNode(pstUlQueue);

    if (VOS_NULL_PTR == pstULImmZcData)
    {
        CSD_DBG_UL_QUENULL_NUM(1);
        CSD_ERROR_LOG(ACPU_PID_CSD, "CSD_UL_ConfigDataToDICC pstULNode Is Null!");

        return VOS_ERR;
    }

    /*IMM_ZcMapToImmMem????IMM??*/
    pstULImmData                        = IMM_ZcMapToImmMem(pstULImmZcData);

    if (VOS_NULL_PTR == pstULImmData)
    {
        /*????????????*/
        IMM_ZcFree(pstULImmZcData);

        CSD_DBG_UL_ZCTOIMM_FAIL_NUM(1);

        CSD_ERROR_LOG(ACPU_PID_CSD, "CSD_UL_ConfigDataToDICC pstULImmData Is Null!");

        return VOS_ERR;
    }

    /*??????????????????*/
    pstULData->pucData                  = (VOS_UINT8 *)TTF_VIRT_TO_PHY((VOS_VOID *)(pstULImmZcData->data));

    pstULData->pGarbage                 = (VOS_VOID *)pstULImmData;
    pstULData->usLen                    = (VOS_UINT16)pstULImmZcData->len;

    /*????????????????????*/
    pstDiccInsertData->enAutoTrigTx     = PS_FALSE;
    pstDiccInsertData->ucDataLen        = DICC_INFO_BLOCK_MAX_LEN;
    pstDiccInsertData->pucUserData      = (VOS_UINT8 *)(pstULData);

    /*????????????*/
    IMM_ZcHeadFree(pstULImmZcData);

    CSD_NORMAL_LOG(ACPU_PID_CSD,"CSD_UL_ConfigDataToDICC  Done!");

    return VOS_OK;
}

CSD_UL_SEND_DATA_STATE_ENUM_UINT16 CSD_UL_SendDataToDICC(VOS_VOID)
{
    VOS_UINT32                          ulCnt;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulFreeSpaceCnt;
    VOS_UINT32                          ulLinkCnt;
    IMM_ZC_HEAD_STRU                   *pstULQueue;
    CSD_CST_DATA_REQ_STRU               stULData;
    DICC_INSERT_DSCP_BLK_STRU           stDiccInsertData;
    IMM_MEM_STRU                       *pstULImmData;

    /*????????????*/
    pstULQueue      = CSD_UL_GetQueue();

    /*????DICC????????????*/
    ulFreeSpaceCnt  = DICC_GetChannelFreeSpaceCnt(ACPU_PID_CSD,
                                                  DICC_CHAN_ID_UL_CSD_DATA_CHAN,
                                                  DICC_CPU_ID_ACPU);

    if (DICC_INVALID_VALUE == ulFreeSpaceCnt)
    {
       CSD_ERROR_LOG(ACPU_PID_CSD,
                     "CSD_UL_SendDataToDICC:: DICC_GetChannelFreeSpaceCnt Fail");

       return CSD_UL_SEND_DATA_NOT_CONTINUE;
    }

    /*????????????????*/
    VOS_TaskLock();

    /*??????????????*/
    ulLinkCnt                           = IMM_ZcQueueLen(pstULQueue);

    if (0 == ulLinkCnt )
    {
        CSD_WARNING_LOG1(ACPU_PID_CSD,
                        "CSD_UL_SendDataToDICC:: ulLinkCnt is Null",
                        ulLinkCnt);

        VOS_TaskUnlock();

        return CSD_UL_SEND_DATA_NOT_CONTINUE;
    }

    ulCnt                               = PS_MIN(ulLinkCnt, ulFreeSpaceCnt);

    /* CST??????BUFFER??????????8K??????50??????????2.5K??CST??????????????????3K */
    /* ??CST????????????????????????0.5????????????????????????????????????CSD */
    /* ??????DICC???????????????????????????????????????????????? */
    ulCnt                               = PS_MIN(ulCnt, CSD_UL_MAX_SEND_CNT);

    while (0 < ulCnt)
    {
        /*????????????*/
        ulRslt = CSD_UL_BuildDiccInsertData(pstULQueue,
                                            &stULData,
                                            &stDiccInsertData);
        if (VOS_OK != ulRslt)
        {
            CSD_ERROR_LOG(ACPU_PID_CSD,
                         "CSD_UL_SendDataToDICC:: CSD_UL_BuildDiccInsertData Fail");

            VOS_TaskUnlock();

            return CSD_UL_SEND_DATA_NOT_CONTINUE;
        }

        /*DICC???????????? */
        ulRslt = DICC_InsertChannelData(ACPU_PID_CSD,
                                        DICC_CHAN_ID_UL_CSD_DATA_CHAN,
                                        &stDiccInsertData,
                                        DICC_CPU_ID_ACPU);
        if ( DICC_OK != ulRslt )
        {
            /*????????IMM????*/
            pstULImmData                = (IMM_MEM_STRU *)stULData.pGarbage;
            IMM_MemFree(pstULImmData);

            CSD_DBG_UL_INSERT_FAIL_NUM(1);

            CSD_ERROR_LOG1(ACPU_PID_CSD,
                          "CSD_UL_SendDataToDICC:: DICC_InsertChannelData Fail",
                          ulRslt);
            break;
        }

        ulCnt--;

        CSD_NORMAL_LOG1(ACPU_PID_CSD,
                        "CSD_UL_SendDataToDICC:: ulCnt DEC",
                        ulCnt);
    }

    /*??????????????????????*/
    VOS_TaskUnlock();

    /*????????*/
    ulRslt = DICC_TriggerChanDataTx(ACPU_PID_CSD,
                                    DICC_CHAN_ID_UL_CSD_DATA_CHAN,
                                    DICC_CPU_ID_ACPU);

    /*????????SLICE*/
    CSD_SetCurrTxSlice(VOS_GetSlice());

    if ( DICC_OK != ulRslt )
    {
        CSD_ERROR_LOG1(ACPU_PID_CSD,
                      "CSD_UL_SendDataToDICC:: DICC_TriggerChanDataTx Fail",
                      ulRslt);

        return CSD_UL_SEND_DATA_NOT_CONTINUE;
    }

    if (ulLinkCnt > ulFreeSpaceCnt)
    {
        CSD_WARNING_LOG(ACPU_PID_CSD,
                        "CSD_UL_SendDataToDICC CSD_UL_SEND_DATA_CONTINUE !");

        return CSD_UL_SEND_DATA_CONTINUE;
    }

    return CSD_UL_SEND_DATA_NOT_CONTINUE;
}



VOS_UINT32 CSD_UL_CalcIsrSlice(VOS_VOID)
{
    VOS_UINT32                          ulCurrSliceCnt;
    VOS_UINT32                          ulLastSliceCnt;
    VOS_UINT32                          ulSliceDuration;

    ulLastSliceCnt     = CSD_GetLastTxSlice();
    ulCurrSliceCnt     = VOS_GetSlice();

    /* ??????????0??????????????????????????????????????????????????
       ??0xFFFFFFFF??????????0??????else???????????????? */
    if (ulCurrSliceCnt > ulLastSliceCnt)
    {
        ulSliceDuration = ulCurrSliceCnt - ulLastSliceCnt;
    }
    else
    {
        ulSliceDuration = CSD_MAX_SLICE_CNT - ulLastSliceCnt + ulCurrSliceCnt;
    }

    return ulSliceDuration;
}


VOS_VOID CSD_UL_ProcDataTask(VOS_VOID)
{
    VOS_SEM                             hUpLinkSem = VOS_NULL_PTR;
    VOS_UINT32                          ulSliceDurationCnt;
    CSD_UL_SEND_DATA_STATE_ENUM_UINT16  enSendDataRslt;

    hUpLinkSem               = CSD_GetUpLinkDataSem();

    for ( ; ; )
    {

        /* ??????????, ????????????, ???????????????? */
        if (VOS_OK != VOS_SmP(hUpLinkSem, 0))
        {
            CSD_NORMAL_LOG(ACPU_PID_CSD,
                          "CSD_UL_ProcDataTask:: VOS_SmP pulUpLinkSem then continue !");


            continue;

        }


        for ( ; ; )
        {
            /*????????????????????*/
            if (AT_CSD_CALL_STATE_ON != CSD_GetCallState())
            {
                CSD_WARNING_LOG(ACPU_PID_CSD,
                                "CSD_UL_ProcDataTask:: NOT AT_CSD_CALL_STATE_ON then Break !");

                break;
            }

            /*????????????????????*/
            ulSliceDurationCnt = CSD_UL_CalcIsrSlice();

            /*????????SLICE??????????????????????10MS, ????????, ?????????? */
            if (ulSliceDurationCnt < CSD_TEN_MILLISECOND)
            {
                CSD_WARNING_LOG(ACPU_PID_CSD,
                                "CSD_UL_ProcDataTask:: ulSliceDurationCnt < CSD_TX_DURATION then continue !");

                VOS_TaskDelay(CSD_UL_TASK_DELAY_TIME);

                continue;
            }

            /* ??DICC???????????? */
            enSendDataRslt = CSD_UL_SendDataToDICC();

            /* ????????????????????????, ???????? */
            if (CSD_UL_SEND_DATA_CONTINUE != enSendDataRslt)
            {
                CSD_NORMAL_LOG(ACPU_PID_CSD,
                               "CSD_UL_ProcDataTask:: CSD_UL_SEND_DATA_NOT_CONTINUE ");

                break;
            }
        }

    }
}

VOS_UINT32 CSD_UL_RcvAtCallStateOn(VOS_VOID)
{
    VOS_UINT32                          ulRslt;

    /*??????????????*/
    ulRslt  = CSD_UL_InitQueue();

    if (VOS_ERR == ulRslt )
    {
        CSD_ERROR_LOG(ACPU_PID_CSD,
                      "CSD_UL_RcvAtCallStateInd:: CSD_UL_InitQueue Fail Set State Off");

        /*??????????????????????????????????????*/
        CSD_SetCallState(AT_CSD_CALL_STATE_OFF);

        return VOS_ERR;
    }

    CSD_SetCallState(AT_CSD_CALL_STATE_ON);

    return VOS_OK;
}

VOS_UINT32 CSD_UL_RcvAtCallStateOff(VOS_VOID)
{
    IMM_ZC_HEAD_STRU                   *pstULQueue;
    VOS_UINT32                          ulRslt;

    pstULQueue      = CSD_UL_GetQueue();

    if (VOS_NULL_PTR == pstULQueue )
    {
        CSD_WARNING_LOG(ACPU_PID_CSD,
                        "CSD_UL_RcvAtCallStateInd:: pstULQueue Is NullSet State Off !");

        CSD_SetCallState(AT_CSD_CALL_STATE_OFF);

        return VOS_OK;
    }

    VOS_TaskLock();

    /*????????????*/
    ulRslt          = CSD_UL_FreeQueue(pstULQueue);

    /*??????????*/
    PS_MEM_FREE(ACPU_PID_CSD, pstULQueue);

    /*????????????*/
    CSD_UL_SetQueue(VOS_NULL_PTR);

    VOS_TaskUnlock();

    if (VOS_OK != ulRslt)
    {
        CSD_ERROR_LOG(ACPU_PID_CSD,
                      "CSD_UL_RcvAtCallStateInd:: CSD_UL_FreeQueue Fail Set State Off");
    }

    /*????????SLICE????*/
    CSD_SetCurrTxSlice(CSD_DEFAULT_TX_SLICE);

    CSD_SetCallState(AT_CSD_CALL_STATE_OFF);

    return VOS_OK;
}


VOS_UINT32 CSD_UL_RcvAtCallStateInd(MsgBlock* pMsg)
{
    VOS_UINT32                          ulRslt;
    AT_CSD_CALL_TYPE_STATE_ENUM_UINT16  enCallState;
    AT_CSD_CALL_STATE_TYPE_IND_STRU    *pstMsg;

    pstMsg          = (AT_CSD_CALL_STATE_TYPE_IND_STRU *)pMsg;
    enCallState     = pstMsg->enCallState;

    CSD_UL_SetAtClientIndex(pstMsg->ucIndex);

    if (AT_CSD_CALL_STATE_ON == enCallState)
    {
        ulRslt      = CSD_UL_RcvAtCallStateOn();
    }
    else if (AT_CSD_CALL_STATE_OFF == enCallState)
    {
        ulRslt      = CSD_UL_RcvAtCallStateOff();
    }
    else
    {
        CSD_ERROR_LOG1(ACPU_PID_CSD,
                      "CSD_UL_RcvAtCallStateInd:: enCallState Illage",
                      enCallState);

        return VOS_ERR;
    }

    return ulRslt;

}


VOS_VOID CSD_UL_ProcMsg(MsgBlock* pMsg)
{

    if (VOS_NULL_PTR == pMsg)
    {
        CSD_ERROR_LOG1(ACPU_PID_CSD,
                      "CSD_UL_ProcMsg:: pMsg is Null",
                      pMsg);
        return ;
    }

    switch ( pMsg->ulSenderPid )
    {
        case WUEPS_PID_AT :
            CSD_UL_RcvAtMsg(pMsg);
            break;

        default:
            CSD_ERROR_LOG1(ACPU_PID_CSD,
                          "CSD_UL_ProcMsg:: ulSenderPid Illage",
                          pMsg->ulSenderPid);
            break;
    }
}


VOS_VOID CSD_UL_RcvAtMsg(MsgBlock* pMsg)
{
    MSG_HEADER_STRU                    *pstMsgHeader;

    /* ??????????????MSG ID */
    pstMsgHeader = (MSG_HEADER_STRU *)pMsg;

    switch (pstMsgHeader->ulMsgName)
    {
        case ID_AT_CSD_CALL_STATE_IND :
             CSD_UL_RcvAtCallStateInd((MsgBlock *)pMsg);
             break;

        default:
            CSD_ERROR_LOG1(ACPU_PID_CSD,
                          "CSD_UL_RcvAtMsg:: enMsgId Illage",
                          pstMsgHeader->ulMsgName);
            break;
    }


}

#endif /*FEATURE_CSD*/





