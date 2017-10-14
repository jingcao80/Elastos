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

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/VoiceMailConstants.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Environment;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Core::CoreUtils;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IReader;
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                          VoiceMailConstants
//=====================================================================
const String VoiceMailConstants::LOGTAG("VoiceMailConstants");
const String VoiceMailConstants::PARTNER_VOICEMAIL_PATH("etc/voicemail-conf.xml");
const Int32 VoiceMailConstants::NAME;
const Int32 VoiceMailConstants::NUMBER;
const Int32 VoiceMailConstants::TAG;
const Int32 VoiceMailConstants::SIZE;

VoiceMailConstants::VoiceMailConstants()
{
    CHashMap::New((IHashMap**)&mCarrierVmMap);
    LoadVoiceMail();
}

ECode VoiceMailConstants::ContainsCarrier(
    /* [in] */ const String& carrier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mCarrierVmMap->ContainsKey(CoreUtils::Convert(carrier), result);
}

ECode VoiceMailConstants::GetCarrierName(
    /* [in] */ const String& carrier,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mCarrierVmMap->Get(CoreUtils::Convert(carrier), (IInterface**)&p);
    AutoPtr<IArrayOf> arrayOf = IArrayOf::Probe(p);
    AutoPtr<IInterface> obj;
    arrayOf->Get(NAME, (IInterface**)&obj);
    if (obj != NULL) {
        ICharSequence::Probe(obj)->ToString(result);
    }
    //*result = (*data)[NAME];
    return NOERROR;
}

ECode VoiceMailConstants::GetVoiceMailNumber(
    /* [in] */ const String& carrier,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mCarrierVmMap->Get(CoreUtils::Convert(carrier), (IInterface**)&p);
    AutoPtr<IArrayOf> arrayOf = IArrayOf::Probe(p);
    AutoPtr<IInterface> obj;
    arrayOf->Get(NUMBER, (IInterface**)&obj);
    if (obj != NULL) {
        ICharSequence::Probe(obj)->ToString(result);
    }
    return NOERROR;
}

ECode VoiceMailConstants::GetVoiceMailTag(
    /* [in] */ const String& carrier,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mCarrierVmMap->Get(CoreUtils::Convert(carrier), (IInterface**)&p);
    AutoPtr<IArrayOf> arrayOf = IArrayOf::Probe(p);
    AutoPtr<IInterface> obj;
    arrayOf->Get(TAG, (IInterface**)&obj);
    if (obj != NULL) {
        ICharSequence::Probe(obj)->ToString(result);
    }
    return NOERROR;
}

void VoiceMailConstants::LoadVoiceMail()
{
    AutoPtr<IFileReader> vmReader;

    AutoPtr<IFile> vmFile;
    CFile::New(Environment::GetRootDirectory(),
            PARTNER_VOICEMAIL_PATH, (IFile**)&vmFile);

    // try {
    if (FAILED(CFileReader::New(vmFile, (IFileReader**)&vmReader))) {
        Logger::W(LOGTAG, "Can't open %s", TO_CSTR(vmFile));
        return;
    }
    // } catch (FileNotFoundException e) {
    //     Rlog.w(LOG_TAG, "Can't open " +
    //             Environment.getRootDirectory() + "/" + PARTNER_VOICEMAIL_PATH);
    //     return;
    // }
    //if (vmReader == NULL) {
    //    Logger::E("VoiceMailConstants", "Can't open file: %s", PARTNER_VOICEMAIL_PATH.string());
    //    return;
    //}

    // try {
    ECode ec = NOERROR;
    do {
        AutoPtr<IXmlPullParser> parser;
        Xml::NewPullParser((IXmlPullParser**)&parser);
        parser->SetInput(IReader::Probe(vmReader));

        ec = XmlUtils::BeginDocument(parser, String("voicemail"));
        if (FAILED(ec)) {
            break;
        }

        while (TRUE) {
            ec = XmlUtils::NextElement(parser);
            if (FAILED(ec)) {
                break;
            }

            String name;
            parser->GetName(&name);
            if (!String("voicemail").Equals(name)) {
                break;
            }

            AutoPtr<ArrayOf<String> > data = ArrayOf<String>::Alloc(SIZE);
            String numeric;
            parser->GetAttributeValue(String(NULL), String("numeric"), &numeric);
            parser->GetAttributeValue(String(NULL), String("carrier"), &((*data)[NAME]));
            parser->GetAttributeValue(String(NULL), String("vmnumber"), &((*data)[NUMBER]));
            parser->GetAttributeValue(String(NULL), String("vmtag"), &((*data)[TAG]));

            mCarrierVmMap->Put(CoreUtils::Convert(numeric), CoreUtils::Convert(data));
        }
    } while (0);

    if (FAILED(ec)) {
        Logger::D(LOGTAG, "Exception in Voicemail parser");
    }

    // } catch (XmlPullParserException e) {
    //     Rlog.w(LOG_TAG, "Exception in Voicemail parser " + e);
    // } catch (IOException e) {
    //     Rlog.w(LOG_TAG, "Exception in Voicemail parser " + e);
    // } finally {
        // try {
            if (vmReader != NULL) {
                ICloseable::Probe(vmReader)->Close();
            }
        // } catch (IOException e) {}
    // }
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
