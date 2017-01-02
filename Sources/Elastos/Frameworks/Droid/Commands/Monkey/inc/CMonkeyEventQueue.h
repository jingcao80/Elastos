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

#ifndef __CMONKEYEVENTQUEUE_H__
#define __CMONKEYEVENTQUEUE_H__

#include "_CMonkeyEventQueue.h"
#include "MonkeyEventQueue.h"

using Elastos::Core::IRandom;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyEventQueue), public MonkeyEventQueue
{
public:
    CAR_OBJECT_DECL()

    CARAPI IsEmpty(
        /* [out] */ Boolean *isEmpty);

    CARAPI GetFirst(
        /* [out] */ IMonkeyEvent **monkeyEvent);

    CARAPI RemoveFirst();

    CARAPI AddLast(
        /* [in] */ IMonkeyEvent *event);

    CARAPI constructor(
        /* [in] */ IRandom *random,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYEVENTQUEUE_H__
