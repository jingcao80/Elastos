#ifndef __ELASTOS_DROID_MEDIA_TEXTTRACKREGION_H__
#define __ELASTOS_DROID_MEDIA_TEXTTRACKREGION_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

class TextTrackRegion
    : public Object
    , public ITextTrackRegion
{
public:
    TextTrackRegion();

    virtual ~TextTrackRegion();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetId(
        /* [out] */ String* id);

    CARAPI SetId(
        /* [in] */ const String& id);

    CARAPI GetWidth(
        /* [out] */ Float* width);

    CARAPI SetWidth(
        /* [in] */ Float width);

    CARAPI GetLines(
        /* [out] */ Int32* lines);

    CARAPI SetLines(
        /* [in] */ Int32 lines);

    CARAPI GetAnchorPointX(
        /* [out] */ Float* anchorPointX);

    CARAPI SetAnchorPointX(
        /* [in] */ Float anchorPointX);

    CARAPI GetAnchorPointY(
        /* [out] */ Float* anchorPointY);

    CARAPI SetAnchorPointY(
        /* [in] */ Float anchorPointY);

    CARAPI GetViewportAnchorPointX(
        /* [out] */ Float* pointX);

    CARAPI SetViewportAnchorPointX(
        /* [in] */ Float pointX);

    CARAPI GetViewportAnchorPointY(
        /* [out] */ Float* pointY);

    CARAPI SetViewportAnchorPointY(
        /* [in] */ Float pointY);

    CARAPI GetScrollValue(
        /* [out] */ Int32* scrollValue);

    CARAPI SetScrollValue(
        /* [in] */ Int32 scrollValue);

public:
    String mId;
    Float mWidth;
    Int32 mLines;
    Float mAnchorPointX, mAnchorPointY;
    Float mViewportAnchorPointX, mViewportAnchorPointY;
    Int32 mScrollValue;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_TEXTTRACKREGION_H__