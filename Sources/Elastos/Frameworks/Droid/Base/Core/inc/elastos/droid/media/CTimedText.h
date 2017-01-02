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

#ifndef __ELASTOS_DROID_MEDIA_CTIMEDTEXT_H__
#define __ELASTOS_DROID_MEDIA_CTIMEDTEXT_H__

#include "_Elastos_Droid_Media_CTimedText.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/Set.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::Set;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CTimedText)
    , public Object
    , public ITimedText
{
public:
    /**
     * Helper class to hold the start char offset and end char offset
     * for Blinking Text or Highlight Text. endChar is the end offset
     * of the text (startChar + number of characters to be highlighted
     * or blinked). The member variables in this class are read-only.
     * {@hide}
     */
    class CharPos
        : public Object
        , public ICharPos
    {
    public:
        CharPos();

        ~CharPos();

        CAR_INTERFACE_DECL()

        /**
         * Constuctor
         * @param startChar the offset of the start character.
         * @param endChar the offset of the end character.
         */
        CARAPI constructor(
            /* [in] */ Int32 startChar,
            /* [in] */ Int32 endChar);

    public:
        /**
         * The offset of the start character
         */
        Int32 mStartChar;

        /**
         * The offset of the end character
         */
        Int32 mEndChar;
    };

    /**
     * Helper class to hold the justification for text display in the text box.
     * The member variables in this class are read-only.
     * {@hide}
     */
    class Justification
        : public Object
        , public IJustification
    {
    public:
        Justification();

        ~Justification();

        CAR_INTERFACE_DECL()

        /**
         * Constructor
         * @param horizontal the horizontal justification of the text.
         * @param vertical the vertical justification of the text.
         */
        CARAPI constructor(
            /* [in] */ Int32 horizontal,
            /* [in] */ Int32 vertical);

    public:
        /**
         * horizontal justification  0: left, 1: centered, -1: right
         */
        Int32 mHorizontalJustification;

        /**
         * vertical justification  0: top, 1: centered, -1: bottom
         */
        Int32 mVerticalJustification;
    };

    /**
     * Helper class to hold the style information to display the text.
     * The member variables in this class are read-only.
     * {@hide}
     */
    class Style
        : public Object
        , public IStyle
    {
    public:
        Style();

        ~Style();

        CAR_INTERFACE_DECL()

        /**
         * Constructor
         * @param startChar the offset of the start character which applys this style
         * @param endChar the offset of the end character which applys this style
         * @param fontId the ID of the font.
         * @param isBold whether the characters should be bold.
         * @param isItalic whether the characters should be italic.
         * @param isUnderlined whether the characters should be underlined.
         * @param fontSize the size of the font.
         * @param colorRGBA red, green, blue, and alpha value for color.
         */
        CARAPI constructor(
            /* [in] */ Int32 startChar,
            /* [in] */ Int32 endChar,
            /* [in] */ Int32 fontId,
            /* [in] */ Boolean isBold,
            /* [in] */ Boolean isItalic,
            /* [in] */ Boolean isUnderlined,
            /* [in] */ Int32 fontSize,
            /* [in] */ Int32 colorRGBA);

    public:
        /**
         * The offset of the start character which applys this style
         */
        Int32 mStartChar;

        /**
         * The offset of the end character which applys this style
         */
        Int32 mEndChar;

        /**
         * ID of the font. This ID will be used to choose the font
         * to be used from the font list.
         */
        Int32 mFontID;

        /**
         * True if the characters should be bold
         */
        Boolean mIsBold;

        /**
         * True if the characters should be italic
         */
        Boolean mIsItalic;

        /**
         * True if the characters should be underlined
         */
        Boolean mIsUnderlined;

        /**
         * The size of the font
         */
        Int32 mFontSize;

        /**
         * To specify the RGBA color: 8 bits each of red, green, blue,
         * and an alpha(transparency) value
         */
        Int32 mColorRGBA;
    };

    /**
     * Helper class to hold the font ID and name.
     * The member variables in this class are read-only.
     * {@hide}
     */
    class Font
        : public Object
        , public IFont
    {
    public:
        Font();

        ~Font();

        CAR_INTERFACE_DECL()

        /**
         * Constructor
         * @param id the font ID.
         * @param name the font name.
         */
        CARAPI constructor(
            /* [in] */ Int32 id,
            /* [in] */ const String& name);

    public:
        /**
         * The font ID
         */
        Int32 ID;

        /**
         * The font name
         */
        String mName;
    };

    /**
     * Helper class to hold the karaoke information.
     * The member variables in this class are read-only.
     * {@hide}
     */
    class Karaoke
        : public Object
        , public IKaraoke
    {
    public:
        Karaoke();

        ~Karaoke();

        CAR_INTERFACE_DECL()

        /**
         * Constructor
         * @param startTimeMs the start time (in milliseconds) to highlight
         * the characters between startChar and endChar.
         * @param endTimeMs the end time (in milliseconds) to highlight
         * the characters between startChar and endChar.
         * @param startChar the offset of the start character to be highlighted.
         * @param endChar the offset of the end character to be highlighted.
         */
        CARAPI constructor(
            /* [in] */ Int32 startTimeMs,
            /* [in] */ Int32 endTimeMs,
            /* [in] */ Int32 startChar,
            /* [in] */ Int32 endChar);

    public:
        /**
         * The start time (in milliseconds) to highlight the characters
         * specified by startChar and endChar.
         */
        Int32 mStartTimeMs;

        /**
         * The end time (in milliseconds) to highlight the characters
         * specified by startChar and endChar.
         */
        Int32 mEndTimeMs;

        /**
         * The offset of the start character to be highlighted
         */
        Int32 mStartChar;

        /**
         * The offset of the end character to be highlighted
         */
        Int32 mEndChar;
    };

    /**
     * Helper class to hold the hyper text information.
     * The member variables in this class are read-only.
     * {@hide}
     */
    class HyperText
        : public Object
        , public IHyperText
    {
    public:
        HyperText();

        ~HyperText();

        CAR_INTERFACE_DECL()

        /**
         * Constructor
         * @param startChar the offset of the start character.
         * @param endChar the offset of the end character.
         * @param url the linked-to URL.
         * @param alt the "alt" string for display.
         */
        CARAPI constructor(
            /* [in] */ Int32 startChar,
            /* [in] */ Int32 endChar,
            /* [in] */ const String& url,
            /* [in] */ const String& alt);

    public:
        /**
         * The offset of the start character
         */
        Int32 mStartChar;

        /**
         * The offset of the end character
         */
        Int32 mEndChar;

        /**
         * The linked-to URL
         */
        String URL;

        /**
         * The "alt" string for user display
         */
        String altString;
    };

public:
    /**
     * @param obj the byte array which contains the timed text.
     * @throws IllegalArgumentExcept if parseParcel() fails.
     * {@hide}
     */
    CTimedText();

    virtual ~CTimedText();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IParcel* parcel);

    /**
     * Get the characters in the timed text.
     *
     * @return the characters as a String object in the TimedText. Applications
     * should stop rendering previous timed text at the current rendering region if
     * a null is returned, until the next non-null timed text is received.
     */
    CARAPI GetText(
        /* [out] */ String* result);

    /**
     * Get the rectangle area or region for rendering the timed text as specified
     * by a Rect object.
     *
     * @return the rectangle region to render the characters in the timed text.
     * If no bounds information is available (a null is returned), render the
     * timed text at the center bottom of the display.
     */
    CARAPI GetBounds(
        /* [out] */ IRect** result);

