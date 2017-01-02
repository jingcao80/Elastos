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

#include "elastos/droid/server/telecom/ServiceBinder.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/ThreadUtil.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <elastos/droid/app/Service.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Utility::CCollections;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// ServiceBinder::Binder
//=============================================================================
ServiceBinder::Binder::Binder(
    /* [in] */ ServiceBinder* host)
    : mHost(host)
{}

ECode ServiceBinder::Binder::Bind(
    /* [in] */ IServiceBinderBindCallback* callback)
{
    ThreadUtil::CheckOnMainThread();
    Log::D("ServiceBinder", "bind()");
    // Reset any abort request if we're asked to bind again.
    mHost->ClearAbort();
    Boolean isEmpty;
    mHost->mCallbacks->IsEmpty(&isEmpty);
    if (!isEmpty) {
        // Binding already in progress, append to the list of callbacks and bail out.
        mHost->mCallbacks->Add(callback);
        return NOERROR;
    }
    mHost->mCallbacks->Add(callback);
    if (mHost->mServiceConnection == NULL) {
        AutoPtr<IIntent> serviceIntent;
        CIntent::New(mHost->mServiceAction, (IIntent**)&serviceIntent);
        serviceIntent->SetComponent(mHost->mComponentName);
        AutoPtr<IServiceConnection> connection = new ServiceBinderConnection(mHost);
        Log::D("ServiceBinder", "Binding to service with intent: %s", TO_CSTR(serviceIntent));
        Boolean isBindServiceOk;
        mHost->mContext->BindService(serviceIntent, connection, IContext::BIND_AUTO_CREATE, &isBindServiceOk);
        if (!isBindServiceOk) {
            mHost->HandleFailedConnection();
            return NOERROR;
        }
    } else {
        Log::D("ServiceBinder", "Service is already bound.");
        AutoPtr<IPreconditions> preconditionsHelper;
        CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
        preconditionsHelper->CheckNotNull(mHost->mBinder);
        mHost->HandleSuccessfulConnection();
    }
    return NOERROR;
}

//=============================================================================
// ServiceBinder::ServiceBinderConnection
//=============================================================================
CAR_INTERFACE_IMPL(ServiceBinder::ServiceBinderConnection, Object, IServiceConnection)

ServiceBinder::ServiceBinderConnection::ServiceBinderConnection(
    /* [in] */ ServiceBinder* host)
    : mHost(host)
{}

ECode ServiceBinder::ServiceBinderConnection::OnServiceConnected(
    /* [in] */ IComponentName* componentName,
    /* [in] */ IBinder* binder)
{
    ThreadUtil::CheckOnMainThread();
    Log::I("ServiceBinder", "ServiceBinderConnection::OnServiceConnected Service bound %s", TO_CSTR(componentName));
    // Unbind request was queued so unbind immediately.
    if (mHost->mIsBindingAborted) {
        mHost->ClearAbort();
        mHost->LogServiceDisconnected(String("onServiceConnected"));
        mHost->mContext->UnbindService(this);
        mHost->HandleFailedConnection();
        return NOERROR;
    }
    mHost->mServiceConnection = this;
    mHost->SetBinder(binder);
    mHost->HandleSuccessfulConnection();
    return NOERROR;
}

ECode ServiceBinder::ServiceBinderConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* componentName)
{
    mHost->LogServiceDisconnected(String("onServiceDisconnected"));
    mHost->mServiceConnection = NULL;
    mHost->ClearAbort();
    mHost->HandleServiceDisconnected();
    return NOERROR;
}

//=============================================================================
// ServiceBinder
//=============================================================================
CAR_INTERFACE_IMPL(ServiceBinder, Object, IServiceBinder)

ServiceBinder::ServiceBinder()
    : mAssociatedCallCount(0)
    , mIsBindingAborted(FALSE)
{
    CHashSet::New((ISet**)&mCallbacks);
    AutoPtr<IMap> map;
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&map);
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->SetFromMap(map, (ISet**)&mListeners);
}

