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

#ifndef  __ELASTOS_DROID_LAUNCHER2_DRAGVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_DRAGVIEW_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/view/View.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Core::Runnable;
using Elastos::Droid::View::View;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class DragView
    : public Elastos::Droid::View::View
    , public IDragView
{
private:
    class MyListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyListener(
            /* [in] */ DragView* host,
            /* [in] */ Float offsetX,
            /* [in] */ Float offsetY,
            /* [in] */ Float initialScale,
            /* [in] */ Float scale);

        //Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        DragView* mHost;
        Float mOffsetX;
        Float mOffsetY;
        Float mInitialScale;
        Float mScale;
    };

    class MyListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyListener2(
            /* [in] */ DragView* host);

        //Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        DragView* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ DragView* host);

        CARAPI Run();

    private:
        DragView* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("DragView")

    DragView();

    /**
     * Construct the drag view.
     * <p>
     * The registration point is the point inside our view that the touch events should
     * be centered upon.
     *
     * @param launcher The Launcher instance
     * @param bitmap The view that we're dragging around.  We scale it up when we draw it.
     * @param registrationX The x coordinate of the registration point.
     * @param registrationY The y coordinate of the registration point.
     */
    CARAPI constructor(
        /* [in] */ ILauncher* launcher,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 registrationX,
        /* [in] */ Int32 registrationY,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Float initialScale);

    CARAPI GetOffsetY(
        /* [out] */ Float* y);

    CARAPI GetDragRegionLeft(
        /* [out] */ Int32* left);

    CARAPI GetDragRegionTop(
        /* [out] */ Int32* top);

    CARAPI GetDragRegionWidth(
        /* [out] */ Int32* width);

    CARAPI GetDragRegionHeight(
        /* [out] */ Int32* height);

    CARAPI SetDragVisualizeOffset(
        /* [in] */ IPoint* p);

    CARAPI GetDragVisualizeOffset(
        /* [out] */ IPoint** point);

    CARAPI SetDragRegion(
        /* [in] */ IRect* r);

    CARAPI GetDragRegion(
        /* [out] */ IRect** rect);

    CARAPI GetInitialScale(
        /* [out] */ Float* scale);

    CARAPI UpdateInitialScaleToCurrentScale();

    CARAPI SetCrossFadeBitmap(
        /* [in] */ IBitmap* crossFadeBitmap);

    CARAPI CrossFade(
        /* [in] */ Int32 duration);

    CARAPI SetColor(
        /* [in] */ Int32 color);

    CARAPI HasDrawn(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetAlpha(
        /* [in] */ Float alpha);

    /**
     * Create a window containing this view and show it.
     *
     * @param windowToken obtained from v.getWindowToken() from one of your views
     * @param touchX the x coordinate the user touched in DragLayer coordinates
     * @param touchY the y coordinate the user touched in DragLayer coordinates
     */
    CARAPI Show(
        /* [in] */ Int32 touchX,
        /* [in] */ Int32 touchY);

    CARAPI CancelAnimation();

    CARAPI ResetLayoutParams();

    /**
     * Move the window containing this view.
     *
     * @param touchX the x coordinate the user touched in DragLayer coordinates
     * @param touchY the y coordinate the user touched in DragLayer coordinates
     */
    CARAPI Move(
        /* [in] */ Int32 touchX,
        /* [in] */ Int32 touchY);

    CARAPI Remove();

protected:
    //@Override
     CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
     CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    static Float sDragAlpha;

    AutoPtr<IBitmap> mBitmap;
    AutoPtr<IBitmap> mCrossFadeBitmap;
    AutoPtr<IPaint> mPaint;
    Int32 mRegistrationX;
    Int32 mRegistrationY;

    AutoPtr<IPoint> mDragVisualizeOffset;
    AutoPtr<IRect> mDragRegion;
    IDragLayer* mDragLayer;
    Boolean mHasDrawn;
    Float mCrossFadeProgress;

    AutoPtr<IValueAnimator> mAnim;
    Float mOffsetX;
    Float mOffsetY;
    Float mInitialScale;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_DRAGVIEW_H__