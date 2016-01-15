
#include "elastos/droid/media/CTimedText.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::CInteger32;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Graphics::CRect;


namespace Elastos {
namespace Droid {
namespace Media {

/*static*/ const String CTimedText::TAG("TimedText");

CTimedText::CharPos::CharPos(
    /* [in] */ Int32 startChar,
    /* [in] */ Int32 endChar)
    : mStartChar(startChar)
    , mEndChar(endChar)
{}

CAR_INTERFACE_IMPL(CTimedText::Justification, IInterface)

CTimedText::Justification::Justification(
    /* [in] */ Int32 horizontal,
    /* [in] */ Int32 vertical)
    : mHorizontalJustification(horizontal)
    , mVerticalJustification(vertical)
{}

CTimedText::Style::Style(
    /* [in] */ Int32 startChar,
    /* [in] */ Int32 endChar,
    /* [in] */ Int32 fontId,
    /* [in] */ Boolean isBold,
    /* [in] */ Boolean isItalic,
    /* [in] */ Boolean isUnderlined,
    /* [in] */ Int32 fontSize,
    /* [in] */ Int32 colorRGBA)
    : mStartChar(startChar)
    , mEndChar(endChar)
    , mFontID(fontId)
    , mIsBold(isBold)
    , mIsItalic(isItalic)
    , mIsUnderlined(isUnderlined)
    , mFontSize(fontSize)
    , mColorRGBA(colorRGBA)
{}

CTimedText::Font::Font(
    /* [in] */ Int32 id,
    /* [in] */ const String& name)
    : ID(id)
    , mName(name)
{}

CTimedText::Karaoke::Karaoke(
    /* [in] */ Int32 startTimeMs,
    /* [in] */ Int32 endTimeMs,
    /* [in] */ Int32 startChar,
    /* [in] */ Int32 endChar)
    : mStartTimeMs(startTimeMs)
    , mEndTimeMs(endTimeMs)
    , mStartChar(startChar)
    , mEndChar(endChar)
{}

CTimedText::HyperText::HyperText(
    /* [in] */ Int32 startChar,
    /* [in] */ Int32 endChar,
    /* [in] */ const String& url,
    /* [in] */ const String& alt)
    : mStartChar(startChar)
    , mEndChar(endChar)
    , URL(url)
    , altString(alt)
{}

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
    parcel->GetElementSize(&size);
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
        AutoPtr<IInteger32> iObj;
        CInteger32::New(mStartTimeMs, (IInteger32**)&iObj);
        mKeyObjectMap[type] = iObj;

        parcel->ReadInt32(&type);
        if (type != KEY_STRUCT_TEXT) {
            return FALSE;
        }

        Int32 textLen;
        parcel->ReadInt32(&textLen);

