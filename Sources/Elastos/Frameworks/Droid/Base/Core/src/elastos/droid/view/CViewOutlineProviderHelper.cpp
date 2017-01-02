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

#include "elastos/droid/view/CViewOutlineProviderHelper.h"
#include "elastos/droid/view/ViewOutlineProvider.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CViewOutlineProviderHelper);
CAR_INTERFACE_IMPL(CViewOutlineProviderHelper, Singleton, IViewOutlineProviderHelper);
ECode CViewOutlineProviderHelper::GetBACKGROUND(
    /* [out] */ IViewOutlineProvider** background)
{
    VALIDATE_NOT_NULL(background);
    *background = ViewOutlineProvider::BACKGROUND;
    REFCOUNT_ADD(*background);
    return NOERROR;
}

ECode CViewOutlineProviderHelper::GetBOUNDS(
    /* [out] */ IViewOutlineProvider** bounds)
{
    VALIDATE_NOT_NULL(bounds);
    *bounds = ViewOutlineProvider::BOUNDS;
    REFCOUNT_ADD(*bounds);
    return NOERROR;
}

ECode CViewOutlineProviderHelper::GetPADDEDBOUNDS(
    /* [out] */ IViewOutlineProvider** bounds)
{
    VALIDATE_NOT_NULL(bounds);
    *bounds = ViewOutlineProvider::PADDED_BOUNDS;
    REFCOUNT_ADD(*bounds);
    return NOERROR;
}

}// namespace View
}// namespace Droid
}// namespace Elastos
