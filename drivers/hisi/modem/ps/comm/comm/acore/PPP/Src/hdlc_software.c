/*
 *
 * All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses. You may choose this file to be licensed under the terms
 * of the GNU General Public License (GPL) Version 2 or the 2-clause
 * BSD license listed below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/******************************************************************************
   ??????????
******************************************************************************/
#include "PPP/Inc/ppp_public.h"
#include "PPP/Inc/layer.h"
#include "PPP/Inc/ppp_fsm.h"
#include "PPP/Inc/ppp_mbuf.h"
#include "PPP/Inc/acf.h"
#include "PPP/Inc/proto.h"
#include "PPP/Inc/throughput.h"
#include "PPP/Inc/hdlc.h"
#include "PPP/Inc/lcp.h"
#include "PPP/Inc/async.h"
#include "PPP/Inc/auth.h"
#include "PPP/Inc/ipcp.h"
#include "PPP/Inc/pppid.h"
#include "PPP/Inc/link.h"
#include "PPP/Inc/hdlc_software.h"
#include "TTFComm.h"
#include "gucttf_tag.h"

#if(FEATURE_ON == FEATURE_PPP)

/*****************************************************************************
   1 ??????????????????????.C??????????
*****************************************************************************/
/*lint -e767 */
#define    THIS_FILE_ID        PS_FILE_ID_HDLC_SOFTWARE_C
#define    THIS_MODU           mod_ppp
/*lint +e767 */

/******************************************************************************
   2 ????????????????
******************************************************************************/

/*****************************************************************************
   3 ????????
*****************************************************************************/


/*****************************************************************************
   4 ????????????
*****************************************************************************/
PPP_HDLC_SOFT_DATA_PROC_STAT_ST g_PppHdlcSoftStat = {0};


/******************************************************************************
   5 ????????
******************************************************************************/

/*lint -e574*/
void link_PushTtfMemPacket(struct link *l, PPP_ZC_STRU *bp, VOS_INT32 pri, VOS_UINT16 proto)
{
    VOS_UINT8   aucPppDataHdr[4];
    VOS_UINT16  usHdrLen=0;


    /*
    * When we ``push'' a packet into the link, it gets processed by the
    * ``push'' function in each layer starting at the top.
    * We never expect the result of a ``push'' to be more than one
    * packet (as we do with ``pull''s).
    */

    if( (pri < 0) || (pri >= LINK_QUEUES(l)) )
    {
        pri = 0;
    }

    /*
    bp = proto_LayerPush(l, bp, pri, &proto);
    bp = acf_LayerPush(l, bp, pri, &proto);
    bp = async_LayerPush(l, bp, pri, &proto);
    */


    /* Address and Control */
    if ( (proto == PROTO_LCP) || (l->lcp.his_acfcomp == 0) )
    {
        aucPppDataHdr[usHdrLen++] = HDLC_ADDR;
        aucPppDataHdr[usHdrLen++] = HDLC_UI;
    }

    /* ???????????????????????? */
    /*??????????????????????????????????*/
    if ( (l->lcp.his_protocomp) && (((VOS_UINT8)(proto >> 8)) == 0))
    {
        aucPppDataHdr[usHdrLen++]   = proto & 0xff;
    }
    else
    {
        aucPppDataHdr[usHdrLen++]   = proto >> 8;
        aucPppDataHdr[usHdrLen++]   = proto & 0xff;
    }

    (void)async_TtfMemLayerPush(l, aucPppDataHdr, usHdrLen, bp, pri, &proto);

    return;
} /* link_PushTtfMemPacket */
/*lint +e574*/

