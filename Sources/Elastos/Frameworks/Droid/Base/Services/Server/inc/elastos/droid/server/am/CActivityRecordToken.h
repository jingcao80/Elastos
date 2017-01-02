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

#ifndef __ELASTOS_DROID_SERVER_AM_CACTIVITYRECORDTOKEN_H__
#define __ELASTOS_DROID_SERVER_AM_CACTIVITYRECORDTOKEN_H__

#include "_Elastos_Droid_Server_Am_CActivityRecordToken.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IApplicationToken;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityRecord;

CarClass(CActivityRecordToken)
    , public Object
    , public IApplicationToken
    , public IBinder
{
public:
    CActivityRecordToken();

    ~CActivityRecordToken();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IActivityRecord* activity);

    CARAPI WindowsDrawn();

    CARAPI WindowsVisible();

    CARAPI WindowsGone();

    CARAPI KeyDispatchingTimedOut(
        /* [in] */ const String& reason,
        /* [out] */ Boolean* result);

    CARAPI GetKeyDispatchingTimeout(
        /* [out] */ Int64* timeout);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(AutoPtr<ActivityRecord>) GetActivityRecord();

public:
    AutoPtr<IWeakReference> mWeakActivity; // WeakReference<ActivityRecord>
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_CACTIVITYRECORDTOKEN_H__
