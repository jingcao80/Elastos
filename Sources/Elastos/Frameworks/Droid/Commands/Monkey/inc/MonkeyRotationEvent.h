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

#ifndef __MONKEYROTATIONEVENT_H__
#define __MONKEYROTATIONEVENT_H__

#include "MonkeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyRotationEvent : public MonkeyEvent
{
protected:
    MonkeyRotationEvent();

    MonkeyRotationEvent(
        /* [in] */ Int32 degree,
        /* [in] */ Boolean persist);

    CARAPI Init(
        /* [in] */ Int32 degree,
        /* [in] */ Boolean persist);

    CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

private:
    const volatile Int32 mRotationDegree;
    const volatile Boolean mPersist;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYROTATIONEVENT_H__
