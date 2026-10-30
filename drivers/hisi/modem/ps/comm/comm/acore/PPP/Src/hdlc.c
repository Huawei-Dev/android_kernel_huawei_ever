/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 1996 - 2001 Brian Somers <brian@Awfulhak.org>
 *          based on work by Toshiharu OHNO <tony-o@iij.ad.jp>
 *                           Internet Initiative Japan, Inc (IIJ)
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
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
 * $FreeBSD: releng/11.2/usr.sbin/ppp/hdlc.c 330449 2018-03-05 07:26:05Z eadler $
 */


/******************************************************************************
   ??????????
******************************************************************************/
#include "mdrv.h"
#include "TTFComm.h"
#include "soc_sctrl_interface.h"
#include "PPP/Inc/ppp_public.h"
#include "PPP/Inc/layer.h"
#include "PPP/Inc/ppp_mbuf.h"
#include "PPP/Inc/ppp_fsm.h"
#include "PPP/Inc/hdlc.h"
#include "PPP/Inc/throughput.h"
#include "PPP/Inc/lcp.h"
#include "PPP/Inc/async.h"
#include "PPP/Inc/auth.h"
#include "PPP/Inc/ipcp.h"
#include "PPP/Inc/pppid.h"
#include "PPP/Inc/link.h"
#include "PPP/Inc/ppp_init.h"
#if (FEATURE_ON == FEATURE_HARDWARE_HDLC_FUNC)
#include "PPP/Inc/hdlc_hardware_service.h"
#include <linux/io.h>
#include "v_blkMem.h"
#include <linux/version.h>
#include <linux/of_platform.h>
#endif


/*****************************************************************************
    ??????????????????????.C??????????
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_HDLC_C

#if(FEATURE_ON == FEATURE_PPP)

VOS_UINT16 const fcstab[256] = {
   /* 00 */ 0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
   /* 08 */ 0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
   /* 10 */ 0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
   /* 18 */ 0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
   /* 20 */ 0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
   /* 28 */ 0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
   /* 30 */ 0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
   /* 38 */ 0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
   /* 40 */ 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
   /* 48 */ 0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
   /* 50 */ 0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
   /* 58 */ 0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
   /* 60 */ 0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
   /* 68 */ 0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
   /* 70 */ 0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
   /* 78 */ 0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
   /* 80 */ 0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
   /* 88 */ 0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
   /* 90 */ 0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
   /* 98 */ 0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
   /* a0 */ 0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
   /* a8 */ 0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
   /* b0 */ 0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
   /* b8 */ 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
   /* c0 */ 0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
   /* c8 */ 0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
   /* d0 */ 0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
   /* d8 */ 0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
   /* e0 */ 0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
   /* e8 */ 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
   /* f0 */ 0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
   /* f8 */ 0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

#if (FEATURE_ON == FEATURE_HARDWARE_HDLC_FUNC)
/* HDLC?????????????????????????? */
HTIMER                                      g_hHdlcFrmTimerHandle = VOS_NULL_PTR;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
struct device                       g_stHdlcDev = {0};
#ifdef CONFIG_ARM64
VOS_UINT64                          g_ullHdlcDmaMask = 0xffffffffffffffffULL;
#else
VOS_UINT64                          g_ullHdlcDmaMask = 0xffffffffULL;
#endif
#endif

struct device                      *g_pstHdlcDev;

#endif




/******************************************************************************
   5 ????????
******************************************************************************/
void
hdlc_Init(struct hdlc *hdlc, struct lcp *lcp)
{
  PSACORE_MEM_SET(hdlc, sizeof(struct hdlc), '\0', sizeof(struct hdlc));
  hdlc->lqm.owner = lcp;
}

/*
 *  HDLC FCS computation. Read RFC 1171 Appendix B and CCITT X.25 section
 *  2.27 for further details.
 */
VOS_UINT16
hdlc_Fcs(VOS_CHAR *cp, VOS_UINT32 len)
{
  VOS_UINT16 fcs = INITFCS;

  while (len--)
    fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];

  return fcs;
}

struct ppp_mbuf *
hdlc_LayerPush(struct link *l, struct ppp_mbuf *bp,
               VOS_INT32 pri, VOS_UINT16 *proto)
{
  return bp;
}

PPP_ZC_STRU *hdlc_LayerPull(struct link *l, PPP_ZC_STRU *pstMem, VOS_UINT16 *proto)
{
  VOS_UINT16    usFcs;
  VOS_UINT16    usLen;

  usLen = PPP_ZC_GET_DATA_LEN(pstMem);

  usFcs = hdlc_Fcs((VOS_CHAR*)PPP_ZC_GET_DATA_PTR(pstMem), usLen);

  /* PPP_MNTN_LOG1(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL, "hdlc_LayerPull: fcs = <1>\r\n",usFcs); */

  l->hdlc.lqm.ifInOctets += usLen + 1;        /* plus 1 flag octet! */

  if (usFcs != GOODFCS)
  {
    l->hdlc.lqm.ifInErrors++;
    l->hdlc.stats.badfcs++;
    PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_WARNING, "bad hdlc fcs\r\n");
    PPP_MemFree(pstMem);

    return VOS_NULL_PTR;
  }

  /* Either done here or by the sync layer */
  l->hdlc.lqm.lqr.InGoodOctets += usLen + 1;  /* plus 1 flag octet! */
  l->hdlc.lqm.ifInUniPackets++;

  /* rfc1662 section 4.3 */
  if (usLen < 4)
  {
    PPP_MemFree(pstMem);

    return VOS_NULL_PTR;
  }

  PPP_MemCutTailData(&pstMem, (VOS_UINT8 *)(&usFcs), 2, PPP_ZC_UL_RESERVE_LEN);

  return pstMem;
}

struct layer hdlclayer = { LAYER_HDLC, "hdlc", hdlc_LayerPush, hdlc_LayerPull };

#if (FEATURE_ON == FEATURE_HARDWARE_HDLC_FUNC)

VOS_VOID PPP_HDLC_HARD_CommReleaseLink
(
    PPP_HDLC_HARD_NODE_PARA_STRU       *pstLinkNode,
    VOS_UINT32                          ulRelCnt
)
{
    VOS_UINT32                          ulNodeLoop;


    for ( ulNodeLoop = 0; ulNodeLoop < ulRelCnt; ulNodeLoop++ )
    {
        if (VOS_NULL_PTR != pstLinkNode[ulNodeLoop].pDataNode)
        {
            PPP_MemFree((PPP_ZC_STRU*)(pstLinkNode[ulNodeLoop].pDataNode));

            pstLinkNode[ulNodeLoop].pDataNode = VOS_NULL_PTR;
        }
    }

    return;
}



