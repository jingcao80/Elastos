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

#ifndef __CMONKEYTOUCHEVENT_H__
#define __CMONKEYTOUCHEVENT_H__

#include "_CMonkeyTouchEvent.h"
#include "MonkeyTouchEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyTouchEvent) , public MonkeyTouchEvent
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetEventType(
        /* [out] */ Int32 *eventType);

    CARAPI IsThrottlable(
        /* [out] */ Boolean *isThrottlable);

    CARAPI InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose,
        /* [out] */ Int32 *result);

    CARAPI AddPointer(
        /* [in] */ Int32 id,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI AddPointer(
        /* [in] */ Int32 id,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float pressure,
        /* [in] */ Float size);

    CARAPI SetIntermediateNote(
        /* [in] */ Boolean b);

    CARAPI GetIntermediateNote(
        /* [out] */ Boolean *b);

    CARAPI GetAction(
        /* [out] */ Int32 *action);

    CARAPI GetDownTime(
        /* [out] */ Int64 *downtime);

    CARAPI GetEventTime(
        /* [out] */ Int64 *eventTime);

    CARAPI SetDownTime(
        /* [in] */ Int64 downTime);

    CARAPI SetEventTime(
        /* [in] */ Int64 eventTime);

    CARAPI SetMetaState(
        /* [in] */ Int32 metaState);

    CARAPI SetPrecision(
        /* [in] */ Float xPrecision,
        /* [in] */ Float yPrecision);

    CARAPI SetDeviceId(
        /* [in] */ Int32 deviceId);

    CARAPI SetEdgeFlags(
        /* [in] */ Int32 edgeFlags);

    CARAPI constructor(
        /* [in] */ Int32 action);

    PInterface Probe(
    /* [in] */ REIID riid);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYTOUCHEVENT_H__
