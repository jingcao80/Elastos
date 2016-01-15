
#include "elastos/droid/webkit/native/content/browser/RenderCoordinates.h"
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//=====================================================================
//                  RenderCoordinates::NormalizedPoint
//=====================================================================

RenderCoordinates::NormalizedPoint::NormalizedPoint(
    /* [in] */ RenderCoordinates* owner)
    : mOwner(owner)
{
}

Float RenderCoordinates::NormalizedPoint::GetXAbsoluteCss()
{
    return mXAbsoluteCss;
}

Float RenderCoordinates::NormalizedPoint::GetYAbsoluteCss()
{
    return mYAbsoluteCss;
}

Float RenderCoordinates::NormalizedPoint::GetXLocalDip()
{
    return (mXAbsoluteCss - mOwner->mScrollXCss) * mOwner->mPageScaleFactor;
}

Float RenderCoordinates::NormalizedPoint::GetYLocalDip()
{
    return (mYAbsoluteCss - mOwner->mScrollYCss) * mOwner->mPageScaleFactor;
}

Float RenderCoordinates::NormalizedPoint::GetXPix()
{
    return GetXLocalDip() * mOwner->mDeviceScaleFactor;
}

Float RenderCoordinates::NormalizedPoint::GetYPix()
{
    return GetYLocalDip() * mOwner->mDeviceScaleFactor + mOwner->mContentOffsetYPix;
}

ECode RenderCoordinates::NormalizedPoint::SetAbsoluteCss(
    /* [in] */ Float xCss,
    /* [in] */ Float yCss)
{
    mXAbsoluteCss = xCss;
    mYAbsoluteCss = yCss;
    return NOERROR;
}

ECode RenderCoordinates::NormalizedPoint::SetLocalDip(
    /* [in] */ Float xDip,
    /* [in] */ Float yDip)
{
    SetAbsoluteCss(
            xDip / mOwner->mPageScaleFactor + mOwner->mScrollXCss,
            yDip / mOwner->mPageScaleFactor + mOwner->mScrollYCss);

    return NOERROR;
}

ECode RenderCoordinates::NormalizedPoint::SetScreen(
    /* [in] */ Float xPix,
    /* [in] */ Float yPix)
{
    SetLocalDip(xPix / mOwner->mDeviceScaleFactor, yPix / mOwner->mDeviceScaleFactor);

    return NOERROR;
}

//=====================================================================
//                          RenderCoordinates
//=====================================================================

ECode RenderCoordinates::Reset()
{
    mScrollXCss = mScrollYCss = 0;
    mPageScaleFactor = 1.0f;

    return NOERROR;
}

ECode RenderCoordinates::UpdateContentSizeCss(
    /* [in] */ Float contentWidthCss,
    /* [in] */ Float contentHeightCss)
{
    mContentWidthCss = contentWidthCss;
    mContentHeightCss = contentHeightCss;

    return NOERROR;
}

ECode RenderCoordinates::SetDeviceScaleFactor(
    /* [in] */ Float deviceScaleFactor)
{
    mDeviceScaleFactor = deviceScaleFactor;
    return NOERROR;
}

ECode RenderCoordinates::UpdateFrameInfo(
    /* [in] */ Float scrollXCss,
    /* [in] */ Float scrollYCss,
    /* [in] */ Float contentWidthCss,
    /* [in] */ Float contentHeightCss,
    /* [in] */ Float viewportWidthCss,
    /* [in] */ Float viewportHeightCss,
    /* [in] */ Float pageScaleFactor,
    /* [in] */ Float minPageScaleFactor,
    /* [in] */ Float maxPageScaleFactor,
    /* [in] */ Float contentOffsetYPix)
{
    mScrollXCss = scrollXCss;
    mScrollYCss = scrollYCss;
    mPageScaleFactor = pageScaleFactor;
    mMinPageScaleFactor = minPageScaleFactor;
    mMaxPageScaleFactor = maxPageScaleFactor;
    mContentOffsetYPix = contentOffsetYPix;

    UpdateContentSizeCss(contentWidthCss, contentHeightCss);
    mLastFrameViewportWidthCss = viewportWidthCss;
    mLastFrameViewportHeightCss = viewportHeightCss;

    return NOERROR;
}

AutoPtr<RenderCoordinates::NormalizedPoint> RenderCoordinates::CreateNormalizedPoint()
{
    return new RenderCoordinates::NormalizedPoint(this);
}

