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

#include "MonkeyCommandReturn.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyCommandReturn::MonkeyCommandReturn()
    : mMessage()
    , mSuccess(FALSE)
{
}

ECode MonkeyCommandReturn::HasMessage(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = !const_cast<String*>(&mMessage)->IsNull();
    return NOERROR;
}

ECode MonkeyCommandReturn::GetMessage(
    /* [out] */ String *message)
{
    VALIDATE_NOT_NULL(message)
    *message = *const_cast<String*>(&mMessage);
    return NOERROR;
}

ECode MonkeyCommandReturn::WasSuccessful(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSuccess;
    return NOERROR;
}

ECode MonkeyCommandReturn::Init(
    /* [in] */ Boolean success,
    /* [in] */ const String& message)
{
    Boolean* suc = const_cast<Boolean*>(&mSuccess);
    *suc = success;
    String* msg = const_cast<String*>(&mMessage);
    *msg = message;
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
