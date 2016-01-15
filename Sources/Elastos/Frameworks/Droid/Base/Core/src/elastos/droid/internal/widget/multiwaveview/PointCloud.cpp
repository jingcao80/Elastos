
#include "elastos/droid/internal/widget/multiwaveview/PointCloud.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CColor.h"
#include "elastos/droid/utility/FloatMath.h"

#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Utility::FloatMath;

using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

const Float PointCloud::MIN_POINT_SIZE = 2.0f;
const Float PointCloud::MAX_POINT_SIZE = 4.0f;
const Int32 PointCloud::INNER_POINTS = 8;
const String PointCloud::TAG(String("PointCloud"));
const Float PointCloud::PI = (Float) Elastos::Core::Math::PI;

//=====================================================================
//               PointCloud::WaveManager::
//=====================================================================
CAR_INTERFACE_IMPL(PointCloud::WaveManager, Object, IWaveManager)

PointCloud::WaveManager::WaveManager()
    : mRadius(50.0f)
    , mAlpha(0.0f)
{}

ECode PointCloud::WaveManager::SetRadius(
    /* [in] */ Float r)
{
    mRadius = r;
    return NOERROR;
}

ECode PointCloud::WaveManager::GetRadius(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRadius;
    return NOERROR;
}

ECode PointCloud::WaveManager::SetAlpha(
    /* [in] */ Float a)
{
    mAlpha = a;
    return NOERROR;
}

ECode PointCloud::WaveManager::GetAlpha(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAlpha;
    return NOERROR;
}

//=====================================================================
//               PointCloud::GlowManager::
//=====================================================================
CAR_INTERFACE_IMPL(PointCloud::GlowManager, Object, IGlowManager)

PointCloud::GlowManager::GlowManager()
    : mX(0.0f)
    , mY(0.0f)
    , mRadius(0.0f)
    , mAlpha(0.0f)
{}

ECode PointCloud::GlowManager::SetX(
    /* [in] */ Float x1)
{
    mX = x1;
    return NOERROR;
}

ECode PointCloud::GlowManager::GetX(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mX;
    return NOERROR;
}

ECode PointCloud::GlowManager::SetY(
    /* [in] */ Float y1)
{
    mY = y1;
    return NOERROR;
}

ECode PointCloud::GlowManager::GetY(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mY;
    return NOERROR;
}

ECode PointCloud::GlowManager::SetAlpha(
    /* [in] */ Float a)
{
    mAlpha = a;
    return NOERROR;
}

ECode PointCloud::GlowManager::GetAlpha(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAlpha;
    return NOERROR;
}

ECode PointCloud::GlowManager::SetRadius(
    /* [in] */ Float r)
{
    mRadius = r;
    return NOERROR;
}

ECode PointCloud::GlowManager::GetRadius(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRadius;
    return NOERROR;
}

//=====================================================================
//               PointCloud::Point::
//=====================================================================
CAR_INTERFACE_IMPL(PointCloud::Point, Object, IPointCloudPoint)

PointCloud::Point::Point(
    /* [in] */ Float x2,
    /* [in] */ Float y2,
    /* [in] */ Float r)
{
    mX = (Float) x2;
    mY = (Float) y2;
    mRadius = r;
}

//=====================================================================
//               PointCloud::
//=====================================================================
CAR_INTERFACE_IMPL(PointCloud, Object, IPointCloud)

PointCloud::PointCloud()
    : mCenterX(0.0f)
    , mCenterY(0.0f)
    , mScale(1.0f)
    , mOuterRadius(0.0f)
{
    CArrayList::New((IArrayList**)&mPointCloud);

    mWaveManager = new WaveManager();
    mGlowManager = new GlowManager();
}

ECode PointCloud::constructor(
    /* [in] */ IDrawable* drawable)
{
    CPaint::New((IPaint**)&mPaint);
    mPaint->SetFilterBitmap(TRUE);
    AutoPtr<IColor> clr;
    CColor::AcquireSingleton((IColor**)&clr);
    Int32 c = 0;
    clr->Rgb(255, 255, 255, &c);
    mPaint->SetColor(c); // TODO: make configurable
    mPaint->SetAntiAlias(TRUE);
    mPaint->SetDither(TRUE);

    mDrawable = drawable;
    if (mDrawable != NULL) {
        Int32 w = 0, h = 0;
        drawable->GetIntrinsicWidth(&w);
        drawable->GetIntrinsicHeight(&h);
        drawable->SetBounds(0, 0, w, h);
    }
    return NOERROR;
}

ECode PointCloud::SetCenter(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mCenterX = x;
    mCenterY = y;
    return NOERROR;
}

ECode PointCloud::MakePointCloud(
    /* [in] */ Float innerRadius,
    /* [in] */ Float outerRadius)
{
    if (innerRadius == 0) {
        Logger::W(TAG, "Must specify an inner radius");
        return NOERROR;
    }
    mOuterRadius = outerRadius;
    mPointCloud->Clear();
    Float pointAreaRadius = (outerRadius - innerRadius);
    Float ds = (2.0f * PI * innerRadius / INNER_POINTS);
    Int32 bands = (Int32) Elastos::Core::Math::Round(pointAreaRadius / ds);
    Float dr = pointAreaRadius / bands;
    Float r = innerRadius;
    for (Int32 b = 0; b <= bands; b++, r += dr) {
        Float circumference = 2.0f * PI * r;
        Int32 pointsInBand = (Int32) (circumference / ds);
        Float eta = PI/2.0f;
        Float dEta = 2.0f * PI / pointsInBand;
        for (Int32 i = 0; i < pointsInBand; i++) {
            Float x = r * FloatMath::Cos(eta);
            Float y = r * FloatMath::Sin(eta);
            eta += dEta;
            AutoPtr<Point> pt = new Point(x, y, r);
            mPointCloud->Add((IInterface*)(IObject*)pt.Get());
        }
    }
    return NOERROR;
}

ECode PointCloud::SetScale(
    /* [in] */ Float scale)
{
    mScale  = scale;
    return NOERROR;
}

ECode PointCloud::GetScale(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mScale;
    return NOERROR;
}

Float PointCloud::Hypot(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return FloatMath::Sqrt(x*x + y*y);
}

Float PointCloud::Max(
    /* [in] */ Float a,
    /* [in] */ Float b)
{
    return a > b ? a : b;
}

ECode PointCloud::GetAlphaForPoint(
    /* [in] */ IPointCloudPoint* point,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    // Contribution from positional glow
    AutoPtr<Point> cpt = (Point*)point;
    AutoPtr<GlowManager> cgm = (GlowManager*)mGlowManager.Get();
    Float glowDistance = Hypot(cgm->mX - cpt->mX, cgm->mY - cpt->mY);
    Float glowAlpha = 0.0f;
    if (glowDistance < cgm->mRadius) {
        Float cosf = FloatMath::Cos(PI * 0.25f * glowDistance / cgm->mRadius);
        glowAlpha = cgm->mAlpha * Max(0.0f, (Float) Elastos::Core::Math::Pow(cosf, 10.0f));
    }

    // Compute contribution from Wave
    Float radius = Hypot(cpt->mX, cpt->mY);
    Float waveAlpha = 0.0f;
    AutoPtr<WaveManager> cwm = (WaveManager*)mWaveManager.Get();
    if (radius < cwm->mRadius * 2) {
        Float distanceToWaveRing = (radius - cwm->mRadius);
        Float cosf = FloatMath::Cos(PI * 0.5f * distanceToWaveRing / cwm->mRadius);
        waveAlpha = cwm->mAlpha * Max(0.0f, (Float) Elastos::Core::Math::Pow(cosf, 6.0f));
    }
    *result = (Int32) (Max(glowAlpha, waveAlpha) * 255);
    return NOERROR;
}

Float PointCloud::Interp(
    /* [in] */ Float min,
    /* [in] */ Float max,
    /* [in] */ Float f)
{
    return min + (max - min) * f;
}

ECode PointCloud::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IArrayList> points = mPointCloud;
    Int32 result = 0;
    canvas->Save(ICanvas::MATRIX_SAVE_FLAG, &result);
    canvas->Scale(mScale, mScale, mCenterX, mCenterY);
    Int32 s = 0;
    points->GetSize(&s);
    for (Int32 i = 0; i < s; i++) {
        AutoPtr<IInterface> node;
        points->Get(i, (IInterface**)&node);
        AutoPtr<IPointCloudPoint> point = IPointCloudPoint::Probe(node);
        AutoPtr<Point> cpt = (Point*)point.Get();
        Float pointSize = Interp(MAX_POINT_SIZE, MIN_POINT_SIZE,
                cpt->mRadius / mOuterRadius);
        Float px = cpt->mX + mCenterX;
        Float py = cpt->mY + mCenterY;
        Int32 alpha = 0;
        GetAlphaForPoint(point, &alpha);

        if (alpha == 0) continue;

        if (mDrawable != NULL) {
            canvas->Save(ICanvas::MATRIX_SAVE_FLAG, &result);
            Int32 w = 0, h = 0;
            mDrawable->GetIntrinsicWidth(&w);
            mDrawable->GetIntrinsicHeight(&h);
            Float cx = w * 0.5f;
            Float cy = h * 0.5f;
            Float s = pointSize / MAX_POINT_SIZE;
            canvas->Scale(s, s, px, py);
            canvas->Translate(px - cx, py - cy);
            mDrawable->SetAlpha(alpha);
            mDrawable->Draw(canvas);
            canvas->Restore();
        }
        else {
            mPaint->SetAlpha(alpha);
            canvas->DrawCircle(px, py, pointSize, mPaint);
        }
    }
    canvas->Restore();
    return NOERROR;
}

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
