#include "elastos/droid/media/TextTrackRegion.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

TextTrackRegion::TextTrackRegion()
    : mId(String(""))
    , mWidth(100)
    , mLines(3)
    , mAnchorPointX(0)
    , mAnchorPointY(100)
    , mViewportAnchorPointX(0)
    , mViewportAnchorPointY(100)
    , mScrollValue(ITextTrackRegion::SCROLL_VALUE_NONE)
{}

TextTrackRegion::~TextTrackRegion()
{}

CAR_INTERFACE_IMPL(TextTrackRegion, Object, ITextTrackRegion)

ECode TextTrackRegion::constructor()
{
    return NOERROR;
}

ECode TextTrackRegion::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder res(String(" {id:\""));
    res.Append(mId);
    res.Append(String("\", width:"));
    res.Append(mWidth);
    res.Append(String(", lines:"));
    res.Append(mLines);
    res.Append(String(", anchorPoint:("));
    res.Append(mAnchorPointX);
    res.Append(String(", "));
    res.Append(mAnchorPointY);
    res.Append(String("), viewportAnchorPoints:"));
    res.Append(mViewportAnchorPointX);
    res.Append(String(", "));
    res.Append(mViewportAnchorPointY);
    res.Append(String("), scrollValue:"));
    res.Append(mScrollValue == ITextTrackRegion::SCROLL_VALUE_NONE ? String("none") :
                mScrollValue == ITextTrackRegion::SCROLL_VALUE_SCROLL_UP ? String("scroll_up") :
                String("INVALID"));
    res.Append("}");
    return res.ToString(result);
}

ECode TextTrackRegion::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode TextTrackRegion::SetId(
    /* [in] */ const String& id)
{
    mId = id;
    return NOERROR;
}

ECode TextTrackRegion::GetWidth(
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mWidth;
    return NOERROR;
}

ECode TextTrackRegion::SetWidth(
    /* [in] */ Float width)
{
    mWidth = width;
    return NOERROR;
}

ECode TextTrackRegion::GetLines(
    /* [out] */ Int32* lines)
{
    VALIDATE_NOT_NULL(lines);
    *lines = mLines;
    return NOERROR;
}

ECode TextTrackRegion::SetLines(
    /* [in] */ Int32 lines)
{
    mLines = lines;
    return NOERROR;
}

ECode TextTrackRegion::GetAnchorPointX(
    /* [out] */ Float* anchorPointX)
{
    VALIDATE_NOT_NULL(anchorPointX);
    *anchorPointX = mAnchorPointX;
    return NOERROR;
}

ECode TextTrackRegion::SetAnchorPointX(
    /* [in] */ Float anchorPointX)
{
    mAnchorPointX = anchorPointX;
    return NOERROR;
}

ECode TextTrackRegion::GetAnchorPointY(
    /* [out] */ Float* anchorPointY)
{
    VALIDATE_NOT_NULL(anchorPointY);
    *anchorPointY = mAnchorPointY;
    return NOERROR;
}

ECode TextTrackRegion::SetAnchorPointY(
    /* [in] */ Float anchorPointY)
{
    mAnchorPointY = anchorPointY;
    return NOERROR;
}

ECode TextTrackRegion::GetViewportAnchorPointX(
    /* [out] */ Float* pointX)
{
    VALIDATE_NOT_NULL(pointX);
    *pointX = mViewportAnchorPointX;
    return NOERROR;
}

ECode TextTrackRegion::SetViewportAnchorPointX(
    /* [in] */ Float pointX)
{
    mViewportAnchorPointX = pointX;
    return NOERROR;
}

ECode TextTrackRegion::GetViewportAnchorPointY(
    /* [out] */ Float* pointY)
{
    VALIDATE_NOT_NULL(pointY);
    *pointY = mViewportAnchorPointY;
    return NOERROR;
}

ECode TextTrackRegion::SetViewportAnchorPointY(
    /* [in] */ Float pointY)
{
    mViewportAnchorPointY = pointY;
    return NOERROR;
}

ECode TextTrackRegion::GetScrollValue(
    /* [out] */ Int32* scrollValue)
{
    VALIDATE_NOT_NULL(scrollValue);
    *scrollValue = mScrollValue;
    return NOERROR;
}

ECode TextTrackRegion::SetScrollValue(
    /* [in] */ Int32 scrollValue)
{
    mScrollValue = scrollValue;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos