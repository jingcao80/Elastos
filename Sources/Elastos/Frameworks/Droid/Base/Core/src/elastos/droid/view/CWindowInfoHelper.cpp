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

#include "elastos/droid/view/CWindowInfoHelper.h"
#include "elastos/droid/view/CWindowInfo.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CWindowInfoHelper);
CAR_INTERFACE_IMPL(CWindowInfoHelper, Singleton, IWindowInfoHelper);
ECode CWindowInfoHelper::Obtain(
    /* [out] */ IWindowInfo** info)
{
    return CWindowInfo::Obtain(info);
}

ECode CWindowInfoHelper::Obtain(
    /* [in] */ IWindowInfo *other,
    /* [out] */ IWindowInfo** info)
{
    return CWindowInfo::Obtain(other, info);
}

} // namespace View
} // namespace Droid
} // namespace Elastos