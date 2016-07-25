#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/VoiceMailConstants.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/internal/utility/XmlUtils.h"

#include <elastos/core/CoreUtils.h>

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
    assert(0 && "TODO");
    AutoPtr<ArrayOf<String> > data; // = p;
    *result = (*data)[NAME];
    return NOERROR;
}

ECode VoiceMailConstants::GetVoiceMailNumber(
    /* [in] */ const String& carrier,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mCarrierVmMap->Get(CoreUtils::Convert(carrier), (IInterface**)&p);
    assert(0 && "TODO");
    AutoPtr<ArrayOf<String> > data; // = p;
    *result = (*data)[NUMBER];
    return NOERROR;
}

ECode VoiceMailConstants::GetVoiceMailTag(
    /* [in] */ const String& carrier,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> p;
    mCarrierVmMap->Get(CoreUtils::Convert(carrier), (IInterface**)&p);
    assert(0 && "TODO");
    AutoPtr<ArrayOf<String> > data; // = p;
    *result = (*data)[TAG];
    return NOERROR;
}

void VoiceMailConstants::LoadVoiceMail()
{
    AutoPtr<IFileReader> vmReader;

    AutoPtr<IFile> vmFile;
    CFile::New(Environment::GetRootDirectory(),
            PARTNER_VOICEMAIL_PATH, (IFile**)&vmFile);

    // try {
        CFileReader::New(vmFile, (IFileReader**)&vmReader);
    // } catch (FileNotFoundException e) {
    //     Rlog.w(LOG_TAG, "Can't open " +
    //             Environment.getRootDirectory() + "/" + PARTNER_VOICEMAIL_PATH);
    //     return;
    // }

    // try {
        AutoPtr<IXmlPullParser> parser;
        Xml::NewPullParser((IXmlPullParser**)&parser);
        parser->SetInput(IReader::Probe(vmReader));

        XmlUtils::BeginDocument(parser, String("voicemail"));

        while (TRUE) {
            XmlUtils::NextElement(parser);

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

            assert(0 && "TODO");
            // mCarrierVmMap->Put(CoreUtils::Convert(numeric), data);
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
