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