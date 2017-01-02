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

#include "elastos/droid/server/am/CPendingAssistExtras.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL_2(CPendingAssistExtras, Object, IBinder, IRunnable)
CAR_OBJECT_IMPL(CPendingAssistExtras)

CPendingAssistExtras::CPendingAssistExtras()
    : mHaveResult(FALSE)
{}

ECode CPendingAssistExtras::constructor(
    /* [in] */ IActivityRecord* activity,
    /* [in] */ IBundle* extras,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& hint,
    /* [in] */ Int32 userHandle)
{
    mActivity = (ActivityRecord*)activity;
    mExtras = extras;
    mIntent = intent;
    mHint = hint;
    mUserHandle = userHandle;
    return NOERROR;
}

ECode CPendingAssistExtras::Run()
{
    Slogger::W("ActivityManager", "getAssistContextExtras failed: timeout retrieving from %s",
            mActivity->ToString().string());

    AutoLock syncLock(this);
    mHaveResult = TRUE;
    NotifyAll();
    return NOERROR;
}

ECode CPendingAssistExtras::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CPendingAssistExtras";
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
