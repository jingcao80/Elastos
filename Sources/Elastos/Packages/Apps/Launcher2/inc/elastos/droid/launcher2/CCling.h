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

#ifndef  __ELASTOS_DROID_LAUNCHER2_CCLING_H__
#define  __ELASTOS_DROID_LAUNCHER2_CCLING_H__

#include "_Elastos_Droid_Launcher2_CCling.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CCling)
    , public FrameLayout
    , public ICling
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CCling();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI CCling_Init(
        /* [in] */ ILauncher* l,
        /* [in] */ ArrayOf<Int32>* positionData);

    CARAPI Cleanup();

    CARAPI GetDrawIdentifier(
        /* [out] */ String* str);

    //@Override
    CARAPI FocusSearch(
        /* [in] */ Int32 direction,
        /* [out] */ IView** res);

    //@Override
    CARAPI FocusSearch(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out] */ IView** res);

    //@Override
    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

protected:
    //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetPunchThroughPositions();

private:
    static String WORKSPACE_PORTRAIT;
    static String WORKSPACE_LANDSCAPE;
    static String WORKSPACE_LARGE;
    static String WORKSPACE_CUSTOM;

    static String ALLAPPS_PORTRAIT;
    static String ALLAPPS_LANDSCAPE;
    static String ALLAPPS_LARGE;

    static String FOLDER_PORTRAIT;
    static String FOLDER_LANDSCAPE;
    static String FOLDER_LARGE;

    ILauncher* mLauncher;
    Boolean mIsInitialized;
    String mDrawIdentifier;
    AutoPtr<IDrawable> mBackground;
    AutoPtr<IDrawable> mPunchThroughGraphic;
    AutoPtr<IDrawable> mHandTouchGraphic;
    Int32 mPunchThroughGraphicCenterRadius;
    Int32 mAppIconSize;
    Int32 mButtonBarHeight;
    Float mRevealRadius;
    AutoPtr<ArrayOf<Int32> > mPositionData;

    AutoPtr<IPaint> mErasePaint;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CCLING_H__