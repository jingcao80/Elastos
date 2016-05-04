/*
 * Copyright (c) 2014, Linux Foundation. All rights reserved.
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.uicc;

/**
 * {@hide}
 */
public interface IccConstants {
    // GSM SIM file ids from TS 51.011
    static const Int32 EF_ADN = 0x6F3A;
    static const Int32 EF_FDN = 0x6F3B;
    static const Int32 EF_GID1 = 0x6F3E;
    static const Int32 EF_SDN = 0x6F49;
    static const Int32 EF_EXT1 = 0x6F4A;
    static const Int32 EF_EXT2 = 0x6F4B;
    static const Int32 EF_EXT3 = 0x6F4C;
    static const Int32 EF_EXT5 = 0x6F4E;
    static const Int32 EF_EXT6 = 0x6fc8;   // Ext record for EF[MBDN]
    static const Int32 EF_MWIS = 0x6FCA;
    static const Int32 EF_MBDN = 0x6fc7;
    static const Int32 EF_PNN = 0x6fc5;
    static const Int32 EF_OPL = 0x6fc6;
    static const Int32 EF_SPN = 0x6F46;
    static const Int32 EF_SMS = 0x6F3C;
    static const Int32 EF_ICCID = 0x2fe2;
    static const Int32 EF_AD = 0x6FAD;
    static const Int32 EF_MBI = 0x6fc9;
    static const Int32 EF_MSISDN = 0x6f40;
    static const Int32 EF_SPDI = 0x6fcd;
    static const Int32 EF_SST = 0x6f38;
    static const Int32 EF_CFIS = 0x6FCB;
    static const Int32 EF_IMG = 0x4f20;

    // USIM SIM file ids from TS 131.102
    public static const Int32 EF_PBR = 0x4F30;
    public static const Int32 EF_LI = 0x6F05;

    // GSM SIM file ids from CPHS (phase 2, version 4.2) CPHS4_2.WW6
    static const Int32 EF_MAILBOX_CPHS = 0x6F17;
    static const Int32 EF_VOICE_MAIL_INDICATOR_CPHS = 0x6F11;
    static const Int32 EF_CFF_CPHS = 0x6F13;
    static const Int32 EF_SPN_CPHS = 0x6f14;
    static const Int32 EF_SPN_SHORT_CPHS = 0x6f18;
    static const Int32 EF_INFO_CPHS = 0x6f16;
    static const Int32 EF_CSP_CPHS = 0x6f15;

    // CDMA RUIM file ids from 3GPP2 C.S0023-0
    // RUIM EF stores The (up to) 56-bit electronic identification
    // Number (ID) unique to the R-UIM. (Removable UIM_ID)
    static const Int32 EF_RUIM_ID = 0x6f31;
    static const Int32 EF_CST = 0x6f32;
    static const Int32 EF_RUIM_SPN =0x6F41;
    static const Int32 EF_MODEL = 0x6F90;

    // ETSI TS.102.221
    static const Int32 EF_PL = 0x2F05;
    // 3GPP2 C.S0065
    static const Int32 EF_CSIM_LI = 0x6F3A;
    static const Int32 EF_CSIM_SPN =0x6F41;
    static const Int32 EF_CSIM_MDN = 0x6F44;
    static const Int32 EF_CSIM_IMSIM = 0x6F22;
    static const Int32 EF_CSIM_CDMAHOME = 0x6F28;
    static const Int32 EF_CSIM_EPRL = 0x6F5A;
    static const Int32 EF_CSIM_MODEL = 0x6F81;
    static const Int32 EF_CSIM_PRL = 0x6F30;
    // C.S0074-Av1.0 Section 4
    static const Int32 EF_CSIM_MLPL = 0x4F20;
    static const Int32 EF_CSIM_MSPL = 0x4F21;

    //ISIM access
    static const Int32 EF_IMPU = 0x6f04;
    static const Int32 EF_IMPI = 0x6f02;
    static const Int32 EF_DOMAIN = 0x6f03;
    static const Int32 EF_IST = 0x6f07;
    static const Int32 EF_PCSCF = 0x6f09;
    static const Int32 EF_PSI = 0x6fe5;

    //plmnwact
    static const Int32 EF_PLMNWACT = 0x6F60;

    // SMS record length from TS 51.011 10.5.3
    static public final Int32 SMS_RECORD_LENGTH = 176;
    // SMS record length from C.S0023 3.4.27
    static public final Int32 CDMA_SMS_RECORD_LENGTH = 255;

    static const String MF_SIM = "3F00";
    static const String DF_TELECOM = "7F10";
    static const String DF_PHONEBOOK = "5F3A";
    static const String DF_GRAPHICS = "5F50";
    static const String DF_GSM = "7F20";
    static const String DF_CDMA = "7F25";
    static const String DF_MMSS = "5F3C";

    //UICC access
    static const String DF_ADF = "7FFF";
}