private:
    /*
     * Go over all the records, collecting metadata keys and fields in the
     * Parcel. These are stored in mKeyObjectMap for application to retrieve.
     * @return false if an error occurred during parsing. Otherwise, true.
     */
    CARAPI_(Boolean) ParseParcel(
        /* [in] */ IParcel* parcel);

    /*
     * To parse and store the Style list.
     */
    CARAPI_(void) ReadStyle(
        /* [in] */ IParcel* parcel);

    /*
     * To parse and store the Font list
     */
    CARAPI_(void) ReadFont(
        /* [in] */ IParcel* parcel);

    /*
     * To parse and store the Highlight list
     */
    CARAPI_(void) ReadHighlight(
        /* [in] */ IParcel* parcel);

    /*
     * To parse and store the Karaoke list
     */
    CARAPI_(void) ReadKaraoke(
        /* [in] */ IParcel* parcel);

    /*
     * To parse and store HyperText list
     */
    CARAPI_(void) ReadHyperText(
        /* [in] */ IParcel* parcel);

    /*
     * To parse and store blinking text list
     */
    CARAPI_(void) ReadBlinkingText(
        /* [in] */ IParcel* parcel);

    /*
     * To check whether the given key is valid.
     * @param key the key to be checked.
     * @return true if the key is a valid one. Otherwise, false.
     */
    CARAPI_(Boolean) IsValidKey(
        /* [in] */ Int32 key);

    /*
     * To check whether the given key is contained in this TimedText object.
     * @param key the key to be checked.
     * @return true if the key is contained in this TimedText object.
     *         Otherwise, false.
     */
    CARAPI_(Boolean) ContainsKey(
        /* [in] */ Int32 key);

    /*
     * @return a set of the keys contained in this TimedText object.
     */
    CARAPI_(Set<Int32>) KeySet();

    /*
     * To retrieve the object associated with the key. Caller must make sure
     * the key is present using the containsKey method otherwise a
     * RuntimeException will occur.
     * @param key the key used to retrieve the object.
     * @return an object. The object could be 1) an instance of Integer; 2) a
     * List of CharPos, Karaoke, Font, Style, and HyperText, or 3) an instance of
     * Justification.
     */
    CARAPI_(AutoPtr<IInterface>) GetObject(
        /* [in] */ Int32 key);

