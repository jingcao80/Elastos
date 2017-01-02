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

#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/TextTrackCue.h"
#include "elastos/droid/media/WebVttParserHelper.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::CInteger32;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Media {

TextTrackCue::TextTrackCue()
    : mId(String(""))
    , mPauseOnExit(FALSE)
    , mWritingDirection(ITextTrackCue::WRITING_DIRECTION_HORIZONTAL)
    , mRegionId(String(""))
    , mSnapToLines(TRUE)
    , mAutoLinePosition(FALSE)
    , mTextPosition(50)
    , mSize(100)
    , mAlignment(ITextTrackCue::ALIGNMENT_MIDDLE)
{}

TextTrackCue::~TextTrackCue()
{}

CAR_INTERFACE_IMPL_2(TextTrackCue, Object, ITextTrackCue, ISubtitleTrackCue)

ECode TextTrackCue::constructor()
{
    return NOERROR;
}

ECode TextTrackCue::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    if (ITextTrackCue::Probe(other) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    if (Object::Equals(ITextTrackCue::Probe(this),  ITextTrackCue::Probe(other))) {
        *result = TRUE;
        return NOERROR;
    }

    // try {
    Int32 length = mLines->GetLength();
    AutoPtr<TextTrackCue> cue = (TextTrackCue*)(ITextTrackCue::Probe(other));
    AutoPtr<ArrayOf<IArrayOf*> > lines = cue->mLines;
    Int32 len = lines->GetLength();
    Boolean res = mId.Equals(cue->mId) &&
            mPauseOnExit == cue->mPauseOnExit &&
            mWritingDirection == cue->mWritingDirection &&
            mRegionId.Equals(cue->mRegionId) &&
            mSnapToLines == cue->mSnapToLines &&
            mAutoLinePosition == cue->mAutoLinePosition &&
            (mAutoLinePosition || mLinePosition == cue->mLinePosition) &&
            mTextPosition == cue->mTextPosition &&
            mSize == cue->mSize && mAlignment == cue->mAlignment &&
            length == len;
    if (res == TRUE) {
        for (Int32 line = 0; line < length; line++) {
            AutoPtr<IArrayOf> spans = (*mLines)[line];
            AutoPtr<IArrayOf> spans_ = (*lines)[line];
            Boolean bEqual = FALSE;
            spans->DeepEquals(spans_, &bEqual);
            if (!bEqual) {
                *result = FALSE;
                return NOERROR;
            }
        }
    }
    *result = res;
    return NOERROR;
    // } catch(IncompatibleClassChangeError e) {
        // *result = FALSE;
        // return NOERROR;
    // }
}

ECode TextTrackCue::AppendStringsToBuilder(
    /* [in] */ IStringBuilder* builder,
    /* [out] */ IStringBuilder** result)
{
    VALIDATE_NOT_NULL(result)

    if (mStrings == NULL) {
        builder->Append(String("NULL"));
    } else {
        builder->Append(String("["));
        Boolean first = TRUE;
        Int32 len = mStrings->GetLength();
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<ICharSequence> s = (*mStrings)[i];
            if (!first) {
                builder->Append(String(", "));
            }
            if (s == NULL) {
                builder->Append(String("NULL"));
            } else {
                builder->Append(String("\""));
                builder->Append(s);
                builder->Append(String("\""));
            }
            first = FALSE;
        }
        builder->Append(String("]"));
    }
    *result = builder;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TextTrackCue::AppendLinesToBuilder(
    /* [in] */ IStringBuilder* builder,
    /* [out] */ IStringBuilder** result)
{
    VALIDATE_NOT_NULL(result);

    if (mLines == NULL) {
        builder->Append(String("NULL"));
    } else {
        builder->Append(String("["));
        Boolean first = TRUE;
        Int32 length = mLines->GetLength();
        for (Int32 i = 0; i < length; i++) {
            if (!first) {
                builder->Append(String(", "));
            }
            AutoPtr<IArrayOf> spans = (*mLines)[i];
            if (spans == NULL) {
                builder->Append(String("NULL"));
            } else {
                builder->Append(String("\""));
                Boolean innerFirst = TRUE;
                Int64 lastTimestamp = -1;
                Int32 len;
                spans->GetLength(&len);
                for (Int32 i = 0; i < len; i++) {
                    AutoPtr<IInterface> obj;
                    spans->Get(i, (IInterface**)&obj);
                    AutoPtr<ITextTrackCueSpan> span = ITextTrackCueSpan::Probe(obj);
                    if (!innerFirst) {
                        builder->Append(String(" "));
                    }
                    Int64 timestampMs;
                    span->GetTimeStampMs(&timestampMs);
                    if (timestampMs != lastTimestamp) {
                        builder->Append(String("<"));
                        String inStr;
                        WebVttParserHelper::TimeToString(timestampMs, &inStr);
                        builder->Append(inStr);
                        builder->Append(String(">"));
                        lastTimestamp = timestampMs;
                    }
                    String text;
                    span->GetText(&text);
                    builder->Append(text);
                    innerFirst = FALSE;
                }
                builder->Append(String("\""));
            }
            first = FALSE;
        }
        builder->Append(String("]"));
    }
    *result = builder;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TextTrackCue::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IStringBuilder> res;

    String str;
    WebVttParserHelper::TimeToString(mStartTimeMs, &str);
    String str_;
    WebVttParserHelper::TimeToString(mEndTimeMs, &str_);
    res->Append(str);
    res->Append(String(" --> "));
    res->Append(str_);
    res->Append(String(" {id:\""));
    res->Append(mId);
    res->Append(String("\", pauseOnExit:"));
    res->Append(mPauseOnExit);
    res->Append(String(", direction:"));
    res->Append(mWritingDirection == ITextTrackCue::WRITING_DIRECTION_HORIZONTAL ? String("horizontal") :
                mWritingDirection == ITextTrackCue::WRITING_DIRECTION_VERTICAL_LR ? String("vertical_lr") :
                mWritingDirection == ITextTrackCue::WRITING_DIRECTION_VERTICAL_RL ? String("vertical_rl") :
                String("INVALID"));
    res->Append(String(", regionId:\""));
    res->Append(mRegionId);
    res->Append(String("\", snapToLines:"));
    res->Append(mSnapToLines);
    res->Append(String(", linePosition:"));
    if (mAutoLinePosition) {
        res->Append(String("auto"));
    } else {
        res->Append(mLinePosition);
    }
    res->Append(String(", textPosition:"));
    res->Append(mTextPosition);
    res->Append(String(", size:"));
    res->Append(mSize);
    res->Append(String(", alignment:"));
    res->Append(mAlignment == ITextTrackCue::ALIGNMENT_END ? String("end") :
                mAlignment == ITextTrackCue::ALIGNMENT_LEFT ? String("left") :
                mAlignment == ITextTrackCue::ALIGNMENT_MIDDLE ? String("middle") :
                mAlignment == ITextTrackCue::ALIGNMENT_RIGHT ? String("right") :
                mAlignment == ITextTrackCue::ALIGNMENT_START ? String("start") : String("INVALID"));
    res->Append(String(", text:"));
    AutoPtr<IStringBuilder> ret;
    AppendStringsToBuilder(res.Get(), (IStringBuilder**)&ret);
    ret->Append(String("}"));
    StringBuilder sbr;
    sbr.Append(ret);
    return sbr.ToString(result);
}

ECode TextTrackCue::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    String str;
    ToString(&str);
    *hashCode = str.GetHashCode();
    return NOERROR;
}

ECode TextTrackCue::OnTime(
    /* [in] */ Int64 timeMs)
{
    Int32 len = mLines->GetLength();
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IArrayOf> spans = (*mLines)[i];
        Int32 length;
        spans->GetLength(&length);
        for (Int32 j = 0; j < length; j++) {
            AutoPtr<IInterface> obj;
            spans->Get(j, (IInterface**)&obj);
            AutoPtr<ITextTrackCueSpan> span = ITextTrackCueSpan::Probe(obj);
            Int64 timestampMs;
            span->GetTimeStampMs(&timestampMs);
            Boolean flag = timeMs >= timestampMs;
            span->SetEnabled(flag);
        }
    }
    return NOERROR;
}

ECode TextTrackCue::SetId(
    /* [in] */ const String& id)
{
    mId = id;
    return NOERROR;
}

ECode TextTrackCue::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode TextTrackCue::SetPauseOnExit(
    /* [in] */ Boolean bPause)
{
    mPauseOnExit = bPause;
    return NOERROR;
}

ECode TextTrackCue::GetPauseOnExit(
    /* [out] */ Boolean* bPause)
{
    VALIDATE_NOT_NULL(bPause);
    *bPause = mPauseOnExit;
    return NOERROR;
}

ECode TextTrackCue::SetWritingDirection(
    /* [in] */ Int32 wd)
{
    mWritingDirection = wd;
    return NOERROR;
}

ECode TextTrackCue::GetWritingDirection(
    /* [out] */ Int32* wd)
{
    VALIDATE_NOT_NULL(wd);
    *wd = mWritingDirection;
    return NOERROR;
}

ECode TextTrackCue::SetRegionId(
    /* [in] */ const String& regionId)
{
    mRegionId = regionId;
    return NOERROR;
}

ECode TextTrackCue::GetRegionId(
    /* [out] */ String* regionId)
{
    VALIDATE_NOT_NULL(regionId);
    *regionId = mRegionId;
    return NOERROR;
}

ECode TextTrackCue::SetSnapToLines(
    /* [in] */ Boolean bSnapToLines)
{
    mSnapToLines = bSnapToLines;
    return NOERROR;
}

ECode TextTrackCue::GetSnapToLines(
    /* [out] */ Boolean* bSnapToLines)
{
    VALIDATE_NOT_NULL(bSnapToLines);
    *bSnapToLines = mSnapToLines;
    return NOERROR;
}

ECode TextTrackCue::SetLinePosition(
    /* [in] */ Int32 linePosition)
{
    AutoPtr<IInteger32> lp;
    CInteger32::New(linePosition, (IInteger32**)&lp);
    mLinePosition = lp;
    return NOERROR;
}

ECode TextTrackCue::GetLinePosition(
    /* [out] */ Int32* linePosition)
{
    VALIDATE_NOT_NULL(linePosition)
    Int32 lp;
    mLinePosition->GetValue(&lp);
    *linePosition = lp;
    return NOERROR;
}

ECode TextTrackCue::SetTextPosition(
    /* [in] */ Int32 textPosition)
{
    mTextPosition = textPosition;
    return NOERROR;
}

ECode TextTrackCue::GetTextPosition(
    /* [out] */ Int32* textPosition)
{
    VALIDATE_NOT_NULL(textPosition);
    *textPosition = mTextPosition;
    return NOERROR;
}

ECode TextTrackCue::SetSize(
    /* [in] */ Int32 size)
{
    mSize = size;
    return NOERROR;
}

ECode TextTrackCue::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = mSize;
    return NOERROR;
}

ECode TextTrackCue::SetAlignment(
    /* [in] */ Int32 alignment)
{
    mAlignment = alignment;
    return NOERROR;
}

ECode TextTrackCue::GetAlignment(
    /* [out] */ Int32* alignment)
{
    VALIDATE_NOT_NULL(alignment);
    *alignment = mAlignment;
    return NOERROR;
}

ECode TextTrackCue::GetStrings(
    /* [out, callee] */ ArrayOf<ICharSequence*>** strings)
{
    VALIDATE_NOT_NULL(strings);
    *strings = mStrings;
    REFCOUNT_ADD(*strings);
    return NOERROR;
}

ECode TextTrackCue::SetStrings(
    /* [in] */ ArrayOf<ICharSequence*>* strings)
{
    mStrings = strings;
    return NOERROR;
}

ECode TextTrackCue::GetLines(
    /* [out, callee] */ ArrayOf<IArrayOf*>** lines)
{
    VALIDATE_NOT_NULL(lines);
    *lines = mLines;
    REFCOUNT_ADD(*lines);
    return NOERROR;
}

ECode TextTrackCue::SetLines(
    /* [in] */ ArrayOf<IArrayOf*>* lines)
{
    mLines = lines;
    return NOERROR;
}

ECode TextTrackCue::GetRegion(
    /* [out] */ ITextTrackRegion** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRegion;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TextTrackCue::SetRegion(
    /* [in] */ ITextTrackRegion* ttr)
{
    mRegion = ttr;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos