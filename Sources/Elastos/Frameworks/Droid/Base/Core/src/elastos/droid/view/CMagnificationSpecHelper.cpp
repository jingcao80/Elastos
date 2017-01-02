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

#include "elastos/droid/view/CMagnificationSpec.h"
#include "elastos/droid/view/CMagnificationSpecHelper.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CMagnificationSpecHelper, Singleton, IMagnificationSpecHelper)

CAR_SINGLETON_IMPL(CMagnificationSpecHelper)

ECode CMagnificationSpecHelper::Obtain(
    /* [in] */ IMagnificationSpec* other,
    /* [out] */ IMagnificationSpec** spec)
{
    VALIDATE_NOT_NULL(spec)
    AutoPtr<IMagnificationSpec> temp = CMagnificationSpec::Obtain(other);
    *spec = temp;
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

ECode CMagnificationSpecHelper::Obtain(
    /* [out] */ IMagnificationSpec** spec)
{
    VALIDATE_NOT_NULL(spec)
    AutoPtr<IMagnificationSpec> temp = CMagnificationSpec::Obtain();
    *spec = temp;
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
