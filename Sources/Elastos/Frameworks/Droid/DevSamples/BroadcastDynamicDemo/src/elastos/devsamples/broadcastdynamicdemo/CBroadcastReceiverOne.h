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

#ifndef __Elastos_DevSamples_BroadcastDynamicDemo_CBroadcastReceiverOne_H__
#define __Elastos_DevSamples_BroadcastDynamicDemo_CBroadcastReceiverOne_H__

#include "_Elastos_DevSamples_BroadcastDynamicDemo_CBroadcastReceiverOne.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace DevSamples {
namespace BroadcastDynamicDemo {

CarClass(CBroadcastReceiverOne)
    , public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("BroadcastDynamicDemo::CBroadcastReceiverOne")

    CARAPI constructor();

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);
};

} // namespace BroadcastDynamicDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_BroadcastDynamicDemo_CBroadcastReceiverOne_H__
