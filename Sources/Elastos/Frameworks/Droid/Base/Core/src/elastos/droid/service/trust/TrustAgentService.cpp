#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/service/trust/TrustAgentService.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Manifest;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Trust {

//==================
//TrustAgentService::MHandler
//==================

TrustAgentService::MHandler::MHandler(
    /* [in] */ TrustAgentService* host)
    : mHost(host)
{
}

ECode TrustAgentService::MHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case TrustAgentService::MSG_UNLOCK_ATTEMPT: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->OnUnlockAttempt(arg1 != 0);
            break;
        }
        case TrustAgentService::MSG_SET_TRUST_AGENT_FEATURES_ENABLED: {
            AutoPtr<IBundle> features;
            msg->PeekData((IBundle**)&features);
            AutoPtr<IBinder> token;
            msg->GetObj((IInterface**)&token);
            Boolean result;
            mHost->OnSetTrustAgentFeaturesEnabled(features, &result);
            synchronized(this) {
                ECode ec = mHost->mCallback->OnSetTrustAgentFeaturesEnabledCompleted(result, token);
                if (FAILED(ec)) {
                    mHost->OnError(String("calling onSetTrustAgentFeaturesEnabledCompleted()"));
                    return E_REMOTE_EXCEPTION;
                }
            }
            break;
        }
        case TrustAgentService::MSG_TRUST_TIMEOUT: {
            mHost->OnTrustTimeout();
            break;
        }
    }
    return NOERROR;
}

//====================
//TrustAgentService::GTRunnable
//====================

TrustAgentService::GTRunnable::GTRunnable(
    /* [in] */ ICharSequence* message,
    /* [in] */ Int64 durationMs,
    /* [in] */ Boolean initiatedByUser,
    /* [in] */ TrustAgentService* host)
    : mMessage(message)
    , mDurationMs(durationMs)
    , mInitiatedByUser(initiatedByUser)
    , mHost(host)
{
}

ECode TrustAgentService::GTRunnable::Run()
{
    return mHost->GrantTrust(mMessage, mDurationMs, mInitiatedByUser);
}

//=============================
//TrustAgentService::TrustAgentServiceWrapper
//=============================

CAR_INTERFACE_IMPL_3(TrustAgentService::TrustAgentServiceWrapper, Object, ITrustAgentServiceWrapper, IITrustAgentService, IBinder)

TrustAgentService::TrustAgentServiceWrapper::TrustAgentServiceWrapper()
{
}

TrustAgentService::TrustAgentServiceWrapper::TrustAgentServiceWrapper(
    /* [in] */ TrustAgentService* host)
    : mHost(host)
{
}

ECode TrustAgentService::TrustAgentServiceWrapper::constructor()
{
    return NOERROR;
}

ECode TrustAgentService::TrustAgentServiceWrapper::OnUnlockAttempt(
    /* [in] */ Boolean successful)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(TrustAgentService::MSG_UNLOCK_ATTEMPT, successful ? 1 : 0, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode TrustAgentService::TrustAgentServiceWrapper::OnTrustTimeout()
{
    Boolean result;
    return mHost->mHandler->SendEmptyMessage(TrustAgentService::MSG_TRUST_TIMEOUT, &result);
}

ECode TrustAgentService::TrustAgentServiceWrapper::SetCallback(
    /* [in] */ IITrustAgentServiceCallback* callback)
{
    synchronized(this) {
        mHost->mCallback = callback;
        // The managingTrust property is false implicitly on the server-side, so we only
        // need to set it here if the agent has decided to manage trust.
        if (mHost->mManagingTrust) {
            ECode ec = mHost->mCallback->SetManagingTrust(mHost->mManagingTrust);
            if (FAILED(ec)) {
                mHost->OnError(String("calling setManagingTrust()"));
                return E_REMOTE_EXCEPTION;
            }
        }
        if (mHost->mPendingGrantTrustTask != NULL) {
            mHost->mPendingGrantTrustTask->Run();
            mHost->mPendingGrantTrustTask = NULL;
        }
    }
    return NOERROR;
}

ECode TrustAgentService::TrustAgentServiceWrapper::SetTrustAgentFeaturesEnabled(
    /* [in] */ IBundle* features,
    /* [in] */ IBinder* token)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(TrustAgentService::MSG_SET_TRUST_AGENT_FEATURES_ENABLED, token, (IMessage**)&msg);
    msg->SetData(features);
    msg->SendToTarget();
    return NOERROR;
}

//============
//TrustAgentService
//============

const Boolean TrustAgentService::DEBUG = FALSE;

