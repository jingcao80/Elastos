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

#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTACTMETHODSHELPER_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTACTMETHODSHELPER_H__

#include "_Elastos_Droid_Provider_CContactsContactMethodsHelper.h"
#include "elastos/droid/provider/CContactsContactMethods.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContactMethodsHelper)
    , public Singleton
    , public IContactsContactMethodsHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URL for this table
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for sub-directory of e-mail addresses.
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetRAW_CONTENT_EMAIL_URL(
        /* [out] */ IUri** uri);

    /**
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI EncodePredefinedImProtocol(
        /* [in] */ Int32 protocol,
        /* [out] */ String* value);

    /**
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI EncodeCustomImProtocol(
        /* [in] */ const String& protocolString,
        /* [out] */ String* value);

    /**
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI DecodeImProtocol(
        /* [in] */ const String& encodedString,
        /* [out] */ IInterface** value);

    /**
     * This looks up the provider name defined in
     * from the predefined IM protocol id.
     * This is used for interacting with the IM application.
     *
     * @param protocol the protocol ID
     * @return the provider name the IM app uses for the given protocol, or null if no
     * provider is defined for the given protocol
     * @deprecated see {@link android.provider.ContactsContract}
     * @hide
     */
    // @Deprecated
    CARAPI LookupProviderNameFromId(
        /* [in] */ Int32 protocol,
        /* [out] */ String* name);

    /**
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI GetDisplayLabel(
        /* [in] */ IContext* context,
        /* [in] */ Int32 kind,
        /* [in] */ Int32 type,
        /* [in] */ ICharSequence* label,
        /* [out] */ ICharSequence** lb);
};

} // Provider
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTACTMETHODSHELPER_H__
