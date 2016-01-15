
#include "elastos/droid/ext/frameworkext.h"
#include "accessibility/CMagnificationSpec.h"
#include "accessibility/ScreenMagnifier.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

const Float CMagnificationSpec::DEFAULT_SCALE = 1.0f;

CMagnificationSpec::CMagnificationSpec()
    : mScale(DEFAULT_SCALE)
    , mMagnifiedRegionCenterX(0)
    , mMagnifiedRegionCenterY(0)
    , mScaledOffsetX(0)
    , mScaledOffsetY(0)
{
}

ECode CMagnificationSpec::constructor(
    /* [in] */ Handle32 screenMagnifier)
{
    mHost = (ScreenMagnifier*)screenMagnifier;
    return NOERROR;
}

ECode CMagnificationSpec::Initialize(
    /* [in] */ Float scale,
    /* [in] */ Float magnifiedRegionCenterX,
    /* [in] */ Float magnifiedRegionCenterY)
{
    using Elastos::Core::Math;

    mScale = scale;

    AutoPtr<IRect> bounds = mHost->mViewport->GetBounds();
    Int32 viewportWidth, viewportHeight;
    bounds->GetWidth(&viewportWidth);
    bounds->GetHeight(&viewportHeight);
    Float minMagnifiedRegionCenterX = (viewportWidth / 2) / scale;
    Float minMagnifiedRegionCenterY = (viewportHeight / 2) / scale;
    Float maxMagnifiedRegionCenterX = viewportWidth - minMagnifiedRegionCenterX;
    Float maxMagnifiedRegionCenterY = viewportHeight - minMagnifiedRegionCenterY;

    mMagnifiedRegionCenterX = Math::Min(Math::Max(magnifiedRegionCenterX,
        minMagnifiedRegionCenterX), maxMagnifiedRegionCenterX);
    mMagnifiedRegionCenterY = Math::Min(Math::Max(magnifiedRegionCenterY,
        minMagnifiedRegionCenterY), maxMagnifiedRegionCenterY);

    mScaledOffsetX = -(mMagnifiedRegionCenterX * scale - viewportWidth / 2);
    mScaledOffsetY = -(mMagnifiedRegionCenterY * scale - viewportHeight / 2);
    return NOERROR;
}

ECode CMagnificationSpec::UpdateFrom(
    /* [in] */ IMagnificationSpec* other)
{
    VALIDATE_NOT_NULL(other);

    other->GetScale(&mScale);
    other->GetMagnifiedRegionCenterX(&mMagnifiedRegionCenterX);
    other->GetMagnifiedRegionCenterY(&mMagnifiedRegionCenterY);
    other->GetScaledOffsetX(&mScaledOffsetX);
    other->GetScaledOffsetY(&mScaledOffsetY);
    return NOERROR;
}

ECode CMagnificationSpec::Reset()
{
    mScale = DEFAULT_SCALE;
    mMagnifiedRegionCenterX = 0;
    mMagnifiedRegionCenterY = 0;
    mScaledOffsetX = 0;
    mScaledOffsetY = 0;
    return NOERROR;
}

ECode CMagnificationSpec::SetScale(
    /* [in] */ Float value)
{
    mScale = value;
    return NOERROR;
}

ECode CMagnificationSpec::SetMagnifiedRegionCenterX(
    /* [in] */ Float value)
{
    mMagnifiedRegionCenterX = value;
    return NOERROR;
}

ECode CMagnificationSpec::SetMagnifiedRegionCenterY(
    /* [in] */ Float value)
{
    mMagnifiedRegionCenterY = value;
    return NOERROR;
}

ECode CMagnificationSpec::SetScaledOffsetX(
    /* [in] */ Float value)
{
    mScaledOffsetX = value;
    return NOERROR;
}

ECode CMagnificationSpec::SetScaledOffsetY(
    /* [in] */ Float value)
{
    mScaledOffsetY = value;
    return NOERROR;
}

ECode CMagnificationSpec::GetScale(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mScale;
    return NOERROR;
}

ECode CMagnificationSpec::GetMagnifiedRegionCenterX(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mMagnifiedRegionCenterX;
    return NOERROR;
}

ECode CMagnificationSpec::GetMagnifiedRegionCenterY(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mMagnifiedRegionCenterY;
    return NOERROR;
}

ECode CMagnificationSpec::GetScaledOffsetX(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mScaledOffsetX;
    return NOERROR;
}

ECode CMagnificationSpec::GetScaledOffsetY(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mScaledOffsetY;
    return NOERROR;
}

} // Accessibility
} // Server
} // Droid
} // Elastos
