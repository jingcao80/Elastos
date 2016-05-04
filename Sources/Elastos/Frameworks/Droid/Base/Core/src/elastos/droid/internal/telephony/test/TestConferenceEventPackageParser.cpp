/*
 * Copyright (C) 2014 The Android Open Source Project
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
 * limitations under the License
 */

package com.android.internal.telephony.test;

using Elastos::Droid::Ims::IImsConferenceState;
using Elastos::Droid::Internal::Utility::IXmlUtils;

using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::IXml;

using Elastos::IO::IBufferedInputStream;
using Elastos::IO::IInputStream;

/**
 * Implements a basic XML parser used to parse test IMS conference event packages which can be
 * injected into the IMS framework via the {@link com.android.internal.telephony.TelephonyTester}.
 * <pre>
 * {@code
 * <xml>
 *     <participant>
 *         <user>tel:+16505551212</user>
 *         <display-text>Joe Q. Public</display-text>
 *         <endpoint>sip:+16505551212@ims-test-provider.com</endpoint>
 *         <status>connected</status>
 *     </participant>
 * </xml>
 * }
 * </pre>
 * <p>
 * Note: This XML format is similar to the information stored in the
 * {@link com.android.ims.ImsConferenceState} parcelable.  The {@code status} values expected in the
 * XML are those found in the {@code ImsConferenceState} Class (e.g.
 * {@link com.android.ims.ImsConferenceState#STATUS_CONNECTED}).
 * <p>
 * Place a file formatted similar to above in /data/data/com.android.phone/files/ and invoke the
 * following command while you have an ongoing IMS call:
 * <pre>
 *     adb shell am broadcast
 *          -a com.android.internal.telephony.TestConferenceEventPackage
 *          -e filename test.xml
 * </pre>
 */
public class TestConferenceEventPackageParser {
    private static const String LOG_TAG = "TestConferenceEventPackageParser";
    private static const String PARTICIPANT_TAG = "participant";

    /**
     * The XML input stream to parse.
     */
    private InputStream mInputStream;

    /**
     * Constructs an input of the conference event package parser for the given input stream.
     *
     * @param inputStream The input stream.
     */
    public TestConferenceEventPackageParser(InputStream inputStream) {
        mInputStream = inputStream;
    }

    /**
     * Parses the conference event package XML file and returns an
     * {@link com.android.ims.ImsConferenceState} instance containing the participants described in
     * the XML file.
     *
     * @return The {@link com.android.ims.ImsConferenceState} instance.
     */
    public ImsConferenceState Parse() {
        ImsConferenceState conferenceState = new ImsConferenceState();

        XmlPullParser parser;
        try {
            parser = Xml->NewPullParser();
            parser->SetInput(mInputStream, NULL);
            parser->NextTag();

            Int32 outerDepth = parser->GetDepth();
            While (XmlUtils->NextElementWithin(parser, outerDepth)) {
                If (parser->GetName()->Equals(PARTICIPANT_TAG)) {
                    Logger::V(LOG_TAG, "Found participant.");
                    Bundle participant = ParseParticipant(parser);
                    conferenceState.mParticipants->Put(participant->GetString(
                            ImsConferenceState.ENDPOINT), participant);
                }
            }
        } Catch (IOException | XmlPullParserException e) {
            Logger::E(LOG_TAG, "Failed to read test conference event package from XML file", e);
            return NULL;
        } finally {
            try {
                mInputStream->Close();
            } Catch (IOException e) {
                Logger::E(LOG_TAG, "Failed to close test conference event package InputStream", e);
                return NULL;
            }
        }

        return conferenceState;
    }

    /**
     * Parses a participant record from a conference event package XML file.
     *
     * @param parser The XML parser.
     * @return {@link Bundle} containing the participant information.
     */
    private Bundle ParseParticipant(XmlPullParser parser)
            throws IOException, XmlPullParserException {
        Bundle bundle = new Bundle();

        String user = "";
        String displayText = "";
        String endpoint = "";
        String status = "";

        Int32 outerDepth = parser->GetDepth();
        While (XmlUtils->NextElementWithin(parser, outerDepth)) {
            If (parser->GetName()->Equals(ImsConferenceState.USER)) {
                parser->Next();
                user = parser->GetText();
            } else If (parser->GetName()->Equals(ImsConferenceState.DISPLAY_TEXT)) {
                parser->Next();
                displayText = parser->GetText();
            }  else If (parser->GetName()->Equals(ImsConferenceState.ENDPOINT)) {
                parser->Next();
                endpoint = parser->GetText();
            }  else If (parser->GetName()->Equals(ImsConferenceState.STATUS)) {
                parser->Next();
                status = parser->GetText();
            }
        }

        Logger::V(LOG_TAG, "User: "+user);
        Logger::V(LOG_TAG, "DisplayText: "+displayText);
        Logger::V(LOG_TAG, "Endpoint: "+endpoint);
        Logger::V(LOG_TAG, "Status: "+status);

        bundle->PutString(ImsConferenceState.USER, user);
        bundle->PutString(ImsConferenceState.DISPLAY_TEXT, displayText);
        bundle->PutString(ImsConferenceState.ENDPOINT, endpoint);
        bundle->PutString(ImsConferenceState.STATUS, status);

        return bundle;
    }
}
