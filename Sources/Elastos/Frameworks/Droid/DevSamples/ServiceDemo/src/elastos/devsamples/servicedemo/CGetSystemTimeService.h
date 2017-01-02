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

#ifndef __Elastos_DevSamples_ServiceDemo_CGetSystemTimeService_H__
#define __Elastos_DevSamples_ServiceDemo_CGetSystemTimeService_H__

#include "_Elastos_DevSamples_ServiceDemo_CGetSystemTimeService.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace DevSamples {
namespace ServiceDemo {

CarClass(CGetSystemTimeService)
    , public Object
    , public IGetSystemTimeService
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetSystemTime(
        /* [out] */ String* timeStr);

    TO_STRING_IMPL("CGetSystemTimeService")
};

} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ServiceDemo_CGetSystemTimeService_H__