VOS_VOID PPP_HDLC_HARD_DefProcErrorFrames
(
    VOS_UINT16                                      usPppId,
    PPP_DRIVER_HDLC_HARD_DEF_ERR_FRAMES_CNT_STRU   *pstErrCnt
)
{
    struct link                        *pstLink;
    /* ?????????????? */
    if ((VOS_NULL_PTR == pstErrCnt) || (0 == pstErrCnt->ucErrType))
    {
        return;
    }

    pstLink = PPP_LINK(usPppId);

    pstLink->hdlc.stats.badfcs      += pstErrCnt->usFCSErrCnt;
    pstLink->hdlc.lqm.ifInErrors    += pstErrCnt->usFCSErrCnt;

    pstLink->hdlc.lqm.ifInErrors    += pstErrCnt->usLenLongCnt;

    pstLink->hdlc.lqm.ifInErrors    += pstErrCnt->usLenShortCnt;

    pstLink->hdlc.lqm.ifInErrors    += pstErrCnt->usErrProtocolCnt;

    pstLink->hdlc.lqm.ifInErrors    += pstErrCnt->usErrCtrlCnt;
    pstLink->hdlc.stats.badcommand  += pstErrCnt->usErrCtrlCnt;

    pstLink->hdlc.lqm.ifInErrors    += pstErrCnt->usErrAddrCnt;
    pstLink->hdlc.stats.badaddr     += pstErrCnt->usErrAddrCnt;

    pstLink->hdlc.lqm.ifInErrors    += pstErrCnt->usFlagPosErrCnt;

    return;
}



VOS_VOID PPP_HDLC_HARD_DefResultProc
(
    PPP_HDLC_HARD_DEF_RESULT_STRU      *pstDefResult
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT32                          usFistSegLen;
    VOS_UINT16                          usProtocol;
    struct link                        *pstLink;
    VOS_UINT16                          usDefOutLen;

    if (VOS_NULL_PTR == pstDefResult)
    {
        PPP_HDLC_WARNING_LOG("pstDefResult is NULL");
        return;
    }

    usProtocol       = pstDefResult->usProtocol;
    if (PS_TRUE == pstDefResult->enSegment)
    {
        usDefOutLen = pstDefResult->astDataInfo[0].usDataLen + pstDefResult->astDataInfo[1].usDataLen;
    }
    else
    {
        usDefOutLen = pstDefResult->astDataInfo[0].usDataLen;
    }

    pstMem = PPP_MemAlloc(usDefOutLen, PPP_ZC_UL_RESERVE_LEN);
    if (VOS_NULL_PTR == pstMem)
    {
        return;
    }

    if (PS_TRUE == pstDefResult->enSegment)
    {
        usFistSegLen = pstDefResult->astDataInfo[0].usDataLen;
        /* ?????????????????????????????????? */
        PPP_MemSingleCopy(PPP_ZC_GET_DATA_PTR(pstMem), usDefOutLen, pstDefResult->astDataInfo[0].pucDataAddr,
                          usFistSegLen);

        /* ???????????????????????????? */
        PPP_MemSingleCopy(PPP_ZC_GET_DATA_PTR(pstMem) + usFistSegLen,
                          (usDefOutLen - usFistSegLen),
                          pstDefResult->astDataInfo[1].pucDataAddr,
                          pstDefResult->astDataInfo[1].usDataLen);
    }
    else
    {
        PPP_MemSingleCopy(PPP_ZC_GET_DATA_PTR(pstMem), usDefOutLen,
                pstDefResult->astDataInfo[0].pucDataAddr, usDefOutLen);
    }

    /* ?????????????????? */
    PPP_ZC_SET_DATA_LEN(pstMem, usDefOutLen);

    if (PPP_SERVICE_HDLC_HARD_IP_MODE == pstDefResult->ulPara)
    {
        pstLink = PPP_LINK(pstDefResult->usPppId);
        PPP_HDLC_ProcIpModeUlData(pstLink, pstMem, usProtocol);
    }
    else
    {
        PPP_HDLC_ProcPppModeUlData(pstDefResult->usPppId, pstMem);
    }

    return;
}


PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32 PPP_HDLC_HARD_DefCheckPara
(
    PPP_ZC_STRU                 *pstMem,
    VOS_UINT32                   ulDataLen,
    PPP_DATA_TYPE_ENUM_UINT8     ucDataType
)
{
    PPP_DATA_TYPE_ENUM_UINT8     ucCurrDataType;


    /* ???????????????????? */
    ucCurrDataType = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);

    if ( ucDataType != ucCurrDataType )
    {
        return PPP_HDLC_PARA_CHECK_FAIL_KEEP;
    }

    /* ??????????????????????????????????????????????????1536B */
    /* ????HDLC Bug????????????1 */
    if ( (0 == ulDataLen) || (HDLC_DEF_IN_PER_MAX_CNT < ulDataLen) )
    {
        PPP_HDLC_WARNING_LOG1("PPP_HDLC_HARD_DefCheckPara, NORMAL, invalid data length %d\r\n", ulDataLen);

        return PPP_HDLC_PARA_CHECK_FAIL_DISCARD;
    }

    return PPP_HDLC_PARA_CHECK_PASS;
}



