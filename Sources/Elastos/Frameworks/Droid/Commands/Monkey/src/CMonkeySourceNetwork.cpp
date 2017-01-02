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

#include "CMonkeySourceNetwork.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeySourceNetwork)

ECode CMonkeySourceNetwork::GetNextEvent(
    /* [out] */ IMonkeyEvent **event)
{
    // TODO: Add your code here
    VALIDATE_NOT_NULL(event)
    MonkeySourceNetwork::GetNextEvent(event);
    return NOERROR;
}

ECode CMonkeySourceNetwork::SetVerbose(
    /* [in] */ Int32 verbose)
{
    // TODO: Add your code here
    MonkeySourceNetwork::SetVerbose(verbose);
    return NOERROR;
}

ECode CMonkeySourceNetwork::Validate(
    /* [out] */ Boolean *result)
{
    // TODO: Add your code here
    VALIDATE_NOT_NULL(result)
    MonkeySourceNetwork::Validate(result);
    return NOERROR;
}

ECode CMonkeySourceNetwork::constructor(
    /* [in] */ Int32 port)
{
    // TODO: Add your code here
    MonkeySourceNetwork::Init(port);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
