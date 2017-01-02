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

#ifndef __CMONKEYSOURCERANDOMSCRIPT_H__
#define __CMONKEYSOURCERANDOMSCRIPT_H__

#include "_CMonkeySourceRandomScript.h"
#include "MonkeySourceRandomScript.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceRandomScript), public MonkeySourceRandomScript
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
        /* [in] */ const String& setupFileName,
        /* [in] */ ArrayOf<String> *scriptFileNames,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle,
        /* [in] */ IRandom *random,
        /* [in] */ Int64 profileWaitTime,
        /* [in] */ Int64 deviceSleepTime,
        /* [in] */ Boolean randomizeScript);

    CARAPI constructor(
        /* [in] */ ArrayOf<String> *scriptFileNames,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle,
        /* [in] */ IRandom *random,
        /* [in] */ Int64 profileWaitTime,
        /* [in] */ Int64 deviceSleepTime,
        /* [in] */ Boolean randomizeScript);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCERANDOMSCRIPT_H__
