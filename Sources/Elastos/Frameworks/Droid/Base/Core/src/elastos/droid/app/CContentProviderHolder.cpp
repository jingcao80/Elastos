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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/CContentProviderHolder.h"
#include "elastos/droid/content/ContentProviderNative.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentProviderProxy;
using Elastos::Droid::Content::ContentProviderNative;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CContentProviderHolder, Object, IContentProviderHolder, IParcelable)

CAR_OBJECT_IMPL(CContentProviderHolder)

CContentProviderHolder::CContentProviderHolder()
    : mNoReleaseNeeded(FALSE)
{
}

ECode CContentProviderHolder::constructor()
{
    return NOERROR;
}

ECode CContentProviderHolder::constructor(
    /* [in] */ IProviderInfo* info)
{
    mInfo = info;
    return NOERROR;
}

ECode CContentProviderHolder::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    AutoPtr<IBinder> binder;
    IContentProviderProxy* proxy = IContentProviderProxy::Probe(mProvider);
    if (proxy) {
        proxy->AsBinder((IBinder**)&binder);
    }
    else {
        binder = IBinder::Probe(mProvider);
    }

    dest->WriteInterfacePtr(mInfo.Get());
    dest->WriteInterfacePtr(binder.Get());
    dest->WriteInterfacePtr(mConnection.Get());
    dest->WriteBoolean(mNoReleaseNeeded);

    return NOERROR;
}

ECode CContentProviderHolder::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IBinder> binder;
    source->ReadInterfacePtr((HANDLE*)&mInfo);
    source->ReadInterfacePtr((HANDLE*)&binder);
    source->ReadInterfacePtr((HANDLE*)&mConnection);
    source->ReadBoolean(&mNoReleaseNeeded);

    mProvider = ContentProviderNative::AsInterface(binder);

    return NOERROR;
}

ECode CContentProviderHolder::GetProviderInfo(
    /* [out] */ IProviderInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CContentProviderHolder::SetProviderInfo(
    /* [in] */ IProviderInfo* info)
{
    mInfo = info;
    return NOERROR;
}

ECode CContentProviderHolder::GetContentProvider(
    /* [out] */ IIContentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CContentProviderHolder::SetContentProvider(
    /* [in] */ IIContentProvider* provider)
{
    mProvider = provider;
    return NOERROR;
}

ECode CContentProviderHolder::GetConnection(
    /* [out] */ IBinder** connection)
{
    VALIDATE_NOT_NULL(connection);
    *connection = mConnection;
    REFCOUNT_ADD(*connection);
    return NOERROR;
}

ECode CContentProviderHolder::SetConnection(
    /* [in] */ IBinder* connection)
{
    mConnection = connection;
    return NOERROR;
}

ECode CContentProviderHolder::GetNoReleaseNeeded(
    /* [out] */ Boolean* noReleaseNeeded)
{
    VALIDATE_NOT_NULL(noReleaseNeeded);
    *noReleaseNeeded = mNoReleaseNeeded;
    return NOERROR;
}

ECode CContentProviderHolder::SetNoReleaseNeeded(
    /* [in] */ Boolean noReleaseNeeded)
{
    mNoReleaseNeeded = noReleaseNeeded;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos

