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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/contacts/common/util/LocalizedNameResolver.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Accounts::IAccountManagerHelper;
using Elastos::Droid::Accounts::CAccountManagerHelper;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAuthenticatorDescription;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

const String LocalizedNameResolver::TAG("LocalizedNameResolver");
const String LocalizedNameResolver::METADATA_CONTACTS("android.provider.CONTACTS_STRUCTURE");
const String LocalizedNameResolver::CONTACTS_DATA_KIND("ContactsDataKind");

ECode LocalizedNameResolver::GetAllContactsName(
    /* [in] */ IContext* context,
    /* [in] */ const String& accountType,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String(NULL);
    if (context == NULL) {
        Logger::E("LocalizedNameResolver", "Context must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (accountType.IsNull()) return NOERROR;

    *name = ResolveAllContactsName(context, accountType);
    return NOERROR;
}

String LocalizedNameResolver::ResolveAllContactsName(
    /* [in] */ IContext* context,
    /* [in] */ const String& accountType)
{
    AutoPtr<IAccountManagerHelper> helper;
    CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper);
    AutoPtr<IAccountManager> am;
    helper->Get(context, (IAccountManager**)&am);

    AutoPtr<ArrayOf<IAuthenticatorDescription*> > types;
    am->GetAuthenticatorTypes((ArrayOf<IAuthenticatorDescription*>**)&types);
    for (Int32 i = 0; i < types->GetLength(); ++i) {
        AutoPtr<IAuthenticatorDescription> auth = (*types)[i];
        String type;
        auth->GetType(&type);
        if (accountType.Equals(type)) {
            String pkgName;
            auth->GetPackageName(&pkgName);
            return ResolveAllContactsNameFromMetaData(context, pkgName);
        }
    }

    return String(NULL);
}

String LocalizedNameResolver::ResolveAllContactsNameFromMetaData(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName)
{
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    // try {
    AutoPtr<IPackageInfo> pi;
    pm->GetPackageInfo(packageName, IPackageManager::GET_SERVICES
            | IPackageManager::GET_META_DATA, (IPackageInfo**)&pi);
    AutoPtr<ArrayOf<IServiceInfo*> > services;
    if (pi != NULL && (pi->GetServices((ArrayOf<IServiceInfo*>**)&services), services != NULL)) {
        for (Int32 i = 0; i < services->GetLength(); ++i) {
            AutoPtr<IServiceInfo> si = (*services)[i];
            AutoPtr<IXmlResourceParser> parser;
            IPackageItemInfo::Probe(si)->LoadXmlMetaData(pm, METADATA_CONTACTS, (IXmlResourceParser**)&parser);
            if (parser != NULL) {
                String str;
                ECode ec = LoadAllContactsNameFromXml(context, IXmlPullParser::Probe(parser), packageName, &str);
                if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                    Logger::W(TAG, "Problem loading \"All Contacts\"-name: 0x%08x", ec);
                    break;
                }
                return str;
            }
        }
    }
    // } catch (NameNotFoundException e) {
    //     Log.w(TAG, "Problem loading \"All Contacts\"-name: " + e.toString());
    // }
    return String(NULL);
}

ECode LocalizedNameResolver::LoadAllContactsNameFromXml(
    /* [in] */ IContext* context,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& packageName,
    /* [out] */ String* _name)
{
    VALIDATE_NOT_NULL(_name)
    *_name = String(NULL);
    // try {
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);
    Int32 type;
    while ((parser->Next(&type), type != IXmlPullParser::START_TAG)
            && type != IXmlPullParser::END_DOCUMENT) {
        // Drain comments and whitespace
    }

    if (type != IXmlPullParser::START_TAG) {
        Logger::E(TAG, "No start tag found");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 depth;
    parser->GetDepth(&depth);
    Int32 depthTemp;
    while (((parser->Next(&type), type != IXmlPullParser::END_TAG)
            || (parser->GetDepth(&depthTemp), depthTemp > depth))
            && type != IXmlPullParser::END_DOCUMENT) {
        String name;
        parser->GetName(&name);
        if (type == IXmlPullParser::START_TAG && CONTACTS_DATA_KIND.Equals(name)) {
            AutoPtr<ITypedArray> typedArray;
            AutoPtr<ArrayOf<Int32> > params = TO_ATTRS_ARRAYOF(Elastos::Droid::Dialer::R::styleable::ContactsDataKind);
            context->ObtainStyledAttributes(attrs, params, (ITypedArray**)&typedArray);
            // try {
            // See if a string has been hardcoded directly into the xml
            String nonResourceString;
            typedArray->GetNonResourceString(
                    Elastos::Droid::Dialer::R::styleable::ContactsDataKind_android_allContactsName, &nonResourceString);
            if (!nonResourceString.IsNull()) {
                *_name = nonResourceString;
                typedArray->Recycle();
                return NOERROR;
            }

            // See if a resource is referenced. We can't rely on getString
            // to automatically resolve it as the resource lives in a different package
            Int32 id;
            typedArray->GetResourceId(
                    Elastos::Droid::Dialer::R::styleable::ContactsDataKind_android_allContactsName, 0, &id);
            if (id == 0) {
                typedArray->Recycle();
                return NOERROR;
            }

            // Resolve the resource Id
            AutoPtr<IPackageManager> packageManager;
            context->GetPackageManager((IPackageManager**)&packageManager);
            AutoPtr<IResources> resources;
            // try {
            ECode ec = packageManager->GetResourcesForApplication(packageName, (IResources**)&resources);
            if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                typedArray->Recycle();
                return NOERROR;
            }
            // } catch (NameNotFoundException e) {
            //     return null;
            // }
            // try {
            resources->GetString(id, _name);
            typedArray->Recycle();
            return NOERROR;
            // } catch (NotFoundException e) {
            //     return null;
            // }
            // } finally {
            //     typedArray.recycle();
            // }
        }
    }
    return NOERROR;
    // } catch (XmlPullParserException e) {
    //     throw new IllegalStateException("Problem reading XML", e);
    // } catch (IOException e) {
    //     throw new IllegalStateException("Problem reading XML", e);
    // }
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
