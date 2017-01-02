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

#include "elastos/droid/service/gesture/EdgeGestureManager.h"
#include "elastos/droid/service/gesture/CEdgeGestureManager.h"
#include "elastos/droid/service/gesture/CEdgeGestureActivationListenerDelegator.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/internal/utility/gesture/CEdgeGesturePosition.h"
#include <elastos/core/Thread.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Internal::Utility::Gesture::CEdgeGesturePosition;
using Elastos::Core::AutoLock;
using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Gesture {

//======================================================================
// EdgeGestureManager::EdgeGestureActivationListener
//======================================================================

CAR_INTERFACE_IMPL(EdgeGestureManager::EdgeGestureActivationListener, Object, IEdgeGestureActivationListener)

EdgeGestureManager::EdgeGestureActivationListener::EdgeGestureActivationListener()
{}

ECode EdgeGestureManager::EdgeGestureActivationListener::constructor()
{
    AutoPtr<ILooper> mainLooper = Looper::GetMainLooper();
    return constructor(mainLooper);
}

ECode EdgeGestureManager::EdgeGestureActivationListener::constructor(
    /* [in] */ ILooper* looper)
{
    CHandler::New(looper, (IHandler**)&mHandler);
    return CEdgeGestureActivationListenerDelegator::New(this, (IIEdgeGestureActivationListener**)&mDelegator);
}

ECode EdgeGestureManager::EdgeGestureActivationListener::SetHostCallback(
    /* [in] */ IIEdgeGestureHostCallback* hostCallback)
{
    mCallback = hostCallback;
    return NOERROR;
}

ECode EdgeGestureManager::EdgeGestureActivationListener::GainTouchFocus(
    /* [in] */ IBinder* applicationWindowToken,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ECode ec = mCallback->GainTouchFocus(applicationWindowToken, result);
    if (FAILED(ec)) {
        Slogger::W(EdgeGestureManager::TAG, "gainTouchFocus failed: ec=%08x", ec);
        /* fall through */
    }
    return NOERROR;
}

ECode EdgeGestureManager::EdgeGestureActivationListener::DropEventsUntilLift(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ECode ec = mCallback->DropEventsUntilLift(result);
    if (FAILED(ec)) {
        Slogger::W(EdgeGestureManager::TAG, "dropNextEvents failed: ec=%08x", ec);
        /* fall through */
    }
    return NOERROR;
}

ECode EdgeGestureManager::EdgeGestureActivationListener::RestoreListenerState()
{
    if (EdgeGestureManager::DEBUG) {
        AutoPtr<IThread> thread = Thread::GetCurrentThread();
        String name;
        thread->GetName(&name);
        Slogger::D(TAG, "restore listener state: %s", name.string());
    }

    ECode ec = mCallback->RestoreListenerState();
    if (FAILED(ec)) {
        Slogger::W(EdgeGestureManager::TAG, "restoreListenerState failed: ec=%08x", ec);
        /* fall through */
    }
    return NOERROR;
}

ECode EdgeGestureManager::EdgeGestureActivationListener::OnEdgeGestureActivationInner(
    /* [in] */ Int32 touchX,
    /* [in] */ Int32 touchY,
    /* [in] */ Int32 positionIndex,
    /* [in] */ Int32 flags)
{
    AutoPtr<IRunnable> runnable = new OnEdgeGestureActivationRunnable(
        this, touchX, touchY, positionIndex, flags);
    Boolean bval;
    return mHandler->Post(runnable, &bval);
}

//======================================================================
// EdgeGestureManager::EdgeGestureActivationListener::OnEdgeGestureActivationRunnable
//======================================================================
EdgeGestureManager::EdgeGestureActivationListener::OnEdgeGestureActivationRunnable::OnEdgeGestureActivationRunnable(
    /* [in] */ EdgeGestureManager::EdgeGestureActivationListener* host,
    /* [in] */ Int32 touchX,
    /* [in] */ Int32 touchY,
    /* [in] */ Int32 positionIndex,
    /* [in] */ Int32 flags)
    : mHost(host)
    , mTouchX(touchX)
    , mTouchY(touchY)
    , mPositionIndex(positionIndex)
    , mFlags(flags)
{}

ECode EdgeGestureManager::EdgeGestureActivationListener::OnEdgeGestureActivationRunnable::Run()
{
    AutoPtr<ArrayOf<IEdgeGesturePosition*> > values = CEdgeGesturePosition::Values();
    return mHost->OnEdgeGestureActivation(mTouchX, mTouchY, (*values)[mPositionIndex], mFlags);
}

//======================================================================
// EdgeGestureManager
//======================================================================

const String EdgeGestureManager::TAG("EdgeGestureManager");
const Boolean EdgeGestureManager::DEBUG = FALSE;
AutoPtr<IEdgeGestureManager> EdgeGestureManager::sInstance;
Object EdgeGestureManager::sLock;

CAR_INTERFACE_IMPL(EdgeGestureManager, Object, IEdgeGestureManager)

EdgeGestureManager::EdgeGestureManager()
{}

ECode EdgeGestureManager::constructor()
{
    return NOERROR;
}

ECode EdgeGestureManager::constructor(
    /* [in] */ IIEdgeGestureService* ps)
{
    mPs = ps;
    return NOERROR;
}

AutoPtr<IEdgeGestureManager> EdgeGestureManager::GetInstance()
{
    AutoLock lock(sLock);
    if (sInstance == NULL) {
        AutoPtr<IInterface> obj = ServiceManager::GetService(String("edgegestureservice"));
        IIEdgeGestureService* service = IIEdgeGestureService::Probe(obj);
        CEdgeGestureManager::New((IEdgeGestureManager**)&sInstance);
        ((CEdgeGestureManager*)sInstance.Get())->constructor(service);
    }
    return sInstance;
}

ECode EdgeGestureManager::IsPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPs != NULL;
    return NOERROR;
}

ECode EdgeGestureManager::SetEdgeGestureActivationListener(
    /* [in] */ IEdgeGestureActivationListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (DEBUG) {
        Slogger::D(TAG, "Set edge gesture activation listener");
    }

    if (mPs == NULL) {
        return NOERROR;
    }

    EdgeGestureActivationListener* agal = (EdgeGestureActivationListener*)listener;

    AutoPtr<IIEdgeGestureHostCallback> callback;
    ECode ec = mPs->RegisterEdgeGestureActivationListener(
        agal->mDelegator, (IIEdgeGestureHostCallback**)&callback);
    FAIL_GOTO(ec, _EXIT_)

    listener->SetHostCallback(callback);
    *result = TRUE;

_EXIT_:
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to set edge gesture activation listener: ec=%08x", ec);
    }

    return NOERROR;
}

ECode EdgeGestureManager::UpdateEdgeGestureActivationListener(
    /* [in] */ IEdgeGestureActivationListener* listener,
    /* [in] */ Int32 positions)
{
    if (DEBUG) {
        Slogger::D(TAG, "Update edge gesture activation listener: 0x08x", positions);
    }
    if (mPs == NULL) {
        return NOERROR;
    }

    EdgeGestureActivationListener* agal = (EdgeGestureActivationListener*)listener;
    ECode ec = mPs->UpdateEdgeGestureActivationListener(IBinder::Probe(agal->mDelegator), positions);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to update edge gesture activation listener: %08x", ec);
    }
    return ec;
}


} // namespace Gesture
} // namespace Service
} // namepsace Droid
} // namespace Elastos
