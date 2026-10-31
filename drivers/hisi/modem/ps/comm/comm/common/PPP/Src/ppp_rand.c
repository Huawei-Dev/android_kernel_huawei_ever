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


/******************************************************************************
   1 ??????????
******************************************************************************/
#include "ppp_rand.h"

/*****************************************************************************
    ??????????????????????.C??????????
*****************************************************************************/
#define    THIS_FILE_ID                 PS_FILE_ID_PPP_RAND_C


/******************************************************************************
   2 ????????????????
******************************************************************************/

/******************************************************************************
   3 ????????
******************************************************************************/
/* ????????????????????????????????????????
         C0  C1  C2  C3              C0  C1  C2  C3
    R0 | 0 | 1 | 2 | 3 |        R0 | 3 | 6 | 9 | C |
       -----------------           -----------------
    R1 | 4 | 5 | 6 | 7 |        R1 | 7 | A | D | 0 |
       -----------------   ==>     -----------------
    R2 | 8 | 9 | A | B |        R2 | B | E | 1 | 4 |
       -----------------           -----------------
    R3 | C | D | E | F |        R3 | F | 2 | 5 | 8 |
*/
#define PPP_MATRIX_TRANSFORM(aucDst, aucSrc) \
{                               \
    aucDst[0] = aucSrc[0][3];   \
    aucDst[1] = aucSrc[1][2];   \
    aucDst[2] = aucSrc[2][1];   \
    aucDst[3] = aucSrc[3][0];   \
    aucDst[4] = aucSrc[1][3];   \
    aucDst[5] = aucSrc[2][2];   \
    aucDst[6] = aucSrc[3][1];   \
    aucDst[7] = aucSrc[0][0];   \
    aucDst[8] = aucSrc[2][3];   \
    aucDst[9] = aucSrc[3][2];   \
    aucDst[10] = aucSrc[0][1];  \
    aucDst[11] = aucSrc[1][0];  \
    aucDst[12] = aucSrc[3][3];  \
    aucDst[13] = aucSrc[0][2];  \
    aucDst[14] = aucSrc[1][1];  \
    aucDst[15] = aucSrc[2][0];  \
}


/******************************************************************************
   4 ????????????
******************************************************************************/

/******************************************************************************
   5 ????????
******************************************************************************/

VOS_UINT32 PPP_GenerateSeed
(
    VOS_UINT8                           aucSerial[],
    VOS_UINT8                           ucSerialLen,
    VOS_UINT32                          ulStartIndex,
    VOS_UINT32                          ulDisturbaceFactor
)
{

    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulSeed;
    VOS_UINT8                          *pucSeed;
    VOS_UINT8                           ucTempValue = 0xFF;


    /* ?????????????????????????????? */
    pucSeed = (VOS_UINT8 *)&ulSeed;
    for ( ulLoop = 0; ulLoop < sizeof(ulSeed); ulLoop++ )
    {
        *pucSeed++ = aucSerial[(VOS_UINT8)((ulStartIndex + ulLoop) % ucSerialLen)];
    }

    /* ???? */
    ulSeed += ulDisturbaceFactor;

    /* ?????????? */
    pucSeed = (VOS_UINT8 *)&ulSeed;
    for ( ulLoop = 0; ulLoop < sizeof(ulSeed); ulLoop++ )
    {
        if (0 == *pucSeed)
        {
            *pucSeed = ucTempValue--;
        }
        pucSeed++;
    }

    return ulSeed;
}


VOS_VOID PPP_Get16ByteSerial
(
    VOS_UINT8                           aucSerial[]
)
{
    VOS_UINT8                           aucSrcSerial[4][4];
    VOS_UINT32                          ulTick      = VOS_GetTick();
    VOS_UINT32                          ulSlice     = VOS_GetSlice();
    VOS_UINT32                          ulTaskId    = VOS_GetCurrentTaskID();
    VOS_UINT32                          ulStatSum   = PPP_RAND_GET_STAT_SUM();

    /* ??????????????0???????????????? */
    ulTick      = (0x00UL != ulTick) ? ulTick : 0x12345678;
    ulSlice     = (0x00UL != ulSlice) ? ulSlice : 0x9ABCDEF1;
    ulTaskId    = (0x00UL != ulTaskId) ? ulTaskId : 0xFDB97531;
    ulStatSum   = (0x00UL != ulStatSum) ? ulStatSum : 0xECA8642F;

    /* ???????????????????????? */
    *((VOS_UINT32*)&aucSrcSerial[0][0])     = ulTick;
    *((VOS_UINT32*)&aucSrcSerial[1][0])     = ulSlice;
    *((VOS_UINT32*)&aucSrcSerial[2][0])     = ulTaskId;
    *((VOS_UINT32*)&aucSrcSerial[3][0])     = ulStatSum;

    /* ????????????????????????????*/
    PPP_MATRIX_TRANSFORM(aucSerial, aucSrcSerial);

    return;
}


