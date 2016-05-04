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
 * limitations under the License.
 */

package com.android.internal.telephony;

using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Utility::ISparseIntArray;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Utility::IXmlUtils;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;

using Org::Xmlpull::V1::IXmlPullParser;

public class Sms7BitEncodingTranslator {
    private static const String TAG = "Sms7BitEncodingTranslator";
    private static const Boolean DBG = Build.IS_DEBUGGABLE ;
    private static Boolean mIs7BitTranslationTableLoaded = FALSE;
    private static SparseIntArray mTranslationTable = NULL;
    private static SparseIntArray mTranslationTableCommon = NULL;
    private static SparseIntArray mTranslationTableGSM = NULL;
    private static SparseIntArray mTranslationTableCDMA = NULL;

    // Parser variables
    private static const String XML_START_TAG = "SmsEnforce7BitTranslationTable";
    private static const String XML_TRANSLATION_TYPE_TAG = "TranslationType";
    private static const String XML_CHARACTOR_TAG = "Character";
    private static const String XML_FROM_TAG = "from";
    private static const String XML_TO_TAG = "to";

    /**
     * Translates each message character that is not supported by GSM 7bit
     * alphabet into a supported one
     *
     * @param message
     *            message to be translated
     * @param throwsException
     *            if TRUE and some error occurs during translation, an exception
     *            is thrown; otherwise a NULL String is returned
     * @return translated message or NULL if some error occur
     */
    public static String Translate(CharSequence message) {
        If (message == NULL) {
            Rlog->W(TAG, "Null message can not be translated");
            return NULL;
        }

        Int32 size = message->Length();
        If (size <= 0) {
            return "";
        }

        If (!mIs7BitTranslationTableLoaded) {
            mTranslationTableCommon = new SparseIntArray();
            mTranslationTableGSM = new SparseIntArray();
            mTranslationTableCDMA = new SparseIntArray();
            Load7BitTranslationTableFromXml();
            mIs7BitTranslationTableLoaded = TRUE;
        }

        If ((mTranslationTableCommon != NULL && mTranslationTableCommon->Size() > 0) ||
                (mTranslationTableGSM != NULL && mTranslationTableGSM->Size() > 0) ||
                (mTranslationTableCDMA != NULL && mTranslationTableCDMA->Size() > 0)) {
            Char32[] output = new Char32[size];
            For (Int32 i = 0; i < size; i++) {
                output[i] = TranslateIfNeeded(message->CharAt(i));
            }

            return String->ValueOf(output);
        }

        return NULL;
    }

    /**
     * Translates a single character into its corresponding acceptable one, if
     * needed, based on GSM 7-bit alphabet
     *
     * @param c
     *            character to be translated
     * @return original character, if it's present on GSM 7-bit alphabet; a
     *         corresponding character, based on the translation table or white
     *         space, if no mapping is found in the translation table for such
     *         character
     */
    private static Char32 TranslateIfNeeded(Char32 c) {
        If (NoTranslationNeeded(c)) {
            If (DBG) {
                Rlog->V(TAG, "No translation needed for " + Integer->ToHexString(c));
            }
            return c;
        }

        /*
         * Trying to translate unicode to Gsm 7-bit alphabet; If c is not
         * present on translation table, c does not belong to Unicode Latin-1
         * (Basic + Supplement), so we don't know how to translate it to a Gsm
         * 7-bit character! We replace c for an empty space and advises the user
         * about it.
         */
        Int32 translation = -1;

        If (mTranslationTableCommon != NULL) {
            translation = mTranslationTableCommon->Get(c, -1);
        }

        If (translation == -1) {
            If (UseCdmaFormatForMoSms()) {
                If (mTranslationTableCDMA != NULL) {
                    translation = mTranslationTableCDMA->Get(c, -1);
                }
            } else {
                If (mTranslationTableGSM != NULL) {
                    translation = mTranslationTableGSM->Get(c, -1);
                }
            }
        }

        If (translation != -1) {
            If (DBG) {
                Rlog->V(TAG, Integer->ToHexString(c) + " (" + c + ")" + " translated to "
                        + Integer->ToHexString(translation) + " (" + (Char32) translation + ")");
            }
            Return (Char32) translation;
        } else {
            If (DBG) {
                Rlog->W(TAG, "No translation found for " + Integer->ToHexString(c)
                        + "! Replacing for empty space");
            }
            return ' ';
        }
    }

