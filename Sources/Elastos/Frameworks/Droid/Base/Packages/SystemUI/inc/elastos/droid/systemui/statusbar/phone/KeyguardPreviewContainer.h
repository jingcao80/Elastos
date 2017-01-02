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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/graphics/drawable/Drawable.h>
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A view group which contains the preview of phone/camera and draws a black bar at the bottom as
 * the fake navigation bar.
 */
class KeyguardPreviewContainer
    : public FrameLayout
    , public IKeyguardPreviewContainer
{
public:
    CAR_INTERFACE_DECL()

    KeyguardPreviewContainer();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI OnApplyWindowInsets(
        /* [in] */ IWindowInsets* insets,
        /* [out] */ IWindowInsets** sets);

private:
    AutoPtr<IDrawable> mBlackBarDrawable;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_ICONMERGER_H__
