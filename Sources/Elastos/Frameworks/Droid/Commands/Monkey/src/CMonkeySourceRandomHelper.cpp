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

#include "CMonkeySourceRandomHelper.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {


ECode CMonkeySourceRandomHelper::GetKeyName(
    /* [in] */ Int32 keycode,
    /* [out] */ String *ret)
{
    VALIDATE_NOT_NULL(ret);
    MonkeySourceRandomHelper::GetKeyName(keycode, ret);
    return NOERROR;
}

ECode CMonkeySourceRandomHelper::GetKeyCode(
    /* [in] */ const String& keyName,
    /* [out] */ Int32 *keycode)
{
    VALIDATE_NOT_NULL(keycode);
    MonkeySourceRandomHelper::GetKeyCode(keyName, keycode);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
