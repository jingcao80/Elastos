
#include "elastos/droid/server/pm/PackageSignatures.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Server::Pm::CPackageManagerService;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Content::Pm::CSignature;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

PackageSignatures::PackageSignatures(
    /* [in] */ PackageSignatures* orig)
{
    if (orig != NULL && orig->mSignatures != NULL) {
        mSignatures = orig->mSignatures->Clone();
    }
}

PackageSignatures::PackageSignatures(
    /* [in] */ ArrayOf<ISignature*>* sigs)
{
    AssignSignatures(sigs);
}

PackageSignatures::PackageSignatures()
{}

void PackageSignatures::WriteXml(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ const String& tagName,
    /* [in] */ List< AutoPtr<ISignature> >& pastSignatures)
{
    if (mSignatures == NULL) {
        return;
    }
    serializer->WriteStartTag(String(NULL), tagName);
    serializer->WriteAttribute(String(NULL), String("count"),
            StringUtils::ToString(mSignatures->GetLength()));
    for (Int32 i = 0; i < mSignatures->GetLength(); i++) {
        serializer->WriteStartTag(String(NULL), String("cert"));
        AutoPtr<ISignature> sig = (*mSignatures)[i];

        Int32 sigHash;
        IObject::Probe(sig)->GetHashCode(&sigHash);
        Int32 j;
        List< AutoPtr<ISignature> >::Iterator it;
        for (it = pastSignatures.Begin(), j = 0; it != pastSignatures.End(); ++it, ++j) {
            AutoPtr<ISignature> pastSig = *it;
            Int32 hash;
            Boolean isEqual;
            if ((IObject::Probe(pastSig)->GetHashCode(&hash), hash == sigHash) &&
                    (IObject::Probe(pastSig)->Equals(sig, &isEqual), isEqual)) {
                serializer->WriteAttribute(String(NULL), String("index"), StringUtils::ToString(j));
                break;
            }
        }
        if (it == pastSignatures.End()) {
            pastSignatures.PushBack(sig);
            serializer->WriteAttribute(String(NULL), String("index"), StringUtils::ToString((Int32)pastSignatures.GetSize()));
            String str;
            sig->ToCharsString(&str);
            serializer->WriteAttribute(String(NULL), String("key"), str);
        }
        serializer->WriteEndTag(String(NULL), String("cert"));
    }
    serializer->WriteEndTag(String(NULL), tagName);
}

void PackageSignatures::ReadXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ List< AutoPtr<ISignature> >& pastSignatures)
{
    String countStr;
    parser->GetAttributeValue(String(NULL), String("count"), &countStr);
    if (countStr.IsNull()) {
        String des;
        parser->GetPositionDescription(&des);
        CPackageManagerService::ReportSettingsProblem(5/*TODO: Log.WARN*/,
                String("Error in package manager settings: <signatures> has")
                   + " no count at " + des);
        XmlUtils::SkipCurrentTag(parser);
    }
    const Int32 count = StringUtils::ParseInt32(countStr);
    mSignatures = ArrayOf<ISignature*>::Alloc(count);
    Int32 pos = 0;

    Int32 outerDepth, depth, type;
    parser->GetDepth(&outerDepth);
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
           && (type != IXmlPullParser::END_TAG
                   || (parser->GetDepth(&depth), depth) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG
                || type == IXmlPullParser::TEXT) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("cert")) {
            if (pos < count) {
                String index;
                parser->GetAttributeValue(String(NULL), String("index"), &index);
                if (!index.IsNull()) {
                    // try {
                    Int32 idx = StringUtils::ParseInt32(index);
                    String key;
                    parser->GetAttributeValue(String(NULL), String("key"), &key);
                    if (key.IsNull()) {
                        if (idx >= 0 && idx < (Int32)pastSignatures.GetSize()) {
                            AutoPtr<ISignature> sig = pastSignatures[idx];
                            if (sig != NULL) {
                                mSignatures->Set(pos, sig);
                                pos++;
                            }
                            else {
                                String des;
                                parser->GetPositionDescription(&des);
                                CPackageManagerService::ReportSettingsProblem(5/*TODO: Log.WARN*/,
                                        String("Error in package manager settings: <cert> ")
                                           + "index " + index + " is not defined at "
                                           + des);
                            }
                        }
                        else {
                            String des;
                            parser->GetPositionDescription(&des);
                            CPackageManagerService::ReportSettingsProblem(5/*TODO: Log.WARN*/,
                                    String("Error in package manager settings: <cert> ")
                                       + "index " + index + " is out of bounds at "
                                       + des);
                        }
                    }
                    else {
                        while ((Int32)pastSignatures.GetSize() <= idx) {
                            pastSignatures.PushBack(NULL);
                        }
                        AutoPtr<ISignature> sig;
                        ECode ec = CSignature::New(key, (ISignature**)&sig);
                        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
                            String des;
                            parser->GetPositionDescription(&des);
                             CPackageManagerService::ReportSettingsProblem(5/*TODO: Log.WARN*/,
                                    String("Error in package manager settings: <cert> ")
                                       + "index " + index + " has an invalid signature at "
                                       + des);
                        }
                        if (SUCCEEDED(ec)) {
                            pastSignatures[idx] = sig;
                            mSignatures->Set(pos, sig);
                            pos++;
                        }
                    }
                    // } catch (NumberFormatException e) {
                    //     PackageManagerService.reportSettingsProblem(Log.WARN,
                    //             "Error in package manager settings: <cert> "
                    //                + "index " + index + " is not a number at "
                    //                + parser.getPositionDescription());
                    // } catch (IllegalArgumentException e) {
                    //     PackageManagerService.reportSettingsProblem(Log.WARN,
                    //             "Error in package manager settings: <cert> "
                    //                + "index " + index + " has an invalid signature at "
                    //                + parser.getPositionDescription() + ": "
                    //                + e.getMessage());
                    // }
                }
                else {
                    String des;
                    parser->GetPositionDescription(&des);
                    CPackageManagerService::ReportSettingsProblem(5/*TODO: Log.WARN*/,
                            String("Error in package manager settings: <cert> has")
                               + " no index at " + des);
                }
            }
            else {
                String des;
                parser->GetPositionDescription(&des);
                CPackageManagerService::ReportSettingsProblem(5/*TODO: Log.WARN*/,
                        String("Error in package manager settings: too ")
                           + "many <cert> tags, expected " + StringUtils::ToString(count)
                           + " at " + des);
            }
        }
        else {
            String name;
            parser->GetName(&name);
            CPackageManagerService::ReportSettingsProblem(5/*TODO: Log.WARN*/,
                    String("Unknown element under <cert>: ")
                    + name);
        }
        XmlUtils::SkipCurrentTag(parser);
    }

    if (pos < count) {
        // Should never happen -- there is an error in the written
        // settings -- but if it does we don't want to generate
        // a bad array.
        AutoPtr< ArrayOf<ISignature*> > newSigs = ArrayOf<ISignature*>::Alloc(pos);
        for (Int32 i = 0; i < pos; ++i) {
            newSigs->Set(i, (*mSignatures)[i]);
        }
        mSignatures = newSigs;
    }
}

void PackageSignatures::AssignSignatures(
    /* [in] */ ArrayOf<ISignature*>* sigs)
{
    if (sigs == NULL) {
        mSignatures = NULL;
        return;
    }
    mSignatures = ArrayOf<ISignature*>::Alloc(sigs->GetLength());
    for (Int32 i = 0; i < sigs->GetLength(); i++) {
        mSignatures->Set(i, (*sigs)[i]);
    }
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
