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
#include "CsdInit.h"





/*****************************************************************************
    ??????????????????????.C??????????
*****************************************************************************/
#define    THIS_FILE_ID                 PS_FILE_ID_CSD_INIT_C

#if( FEATURE_ON == FEATURE_CSD )

/*****************************************************************************
  2 ????????????
*****************************************************************************/

/*****************************************************************************
  3 ????????
*****************************************************************************/

VOS_UINT32 CSD_InitDicc(VOS_VOID)
{
    VOS_UINT32                          ulDICCRslt;
    DICC_INIT_CHN_INFO_STRU             stSendInitInfo;
    DICC_INIT_CHN_INFO_STRU             stRcvInitInfo;

    /* ?????????????? */
    stSendInitInfo.enUserRole      = DICC_CHAN_ROLE_SENDER;
    stSendInitInfo.u.stSndInitInfo.stSndChnInfo.usChnBlkNum
                                   = DICC_PER_QUEUE_MAX_NODE_NUM;
    stSendInitInfo.u.stSndInitInfo.stSndChnInfo.usChnBlkSize
                                   = DICC_INFO_BLOCK_MAX_LEN;

    /* ?????????????? */
    stRcvInitInfo.enUserRole       = DICC_CHAN_ROLE_RECVER;

    /*????????????*/
    stRcvInitInfo.u.stRcvInitInfo.pfuncRecvIsr
                                   = CSD_DL_ProcIsr;

    /*??????DICC????????*/
    ulDICCRslt                     = DICC_SingleChnInit(ACPU_PID_CSD,
                                                        DICC_SERVICE_TYPE_CSD_DATA,
                                                        &stSendInitInfo,
                                                        DICC_CPU_ID_ACPU);
    if (DICC_OK != ulDICCRslt)
    {
        CSD_ERROR_LOG1(ACPU_PID_CSD, "CSD_InitDicc::DICC Sender INIT FAIL",ulDICCRslt);

        return VOS_ERR;
    }

    /* ??????DICC????????*/
    ulDICCRslt                     = DICC_SingleChnInit(ACPU_PID_CSD,
                                                        DICC_SERVICE_TYPE_CSD_DATA,
                                                        &stRcvInitInfo,
                                                        DICC_CPU_ID_ACPU);
    if (DICC_OK != ulDICCRslt)
    {
        CSD_ERROR_LOG1(ACPU_PID_CSD, "CSD_InitDicc::DICC Sender INIT FAIL",ulDICCRslt);

        return VOS_ERR;
    }


    /*????????DICC????????*/
    ulDICCRslt                      = DICC_OpenChannel(ACPU_PID_CSD,
                                                       DICC_CHAN_ID_UL_CSD_DATA_CHAN,
                                                       DICC_CHAN_ROLE_SENDER,
                                                       DICC_CPU_ID_ACPU);
    if (DICC_OK != ulDICCRslt)
    {
        CSD_ERROR_LOG1(ACPU_PID_CSD, "CSD_InitDicc::DICC SENDER OPEN FAIL",ulDICCRslt);

        return VOS_ERR;
    }

    /*????????DICC????????*/
    ulDICCRslt                      = DICC_OpenChannel(ACPU_PID_CSD,
                                                       DICC_CHAN_ID_DL_CSD_DATA_CHAN,
                                                       DICC_CHAN_ROLE_RECVER,
                                                       DICC_CPU_ID_ACPU);
    if (DICC_OK != ulDICCRslt)
    {
         CSD_ERROR_LOG1(ACPU_PID_CSD,"CSD_InitDicc :DICC RECVER OPEN FAIL",ulDICCRslt);

        return VOS_ERR;
    }


    return VOS_OK;
}

