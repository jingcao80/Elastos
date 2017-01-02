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

#include "CMonkeySourceScript.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeySourceScript::GetNextEvent(
    /* [out] */ IMonkeyEvent **event)
{
    return MonkeySourceScript::GetNextEvent(event);
}

ECode CMonkeySourceScript::SetVerbose(
    /* [in] */ Int32 verbose)
{
    return MonkeySourceScript::SetVerbose(verbose);
}

ECode CMonkeySourceScript::Validate(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    *result = MonkeySourceScript::Validate();
    return NOERROR;
}

ECode CMonkeySourceScript::constructor(
    /* [in] */ IRandom *random,
    /* [in] */ const String& filename,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle,
    /* [in] */ Int64 profileWaitTime,
    /* [in] */ Int64 deviceSleepTime)
{
    MonkeySourceScript::Init(
        random,
        filename,
        throttle,
        randomizeThrottle,
        profileWaitTime,
        deviceSleepTime);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
