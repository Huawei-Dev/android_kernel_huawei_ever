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

#ifndef _MDRV_OM_COMMON_H_
#define _MDRV_OM_COMMON_H_

#ifdef _cplusplus
extern "C"
{
#endif

#define NR_MODID_TAG      (0xB)
#define MODID_TAG         (32)
#define MODID_SYS         (28)
#define MODID_MODULE      (24)
#define MODID_CODE        (16)
/************************************************
31-28bit	 27-24bit	  23-16bit	  15-0bit
NRTAG        ??????????	  ????????	  ??????????????????????
**************************************************/

typedef enum nr_sys
{
    NR_SYSTEM_AP        = 0,
    NR_SYSTEM_CCPU      = 1,
    NR_SYSTEM_L2HAC     = 2,
    NR_SYSTEM_HL1C      = 3,
    NR_SYSTEM_LL1C      = 4,
}NR_SYS;



typedef enum nr_module
{
    MODULE_BSP     = 0,
    MODULE_MSP     ,
    MODULE_OSA     ,
    MODULE_OAM     ,
    MODULE_NAS     ,
    MODULE_AS      ,
    MODULE_PS      ,
    MODULE_PHY     ,
}NR_MODULE;


#define OM_MODID_HEAD(__subsys,__module)   ((unsigned int)(NR_MODID_TAG << (MODID_SYS))|(unsigned int)((__subsys)<< MODID_MODULE)| (unsigned int)((__module) << MODID_CODE))


#define OM_GET_MODULE(error_code)          ((unsigned int)(((error_code)&((unsigned int)(0xff << MODID_CODE))>> MODID_CODE)))



enum OM_FIELD_ID_ENUM
{
    OM_AP_FIELD_BEGIN   = (0x01200000),
    OM_AP_OSA           = (OM_AP_FIELD_BEGIN),
    OM_AP_DIAG          = (OM_AP_FIELD_BEGIN + 1),
    OM_AP_FIELD_END,
#ifndef __OS_NRCCPU__
    OM_CP_FIELD_BEGIN   = (0x02200000),
    OM_CP_OSA           = (OM_CP_FIELD_BEGIN),
    OM_CP_MSP_SLEEP     = (OM_CP_FIELD_BEGIN + 1),
    OM_CP_TLPS          = (OM_CP_FIELD_BEGIN + 2),
    OM_CP_FTM_MNTN      = (OM_CP_FIELD_BEGIN + 3),
    OM_CP_GUAS          = (OM_CP_FIELD_BEGIN + 4),
    OM_CP_DIAG          = (OM_CP_FIELD_BEGIN + 5),
    OM_CP_GUNAS         = (OM_CP_FIELD_BEGIN + 6),
    OM_CP_CPROC         = (OM_CP_FIELD_BEGIN + 7),
    OM_CP_CAS           = (OM_CP_FIELD_BEGIN + 8),
    OM_CP_CNAS          = (OM_CP_FIELD_BEGIN + 9),
    OM_CP_MSCC          = (OM_CP_FIELD_BEGIN + 10),
    OM_CP_CTTF          = (OM_CP_FIELD_BEGIN + 11),
    OM_CP_USIMM0        = (OM_CP_FIELD_BEGIN + 12),
    OM_CP_USIMM1        = (OM_CP_FIELD_BEGIN + 13),
    OM_CP_TLPS1         = (OM_CP_FIELD_BEGIN + 14),
    OM_CP_GUPHY         = (OM_CP_FIELD_BEGIN + 15),
    OM_CP_RFDSP         = (OM_CP_FIELD_BEGIN + 16),
    OM_CP_GUCSLEEP      = (OM_CP_FIELD_BEGIN + 17),
    OM_CP_TAF           = (OM_CP_FIELD_BEGIN + 18),
    OM_CP_FIELD_END,
#else
    OM_CP_FIELD_BEGIN   = (0x07200000),
    OM_CP_OSA           = (OM_CP_FIELD_BEGIN),
    OM_CP_MSP_SLEEP     = (OM_CP_FIELD_BEGIN + 1),
    OM_CP_TLPS          = (OM_CP_FIELD_BEGIN + 2),
    OM_CP_FTM_MNTN      = (OM_CP_FIELD_BEGIN + 3),
    OM_CP_GUAS          = (OM_CP_FIELD_BEGIN + 4),
    OM_CP_DIAG          = (OM_CP_FIELD_BEGIN + 5),
    OM_CP_GUNAS         = (OM_CP_FIELD_BEGIN + 6),
    OM_CP_CPROC         = (OM_CP_FIELD_BEGIN + 7),
    OM_CP_CAS           = (OM_CP_FIELD_BEGIN + 8),
    OM_CP_CNAS          = (OM_CP_FIELD_BEGIN + 9),
    OM_CP_MSCC          = (OM_CP_FIELD_BEGIN + 10),
    OM_CP_CTTF          = (OM_CP_FIELD_BEGIN + 11),
    OM_CP_USIMM0        = (OM_CP_FIELD_BEGIN + 12),
    OM_CP_USIMM1        = (OM_CP_FIELD_BEGIN + 13),
    OM_CP_TLPS1         = (OM_CP_FIELD_BEGIN + 14),
    OM_CP_GUPHY         = (OM_CP_FIELD_BEGIN + 15),
    OM_CP_RFDSP         = (OM_CP_FIELD_BEGIN + 16),
    OM_CP_GUCSLEEP      = (OM_CP_FIELD_BEGIN + 17),
    OM_CP_NRNAS         = (OM_CP_FIELD_BEGIN + 18),

#endif
    OM_HAC_FIELD_BEGIN   = (0x08200000),
    OM_HAC_OSA           = (OM_HAC_FIELD_BEGIN),
    OM_HAC_MSP_SLEEP     = (OM_HAC_FIELD_BEGIN + 1),
    OM_HAC_TLPS          = (OM_HAC_FIELD_BEGIN + 2),
    OM_HAC_FTM_MNTN      = (OM_HAC_FIELD_BEGIN + 3),
    OM_HAC_GUAS          = (OM_HAC_FIELD_BEGIN + 4),
    OM_HAC_DIAG          = (OM_HAC_FIELD_BEGIN + 5),
    OM_HAC_GUNAS         = (OM_HAC_FIELD_BEGIN + 6),
    OM_HAC_CPROC         = (OM_HAC_FIELD_BEGIN + 7),
    OM_HAC_CAS           = (OM_HAC_FIELD_BEGIN + 8),
    OM_HAC_CNAS          = (OM_HAC_FIELD_BEGIN + 9),
    OM_HAC_MSCC          = (OM_HAC_FIELD_BEGIN + 10),
    OM_HAC_CTTF          = (OM_HAC_FIELD_BEGIN + 11),
    OM_HAC_USIMM0        = (OM_HAC_FIELD_BEGIN + 12),
    OM_HAC_USIMM1        = (OM_HAC_FIELD_BEGIN + 13),
    OM_HAC_TLPS1         = (OM_HAC_FIELD_BEGIN + 14),
    OM_HAC_GUPHY         = (OM_HAC_FIELD_BEGIN + 15),
    OM_HAC_RFDSP         = (OM_HAC_FIELD_BEGIN + 16),
    OM_HAC_GUCSLEEP      = (OM_HAC_FIELD_BEGIN + 17),
    OM_HAC_FIELD_END,


};



enum
{
    DUMP_OUT_VOICE = 0,
    DUMP_IN_VOICE = 1,
    DUMP_VOICE_BUTT,
};
typedef int dump_handle;
typedef void (*dump_hook)(void);
typedef void (*dump_notify)(void*);


/*****************************************************************************
 * ?? ?? ??  : mdrv_om_system_error
 * ????????  : ????????????????????????????????????????????????????
 * ????????  : modId:   ????????
 *             arg1:    ??????????????1
 *             arg2:    ??????????????2
 *             arg3:    ??????????????3??
 *                       buf??????dump????????????????dump??????
 *                       ??dump????????field????????????????
 *             arg3Length:  arg????buf??????????????max_len(v7??4k??v8??1k)
 *                       ????max_len??????????max_len????
 * ????????  : ????
 * ?? ?? ??  : ????
 *****************************************************************************/
void mdrv_om_system_error(int modId, int arg1, int arg2, char * arg3, int arg3Length);

/*****************************************************************************
 * ?? ?? ??  : mdrv_om_register_field
 * ????????  : ??????????????????????????????????????????????????????????????????
 *             ??system_error??????????????dump??????????field????????
 * ????????  : field_id :   ????buffer??????field id
 *             field_name:  field????????????????????????????
 *             virt_addr:   ????????????????????????????????????????dump??????????
 *             phy_addr:    ??????????????????????
 *             length:      ????buffer??????
 *             version:     ????buffer????????
 *
 * ????????  : NULL:  ????buf????
 *
 * ?? ?? ??  : ????
 *****************************************************************************/
unsigned char* mdrv_om_register_field(unsigned int field_id, char * field_name, void * virt_addr,
                            void * phy_addr, unsigned int length, unsigned short version);

/*****************************************************************************
 * ?? ?? ??  : mdrv_om_get_field_addr
 * ????????  : ????field_id??????dump??????????????????????????
 *
 * ????????  : field_id: ????mdrv_om_register_field??????dump??????????ID
 *
 * ????????  : ??
 *
 * ?? ?? ??  : >0   ??????????field????????
 *             0    ????
 *
 *****************************************************************************/
unsigned char * mdrv_om_get_field_addr(unsigned int field_id);

/*****************************************************************************
 * ?? ?? ??  : mdrv_om_get_field_phy_addr
 * ????????  : ????field_id??????dump??????????????????????????
 *
 * ????????  : field_id: ????mdrv_om_register_field??????dump??????????ID
 *
 * ????????  : ??
 *
 * ?? ?? ??  : >0   ??????????field????????
 *             0    ????
 *
 *****************************************************************************/
unsigned char * mdrv_om_get_field_phy_addr(unsigned int field_id);

/*****************************************************************************
 * ?? ?? ??  : mdrv_om_register_callback
 * ????????  : DUMP??????????????????dump????????????????????????????????????????
 * system_error????????????????????????????????????????
 *
 * ????????  : name :   ??????????????????
 *             pFunc:   ????????????????????
 *
 * ????????  : ??
 *
 * ?? ?? ??     : >0  ??????????????????????
 *        -1, DUMP_HANDLE_INVALID  ????
 *
 *****************************************************************************/
dump_handle mdrv_om_register_callback(char * name, dump_hook pFunc);

/*****************************************************************************
  ?? ?? ??  : mdrv_om_unregister_callback
  ????????  : ????DUMP??????????

  ????????  : handle :   mdrv_om_register_callback????????????????

  ????????  : ??

  ?? ?? ??     : 0  ????
  -1   ????

 *****************************************************************************/
 int mdrv_om_unregister_callback(dump_handle handle);


#ifdef _cplusplus
}
#endif
#endif


