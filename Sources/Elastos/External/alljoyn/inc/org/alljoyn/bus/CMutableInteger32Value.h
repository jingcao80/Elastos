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

#ifndef __ORG_ALLJOYN_BUS_CMUTABLEINTEGER32VALUE_H__
#define __ORG_ALLJOYN_BUS_CMUTABLEINTEGER32VALUE_H__

#include "_Org_Alljoyn_Bus_CMutableInteger32Value.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CMutableInteger32Value)
    , public Object
    , public IMutableInteger32Value
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 v);

    CARAPI GetValue(
        /* [out] */ Int32* value);

    CARAPI SetValue(
        /* [in] */ Int32 value);

private:
    Int32 mValue;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CMUTABLEINTEGER32VALUE_H__
