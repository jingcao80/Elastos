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
#include "elastos/droid/internal/policy/impl/keyguard/KeyguardServiceWrapper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

CAR_INTERFACE_IMPL(KeyguardServiceWrapper, Object, IIKeyguardService)

String KeyguardServiceWrapper::TAG("KeyguardServiceWrapper");

KeyguardServiceWrapper::KeyguardServiceWrapper()
{
}

ECode KeyguardServiceWrapper::constructor(
    /* [in] */ IIKeyguardService* service)
{
    mService = service;
    return NOERROR;
}

ECode KeyguardServiceWrapper::IsShowing(
    /* [out] */ Boolean* showing)
{
    VALIDATE_NOT_NULL(showing);
    *showing = FALSE;
    ECode ec = mService->IsShowing(showing);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::IsSecure(
    /* [out] */ Boolean* secure)
{
    VALIDATE_NOT_NULL(secure);
    *secure = FALSE;
    ECode ec = mService->IsSecure(secure);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::IsShowingAndNotOccluded(
    /* [out] */ Boolean* showingAndNotOcclude)
{
    VALIDATE_NOT_NULL(showingAndNotOcclude);
    *showingAndNotOcclude = FALSE;
    ECode ec = mService->IsShowingAndNotOccluded(showingAndNotOcclude);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::IsInputRestricted(
    /* [out] */ Boolean* inputRestricted)
{
    VALIDATE_NOT_NULL(inputRestricted);
    *inputRestricted = FALSE;
    ECode ec = mService->IsInputRestricted(inputRestricted);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::IsDismissable(
    /* [out] */ Boolean* dismissable)
{
    VALIDATE_NOT_NULL(dismissable);
    *dismissable = FALSE;
    ECode ec = mService->IsDismissable(dismissable);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::VerifyUnlock(
    /* [in] */ IIKeyguardExitCallback* cb)
{
    ECode ec = mService->VerifyUnlock(cb);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::KeyguardDone(
    /* [in] */ Boolean authenticated,
    /* [in] */ Boolean wakeup)
{
    ECode ec = mService->KeyguardDone(authenticated, wakeup);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::SetOccluded(
    /* [in] */ Boolean isOccluded,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    ECode ec = mService->SetOccluded(isOccluded, result);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        *result = IIKeyguardServiceConstants::KEYGUARD_SERVICE_SET_OCCLUDED_RESULT_NONE;
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::Dismiss()
{
    ECode ec = mService->Dismiss();
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnDreamingStarted()
{
    ECode ec = mService->OnDreamingStarted();
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnDreamingStopped()
{
    ECode ec = mService->OnDreamingStopped();
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnScreenTurnedOff(
    /* [in] */ Int32 reason)
{
    ECode ec = mService->OnScreenTurnedOff(reason);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnScreenTurnedOn(
    /* [in] */ IIKeyguardShowCallback* cb)
{
    ECode ec = mService->OnScreenTurnedOn(cb);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::SetKeyguardEnabled(
    /* [in] */ Boolean enabled)
{
    ECode ec = mService->SetKeyguardEnabled(enabled);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnSystemReady()
{
    ECode ec = mService->OnSystemReady();
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::DoKeyguardTimeout(
    /* [in] */ IBundle* options)
{
    ECode ec = mService->DoKeyguardTimeout(options);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::SetCurrentUser(
    /* [in] */ Int32 userId)
{
    ECode ec = mService->SetCurrentUser(userId);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnBootCompleted()
{
    ECode ec = mService->OnBootCompleted();
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::StartKeyguardExitAnimation(
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 fadeoutDuration)
{
    ECode ec = mService->StartKeyguardExitAnimation(startTime, fadeoutDuration);
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnActivityDrawn()
{
    ECode ec = mService->OnActivityDrawn();
    if (FAILED(ec)) {
        Logger::W(TAG, "Remote Exception, ec: 0x%x", ec);
        return ec;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::ShowAssistant()
{
    // Not used by PhoneWindowManager
    return NOERROR;
}

ECode KeyguardServiceWrapper::Dispatch(
    /* [in] */ IMotionEvent* event)
{
    // Not used by PhoneWindowManager.  See code in {@link NavigationBarView}
    return NOERROR;
}

ECode KeyguardServiceWrapper::LaunchCamera()
{
    // Not used by PhoneWindowManager.  See code in {@link NavigationBarView}
    return NOERROR;
}


ECode KeyguardServiceWrapper::AsBinder(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    *binder = IBinder::Probe(mService);
    REFCOUNT_ADD(*binder)
    return NOERROR;
}


} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