CAR_INTERFACE_IMPL(TrustAgentService, Service, ITrustAgentService)

TrustAgentService::TrustAgentService()
    : TAG(String("TrustAgentService"))
    , mManagingTrust(FALSE)
{
}

ECode TrustAgentService::constructor()
{
    AutoPtr<MHandler> mh = new MHandler(this);
    mHandler = IHandler::Probe(mh.Get());
    return NOERROR;
}

ECode TrustAgentService::OnCreate()
{
    Service::OnCreate();
    String path("");//TODO
    AutoPtr<IModuleInfo> moduleInfo;
    ECode ec = CReflector::AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo);
    AutoPtr<IClassInfo> classInfo;
    String shortClassName("");//TODO
    ec = moduleInfo->GetClassInfo(shortClassName, (IClassInfo**)&classInfo);
    AutoPtr<IComponentName> component;
    CComponentName::New((IContext*)this, classInfo, (IComponentName**)&component);
    AutoPtr<IPackageManager> pm;
    GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IServiceInfo> serviceInfo;
    ec = pm->GetServiceInfo(component, 0, (IServiceInfo**)&serviceInfo);
    if (FAILED(ec)) {
        String str("Can't get ServiceInfo for ");
        String str2;
        component->ToShortString(&str2);
        Logger::E(TAG, str + str2);
        return E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION;
    }
    String permission;
    serviceInfo->GetPermission(&permission);
    if (!Manifest::permission::BIND_TRUST_AGENT.Equals(permission)) {
        String str;
        component->FlattenToShortString(&str);
        Logger::E(TAG, str + " is not declared with the permission \"" + Manifest::permission::BIND_TRUST_AGENT + "\"");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode TrustAgentService::OnUnlockAttempt(
    /* [in] */ Boolean successful)
{
    return NOERROR;
}

ECode TrustAgentService::OnTrustTimeout()
{
    return NOERROR;
}

ECode TrustAgentService::OnError(
    /* [in] */ const String& msg)
{
    Slogger::V(TAG, "Remote exception while %s", msg.string());
    return NOERROR;
}

ECode TrustAgentService::OnSetTrustAgentFeaturesEnabled(
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode TrustAgentService::GrantTrust(
    /* [in] */ ICharSequence* message,
    /* [in] */ Int64 durationMs,
    /* [in] */ Boolean initiatedByUser)
{
    synchronized(mLock) {
        if (!mManagingTrust) {
            Logger::E(TAG, "Cannot grant trust if agent is not managing trust. Call setManagingTrust(true) first.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        if (mCallback != NULL) {
            ECode ec = mCallback->GrantTrust(message, durationMs, initiatedByUser);
            if (FAILED(ec)) {
                OnError(String("calling enableTrust()"));
                return E_REMOTE_EXCEPTION;
            }
        } else {
            // Remember trust has been granted so we can effectively grant it once the service
            // is bound.
            AutoPtr<GTRunnable> gtr = new GTRunnable(message, durationMs, initiatedByUser, this);
            mPendingGrantTrustTask = IRunnable::Probe(gtr);
        }
    }
    return NOERROR;
}

ECode TrustAgentService::RevokeTrust()
{
    synchronized(mLock) {
        if (mPendingGrantTrustTask != NULL) {
            mPendingGrantTrustTask = NULL;
        }
        if (mCallback != NULL) {
            ECode ec = mCallback->RevokeTrust();
            if (FAILED(ec)) {
                OnError(String("calling revokeTrust()"));
                return E_REMOTE_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

ECode TrustAgentService::SetManagingTrust(
    /* [in] */ Boolean managingTrust)
{
    synchronized(mLock) {
        if (mManagingTrust != managingTrust) {
            mManagingTrust = managingTrust;
            if (mCallback != NULL) {
                ECode ec = mCallback->SetManagingTrust(managingTrust);
                if (FAILED(ec)) {
                    OnError(String("calling setManagingTrust()"));
                    return E_REMOTE_EXCEPTION;
                }
            }
        }
    }
    return NOERROR;
}

ECode TrustAgentService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** b)
{
    String str;
    IObject::Probe(intent)->ToString(&str);
    if (DEBUG) Slogger::V(TAG, String("onBind() intent = ") + str);
    AutoPtr<TrustAgentServiceWrapper> wrapper = new TrustAgentServiceWrapper(this);
    *b = IBinder::Probe(wrapper.Get());
    REFCOUNT_ADD(*b)
    return NOERROR;
}

} // namespace Trust
} // namespace Service
} // namepsace Droid
} // namespace Elastos
