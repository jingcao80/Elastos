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

#include "CMonkeyTrackballEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyTrackballEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType);
    *eventType = MonkeyTrackballEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyTrackballEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable);
    *isThrottlable = MonkeyTrackballEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyTrackballEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result);
    *result = MonkeyTrackballEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyTrackballEvent::AddPointer(
    /* [in] */ Int32 id,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    MonkeyTrackballEvent::AddPointer(id, x, y);
    return NOERROR;
}

ECode CMonkeyTrackballEvent::AddPointer(
    /* [in] */ Int32 id,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float pressure,
    /* [in] */ Float size)
{
    MonkeyTrackballEvent::AddPointer(id, x, y, pressure, size);
    return NOERROR;
}

ECode CMonkeyTrackballEvent::SetIntermediateNote(
    /* [in] */ Boolean b)
{
    MonkeyTrackballEvent::SetIntermediateNote(b);
    return NOERROR;
}

ECode CMonkeyTrackballEvent::GetIntermediateNote(
    /* [out] */ Boolean *b)
{
    VALIDATE_NOT_NULL(b);
    *b = MonkeyTrackballEvent::GetIntermediateNote();
    return NOERROR;
}

ECode CMonkeyTrackballEvent::GetAction(
    /* [out] */ Int32 *action)
{
    VALIDATE_NOT_NULL(action);
    *action = MonkeyTrackballEvent::GetAction();
    return NOERROR;
}

ECode CMonkeyTrackballEvent::GetDownTime(
    /* [out] */ Int64 *downtime)
{
    VALIDATE_NOT_NULL(downtime);
    *downtime = MonkeyTrackballEvent::GetDownTime();
    return NOERROR;
}

ECode CMonkeyTrackballEvent::GetEventTime(
    /* [out] */ Int64 *eventtime)
{
    VALIDATE_NOT_NULL(eventtime);
    *eventtime = MonkeyTrackballEvent::GetEventTime();
    return NOERROR;
}

ECode CMonkeyTrackballEvent::SetDownTime(
    /* [in] */ Int64 downtime)
{
    MonkeyTrackballEvent::SetDownTime(downtime);
    return NOERROR;
}

ECode CMonkeyTrackballEvent::SetEventTime(
    /* [in] */ Int64 eventtime)
{
    MonkeyTrackballEvent::SetEventTime(eventtime);
    return NOERROR;
}

ECode CMonkeyTrackballEvent::SetMetaState(
    /* [in] */ Int32 metaState)
{
    MonkeyTrackballEvent::SetMetaState(metaState);
    return NOERROR;
}

ECode CMonkeyTrackballEvent::SetPrecision(
    /* [in] */ Float xPrecision,
    /* [in] */ Float yPrecision)
{
    MonkeyTrackballEvent::SetPrecision(xPrecision, yPrecision);
    return NOERROR;
}

ECode CMonkeyTrackballEvent::SetDeviceId(
    /* [in] */ Int32 deviceId)
{
    MonkeyTrackballEvent::SetDeviceId(deviceId);
    return NOERROR;
}

ECode CMonkeyTrackballEvent::SetEdgeFlags(
    /* [in] */ Int32 edgeFlags)
{
    MonkeyTrackballEvent::SetEdgeFlags(edgeFlags);
    return NOERROR;
}

ECode CMonkeyTrackballEvent::constructor(
    /* [in] */ Int32 action)
{
    MonkeyTrackballEvent::Init(action);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