VOS_VOID PPP_HDLC_HARD_ForbiddenHdlcBug(PPP_ZC_STRU **ppstMem)
{
    PPP_ZC_STRU *pstMem;
    PPP_ZC_STRU *pstTmpMem;
    VOS_UINT8   *pstData;
    VOS_UINT16   usLen;
    const VOS_UINT8    ucEndByte = 0x7e;

    pstMem  = *ppstMem;

    if (VOS_NULL_PTR == pstMem)
    {
        return;
    }

    usLen   = PPP_ZC_GET_DATA_LEN(pstMem);
    pstData = PPP_ZC_GET_DATA_PTR(pstMem);

    if (usLen <= 1)
    {
        return;
    }

    if (ucEndByte != pstData[usLen - 2])
    {
        /* ??????????????????7e???????? */
        return;
    }
    else if ((usLen >= 3) && (ucEndByte == pstData[usLen - 3]))
    {
        /* ????????????????7e???????? */
        return;
    }
    else
    {
        /*
            ????????????????7e,??????????????????7e??????
            ????????????????????????????,??????????????+1

            skb??????????????????
           |--------data(uslen)--------|----32bytes align(0~31)-----|--skb_share_info(256)----|
        */
        if (PPP_ZC_GET_RESERVE_ROOM(pstMem) > 0)
        {
            INSERT_BYTE_LAST(pstMem, pstData, usLen, ucEndByte);
            g_PppHdlcHardStat.ulForbidHdlcBugNoCpy++;
        }
        else
        {
            /* ????????????1?????????? */

            pstTmpMem = PPP_MemAlloc((VOS_UINT16)(usLen + 1), 0);
            if (VOS_NULL_PTR != pstTmpMem)
            {
                PPP_MemWriteData(pstTmpMem, (VOS_UINT16)(usLen + 1), pstData, usLen);

                pstData = PPP_ZC_GET_DATA_PTR(pstTmpMem);

                INSERT_BYTE_LAST(pstTmpMem, pstData, usLen, ucEndByte);
                g_PppHdlcHardStat.ulForbidHdlcBugCpy++;
            }

            /* ????????????????????????????????,?????????? */
            *ppstMem = pstTmpMem;

            /* ???????????? */
            PPP_MemFree(pstMem);
        }
    }
}


VOS_UINT32 PPP_HDLC_HARD_DefBuildInputParaLink
(
    HDLC_PARA_LINK_BUILD_PARA_STRU     *pstBuildPara,
    PPP_HDLC_HARD_DEF_PARA_STRU        *pstDefPara,
    VOS_UINT32                         *pulDealCnt
)
{
    PPP_ZC_STRU                            *pstMem;
    PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32  enCheckResult;
    VOS_UINT16                              usDataLen;
    VOS_UINT32                              ulNodeCnt = 0;
    VOS_UINT32                              ulMaxDataLen1Time;
    VOS_UINT32                              ulDropCnt = 0;


    pstDefPara->usPppId     = pstBuildPara->usPppId;
    pstDefPara->ulPppAcFlag = pstBuildPara->pstLink->lcp.want_acfcomp;
    pstDefPara->ulPppPcFlag = pstBuildPara->pstLink->lcp.want_protocomp;
    pstDefPara->enPppMode   = pstBuildPara->enPppMode;
    pstDefPara->ulPara      = pstBuildPara->enPppMode;

    ulMaxDataLen1Time = PPP_Service_HdlcHardGetMaxDefLen(pstBuildPara->usPppId);

    while (ulNodeCnt < TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_NUM)
    {
        pstMem     = (PPP_ZC_STRU *)PPP_ZC_PEEK_QUEUE_HEAD(pstBuildPara->pstDataQ);

        /* ?????????????????????????????????????????? */
        if( VOS_NULL_PTR == pstMem )
        {
            break;
        }

        usDataLen     = PPP_ZC_GET_DATA_LEN(pstMem);
        enCheckResult = PPP_HDLC_HARD_DefCheckPara(pstMem, usDataLen, pstBuildPara->ucDataType);

        if( PPP_HDLC_PARA_CHECK_FAIL_DISCARD == enCheckResult )
        {
            pstMem = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);
            PPP_MemFree(pstMem);
            ulDropCnt++;

            g_PppHdlcHardStat.ulDefInputDiscardCnt++;
            continue;
        }

        if( PPP_HDLC_PARA_CHECK_FAIL_KEEP == enCheckResult )
        {
            break;
        }

        /* ??PPP?????????????????????????????? pstBuildInfo->apstInputLinkNode */
        pstMem  = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);

        /* ????HDLC BUG,??????HDLC BUG????????????????????????,
           ??????pstMem??????????????????,??????????????????????????
         */
        PPP_HDLC_HARD_ForbiddenHdlcBug(&pstMem);

        /* ??????????????????????????,??????????????????  */
        if (VOS_NULL_PTR == pstMem)
        {
            continue;
        }

        /* ????????????????,?????????? */
        usDataLen = PPP_ZC_GET_DATA_LEN(pstMem);

        /* ????????????????????????????????????????15KB */
        /* ????HDLC BUG,????????????????,??????????1????,???????????????? */
        if( pstDefPara->ulInputTotalSize + usDataLen  > ulMaxDataLen1Time )
        {
            /* ????????????,?????????? */
            PPP_ZC_ENQUEUE_HEAD(pstBuildPara->pstDataQ, pstMem);

            break;
        }

        /* ?????????????????????????? */
        pstDefPara->astInputNodePara[ulNodeCnt].pucDataAddr = PPP_ZC_GET_DATA_PTR(pstMem);
        pstDefPara->astInputNodePara[ulNodeCnt].usDataLen   = usDataLen;
        pstDefPara->astInputNodePara[ulNodeCnt].pDataNode   = pstMem;
        pstDefPara->ulInputTotalSize += usDataLen;

        /* ??????????????DDR??HDLC??DDR???????? */
        PPP_HDLC_CACHE_FLUSH(PPP_ZC_GET_DATA_PTR(pstMem), PPP_ZC_GET_DATA_LEN(pstMem));

        ulNodeCnt++;
    }

    pstDefPara->ulInputNodeParaCnt = ulNodeCnt;
    *pulDealCnt = ulNodeCnt + ulDropCnt;

    if (0 == ulNodeCnt)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 PPP_HDLC_HARD_DefPacket
(
    HDLC_PARA_LINK_BUILD_PARA_STRU     *pstBuildPara,
    VOS_UINT32                         *pulDealCnt
)
{
    VOS_UINT32                          ulBuildResult;
    PPP_HDLC_HARD_DEF_PARA_STRU         stDefPara;


    /* ???????????? */
    PSACORE_MEM_SET(&stDefPara, sizeof(PPP_HDLC_HARD_DEF_PARA_STRU),
                 0, sizeof(PPP_HDLC_HARD_DEF_PARA_STRU));

    /* ???????????????????????????????????????? */
    ulBuildResult = PPP_HDLC_HARD_DefBuildInputParaLink(pstBuildPara, &stDefPara, pulDealCnt);
    if (VOS_OK != ulBuildResult)
    {
        PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_DefPacket, WARNING, BuildParaLink Fail.\r\n");
        return ulBuildResult;
    }

    PPP_Service_HdlcHardDefPacket(&stDefPara);

    /* ?????????????????????? */
    PPP_HDLC_HARD_CommReleaseLink(stDefPara.astInputNodePara, stDefPara.ulInputNodeParaCnt);

    return VOS_OK;
}


