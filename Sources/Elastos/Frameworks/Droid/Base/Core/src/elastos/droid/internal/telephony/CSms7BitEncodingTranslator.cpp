//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/internal/telephony/CSms7BitEncodingTranslator.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/internal/telephony/GsmAlphabet.h"
#include "elastos/droid/telephony/CSmsManagerHelper.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/utility/CSparseInt32Array.h"
#include "elastos/droid/R.h"

#include <elastos/core/IntegralToString.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Utility::XmlUtils;
//using Elastos::Droid::Internal::Telephony::Cdma::Sms::IUserData;
using Elastos::Droid::Os::Build;
//using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ISmsManagerHelper;
using Elastos::Droid::Telephony::CSmsManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::R;

using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_SINGLETON_IMPL(CSms7BitEncodingTranslator)

CAR_INTERFACE_IMPL(CSms7BitEncodingTranslator, Singleton, ISms7BitEncodingTranslator)

const String CSms7BitEncodingTranslator::TAG("CSms7BitEncodingTranslator");
const Boolean CSms7BitEncodingTranslator::DBG = Build::IS_DEBUGGABLE;
Boolean CSms7BitEncodingTranslator::mIs7BitTranslationTableLoaded = FALSE;
AutoPtr<ISparseInt32Array> CSms7BitEncodingTranslator::mTranslationTable;
AutoPtr<ISparseInt32Array> CSms7BitEncodingTranslator::mTranslationTableCommon;
AutoPtr<ISparseInt32Array> CSms7BitEncodingTranslator::mTranslationTableGSM;
AutoPtr<ISparseInt32Array> CSms7BitEncodingTranslator::mTranslationTableCDMA;

// Parser variables
const String CSms7BitEncodingTranslator::XML_START_TAG("SmsEnforce7BitTranslationTable");
const String CSms7BitEncodingTranslator::XML_TRANSLATION_TYPE_TAG("TranslationType");
const String CSms7BitEncodingTranslator::XML_CHARACTOR_TAG("Character");
const String CSms7BitEncodingTranslator::XML_FROM_TAG("from");
const String CSms7BitEncodingTranslator::XML_TO_TAG("to");

ECode CSms7BitEncodingTranslator::Translate(
    /* [in] */ ICharSequence* message,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (message == NULL) {
//        Rlog->W(TAG, "Null message can not be translated");
        *result = String(NULL);
        return NOERROR;
    }

    Int32 size = 0;
    message->GetLength(&size);
    if (size <= 0) {
        *result = String("");
        return NOERROR;
    }

    if (!mIs7BitTranslationTableLoaded) {
        CSparseInt32Array::New((ISparseInt32Array**)&mTranslationTableCommon);
        CSparseInt32Array::New((ISparseInt32Array**)&mTranslationTableGSM);
        CSparseInt32Array::New((ISparseInt32Array**)&mTranslationTableCDMA);
        Load7BitTranslationTableFromXml();
        mIs7BitTranslationTableLoaded = TRUE;
    }

    Int32 sizeCommon = 0, sizeGSM = 0, sizeCDMA = 0;
    if ((mTranslationTableCommon != NULL && (mTranslationTableCommon->GetSize(&sizeCommon), sizeCommon) > 0) ||
            (mTranslationTableGSM != NULL && (mTranslationTableGSM->GetSize(&sizeGSM), sizeGSM) > 0) ||
            (mTranslationTableCDMA != NULL && (mTranslationTableCDMA->GetSize(&sizeCDMA), sizeCDMA) > 0)) {
        AutoPtr<ArrayOf<Char32> > output = ArrayOf<Char32>::Alloc(size);
        Char32 c;
        for (Int32 i = 0; i < size; i++) {
            message->GetCharAt(i, &c);
            output->Set(i, TranslateIfNeeded(c));
        }

        String str(*output);
        *result = str;
        return NOERROR;
    }
    *result = String(NULL);
    return NOERROR;
}

Char32 CSms7BitEncodingTranslator::TranslateIfNeeded(
    /* [in] */ Char32 c)
{
    if (NoTranslationNeeded(c)) {
//        if (DBG) {
//            Rlog->V(TAG, String("No translation needed for ") + IntegralToString::ToHexString(c));
//        }
        return c;
    }

    /*
     * Trying to translate unicode to Gsm 7-bit alphabet; if c is not
     * present on translation table, c does not belong to Unicode Latin-1
     * (Basic + Supplement), so we don't know how to translate it to a Gsm
     * 7-bit character! We replace c for an empty space and advises the user
     * about it.
     */
    Int32 translation = -1;

    if (mTranslationTableCommon != NULL) {
        mTranslationTableCommon->Get(c, -1, &translation);
    }

    if (translation == -1) {
        if (UseCdmaFormatForMoSms()) {
            if (mTranslationTableCDMA != NULL) {
                mTranslationTableCDMA->Get(c, -1, &translation);
            }
        }
        else {
            if (mTranslationTableGSM != NULL) {
                mTranslationTableGSM->Get(c, -1, &translation);
            }
        }
    }

    if (translation != -1) {
//        if (DBG) {
//            Rlog->V(TAG, Integer->ToHexString(c) + " (" + c + ")" + " translated to "
//                    + Integer->ToHexString(translation) + " (" + (Char32) translation + ")");
//        }
        return (Char32) translation;
    }
    else {
//        if (DBG) {
//            Rlog->W(TAG, "No translation found for " + Integer->ToHexString(c)
//                    + "! Replacing for empty space");
//        }
        return ' ';
    }
}

