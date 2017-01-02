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

#ifndef __ELASTOS_DROID_COMMANDS_AM_CMYACTIVITYCONTROLLER_H__
#define __ELASTOS_DROID_COMMANDS_AM_CMYACTIVITYCONTROLLER_H__

#include "_Elastos_Droid_Commands_Am_CMyActivityController.h"
#include "Am.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CMyActivityController)
    , public Am::MyActivityController
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIActivityManager* am,
        /* [in] */ const String& gdbPort);

    CARAPI ActivityResuming(
            /* [in] */ const String& pkg,
            /* [out] */ Boolean* result);

    CARAPI ActivityStarting(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* result);

    CARAPI AppCrashed(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& shortMsg,
        /* [in] */ const String& longMsg,
        /* [in] */ Int64 timeMillis,
        /* [in] */ const String& stackTrace,
        /* [out] */ Boolean* result);

    CARAPI AppEarlyNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& annotation,
        /* [out] */ Int32* result);

    CARAPI AppNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& processStats,
        /* [out] */ Int32* result);

    CARAPI SystemNotResponding(
        /* [in] */ const String& message,
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI Run();
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_AM_CMYACTIVITYCONTROLLER_H__