ECode ServiceBinder::constructor(
    /* [in] */ const String& serviceAction,
    /* [in] */ IComponentName* componentName,
    /* [in] */ IContext* context)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckState(!serviceAction.IsNullOrEmpty());
    preconditionsHelper->CheckNotNull(componentName);
    mContext = context;
    mServiceAction = serviceAction;
    mComponentName = componentName;
    return NOERROR;
}

ECode ServiceBinder::IncrementAssociatedCallCount()
{
    mAssociatedCallCount++;
    String s;
    mComponentName->FlattenToShortString(&s);
    Log::V("ServiceBinder", "Call count increment %d, %s", mAssociatedCallCount,
            s.string());
    return NOERROR;
}

ECode ServiceBinder::DecrementAssociatedCallCount()
{
    if (mAssociatedCallCount > 0) {
        mAssociatedCallCount--;
        String s;
        mComponentName->FlattenToShortString(&s);
        Log::V("ServiceBinder", "Call count decrement %d, %s", mAssociatedCallCount,
                s.string());
        if (mAssociatedCallCount == 0) {
            Unbind();
        }
    } else {
        String className;
        mComponentName->GetClassName(&className);
        Log::Wtf("ServiceBinder", "%s: ignoring a request to decrement mAssociatedCallCount below zero", className.string());
    }
    return NOERROR;
}

ECode ServiceBinder::GetAssociatedCallCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAssociatedCallCount;
    return NOERROR;
}

ECode ServiceBinder::Unbind()
{
    ThreadUtil::CheckOnMainThread();
    if (mServiceConnection == NULL) {
        // We're not yet bound, so queue up an abort request.
        mIsBindingAborted = TRUE;
    } else {
        LogServiceDisconnected(String("unbind"));
        mContext->UnbindService(mServiceConnection);
        mServiceConnection = NULL;
        SetBinder(NULL);
    }
    return NOERROR;
}

ECode ServiceBinder::GetComponentName(
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mComponentName;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ServiceBinder::IsServiceValid(
    /* [in] */ const String& actionName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mBinder == NULL) {
        Log::W("ServiceBinder", "%s invoked while service is unbound", actionName.string());
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode ServiceBinder::AddListener(
    /* [in] */ IServiceBinderListener* listener)
{
    return mListeners->Add(listener);
}

ECode ServiceBinder::RemoveListener(
    /* [in] */ IServiceBinderListener* listener)
{
    if (listener != NULL) {
        mListeners->Remove(listener);
    }
    return NOERROR;
}

ECode ServiceBinder::LogServiceDisconnected(
    /* [in] */ const String& sourceTag)
{
    Log::I("ServiceBinder", "Service unbound %s, from %s.", TO_CSTR(mComponentName), sourceTag.string());
    return NOERROR;
}

ECode ServiceBinder::HandleSuccessfulConnection()
{
    Log::I("ServiceBinder", "HandleSuccessfulConnection");
    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IServiceBinderBindCallback> callback = IServiceBinderBindCallback::Probe(obj);
        callback->OnSuccess();
    }
    mCallbacks->Clear();
    return NOERROR;
}

ECode ServiceBinder::HandleFailedConnection()
{
    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IServiceBinderBindCallback> callback = IServiceBinderBindCallback::Probe(obj);
        callback->OnFailure();
    }
    mCallbacks->Clear();
    return NOERROR;
}

ECode ServiceBinder::HandleServiceDisconnected()
{
    return SetBinder(NULL);
}

ECode ServiceBinder::ClearAbort()
{
    mIsBindingAborted = FALSE;
    return NOERROR;
}

ECode ServiceBinder::SetBinder(
    /* [in] */ IBinder* binder)
{
    if (mBinder.Get() != binder) {
        mBinder = binder;
        SetServiceInterface(binder);
        if (binder == NULL) {
            AutoPtr<IIterator> it;
            mListeners->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IServiceBinderListener> l = IServiceBinderListener::Probe(obj);
                l->OnUnbind(this);
            }
        }
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
