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

#include "elastos/droid/content/CContentProviderHelper.h"
#include "elastos/droid/content/ContentProvider.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CContentProviderHelper, Singleton, IContentProviderHelper)

CAR_SINGLETON_IMPL(CContentProviderHelper)

ECode CContentProviderHelper::CoerceToLocalContentProvider(
    /* [in] */ IIContentProvider* abstractInterface,
    /* [out] */ IContentProvider** localContentProvider)
{
    return ContentProvider::CoerceToLocalContentProvider(abstractInterface, localContentProvider);
}

ECode CContentProviderHelper::GetAuthorityWithoutUserId(
    /* [in] */ const String& auth,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ContentProvider::GetAuthorityWithoutUserId(auth);
    return NOERROR;
}

/** @hide */
ECode CContentProviderHelper::GetUriWithoutUserId(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    return ContentProvider::GetUriWithoutUserId(uri, result);
}

/** @hide */
ECode CContentProviderHelper::UriHasUserId(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ContentProvider::UriHasUserId(uri);
    return NOERROR;
}

/** @hide */
ECode CContentProviderHelper::MaybeAddUserId(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IUri> temp = ContentProvider::MaybeAddUserId(uri, userId);
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CContentProviderHelper::GetUserIdFromAuthority(
    /* [in] */ const String& auth,
    /* [in] */ Int32 defaultUserId,
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId)
    *userId = ContentProvider::GetUserIdFromAuthority(auth, defaultUserId);
    return NOERROR;
}

ECode CContentProviderHelper::GetUserIdFromUri(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 defaultUserId,
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId)
    *userId = ContentProvider::GetUserIdFromUri(uri, defaultUserId);
    return NOERROR;
}

ECode CContentProviderHelper::GetUserIdFromUri(
    /* [in] */ IUri* uri,
    /* [out] */ Int32* userId)
{
    VALIDATE_NOT_NULL(userId)
    *userId = ContentProvider::GetUserIdFromUri(uri);
    return NOERROR;
}

}
}
}