VOS_UINT32 CSD_UL_PidInit(enum VOS_INIT_PHASE_DEFINE ip)
{

    VOS_UINT32                          ulRslt;

    switch ( ip )
    {
        case   VOS_IP_LOAD_CONFIG:

            /*DICC??????????*/
            ulRslt = CSD_InitDicc();

            if (VOS_OK != ulRslt)
            {
                CSD_ERROR_LOG(ACPU_PID_CSD, "CSD_UL_PidInit::CSD_InitDicc FAIL");

                return VOS_ERR;
            }

            /*??????????????*/
            ulRslt = CSD_InitCtx();

            break;

        case   VOS_IP_FARMALLOC:
        case   VOS_IP_INITIAL:
        case   VOS_IP_ENROLLMENT:
        case   VOS_IP_LOAD_DATA:
        case   VOS_IP_FETCH_DATA:
        case   VOS_IP_STARTUP:
        case   VOS_IP_RIVAL:
        case   VOS_IP_KICKOFF:
        case   VOS_IP_STANDBY:
        case   VOS_IP_BROADCAST_STATE:
        case   VOS_IP_RESTART:
        case   VOS_IP_BUTT:
            break;
    }

    return VOS_OK;
}



VOS_UINT32 CSD_FidInit (enum VOS_INIT_PHASE_DEFINE  enInitPhase)
{
    VOS_UINT32 ulReturnCode;

    switch ( enInitPhase )
    {
        case   VOS_IP_LOAD_CONFIG:

            /*????????????????????*/
           ulReturnCode = CSD_InitSem();

            if (VOS_OK != ulReturnCode)
            {
                CSD_ERROR_LOG(ACPU_PID_CSD, "CSD_FidInit::CSD_InitSem FAIL");

                return VOS_ERR;
            }

            ulReturnCode = VOS_RegisterPIDInfo(ACPU_PID_CSD,
                                              (Init_Fun_Type) CSD_UL_PidInit,
                                              (Msg_Fun_Type) CSD_UL_ProcMsg);
            if ( VOS_OK != ulReturnCode )
            {
                CSD_ERROR_LOG(ACPU_PID_CSD,
                              "CSD_FidInit::VOS_RegisterPIDInfo ACPU_PID_CSD FAIL");

                return VOS_ERR;
            }


            /* ?????????????????? */
            ulReturnCode  = VOS_RegisterSelfTask(ACPU_FID_CSD,
                                                 (VOS_TASK_ENTRY_TYPE)CSD_UL_ProcDataTask,
                                                 VOS_PRIORITY_P5,
                                                 CSD_UL_DATA_TASK_STACK_SIZE);
            if ( VOS_NULL_BYTE == ulReturnCode )
            {
                CSD_ERROR_LOG(ACPU_PID_CSD,
                              "CSD_FidInit, ERROR, Fail regist CSD_UL_ProcDataTask");

                return VOS_ERR;
            }

            /* ?????????????????? */
            ulReturnCode  = VOS_RegisterSelfTask(ACPU_FID_CSD,
                                                 (VOS_TASK_ENTRY_TYPE)CSD_DL_ProcDataTask,
                                                 VOS_PRIORITY_P5,
                                                 CSD_DL_DATA_TASK_STACK_SIZE);
            if ( VOS_NULL_BYTE == ulReturnCode )
            {
                CSD_ERROR_LOG(ACPU_PID_CSD,
                              "CSD_FidInit, ERROR, Fail regist CSD_DL_ProcDataTask");

                return VOS_ERR;
            }


            /* ?????????? */
            ulReturnCode = VOS_RegisterMsgTaskPrio(ACPU_FID_CSD, VOS_PRIORITY_P3);
            if( VOS_OK != ulReturnCode )
            {
                return VOS_ERR;
            }

            break;

        case   VOS_IP_FARMALLOC:
        case   VOS_IP_INITIAL:
        case   VOS_IP_ENROLLMENT:
        case   VOS_IP_LOAD_DATA:
        case   VOS_IP_FETCH_DATA:
        case   VOS_IP_STARTUP:
        case   VOS_IP_RIVAL:
        case   VOS_IP_KICKOFF:
        case   VOS_IP_STANDBY:
        case   VOS_IP_BROADCAST_STATE:
        case   VOS_IP_RESTART:
        case   VOS_IP_BUTT:
            break;

    }

    return VOS_OK;
}
#else

VOS_UINT32 CSD_FidInit (enum VOS_INIT_PHASE_DEFINE  enInitPhase)
{
    return VOS_OK;
}
#endif /*FEATURE_CSD*/