VOS_UINT32 PPP_HDLC_HARD_FrmOutputMemAlloc
(
    VOS_UINT16                           usLen,
    PPP_HDLC_HARD_FRM_PARA_STRU         *pstFrmPara
)
{
    VOS_UINT16                          usFrmedMaxLen;                 /* ?????????????????????? */
    VOS_UINT16                          usAllocLen;
    VOS_UINT32                          ulOutputNodeCnt = 0;
    VOS_UINT32                          ulOutputNodeIndex = 0;
    VOS_UINT32                          ulLoopCnt;
    PPP_ZC_STRU                        *pstMem;
    PPP_ZC_STRU*                        apstOutputNode[PPP_HDLC_MAX_OUT_NODE_FOR_ONE_INPUT];
    PPP_HDLC_HARD_NODE_PARA_STRU       *pstNodePara;
    VOS_UINT16                          ausOutputNodeLen[PPP_HDLC_MAX_OUT_NODE_FOR_ONE_INPUT];


    /* ????????????????????????(2*????????????+13B) */
    usFrmedMaxLen = (VOS_UINT16)HDLC_FRM_GET_MAX_FRAMED_LEN(usLen);
    ulLoopCnt     = TTF_CEIL(usFrmedMaxLen, PPP_ZC_MAX_DATA_LEN);

    while (ulOutputNodeCnt < ulLoopCnt)
    {
        usAllocLen = PPP_ZC_MAX_DATA_LEN;
        pstMem = PPP_MemAlloc(usAllocLen, PPP_ZC_DL_RESERVE_LEN);

        if (VOS_NULL_PTR == pstMem)
        {
            break;
        }

        apstOutputNode[ulOutputNodeCnt]     = pstMem;
        ausOutputNodeLen[ulOutputNodeCnt]   = usAllocLen;

        ulOutputNodeCnt++;
    }

    /* ?????????????????????????????????????????????????? */
    if (ulOutputNodeCnt < ulLoopCnt)
    {
        PPP_MemFree(apstOutputNode[0]);
        return VOS_ERR;
    }

    for (ulOutputNodeIndex = 0; ulOutputNodeIndex < ulOutputNodeCnt; ulOutputNodeIndex++)
    {
        /* ???????????????????????????????????????????????? */
        pstNodePara = &(pstFrmPara->astOutputNodePara[pstFrmPara->ulOutputNodeParaCnt + ulOutputNodeIndex]);
        pstNodePara->pucDataAddr    = PPP_ZC_GET_DATA_PTR(apstOutputNode[ulOutputNodeIndex]);
        pstNodePara->usDataLen      = ausOutputNodeLen[ulOutputNodeIndex];
        /* ?????????????????????????????????????????????? */
        pstNodePara->pDataNode      = apstOutputNode[ulOutputNodeIndex];

        PPP_HDLC_CACHE_INVALID(pstNodePara->pucDataAddr, ausOutputNodeLen[ulOutputNodeIndex]);
    }

    pstFrmPara->ulOutputNodeParaCnt += ulOutputNodeCnt;

    return VOS_OK;
}