/*lint -e{429} pLastMem???????????????? */
void link_PullMemPacket(struct link *l, PPP_ZC_STRU *pstMem)
{
    PPP_ZC_STRU      *pLastMem;
    PPP_ZC_STRU      *pTmpMem;
    VOS_UINT32        ulCnt;
    VOS_UINT16        usProto;
    VOS_UINT16        usLen;
    VOS_UINT8        *pucData;

    usLen = PPP_ZC_GET_DATA_LEN(pstMem);

    for (ulCnt = 0; ulCnt < usLen; ulCnt++)
    {
        pucData     = PPP_ZC_GET_DATA_PTR(pstMem);
        pLastMem    = async_Decode(&l->async, (VOS_CHAR)(pucData[ulCnt]));

        /* ??????????????PPP????????HDLC->ACF->Protocol?????????????????? */
        if (VOS_NULL_PTR != pLastMem)
        {
            pLastMem    = hdlc_LayerPull(l, pLastMem, &usProto);

            if (VOS_NULL_PTR != pLastMem)
            {
                pLastMem    = acf_LayerPull(l, pLastMem, &usProto);
            }

            if (VOS_NULL_PTR != pLastMem)
            {
                pLastMem    = proto_LayerPull(l, pLastMem, &usProto);
            }

            if (VOS_NULL_PTR != pLastMem)
            {
                /* ??????????????????????????????????????????????????????????????
                   ????????MAC??????????NDIS???? */

                pTmpMem = pLastMem;

                pLastMem = PPP_MemCopyAlloc(PPP_ZC_GET_DATA_PTR(pTmpMem),
                                        PPP_ZC_GET_DATA_LEN(pTmpMem),
                                        PPP_ZC_UL_RESERVE_LEN);

                PPP_MemFree(pTmpMem);
            }

            if (VOS_NULL_PTR != pLastMem)
            {
                PPP_HDLC_ProcIpModeUlData(/*b, */l, pLastMem, usProto);
            }
        }
    } /* for cnt */

    PPP_MemFree(pstMem);
}

#if (PPP_FEATURE == PPP_FEATURE_PPP)
/*****************************************************************************
 Prototype      : Ppp_FrameEncap
 Description    : ????PPP????????????????????????????????????PPP??????????
                  ??????????????????TE????

 Input          : ---
 Output         : ---??????
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2006-5-25
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_FrameEncap(PPP_ID PppId, PPP_ZC_STRU *pstData)
{
    VOS_UINT16          proto;
    VOS_UINT8           aucProto[2]={0,0};
    VOS_UINT32          ulRtn;


    /* ????????????????1????2?????? */
    ulRtn   = PPP_MemCutHeadData(&pstData, &aucProto[0], 1);

    if (PS_FAIL == ulRtn)
    {
        PPP_MemFree(pstData);
        return VOS_ERR;
    }

    if ( 0x01 == (aucProto[0] & 0x01) )     /* ??????????LSB??1?????????????????????????????????????? */
    {
        proto = ((VOS_UINT16)(aucProto[0]));
        PPP_LINK(PppId)->lcp.his_protocomp  = 1;
    }
    else
    {
        ulRtn   = PPP_MemCutHeadData(&pstData, &aucProto[1], 1);

        if (PS_FAIL == ulRtn)
        {
            PPP_MemFree(pstData);
            return VOS_ERR;
        }

        proto = ( (((VOS_UINT16)(aucProto[0]))<<8) | (aucProto[1]) );
        PPP_LINK(PppId)->lcp.his_protocomp  = 0;
    }

    if (VOS_NULL_PTR == pstData)
    {
        return VOS_ERR;
    }


    /* ??????????????????????????????IP???????????????????????? */
    link_PushTtfMemPacket(PPP_LINK(PppId), pstData, 0, proto);

    return VOS_OK;
}