private:
    static const Int32 FIRST_PUBLIC_KEY;

    // These keys must be in sync with the keys in TextDescription.h
    static const Int32 KEY_DISPLAY_FLAGS;
    static const Int32 KEY_STYLE_FLAGS;
    static const Int32 KEY_BACKGROUND_COLOR_RGBA;
    static const Int32 KEY_HIGHLIGHT_COLOR_RGBA;
    static const Int32 KEY_SCROLL_DELAY;
    static const Int32 KEY_WRAP_TEXT;
    static const Int32 KEY_START_TIME;
    static const Int32 KEY_STRUCT_BLINKING_TEXT_LIST;
    static const Int32 KEY_STRUCT_FONT_LIST;
    static const Int32 KEY_STRUCT_HIGHLIGHT_LIST;
    static const Int32 KEY_STRUCT_HYPER_TEXT_LIST;
    static const Int32 KEY_STRUCT_KARAOKE_LIST;
    static const Int32 KEY_STRUCT_STYLE_LIST;
    static const Int32 KEY_STRUCT_TEXT_POS;
    static const Int32 KEY_STRUCT_JUSTIFICATION;
    static const Int32 KEY_STRUCT_TEXT;
    static const Int32 LAST_PUBLIC_KEY;
    static const Int32 FIRST_PRIVATE_KEY;

    // The following keys are used between TimedText.java and
    // TextDescription.cpp in order to parce the Parcel.
    static const Int32 KEY_GLOBAL_SETTING;
    static const Int32 KEY_LOCAL_SETTING;
    static const Int32 KEY_START_CHAR;
    static const Int32 KEY_END_CHAR;
    static const Int32 KEY_FONT_ID;
    static const Int32 KEY_FONT_SIZE;
    static const Int32 KEY_TEXT_COLOR_RGBA;
    static const Int32 LAST_PRIVATE_KEY;

    static const String TAG;

    HashMap<Int32, AutoPtr<IInterface> > mKeyObjectMap;

    Int32 mDisplayFlags;
    Int32 mBackgroundColorRGBA;
    Int32 mHighlightColorRGBA;
    Int32 mScrollDelay;
    Int32 mWrapText;

    AutoPtr<IList> mBlinkingPosList;
    AutoPtr<IList> mHighlightPosList;
    AutoPtr<IList> mKaraokeList;
    AutoPtr<IList> mFontList;
    AutoPtr<IList> mStyleList;
    AutoPtr<IList> mHyperTextList;

    AutoPtr<IRect> mTextBounds;
    String mTextChars;
    AutoPtr<Justification> mJustification;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CTIMEDTEXT_H__
