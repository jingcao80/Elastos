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

#ifndef __CMONKEYKEYEVENT_H__
#define __CMONKEYKEYEVENT_H__

#include "_CMonkeyKeyEvent.h"
#include "MonkeyKeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyKeyEvent) , public MonkeyKeyEvent
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

    CARAPI GetKeyCode(
        /* [out] */ Int32 *keyCode);

    CARAPI GetAction(
        /* [out] */ Int32 *action);

    CARAPI GetDownTime(
        /* [out] */ Int64 *downTime);

    CARAPI GetEventTime(
        /* [out] */ Int64 *eventTime);

    CARAPI SetDownTime(
        /* [in] */ Int64 downTime);

    CARAPI SetEventTime(
        /* [in] */ Int64 eventTime);

    CARAPI constructor(
        /* [in] */ Int32 action,
        /* [in] */ Int32 keyCode);

    CARAPI constructor(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 repeatCount,
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 device,
        /* [in] */ Int32 scanCode);

    CARAPI constructor(
        /* [in] */ IKeyEvent *keyEvent);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYKEYEVENT_H__
