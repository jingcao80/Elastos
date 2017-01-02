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

#ifndef __ELASTOS_DROID_COMMANDS_AM_CINSTRUMENTATIONWATCHER_H__
#define __ELASTOS_DROID_COMMANDS_AM_CINSTRUMENTATIONWATCHER_H__

#include "_Elastos_Droid_Commands_Am_CInstrumentationWatcher.h"
#include "Am.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CInstrumentationWatcher)
    , public Am::InstrumentationWatcher
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI InstrumentationStatus(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI InstrumentationFinished(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI SetRawOutput(
        /* [in] */ Boolean rawMode);

    CARAPI WaitForFinish(
        /* [out] */ Boolean* result);
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_AM_CINSTRUMENTATIONWATCHER_H__
