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

#ifndef __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERHELPER_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERHELPER_H__

#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_Content_CContentProviderHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentProviderHelper)
    , public Singleton
    , public IContentProviderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Given an IContentProvider, try to coerce it back to the real
     * ContentProvider object if it is running in the local process.  This can
     * be used if you know you are running in the same process as a provider,
     * and want to get direct access to its implementation details.  Most
     * clients should not nor have a reason to use it.
     *
     * @param abstractInterface The ContentProvider interface that is to be
     *              coerced.
     * @return If the IContentProvider is non-{@code null} and local, returns its actual
     * ContentProvider instance.  Otherwise returns {@code null}.
     * @hide
     */
    CARAPI CoerceToLocalContentProvider(
        /* [in] */ IIContentProvider* abstractInterface,
        /* [out] */ IContentProvider** localContentProvider);

    /**
     * Removes userId part from authority string. Expects format:
     * userId@some.authority
     * If there is no userId in the authority, it symply returns the argument
     * @hide
     */
    CARAPI GetAuthorityWithoutUserId(
        /* [in] */ const String& auth,
        /* [out] */ String* result);

    /** @hide */
    CARAPI GetUriWithoutUserId(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    /** @hide */
    CARAPI UriHasUserId(
        /* [in] */ IUri* uri,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI MaybeAddUserId(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId,
        /* [out] */ IUri** result);

    CARAPI GetUserIdFromAuthority(
        /* [in] */ const String& auth,
        /* [in] */ Int32 defaultUserId,
        /* [out] */ Int32* userId);

    CARAPI GetUserIdFromUri(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 defaultUserId,
        /* [out] */ Int32* userId);

    CARAPI GetUserIdFromUri(
        /* [in] */ IUri* uri,
        /* [out] */ Int32* userId);

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERHELPER_H__
