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

#include "CMonkeySourceRandom.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {


ECode CMonkeySourceRandom::GetNextEvent(
    /* [out] */ IMonkeyEvent **event)
{
    VALIDATE_NOT_NULL(event);
    MonkeySourceRandom::GetNextEvent(event);
    return NOERROR;
}

ECode CMonkeySourceRandom::SetVerbose(
    /* [in] */ Int32 verbose)
{
    return MonkeySourceRandom::SetVerbose(verbose);
}

ECode CMonkeySourceRandom::Validate(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    return MonkeySourceRandom::Validate(result);
}

ECode CMonkeySourceRandom::SetFactors(
    /* [in] */ ArrayOf<Float> *factors)
{
    return MonkeySourceRandom::SetFactors(factors);
}

ECode CMonkeySourceRandom::SetFactors(
    /* [in] */ Int32 index,
    /* [in] */ Float v)
{
    return MonkeySourceRandom::SetFactors(index, v);
}

ECode CMonkeySourceRandom::GenerateActivity()
{
    return MonkeySourceRandom::GenerateActivity();
}

ECode CMonkeySourceRandom::constructor(
    /* [in] */ Elastos::Core::IRandom *random,
    /* [in] */ IObjectContainer *mainApps,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle)
{
    MonkeySourceRandom::Init(
        random,
        mainApps,
        throttle,
        randomizeThrottle);
    return NOERROR;
}

CARAPI CMonkeySourceRandom::GetKeyName(
    /* [in] */ Int32 keycode,
    /* [out] */ String *ret)
{
    VALIDATE_NOT_NULL(ret);
    MonkeySourceRandom::GetKeyName(keycode, ret);
    return NOERROR;
}

CARAPI CMonkeySourceRandom::GetKeyCode(
        /* [in] */ const String& keyName,
        /* [out] */ Int32 *keycode)
{
    VALIDATE_NOT_NULL(keycode);
    MonkeySourceRandom::GetKeyCode(keyName, keycode);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos