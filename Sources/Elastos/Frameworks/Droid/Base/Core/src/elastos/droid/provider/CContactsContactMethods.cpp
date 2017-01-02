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

#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContactMethods.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::R;
using Elastos::Core::CDouble;
using Elastos::Core::CInteger64;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::IDouble;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_OBJECT_IMPL(CContactsContactMethods)

CAR_INTERFACE_IMPL_4(CContactsContactMethods, Object, IContactsContactMethods, IBaseColumns, IContactsContactMethodsColumns, IContactsPeopleColumns)

ECode CContactsContactMethods::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/contact_methods"), uri);
}

ECode CContactsContactMethods::GetRAW_CONTENT_EMAIL_URL(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/contact_methods/email"), uri);
}

ECode CContactsContactMethods::EncodePredefinedImProtocol(
    /* [in] */ Int32 protocol,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    *value = String("pre:") + StringUtils::ToString(protocol);
    return NOERROR;
}

ECode CContactsContactMethods::EncodeCustomImProtocol(
    /* [in] */ const String& protocolString,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    *value = String("custom:") + protocolString;
    return NOERROR;
}

ECode CContactsContactMethods::DecodeImProtocol(
    /* [in] */ const String& encodedString,
    /* [out] */ IInterface** value)
{
    if (encodedString.IsNull()) {
        *value = NULL;
        return NOERROR;
    }

    if (encodedString.StartWith("pre:")) {
        Int32 num = StringUtils::ParseInt32(encodedString.Substring(4));
        AutoPtr<IInteger32> result;
        FAIL_RETURN(CInteger32::New(num, (IInteger32**)&result))
        *value = (IInterface*)result;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }

    if (encodedString.StartWith("custom:")) {
        String str = encodedString.Substring(7);
        AutoPtr<ICharSequence> result;
        FAIL_RETURN(CString::New(str, (ICharSequence**)&result))
        *value = (IInterface*)result;
        REFCOUNT_ADD(*value);
        return NOERROR;
    }

    // throw new IllegalArgumentException(
    //         "the value is not a valid encoded protocol, " + encodedString);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CContactsContactMethods::LookupProviderNameFromId(
    /* [in] */ Int32 protocol,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    switch (protocol) {
        case IContactsContactMethods::PROTOCOL_GOOGLE_TALK:
            *name = IContactsContactMethodsProviderNames::GTALK;
            return NOERROR;
        case IContactsContactMethods::PROTOCOL_AIM:
            *name = IContactsContactMethodsProviderNames::AIM;
            return NOERROR;
        case IContactsContactMethods::PROTOCOL_MSN:
            *name = IContactsContactMethodsProviderNames::MSN;
            return NOERROR;
        case IContactsContactMethods::PROTOCOL_YAHOO:
            *name = IContactsContactMethodsProviderNames::YAHOO;
            return NOERROR;
        case IContactsContactMethods::PROTOCOL_ICQ:
            *name = IContactsContactMethodsProviderNames::ICQ;
            return NOERROR;
        case IContactsContactMethods::PROTOCOL_JABBER:
            *name = IContactsContactMethodsProviderNames::JABBER;
            return NOERROR;
        case IContactsContactMethods::PROTOCOL_SKYPE:
            *name = IContactsContactMethodsProviderNames::SKYPE;
            return NOERROR;
        case IContactsContactMethods::PROTOCOL_QQ:
            *name = IContactsContactMethodsProviderNames::QQ;
            return NOERROR;
    }
    *name = NULL;
    return NOERROR;
}

/**
 * no public constructor since this is a utility class
 */
ECode CContactsContactMethods::constructor()
{
    return NOERROR;
}

ECode CContactsContactMethods::GetDisplayLabel(
    /* [in] */ IContext* context,
    /* [in] */ Int32 kind,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* label,
    /* [out] */ ICharSequence** lb)
{
    VALIDATE_NOT_NULL(lb);

    AutoPtr<ICharSequence> display;
    FAIL_RETURN(CString::New(String(""), (ICharSequence**)&display))
    switch (kind) {
        case IContacts::KIND_EMAIL: {
            if (type != IContactsContactMethodsColumns::TYPE_CUSTOM) {
                AutoPtr<ArrayOf<ICharSequence*> > labels;
                AutoPtr<IResources> res;
                FAIL_RETURN(context->GetResources((IResources**)&res))
                FAIL_RETURN(res->GetTextArray(Elastos::Droid::R::array::emailAddressTypes, (ArrayOf<ICharSequence*>**)&labels))

                Int32 size = labels->GetLength();
                if (type - 1 >= 0 && type - 1 < size) {
                    display = (*labels)[type - 1];
                }
                else {
                    display = (*labels)[IContactsContactMethodsColumns::TYPE_HOME - 1];
                }
            }
            else {
                Int32 length;
                FAIL_RETURN(label->GetLength(&length))
                if (length != 0) {
                    display = label;
                }
            }
            break;
        }

        case IContacts::KIND_POSTAL: {
            if (type != IContactsContactMethodsColumns::TYPE_CUSTOM) {
                AutoPtr<ArrayOf<ICharSequence*> > labels;
                AutoPtr<IResources> res;
                FAIL_RETURN(context->GetResources((IResources**)&res))
                FAIL_RETURN(res->GetTextArray(Elastos::Droid::R::array::postalAddressTypes, (ArrayOf<ICharSequence*>**)&labels))

                Int32 size = labels->GetLength();
                if (type - 1 >= 0 && type - 1 < size) {
                    display = (*labels)[type - 1];
                }
                else {
                    display = (*labels)[IContactsContactMethodsColumns::TYPE_HOME - 1];
                }
            }
            else {
                Int32 length;
                FAIL_RETURN(label->GetLength(&length))
                if (length != 0) {
                    display = label;
                }
            }
            break;
        }

        default:
            String str;
            AutoPtr<ICharSequence> cstr;
            FAIL_RETURN(context->GetString(Elastos::Droid::R::string::untitled, &str))
            FAIL_RETURN(CString::New(str, (ICharSequence**)&cstr))
            display = cstr;
    }
    *lb = display;
    REFCOUNT_ADD(*lb);
    return NOERROR;
}

ECode CContactsContactMethods::AddPostalLocation(
    /* [in] */ IContext* context,
    /* [in] */ Int64 postalId,
    /* [in] */ Double latitude,
    /* [in] */ Double longitude)
{
    AutoPtr<IContentResolver> resolver;
    FAIL_RETURN(context->GetContentResolver((IContentResolver**)&resolver))
    // Insert the location
    AutoPtr<IContentValues> values;
    FAIL_RETURN(CContentValues::New(2, (IContentValues**)&values))

    values->Put(POSTAL_LOCATION_LATITUDE, latitude);
    values->Put(POSTAL_LOCATION_LONGITUDE, longitude);

    AutoPtr<IUri> loc, curi;
    GetCONTENT_URI((IUri**)&curi);
    resolver->Insert(curi, values, (IUri**)&loc);
    Int64 locId;
    AutoPtr<IContentUris> helper;
    CContentUris::AcquireSingleton((IContentUris**)&helper);
    helper->ParseId(loc, &locId);

    // Update the postal address
    values->Clear();
    values->Put(AUX_DATA, locId);

    AutoPtr<IUri> newUri;
    FAIL_RETURN(helper->WithAppendedId(curi, postalId, (IUri**)&newUri))
    Int32 result;
    return resolver->Update(newUri, values, String(NULL), NULL, &result);
}

} // Provider
} // Droid
} // Elastos