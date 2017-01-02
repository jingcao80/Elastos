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

#include "CBroadcastReceiverTwo.h"
#include "CActivityOne.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace BroadcastStaticDemo {

static const String TAG("BroadcastStaticDemo::CBroadcastReceiverTwo");

CAR_OBJECT_IMPL(CBroadcastReceiverTwo)

ECode CBroadcastReceiverTwo::constructor()
{
    return NOERROR;
}

ECode CBroadcastReceiverTwo::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String msg;
    intent->GetStringExtra(String("msg"), &msg);
    Logger::I(TAG, "Get Broadcast: %s, msg: %s", TO_CSTR(intent), msg.string());

    if (CActivityOne::TEST_ORDERED_BROADCAST) {
        AutoPtr<IBundle> bundle;
        CBundle::New((IBundle**)&bundle);
        bundle->PutString(String("msg"), msg + "@BroadcastStaticDemo::ReceiverTwo");
        return SetResultExtras(bundle);
    }

    return NOERROR;
}

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Elastos
