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

#include "elastos/droid/internal/view/CActionBarPolicyHelper.h"
#include "elastos/droid/internal/view/ActionBarPolicy.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CAR_INTERFACE_IMPL(CActionBarPolicyHelper, Singleton, IActionBarPolicyHelper)

CAR_SINGLETON_IMPL(CActionBarPolicyHelper)

ECode CActionBarPolicyHelper::Get(
    /* [in] */ IContext* context,
    /* [out] */ IActionBarPolicy** policy)
{
    VALIDATE_NOT_NULL(policy)
    AutoPtr<IActionBarPolicy> temp = ActionBarPolicy::Get(context);
    *policy = temp;
    REFCOUNT_ADD(*policy)
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
