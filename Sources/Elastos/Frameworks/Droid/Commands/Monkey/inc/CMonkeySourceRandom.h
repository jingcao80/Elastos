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

#ifndef __CMONKEYSOURCERANDOM_H__
#define __CMONKEYSOURCERANDOM_H__

#include "_CMonkeySourceRandom.h"
#include "MonkeySourceRandom.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceRandom), public MonkeySourceRandom
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetNextEvent(
        /* [out] */ IMonkeyEvent **event);

    CARAPI SetVerbose(
        /* [in] */ Int32 verbose);

    CARAPI Validate(
        /* [out] */ Boolean *result);

    CARAPI SetFactors(
        /* [in] */ ArrayOf<Float> *factors);

    CARAPI SetFactors(
        /* [in] */ Int32 index,
        /* [in] */ Float v);

    CARAPI GenerateActivity();

    CARAPI constructor(
        /* [in] */ IRandom *random,
        /* [in] */ IObjectContainer *mainApps,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle);

    static CARAPI GetKeyName(
    /* [in] */ Int32 keycode,
    /* [out] */ String *ret);

    static CARAPI GetKeyCode(
        /* [in] */ const String& keyName,
        /* [out] */ Int32 *keycode);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCERANDOM_H__
