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

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos.Droid.App.h"
#include "elastos/droid/server/am/CActivityRecordToken.h"
#include "elastos/droid/server/am/ActivityRecord.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::EIID_IApplicationToken;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL_2(CActivityRecordToken, Object, IApplicationToken, IBinder)
CAR_OBJECT_IMPL(CActivityRecordToken)

CActivityRecordToken::CActivityRecordToken()
{
}

CActivityRecordToken::~CActivityRecordToken()
{
}

ECode CActivityRecordToken::constructor(
    /* [in] */ IActivityRecord* activity)
{
    ActivityRecord* a = (ActivityRecord*)activity;
    if (a) {
        a->GetWeakReference((IWeakReference**)&mWeakActivity);
    }
    return NOERROR;
}

AutoPtr<ActivityRecord> CActivityRecordToken::GetActivityRecord()
{
    AutoPtr<ActivityRecord> activity;
    AutoPtr<IActivityRecord> obj;
    mWeakActivity->Resolve(EIID_IActivityRecord, (IInterface**)&obj);
    if (obj) {
        activity = (ActivityRecord*)obj.Get();
    }

    return activity;
}

ECode CActivityRecordToken::WindowsDrawn()
{
    AutoPtr<ActivityRecord> ar = GetActivityRecord();
    if (ar != NULL) {
        return ar->WindowsDrawn();
    }
    return NOERROR;
}

ECode CActivityRecordToken::WindowsVisible()
{
    AutoPtr<ActivityRecord> ar = GetActivityRecord();
    if (ar != NULL) {
        return ar->WindowsVisible();
    }
    return NOERROR;
}

ECode CActivityRecordToken::WindowsGone()
{
    AutoPtr<ActivityRecord> ar = GetActivityRecord();
    if (ar != NULL) {
        return ar->WindowsGone();
    }
    return NOERROR;
}

ECode CActivityRecordToken::KeyDispatchingTimedOut(
    /* [in] */ const String& reason,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ActivityRecord> ar = GetActivityRecord();
    if (ar != NULL) {
        return ar->KeyDispatchingTimedOut(reason, result);
    }

    *result = FALSE;
    return NOERROR;
}

ECode CActivityRecordToken::GetKeyDispatchingTimeout(
    /* [out] */ Int64* timeout)
{
    VALIDATE_NOT_NULL(timeout);

    AutoPtr<ActivityRecord> ar = GetActivityRecord();
    if (ar != NULL) {
        return ar->GetKeyDispatchingTimeout(timeout);
    }

    *timeout = 0;
    return NOERROR;
}

ECode CActivityRecordToken::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb(128);
    sb += "CActivityRecordToken{";
    sb += StringUtils::ToHexString((Int32)this);
    sb += ", ActivityRecord:";

    AutoPtr<ActivityRecord> ar = GetActivityRecord();
    if (ar != NULL) {
        sb += ar->ToString();
    }
    else {
        sb += "NULL";
    }

    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
