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

#include "elastos/droid/view/CPointerIcon.h"
#include "elastos/droid/view/CPointerIconHelper.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CPointerIconHelper)

CAR_INTERFACE_IMPL(CPointerIconHelper, Singleton, IPointerIconHelper)


ECode CPointerIconHelper::GetNullIcon(
    /* [out] */ IPointerIcon** pointerIcon)
{
    return CPointerIcon::GetNullIcon(pointerIcon);
}

ECode CPointerIconHelper::GetDefaultIcon(
    /* [in] */ IContext* context,
    /* [out] */ IPointerIcon** pointerIcon)
{
    return CPointerIcon::GetDefaultIcon(context, pointerIcon);
}

ECode CPointerIconHelper::GetSystemIcon(
    /* [in] */ IContext* context,
    /* [in] */ Int32 style,
    /* [out] */ IPointerIcon** pointerIcon)
{
    return CPointerIcon::GetSystemIcon(context, style, pointerIcon);
}

ECode CPointerIconHelper::CreateCustomIcon(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Float hotSpotX,
    /* [in] */ Float hotSpotY,
    /* [out] */ IPointerIcon** pointerIcon)
{
    return CPointerIcon::CreateCustomIcon(bitmap, hotSpotX, hotSpotY, pointerIcon);
}

ECode CPointerIconHelper::LoadCustomIcon(
    /* [in] */ IResources* resources,
    /* [in] */ Int32 resourceId,
    /* [out] */ IPointerIcon** pointerIcon)
{
    return CPointerIcon::LoadCustomIcon(resources, resourceId, pointerIcon);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
