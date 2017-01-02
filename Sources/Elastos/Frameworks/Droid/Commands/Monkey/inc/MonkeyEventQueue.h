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

#ifndef __MONKEYEVENTQUEUE_H_
#define __MONKEYEVENTQUEUE_H_

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Commands.Monkey_server.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::IRandom;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyEventQueue
{
protected:
    CARAPI IsEmpty(
        /* [out] */ Boolean *isEmpty);

    CARAPI RemoveFirst();

    CARAPI AddLast(
        /* [in] */ IMonkeyEvent *e);

    CARAPI GetFirst(
        /* [out] */ IMonkeyEvent **monkeyEvent);

    CARAPI Init(
        /* [in] */ IRandom *random,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle);

private:
    AutoPtr<IRandom> mRandom;
    Int64 mThrottle;
    Boolean mRandomizeThrottle;
    List<AutoPtr<IMonkeyEvent> > mList;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYEVENTQUEUE_H_
