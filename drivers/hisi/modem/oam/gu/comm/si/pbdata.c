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

/************************************************************************
  Copyright    : 2005-2007, Huawei Tech. Co., Ltd.
  File name    : pbdata.c
  Author       : zhuli
  Version      : V100R002
  Date         : 2008-5-15
  Description  : ??C??????????---????????????????????????
  Function List:
  History      :
 ************************************************************************/

#include "si_pb.h"
#include "TafTypeDef.h"
#include "UsimPsInterface.h"
#include "NVIM_Interface.h"
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#include "usimmbase.h"
#endif

/*****************************************************************************
    ??????????????????????.C??????????
*****************************************************************************/
#define    THIS_FILE_ID PS_FILE_ID_PB_DATA_C


#if (FEATURE_OFF == FEATURE_PHONE_SC)


VOS_UINT32 SI_PB_FindPBOffset(SI_PB_TYPE_ENUM_UINT32  enPBType, VOS_UINT8 *pucOffset)
{
    VOS_UINT8 i;

    for(i=0; i<SI_PB_MAX_NUMBER; i++)
    {
        if(gastPBContent[i].enPBType == enPBType)   /*??????????????????*/
        {
            PB_INFO_LOG("SI_PB_FindPBOffset Info: Locate the PhoneBook Accurately");

            *pucOffset = i;

            return VOS_OK;
        }
    }

    PB_ERROR_LOG("SI_PB_FindPBOffset Error: The PhoneBook Info is Not Exist");

    return VOS_ERR;             /*??????????????*/
}



VOS_UINT32 SI_PB_LocateRecord(SI_PB_TYPE_ENUM_UINT32  enPBType, VOS_UINT16 usIndex1, VOS_UINT16 usIndex2, VOS_UINT8 *pucNumber)
{
    VOS_UINT8   ucPBOffset;
    VOS_UINT32 ulResult;

    ulResult = SI_PB_FindPBOffset(enPBType, &ucPBOffset);    /*??????????????????????*/

    if(VOS_ERR == ulResult)
    {
        PB_WARNING_LOG("SI_PB_LocateRecord: Locate PhoneBook Error");

        return TAF_ERR_UNSPECIFIED_ERROR;
    }

    if(PB_NOT_INITIALISED == gastPBContent[ucPBOffset].enInitialState)   /*??????????????????????????*/
    {
        PB_ERROR_LOG("SI_PB_LocateRecord:The PhoneBook is Not Initializtion");

        return TAF_ERR_SIM_BUSY;
    }

    if(PB_FILE_NOT_EXIST == gastPBContent[ucPBOffset].enInitialState)   /*??????????????????????????*/
    {
        PB_ERROR_LOG("SI_PB_LocateRecord:The PhoneBook is Not Exit");

        return TAF_ERR_FILE_NOT_EXIST;
    }

    if((usIndex1 > gastPBContent[ucPBOffset].usTotalNum)
        || (usIndex2 > gastPBContent[ucPBOffset].usTotalNum)
        || (usIndex1 > usIndex2))/*??????????????????????*/
    {
        PB_WARNING_LOG("SI_PB_LocateRecord: The Index is Not in The Range of PhoneBook");

        return TAF_ERR_PB_WRONG_INDEX;
    }

    *pucNumber = ucPBOffset;        /*????????????????????????*/

    return VOS_OK;
}