Boolean CSms7BitEncodingTranslator::NoTranslationNeeded(
    /* [in] */ Char32 c)
{
    Boolean bResult = FALSE;
    GsmAlphabet::IsGsmSeptets(c, &bResult);
    if (UseCdmaFormatForMoSms()) {
//        Int32 value;
//        UserData::charToAscii()->Get(c, -1, &value);
//        return (value != -1);
        //TODO
        return bResult;
    }
    else {
        return bResult;
    }
}

Boolean CSms7BitEncodingTranslator::UseCdmaFormatForMoSms()
{
    AutoPtr<ISmsManagerHelper> sMHelper;
    AutoPtr<ISmsManager> sM;;
    CSmsManagerHelper::AcquireSingleton((ISmsManagerHelper**)&sMHelper);
    sMHelper->GetDefault((ISmsManager**)&sM);

    Boolean bSupported = FALSE;
    sM->IsImsSmsSupported(&bSupported);
    if (!bSupported) {
        // use Voice technology to determine SMS format.
        AutoPtr<ITelephonyManagerHelper> tMHelper;
        AutoPtr<ITelephonyManager> tM;
        Int32 type;

        CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tMHelper);
        tMHelper->GetDefault((ITelephonyManager**)&tM);
        tM->GetCurrentPhoneType(&type);
        return type == IPhoneConstants::PHONE_TYPE_CDMA;
    }
    // IMS is registered with SMS support, check the SMS format supported
    String str;
    sM->GetImsSmsFormat(&str);
    return ISmsConstants::FORMAT_3GPP2.Equals(str);
}

void CSms7BitEncodingTranslator::Load7BitTranslationTableFromXml()
{
    AutoPtr<IXmlResourceParser> parser;
    AutoPtr<IResourcesHelper> rHelper;
    AutoPtr<IResources> r;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&rHelper);
    rHelper->GetSystem((IResources**)&r);

    if (parser == NULL) {
//        if (DBG) Rlog->D(TAG, "load7BitTranslationTableFromXml: open normal file");
        r->GetXml(R::xml::sms_7bit_translation_table, (IXmlResourceParser**)&parser);
    }

//    try {
    XmlUtils::BeginDocument(IXmlPullParser::Probe(parser), XML_START_TAG);
    while (TRUE)  {
        XmlUtils::NextElement(IXmlPullParser::Probe(parser));
        String tag;
        IXmlPullParser::Probe(parser)->GetName(&tag);
//        if (DBG) {
//            Rlog->D(TAG, "tag: " + tag);
//        }
        if (XML_TRANSLATION_TYPE_TAG.Equals(tag)) {
            String type;
            IXmlPullParser::Probe(parser)->GetAttributeValue(String(NULL), String("Type"), &type);
//            if (DBG) {
//                Rlog->D(TAG, "type: " + type);
//            }
            if (type.Equals("common")) {
                mTranslationTable = mTranslationTableCommon;
            }
            else if (type.Equals("gsm")) {
                mTranslationTable = mTranslationTableGSM;
            }
            else if (type.Equals("cdma")) {
                mTranslationTable = mTranslationTableCDMA;
            }
            else {
//                Rlog->E(TAG, "Error Parsing 7BitTranslationTable: found incorrect type" + type);
            }
        }
        else if (XML_CHARACTOR_TAG.Equals(tag) && mTranslationTable != NULL) {
            AutoPtr<IAttributeSet> aset = IAttributeSet::Probe(parser);
            Int32 from, to;
            aset->GetAttributeUnsignedIntValue(String(NULL), XML_FROM_TAG, -1, &from);
            aset->GetAttributeUnsignedIntValue(String(NULL), XML_TO_TAG, -1, &to);
            if ((from != -1) && (to != -1)) {
                if (DBG) {
                    // Rlog::D(TAG, "Loading mapping " + IntegralToString::ToHexString(from)
                    //    + " -> " + IntegralToString::ToHexString(to));
                }
                mTranslationTable->Put(from, to);
            }
            else {
//                Rlog->D(TAG, "Invalid translation table file format");
            }
        }
        else {
            break;
        }
    }
//    if (DBG) Rlog->D(TAG, "load7BitTranslationTableFromXml: parsing successful, file loaded");
//    } Catch (Exception e) {
//        Rlog->E(TAG, "Got exception while loading 7BitTranslationTable file.", e);

    if (IXmlResourceParser::Probe(parser) != NULL) {
        IXmlResourceParser::Probe(parser)->Close();
    }
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