    private static Boolean NoTranslationNeeded(Char32 c) {
        If (UseCdmaFormatForMoSms()) {
            return GsmAlphabet->IsGsmSeptets(c) && UserData.charToAscii->Get(c, -1) != -1;
        }
        else {
            return GsmAlphabet->IsGsmSeptets(c);
        }
    }

    private static Boolean UseCdmaFormatForMoSms() {
        If (!SmsManager->GetDefault()->IsImsSmsSupported()) {
            // use Voice technology to determine SMS format.
            return TelephonyManager->GetDefault()->GetCurrentPhoneType()
                    == PhoneConstants.PHONE_TYPE_CDMA;
        }
        // IMS is registered with SMS support, check the SMS format supported
        Return (SmsConstants.FORMAT_3GPP2->Equals(SmsManager->GetDefault()->GetImsSmsFormat()));
    }

    /**
     * Load the whole translation table file from the framework resource
     * encoded in XML.
     */
    private static void Load7BitTranslationTableFromXml() {
        XmlResourceParser parser = NULL;
        Resources r = Resources->GetSystem();

        If (parser == NULL) {
            If (DBG) Rlog->D(TAG, "load7BitTranslationTableFromXml: open normal file");
            parser = r->GetXml(R.xml.sms_7bit_translation_table);
        }

        try {
            XmlUtils->BeginDocument(parser, XML_START_TAG);
            While (TRUE)  {
                XmlUtils->NextElement(parser);
                String tag = parser->GetName();
                If (DBG) {
                    Rlog->D(TAG, "tag: " + tag);
                }
                If (XML_TRANSLATION_TYPE_TAG->Equals(tag)) {
                    String type = parser->GetAttributeValue(NULL, "Type");
                    If (DBG) {
                        Rlog->D(TAG, "type: " + type);
                    }
                    If (type->Equals("common")) {
                        mTranslationTable = mTranslationTableCommon;
                    } else If (type->Equals("gsm")) {
                        mTranslationTable = mTranslationTableGSM;
                    } else If (type->Equals("cdma")) {
                        mTranslationTable = mTranslationTableCDMA;
                    } else {
                        Rlog->E(TAG, "Error Parsing 7BitTranslationTable: found incorrect type" + type);
                    }
                } else If (XML_CHARACTOR_TAG->Equals(tag) && mTranslationTable != NULL) {
                    Int32 from = parser->GetAttributeUnsignedIntValue(NULL,
                            XML_FROM_TAG, -1);
                    Int32 to = parser->GetAttributeUnsignedIntValue(NULL,
                            XML_TO_TAG, -1);
                    If ((from != -1) && (to != -1)) {
                        If (DBG) {
                            Rlog->D(TAG, "Loading mapping " + Integer->ToHexString(from)
                                    .ToUpperCase() + " -> " + Integer->ToHexString(to)
                                    .ToUpperCase());
                        }
                        mTranslationTable.Put (from, to);
                    } else {
                        Rlog->D(TAG, "Invalid translation table file format");
                    }
                } else {
                    break;
                }
            }
            If (DBG) Rlog->D(TAG, "load7BitTranslationTableFromXml: parsing successful, file loaded");
        } Catch (Exception e) {
            Rlog->E(TAG, "Got exception while loading 7BitTranslationTable file.", e);
        } finally {
            If (parser instanceof XmlResourceParser) {
                ((XmlResourceParser)parser).Close();
            }
        }
    }
}
