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

#ifndef __MONKEYUTILS_H__
#define __MONKEYUTILS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Utility::IDate;
using Elastos::Text::ISimpleDateFormat;
namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyUtils {
public:
    static void ToCalendarTime(
            /* [in] */ Int64 time,
            /* [out] */ String *formatedTime);

    static AutoPtr<IDate> InitStatics();

private:
    static Object mLockLock;
    MonkeyUtils();
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYUTILS_H__