/*****************************************************************************
 Prototype      : Ppp_FrameDecap
 Description    : ????PPP??????????????????TE??????????????PPP????????
                  ????????????????????????????

 Input          : ---
 Output         :
 Return Value   : VOS_NULL                       //????
                  ppp_mbuf *bp                   //??????????????????
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2006-5-25
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 Ppp_FrameDecap(PPP_ID PppId, PPP_ZC_STRU *pstMem)
{
    PPP_ZC_STRU      *pLastMem;
    VOS_UINT16        usCnt;
    VOS_UINT16        usProto;
    struct link      *l;
    VOS_UINT16        usLen;


    l     = PPP_LINK(PppId);
    usLen = PPP_ZC_GET_DATA_LEN(pstMem);

    for (usCnt = 0; usCnt < usLen; usCnt++)
    {
        pLastMem = async_Decode(&l->async, (VOS_CHAR)(PPP_ZC_GET_DATA_PTR(pstMem)[usCnt]));

        if (pLastMem != VOS_NULL_PTR)
        {
            /* ??????????????PPP????????HDLC->ACF??????????
            ????PPP????PDP????????????????????????????????Protocol?????????? */
            pLastMem    = hdlc_LayerPull(l, pLastMem, &usProto);

            if (VOS_NULL_PTR != pLastMem)
            {
                pLastMem    = acf_LayerPull(l, pLastMem, &usProto);
            }

            if (VOS_NULL_PTR != pLastMem)
            {
                PPP_HDLC_ProcPppModeUlData(PppId, pLastMem);
            }
        }
    } /* for cnt */

    PPP_MemFree(pstMem);

    return VOS_OK;
}

#else
void
link_PullPacket(struct link *l, VOS_CHAR *buf, VOS_UINT32 len)
{
  struct ppp_mbuf *bp, *lbp[LAYER_MAX];
  VOS_UINT16 lproto[LAYER_MAX], proto;
  VOS_INT32 layer;
  PPP_ZC_STRU   *pstMem;


  if (!l)
  {
    PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "link_PullPacket:Can't Pull an async packet from a logical link\r\n");
    return;
  }

  PSACORE_MEM_SET(lbp, sizeof (lbp), '\0', sizeof (lbp));

    lbp[0] = ppp_m_get((VOS_INT32)len);

  if(lbp[0] == VOS_NULL)
  {
    return;
  }

  PSACORE_MEM_CPY(PPP_MBUF_CTOP(lbp[0]), len, buf, len);


  lproto[0] = 0;
  layer = 0;

  while (layer || lbp[layer]) {
    if (lbp[layer] == VOS_NULL_PTR) {
      layer--;
      continue;
    }
    bp = lbp[layer];
    lbp[layer] = bp->m_nextpkt;
    bp->m_nextpkt = VOS_NULL_PTR;
    proto = lproto[layer];

    if (l->layer[layer]->pull != VOS_NULL_PTR)
      bp = (*l->layer[layer]->pull)(/*b, */l, bp, &proto);

    if (layer == (l->nlayers - 1)) {
      /* We've just done the top layer, despatch the packet(s) */
      if (bp) {
        pstMem  = ppp_m_alloc_ttfmem_from_mbuf(bp);
        ppp_m_freem(bp);

        if (VOS_NULL_PTR != pstMem)
        {
          PPP_HDLC_ProcIpModeUlData(/*b, */l, pstMem, proto);
        }
      }
    } else {
      lbp[++layer] = bp;
      lproto[layer] = proto;
    }
  }
}


/*****************************************************************************
 Prototype      : PPP_RcvPPPoEDataIndIPType
 Description    : ????IP??????????????????GGSN????????IP????????
                  ??????????????????PPPoE??????????

 Input          : ---
 Output         : ---
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2006-5-25
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 PPP_RcvPPPoEDataIndIPType(PPP_ID PppId, VOS_UINT8 * pucData, VOS_UINT16 usLen)
{
    struct ppp_mbuf *bp;

    bp = ppp_m_get(usLen + PPP_RECIEVE_RESERVE_FOR_HEAD + PPP_RECIEVE_RESERVE_FOR_TAIL);

    if(bp == VOS_NULL)
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "no mbuf\r\n");
        return VOS_ERR;
    }
    /*????????*/
    bp->m_offset = PPP_RECIEVE_RESERVE_FOR_HEAD;

    /*????????????????????*/
    bp->m_len = usLen;

    /*????????*/
    PSACORE_MEM_CPY(PPP_MBUF_CTOP(bp), usLen, pucData, usLen);
    link_PushPacket(PPP_LINK(PppId),bp,0,PROTO_IP);
    return VOS_OK;
}

