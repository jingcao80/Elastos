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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CTableMaskFilterHelper.h"
#include "elastos/droid/graphics/CTableMaskFilter.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CTableMaskFilterHelper);
CAR_INTERFACE_IMPL(CTableMaskFilterHelper, Singleton, ITableMaskFilterHelper);
ECode CTableMaskFilterHelper::CreateClipTable(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max,
    /* [out] */ ITableMaskFilter** filter)
{
    VALIDATE_NOT_NULL(filter);

    return CTableMaskFilter::CreateClipTable(min, max, filter);
}

ECode CTableMaskFilterHelper::CreateGammaTable(
    /* [in] */ Float gamma,
    /* [out] */ ITableMaskFilter** filter)
{
    VALIDATE_NOT_NULL(filter);

    return CTableMaskFilter::CreateGammaTable(gamma, filter);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
