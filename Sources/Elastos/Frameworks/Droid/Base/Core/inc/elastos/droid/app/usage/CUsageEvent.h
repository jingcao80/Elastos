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

#ifndef __ELASTOS_DROID_APP_USAGE_CUSAGE_EVENT_H__
#define __ELASTOS_DROID_APP_USAGE_CUSAGE_EVENT_H__

#include "_Elastos_Droid_App_Usage_CUsageEvent.h"
#include "elastos/droid/app/usage/UsageEvents.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Usage {

CarClass(CUsageEvent)
    , public UsageEvent
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usage
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_USAGE_CUSAGE_EVENT_H__
