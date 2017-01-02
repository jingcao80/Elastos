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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CLayoutDirections.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CLayoutDirections, Object, ILayoutDirections)

CAR_OBJECT_IMPL(CLayoutDirections)

ECode CLayoutDirections::constructor(
    /* [in] */ ArrayOf<Int32>* directions)
{
    mDirections = directions;
    return NOERROR;
}

ECode CLayoutDirections::SetDirections(
    /* [in] */ ArrayOf<Int32>* directions)
{
    mDirections = directions;
    return NOERROR;
}

ECode CLayoutDirections::GetDirections(
    /* [out, callee] */ ArrayOf<Int32>** directions)
{
    VALIDATE_NOT_NULL(directions);
    *directions = mDirections;
    REFCOUNT_ADD(*directions);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
