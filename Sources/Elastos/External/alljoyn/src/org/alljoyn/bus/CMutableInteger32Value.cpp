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

#include "org/alljoyn/bus/CMutableInteger32Value.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(CMutableInteger32Value, Object, IMutableInteger32Value)

CAR_OBJECT_IMPL(CMutableInteger32Value)

ECode CMutableInteger32Value::constructor()
{
    mValue = 0;
    return NOERROR;
}

ECode CMutableInteger32Value::constructor(
    /* [in] */ Int32 v)
{
    mValue = v;
    return NOERROR;
}

ECode CMutableInteger32Value::GetValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CMutableInteger32Value::SetValue(
    /* [in] */ Int32 value)
{
    mValue = value;
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
