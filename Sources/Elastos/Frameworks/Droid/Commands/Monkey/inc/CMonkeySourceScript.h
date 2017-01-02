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

#ifndef __CMONKEYSOURCESCRIPT_H__
#define __CMONKEYSOURCESCRIPT_H__

#include "_CMonkeySourceScript.h"
#include "MonkeySourceScript.h"

using Elastos::Droid::Commands::Monkey::IMonkeyEvent;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceScript), public MonkeySourceScript
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetNextEvent(
        /* [out] */ IMonkeyEvent **event);

    CARAPI SetVerbose(
        /* [in] */ Int32 verbose);

    CARAPI Validate(
        /* [out] */ Boolean *result);

    CARAPI constructor(
        /* [in] */ IRandom *random,
        /* [in] */ const String& filename,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle,
        /* [in] */ Int64 profileWaitTime,
        /* [in] */ Int64 deviceSleepTime);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCESCRIPT_H__
