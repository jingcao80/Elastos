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

#include "CIntentReceiver.h"

using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CAR_INTERFACE_IMPL_2(CIntentReceiver, Object, IIntentReceiver, IBinder);

CAR_OBJECT_IMPL(CIntentReceiver)

ECode CIntentReceiver::constructor()
{
    return NOERROR;
}

ECode CIntentReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    return Am::IntentReceiver::PerformReceive(
            intent, resultCode,  data,  extras, ordered, sticky, sendingUser);
}

ECode CIntentReceiver::ToString(
    /* [out] */ String* string)
{
    return Object::ToString(string);
}

ECode CIntentReceiver::WaitForFinish()
{
    return Am::IntentReceiver::WaitForFinish();
}

}
}
}
}

