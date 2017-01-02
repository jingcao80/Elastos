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

#include "elastos/droid/transition/CTransitionInflaterHelper.h"
#include "elastos/droid/transition/TransitionInflater.h"

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CTransitionInflaterHelper::
//===============================================================

CAR_SINGLETON_IMPL(CTransitionInflaterHelper)

CAR_INTERFACE_IMPL(CTransitionInflaterHelper, Singleton, ITransitionInflaterHelper)

ECode CTransitionInflaterHelper::From(
    /* [in] */ IContext* context,
    /* [out] */ ITransitionInflater** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITransitionInflater> p = TransitionInflater::From(context);
    *result = p.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
