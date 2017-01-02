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
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/SpnOverride.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/os/CEnvironment.h"

#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;

using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::IO::IReader;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashMap;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                             SpnOverride
//=====================================================================
CAR_INTERFACE_IMPL(SpnOverride, Object, ISpnOverride);

const String SpnOverride::LOGTAG("SpnOverride");
const String SpnOverride::PARTNER_SPN_OVERRIDE_PATH("etc/spn-conf.xml");

SpnOverride::SpnOverride()
{
    CHashMap::New((IHashMap**)&mCarrierSpnMap);
    LoadSpnOverrides();
}

ECode SpnOverride::ContainsCarrier(
    /* [in] */ const String& carrier,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mCarrierSpnMap->ContainsKey(CoreUtils::Convert(carrier), result);
}

ECode SpnOverride::GetSpn(
    /* [in] */ const String& carrier,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mCarrierSpnMap->Get(CoreUtils::Convert(carrier), (IInterface**)&p);
    return ICharSequence::Probe(p)->ToString(result);
}

void SpnOverride::LoadSpnOverrides()
{
    AutoPtr<IFileReader> spnReader;

    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> f;
    env->GetRootDirectory((IFile**)&f);
    AutoPtr<IFile> spnFile;
    CFile::New(f,
            PARTNER_SPN_OVERRIDE_PATH,
            (IFile**)&spnFile);

    // try {
        CFileReader::New(spnFile, (IFileReader**)&spnReader);
    // } catch (FileNotFoundException e) {
    //     Rlog.w(LOGTAG, "Can not open " +
    //             Environment.getRootDirectory() + "/" + PARTNER_SPN_OVERRIDE_PATH);
    //     return;
    // }

    // try {
        AutoPtr<IXmlPullParser> parser;
        Xml::NewPullParser((IXmlPullParser**)&parser);
        parser->SetInput(IReader::Probe(spnReader));

        XmlUtils::BeginDocument(parser, String("spnOverrides"));

        while (TRUE) {
            XmlUtils::NextElement(parser);

            String name;
            parser->GetName(&name);
            if (!name.Equals("spnOverride")) {
                break;
            }

            String numeric;
            parser->GetAttributeValue(String(NULL), String("numeric"), &numeric);
            String data;
            parser->GetAttributeValue(String(NULL), String("spn"), &data);

            mCarrierSpnMap->Put(CoreUtils::Convert(numeric), CoreUtils::Convert(data));
        }
        ICloseable::Probe(spnReader)->Close();
    // } catch (XmlPullParserException e) {
    //     Rlog.w(LOGTAG, "Exception in spn-conf parser " + e);
    // } catch (IOException e) {
    //     Rlog.w(LOGTAG, "Exception in spn-conf parser " + e);
    // }
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