VOS_UINT32 SI_PB_CountADNRecordNum(VOS_UINT16 usIndex, VOS_UINT16 *pusFileId, VOS_UINT8 *pucRecordNum)
{
    VOS_UINT8 i;
    VOS_UINT16 usTemp = 0;

    for(i=0; i<SI_PB_ADNMAX; i++)   /*??????????ADN????????????*/
    {
        if((usTemp < usIndex)&&(usIndex <= (gstPBCtrlInfo.astADNInfo[i].ucRecordNum + usTemp)))/*????????????????????*/
        {
            *pusFileId = gstPBCtrlInfo.astADNInfo[i].usFileID;  /*??????????????ID*/
            *pucRecordNum = (VOS_UINT8)(usIndex - usTemp);/*????????????????*/

            return VOS_OK;
        }
        else
        {
            usTemp += gstPBCtrlInfo.astADNInfo[i].ucRecordNum;  /*??????????*/
        }
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GetXDNFileID(VOS_UINT32 ulStorage, VOS_UINT16 *pusFileId)
{
    VOS_UINT32 ulResult = VOS_OK;

    switch(ulStorage)
    {
        case SI_PB_STORAGE_ON:
            *pusFileId = EFMSISDN;
            break;
        case SI_PB_STORAGE_FD:
            *pusFileId = EFFDN;
            break;
        case SI_PB_STORAGE_BD:
            *pusFileId = EFBDN;
            break;
        default:
            ulResult = VOS_ERR;
    }

    return ulResult;
}


VOS_VOID SI_PB_BcdToAscii(VOS_UINT8 ucBcdNumLen,VOS_UINT8 *pucBcdNum, VOS_UINT8 *pucAsciiNum, VOS_UINT8 *pucLen)
{

    VOS_UINT8       ucTmp;
    VOS_UINT8       ucLen = 0;
    VOS_UINT8       ucFirstNumber;
    VOS_UINT8       ucSecondNumber;

    for (ucTmp = 0; ucTmp < ucBcdNumLen; ucTmp++)
    {

        if(0xFF == pucBcdNum[ucTmp])
        {
            break;
        }

        ucFirstNumber  = (VOS_UINT8)(pucBcdNum[ucTmp] & 0x0F); /*????????????*/

        ucSecondNumber = (VOS_UINT8)((pucBcdNum[ucTmp] >> 4) & 0x0F);/*????????????*/

        if(ucFirstNumber <= 9)  /*????????*/
        {
            *pucAsciiNum = ucFirstNumber + 0x30;

            pucAsciiNum++;

            ucLen++;
        }
        else if(0x0A == ucFirstNumber)   /*????*????*/
        {
            *pucAsciiNum = 0x2a;

            pucAsciiNum++;

            ucLen++;
        }
        else if(0x0B == ucFirstNumber)   /*????#????*/
        {
            *pucAsciiNum = 0x23;

            pucAsciiNum++;

            ucLen++;
        }
        else if(0x0C == ucFirstNumber)   /*????'P'????*/
        {
            *pucAsciiNum = 0x50;

            pucAsciiNum++;

            ucLen++;
        }
        else if(0x0D == ucFirstNumber)   /*????'?'????*/
        {
            *pucAsciiNum = 0x3F;

            pucAsciiNum++;

            ucLen++;
        }
        else                                    /*????????*/
        {
            *pucAsciiNum = ucFirstNumber + 0x57;

            pucAsciiNum++;

            ucLen++;
        }

        if(0x0F == ucSecondNumber)
        {
            break;
        }
        if(ucSecondNumber <= 9)
        {
            *pucAsciiNum = ucSecondNumber + 0x30;

            pucAsciiNum++;

            ucLen++;
        }
        else if(0x0A == ucSecondNumber)
        {
            *pucAsciiNum = 0x2a;

            pucAsciiNum++;

            ucLen++;
        }
        else if(0x0B == ucSecondNumber)
        {
            *pucAsciiNum = 0x23;

            pucAsciiNum++;

            ucLen++;
        }
        else if(0x0C == ucSecondNumber)   /*????'P'????*/
        {
            *pucAsciiNum = 0x50;

            pucAsciiNum++;

            ucLen++;
        }
        else if(0x0D == ucSecondNumber)   /*????'?'????*/
        {
            *pucAsciiNum = 0x3F;

            pucAsciiNum++;

            ucLen++;
        }
        else
        {
            *pucAsciiNum = ucSecondNumber + 0x57;

            pucAsciiNum++;

            ucLen++;
        }
    }

    *pucLen = ucLen;

    return;
}


VOS_VOID SI_PB_DecodePBName(VOS_UINT8 ucNameMax, VOS_UINT8 *pucName,
                                        VOS_UINT32 *pAlphaType, VOS_UINT8 *pNameLen)
{
    VOS_UINT8 i = 0;

    /* ??????UCS2 80 */
    if (SI_PB_ALPHATAG_TYPE_UCS2_80 == pucName[0])
    {
        *pAlphaType = pucName[0];

        ucNameMax--;
        for(i=0;i<(ucNameMax-(ucNameMax%2));i+=2)   /*??????????????????*/
        {
            if((pucName[i+1] == 0xFF)&&(pucName[i+2] == 0xFF))
            {
                break;
            }
        }

        *pNameLen = i;      /*????????????*/

        return;
    }

    /* ??????UCS2 81 */
    if (SI_PB_ALPHATAG_TYPE_UCS2_81 == pucName[0])
    {
        *pAlphaType = pucName[0];

        if(pucName[1] > (ucNameMax - 3))/* ?????????????????????????????????????????? */
        {
           i = ucNameMax -1;
        }
        else
        {
           i = pucName[1] + 2;     /*????????????????*/
        }

        *pNameLen = i;      /*????????????*/

        return;
    }

    /* ??????UCS2 82 */
    if (SI_PB_ALPHATAG_TYPE_UCS2_82 == pucName[0])
    {
        *pAlphaType = pucName[0];

        if(pucName[1] > (ucNameMax - 4))/* ?????????????????????????????????????????? */
        {
           i = ucNameMax -1;
        }
        else
        {
           i = pucName[1] + 3;     /*????????????????*/
        }

        *pNameLen = i;      /*????????????*/

        return;
    }


    /* ??????GSM???????? */
    *pAlphaType = SI_PB_ALPHATAG_TYPE_GSM;

    for (i = 0; i < ucNameMax; i++)         /*??????????????????*/
    {
        if (0xFF == pucName[i])
        {
            break;
        }
    }

    *pNameLen = i;      /*????????????*/

    return;
}


VOS_VOID SI_PB_TransPBFromate(SI_PB_CONTENT_STRU *pstPBContent, VOS_UINT16 usIndex, VOS_UINT8 *pContent, SI_PB_RECORD_STRU *pstRecord)
{
    VOS_UINT32 ulResult;
    VOS_UINT32 ulExtInfoNum;
    VOS_UINT8  ucExtRecord;
    VOS_UINT8  aucPhoneNumber[SI_PB_NUM_LEN] = {0};
    VOS_UINT8  *pucExtContent;
    VOS_UINT8  ucExtNumLen;
    VOS_UINT8  ucXdnNumLen;

    ulResult = SI_PB_CheckContentValidity(pstPBContent, pContent);/*??????????????????????*/

    if(ulResult != VOS_OK)
    {
        pstRecord->ValidFlag = SI_PB_CONTENT_INVALID;     /*??????????????????*/

        /*??????0*/
    }
    else
    {
        pstRecord->ValidFlag = SI_PB_CONTENT_VALID;/*??????????????????*/

        SI_PB_DecodePBName(pstPBContent->ucNameLen, pContent,
                            &pstRecord->AlphaTagType,&pstRecord->ucAlphaTagLength);

        if(pstRecord->ucAlphaTagLength != 0x00)         /*??????????????*/
        {
            if(pstRecord->AlphaTagType == SI_PB_ALPHATAG_TYPE_GSM)  /*????????????????????*/
            {
                PAM_MEM_CPY_S(pstRecord->AlphaTag, SI_PB_ALPHATAG_MAX_LEN, pContent, pstRecord->ucAlphaTagLength);
            }
            else                                                                                        /*????????????????????????????*/
            {
                /*ucs2????????????????????????*/
                PAM_MEM_CPY_S(pstRecord->AlphaTag, SI_PB_ALPHATAG_MAX_LEN, pContent+1, pstRecord->ucAlphaTagLength);
            }
        }

        if ((pContent[pstPBContent->ucNameLen+1] == 0xFF)
            && (pContent[pstPBContent->ucNameLen] == 0x0))
        {
            pstRecord->NumberType   =  PB_NUMBER_TYPE_NORMAL;
        }
        else
        {
            pstRecord->NumberType   =  pContent[pstPBContent->ucNameLen+1];
        }

        ucExtRecord = ((PB_BDN == pstPBContent->enPBType)?\
                        pContent[pstPBContent->ucRecordLen-2]:\
                        pContent[pstPBContent->ucRecordLen-1]);

        ulExtInfoNum  = pstPBContent->ulExtInfoNum;
        pucExtContent = gastEXTContent[ulExtInfoNum].pExtContent;

        ucExtRecord = ((VOS_NULL_PTR == pucExtContent) ? 0xFF : (ucExtRecord));

        if(pContent[pstPBContent->ucNameLen] < 2)           /*????????????*/
        {
            pstRecord->NumberLength = 0;
        }
        else if((0xFF != ucExtRecord)&&(0 != ucExtRecord)&&(VOS_NULL_PTR != pucExtContent)
                &&(ucExtRecord <= gastEXTContent[ulExtInfoNum].usExtTotalNum))
        {
            pucExtContent += (ucExtRecord - 1) * SI_PB_EXT_LEN;
            ucExtNumLen   = (*(pucExtContent + 1) > (SI_PB_NUM_LEN/2))?
                            (SI_PB_NUM_LEN/2) : *(pucExtContent + 1);

            PAM_MEM_CPY_S(aucPhoneNumber, SI_PB_NUM_LEN/2, pContent+(pstPBContent->ucNameLen+2), SI_PB_NUM_LEN/2 );
            PAM_MEM_CPY_S(aucPhoneNumber + (SI_PB_NUM_LEN/2), SI_PB_NUM_LEN/2, pucExtContent + 2, SI_PB_NUM_LEN/2 );

            SI_PB_BcdToAscii((VOS_UINT8)(ucExtNumLen + (SI_PB_NUM_LEN/2)), aucPhoneNumber,
                                pstRecord->Number, &pstRecord->NumberLength);
        }
        else                                                                                /*??????????????????*/
        {
            ucXdnNumLen = ((pContent[pstPBContent->ucNameLen]-1) > (SI_PB_NUM_LEN/2))?
                           (SI_PB_NUM_LEN/2) : (pContent[pstPBContent->ucNameLen]-1);

            SI_PB_BcdToAscii(ucXdnNumLen, &pContent[pstPBContent->ucNameLen+2],
                            pstRecord->Number, &pstRecord->NumberLength);
        }
    }

    pstRecord->Index = usIndex;

    return ;
}


VOS_VOID SI_PB_InitContent(VOS_UINT8 ucLen, VOS_UINT8 ucValue,VOS_UINT8 *pucMem)
{
    VOS_UINT8 i;

    for(i=0; i<ucLen; i++)
    {
        pucMem[i] = ucValue;
    }

    return;
}


VOS_UINT32 SI_PB_GetBitFromBuf(VOS_UINT8 *pucDataBuf, VOS_UINT32 ulBitNo)
{
    VOS_UINT32  ulOffset;
    VOS_UINT8   ucBit;

    if(VOS_NULL_PTR == pucDataBuf)
    {
        PB_ERROR_LOG("SI_PB_GetBitFromBuf: Input Null Ptr");

        return VOS_FALSE;
    }

    ulOffset = (ulBitNo - 1)/ 8;

    ucBit    = (VOS_UINT8)((ulBitNo - 1)%8);

    return (pucDataBuf[ulOffset]>>ucBit)&0x1;
}


VOS_UINT32 SI_PB_CheckEccValidity(VOS_UINT8 *pContent)
{
    if (VOS_NULL_PTR == pContent)
    {
        PB_ERROR_LOG("SI_PB_CheckEccValidity: Input NULL PTR");

        return VOS_ERR;
    }

    if( 0xFF == pContent[0])
    {
        PB_INFO_LOG("SI_PB_CheckEccValidity: The ECC is Empty");

        return VOS_ERR;
    }
    else
    {
        PB_INFO_LOG("SI_PB_CheckEccValidity: The ECC is Not Empty");

        return VOS_OK;
    }
}


VOS_UINT32 SI_PB_CheckContentValidity(SI_PB_CONTENT_STRU *pstPBContent, VOS_UINT8 *pContent)
{
    if ((VOS_NULL_PTR == pContent)||(VOS_NULL_PTR == pstPBContent))
    {
        PB_ERROR_LOG("SI_PB_CheckContentValidity: Input NULL PTR");

        return VOS_ERR;
    }

    if(((pContent[pstPBContent->ucNameLen] == 0)||(pContent[pstPBContent->ucNameLen] == 0xFF))
        &&(pContent[0] == 0xFF))/*??????????????????????*/
    {
        PB_INFO_LOG("SI_PB_CheckContentValidity: The PhoneBook Content is Empty");

        return VOS_ERR;
    }
    else
    {
        PB_INFO_LOG("SI_PB_CheckContentValidity: The PhoneBook Content is Not Empty");

        return VOS_OK;
    }
}


VOS_UINT32 SI_PB_CheckANRValidity(VOS_UINT8 *pContent)
{
    if (VOS_NULL_PTR == pContent)
    {
        PB_ERROR_LOG("SI_PB_CheckANRValidity: Input NULL PTR");

        return VOS_ERR;
    }

    if((0xFF == pContent[0])||(0xFF == pContent[1])||(0 == pContent[1]))/*????????????????*/
    {
        PB_INFO_LOG("SI_PB_CheckANRValidity: The PhoneBook Content is Empty");

        return VOS_ERR;
    }
    else
    {
        PB_INFO_LOG("SI_PB_CheckANRValidity: The PhoneBook Content is Not Empty");

        return VOS_OK;
    }
}


#if ((OSA_CPU_CCPU == VOS_OSA_CPU)||(defined(DMT)))

VOS_UINT32 SI_PB_CountADNIndex(VOS_UINT16 usFileId, VOS_UINT8 ucRecordNum, VOS_UINT16 *pusIndex)
{
    VOS_UINT8 i;
    VOS_UINT16 usIndex = 0;

    for(i=0; i<SI_PB_ADNMAX; i++)       /*??????????ADN ????????????????*/
    {
        if(usFileId == gstPBCtrlInfo.astADNInfo[i].usFileID)/*??????????????ID ????????????*/
        {
            *pusIndex = usIndex + ucRecordNum;      /*????????????????????????*/

            return VOS_OK;
        }
        else
        {
            usIndex += gstPBCtrlInfo.astADNInfo[i].ucRecordNum;/*??????????????*/
        }
    }

    return VOS_ERR;             /*????????????????????????*/
}


VOS_UINT32 SI_PB_GetADNSfi(VOS_UINT8 *pucSFI, VOS_UINT16 usFileId)
{
    VOS_UINT32                          i;

    for(i = 0; i < gstPBCtrlInfo.ulADNFileNum; i++)   /*??????????ADN????????????*/
    {
        if(usFileId == gstPBCtrlInfo.astADNInfo[i].usFileID)
        {
            *pucSFI = gstPBCtrlInfo.astADNInfo[i].ucSFI;

            return VOS_OK;
        }
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_CheckADNFileID(VOS_UINT16 usFileID)
{
    VOS_UINT8 j;

    for(j=0; j<SI_PB_ADNMAX; j++)/*????????ID??????????ADN??????*/
    {
        if(usFileID == gstPBCtrlInfo.astADNInfo[j].usFileID)
        {
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GetXDNPBType(VOS_UINT32 *pulPBType, VOS_UINT16 usFileId)
{
    VOS_UINT32 ulResult = VOS_OK;

    switch(usFileId)
    {
        case EFFDN:
            *pulPBType = PB_FDN_CONTENT;
            break;
        case EFBDN:
            *pulPBType = PB_BDN_CONTENT;
            break;
        case EFMSISDN:
            *pulPBType = PB_MSISDN_CONTENT;
            break;
        case EFSDN:
            *pulPBType = PB_SDN_CONTENT;
            break;
        default:
            ulResult = SI_PB_CheckADNFileID(usFileId);/*??????????ADN??FID*/

            if ( VOS_OK == ulResult )
            {
                *pulPBType = PB_ADN_CONTENT;
            }
            else
            {
                ulResult = VOS_ERR;
            }
            break;
    }

    return ulResult;
}


VOS_UINT32 SI_PB_GetANRFid(VOS_UINT32 ulANRFileNum,VOS_UINT16 *pusANRFileId)
{
    VOS_UINT32 i;
    VOS_UINT32 j;
    VOS_UINT32 ulFileCount = 0;

    for(i = 0; i < gstPBCtrlInfo.ulADNFileNum; i++)
    {
        for(j = 0; j < SI_PB_ANRMAX; j++)
        {
            if (SI_PB_FILE_NOT_EXIST != gstPBCtrlInfo.astANRInfo[i][j].usANRFileID)
            {
                ulFileCount++;
            }

            if (ulANRFileNum == ulFileCount)
            {
               *pusANRFileId = gstPBCtrlInfo.astANRInfo[i][j].usANRFileID;

               return VOS_OK;
            }
        }
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GetANRSuffix(VOS_UINT8 *pucXSuffix,VOS_UINT8 *pucYSuffix
                                      ,VOS_UINT16 usANRFileId)
{
    VOS_UINT8                           i;
    VOS_UINT8                           j;
    VOS_UINT32                          ulADNFileNum;

    ulADNFileNum = (gstPBCtrlInfo.ulADNFileNum > SI_PB_ADNMAX)? SI_PB_ADNMAX : gstPBCtrlInfo.ulADNFileNum;

    for(i = 0; i < ulADNFileNum; i++)
    {
        for(j = 0; j < SI_PB_ANRMAX; j++)
        {
            if (usANRFileId == gstPBCtrlInfo.astANRInfo[i][j].usANRFileID)
            {
                *pucXSuffix = i;
                *pucYSuffix = j;

                return VOS_OK;
            }
        }
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GetIAPFidFromANR(VOS_UINT16 usANRFileId, VOS_UINT16 *pusIAPFileId)
{
    VOS_UINT32 i;
    VOS_UINT32 j;

    for(i = 0; i < gstPBCtrlInfo.ulADNFileNum; i++)
    {
        for(j = 0; j < SI_PB_ANRMAX; j++)
        {
            if(usANRFileId == gstPBCtrlInfo.astANRInfo[i][j].usANRFileID)
            {
                *pusIAPFileId = gstPBCtrlInfo.astIAPInfo[i].usIAPFileID;

                return VOS_OK;
            }
        }
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GetIAPFidFromEML(VOS_UINT16 usEMLFileId, VOS_UINT16 *pusIAPFileId)
{
    VOS_UINT32                          i;

    for(i = 0; i < gstPBCtrlInfo.ulEMLFileNum; i++)
    {
        if (usEMLFileId == gstPBCtrlInfo.astEMLInfo[i].usEMLFileID)
        {
            *pusIAPFileId = gstPBCtrlInfo.astIAPInfo[i].usIAPFileID;

            return VOS_OK;
        }
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GheckANRLast(VOS_UINT8 ucXSuffix,VOS_UINT8 ucYSuffix)
{
    if ((ucXSuffix+1) == (VOS_UINT8)gstPBCtrlInfo.ulADNFileNum)
    {
        return VOS_OK;
    }

    if (SI_PB_FILE_NOT_EXIST == gstPBCtrlInfo.astANRInfo[ucXSuffix+1][ucYSuffix].usANRFileID)
    {
        return VOS_OK;
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GetEMLFIdFromADN(VOS_UINT16 *pusEMLFileId, VOS_UINT16 usADNId)
{
    VOS_UINT32 i;

    for (i = 0; i < gstPBCtrlInfo.ulEMLFileNum; i++)
    {
        if(usADNId == gstPBCtrlInfo.astADNInfo[i].usFileID)
        {
            *pusEMLFileId = gstPBCtrlInfo.astEMLInfo[i].usEMLFileID;

            return VOS_OK;
        }
    }

    PB_NORMAL1_LOG("SI_PB_GetEMLFIdFromADN: Get ADN %d Email Error", usADNId);

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GetANRFidFromADN(VOS_UINT8 ucANROffset, VOS_UINT16 usADNFileId,VOS_UINT16 *pusANRFileId)
{
    VOS_UINT32 i;

    for(i = 0; i < gstPBCtrlInfo.ulADNFileNum; i++)
    {
        if(usADNFileId == gstPBCtrlInfo.astADNInfo[i].usFileID)
        {
            *pusANRFileId = gstPBCtrlInfo.astANRInfo[i][ucANROffset].usANRFileID;

            return VOS_OK;
        }

    }

    return VOS_ERR;
}

VOS_UINT32 SI_PB_GetFreeANRRecordNum(VOS_UINT16 usANRFid,VOS_UINT8 *pucRecordNum, VOS_UINT16 *pusIAPFid)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 j = 0;
    VOS_UINT32 k = 0;
    VOS_UINT32 ulOffset = 0;
    VOS_UINT32 ulFlag = VOS_FALSE;

    for(i = 0; i < gstPBCtrlInfo.ulADNFileNum; i++)   /*??????????ADN????????????*/
    {
        for(j = 0; j < SI_PB_ANRMAX; j++)
        {
            if(usANRFid == gstPBCtrlInfo.astANRInfo[i][j].usANRFileID)
            {
                *pusIAPFid = gstPBCtrlInfo.astIAPInfo[i].usIAPFileID;

                ulFlag = VOS_TRUE;

                break;
            }

        }

        if(VOS_TRUE == ulFlag)
        {
            break;
        }

        ulOffset += (gstPBCtrlInfo.astANRInfo[i][0].ucRecordNum*gstPBCtrlInfo.astANRInfo[i][0].ucRecordLen);
    }

    if ((i == gstPBCtrlInfo.ulADNFileNum) || (SI_PB_ANRMAX  <= j))
    {
        return VOS_ERR;
    }

    for(k = 0; k < gstPBCtrlInfo.astANRInfo[i][j].ucRecordNum; k++)
    {
        if(VOS_OK != SI_PB_CheckANRValidity(&gastANRContent[j%SI_PB_ANRMAX].pContent[ulOffset+(k*gastANRContent[j%SI_PB_ANRMAX].ucRecordLen)]))
        {
            *pucRecordNum = (VOS_UINT8)(k+1);
            return VOS_OK;
        }
    }

    return VOS_ERR;
}

VOS_UINT32 SI_PB_GetFreeEMLRecordNum(VOS_UINT16 ucEMLFid,VOS_UINT8 *pucRecordNum, VOS_UINT16 *pusIAPFid)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 j = 0;
    VOS_UINT32 ulOffset = 0;

    for(i = 0; i < gstPBCtrlInfo.ulEMLFileNum; i++)   /*??????????ADN????????????*/
    {
        if(ucEMLFid == gstPBCtrlInfo.astEMLInfo[i].usEMLFileID)
        {
            *pusIAPFid = gstPBCtrlInfo.astIAPInfo[i].usIAPFileID;
            break;
        }

        ulOffset += (gstPBCtrlInfo.astEMLInfo[i].ucRecordNum*gstPBCtrlInfo.astEMLInfo[i].ucRecordLen);
    }

    if(i == gstPBCtrlInfo.ulEMLFileNum)
    {
        return VOS_ERR;
    }

    for(j = 0; j < gstPBCtrlInfo.astEMLInfo[i].ucRecordNum;j++)
    {
         if(0xFF == gstEMLContent.pContent[ulOffset+(j*gstEMLContent.ucRecordLen)])
         {
            *pucRecordNum = (VOS_UINT8)(j+1);
            return VOS_OK;
         }
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GetEMLRecord(VOS_UINT8 **ppEMLContent, VOS_UINT16 usEMLFileId,VOS_UINT8 ucRecordNum, VOS_UINT32 *pulFreeSize)
{
    VOS_UINT32 i = 0;
    VOS_UINT16 usOffset = 0;
    VOS_UINT32 ulSrcOffset = 0;

    for(i = 0; i < gstPBCtrlInfo.ulEMLFileNum; i++)
    {
        if(usEMLFileId == gstPBCtrlInfo.astEMLInfo[i].usEMLFileID)
        {
            break;
        }
    }

    if((i >= gstPBCtrlInfo.ulEMLFileNum)
        ||(VOS_NULL_PTR == gstEMLContent.pContent))
    {
        return VOS_ERR;
    }

    usOffset = (VOS_UINT16)((i*gstPBCtrlInfo.astEMLInfo[i].ucRecordNum) + ucRecordNum);
    ulSrcOffset = (usOffset-1)*gstEMLContent.ucRecordLen;

    if (gstEMLContent.ulContentSize < ulSrcOffset)
    {
        return VOS_ERR;
    }

    *ppEMLContent = &gstEMLContent.pContent[ulSrcOffset];
    *pulFreeSize  = gstEMLContent.ulContentSize - ulSrcOffset;

    return VOS_OK;

}


VOS_UINT32  SI_PB_FindUnusedExtRecord(SI_EXT_CONTENT_STRU *pstEXTContent,
                                                    VOS_UINT8 *pucRecord,
                                                    VOS_UINT8 ucRecordCount)
{
    VOS_UINT32                          ucCountFreeRec = 1;
    VOS_UINT8                          *pContent;
    VOS_UINT8                           i;

    /* ????????????????????????????????????????????????????EXT???? */
    if( pstEXTContent->usExtTotalNum == pstEXTContent->usExtUsedNum )
    {
        *pucRecord = 0xFF;

         return VOS_ERR;
    }

    pContent = pstEXTContent->pExtContent;

    if(VOS_NULL_PTR == pContent)
    {
        *pucRecord = 0xFF;

         return VOS_ERR;
    }

    if (0xFF < pstEXTContent->usExtTotalNum)
    {
        PB_WARNING1_LOG("SI_PB_FindUnusedExtRecord: pstEXTContent->usExtTotalNum err",  pstEXTContent->usExtTotalNum);

        return VOS_ERR;
    }

    /* ????EXT?????????????????????????????????? */
    for( i = 0 ; i < pstEXTContent->usExtTotalNum ; i++ )
    {
        if( 0xFF == pContent[1] )
        {
            if(ucCountFreeRec == ucRecordCount)
            {
                break;
            }
            else
            {
                ucCountFreeRec++;
            }
        }

        pContent += SI_PB_EXT_LEN;
    }

    if( i < pstEXTContent->usExtTotalNum )
    {
        *pucRecord = i + 1;

        return VOS_OK;
    }
    else
    {
        *pucRecord = 0xFF;

        return VOS_ERR;
    }
}


VOS_VOID SI_PB_AsciiToBcd(VOS_UINT8 *pucAsciiNum, VOS_UINT8 ucAsciiNumLen, VOS_UINT8 *pucBcdNum, VOS_UINT8 *pucBcdNumLen)
{

    VOS_UINT8     i, k;
    VOS_UINT8     *pucTemp;

    pucTemp = pucBcdNum;

    for (i=0; i<ucAsciiNumLen; i++)
    {
        k = i % 2;

        if ((*(pucAsciiNum + i) >= 0x30) && (*(pucAsciiNum + i) <= 0x39))/*????????*/
        {
            if(k == 0)
            {
                *(pucTemp + (i / 2) ) = *(pucAsciiNum + i) - 0x30;
            }
            else
            {
                *(pucTemp + (i / 2) ) = (VOS_UINT8)(((VOS_UINT8)(*(pucAsciiNum + i) - 0x30) << 4) | *(pucTemp + (i / 2)));
            }
        }
        else if('*' == *(pucAsciiNum + i))  /*????*????*/
        {
            if(k == 0)
            {
                *(pucTemp + (i/2)) = 0xa;
            }
            else
            {
                *(pucTemp + (i/2)) = (0xa << 4) | *(pucTemp + (i/2));
            }
        }
        else if('#' == *(pucAsciiNum + i))  /*????#????*/
        {
            if(k == 0)
            {
                *(pucTemp + (i/2)) = 0xb;
            }
            else
            {
                *(pucTemp + (i/2)) = (0xb << 4) | *(pucTemp + (i/2));
            }
        }
        else if('?' == *(pucAsciiNum + i))  /*?????????*/
        {
            if(k == 0)
            {
                *(pucTemp + (i/2)) = 0xd;
            }
            else
            {
                *(pucTemp + (i/2)) = (0xd << 4) | *(pucTemp + (i/2));
            }
        }
        else if (('P' == *(pucAsciiNum + i))
             || ('p' == *(pucAsciiNum + i))
             || (',' == *(pucAsciiNum + i)) ) /*????PAUSE????*/
        {
            if(k == 0)
            {
                *(pucTemp + (i/2)) = 0xc;
            }
            else
            {
                *(pucTemp + (i/2)) = (0xc << 4) | *(pucTemp + (i/2));
            }
        }
        else                                        /*??????????????*/
        {
            PB_WARNING_LOG("SI_PB_AsciiToBcd: The Char is Not Suspensory");/*????????*/
        }
    }

    *pucBcdNumLen = ucAsciiNumLen/2;

    if((ucAsciiNumLen % 2) == 1)    /*??????????????F*/
    {
        *(pucTemp + (ucAsciiNumLen / 2)) = 0xf0 | *(pucTemp + (ucAsciiNumLen / 2));

        (*pucBcdNumLen)++;
    }

    return;
}


VOS_UINT32 SI_FindMultiTagInBERTLV(VOS_UINT8 *pType1TagAddr, VOS_UINT8 ucTag,
                                            VOS_UINT8 *pucOffset,VOS_UINT8 ucTagCount)
{
    VOS_UINT32                          i;
    VOS_UINT8                          *pucDataBuf;
    VOS_UINT8                           ucFindCount = 0;
    VOS_UINT32                          ulDataLen;

    pucDataBuf = pType1TagAddr + 2;
    ulDataLen  = pType1TagAddr[1];

    for(i=0; (i < ulDataLen)&&(ucFindCount < ucTagCount);)
    {
        if(pucDataBuf[i] == ucTag)   /*????????????????*/
        {
            pucOffset[ucFindCount] = (VOS_UINT8)(i + 1);

            ucFindCount++;
        }

        i += pucDataBuf[i+1] + 2; /*??????????Tag*/
    }

    return (0 == ucFindCount)?SI_TAGNOTFOUND:ucFindCount;
}


VOS_UINT32 SI_FindTagInBERTLV(VOS_UINT8 *pucDataBuf, VOS_UINT8 ucTag, VOS_UINT32 ulDataLen)
{
    VOS_UINT32 i;

    for(i=0; i<ulDataLen;)
    {
        if(pucDataBuf[i] == ucTag)   /*????????????????*/
        {
            return (i+1);
        }
        else
        {
            i += pucDataBuf[i+1] + 2; /*??????????Tag*/
        }
    }

    return SI_TAGNOTFOUND;
}


VOS_UINT32 SI_FindType2FileTagNum(VOS_UINT8 *pucDataBuf, VOS_UINT8 ucTag, VOS_UINT32 ulDataLen)
{
    VOS_UINT32 i;
    VOS_UINT32 ulTagNum = 1;

    for(i=0; i<ulDataLen;)
    {
        if(pucDataBuf[i] == ucTag)   /*????Tag Num*/
        {
            return ulTagNum;
        }
        else
        {
            ulTagNum++;
            i += pucDataBuf[i+1] + 2; /*??????????Tag*/
        }
    }

    return SI_TAGNOTFOUND;
}



VOS_UINT32 SI_PB_DecodeEFPBR_AdnContent(VOS_UINT8  *pType1TagAddr, VOS_UINT32 i)
{
    VOS_UINT32 ulOffset;

    ulOffset = SI_FindTagInBERTLV(&pType1TagAddr[2], EFADNDO_TAG, pType1TagAddr[1]);/*??????????ADN????ID*/

    if(SI_TAGNOTFOUND == ulOffset)     /*??????????*/
    {
        PB_ERROR_LOG("SI_PB_DecodeEFPBR_AdnContent: Could Not Find the EFADNDO_TAG Tag");

        return ulOffset;
    }

    gstPBCtrlInfo.astADNInfo[i].usFileID = ((pType1TagAddr[ulOffset+3]<<0x08)&0xFF00)+pType1TagAddr[ulOffset+4];

    /*??????SFI??????????????*/
    if(0x03 == pType1TagAddr[ulOffset+2])
    {
        gstPBCtrlInfo.astADNInfo[i].ucSFI = pType1TagAddr[ulOffset+5];
    }

    gstPBCtrlInfo.ulADNFileNum++;

    return VOS_OK;
}



VOS_VOID SI_PB_DecodeEFPBR_UidContent(VOS_UINT8 * pType1TagAddr, VOS_UINT32 i)
{
    VOS_UINT32 ulOffset;

    ulOffset = SI_FindTagInBERTLV(&pType1TagAddr[2], EFUIDDO_TAG, pType1TagAddr[1]);/*??????????UID????ID*/

    if(SI_TAGNOTFOUND == ulOffset) /*??????????*/
    {
        PB_WARNING_LOG("SI_PB_DecodeEFPBR_UidContent: Could Not Find the EFUIDDO_TAG Tag");

        gstPBCtrlInfo.astUIDInfo[i].usFileID = 0xFFFF;
    }
    else
    {
        gstPBCtrlInfo.astUIDInfo[i].usFileID = ((pType1TagAddr[ulOffset+3]<<0x08)&0xFF00)+pType1TagAddr[ulOffset+4];
    }
}


VOS_VOID SI_PB_DecodeEFPBR_PbcContent(VOS_UINT8 * pType1TagAddr, VOS_UINT32 i)
{
    VOS_UINT32 ulOffset;

    ulOffset = SI_FindTagInBERTLV(&pType1TagAddr[2], EFPBCDO_TAG, pType1TagAddr[1]);/*??????????PBC????ID*/

    if(SI_TAGNOTFOUND == ulOffset) /*??????????*/
    {
        PB_WARNING_LOG("SI_PB_DecodeEFPBR_PbcContent: Could Not Find the EFUIDDO_TAG Tag");

        gstPBCtrlInfo.astPBCInfo[i].usFileID = 0xFFFF;

        gucPBCStatus = VOS_FALSE;
    }
    else if(VOS_TRUE == SI_PB_CheckSupportAP())
    {
        gstPBCtrlInfo.astPBCInfo[i].usFileID = 0xFFFF;

        gucPBCStatus = VOS_FALSE;
    }
    else
    {
        gstPBCtrlInfo.astPBCInfo[i].usFileID = ((pType1TagAddr[ulOffset+3]<<0x08)&0xFF00)+pType1TagAddr[ulOffset+4];
    }
}


VOS_VOID SI_PB_DecodeEFPBR_IapContent(VOS_UINT8 * pType1TagAddr, VOS_UINT32 i)
{
    VOS_UINT32 ulOffset;

    ulOffset = SI_FindTagInBERTLV(&pType1TagAddr[2], EFIAPDO_TAG, pType1TagAddr[1]);/*??????????IAP????ID*/

    if(SI_TAGNOTFOUND == ulOffset) /*??????????*/
    {
        PB_WARNING_LOG("SI_PB_DecodeEFPBR_IapContent: Could Not Find the EFIAPDO_TAG Tag");
    }
    else
    {
        gstPBCtrlInfo.astIAPInfo[i].usIAPFileID = ((pType1TagAddr[ulOffset+3]<<0x08)&0xFF00)+pType1TagAddr[ulOffset+4];

        gstPBCtrlInfo.ulIAPFileNum++;
    }
}


VOS_UINT32 SI_PB_DecodeType1EFPBR_AnrContent(VOS_UINT8 * pType1TagAddr, VOS_UINT32 i, VOS_UINT8 *pucOffset)
{
    VOS_UINT32 ulANRCount; /*????????????????ANR????*/
    VOS_UINT32 ulOffset;
    VOS_UINT32 j;

    ulANRCount = SI_FindMultiTagInBERTLV(pType1TagAddr, (VOS_UINT8)EFANRDO_TAG,
                                       pucOffset,(VOS_UINT8)SI_PB_ANR_MAX);/*??????????ANR????*/

    if(SI_TAGNOTFOUND == ulANRCount) /*??????????*/
    {
        PB_NORMAL_LOG("SI_PB_DecodeType1EFPBR_AnrContent: Could Not Find the EFANRDO_TAG Tag");
    }
    else
    {
        ulANRCount = (ulANRCount > SI_PB_ANRMAX)?SI_PB_ANRMAX:ulANRCount;

        for(j = 0; j < ulANRCount; j++)
        {
            ulOffset = pucOffset[j];

            gstPBCtrlInfo.astANRInfo[i][j].usANRFileID  = ((pType1TagAddr[ulOffset+3]<<0x08)&0xFF00)+pType1TagAddr[ulOffset+4];
            gstPBCtrlInfo.astANRInfo[i][j].enANRType = PB_FILE_TYPE1;
            gstPBCtrlInfo.ulANRFileNum++;
        }
    }

    return ulANRCount;
}


VOS_UINT32 SI_PB_DecodeType2EFPBR_AnrContent(VOS_UINT8 * pType2TagAddr, VOS_UINT32 i, VOS_UINT8 *pucOffset)
{
    VOS_UINT32 ulANRCount; /*????????????????ANR????*/
    VOS_UINT32 ulOffset;
    VOS_UINT32 j;

    ulANRCount = SI_FindMultiTagInBERTLV(pType2TagAddr, (VOS_UINT8)EFANRDO_TAG,
                                       pucOffset,(VOS_UINT8)SI_PB_ANR_MAX);/*??????????ANR????*/

    if(SI_TAGNOTFOUND == ulANRCount) /*??????????*/
    {
        PB_NORMAL_LOG("SI_PB_DecodeType2EFPBR_AnrContent: Could Not Find the EFANRDO_TAG Tag");
    }
    else
    {
        ulANRCount = (ulANRCount > SI_PB_ANRMAX)?SI_PB_ANRMAX:ulANRCount;

        for(j = 0; j < ulANRCount; j++)
        {
            ulOffset = pucOffset[j];

            gstPBCtrlInfo.astANRInfo[i][j].usANRFileID  = ((pType2TagAddr[ulOffset+3]<<0x08)&0xFF00)+pType2TagAddr[ulOffset+4];
            gstPBCtrlInfo.astANRInfo[i][j].enANRType = PB_FILE_TYPE2;
            gstPBCtrlInfo.ulANRFileNum++;

            gstPBCtrlInfo.astANRInfo[i][j].ulANRTagNum =
            SI_FindType2FileTagNum(pType2TagAddr+2, EFANRDO_TAG, pType2TagAddr[1]);
        }
    }

    return ulANRCount;
}


VOS_VOID SI_PB_DecodeEFPBR_ExtContent(VOS_UINT8  *pType3TagAddr)
{
    VOS_UINT32 ulOffset;

    /* ??????ADN????????????????????EXT1????,??????????PBR???????????????????? */
    ulOffset = SI_FindTagInBERTLV(pType3TagAddr+2, EFEXT1DO_TAG, pType3TagAddr[1]);/*??????????EXT????ID*/

    /*??????????*/
    if(SI_TAGNOTFOUND == ulOffset)
    {
        PB_WARNING_LOG("SI_PB_DecodeEFPBR_ExtContent: Could Not Find the EFEXTDO_TAG Tag");

        gstPBCtrlInfo.usEXT1FileID = 0xFFFF;
        gastEXTContent[PB_ADN_CONTENT].usExtFileId  = 0xFFFF;
        gastEXTContent[PB_ADN_CONTENT].usExtFlag    = SI_PB_FILE_NOT_EXIST;
    }
    else if( SI_PB_FILE_EXIST == gastEXTContent[PB_ADN_CONTENT].usExtFlag )
    {
        /*do nothing*/
    }
    else
    {
        gstPBCtrlInfo.usEXT1FileID = ((pType3TagAddr[ulOffset+3]<<0x08)&0xFF00)+pType3TagAddr[ulOffset+4];
        gastEXTContent[PB_ADN_CONTENT].usExtFlag = SI_PB_FILE_EXIST;
    }
}


VOS_VOID SI_PB_DecodeType2EFPBR_EmailContent(VOS_UINT8 * pType2TagAddr, VOS_UINT32 i)
{
    VOS_UINT32 ulOffset;

    ulOffset = SI_FindTagInBERTLV(pType2TagAddr+2, EFEMAILDO_TAG, pType2TagAddr[1]);/*??????????EXT????ID*/

    /*??????????*/
    if(SI_TAGNOTFOUND == ulOffset)
    {
        PB_NORMAL_LOG("SI_PB_DecodeType2EFPBR_EmailContent: Could Not Find the EFEMAILDO_TAG Tag");
    }
    else
    {
        gstPBCtrlInfo.astEMLInfo[i].usEMLFileID = ((pType2TagAddr[ulOffset+3]<<0x08)&0xFF00)+pType2TagAddr[ulOffset+4];
        gstPBCtrlInfo.ulEMLFileNum++;

        gstPBCtrlInfo.astEMLInfo[i].enEMLType = PB_FILE_TYPE2;

        gstPBCtrlInfo.astEMLInfo[i].ulEMLTagNum =
        SI_FindType2FileTagNum(pType2TagAddr+2, EFEMAILDO_TAG, pType2TagAddr[1]);
    }
}


VOS_UINT32 SI_PB_DecodeType1EFPBR_EmailContent(VOS_UINT8 * pType2TagAddr, VOS_UINT32 i)
{
    VOS_UINT32 ulOffset;

    ulOffset = SI_FindTagInBERTLV(pType2TagAddr+2, EFEMAILDO_TAG, pType2TagAddr[1]);/*??????????EXT????ID*/

    /*??????????*/
    if(SI_TAGNOTFOUND == ulOffset)
    {
        PB_NORMAL_LOG("SI_PB_DecodeType1EFPBR_EmailContent: Could Not Find the EFEMAILDO_TAG Tag");

        return SI_TAGNOTFOUND;
    }

    gstPBCtrlInfo.astEMLInfo[i].usEMLFileID = ((pType2TagAddr[ulOffset+3]<<0x08)&0xFF00)+pType2TagAddr[ulOffset+4];
    gstPBCtrlInfo.ulEMLFileNum++;

    gstPBCtrlInfo.astEMLInfo[i].enEMLType = PB_FILE_TYPE1;

    return VOS_OK;
}


VOS_UINT32 SI_PB_DecodeEFPBR(VOS_UINT8 ucRecordNum, VOS_UINT8 ucRecordLen, VOS_UINT8 *pContent)
{
    VOS_UINT32 i;
    VOS_UINT32 j;
    VOS_UINT8  *ptemp = pContent;
    VOS_UINT8  *pType1TagAddr;
    VOS_UINT8  *pType2TagAddr;
    VOS_UINT8  *pType3TagAddr;
    VOS_UINT8  aucANROffset[SI_PB_ANR_MAX];
    VOS_UINT32 ulType1EMLFlag = VOS_OK;
    VOS_UINT32 ulType1ANRFlag = VOS_OK;
    VOS_UINT32 ulOffset;

    gstPBCtrlInfo.ulADNFileNum  = 0;

    for(i=0; i<ucRecordNum; i++,(ptemp+= ucRecordLen))
    {
        if((ptemp[0]&0xF0) != 0xA0 ) /*??????????????????????*/
        {
            continue;
        }

        /*????Tpye1????????*/
        ulOffset = SI_FindTagInBERTLV(ptemp, PB_FILE_TYPE1, ucRecordLen);

        if(SI_TAGNOTFOUND == ulOffset)
        {
            PB_ERROR_LOG("SI_PB_DecodeEFPBR: Could Not Find the 0xA8 Tag");

            continue;
        }

        pType1TagAddr = (ptemp + ulOffset) - 1;

        if(SI_TAGNOTFOUND == SI_PB_DecodeEFPBR_AdnContent(pType1TagAddr, i))
        {
            continue;
        }

        ulType1EMLFlag = SI_PB_DecodeType1EFPBR_EmailContent(pType1TagAddr, i);

        SI_PB_DecodeEFPBR_UidContent(pType1TagAddr, i);

        SI_PB_DecodeEFPBR_PbcContent(pType1TagAddr, i);

        SI_PB_DecodeEFPBR_IapContent(pType1TagAddr, i);

        ulType1ANRFlag = SI_PB_DecodeType1EFPBR_AnrContent(pType1TagAddr, i, aucANROffset);

        /*????Tpye2????????*/
        ulOffset = SI_FindTagInBERTLV(ptemp, PB_FILE_TYPE2, ucRecordLen);

        /*Type2 ????????????????????????Type1???? Email????*/
        if(SI_TAGNOTFOUND == ulOffset)
        {
            PB_NORMAL_LOG("SI_PB_DecodeEFPBR: Could Not Find the Type2 Tag");
        }
        else
        {
            pType2TagAddr = (ptemp + ulOffset) - 1;

            if(SI_TAGNOTFOUND == ulType1EMLFlag)
            {
                SI_PB_DecodeType2EFPBR_EmailContent(pType2TagAddr, i);
            }

            if(SI_TAGNOTFOUND == ulType1ANRFlag)
            {
                (VOS_VOID)SI_PB_DecodeType2EFPBR_AnrContent(pType2TagAddr, i, aucANROffset);
            }
        }

        /*????Tpye3????????*/
        ulOffset = SI_FindTagInBERTLV(ptemp, PB_FILE_TYPE3, ucRecordLen);

        if(SI_TAGNOTFOUND == ulOffset)
        {
            PB_ERROR_LOG("SI_PB_DecodeEFPBR: Could Not Find the AA Tag");

            gstPBCtrlInfo.usEXT1FileID = 0xFFFF;
            gastEXTContent[PB_ADN_CONTENT].usExtFileId  = 0xFFFF;
            gastEXTContent[PB_ADN_CONTENT].usExtFlag    = SI_PB_FILE_NOT_EXIST;
        }
        else
        {
            pType3TagAddr = (ptemp + ulOffset) - 1;

            SI_PB_DecodeEFPBR_ExtContent(pType3TagAddr);
        }
    }

    if ( 0 == gstPBCtrlInfo.ulADNFileNum )
    {
        return VOS_ERR;
    }

    for(i = gstPBCtrlInfo.ulADNFileNum -1; i != 0; i--)
    {
        for(j = 0; j < SI_PB_ANRMAX; j++)
        {
            if((gstPBCtrlInfo.astANRInfo[i][j].usANRFileID
               == gstPBCtrlInfo.astANRInfo[i-1][j].usANRFileID)
               &&(0 != gstPBCtrlInfo.astANRInfo[i][j].usANRFileID))
            {
                gstPBCtrlInfo.astANRInfo[i][j].usANRFileID = 0;
                gstPBCtrlInfo.astANRInfo[i][j].ucRecordNum = 0;
                gstPBCtrlInfo.astANRInfo[i][j].ucRecordLen = 0;

                gstPBCtrlInfo.ulANRFileNum--;
            }
        }

        if((gstPBCtrlInfo.astEMLInfo[i].usEMLFileID
           == gstPBCtrlInfo.astEMLInfo[i-1].usEMLFileID)
           &&(0 != gstPBCtrlInfo.astEMLInfo[i].usEMLFileID))
        {
            gstPBCtrlInfo.astEMLInfo[i].usEMLFileID = 0;
            gstPBCtrlInfo.astEMLInfo[i].ucRecordLen = 0;
            gstPBCtrlInfo.astEMLInfo[i].ucRecordNum = 0;

            gstPBCtrlInfo.ulEMLFileNum--;
        }
    }

    /* ????USIM??????????????????????????????SEARCH??????????
       ????????????????SEARCH?????????????????????????? */
    gstPBInitState.enPBSearchState = PB_SEARCH_ENABLE;

    return VOS_OK;
}


VOS_VOID SI_PB_DecodeEFPBCRecord(VOS_UINT8 *pucContent, VOS_UINT8 ucRecordNum)
{

    if( 1 == ((*pucContent)&0x1) )
    {
        /*????????????????????*/
        gstPBInitState.stPBCUpdate.aucRecord[++gstPBInitState.stPBCUpdate.ucEntryChangeNum] = ucRecordNum;

    }

    return;
}


VOS_VOID SI_PB_DecodeEFUIDRecord(VOS_UINT8 *pucContent)
{
    VOS_UINT16 usEFUIDValue = 0;

    usEFUIDValue = *pucContent++;

    usEFUIDValue <<= 8;

    usEFUIDValue |= *pucContent++;

    if(usEFUIDValue == 0xFFFF)
    {
        return;
    }

    if(gstPBCtrlInfo.usUIDMaxValue < usEFUIDValue)
    {
        gstPBCtrlInfo.usUIDMaxValue = usEFUIDValue;
    }

    return;
}


VOS_UINT32 SI_PB_JudgeADNFid(VOS_UINT16 usFileId)
{
    VOS_UINT32 i;

    for(i = 0; i < gstPBCtrlInfo.ulADNFileNum; i++)
    {
        if(usFileId == gstPBCtrlInfo.astADNInfo[i].usFileID)
        {
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_JudgeEXTFid(VOS_UINT16 usFileId, VOS_UINT16 usOffset)
{

    if(usFileId == gastEXTContent[usOffset].usExtFileId)
    {
        return VOS_OK;
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_JudgeIAPFid(VOS_UINT16 usFileId)
{
    VOS_UINT32 i;

    for(i = 0; i < gstPBCtrlInfo.ulADNFileNum; i++)
    {
        if(usFileId == gstPBCtrlInfo.astIAPInfo[i].usIAPFileID)
        {
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_GetEXTContentFromReq(VOS_UINT8 ucRecordNum, VOS_UINT8 **ppucContent)
{
    VOS_UINT32 i;

    if(ucRecordNum == gstPBReqUnit.usExtIndex)
    {
        *ppucContent = gstPBReqUnit.aucEXTContent;

        gstPBReqUnit.usExtIndex = 0;/*??????????*/

        return VOS_OK;
    }

    for(i = 0; i < SI_PB_ANR_MAX; i++)
    {
        if(ucRecordNum == gstPBReqUnit.stSPBReq.usANRExtIndex[i])
        {
            *ppucContent = gstPBReqUnit.stSPBReq.aucANRExtContent[i];

            gstPBReqUnit.stSPBReq.usANRExtIndex[i] = 0;/*??????????*/
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


VOS_VOID SI_PB_SetBitToBuf(VOS_UINT8 *pucDataBuf, VOS_UINT32 ulBitNo,VOS_UINT32 ulValue)
{
    VOS_UINT32  ulOffset;
    VOS_UINT8   ucBit;

    if(VOS_NULL_PTR == pucDataBuf)
    {
        PB_ERROR_LOG("SI_PB_SetBitToBuf: Input Null Ptr");

        return;
    }

    ulOffset = (ulBitNo - 1)/ 8;

    ucBit    = (VOS_UINT8)((ulBitNo - 1)%8);

    if(0 != ulValue)
    {
        /*lint -e701 */
        pucDataBuf[ulOffset] |= (VOS_UINT8)(0x1<<ucBit);
        /*lint +e701 */
    }
    else
    {
        /*lint -e701 */
        pucDataBuf[ulOffset] &= (~(VOS_UINT8)(0x1<<ucBit));
        /*lint +e701 */
    }

    return;
}


VOS_UINT32 SI_PB_CheckFdnNumLen(
    VOS_UINT32                          ulFdnNumLen,
    VOS_UINT32                          ulCmpNumLen)
{
    USIMM_FEATURE_ATT_CFG_STRU          stAttCfg = {0};

    if (VOS_OK != mdrv_nv_read(en_NV_Item_Usim_Att_flg,
                               (VOS_VOID*)&stAttCfg,
                               sizeof(stAttCfg)))
    {
        stAttCfg.ulAtt_flg = VOS_FALSE;
    }

    /*??????????FDN?????????????? ??????FDN?????????????????????????????? */
    if(VOS_FALSE == stAttCfg.ulAtt_flg)
    {
        if (ulFdnNumLen > ulCmpNumLen)
        {
           return VOS_ERR;
        }
    }
    /*ATT FDN????????????????????FDN????????????????????????????????????????*/
    else
    {
        if (ulFdnNumLen != ulCmpNumLen)
        {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


VOS_UINT32 SI_PB_BcdCompare(VOS_UINT8 *pucBcdNumSrc, VOS_UINT8 ucBcdSrcLen, VOS_UINT8 *pucFdnBcdNum, VOS_UINT8 ucFdnBcdLen)
{
    VOS_UINT8                           ucDstIndex;
    VOS_UINT8                           ucFdnNumLen;
    VOS_UINT8                           ucCmpNumLen;
    VOS_UINT8                           aucFdnNum[SI_PB_NUM_LEN*2]  = {0};
    VOS_UINT8                           aucCmpNum[SI_PB_NUM_LEN*2]  = {0};

    PAM_MEM_SET_S(aucFdnNum, sizeof(aucFdnNum), (VOS_CHAR)0xFF, sizeof(aucFdnNum));

    PAM_MEM_SET_S(aucCmpNum, sizeof(aucCmpNum), (VOS_CHAR)0xFF, sizeof(aucCmpNum));

    /* ??????????????40????????BCD????????????20?????? */
    if ((ucBcdSrcLen > SI_PB_NUM_LEN) || (ucFdnBcdLen > SI_PB_NUM_LEN))
    {
        return VOS_ERR;
    }

    /* ??????????????????ASCII?????????????? */
    SI_PB_BcdToAscii(ucBcdSrcLen, pucBcdNumSrc, aucCmpNum, &ucCmpNumLen);

    /* ????????FDN??????????ASCII?????????????? */
    SI_PB_BcdToAscii(ucFdnBcdLen, pucFdnBcdNum, aucFdnNum, &ucFdnNumLen);

    if (VOS_OK != SI_PB_CheckFdnNumLen(ucFdnNumLen, ucCmpNumLen))
    {
        return VOS_ERR;
    }

    /* ??????????????????ASCII?? */
    for (ucDstIndex = 0; ucDstIndex < ucFdnNumLen; ucDstIndex++)
    {
        /* ????'?'????????????????????????.???????????????????? */
        if ((aucFdnNum[ucDstIndex] == aucCmpNum[ucDstIndex])
            || ('?' == aucFdnNum[ucDstIndex]))
        {
            continue;
        }
        else
        {
            break;
        }
    }

    /* ?????????????????????? */
    if (ucFdnNumLen == ucDstIndex)
    {
        return VOS_OK;
    }

    return VOS_ERR;
}


VOS_UINT32 SI_PB_CheckEXT2(VOS_UINT8 ucExtRecord,
                                   VOS_UINT32 ulExtInfoNum,
                                   VOS_UINT8 *pucDstNum,
                                   VOS_UINT8 ucContentSize,
                                   VOS_UINT8 *pucExNumLen)
{
    VOS_UINT8                          *pExTemp;
    VOS_UINT8                           ucExNumLen;

    pExTemp = gastEXTContent[ulExtInfoNum].pExtContent;

    if (VOS_NULL_PTR == pExTemp)
    {
        PB_ERROR_LOG("SI_PB_CheckFdn: gastEXTContent is null.\r\n");
        return VOS_ERR;
    }

    if ((ucExtRecord > gastEXTContent[ulExtInfoNum].usExtTotalNum)
       || (0 == ucExtRecord))
    {
        PB_ERROR1_LOG("SI_PB_CheckFdn: ucExtRecord: %d is wrong .\r\n", ucExtRecord);
        return VOS_ERR;
    }

    pExTemp += (ucExtRecord - 1) * SI_PB_EXT_LEN;

    /* ????EX???????? */
    if ((0xFF == *(pExTemp+1)) || (0x0 == *(pExTemp+1)))
    {
        PB_ERROR_LOG("SI_PB_CheckFdn: EXT NUM Length is error.\r\n");
        return VOS_ERR;
    }

    ucExNumLen = (*(pExTemp+1) > (SI_PB_NUM_LEN/2))?
                    (SI_PB_NUM_LEN/2) : *(pExTemp + 1);

    PAM_MEM_CPY_S(pucDstNum, ucContentSize, (pExTemp+2), ucExNumLen);

    *pucExNumLen = ucExNumLen;

    return VOS_OK;
}


VOS_UINT32 SI_PB_CheckFdn(VOS_UINT8 *pucNum, VOS_UINT32 ulNumLength)
{
    VOS_UINT32                          ulResult;
    VOS_UINT8                           ucPBOffset=0;
    VOS_UINT16                          i;
    VOS_UINT8                          *pTemp;
    VOS_UINT8                          *pucFdnContent;
    VOS_UINT8                           aucNumber[SI_PB_NUM_LEN] = {0};
    VOS_UINT16                          usTotalNum;
    VOS_UINT8                           ucNumLen;
    VOS_UINT8                           ucExNumLen;
    VOS_UINT32                          ulExtInfoNum;
    VOS_UINT8                           ucExtRecord;

    SI_PB_InitContent(SI_PB_NUM_LEN, (VOS_UINT8)0xFF, aucNumber);
    ucNumLen = 0;

    /*????????????????????????????????*/
    ulResult = SI_PB_LocateRecord(PB_FDN, 1, 1, &ucPBOffset);
    if(VOS_OK != ulResult)
    {
        PB_ERROR_LOG("SI_PB_CheckFdn: SI_PB_LocateRecord Return Failed.\r\n");
        return VOS_ERR;
    }

    usTotalNum      = gastPBContent[ucPBOffset].usTotalNum;
    pucFdnContent   = gastPBContent[ucPBOffset].pContent;

    if (VOS_NULL_PTR == pucFdnContent)
    {
        PB_ERROR_LOG("SI_PB_CheckFdn: gastPBContent is null.\r\n");
        return VOS_ERR;
    }

    ulExtInfoNum = gastPBContent[ucPBOffset].ulExtInfoNum;

    /*??????????????????????????????????????????????VOS_OK*/
    for(i=0; i<usTotalNum; i++)   /*????????????????????????*/
    {
        pTemp = pucFdnContent;
        pTemp += gastPBContent[ucPBOffset].ucNameLen;

        /*????????????????????*/
        if ((0xFF == *pTemp)||(*pTemp < 2))
        {
            pucFdnContent += gastPBContent[ucPBOffset].ucRecordLen;

            continue;
        }

        ucNumLen = *pTemp - 1;

        ucNumLen = (ucNumLen > (SI_PB_NUM_LEN / 2))?(SI_PB_NUM_LEN / 2):ucNumLen;

        SI_PB_InitContent(SI_PB_NUM_LEN, (VOS_UINT8)0xFF, aucNumber);

        PAM_MEM_CPY_S(aucNumber, SI_PB_NUM_LEN, pTemp+2, ucNumLen);

        ucExtRecord = pucFdnContent[gastPBContent[ucPBOffset].ucRecordLen-1];

        /* ????EX???? */
        if (0xFF != ucExtRecord)
        {
            if (VOS_OK != SI_PB_CheckEXT2(ucExtRecord, ulExtInfoNum, &(aucNumber[ucNumLen]), SI_PB_NUM_LEN - ucNumLen, &ucExNumLen))
            {
                return VOS_ERR;
            }

            ucNumLen += ucExNumLen;
        }

        /*????????*/
        if (VOS_OK == SI_PB_BcdCompare(pucNum, (VOS_UINT8)ulNumLength, aucNumber, ucNumLen))
        {
            PB_ERROR_LOG("SI_PB_CheckFdn: SI_PB_BcdCompare ok");

            return VOS_OK;
        }

        pucFdnContent   += gastPBContent[ucPBOffset].ucRecordLen;
    }

    return VOS_ERR;
}
/* Added by f62575 for C50_IPC Project, 2012/02/23, end   */


/*lint -save -e958 */
VOS_UINT32 SI_PB_EcallNumberErrProc(SI_PB_STORATE_TYPE ulStorage, VOS_UINT8 ucListLen, VOS_UINT8 *pucList, VOS_UINT8 *pucPBOffset)
{
    VOS_UINT8                           i;
    VOS_UINT8                           ucPBOffset;

    if (VOS_OK != SI_PB_FindPBOffset(ulStorage, pucPBOffset))
    {
        PB_WARNING_LOG("SI_PB_EcallNumberErrProc: Locate PhoneBook Error");

        return TAF_ERR_ERROR;
    }

    ucPBOffset = *pucPBOffset;

    /* ?????????????????????????? */
    if (PB_INITIALISED != gastPBContent[ucPBOffset].enInitialState)
    {
        PB_ERROR_LOG("SI_PB_EcallNumberErrProc:The PhoneBook is Not Initializtion");

        return TAF_ERR_SIM_BUSY;
    }

    /* ???????????????????? */
    if (VOS_NULL_PTR == gastPBContent[ucPBOffset].pContent)
    {
        PB_ERROR_LOG("SI_PB_EcallNumberErrProc:The PhoneBook is Not Initializtion");

        return TAF_ERR_PB_NOT_INIT;
    }

    /* ??????index??????????????????????0 */
    for (i = 0; i < ucListLen; i++)
    {
        if ((pucList[i] > gastPBContent[ucPBOffset].usTotalNum)
            || (VOS_NULL == pucList[i]))
        {
            PB_ERROR_LOG("SI_PB_EcallNumberErrProc:Wrong Index");

            return TAF_ERR_PB_WRONG_INDEX;
        }
    }

    return TAF_ERR_NO_ERROR;
}
/*lint -restore */

/*lint -save -e958 */
VOS_VOID SI_PB_GetEcallNumber(SI_PB_CONTENT_STRU *pstXdnContent, SI_PB_ECALL_NUM_STRU *pstEcallNum, VOS_UINT8 ucListLen, VOS_UINT8 *pucList)
{
    VOS_UINT8                          *pucContent;
    VOS_UINT8                          *pucTmp;
    VOS_UINT8                          *pucExtContent;
    VOS_UINT32                          ulExtInfoNum;
    VOS_UINT8                           ucRecordLen;
    VOS_UINT8                           ucExNumLen;
    VOS_UINT8                           ucExtRecord;
    VOS_UINT8                           i;


    pucContent = pstXdnContent->pContent;
    ucRecordLen= pstXdnContent->ucRecordLen;

    for (i = 0; i < ucListLen; i++)
    {
        pstEcallNum->ucIndex  = pucList[i];

        SI_PB_InitContent(SI_PB_NUM_LEN, (VOS_UINT8)0xFF, pstEcallNum->aucCallNumber);
/*lint -save -e961 */
        pucTmp = pucContent + (pstXdnContent->ucRecordLen) * (pucList[i] - 1);
/*lint -restore */
        ucExtRecord  = pucTmp[ucRecordLen-1];
        ulExtInfoNum = pstXdnContent->ulExtInfoNum;
        pucExtContent = gastEXTContent[ulExtInfoNum].pExtContent;

        ucExtRecord = ((VOS_NULL_PTR == pucExtContent) ? 0xFF : (ucExtRecord));

        if ((pucTmp[pstXdnContent->ucNameLen] < 2) || (0xFF == pucTmp[pstXdnContent->ucNameLen]))
        {
            pstEcallNum->ucLength = 0;
        }
        else if((0xFF != ucExtRecord)&&(0 != ucExtRecord)&&(VOS_NULL_PTR != pucExtContent)
                &&(ucExtRecord <= gastEXTContent[ulExtInfoNum].usExtTotalNum))
        {
            pucExtContent += (ucExtRecord - 1) * SI_PB_EXT_LEN;
            ucExNumLen = (*(pucExtContent + 1) > (SI_PB_NUM_LEN/2))? (SI_PB_NUM_LEN/2) : *(pucExtContent + 1);

            PAM_MEM_CPY_S(pstEcallNum->aucCallNumber, (SI_PB_NUM_LEN/2), pucTmp + pstXdnContent->ucNameLen + 2, (SI_PB_NUM_LEN/2));
            PAM_MEM_CPY_S(pstEcallNum->aucCallNumber + (SI_PB_NUM_LEN/2), (SI_PB_NUM_LEN/2), pucExtContent + 2, (SI_PB_NUM_LEN/2));

            pstEcallNum->ucTon    = pucTmp[pstXdnContent->ucNameLen + 1];
            pstEcallNum->ucLength = (SI_PB_NUM_LEN/2) + ucExNumLen;
        }
        else
        {
            PAM_MEM_CPY_S(pstEcallNum->aucCallNumber, (SI_PB_NUM_LEN/2), pucTmp + pstXdnContent->ucNameLen + 2, (SI_PB_NUM_LEN/2));

            pstEcallNum->ucTon    = pucTmp[pstXdnContent->ucNameLen + 1];
            pstEcallNum->ucLength = ((pucTmp[pstXdnContent->ucNameLen] - 1) > (SI_PB_NUM_LEN/2))?
                                    (SI_PB_NUM_LEN/2) : (pucTmp[pstXdnContent->ucNameLen] - 1);

        }

        pstEcallNum++;
    }

    return;
}
/*lint -restore */
#endif

#if ((OSA_CPU_ACPU == VOS_OSA_CPU)||(defined(DMT)))

VOS_VOID SI_PB_TransANRFromate(VOS_UINT8 ucANROffset,VOS_UINT8 *pANRContent, SI_PB_RECORD_STRU *pstRecord)
{
    VOS_UINT32 ulResult;
    VOS_UINT8  ucExtRecord;
    VOS_UINT8  aucPhoneNumber[20] = {0};
    VOS_UINT8  *pucExtContent;
    VOS_UINT8  ucExtNumLen;
    VOS_UINT8  ucAnrNumLen;

    ulResult = SI_PB_CheckANRValidity(pANRContent);/*??????????????????????*/

    if(ulResult != VOS_OK)
    {
        /*??????0*/
        pstRecord->AdditionNumber[ucANROffset].NumberLength = 0;
    }
    else
    {
        pstRecord->ValidFlag = SI_PB_CONTENT_VALID;/*??????????????????*/

        pstRecord->AdditionNumber[ucANROffset].NumberType = pANRContent[2];

        pucExtContent = gastEXTContent[PB_ADN_CONTENT].pExtContent;

        ucExtRecord = ((VOS_NULL_PTR == pucExtContent)?0xFF:pANRContent[14]);

        if((0xFF != ucExtRecord)&&(VOS_NULL_PTR != pucExtContent)
           &&(ucExtRecord <= gastEXTContent[PB_ADN_CONTENT].usExtTotalNum))
        {
            pucExtContent += (ucExtRecord - 1) * SI_PB_EXT_LEN;
            ucExtNumLen   = (*(pucExtContent + 1) > (SI_PB_NUM_LEN/2))?
                            (SI_PB_NUM_LEN/2) : *(pucExtContent + 1);

            PAM_MEM_CPY_S(aucPhoneNumber, SI_PB_NUM_LEN/2, pANRContent+3, SI_PB_NUM_LEN/2 );
            PAM_MEM_CPY_S(aucPhoneNumber + (SI_PB_NUM_LEN/2), SI_PB_NUM_LEN/2, pucExtContent + 2, SI_PB_NUM_LEN/2 );

            SI_PB_BcdToAscii( (VOS_UINT8)(ucExtNumLen + (SI_PB_NUM_LEN/2)), aucPhoneNumber,
                                pstRecord->AdditionNumber[ucANROffset].Number, &pstRecord->AdditionNumber[ucANROffset].NumberLength);
        }
        else                                                                                /*??????????????????*/
        {
            ucAnrNumLen = ((pANRContent[1]-1) > (SI_PB_NUM_LEN/2))?
                          (SI_PB_NUM_LEN/2) : (pANRContent[1]-1);

            SI_PB_BcdToAscii(ucAnrNumLen, &pANRContent[3],
                            pstRecord->AdditionNumber[ucANROffset].Number,
                            &pstRecord->AdditionNumber[ucANROffset].NumberLength);
        }
    }

    return;
}


VOS_VOID SI_PB_TransEMLFromate(VOS_UINT8 ucEmailMaxLen, VOS_UINT8 *pEMLContent, SI_PB_RECORD_STRU *pstRecord)
{
    VOS_UINT32 i;
    VOS_UINT8  ucEmailLen = 0;

    for(i = 0; i < ucEmailMaxLen; i++)
    {
        if(0xFF == pEMLContent[i])
        {
            break;
        }

        ucEmailLen++;
    }

    if(0 == ucEmailLen)
    {
        pstRecord->Email.EmailLen = 0;
    }
    else
    {
        pstRecord->Email.EmailLen = ucEmailLen;

        pstRecord->ValidFlag = SI_PB_CONTENT_VALID;

        PAM_MEM_CPY_S(pstRecord->Email.Email, SI_PB_EMAIL_MAX_LEN, pEMLContent, ucEmailLen);
    }

    return;
}


VOS_UINT32 SI_PB_GetFileCntFromIndex(VOS_UINT16 ucIndex, VOS_UINT8 *pucFileCnt)
{
    VOS_UINT8                           i;
    VOS_UINT16                          usSum;

    usSum = 0;

    if(SI_PB_ADNMAX < gstPBCtrlInfo.ulADNFileNum)
    {
        PB_WARNING1_LOG("SI_PB_SearchResultProc: gstPBCtrlInfo.ulADNFileNum err", gstPBCtrlInfo.ulADNFileNum);
        return VOS_ERR;
    }

    for (i = 0; i < gstPBCtrlInfo.ulADNFileNum; i++)
    {
        if ((ucIndex <= (usSum + gstPBCtrlInfo.astADNInfo[i].ucRecordNum))
            && (ucIndex > usSum))
        {
            break;
        }
        else
        {
            usSum += gstPBCtrlInfo.astADNInfo[i].ucRecordNum;
        }
    }

    if (i >= gstPBCtrlInfo.ulADNFileNum)
    {
        *pucFileCnt = 0;
        return VOS_ERR;
    }
    else
    {
        *pucFileCnt = i + 1;
        return VOS_OK;
    }
}

#endif

#endif /* (FEATURE_ON == FEATURE_PHONE_SC) */




