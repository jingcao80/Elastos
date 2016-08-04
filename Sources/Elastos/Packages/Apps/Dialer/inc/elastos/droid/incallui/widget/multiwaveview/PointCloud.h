
#ifndef __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_POINTCLOUD_H__
#define __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_POINTCLOUD_H__

#include "Elastos.Droid.Graphics.h"
#include "_Elastos.Droid.Dialer.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

class PointCloud
   : public Object
{
public:
    class WaveManager
        : public Object
        , public IWaveManager
    {
    public:
        WaveManager();

        CAR_INTERFACE_DECL()

        CARAPI SetRadius(
            /* [in] */ Float r);

        CARAPI GetRadius(
            /* [out] */ Float* r);

        CARAPI SetAlpha(
            /* [in] */ Float a);

        CARAPI GetAlpha(
            /* [out] */ Float* a);

    public:
        Float mRadius;
        Float mWidth;
        Float mAlpha;
    };

    class GlowManager
        : public Object
        , public IGlowManager
    {
    public:
        GlowManager();

        CAR_INTERFACE_DECL()

        CARAPI SetX(
            /* [in] */ Float x1);

        CARAPI GetX(
            /* [out] */ Float* x1);

        CARAPI SetY(
            /* [in] */ Float y1);

        CARAPI GetY(
            /* [out] */ Float* y1);

        CARAPI SetAlpha(
            /* [in] */ Float a);

        CARAPI GetAlpha(
            /* [out] */ Float* a);

        CARAPI SetRadius(
            /* [in] */ Float r);

        CARAPI GetRadius(
            /* [out] */ Float* r);

    public:
        Float mX;
        Float mY;
        Float mRadius;
        Float mAlpha;
    };

    class Point
        : public Object
    {
    public:
        Point(
            /* [in] */ Float x2,
            /* [in] */ Float y2,
            /* [in] */ Float r);

    public:
        Float mX;
        Float mY;
        Float mRadius;
    };

public:
    PointCloud(
        /* [in] */ IDrawable* drawable);

    CARAPI_(void) SetCenter(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) MakePointCloud(
        /* [in] */ Float innerRadius,
        /* [in] */ Float outerRadius);

    CARAPI_(void) SetScale(
        /* [in] */ Float scale);

    CARAPI_(Float) GetScale();

    CARAPI_(Int32) GetAlphaForPoint(
        /* [in] */ Point* point);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

private:
    static CARAPI_(Float) Hypot(
        /* [in] */ Float x,
        /* [in] */ Float y);

    static CARAPI_(Float) Max(
        /* [in] */ Float a,
        /* [in] */ Float b);

    CARAPI_(Float) Interp(
        /* [in] */ Float min,
        /* [in] */ Float max,
        /* [in] */ Float f);

public:
    AutoPtr<WaveManager> mWaveManager;
    AutoPtr<GlowManager> mGlowManager;

private:
    static const Float MIN_POINT_SIZE;
    static const Float MAX_POINT_SIZE;
    static const Int32 INNER_POINTS;
    static const String TAG;
    AutoPtr<IArrayList> mPointCloud;
    AutoPtr<IDrawable> mDrawable;
    Float mCenterX;
    Float mCenterY;
    AutoPtr<IPaint> mPaint;
    Float mScale;
    static const Float PI;

    // These allow us to have multiple concurrent animations.
    Float mOuterRadius;
};

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_POINTCLOUD_H__
