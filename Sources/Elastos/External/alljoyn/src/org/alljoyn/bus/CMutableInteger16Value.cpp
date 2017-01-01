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

#include "org/alljoyn/bus/CMutableInteger16Value.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(CMutableInteger16Value, Object, IMutableInteger16Value)

CAR_OBJECT_IMPL(CMutableInteger16Value)

ECode CMutableInteger16Value::constructor()
{
    mValue = 0;
    return NOERROR;
}

ECode CMutableInteger16Value::constructor(
    /* [in] */ Int16 v)
{
    mValue = v;
    return NOERROR;
}

ECode CMutableInteger16Value::GetValue(
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CMutableInteger16Value::SetValue(
    /* [in] */ Int16 value)
{
    mValue = value;
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
