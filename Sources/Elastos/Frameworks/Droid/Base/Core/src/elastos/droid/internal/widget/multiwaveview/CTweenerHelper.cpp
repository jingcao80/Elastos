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

#include "elastos/droid/internal/widget/multiwaveview/CTweenerHelper.h"
#include "elastos/droid/internal/widget/multiwaveview/Tweener.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

//=====================================================================
//               CTweenerHelper::
//=====================================================================
CAR_INTERFACE_IMPL(CTweenerHelper, Singleton, ITweenerHelper)

CAR_SINGLETON_IMPL(CTweenerHelper)

ECode CTweenerHelper::To(
    /* [in] */ IInterface* object,
    /* [in] */ Int64 duration,
    /* [in] */ ArrayOf<IInterface*>* vars,
    /* [out] */ ITweener** result)
{
    VALIDATE_NOT_NULL(result)

    return Tweener::To(object, duration, vars, result);
}

ECode CTweenerHelper::Reset()
{
    return Tweener::Reset();
}

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
