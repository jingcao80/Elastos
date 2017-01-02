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
#include "elastos/droid/app/CInstrumentationActivityMonitor.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CInstrumentationActivityMonitor, Object, IInstrumentationActivityMonitor)

CAR_OBJECT_IMPL(CInstrumentationActivityMonitor)

CInstrumentationActivityMonitor::CInstrumentationActivityMonitor()
    : mHits(0)
    , mBlock(FALSE)
{
}

ECode CInstrumentationActivityMonitor::constructor(
    /* [in] */ IIntentFilter* which,
    /* [in] */ IInstrumentationActivityResult* result,
    /* [in] */ Boolean block)
{
    mWhich = which;
    mResult = result;
    mBlock = block;
    return NOERROR;
}

ECode CInstrumentationActivityMonitor::constructor(
    /* [in] */ const String& cls,
    /* [in] */ IInstrumentationActivityResult* result,
    /* [in] */ Boolean block)
{
    mClass = cls;
    mResult = result;
    mBlock = block;
    return NOERROR;
}

ECode CInstrumentationActivityMonitor::GetFilter(
    /* [out] */ IIntentFilter** filter)
{
    VALIDATE_NOT_NULL(filter)
    *filter = mWhich;
    REFCOUNT_ADD(*filter)
    return NOERROR;
}

ECode CInstrumentationActivityMonitor::GetResult(
    /* [out] */ IInstrumentationActivityResult** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mResult;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CInstrumentationActivityMonitor::IsBlocking(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBlock;
    return NOERROR;
}

ECode CInstrumentationActivityMonitor::GetHits(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time)
    *time = mHits;
    return NOERROR;
}

ECode CInstrumentationActivityMonitor::SetHits(
    /* [in] */ Int32 time)
{
    mHits = time;
    return NOERROR;
}

ECode CInstrumentationActivityMonitor::GetLastActivity(
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)
    *activity = mLastActivity;
    REFCOUNT_ADD(*activity)
    return NOERROR;
}

ECode CInstrumentationActivityMonitor::WaitForActivity(
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)
    *activity = NULL;

    AutoLock lock(this);
    if (mLastActivity == NULL) {
        // try {
        // TODO
        Wait();
        // } catch (InterruptedException e) {
        // }
    }

    *activity = mLastActivity;
    REFCOUNT_ADD(*activity)
    mLastActivity = NULL;
    return NOERROR;
}

ECode CInstrumentationActivityMonitor::WaitForActivityWithTimeout(
    /* [in] */ Int64 timeOut,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);
    *activity = NULL;

    AutoLock lock(this);
    if (mLastActivity == NULL) {
        // try {
        // TODO
        Wait(timeOut);
        // } catch (InterruptedException e) {
        // }
    }

    if (mLastActivity == NULL) {
        return NOERROR;
    }
    else {
        *activity = mLastActivity;
        REFCOUNT_ADD(*activity)
        mLastActivity = NULL;
        return NOERROR;
    }
}

ECode CInstrumentationActivityMonitor::Match(
    /* [in] */ IContext* who,
    /* [in] */ IActivity* activity,
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoLock lock(this);
    if (mWhich != NULL) {
        AutoPtr<IContentResolver> cr;
        who->GetContentResolver((IContentResolver**)&cr);
        Int32 value;
        mWhich->Match(cr, intent, TRUE, String("Instrumentation"), &value);
        if (value < 0) {
            *result = FALSE;
            return NOERROR;
        }
    }
    if (!mClass.IsNull()) {
        String cls;
        AutoPtr<IComponentName> cn;
        if (activity != NULL) {
            EGuid clsId;
            IObject::Probe(activity)->GetClassID(&clsId);
            cls = clsId.mUunm;
        }
        else if (intent->GetComponent((IComponentName**)&cn), cn != NULL) {
            cn->GetClassName(&cls);
        }
        if (cls.IsNull() || !mClass.Equals(cls)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    if (activity != NULL) {
        mLastActivity = activity;
        NotifyAll();
    }
    *result = TRUE;
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos

