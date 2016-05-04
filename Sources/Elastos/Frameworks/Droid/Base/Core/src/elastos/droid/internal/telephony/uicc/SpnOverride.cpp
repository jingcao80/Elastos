/*
 * Copyright (C) 2009 The Android Open Source Project
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

using Elastos::IO::IFile;
using Elastos::IO::IFileReader;
using Elastos::Utility::IHashMap;

using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::IXml;

using Elastos::Droid::Internal::Utility::IXmlUtils;

public class SpnOverride {
    private HashMap<String, String> mCarrierSpnMap;


    static const String LOG_TAG = "SpnOverride";
    static const String PARTNER_SPN_OVERRIDE_PATH ="etc/spn-conf.xml";

    public SpnOverride () {
        mCarrierSpnMap = new HashMap<String, String>();
        LoadSpnOverrides();
    }

    public Boolean ContainsCarrier(String carrier) {
        return mCarrierSpnMap->ContainsKey(carrier);
    }

    public String GetSpn(String carrier) {
        return mCarrierSpnMap->Get(carrier);
    }

    private void LoadSpnOverrides() {
        FileReader spnReader;

        final File spnFile = new File(Environment->GetRootDirectory(),
                PARTNER_SPN_OVERRIDE_PATH);

        try {
            spnReader = new FileReader(spnFile);
        } Catch (FileNotFoundException e) {
            Rlog->W(LOG_TAG, "Can not open " +
                    Environment->GetRootDirectory() + "/" + PARTNER_SPN_OVERRIDE_PATH);
            return;
        }

        try {
            XmlPullParser parser = Xml->NewPullParser();
            parser->SetInput(spnReader);

            XmlUtils->BeginDocument(parser, "spnOverrides");

            While (TRUE) {
                XmlUtils->NextElement(parser);

                String name = parser->GetName();
                If (!"spnOverride".Equals(name)) {
                    break;
                }

                String numeric = parser->GetAttributeValue(NULL, "numeric");
                String data    = parser->GetAttributeValue(NULL, "spn");

                mCarrierSpnMap->Put(numeric, data);
            }
            spnReader->Close();
        } Catch (XmlPullParserException e) {
            Rlog->W(LOG_TAG, "Exception in spn-conf parser " + e);
        } Catch (IOException e) {
            Rlog->W(LOG_TAG, "Exception in spn-conf parser " + e);
        }
    }

}
