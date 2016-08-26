#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CLINEARCOLORBAR_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CLINEARCOLORBAR_H__

#include "_Elastos_Droid_Settings_Applications_CLinearColorBar.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CarClass(CLinearColorBar)
    , public LinearLayout
{
public:
    CAR_OBJECT_DECL()

    CLinearColorBar();

    ~CLinearColorBar();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI SetOnRegionTappedListener(
        /* [in] */ ILinearColorBarOnRegionTappedListener* listener);

    virtual CARAPI SetColoredRegions(
        /* [in] */ Int32 regions);

    virtual CARAPI SetRatios(
        /* [in] */ Float red,
        /* [in] */ Float yellow,
        /* [in] */ Float green);

    virtual CARAPI SetColors(
        /* [in] */ Int32 red,
        /* [in] */ Int32 yellow,
        /* [in] */ Int32 green);

    virtual CARAPI SetShowIndicator(
        /* [in] */ Boolean showIndicator);

    virtual CARAPI SetShowingGreen(
        /* [in] */ Boolean showingGreen);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI PerformClick(
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    //@Override
    CARAPI_(void) DispatchSetPressed(
        /* [in] */ Boolean pressed);

    //@Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) UpdateIndicator();

    CARAPI_(Int32) PickColor(
        /* [in] */ Int32 color,
        /* [in] */ Int32 region);

public:
    static const Int32 REGION_RED;// = 1<<0;
    static const Int32 REGION_YELLOW;// = 1<<1;
    static const Int32 REGION_GREEN;// = 1<<2;
    static const Int32 REGION_ALL;// = REGION_RED | REGION_YELLOW | REGION_GREEN;

protected:
    static const Int32 LEFT_COLOR;// = 0xff009688;
    static const Int32 MIDDLE_COLOR;// = 0xff009688;
    static const Int32 RIGHT_COLOR;// = 0xffced7db;
    static const Int32 GRAY_COLOR;// = 0xff555555;
    static const Int32 WHITE_COLOR;// = 0xffffffff;

    AutoPtr<IRect> mRect;
    AutoPtr<IPaint> mPaint;

    Int32 mLastInterestingLeft, mLastInterestingRight;
    Int32 mLineWidth;

    Int32 mLastLeftDiv, mLastRightDiv;
    Int32 mLastRegion;

    AutoPtr<IPath> mColorPath;
    AutoPtr<IPath> mEdgePath;
    AutoPtr<IPaint> mColorGradientPaint;
    AutoPtr<IPaint> mEdgeGradientPaint;

private:
    Float mRedRatio;
    Float mYellowRatio;
    Float mGreenRatio;

    Int32 mLeftColor;
    Int32 mMiddleColor;
    Int32 mRightColor;

    Boolean mShowIndicator;
    Boolean mShowingGreen;

    AutoPtr<ILinearColorBarOnRegionTappedListener> mOnRegionTappedListener;
    Int32 mColoredRegions;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CLINEARCOLORBAR_H__