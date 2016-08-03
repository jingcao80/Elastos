
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
    {
    public:
        WaveManager();

        CARAPI_(void) SetRadius(
            /* [in] */ Float r);

        CARAPI_(Float) GetRadius();

        CARAPI_(void) SetAlpha(
            /* [in] */ Float a);

        CARAPI_(Float) GetAlpha();

    public:
        Float mRadius;
        Float mAlpha;
    };

    class GlowManager
        : public Object
    {
    public:
        GlowManager();

        CARAPI_(void) SetX(
            /* [in] */ Float x1);

        CARAPI_(Float) GetX();

        CARAPI_(void) SetY(
            /* [in] */ Float y1);

        CARAPI_(Float) GetY();

        CARAPI_(void) SetAlpha(
            /* [in] */ Float a);

        CARAPI_(Float) GetAlpha();

        CARAPI_(void) SetRadius(
            /* [in] */ Float r);

        CARAPI_(Float) GetRadius();

    public:
        Float mX;
        Float mY;
        Float mRadius;
        Float mAlpha;
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

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

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
