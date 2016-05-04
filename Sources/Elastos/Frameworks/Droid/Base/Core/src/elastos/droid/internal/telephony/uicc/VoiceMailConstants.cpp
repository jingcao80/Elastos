/*
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

using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Utility::IXml;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Utility::IHashMap;
using Elastos::IO::IFileReader;
using Elastos::IO::IFile;

using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Droid::Internal::Utility::IXmlUtils;

/**
 * {@hide}
 */
class VoiceMailConstants {
    private HashMap<String, String[]> CarrierVmMap;


    static const String LOG_TAG = "VoiceMailConstants";
    static const String PARTNER_VOICEMAIL_PATH ="etc/voicemail-conf.xml";

    static const Int32 NAME = 0;
    static const Int32 NUMBER = 1;
    static const Int32 TAG = 2;
    static const Int32 SIZE = 3;

    VoiceMailConstants () {
        CarrierVmMap = new HashMap<String, String[]>();
        LoadVoiceMail();
    }

    Boolean ContainsCarrier(String carrier) {
        return CarrierVmMap->ContainsKey(carrier);
    }

    String GetCarrierName(String carrier) {
        String[] data = CarrierVmMap->Get(carrier);
        return data[NAME];
    }

    String GetVoiceMailNumber(String carrier) {
        String[] data = CarrierVmMap->Get(carrier);
        return data[NUMBER];
    }

    String GetVoiceMailTag(String carrier) {
        String[] data = CarrierVmMap->Get(carrier);
        return data[TAG];
    }

    private void LoadVoiceMail() {
        FileReader vmReader;

        final File vmFile = new File(Environment->GetRootDirectory(),
                PARTNER_VOICEMAIL_PATH);

        try {
            vmReader = new FileReader(vmFile);
        } Catch (FileNotFoundException e) {
            Rlog->W(LOG_TAG, "Can't open " +
                    Environment->GetRootDirectory() + "/" + PARTNER_VOICEMAIL_PATH);
            return;
        }

        try {
            XmlPullParser parser = Xml->NewPullParser();
            parser->SetInput(vmReader);

            XmlUtils->BeginDocument(parser, "voicemail");

            While (TRUE) {
                XmlUtils->NextElement(parser);

                String name = parser->GetName();
                If (!"voicemail".Equals(name)) {
                    break;
                }

                String[] data = new String[SIZE];
                String numeric = parser->GetAttributeValue(NULL, "numeric");
                data[NAME]     = parser->GetAttributeValue(NULL, "carrier");
                data[NUMBER]   = parser->GetAttributeValue(NULL, "vmnumber");
                data[TAG]      = parser->GetAttributeValue(NULL, "vmtag");

                CarrierVmMap->Put(numeric, data);
            }
        } Catch (XmlPullParserException e) {
            Rlog->W(LOG_TAG, "Exception in Voicemail parser " + e);
        } Catch (IOException e) {
            Rlog->W(LOG_TAG, "Exception in Voicemail parser " + e);
        } finally {
            try {
                If (vmReader != NULL) {
                    vmReader->Close();
                }
            } Catch (IOException e) {}
        }
    }
}
