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

#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/internal/policy/impl/WakeGestureListener.h"
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Hardware::EIID_ITriggerEventListener;
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//            WakeGestureListener::InnerTriggerEventListener
//=====================================================================
CAR_INTERFACE_IMPL(WakeGestureListener::InnerTriggerEventListener, Object, ITriggerEventListener)

WakeGestureListener::InnerTriggerEventListener::InnerTriggerEventListener(
    /* [in] */ WakeGestureListener* owner)
    : mOwner(owner)
{
}

ECode WakeGestureListener::InnerTriggerEventListener::OnTrigger(
    /* [in] */ ITriggerEvent* event)
{
    VALIDATE_NOT_NULL(event);
    AutoLock(mOwner->mLock);
    mOwner->mTriggerRequested = FALSE;
    Boolean res;
    mOwner->mHandler->Post(mOwner->mWakeUpRunnable, &res);
    return NOERROR;
}

//=====================================================================
//                  WakeGestureListener::InnerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(WakeGestureListener::InnerRunnable, Object, IRunnable)

WakeGestureListener::InnerRunnable::InnerRunnable(
    /* [in] */ WakeGestureListener* owner)
    : mOwner(owner)
{
}

ECode WakeGestureListener::InnerRunnable::Run()
{
    mOwner->OnWakeUp();
    return NOERROR;
}

//=====================================================================
//                         WakeGestureListener
//=====================================================================
CAR_INTERFACE_IMPL(WakeGestureListener, Object, IWakeGestureListener)

const String WakeGestureListener::TAG("WakeGestureListener");

WakeGestureListener::WakeGestureListener()
{
    mLock = new Object();
    mListener = new WakeGestureListener::InnerTriggerEventListener(this);
    mWakeUpRunnable = new WakeGestureListener::InnerRunnable(this);
}

ECode WakeGestureListener::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&obj);
    mSensorManager = ISensorManager::Probe(obj);
    mHandler = handler;
    mSensorManager->GetDefaultSensor(ISensor::TYPE_WAKE_GESTURE, (ISensor**)&mSensor);
    return NOERROR;
}

ECode WakeGestureListener::IsSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    return mSensor != NULL;
}

ECode WakeGestureListener::RequestWakeUpTrigger()
{
    AutoLock lock(mLock);
    if (mSensor != NULL && !mTriggerRequested)
    {
        mTriggerRequested = TRUE;
        Boolean res;
        mSensorManager->RequestTriggerSensor(mListener, mSensor, &res);
    }
    return NOERROR;
}

ECode WakeGestureListener::CancelWakeUpTrigger()
{
    AutoLock lock(mLock);
    if (mSensor != NULL && mTriggerRequested)
    {
        mTriggerRequested = FALSE;
        Boolean res;
        mSensorManager->CancelTriggerSensor(mListener, mSensor, &res);
    }
    return NOERROR;
}

ECode WakeGestureListener::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    VALIDATE_NOT_NULL(pw);
    AutoLock lock(mLock);
    pw->Println(prefix + TAG);
    String p = prefix + String("  ");

    pw->Println(p + String("mTriggerRequested=") + (mTriggerRequested?String("True"):String("False")));
    String name;
    mSensor->GetName(&name);
    pw->Println(p + String("mSensor=") + name);
    return NOERROR;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos


