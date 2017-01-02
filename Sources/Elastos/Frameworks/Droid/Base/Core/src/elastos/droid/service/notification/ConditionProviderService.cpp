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
#include "elastos/droid/service/notification/ConditionProviderService.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

//====================================
// ConditionProviderService
//====================================
const String ConditionProviderService::TAG("ConditionProviderService");

CAR_INTERFACE_IMPL(ConditionProviderService, Service, IConditionProviderService)

ConditionProviderService::ConditionProviderService()
{
}

ECode ConditionProviderService::constructor()
{
    AutoPtr<H> h = new H(this);
    h->constructor();
    mHandler = h.Get();
    return NOERROR;
}

ECode ConditionProviderService::GetNotificationInterface(
    /* [out] */ IINotificationManager** ni)
{
    VALIDATE_NOT_NULL(ni)
    if (mNoMan == NULL) {
        AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::NOTIFICATION_SERVICE);
        mNoMan = IINotificationManager::Probe(obj);
    }
    *ni = mNoMan;
    REFCOUNT_ADD(*ni)
    return NOERROR;
}

ECode ConditionProviderService::NotifyCondition(
    /* [in] */ ICondition* condition)
{
    if (condition == NULL) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<ArrayOf<ICondition*> > array = ArrayOf<ICondition*>::Alloc(1);
    (*array)[0] = condition;
    return NotifyConditions(array.Get());
}

ECode ConditionProviderService::NotifyConditions(
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    if (!IsBound() || conditions == NULL) return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IINotificationManager> nm;
    GetNotificationInterface((IINotificationManager**)&nm);
    String pm;
    GetPackageName(&pm);
    ECode ec = nm->NotifyConditions(pm, mProvider, conditions);
    if (FAILED(ec)) {
        Logger::V(TAG, "Unable to contact notification manager%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode ConditionProviderService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** b)
{
    VALIDATE_NOT_NULL(b)
    if (mProvider == NULL) {
        mProvider = new Provider(this);
    }
    *b = (IBinder*)mProvider;
    REFCOUNT_ADD(*b)
    return NOERROR;
}

Boolean ConditionProviderService::IsBound()
{
    if (mProvider == NULL) {
        Logger::W(TAG, "Condition provider service not yet bound.");
        return FALSE;
    }
    return TRUE;
}

//====================================
// ConditionProviderService::Provider
//====================================

CAR_INTERFACE_IMPL_2(ConditionProviderService::Provider, Object, IIConditionProvider, IBinder)

ConditionProviderService::Provider::Provider()
{
}

ConditionProviderService::Provider::Provider(
    /* [in] */ ConditionProviderService* host)
    : mHost(host)
{
}

ECode ConditionProviderService::Provider::constructor()
{
    return NOERROR;
}

ECode ConditionProviderService::Provider::OnConnected()
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(H::ON_CONNECTED, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode ConditionProviderService::Provider::OnRequestConditions(
    /* [in] */ Int32 relevance)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(H::ON_REQUEST_CONDITIONS, relevance, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode ConditionProviderService::Provider::OnSubscribe(
    /* [in] */ IUri* conditionId)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(H::ON_SUBSCRIBE, conditionId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode ConditionProviderService::Provider::OnUnsubscribe(
    /* [in] */ IUri* conditionId)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(H::ON_UNSUBSCRIBE, conditionId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

//====================================
// ConditionProviderService::H
//====================================

const Int32 ConditionProviderService::H::ON_CONNECTED;
const Int32 ConditionProviderService::H::ON_REQUEST_CONDITIONS;
const Int32 ConditionProviderService::H::ON_SUBSCRIBE;
const Int32 ConditionProviderService::H::ON_UNSUBSCRIBE;

ConditionProviderService::H::H(
    /* [in] */ ConditionProviderService* host)
    : mHost(host)
{
}

ECode ConditionProviderService::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    String name;
    // try {
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case ON_CONNECTED: {
            name = "onConnected";
            assert(0 && "TODO");
            // mHost->OnConnected();
            break;
        }
        case ON_REQUEST_CONDITIONS: {
            name = "onRequestConditions";
            Int32 arg1;
            msg->GetArg1(&arg1);
            assert(0 && "TODO");
            // mHost->OnRequestConditions(arg1);
            break;
        }
        case ON_SUBSCRIBE: {
            name = "onSubscribe";
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            assert(0 && "TODO");
            // mHost->OnSubscribe(IUri::Probe(obj));
            break;
        }
        case ON_UNSUBSCRIBE: {
            name = "onUnsubscribe";
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            assert(0 && "TODO");
            // mHost->OnUnsubscribe(IUri::Probe(obj));
            break;
        }
    }
    // } catch (Throwable t) {
    //     Log.w(TAG, "Error running " + name, t);
    // }
    return NOERROR;
}

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos
