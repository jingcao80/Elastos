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

#ifndef __ELASTOS_DROID_MEDIA_TEXTTRACKCUE_H__
#define __ELASTOS_DROID_MEDIA_TEXTTRACKCUE_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/SubtitleTrack.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::IStringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

class TextTrackCue
    : public SubtitleTrack::Cue
    , public ITextTrackCue
{
public:
    TextTrackCue();

    virtual ~TextTrackCue();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI AppendStringsToBuilder(
        /* [in] */ IStringBuilder* builder,
        /* [out] */ IStringBuilder** result);

    CARAPI AppendLinesToBuilder(
        /* [in] */ IStringBuilder* builder,
        /* [out] */ IStringBuilder** result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI OnTime(
        /* [in] */ Int64 timeMs);

    CARAPI SetId(
        /* [in] */ const String& id);

    CARAPI GetId(
        /* [out] */ String* id);

    CARAPI SetPauseOnExit(
        /* [in] */ Boolean bPause);

    CARAPI GetPauseOnExit(
        /* [out] */ Boolean* bPause);

    CARAPI SetWritingDirection(
        /* [in] */ Int32 wd);

    CARAPI GetWritingDirection(
        /* [out] */ Int32* wd);

    CARAPI SetRegionId(
        /* [in] */ const String& regionId);

    CARAPI GetRegionId(
        /* [out] */ String* regionId);

    CARAPI SetSnapToLines(
        /* [in] */ Boolean bSnapToLines);

    CARAPI GetSnapToLines(
        /* [out] */ Boolean* bSnapToLines);

    CARAPI SetLinePosition(
        /* [in] */ Int32 linePosition);

    CARAPI GetLinePosition(
        /* [out] */ Int32* linePosition);

    CARAPI SetTextPosition(
        /* [in] */ Int32 textPosition);

    CARAPI GetTextPosition(
        /* [out] */ Int32* textPosition);

    CARAPI SetSize(
        /* [in] */ Int32 size);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI SetAlignment(
        /* [in] */ Int32 alignment);

    CARAPI GetAlignment(
        /* [out] */ Int32* alignment);

    CARAPI GetStrings(
        /* [out, callee] */ ArrayOf<ICharSequence*>** strings);

    CARAPI SetStrings(
        /* [in] */ ArrayOf<ICharSequence*>* strings);

    CARAPI GetLines(
        /* [out, callee] */ ArrayOf<IArrayOf*>** lines);

    CARAPI SetLines(
        /* [in] */ ArrayOf<IArrayOf*>* lines);

    CARAPI GetRegion(
        /* [out] */ ITextTrackRegion** result);

    CARAPI SetRegion(
        /* [in] */ ITextTrackRegion* ttr);

public:
    String  mId;
    Boolean mPauseOnExit;
    Int32   mWritingDirection;
    String  mRegionId;
    Boolean mSnapToLines;
    AutoPtr<IInteger32> mLinePosition;  // null means AUTO
    Boolean mAutoLinePosition;
    Int32 mTextPosition;
    Int32 mSize;
    Int32 mAlignment;
    // Vector<String> mText;
    AutoPtr<ArrayOf<ICharSequence*> > mStrings;
    AutoPtr<ArrayOf<IArrayOf*> > mLines;
    AutoPtr<ITextTrackRegion> mRegion;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_TEXTTRACKCUE_H__