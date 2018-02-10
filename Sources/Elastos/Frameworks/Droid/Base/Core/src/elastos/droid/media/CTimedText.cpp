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

#include "elastos/droid/media/CTimedText.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Core::StringUtils;
using Elastos::Core::CInteger32;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Graphics::CRect;


namespace Elastos {
namespace Droid {
namespace Media {

const Int32 CTimedText::FIRST_PUBLIC_KEY                 = 1;

// These keys must be in sync with the keys in TextDescription.h
const Int32 CTimedText::KEY_DISPLAY_FLAGS                 = 1;
const Int32 CTimedText::KEY_STYLE_FLAGS                   = 2;
const Int32 CTimedText::KEY_BACKGROUND_COLOR_RGBA         = 3;
const Int32 CTimedText::KEY_HIGHLIGHT_COLOR_RGBA          = 4;
const Int32 CTimedText::KEY_SCROLL_DELAY                  = 5;
const Int32 CTimedText::KEY_WRAP_TEXT                     = 6;
const Int32 CTimedText::KEY_START_TIME                    = 7;
const Int32 CTimedText::KEY_STRUCT_BLINKING_TEXT_LIST     = 8;
const Int32 CTimedText::KEY_STRUCT_FONT_LIST              = 9;
const Int32 CTimedText::KEY_STRUCT_HIGHLIGHT_LIST         = 10;
const Int32 CTimedText::KEY_STRUCT_HYPER_TEXT_LIST        = 11;
const Int32 CTimedText::KEY_STRUCT_KARAOKE_LIST           = 12;
const Int32 CTimedText::KEY_STRUCT_STYLE_LIST             = 13;
const Int32 CTimedText::KEY_STRUCT_TEXT_POS               = 14;
const Int32 CTimedText::KEY_STRUCT_JUSTIFICATION          = 15;
const Int32 CTimedText::KEY_STRUCT_TEXT                   = 16;

const Int32 CTimedText::LAST_PUBLIC_KEY                  = 16;

const Int32 CTimedText::FIRST_PRIVATE_KEY                = 101;

// The following keys are used between TimedText.java and
// TextDescription.cpp in order to parce the Parcel.
const Int32 CTimedText::KEY_GLOBAL_SETTING               = 101;
const Int32 CTimedText::KEY_LOCAL_SETTING                = 102;
const Int32 CTimedText::KEY_START_CHAR                   = 103;
const Int32 CTimedText::KEY_END_CHAR                     = 104;
const Int32 CTimedText::KEY_FONT_ID                      = 105;
const Int32 CTimedText::KEY_FONT_SIZE                    = 106;
const Int32 CTimedText::KEY_TEXT_COLOR_RGBA              = 107;

const Int32 CTimedText::LAST_PRIVATE_KEY                 = 107;

const String CTimedText::TAG("TimedText");

CAR_INTERFACE_IMPL(CTimedText::CharPos, Object, ICharPos);

CTimedText::CharPos::CharPos()
{}

CTimedText::CharPos::~CharPos()
{}

ECode CTimedText::CharPos::constructor(
    /* [in] */ Int32 startChar,
    /* [in] */ Int32 endChar)
{
    mStartChar = startChar;
    mEndChar = endChar;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CTimedText::Justification, Object, IJustification);

CTimedText::Justification::Justification()
{}

CTimedText::Justification::~Justification()
{}

ECode CTimedText::Justification::constructor(
    /* [in] */ Int32 horizontal,
    /* [in] */ Int32 vertical)
{
    mHorizontalJustification = horizontal;
    mVerticalJustification = vertical;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CTimedText::Style, Object, IStyle);

CTimedText::Style::Style()
{}

CTimedText::Style::~Style()
{}

ECode CTimedText::Style::constructor(
    /* [in] */ Int32 startChar,
    /* [in] */ Int32 endChar,
    /* [in] */ Int32 fontId,
    /* [in] */ Boolean isBold,
    /* [in] */ Boolean isItalic,
    /* [in] */ Boolean isUnderlined,
    /* [in] */ Int32 fontSize,
    /* [in] */ Int32 colorRGBA)
{
    mStartChar = startChar;
    mEndChar = endChar;
    mFontID = fontId;
    mIsBold = isBold;
    mIsItalic = isItalic;
    mIsUnderlined = isUnderlined;
    mFontSize = fontSize;
    mColorRGBA = colorRGBA;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CTimedText::Font, Object, IFont);

CTimedText::Font::Font()
{}

CTimedText::Font::~Font()
{}

ECode CTimedText::Font::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& name)
{
    ID = id;
    mName = name;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CTimedText::Karaoke, Object, IKaraoke);

CTimedText::Karaoke::Karaoke()
{}

CTimedText::Karaoke::~Karaoke()
{}

ECode CTimedText::Karaoke::constructor(
    /* [in] */ Int32 startTimeMs,
    /* [in] */ Int32 endTimeMs,
    /* [in] */ Int32 startChar,
    /* [in] */ Int32 endChar)
{
    mStartTimeMs = startTimeMs;
    mEndTimeMs = endTimeMs;
    mStartChar = startChar;
    mEndChar = endChar;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CTimedText::HyperText, Object, IHyperText);

CTimedText::HyperText::HyperText()
{}

CTimedText::HyperText::~HyperText()
{}

ECode CTimedText::HyperText::constructor(
    /* [in] */ Int32 startChar,
    /* [in] */ Int32 endChar,
    /* [in] */ const String& url,
    /* [in] */ const String& alt)
{
    mStartChar = startChar;
    mEndChar = endChar;
    URL = url;
    altString = alt;
    return NOERROR;
}

CTimedText::CTimedText()
    : mDisplayFlags(-1)
    , mBackgroundColorRGBA(-1)
    , mHighlightColorRGBA(-1)
    , mScrollDelay(-1)
    , mWrapText(-1)
    , mKaraokeList(NULL)
    , mFontList(NULL)
    , mStyleList(NULL)
    , mHyperTextList(NULL)
    , mTextBounds(NULL)
    , mTextChars(NULL)
{
}

CTimedText::~CTimedText()
{}

CAR_OBJECT_IMPL(CTimedText);

CAR_INTERFACE_IMPL(CTimedText, Object, ITimedText);

ECode CTimedText::constructor(
    /* [in] */ IParcel* parcel)
{
    if (!ParseParcel(parcel)) {
        mKeyObjectMap.Clear();
        Logger::E(TAG, "parseParcel() fails");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CTimedText::GetText(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTextChars;
    return NOERROR;
}

ECode CTimedText::GetBounds(
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTextBounds;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Boolean CTimedText::ParseParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->SetDataPosition(0);

    Int32 size;
    parcel->GetDataSize(&size);
    if (size == 0) {
        return FALSE;
    }

    Int32 type;
    parcel->ReadInt32(&type);
    if (type == KEY_LOCAL_SETTING) {
        parcel->ReadInt32(&type);
        if (type != KEY_START_TIME) {
            return FALSE;
        }
        Int32 mStartTimeMs;
        parcel->ReadInt32(&mStartTimeMs);
        mKeyObjectMap[type] = CoreUtils::Convert(mStartTimeMs).Get();

        parcel->ReadInt32(&type);
        if (type != KEY_STRUCT_TEXT) {
            return FALSE;
        }

        Int32 textLen;
        parcel->ReadInt32(&textLen);

        AutoPtr<ArrayOf<Byte> > text;
        parcel->ReadArrayOf((HANDLE*)(&text));

        if (text == NULL || text->GetLength() == 0) {
            mTextChars = String(NULL);
        }
        else {
            mTextChars = String((const char*)text->GetPayload(), text->GetLength());
        }
    }
    else if (type != KEY_GLOBAL_SETTING) {
        Logger::W(TAG, "Invalid timed text key found: %d", type);
        return FALSE;
    }

    Int32 key;
    while ((parcel->GetDataSize(&size),  size) > 0) {
        parcel->ReadInt32(&key);
        if (!IsValidKey(key)) {
            Logger::W(TAG, "Invalid timed text key found: %d", key);
            return FALSE;
        }

        AutoPtr<IInterface> object;

        switch (key) {
            case KEY_STRUCT_STYLE_LIST:
            {
                ReadStyle(parcel);
                object = mStyleList.Get();
                break;
            }
            case KEY_STRUCT_FONT_LIST:
            {
                ReadFont(parcel);
                object = mFontList.Get();
                break;
            }
            case KEY_STRUCT_HIGHLIGHT_LIST:
            {
                ReadHighlight(parcel);
                object = mHighlightPosList.Get();
                break;
            }
            case KEY_STRUCT_KARAOKE_LIST:
            {
                ReadKaraoke(parcel);
                object = mKaraokeList.Get();
                break;
            }
            case KEY_STRUCT_HYPER_TEXT_LIST:
            {
                ReadHyperText(parcel);
                object = mHyperTextList.Get();
                break;
            }
            case KEY_STRUCT_BLINKING_TEXT_LIST:
            {
                ReadBlinkingText(parcel);
                object = mBlinkingPosList.Get();
                break;
            }
            case KEY_WRAP_TEXT:
            {
                parcel->ReadInt32(&mWrapText);
                object = CoreUtils::Convert(mWrapText).Get();
                break;
            }
            case KEY_HIGHLIGHT_COLOR_RGBA:
            {
                parcel->ReadInt32(&mHighlightColorRGBA);
                object = CoreUtils::Convert(mHighlightColorRGBA).Get();
                break;
            }
            case KEY_DISPLAY_FLAGS:
            {
                parcel->ReadInt32(&mDisplayFlags);
                object = CoreUtils::Convert(mDisplayFlags).Get();
                break;
            }
            case KEY_STRUCT_JUSTIFICATION:
            {
                Int32 horizontal;
                parcel->ReadInt32(&horizontal);
                Int32 vertical;
                parcel->ReadInt32(&vertical);
                mJustification = new Justification();
                mJustification->constructor(horizontal, vertical);
                object = IJustification::Probe(mJustification);
                break;
            }
            case KEY_BACKGROUND_COLOR_RGBA:
            {
                parcel->ReadInt32(&mBackgroundColorRGBA);
                object = CoreUtils::Convert(mBackgroundColorRGBA).Get();
                break;
            }
            case KEY_STRUCT_TEXT_POS:
            {
                Int32 top;
                parcel->ReadInt32(&top);
                Int32 left;
                parcel->ReadInt32(&left);
                Int32 bottom;
                parcel->ReadInt32(&bottom);
                Int32 right;
                parcel->ReadInt32(&right);
                mTextBounds = NULL;
                CRect::New(left, top, right, bottom, (IRect**)&mTextBounds);
                break;
            }
            case KEY_SCROLL_DELAY:
            {
                parcel->ReadInt32(&mScrollDelay);
                object = CoreUtils::Convert(mScrollDelay).Get();
                break;
            }
            default:
            {
                break;
            }
        }

        if (object != NULL) {
            HashMap<Int32, AutoPtr<IInterface> >::Iterator it = mKeyObjectMap.Find(key);
            if (it != mKeyObjectMap.End()) {
                mKeyObjectMap.Erase(it);
            }
            // Previous mapping will be replaced with the new object, if there was one.
            mKeyObjectMap[key] = object;
        }
    }

    return TRUE;
}

void CTimedText::ReadStyle(
    /* [in] */ IParcel* parcel)
{
    Boolean endOfStyle = FALSE;
    Int32 startChar = -1;
    Int32 endChar = -1;
    Int32 fontId = -1;
    Boolean isBold = FALSE;
    Boolean isItalic = FALSE;
    Boolean isUnderlined = FALSE;
    Int32 fontSize = -1;
    Int32 colorRGBA = -1;
    Int32 size = 0, key = 0;

    while (!endOfStyle && (parcel->GetDataSize(&size), size) > 0) {
        parcel->ReadInt32(&key);
        switch (key) {
            case KEY_START_CHAR:
            {
                parcel->ReadInt32(&startChar);
                break;
            }
            case KEY_END_CHAR:
            {
                parcel->ReadInt32(&endChar);
                break;
            }
            case KEY_FONT_ID:
            {
                parcel->ReadInt32(&fontId);
                break;
            }
            case KEY_STYLE_FLAGS:
            {
                Int32 flags;
                parcel->ReadInt32(&flags);
                // In the absence of any bits set in flags, the text
                // is plain. Otherwise, 1: bold, 2: italic, 4: underline
                isBold = ((flags % 2) == 1);
                isItalic = ((flags % 4) >= 2);
                isUnderlined = ((flags / 4) == 1);
                break;
            }
            case KEY_FONT_SIZE:
            {
                parcel->ReadInt32(&fontSize);
                break;
            }
            case KEY_TEXT_COLOR_RGBA:
            {
                parcel->ReadInt32(&colorRGBA);
                break;
            }
            default:
            {
                // End of the Style parsing. Reset the data position back
                // to the position before the last parcel.readInt() call.
                Int32 tempValue;
                parcel->GetDataPosition(&tempValue);
                parcel->SetDataPosition(tempValue - 4);
                endOfStyle = TRUE;
                break;
            }
        }
    }

    AutoPtr<Style> style = new Style();
    style->constructor(startChar, endChar, fontId,
        isBold, isItalic, isUnderlined, fontSize, colorRGBA);

    if (mStyleList == NULL) {
        AutoPtr<IArrayList> l;
        CArrayList::New((IArrayList**)&l);
        mStyleList = IList::Probe(l);
    }
    mStyleList->Add(IStyle::Probe(style));
}

void CTimedText::ReadFont(
    /* [in] */ IParcel* parcel)
{
    Int32 entryCount;
    parcel->ReadInt32(&entryCount);

    Int32 id, nameLen;
    for (Int32 i = 0; i < entryCount; i++) {
        parcel->ReadInt32(&id);
        parcel->ReadInt32(&nameLen);

        AutoPtr<ArrayOf<Byte> > text;
        parcel->ReadArrayOf((HANDLE*)(&text));

        String name("");
        if (text != NULL && text->GetLength() > 0) {
            name = String((const char*)text->GetPayload(), nameLen);
        }

        AutoPtr<Font> font = new Font();
        font->constructor(id, name);
        if (mFontList == NULL) {
            AutoPtr<IArrayList> l;
            CArrayList::New((IArrayList**)&l);
            mFontList = IList::Probe(l);
        }
        mFontList->Add(IFont::Probe(font));
    }
}

void CTimedText::ReadHighlight(
    /* [in] */ IParcel* parcel)
{
    Int32 startChar;
    parcel->ReadInt32(&startChar);
    Int32 endChar;
    parcel->ReadInt32(&endChar);
    AutoPtr<CharPos> pos = new CharPos();
    pos->constructor(startChar, endChar);

    if (mHighlightPosList == NULL) {
        AutoPtr<IArrayList> l;
        CArrayList::New((IArrayList**)&l);
        mHighlightPosList = IList::Probe(l);
    }
    mHighlightPosList->Add(ICharPos::Probe(pos));
}

void CTimedText::ReadKaraoke(
    /* [in] */ IParcel* parcel)
{
    Int32 entryCount;
    parcel->ReadInt32(&entryCount);

    for (Int32 i = 0; i < entryCount; i++) {
        Int32 startTimeMs;
        parcel->ReadInt32(&startTimeMs);
        Int32 endTimeMs;
        parcel->ReadInt32(&endTimeMs);
        Int32 startChar;
        parcel->ReadInt32(&startChar);
        Int32 endChar;
        parcel->ReadInt32(&endChar);
        AutoPtr<Karaoke> kara = new Karaoke();
        kara->constructor(startTimeMs, endTimeMs, startChar, endChar);

        if (mKaraokeList == NULL) {
            AutoPtr<IArrayList> l;
            CArrayList::New((IArrayList**)&l);
            mKaraokeList = IList::Probe(l);
        }
        mKaraokeList->Add(IKaraoke::Probe(kara));
    }
}

void CTimedText::ReadHyperText(
    /* [in] */ IParcel* parcel)
{
    Int32 startChar;
    parcel->ReadInt32(&startChar);
    Int32 endChar;
    parcel->ReadInt32(&endChar);

    Int32 len;
    parcel->ReadInt32(&len);

    AutoPtr<ArrayOf<Byte> > url;
    parcel->ReadArrayOf((HANDLE*)(&url));
    String urlString = String((const char*)url->GetPayload(), len);

    parcel->ReadInt32(&len);
    AutoPtr<ArrayOf<Byte> > alt;
    parcel->ReadArrayOf((HANDLE*)(&alt));
    String altString = String((const char*)alt->GetPayload(), len);

    AutoPtr<HyperText> hyperText = new HyperText();
    hyperText->constructor(startChar, endChar, urlString, altString);

    if (mHyperTextList == NULL) {
        AutoPtr<IArrayList> l;
        CArrayList::New((IArrayList**)&l);
        mHyperTextList = IList::Probe(l);
    }
    mHyperTextList->Add(IHyperText::Probe(hyperText));
}

void CTimedText::ReadBlinkingText(
    /* [in] */ IParcel* parcel)
{
    Int32 startChar;
    parcel->ReadInt32(&startChar);
    Int32 endChar;
    parcel->ReadInt32(&endChar);
    AutoPtr<CharPos> blinkingPos = new CharPos();
    blinkingPos->constructor(startChar, endChar);

    if (mBlinkingPosList == NULL) {
        AutoPtr<IArrayList> l;
        CArrayList::New((IArrayList**)&l);
        mBlinkingPosList = IList::Probe(l);
    }
    mBlinkingPosList->Add(ICharPos::Probe(blinkingPos));
}

Boolean CTimedText::IsValidKey(
    /* [in] */ Int32 key)
{
    if (!((key >= FIRST_PUBLIC_KEY) && (key <= LAST_PUBLIC_KEY))
            && !((key >= FIRST_PRIVATE_KEY) && (key <= LAST_PRIVATE_KEY))) {
        return FALSE;
    }
    return TRUE;
}

Boolean CTimedText::ContainsKey(
    /* [in] */ Int32 key)
{
    HashMap<Int32, AutoPtr<IInterface> >::Iterator it = mKeyObjectMap.Find(key);
    if (IsValidKey(key) && (it != mKeyObjectMap.End())) {
        return TRUE;
    }
    return FALSE;
}

Set<Int32> CTimedText::KeySet()
{
    HashMap<Int32, AutoPtr<IInterface> >::Iterator it = mKeyObjectMap.Begin();
    Set<Int32> set;
    for(; it != mKeyObjectMap.End(); ++it) {
        set.Insert(it->mFirst);
    }
    return set;
}

AutoPtr<IInterface> CTimedText::GetObject(
    /* [in] */ Int32 key)
{
    AutoPtr<IInterface> object;
    if (ContainsKey(key)) {
        HashMap<Int32, AutoPtr<IInterface> >::Iterator it = mKeyObjectMap.Find(key);
        object = it->mSecond;
    }
    else {
        Logger::E(TAG, "Invalid key: %d", key);
        //throw new IllegalArgumentException("Invalid key: " + key);
    }
    return object;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
