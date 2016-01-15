
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_POINTCLOUD_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_POINTCLOUD_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Graphics.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Drawable::IDrawable;

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

class PointCloud
    : public Object
    , public IPointCloud
{
public:
    class WaveManager
        : public Object
        , public IWaveManager
    {
    public:
        CAR_INTERFACE_DECL()

        WaveManager();

        CARAPI SetRadius(
            /* [in] */ Float r);

        CARAPI GetRadius(
            /* [out] */ Float* result);

        CARAPI SetAlpha(
            /* [in] */ Float a);

        CARAPI GetAlpha(
            /* [out] */ Float* result);

    public:
        Float mRadius;
        Float mAlpha;
    };

    class GlowManager
        : public Object
        , public IGlowManager
    {
    public:
        CAR_INTERFACE_DECL()

        GlowManager();

        CARAPI SetX(
            /* [in] */ Float x1);

        CARAPI GetX(
            /* [out] */ Float* result);

        CARAPI SetY(
            /* [in] */ Float y1);

        CARAPI GetY(
            /* [out] */ Float* result);

        CARAPI SetAlpha(
            /* [in] */ Float a);

        CARAPI GetAlpha(
            /* [out] */ Float* result);

        CARAPI SetRadius(
            /* [in] */ Float r);

        CARAPI GetRadius(
            /* [out] */ Float* result);

    public:
        Float mX;
        Float mY;
        Float mRadius;
        Float mAlpha;
    };

    class Point
        : public Object
        , public IPointCloudPoint
    {
    public:
        CAR_INTERFACE_DECL()

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
    CAR_INTERFACE_DECL()

    PointCloud();

    CARAPI constructor(
        /* [in] */ IDrawable* drawable);

    CARAPI SetCenter(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI MakePointCloud(
        /* [in] */ Float innerRadius,
        /* [in] */ Float outerRadius);

    CARAPI SetScale(
        /* [in] */ Float scale);

    CARAPI GetScale(
        /* [out] */ Float* result);

    CARAPI GetAlphaForPoint(
        /* [in] */ IPointCloudPoint* point,
        /* [out] */ Int32* result);

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
    AutoPtr<IWaveManager> mWaveManager;
    AutoPtr<IGlowManager> mGlowManager;
    Float mOuterRadius;
};

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_POINTCLOUD_H__