Float RenderCoordinates::GetScrollX()
{
    return mScrollXCss;
}

Float RenderCoordinates::GetScrollY()
{
    return mScrollYCss;
}

Float RenderCoordinates::GetScrollXPix()
{
    return FromLocalCssToPix(mScrollXCss);
}

Float RenderCoordinates::GetScrollYPix()
{
    return FromLocalCssToPix(mScrollYCss);
}

Int32 RenderCoordinates::GetScrollXPixInt()
{
    return (Int32) Elastos::Core::Math::Floor(GetScrollXPix());
}

Int32 RenderCoordinates::GetScrollYPixInt()
{
    return (Int32) Elastos::Core::Math::Floor(GetScrollYPix());
}

Float RenderCoordinates::GetContentWidthCss()
{
    return mContentWidthCss;
}

Float RenderCoordinates::GetContentHeightCss()
{
    return mContentHeightCss;
}

Float RenderCoordinates::GetContentWidthPix()
{
    return FromLocalCssToPix(mContentWidthCss);
}

Float RenderCoordinates::GetContentHeightPix()
{
    return FromLocalCssToPix(mContentHeightCss);
}

Int32 RenderCoordinates::GetContentWidthPixInt()
{
    return (Int32) Elastos::Core::Math::Ceil(GetContentWidthPix());
}

Int32 RenderCoordinates::GetContentHeightPixInt()
{
    return (Int32) Elastos::Core::Math::Ceil(GetContentHeightPix());
}

Float RenderCoordinates::GetLastFrameViewportWidthCss()
{
    return mLastFrameViewportWidthCss;
}

Float RenderCoordinates::GetLastFrameViewportHeightCss()
{
    return mLastFrameViewportHeightCss;
}

Float RenderCoordinates::GetLastFrameViewportWidthPix()
{
    return FromLocalCssToPix(mLastFrameViewportWidthCss);
}

Float RenderCoordinates::GetLastFrameViewportHeightPix()
{
    return FromLocalCssToPix(mLastFrameViewportHeightCss);
}

Int32 RenderCoordinates::GetLastFrameViewportWidthPixInt()
{
    return (Int32) Elastos::Core::Math::Ceil(GetLastFrameViewportWidthPix());
}

Int32 RenderCoordinates::GetLastFrameViewportHeightPixInt()
{
    return (Int32) Elastos::Core::Math::Ceil(GetLastFrameViewportHeightPix());
}

Float RenderCoordinates::GetContentOffsetYPix()
{
    return mContentOffsetYPix;
}

Float RenderCoordinates::GetPageScaleFactor()
{
    return mPageScaleFactor;
}

Float RenderCoordinates::GetMinPageScaleFactor()
{
    return mMinPageScaleFactor;
}

Float RenderCoordinates::GetMaxPageScaleFactor()
{
    return mMaxPageScaleFactor;
}

Float RenderCoordinates::GetDeviceScaleFactor()
{
    return mDeviceScaleFactor;
}

Float RenderCoordinates::GetMaxHorizontalScrollPix()
{
    return GetContentWidthPix() - GetLastFrameViewportWidthPix();
}

Float RenderCoordinates::GetMaxVerticalScrollPix()
{
    return GetContentHeightPix() - GetLastFrameViewportHeightPix();
}

Int32 RenderCoordinates::GetMaxHorizontalScrollPixInt()
{
    return (Int32) Elastos::Core::Math::Floor(GetMaxHorizontalScrollPix());
}

Int32 RenderCoordinates::GetMaxVerticalScrollPixInt()
{
    return (Int32) Elastos::Core::Math::Floor(GetMaxVerticalScrollPix());
}

Float RenderCoordinates::FromPixToDip(
    /* [in] */ Float pix)
{
    return pix / mDeviceScaleFactor;
}

Float RenderCoordinates::FromDipToPix(
    /* [in] */ Float dip)
{
    return dip * mDeviceScaleFactor;
}

Float RenderCoordinates::FromPixToLocalCss(
    /* [in] */ Float pix)
{
    return pix / (mDeviceScaleFactor * mPageScaleFactor);
}

Float RenderCoordinates::FromLocalCssToPix(
    /* [in] */ Float css)
{
    return css * mPageScaleFactor * mDeviceScaleFactor;
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
