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

#include "Elastos.Droid.Graphics.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/incallui/widget/multiwaveview/PointCloud.h"
#include "elastos/droid/incallui/widget/multiwaveview/CGlowManager.h"
#include "elastos/droid/incallui/widget/multiwaveview/CWaveManager.h"
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
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

//=====================================================================
//               PointCloud::WaveManager
//=====================================================================
CAR_INTERFACE_IMPL(PointCloud::WaveManager, Object, IWaveManager)

PointCloud::WaveManager::WaveManager()
    : mRadius(50.0f)
    , mWidth(200.0f)
    , mAlpha(0.0f)
{}

ECode PointCloud::WaveManager::SetRadius(
    /* [in] */ Float r)
{
    mRadius = r;
    return NOERROR;
}

ECode PointCloud::WaveManager::GetRadius(
    /* [out] */ Float* r)
{
    VALIDATE_NOT_NULL(r);
    *r = mRadius;
    return NOERROR;
}

ECode PointCloud::WaveManager::SetAlpha(
    /* [in] */ Float a)
{
    mAlpha = a;
    return NOERROR;
}

ECode PointCloud::WaveManager::GetAlpha(
    /* [out] */ Float* a)
{
    VALIDATE_NOT_NULL(a);
    *a = mAlpha;
    return NOERROR;
}


//=====================================================================
//               PointCloud::GlowManager
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
    /* [out] */ Float* x1)
{
    VALIDATE_NOT_NULL(x1);
    *x1 = mX;
    return NOERROR;
}

ECode PointCloud::GlowManager::SetY(
    /* [in] */ Float y1)
{
    mY = y1;
    return NOERROR;
}

ECode PointCloud::GlowManager::GetY(
    /* [out] */ Float* y1)
{
    VALIDATE_NOT_NULL(y1);
    *y1 = mY;
    return NOERROR;
}

ECode PointCloud::GlowManager::SetAlpha(
    /* [in] */ Float a)
{
    mAlpha = a;
    return NOERROR;
}

ECode PointCloud::GlowManager::GetAlpha(
    /* [out] */ Float* a)
{
    VALIDATE_NOT_NULL(a);
    *a = mAlpha;
    return NOERROR;
}

ECode PointCloud::GlowManager::SetRadius(
    /* [in] */ Float r)
{
    mRadius = r;
    return NOERROR;
}

ECode PointCloud::GlowManager::GetRadius(
    /* [out] */ Float* r)
{
    VALIDATE_NOT_NULL(r);
    *r = mRadius;
    return NOERROR;
}


//=====================================================================
//               PointCloud::Point
//=====================================================================
PointCloud::Point::Point(
    /* [in] */ Float x2,
    /* [in] */ Float y2,
    /* [in] */ Float r)
    : mX(x2)
    , mY(y2)
    , mRadius(r)
{}


//=====================================================================
//               PointCloud
//=====================================================================
const Float PointCloud::MIN_POINT_SIZE = 2.0f;
const Float PointCloud::MAX_POINT_SIZE = 4.0f;
const Int32 PointCloud::INNER_POINTS = 8;
const String PointCloud::TAG(String("PointCloud"));
const Float PointCloud::PI = (Float) Elastos::Core::Math::PI;

PointCloud::PointCloud(
    /* [in] */ IDrawable* drawable)
    : mCenterX(0.0f)
    , mCenterY(0.0f)
    , mScale(1.0f)
    , mOuterRadius(0.0f)
{
    CArrayList::New((IArrayList**)&mPointCloud);

    AutoPtr<CWaveManager> wm;
    CWaveManager::NewByFriend((CWaveManager**)&wm);
    mWaveManager = (WaveManager*)wm.Get();
    AutoPtr<CGlowManager> gm;
    CGlowManager::NewByFriend((CGlowManager**)&gm);
    mGlowManager = (GlowManager*)gm.Get();

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
}

void PointCloud::SetCenter(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mCenterX = x;
    mCenterY = y;
}

void PointCloud::MakePointCloud(
    /* [in] */ Float innerRadius,
    /* [in] */ Float outerRadius)
{
    if (innerRadius == 0) {
        Logger::W(TAG, "Must specify an inner radius");
        return;
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
            mPointCloud->Add((IObject*)pt.Get());
        }
    }
}

void PointCloud::SetScale(
    /* [in] */ Float scale)
{
    mScale  = scale;
}

Float PointCloud::GetScale()
{
    return mScale;
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

Int32 PointCloud::GetAlphaForPoint(
    /* [in] */ Point* point)
{
    // Contribution from positional glow
    Float glowDistance = Hypot(mGlowManager->mX - point->mX, mGlowManager->mY - point->mY);
    Float glowAlpha = 0.0f;

    if (glowDistance < mGlowManager->mRadius) {
        Float cosf = FloatMath::Cos(PI * 0.25f * glowDistance / mGlowManager->mRadius);
        glowAlpha = mGlowManager->mAlpha * Max(0.0f, (Float) Elastos::Core::Math::Pow(cosf, 10.0f));
    }

    // Compute contribution from Wave
    Float radius = Hypot(point->mX, point->mY);
    Float distanceToWaveRing = (radius - mWaveManager->mRadius);
    Float waveAlpha = 0.0f;
    if (distanceToWaveRing < mWaveManager->mWidth * 0.5f && distanceToWaveRing < 0.0f) {
        Float cosf = FloatMath::Cos(PI * 0.25f * distanceToWaveRing / mWaveManager->mWidth);
        waveAlpha = mWaveManager->mAlpha * Max(0.0f, (Float) Elastos::Core::Math::Pow(cosf, 20.0f));
    }

    return (Int32) (Max(glowAlpha, waveAlpha) * 255);
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
        Point* point = (Point*)IObject::Probe(node);
        Float pointSize = Interp(MAX_POINT_SIZE, MIN_POINT_SIZE,
                point->mRadius / mOuterRadius);
        Float px = point->mX + mCenterX;
        Float py = point->mY + mCenterY;
        Int32 alpha = GetAlphaForPoint(point);

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

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
