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

#include "Elastos.Droid.Net.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/database/CContentObserverTransport.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Droid::Os::UserHandle;

namespace Elastos {
namespace Droid {
namespace Database {

//==========================================================
// ContentObserver::NotificationRunnable
//==========================================================
ContentObserver::NotificationRunnable::NotificationRunnable(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId,
    /* [in] */ ContentObserver* owner)
    : mSelfChange(selfChange)
    , mUri(uri)
    , mUserId(userId)
    , mOwner(owner)
{
}

ECode ContentObserver::NotificationRunnable::Run()
{
    return mOwner->OnChange(mSelfChange, mUri, mUserId);
}

//==========================================================
// ContentObserver
//==========================================================
CAR_INTERFACE_IMPL(ContentObserver, Object, IContentObserver)

ContentObserver::ContentObserver()
{}

ContentObserver::~ContentObserver()
{}

ECode ContentObserver::constructor()
{
    return NOERROR;
}

ECode ContentObserver::constructor(
    /* [in]  */ IHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode ContentObserver::GetContentObserver(
    /* [out] */ IIContentObserver** contentObserver)
{
    VALIDATE_NOT_NULL(contentObserver)

    {
        AutoLock syncLock(mLock);
        if (mTransport == NULL) {
            CContentObserverTransport::New(this, (IContentObserverTransport**)&mTransport);
        }
        *contentObserver = IIContentObserver::Probe(mTransport);
        REFCOUNT_ADD(*contentObserver)
    }
    return NOERROR;
}

ECode ContentObserver::ReleaseContentObserver(
    /* [out] */ IIContentObserver** contentObserver)
{
    VALIDATE_NOT_NULL(contentObserver)

    {
        AutoLock syncLock(mLock);
        AutoPtr<IContentObserverTransport> oldTransport = mTransport;
        if (oldTransport != NULL) {
            oldTransport->ReleaseContentObserver();
            mTransport = NULL;
        }
        *contentObserver = IIContentObserver::Probe(oldTransport);
        REFCOUNT_ADD(*contentObserver)
    }
    return NOERROR;
}

ECode ContentObserver::DeliverSelfNotifications(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode ContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    return NOERROR;
}

ECode ContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    return OnChange(selfChange);
}

ECode ContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId)
{
    return OnChange(selfChange, uri);
}

ECode ContentObserver::DispatchChange(
    /* [in] */ Boolean selfChange)
{
    return DispatchChange(selfChange, NULL);
}

ECode ContentObserver::DispatchChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    return DispatchChange(selfChange, uri, UserHandle::GetCallingUserId());
}

ECode ContentObserver::DispatchChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId)
{
    if (mHandler == NULL) {
        OnChange(selfChange, uri, userId);
    }
    else {
        Boolean res;
        AutoPtr<IRunnable> run = new NotificationRunnable(selfChange, uri, userId, this);
        mHandler->Post(run, &res);
    }
    return NOERROR;
}


} //Database
} //Droid
} //Elastos
