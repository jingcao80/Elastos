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

#include "CMonkeySourceRandomScript.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeySourceRandomScript)

ECode CMonkeySourceRandomScript::GetNextEvent(
    /* [out] */IMonkeyEvent **event)
{
    VALIDATE_NOT_NULL(event);
    return MonkeySourceRandomScript::GetNextEvent(event);
}

ECode CMonkeySourceRandomScript::SetVerbose(
    /* [in] */ Int32 verbose)
{
    return MonkeySourceRandomScript::SetVerbose(verbose);
}

ECode CMonkeySourceRandomScript::Validate(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    return MonkeySourceRandomScript::Validate(result);
}

ECode CMonkeySourceRandomScript::constructor(
    /* [in] */ const String& setupFileName,
    /* [in] */ ArrayOf<String> *scriptFileNames,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle,
    /* [in] */ IRandom *random,
    /* [in] */ Int64 profileWaitTime,
    /* [in] */ Int64 deviceSleepTime,
    /* [in] */ Boolean randomizeScript)
{
    return MonkeySourceRandomScript::Init(
        setupFileName,
        scriptFileNames,
        throttle,
        randomizeThrottle,
        random,
        profileWaitTime,
        deviceSleepTime,
        randomizeScript);
}

ECode CMonkeySourceRandomScript::constructor(
    /* [in] */ ArrayOf<String> *scriptFileNames,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle,
    /* [in] */ IRandom *random,
    /* [in] */ Int64 profileWaitTime,
    /* [in] */ Int64 deviceSleepTime,
    /* [in] */ Boolean randomizeScript)
{
    return MonkeySourceRandomScript::Init(
        scriptFileNames,
        throttle,
        randomizeThrottle,
        random,
        profileWaitTime,
        deviceSleepTime,
        randomizeScript);
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
