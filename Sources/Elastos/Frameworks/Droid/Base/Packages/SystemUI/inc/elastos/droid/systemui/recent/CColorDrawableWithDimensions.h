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

#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_COLORDRAWABLEWITHDIMENSIONS_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_COLORDRAWABLEWITHDIMENSIONS_H__

#include "_Elastos_Droid_SystemUI_Recent_CColorDrawableWithDimensions.h"
#include "elastos/droid/graphics/drawable/ColorDrawable.h"

using Elastos::Droid::Graphics::Drawable::ColorDrawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CarClass(CColorDrawableWithDimensions)
    , public ColorDrawable
{
public:
    CAR_OBJECT_DECL()

    CColorDrawableWithDimensions();

    CARAPI constructor(
        /* [in] */ Int32 color,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // @Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    // @Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

private:
    Int32 mWidth;
    Int32 mHeight;
};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_COLORDRAWABLEWITHDIMENSIONS_H__