        AutoPtr<ArrayOf<Byte> > text;
        parcel->ReadArrayOf((Handle32*)(&text));

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
    while ((parcel->GetElementSize(&size),  size) > 0) {
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
                object = (IInterface*)mStyleList.Get();
                break;
            }
            case KEY_STRUCT_FONT_LIST:
            {
                ReadFont(parcel);
                object = (IInterface*)mFontList.Get();
                break;
            }
            case KEY_STRUCT_HIGHLIGHT_LIST:
            {
                ReadHighlight(parcel);
                object = (IInterface*)mHighlightPosList.Get();
                break;
            }
            case KEY_STRUCT_KARAOKE_LIST:
            {
                ReadKaraoke(parcel);
                object = (IInterface*)mKaraokeList.Get();
                break;
            }
            case KEY_STRUCT_HYPER_TEXT_LIST:
            {
                ReadHyperText(parcel);
                object = (IInterface*)mHyperTextList.Get();
                break;
            }
            case KEY_STRUCT_BLINKING_TEXT_LIST:
            {
                ReadBlinkingText(parcel);
                object = (IInterface*)mBlinkingPosList.Get();
                break;
            }
            case KEY_WRAP_TEXT:
            {
                parcel->ReadInt32(&mWrapText);
                AutoPtr<IInteger32> iObj;
                CInteger32::New(mWrapText, (IInteger32**)&iObj);
                object = iObj;
                break;
            }
            case KEY_HIGHLIGHT_COLOR_RGBA:
            {
                parcel->ReadInt32(&mHighlightColorRGBA);
                AutoPtr<IInteger32> iObj;
                CInteger32::New(mHighlightColorRGBA, (IInteger32**)&iObj);
                object = iObj;
                break;
            }
            case KEY_DISPLAY_FLAGS:
            {
                parcel->ReadInt32(&mDisplayFlags);
                AutoPtr<IInteger32> iObj;
                CInteger32::New(mDisplayFlags, (IInteger32**)&iObj);
                object = iObj;
                break;
            }
            case KEY_STRUCT_JUSTIFICATION:
            {
                Int32 horizontal;
                parcel->ReadInt32(&horizontal);
                Int32 vertical;
                parcel->ReadInt32(&vertical);
                mJustification = new Justification(horizontal, vertical);
                object = (IInterface*)mJustification.Get();
                break;
            }
            case KEY_BACKGROUND_COLOR_RGBA:
            {
                parcel->ReadInt32(&mBackgroundColorRGBA);
                AutoPtr<IInteger32> iObj;
                CInteger32::New(mBackgroundColorRGBA, (IInteger32**)&iObj);
                object = iObj;
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
                AutoPtr<IInteger32> iObj;
                CInteger32::New(mScrollDelay, (IInteger32**)&iObj);
                object = iObj;
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

    while (!endOfStyle && (parcel->GetElementSize(&size), size) > 0) {
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

    AutoPtr<Style> style = new Style(startChar, endChar, fontId,
        isBold, isItalic, isUnderlined, fontSize, colorRGBA);

    if (mStyleList == NULL) {
        mStyleList = new MyList< AutoPtr<Style> >();
    }
    mStyleList->PushBack(style);
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
        parcel->ReadArrayOf((Handle32*)(&text));

        String name("");
        if (text != NULL && text->GetLength() > 0) {
            name = String((const char*)text->GetPayload(), nameLen);
        }

        AutoPtr<Font> font = new Font(id, name);
        if (mFontList == NULL) {
            mFontList = new MyList< AutoPtr<Font> >();
        }
        mFontList->PushBack(font);
    }
}

void CTimedText::ReadHighlight(
    /* [in] */ IParcel* parcel)
{
    Int32 startChar;
    parcel->ReadInt32(&startChar);
    Int32 endChar;
    parcel->ReadInt32(&endChar);
    AutoPtr<CharPos> pos = new CharPos(startChar, endChar);

    if (mHighlightPosList == NULL) {
        mHighlightPosList = new CharPostList();
    }
    mHighlightPosList->PushBack(pos);
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
        AutoPtr<Karaoke> kara = new Karaoke(startTimeMs, endTimeMs, startChar, endChar);

        if (mKaraokeList == NULL) {
            mKaraokeList = new MyList< AutoPtr<Karaoke> >();
        }
        mKaraokeList->PushBack(kara);
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
    parcel->ReadArrayOf((Handle32*)(&url));
    String urlString = String((const char*)url->GetPayload(), len);

    parcel->ReadInt32(&len);
    AutoPtr<ArrayOf<Byte> > alt;
    parcel->ReadArrayOf((Handle32*)(&alt));
    String altString = String((const char*)alt->GetPayload(), len);

    AutoPtr<HyperText> hyperText = new HyperText(startChar, endChar, urlString, altString);

    if (mHyperTextList == NULL) {
        mHyperTextList = new MyList< AutoPtr<HyperText> >();
    }
    mHyperTextList->PushBack(hyperText);
}

void CTimedText::ReadBlinkingText(
    /* [in] */ IParcel* parcel)
{
    Int32 startChar;
    parcel->ReadInt32(&startChar);
    Int32 endChar;
    parcel->ReadInt32(&endChar);
    AutoPtr<CharPos> blinkingPos = new CharPos(startChar, endChar);

    if (mBlinkingPosList == NULL) {
        mBlinkingPosList = new CharPostList();
    }
    mBlinkingPosList->PushBack(blinkingPos);
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

AutoPtr<Set<Int32> > CTimedText::KeySet()
{
    HashMap<Int32, AutoPtr<IInterface> >::Iterator it = mKeyObjectMap.Begin();
    AutoPtr<Set<Int32> > set = new Set<Int32>();
    for(; it != mKeyObjectMap.End(); ++it) {
        set->Insert(it->mFirst);
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
