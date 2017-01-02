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

#ifndef __CMONKEYSOURCERANDOMHELPER_H__
#define __CMONKEYSOURCERANDOMHELPER_H__

#include "_CMonkeySourceRandomHelper.h"
#include "MonkeySourceRandomHelper.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceRandomHelper), public MonkeySourceRandomHelper
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetKeyName(
        /* [in] */ Int32 keycode,
        /* [out] */ String *ret);

    CARAPI GetKeyCode(
        /* [in] */ const String& keyName,
        /* [out] */ Int32 *keycode);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCERANDOMHELPER_H__
