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

#ifndef __MONKEYMOTIONEVENT_H__
#define __MONKEYMOTIONEVENT_H__

#include "MonkeyEvent.h"
#include <elastos/utility/etl/HashMap.h>
//#include <view/CMotionEvent.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::CMotionEvent;
//using Elastos::Droid::View::CMotionEvent::PointerCoords;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IPointerCoords;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyMotionEvent : public MonkeyEvent
{
protected:
    MonkeyMotionEvent();

    MonkeyMotionEvent(
        /* [in] */ Int32 type,
        /* [in] */ Int32 source,
        /* [in] */ Int32 action);

    ~MonkeyMotionEvent();

    CARAPI Init(
        /* [in] */ Int32 type,
        /* [in] */ Int32 source,
        /* [in] */ Int32 action);

    CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

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

    CARAPI_(Boolean) GetIntermediateNote();

    CARAPI_(Int32) GetAction();

    CARAPI_(Int64) GetDownTime();

    CARAPI_(Int64) GetEventTime();

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

    CARAPI_(Boolean) IsThrottlable();

    virtual CARAPI_(String) GetTypeLabel() = 0;

private:
    AutoPtr<IMotionEvent> GetEvent();

private:
    Int64 mDownTime;
    Int64 mEventTime;
    Int32 mAction;
    AutoPtr< HashMap<Int32, AutoPtr<IPointerCoords> > > mPointers;
    Int32 mMetaState;
    Float mXPrecision;
    Float mYPrecision;
    Int32 mDeviceId;
    Int32 mSource;
    Int32 mFlags;
    Int32 mEdgeFlags;
    Boolean mIntermediateNote;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYMOTIONEVENT_H__
