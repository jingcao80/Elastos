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

#ifndef __ELASTOS_DROID_COMMANDS_AM_CINTENTRECEIVER_H__
#define __ELASTOS_DROID_COMMANDS_AM_CINTENTRECEIVER_H__

#include "_Elastos_Droid_Commands_Am_CIntentReceiver.h"
#include "Am.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Commands::Am::Am;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CIntentReceiver)
    , public Am::IntentReceiver
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI PerformReceive(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI WaitForFinish();
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_AM_CINTENTRECEIVER_H__