/*****************************************************************************
 Prototype      : PPP_RcvPPPoEDataReqIPType
 Description    : ????IP??????????????????PPPoE????????TE??????????PPP????????
                  ???? ??

 Input          : ---
 Output         : ---
 Return Value   : ---
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2006-5-25
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_UINT32 PPP_RcvPPPoEDataReqIPType(PPP_ID PppId, VOS_UINT8 * pucData, VOS_UINT16 usLen)
{
    link_PullPacket(PPP_LINK(PppId),(VOS_CHAR *)pucData,usLen);
    return VOS_OK;

}
#endif

PPP_HDLC_RESULT_TYPE_ENUM_UINT32 PPP_HDLC_SOFT_ProcData
(
    PPP_ID usPppId,
    struct link *pstLink,
    PPP_ZC_QUEUE_STRU *pstDataQ
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT32                          ulDealCnt = 0;
    PPP_DATA_TYPE_ENUM_UINT8            ucDataType;

    for (;;)
    {
        /*????????????????????????????????????????????????????????
          ????????????????PPP_DATA_PROC_NOTIFY????*/
        if ( ulDealCnt >= PPP_ONCE_DEAL_MAX_CNT )
        {
            VOS_TaskDelay(1);
            return PPP_HDLC_RESULT_COMM_CONTINUE;
        }
        
        pstMem  = (PPP_ZC_STRU *)PPP_ZC_DEQUEUE_HEAD(pstDataQ);

        /* ???????????????????????? */
        if ( VOS_NULL_PTR == pstMem )
        {
            return PPP_HDLC_RESULT_COMM_FINISH;
        }

        /*??????????(??????????????????????????????????????????????????????)*/
        ucDataType = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);

        switch ( ucDataType )
        {
            case PPP_PULL_PACKET_TYPE:
                g_PppHdlcSoftStat.ulUplinkIpDataProcCnt++;

                link_PullMemPacket(pstLink, pstMem);

                LcpEchoAdjust(pstLink);

                break;
            case PPP_PUSH_PACKET_TYPE:
                g_PppHdlcSoftStat.ulDownlinkIpDataProcCnt++;

                link_PushTtfMemPacket(pstLink, pstMem, 0, PROTO_IP);
                break;

            case PPP_PULL_RAW_DATA_TYPE:
                g_PppHdlcSoftStat.ulUplinkPppDataProcCnt++;

                Ppp_FrameDecap(usPppId, pstMem);

                LcpEchoAdjust(pstLink);

                break;
            case PPP_PUSH_RAW_DATA_TYPE:
                g_PppHdlcSoftStat.ulDownlinkPppDataProcCnt++;

                Ppp_FrameEncap(usPppId, pstMem);
                break;

            default:
                PPP_MemFree(pstMem);
                PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, LOG_LEVEL_WARNING,
                              "PPP_ProcDataNotify, WARNING, ucDataType %d is Abnormal!", ucDataType);
                break;
        }

        /*????*/
        ulDealCnt++;

        if ( ulDealCnt > g_PppHdlcSoftStat.ulMaxCntOnce )
        {
            g_PppHdlcSoftStat.ulMaxCntOnce = ulDealCnt;
        }
    } /* for (;;) */
}


VOS_VOID PPP_HDLC_SOFT_ProcProtocolPacket
(
    struct link     *l,
    struct ppp_mbuf *bp,
    VOS_INT32       pri,
    VOS_UINT16      proto
)
{
    /*
    * When we ``push'' a packet into the link, it gets processed by the
    * ``push'' function in each layer starting at the top.
    * We never expect the result of a ``push'' to be more than one
    * packet (as we do with ``pull''s).
    */
    /*lint  -e574 -e960*/
    if(pri < 0 || pri >= LINK_QUEUES(l))
        pri = 0;
    /*lint  +e574 +e960*/

    bp = proto_LayerPush(l, bp, pri, &proto);
    bp = acf_LayerPush(l, bp, pri, &proto);
    (VOS_VOID)async_LayerPush(l, bp, pri, &proto);

    return;
}
#endif



