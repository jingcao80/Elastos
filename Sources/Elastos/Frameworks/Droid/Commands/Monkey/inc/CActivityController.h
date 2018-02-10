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

#ifndef __CACTIVITYCONTROLLER_H__
#define __CACTIVITYCONTROLLER_H__

#include "_CActivityController.h"
#include "Monkey.h"

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CActivityController)
{
public:
    CAR_OBJECT_DECL()

    CARAPI ActivityStarting(
        /* [in] */ IIntent * intent,
        /* [in] */ const String& pkg,
        /* [out] */ Boolean * res);

    CARAPI ActivityResuming(
        /* [in] */ const String& pkg,
        /* [out] */ Boolean * res);

    CARAPI AppCrashed(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& shortMsg,
        /* [in] */ const String& longMsg,
        /* [in] */ Int64 timeMillis,
        /* [in] */ const String& stackTrace,
        /* [out] */ Boolean * pRes);

    CARAPI AppEarlyNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& annotation,
        /* [out] */ Int32 * res);

    CARAPI AppNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& processStats,
        /* [out] */ Int32 * res);

    CARAPI constructor(
        /* [in] */ HANDLE monkey);

private:
    Monkey* mHost;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYCONTROLLER_H__