VOS_UINT32 PPP_HDLC_HARD_FrmGetProtocol
(
    PPP_ZC_STRU     *pstMem,
    VOS_UINT16      *pusProtocol
)
{
    VOS_UINT8           aucProto[2] = {0, 0};
    VOS_UINT32          ulRtn;


    ulRtn = PPP_MemGet(pstMem, 0, aucProto, 2);

    if ( PS_FAIL == ulRtn )
    {
        return VOS_ERR;
    }

    if ( 0x01 == GET_BITS_FROM_BYTE(aucProto[0], 0x01) )
    {
        *pusProtocol = HDLC_MAKE_WORD(0x00, aucProto[0]);
    }
    else
    {
        if ( 0x01 == GET_BITS_FROM_BYTE(aucProto[1], 0x01) )
        {
            *pusProtocol = HDLC_MAKE_WORD(aucProto[0], aucProto[1]);
        }
        else
        {
            PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_FrmGetProtocol, Warning, PPP mode framer data protocol error\r\n");
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32 PPP_HDLC_HARD_FrmCheckPara
(
    PPP_ZC_STRU                 *pstMem,
    VOS_UINT32                   ulDataLen,
    PPP_DATA_TYPE_ENUM_UINT8     ucDataType,
    VOS_UINT16                   usProtocol
)
{
    PPP_DATA_TYPE_ENUM_UINT8     ucCurrDataType;
    VOS_UINT32                   ulRlst;
    VOS_UINT16                   usCurrProtocol;


    /* ???????????????????? */
    ucCurrDataType = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);

    if ( ucDataType != ucCurrDataType )
    {
        return PPP_HDLC_PARA_CHECK_FAIL_KEEP;
    }

    /* ???????????????????? */
    if ( (0 == ulDataLen) || (HDLC_FRM_IN_PER_MAX_CNT < ulDataLen) )
    {
        PPP_HDLC_WARNING_LOG1("PPP_HDLC_HARD_FrmCheckPara,IP mode framer data len wrong %d\r\n", ulDataLen);

        return PPP_HDLC_PARA_CHECK_FAIL_DISCARD;
    }

    /* ????????PPP???? */
    if( PPP_PUSH_RAW_DATA_TYPE != ucCurrDataType )
    {
        return PPP_HDLC_PARA_CHECK_PASS;
    }

    ulRlst  = PPP_HDLC_HARD_FrmGetProtocol(pstMem, &usCurrProtocol);

    if( VOS_OK != ulRlst )
    {
        PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_FrmCheckPara, Warning, PPP mode get framer data protocol error\r\n");
        return PPP_HDLC_PARA_CHECK_FAIL_DISCARD;
    }

    /* ???????????????????????????????????? */
    if( usProtocol != usCurrProtocol )
    {
        return PPP_HDLC_PARA_CHECK_FAIL_KEEP;
    }

    return PPP_HDLC_PARA_CHECK_PASS;
}


VOS_VOID PPP_HDLC_HARD_FrmStartTimer
(
    VOS_UINT16                          usPppId
)
{
    if( VOS_NULL_PTR != g_hHdlcFrmTimerHandle )
    {
        return;
    }

    if (VOS_OK != VOS_StartRelTimer(&g_hHdlcFrmTimerHandle, PS_PID_APP_PPP,
                     HDLC_FRM_TIME_INTERVAL,
                     TIMER_HDLC_FRM_OUTPUT_SPACE_ALLOC_FAIL,
                     (VOS_UINT32)usPppId,
                     VOS_RELTIMER_NOLOOP, VOS_TIMER_PRECISION_0))
    {
        PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_FrmStartTimer, Warning, start reltimer error\r\n");
    }

    return;
}


VOS_UINT32 PPP_HDLC_HARD_FrmBuildParaLink
(
    HDLC_PARA_LINK_BUILD_PARA_STRU         *pstBuildPara,
    PPP_HDLC_HARD_FRM_PARA_STRU            *pstFrmPara,
    VOS_UINT32                             *pulDealCnt
)
{
    PPP_ZC_STRU                            *pstMem;
    VOS_UINT16                              usUnFrmLen;
    VOS_UINT32                              ulAllocResult;
    VOS_UINT32                              ulNodeCnt = 0;
    PPP_HDLC_PARA_CHECK_RESULT_ENUM_UINT32  enCheckResult;
    VOS_UINT32                              ulAllocMemFail;
    VOS_UINT32                              ulDropCnt = 0;


    pstFrmPara->usPppId     = pstBuildPara->usPppId;
    pstFrmPara->usProtocol  = pstBuildPara->usProtocol;
    pstFrmPara->ulPppAcFlag = pstBuildPara->pstLink->lcp.his_acfcomp;
    pstFrmPara->ulPppPcFlag = pstBuildPara->pstLink->lcp.his_protocomp;
    pstFrmPara->ulAccmFlag  = pstBuildPara->pstLink->lcp.his_accmap;
    pstFrmPara->enPppMode   = pstBuildPara->enPppMode;

    pstFrmPara->ulInputNodeParaCnt = 0;
    ulAllocMemFail = VOS_FALSE;

    while (ulNodeCnt < TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_NUM)
    {
        pstMem = (PPP_ZC_STRU *)PPP_ZC_PEEK_QUEUE_HEAD(pstBuildPara->pstDataQ);

        if( VOS_NULL_PTR == pstMem )
        {
            break;
        }

        usUnFrmLen    = PPP_ZC_GET_DATA_LEN(pstMem);
        enCheckResult = PPP_HDLC_HARD_FrmCheckPara(pstMem, usUnFrmLen,
                                                   pstBuildPara->ucDataType,
                                                   pstBuildPara->usProtocol);

        if( PPP_HDLC_PARA_CHECK_FAIL_DISCARD == enCheckResult )
        {
            pstMem = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);
            PPP_MemFree(pstMem);
            ulDropCnt++;

            g_PppHdlcHardStat.ulFrmInputDiscardCnt++;
            continue;
        }

        if( PPP_HDLC_PARA_CHECK_FAIL_KEEP == enCheckResult )
        {
            break;
        }

        /* ????????????????????????????????????????15KB */
        if( pstFrmPara->ulInputTotalSize + usUnFrmLen > TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_SIZE )
        {
            break;
        }

        /* ???????????????? */
        ulAllocResult = PPP_HDLC_HARD_FrmOutputMemAlloc(usUnFrmLen, pstFrmPara);

        if( VOS_OK != ulAllocResult )
        {
            ulAllocMemFail = VOS_TRUE;

            g_PppHdlcHardStat.ulFrmAllocOutputMemFailCnt++;
            break;
        }

        /* ??PPP?????????????????????????????? pstBuildInfo->apstInputLinkNode */
        pstMem = PPP_ZC_DEQUEUE_HEAD(pstBuildPara->pstDataQ);

        pstFrmPara->astInputNodePara[ulNodeCnt].pucDataAddr = PPP_ZC_GET_DATA_PTR(pstMem);
        pstFrmPara->astInputNodePara[ulNodeCnt].usDataLen   = usUnFrmLen;
        pstFrmPara->ulInputTotalSize += usUnFrmLen;
        pstFrmPara->astInputNodePara[ulNodeCnt].pDataNode   = pstMem;

        /* ??????????????DDR??HDLC??DDR???????? */
        PPP_HDLC_CACHE_FLUSH(PPP_ZC_GET_DATA_PTR(pstMem), PPP_ZC_GET_DATA_LEN(pstMem));

        ulNodeCnt++;
    }

    pstFrmPara->ulInputNodeParaCnt = ulNodeCnt;
    *pulDealCnt = ulNodeCnt + ulDropCnt;

    if (0 == ulNodeCnt)
    {
        if (VOS_TRUE == ulAllocMemFail)
        {
            /* ?????????????????????????????????????? */
            PPP_HDLC_HARD_FrmStartTimer(pstBuildPara->usPppId);

            g_PppHdlcHardStat.ulFrmAllocFirstMemFailCnt++;
        }

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID PPP_HDLC_HARD_FrmResultProc
(
    PPP_HDLC_HARD_FRM_RESULT_STRU      *pstFrmResult,
    PPP_HDLC_HARD_FRM_PARA_STRU        *pstFrmPara
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT32                          ucRptSpaceIndex;
    VOS_UINT32                          ulOutputLinkIndex = 0;
    VOS_UINT16                          usFrmOutLen;
    VOS_UINT16                          usDataLen;

    for (ucRptSpaceIndex = 0; ucRptSpaceIndex < pstFrmResult->ucFrmValidCnt; ucRptSpaceIndex++)
    {
        usFrmOutLen = pstFrmResult->astFrmResultNode[ucRptSpaceIndex].usDataLen;

        while( usFrmOutLen > 0 )
        {
            /* ??pstBuildInfo->apstOutputLinkNode?????????? pstMem */
            pstMem    = (PPP_ZC_STRU*)(pstFrmPara->astOutputNodePara[ulOutputLinkIndex].pDataNode);

            /* ?????????????????????????????????? */
            usDataLen = pstFrmPara->astOutputNodePara[ulOutputLinkIndex].usDataLen;

            if( usFrmOutLen > usDataLen )
            {
                PPP_ZC_SET_DATA_LEN(pstMem, usDataLen);
                usFrmOutLen -= usDataLen;
            }
            else
            {
                PPP_ZC_SET_DATA_LEN(pstMem, usFrmOutLen);
                usFrmOutLen  = 0;
            }

            PPP_HDLC_ProcDlData(pstFrmPara->usPppId, pstMem);

            g_PppDataQCtrl.stStat.ulDownlinkSndDataCnt++;

            /* ??????????????????apstOutputLinkNode?????? */
            pstFrmPara->astOutputNodePara[ulOutputLinkIndex].pDataNode = VOS_NULL_PTR;

            ulOutputLinkIndex++;
        }
    }

    return;
}


VOS_UINT32 PPP_HDLC_HARD_FrmPacket
(
    HDLC_PARA_LINK_BUILD_PARA_STRU     *pstBuildPara,
    VOS_UINT32                         *pulDealCnt
)
{
    VOS_UINT32                          ulBuildResult;
    PPP_HDLC_HARD_FRM_PARA_STRU         stFrmPara;
    PPP_HDLC_HARD_FRM_RESULT_STRU       stFrmResult;
    PPP_HDLC_HARD_PROC_RESULT_ENUM_UINT32   ulFrmResult;

    /* ???????? */
    PSACORE_MEM_SET(&stFrmPara, sizeof(PPP_HDLC_HARD_FRM_PARA_STRU),
                0, sizeof(PPP_HDLC_HARD_FRM_PARA_STRU));
    PSACORE_MEM_SET(&stFrmResult, sizeof(PPP_HDLC_HARD_FRM_RESULT_STRU),
                0, sizeof(PPP_HDLC_HARD_FRM_RESULT_STRU));


    /* ???????????????????????????????????????????? */
    ulBuildResult  = PPP_HDLC_HARD_FrmBuildParaLink(pstBuildPara, &stFrmPara, pulDealCnt);
    if (VOS_OK != ulBuildResult)
    {
        PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_FrmPacket, WARNING, BuildParaLink Fail.\r\n");
        return ulBuildResult;
    }


    ulFrmResult = PPP_Service_HdlcHardFrmPacket(&stFrmPara, &stFrmResult);
    if ((PPP_HDLC_HARD_PROC_ALL_SUCC == ulFrmResult)
        || (PPP_HDLC_HARD_PROC_PART_SUCC == ulFrmResult))
    {
        PPP_HDLC_HARD_FrmResultProc(&stFrmResult, &stFrmPara);
    }

    /* ?????????????????????????????????????????????? */
    PPP_HDLC_HARD_CommReleaseLink(stFrmPara.astInputNodePara, stFrmPara.ulInputNodeParaCnt);
    PPP_HDLC_HARD_CommReleaseLink(stFrmPara.astOutputNodePara, stFrmPara.ulOutputNodeParaCnt);

    return VOS_OK;
}


PPP_HDLC_RESULT_TYPE_ENUM_UINT32 PPP_HDLC_HARD_ProcData
(
    PPP_ID              usPppId,
    struct link        *pstLink,
    PPP_ZC_QUEUE_STRU  *pstDataQ
)
{
    PPP_ZC_STRU                        *pstMem;
    VOS_UINT32                          ulDealTotalCnt;
    VOS_UINT32                          ulDealCurCnt;
    VOS_UINT32                          ulDealResult;
    VOS_UINT16                          usProtocol;
    PPP_DATA_TYPE_ENUM_UINT8            ucDataType;
    HDLC_PARA_LINK_BUILD_PARA_STRU      stBuildPara;
    

    PSACORE_MEM_SET(&stBuildPara, sizeof(HDLC_PARA_LINK_BUILD_PARA_STRU), 0 ,sizeof(HDLC_PARA_LINK_BUILD_PARA_STRU));
    g_PppHdlcHardStat.ulHdlcHardProcCnt++;

    ulDealTotalCnt  = 0;
    ulDealCurCnt    = 0;

    stBuildPara.usPppId    = usPppId;
    stBuildPara.pstDataQ   = pstDataQ;
    stBuildPara.pstLink    = pstLink;

    for (;;)
    {
        pstMem  = (PPP_ZC_STRU *)PPP_ZC_PEEK_QUEUE_HEAD(pstDataQ);

        /* ???????????????????????? */
        if ( VOS_NULL_PTR == pstMem )
        {
            return PPP_HDLC_RESULT_COMM_FINISH;
        }

        /*????????????????????????????????????????????????????????
          ????????????????PPP_DATA_PROC_NOTIFY????*/
        if ( (ulDealTotalCnt >= PPP_ONCE_DEAL_MAX_CNT) )
        {
            g_PppHdlcHardStat.ulContinueCnt++;
            return PPP_HDLC_RESULT_COMM_CONTINUE;
        }

        /*??????????(??????????????????????????????????????????????????????)*/
        ucDataType   = (PPP_ZC_GET_DATA_APP(pstMem) & 0x00FF);
        ulDealResult = VOS_OK;
        ulDealCurCnt = 0;

        stBuildPara.ucDataType = ucDataType;

        switch ( ucDataType )
        {
            case PPP_PULL_PACKET_TYPE:

                stBuildPara.usProtocol = PROTO_IP;
                stBuildPara.enPppMode  = PPP_SERVICE_HDLC_HARD_IP_MODE;
                ulDealResult = PPP_HDLC_HARD_DefPacket(&stBuildPara, &ulDealCurCnt);

                g_PppHdlcHardStat.ulDefIpDataProcCnt += ulDealCurCnt;

                LcpEchoAdjust(pstLink);

                break;
            case PPP_PUSH_PACKET_TYPE:

                stBuildPara.usProtocol = PROTO_IP;
                stBuildPara.enPppMode  = PPP_SERVICE_HDLC_HARD_IP_MODE;
                ulDealResult = PPP_HDLC_HARD_FrmPacket(&stBuildPara, &ulDealCurCnt);

                g_PppHdlcHardStat.ulFrmIpDataProcCnt += ulDealCurCnt;
                break;

            case PPP_PULL_RAW_DATA_TYPE:

                stBuildPara.usProtocol = PROTO_IP;
                stBuildPara.enPppMode  = PPP_SERVICE_HDLC_HARD_PPP_MODE;
                ulDealResult = PPP_HDLC_HARD_DefPacket(&stBuildPara, &ulDealCurCnt);

                g_PppHdlcHardStat.ulDefIpDataProcCnt += ulDealCurCnt;

                LcpEchoAdjust(pstLink);

                break;

            case PPP_PUSH_RAW_DATA_TYPE:

                /* ????????????????????????usProtocol */
                if (VOS_OK == PPP_HDLC_HARD_FrmGetProtocol(pstMem, &usProtocol))
                {
                    stBuildPara.usProtocol = usProtocol;
                    stBuildPara.enPppMode  = PPP_SERVICE_HDLC_HARD_PPP_MODE;
                    ulDealResult = PPP_HDLC_HARD_FrmPacket(&stBuildPara, &ulDealCurCnt);

                    g_PppHdlcHardStat.ulFrmPppDataProcCnt += ulDealCurCnt;
                }
                else
                {
                    pstMem  = (PPP_ZC_STRU *)PPP_ZC_DEQUEUE_HEAD(pstDataQ);
                    PPP_MemFree(pstMem);
                }
                break;

            default:
                pstMem  = (PPP_ZC_STRU *)PPP_ZC_DEQUEUE_HEAD(pstDataQ);
                PPP_MemFree(pstMem);
                PPP_HDLC_WARNING_LOG1("PPP_HDLC_HARD_ProcData, WARNING, ucDataType %d is Abnormal!", ucDataType);
                break;
        }

        /*????*/
        ulDealTotalCnt += ulDealCurCnt;

        if ( ulDealTotalCnt > g_PppHdlcHardStat.ulMaxCntOnce )
        {
            g_PppHdlcHardStat.ulMaxCntOnce = ulDealTotalCnt;
        }

        if (VOS_OK != ulDealResult)
        {
            PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_ProcData, WARNING, ulDealResult = ERROR!");
            break;
        }
    } /* for (;;) */

    return PPP_HDLC_RESULT_COMM_ERROR;
}


VOS_UINT32 PPP_HDLC_HARD_SendAsFrmPacketMsg
(
    VOS_UINT16       usPppId,
    VOS_UINT16       usProtocol,
    PPP_ZC_STRU     *pstMem
)
{
    HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU    *pstMsg;
    VOS_UINT32                               ulLength;


    ulLength    = sizeof(HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (HDLC_PROC_AS_FRM_PACKET_IND_MSG_STRU *)PS_ALLOC_MSG(PS_PID_PPP_HDLC, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        /*????????????---??????????????:*/
        PPP_HDLC_ERROR_LOG("PPP_HDLC_HARD_SendMsg, ERROR: PS_ALLOC_MSG Failed!\r\n");
        return VOS_ERR;
    }

    /*??????????:*/
    pstMsg->MsgHeader.ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulSenderPid     = PS_PID_PPP_HDLC;
    pstMsg->MsgHeader.ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->MsgHeader.ulReceiverPid   = PS_PID_PPP_HDLC;
    pstMsg->MsgHeader.ulLength        = ulLength;

    pstMsg->MsgHeader.ulMsgName       = PPP_HDLC_PROC_AS_FRM_PACKET_IND;
    /*??????????:*/
    pstMsg->usPppId                   = usPppId;
    pstMsg->usProtocol                = usProtocol;
    pstMsg->pstMem                    = pstMem;

    /*??????????*/
    if (VOS_OK != PS_SEND_MSG(PS_PID_PPP_HDLC, pstMsg))
    {
        /*????????????---????????????:*/
        PPP_HDLC_ERROR_LOG("PPP_HDLC_HARD_SendAsFrmPacketMsg, ERROR : PS_SEND_MSG Failed!");
        PPP_MemFree(pstMem);
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID PPP_HDLC_HARD_ProcAsFrmPacket
(
    VOS_UINT16       usPppId,
    VOS_UINT16       usProtocol,
    PPP_ZC_STRU     *pstMem
)
{
    VOS_UINT32              ulDealCurCnt;
    PPP_ZC_QUEUE_STRU       stDataQ;
    struct link            *pstLink;
    VOS_UINT32              ulDealResult;
    HDLC_PARA_LINK_BUILD_PARA_STRU      stBuildPara;

    pstLink = PPP_LINK(usPppId);

    ulDealCurCnt = 0;

    /* ??????????IP?????????? */
    PPP_ZC_SET_DATA_APP(pstMem, (VOS_UINT16)(usPppId << 8) | (VOS_UINT16)PPP_PUSH_PACKET_TYPE);

    /* ??????????stDataQ, ???? pstMem ???? */
    PPP_ZC_QUEUE_INIT(&stDataQ);
    PPP_ZC_ENQUEUE_TAIL(&stDataQ, pstMem);

    /* ??????????????????????????PC */
    stBuildPara.ucDataType = PPP_PUSH_PACKET_TYPE;
    stBuildPara.enPppMode  = PPP_SERVICE_HDLC_HARD_IP_MODE;
    stBuildPara.usProtocol = usProtocol;
    stBuildPara.usPppId    = usPppId;
    stBuildPara.pstDataQ   = &stDataQ;
    stBuildPara.pstLink    = pstLink;
    ulDealResult = PPP_HDLC_HARD_FrmPacket(&stBuildPara, &ulDealCurCnt);

    if ((VOS_OK != ulDealResult) || (1 != ulDealCurCnt))
    {
        PPP_HDLC_ERROR_LOG2("PPP_HDLC_HARD_MakeFrmPacket, ERROR, ulDealResult %d ulDealCurCnt %d!\r\n",
                      ulDealResult, ulDealCurCnt);

        /* ???????????????????????????????????????????????? */
        if (0 < PPP_ZC_GET_QUEUE_LEN(&stDataQ))
        {
            PPP_MemFree(pstMem);
        }

        return;
    }

    g_PppHdlcHardStat.ulFrmIpDataProcCnt++;

    return;
}


VOS_VOID PPP_HDLC_HARD_ProcProtocolPacket
(
    struct link     *pstLink,
    struct ppp_mbuf *pstMbuf,
    VOS_INT32        ulPri,
    VOS_UINT16       usProtocol
)
{
    PPP_ZC_STRU            *pstMem;
    VOS_UINT16              usPppId;
    PPP_SERVICE_HDLC_HARD_WORK_STATUS_ENUM_UINT8    enHdlcWorkStatus;


    usPppId = (VOS_UINT16)PPP_LINK_TO_ID(pstLink);

    pstMem  = ppp_m_alloc_ttfmem_from_mbuf(pstMbuf);

    ppp_m_freem(pstMbuf);
    pstMbuf  = VOS_NULL_PTR;

    /* for lint 438 */
    (VOS_VOID)pstMbuf;

    if (VOS_NULL_PTR == pstMem)
    {
        PPP_HDLC_WARNING_LOG("PPP_HDLC_HARD_ProcProtocolPacket, WARNING, pstMem is NULL!\r\n");
        return;
    }

    /* PPP????????????????????????????????PC????????????????????????????????????
       ??????????????????????????PPP?????????????????????????????????????? */
    enHdlcWorkStatus = PPP_Service_HdlcHardGetWorkStatus();
    if(PPP_SERVICE_HDLC_HARD_NOT_WORK != enHdlcWorkStatus)
    {
        PPP_HDLC_WARNING_LOG1("HDLC Hardware is working now", enHdlcWorkStatus);

        PPP_HDLC_HARD_SendAsFrmPacketMsg(usPppId, usProtocol, pstMem);

        return;
    }

    /* ????PPP???????????????????????????????????????????????????? */
    PPP_HDLC_HARD_ProcAsFrmPacket(usPppId, usProtocol, pstMem);

    return;
}


VOS_VOID PPP_HDLC_HARD_Write32Reg
(
    VOS_UINT_PTR                        ulAddr,
    VOS_UINT32                          ulValue
)
{
    writel(ulValue, (volatile unsigned int *)ulAddr);

    return;
}


VOS_UINT32 PPP_HDLC_HARD_Read32Reg(VOS_UINT_PTR ulAddr)
{
    VOS_UINT32                          ulValue;

    ulValue = readl((volatile unsigned int *)ulAddr);

    return ulValue;
}


VOS_UINT_PTR PPP_IoAddressMap(VOS_UINT_PTR ulAddr)
{
    VOS_UINT_PTR                        ulIoAddr;

    ulIoAddr = (VOS_UINT_PTR)ioremap(ulAddr,PPP_HDLC_MEM_MAP_SIZE);

    return ulIoAddr;
}


VOS_UINT32 PPP_HDLC_HARD_Init(VOS_VOID)
{
    PPP_HDLC_HARD_CONFIG_INFO_STRU              stHdlcConfig;
    PPP_SERVICE_HDLC_HARD_DEF_CALLBACK_STRU     stDefCBInfo;

    PSACORE_MEM_SET(&stHdlcConfig, sizeof(PPP_HDLC_HARD_CONFIG_INFO_STRU),
                 0x00, sizeof(PPP_HDLC_HARD_CONFIG_INFO_STRU));

    stHdlcConfig.ulUserPid          = PS_PID_APP_PPP;
    stHdlcConfig.ulPppTotalBufLen   = PAGE_ALIGN(TTF_HDLC_MASTER_LINK_TOTAL_LEN + PAGE_SIZE);
    stHdlcConfig.pMemSetFunc        = (PPP_HDLC_HARD_MEM_SET_FUNC)PSACORE_MEM_SET_EX;
    stHdlcConfig.pMemCpyFunc        = (PPP_HDLC_HARD_MEM_CPY_FUNC)PSACORE_MEM_CPY_EX;
    stHdlcConfig.pWrite32RegFunc    = PPP_HDLC_HARD_Write32Reg;
    stHdlcConfig.pRead32RegFunc     = PPP_HDLC_HARD_Read32Reg;
    stHdlcConfig.ulDefOneMaxSize    = PPP_HDLC_DEF_ONE_MAX_SIZE;
    stHdlcConfig.pMemPhy2VirtFunc   = (PPP_HDLC_HARD_MEM_CVT_FUNC)phys_to_virt;
    stHdlcConfig.pMemVirt2PhyFunc   = (PPP_HDLC_HARD_MEM_CVT_FUNC)virt_to_phys;
    stHdlcConfig.pBufPhy2VirtFunc   = VOS_UncacheMemPhyToVirt;
    stHdlcConfig.pBufVirt2PhyFunc   = VOS_UncacheMemVirtToPhy;
    stHdlcConfig.pIOAddrCvtFunc     = PPP_IoAddressMap;

    stDefCBInfo.pDefErrProcFunc = PPP_HDLC_HARD_DefProcErrorFrames;
    stDefCBInfo.pDefResProcFunc = PPP_HDLC_HARD_DefResultProc;

    if (VOS_OK != PPP_Service_HdlcHardInit(&stHdlcConfig, &stDefCBInfo))
    {
        return VOS_ERR;
    }

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
    g_stHdlcDev.dma_mask = &g_ullHdlcDmaMask;
    g_pstHdlcDev = &g_stHdlcDev;
#endif

    return VOS_OK;
}


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0))

VOS_INT32 PPP_HDLC_PlatDevProbe(struct platform_device *pstDev)
{
    g_pstHdlcDev = &(pstDev->dev);
    dma_set_mask_and_coherent(g_pstHdlcDev, DMA_BIT_MASK(64));//lint !e598 !e648

    return 0;
}


VOS_INT32 PPP_HDLC_PlatDevRemove(struct platform_device *pstDev)
{
    return 0;
}

static const struct of_device_id g_stHdlcPlatDevOfMatch[] = {
    {
        .compatible = "hisilicon,hisi-hdlc",
    },
    { },
};

static struct platform_driver g_stHdlcPlatDevDriver = {
    .probe  = PPP_HDLC_PlatDevProbe,
    .remove = PPP_HDLC_PlatDevRemove,
    .driver = {
        .name = "hisi-hdlc",
        .of_match_table = of_match_ptr(g_stHdlcPlatDevOfMatch),
    },
};


VOS_INT32 __init PPP_HDLC_PlatDevInit(void)
{
    return platform_driver_register(&g_stHdlcPlatDevDriver);
}


VOS_VOID __exit PPP_HDLC_PlatDevExit(void)
{
    platform_driver_unregister(&g_stHdlcPlatDevDriver);
}

#ifndef CONFIG_HISI_BALONG_MODEM_MODULE
module_init(PPP_HDLC_PlatDevInit);
module_exit(PPP_HDLC_PlatDevExit);
#endif
#endif



#endif  /* #if(FEATURE_ON == FEATURE_HARDWARE_HDLC_FUNC) */

#endif /* #if(FEATURE_ON == FEATURE_PPP) */