VOS_VOID PPP_GetSecurityRand
(
    VOS_UINT8                           ucRandByteLen,
    VOS_UINT8                          *pucRand
)
{
    VOS_UINT8                           aucDictionary[256] = {0};
    VOS_UINT8                           aucSerial[16] = {0};
    VOS_UINT8                           ucValue = 0;
    VOS_UINT8                           ucTempValue;
    VOS_UINT32                          ulSeed;
    VOS_UINT32                          ulRandIndex;
    VOS_UINT32                          ulSwapIndex;
    VOS_UINT32                          ulLoop;
    VOS_UINT32                          ulStart;
    VOS_UINT32                          ulRandSum;
    const VOS_UINT32                    ulMax = 0x100;/* ??????????????, ????????????????[0x00..0xFF] */


    /***************************************************************************
     ??????????????????????????????????????????????????????????????????????????
     ??????????????????????????????????????????????????????????????????????????
     ??????????????????????????????????????????????????????????????????????????
     ??????????????????????????????????????????????????????????????????????????
     ????????????????????TICK??????????????????????????????????/????ID????????
     ??????????????????????????????????????????????????????????????????????????
     ????????????????????
    ****************************************************************************/

    /***************************************************************************
     ??????????????????????:
     1??????????
     2??????????????
    ****************************************************************************/

    /* ????16byte???????? */
    PPP_Get16ByteSerial(aucSerial);

    /* ???????? */
    ulSeed = PPP_GenerateSeed(aucSerial, (VOS_UINT8)sizeof(aucSerial), 0, 0);
    VOS_SetSeed(ulSeed);

    /***************************************************************************
     1??????????
    ****************************************************************************/
    /* a??????????????????, ??????????:[0..0xFF] */
    ulStart = VOS_Rand(ulMax);
    ulRandSum = ulStart;

    /* b????????????????????: [ulStart??0xFF] */
    for (ulLoop = ulStart; ulLoop < ulMax; ulLoop++)
    {
        aucDictionary[ulLoop] = ucValue;
        ucValue++;
    }

    /* c????????????????????: [0, ulStart) */
    for (ulLoop = 0; ulLoop < ulStart; ulLoop++)
    {
        aucDictionary[ulLoop] = ucValue;
        ucValue++;
    }

    /***************************************************************************
     2??????????????
       ????ucRandByteLen??????????????????????????ulLoop = 0??????
       ????????????????(ulLoop??[ulLoop, 0xFF]????)????????????????
    ****************************************************************************/
    for (ulLoop = 0; ulLoop < ucRandByteLen; ulLoop++)
    {
        /* ???????? */
        ulSeed = PPP_GenerateSeed(aucSerial, (VOS_UINT8)sizeof(aucSerial), ulLoop, ulRandSum);
        VOS_SetSeed(ulSeed);

        /* ??????????, ??????????:[0..0xFF] */
        ulRandIndex                 = VOS_Rand(ulMax);
        ulRandSum                  += ulRandIndex;

        /* ????????????????????:[ulLoop..0xFF] */
        ulSwapIndex                 = (ulRandIndex % (ulMax - ulLoop)) + ulLoop;

        /* ????aucDictionary[ulLoop]??aucDictionary[ulSwapIndex] */
        ucTempValue                 = aucDictionary[ulLoop];
        aucDictionary[ulLoop]       = aucDictionary[ulSwapIndex];
        aucDictionary[ulSwapIndex]  = ucTempValue;
    }

    /* ????ucRandByteLen???????????? */
    PPP_RAND_MEMCPY_S(pucRand, ucRandByteLen, &(aucDictionary[0]), ucRandByteLen);

    return;
}



