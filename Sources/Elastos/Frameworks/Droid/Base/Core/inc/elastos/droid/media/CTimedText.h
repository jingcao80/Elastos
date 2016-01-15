
#ifndef __ELASTOS_DROID_MEDIA_CTIMEDTEXT_H__
#define __ELASTOS_DROID_MEDIA_CTIMEDTEXT_H__

#include "_Elastos_Droid_Media_CTimedText.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/Set.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Utility::Set;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {

template<typename T>
class MyList
    : public IInterface
    , public List<T>
{
public:
    UInt32 AddRef()
    {
        return ElLightRefBase::AddRef();
    }

    UInt32 Release()
    {
        return ElLightRefBase::Release();
    }

    PInterface Probe(
        /* [in] */ REIID riid)
    {
        if (riid == EIID_IInterface) {
            return (IInterface*)this;
        }
        return NULL;
    }

    ECode GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID)
    {
        assert(0);
        return NOERROR;
    }
};



CarClass(CTimedText)
{
public:
    /**
     * Helper class to hold the start char offset and end char offset
     * for Blinking Text or Highlight Text. endChar is the end offset
     * of the text (startChar + number of characters to be highlighted
     * or blinked). The member variables in this class are read-only.
     * {@hide}
     */
    /*static*/ class CharPos : public ElRefBase
    {
    public:
        /**
         * Constuctor
         * @param startChar the offset of the start character.
         * @param endChar the offset of the end character.
         */
        CharPos(
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
    /*static*/ class Justification
        : public IInterface
        , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Constructor
         * @param horizontal the horizontal justification of the text.
         * @param vertical the vertical justification of the text.
         */
        Justification(
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
    /*static*/ class Style : public ElRefBase
    {
    public:
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
        Style(
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
    /*static*/ class Font : public ElRefBase
    {
    public:
        /**
         * Constructor
         * @param id the font ID.
         * @param name the font name.
         */
        Font(
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
    /*static*/ class Karaoke : public ElRefBase
    {
    public:
        /**
         * Constructor
         * @param startTimeMs the start time (in milliseconds) to highlight
         * the characters between startChar and endChar.
         * @param endTimeMs the end time (in milliseconds) to highlight
         * the characters between startChar and endChar.
         * @param startChar the offset of the start character to be highlighted.
         * @param endChar the offset of the end character to be highlighted.
         */
        Karaoke(
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
    /*static*/ class HyperText : public ElRefBase
    {
    public:
        /**
         * Constructor
         * @param startChar the offset of the start character.
         * @param endChar the offset of the end character.
         * @param url the linked-to URL.
         * @param alt the "alt" string for display.
         */
        HyperText(
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
    CARAPI_(AutoPtr<Set<Int32> >) KeySet();

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
    static const Int32 FIRST_PUBLIC_KEY = 1;

    // These keys must be in sync with the keys in TextDescription.h
    static const Int32 KEY_DISPLAY_FLAGS = 1; // int
    static const Int32 KEY_STYLE_FLAGS = 2; // int
    static const Int32 KEY_BACKGROUND_COLOR_RGBA = 3; // int
    static const Int32 KEY_HIGHLIGHT_COLOR_RGBA = 4; // int
    static const Int32 KEY_SCROLL_DELAY = 5; // int
    static const Int32 KEY_WRAP_TEXT = 6; // int
    static const Int32 KEY_START_TIME = 7; // int
    static const Int32 KEY_STRUCT_BLINKING_TEXT_LIST = 8; // List<CharPos>
    static const Int32 KEY_STRUCT_FONT_LIST = 9; // List<Font>
    static const Int32 KEY_STRUCT_HIGHLIGHT_LIST = 10; // List<CharPos>
    static const Int32 KEY_STRUCT_HYPER_TEXT_LIST = 11; // List<HyperText>
    static const Int32 KEY_STRUCT_KARAOKE_LIST = 12; // List<Karaoke>
    static const Int32 KEY_STRUCT_STYLE_LIST = 13; // List<Style>
    static const Int32 KEY_STRUCT_TEXT_POS = 14; // TextPos
    static const Int32 KEY_STRUCT_JUSTIFICATION = 15; // Justification
    static const Int32 KEY_STRUCT_TEXT = 16; // Text
    static const Int32 LAST_PUBLIC_KEY = 16;
    static const Int32 FIRST_PRIVATE_KEY = 101;

    // The following keys are used between TimedText.java and
    // TextDescription.cpp in order to parce the Parcel.
    static const Int32 KEY_GLOBAL_SETTING = 101;
    static const Int32 KEY_LOCAL_SETTING = 102;
    static const Int32 KEY_START_CHAR = 103;
    static const Int32 KEY_END_CHAR = 104;
    static const Int32 KEY_FONT_ID = 105;
    static const Int32 KEY_FONT_SIZE = 106;
    static const Int32 KEY_TEXT_COLOR_RGBA = 107;
    static const Int32 LAST_PRIVATE_KEY = 107;

    static const String TAG; // = "TimedText";

    HashMap<Int32, AutoPtr<IInterface> > mKeyObjectMap;

    Int32 mDisplayFlags; // = -1;
    Int32 mBackgroundColorRGBA; // = -1;
    Int32 mHighlightColorRGBA; // = -1;
    Int32 mScrollDelay; // = -1;
    Int32 mWrapText; // = -1;

    typedef MyList< AutoPtr<CharPos> > CharPostList;
    AutoPtr<CharPostList> mBlinkingPosList; // = NULL;
    AutoPtr<CharPostList> mHighlightPosList; // = NULL;
    AutoPtr< MyList< AutoPtr<Karaoke> > > mKaraokeList; // = NULL;
    AutoPtr< MyList< AutoPtr<Font> > > mFontList; // = NULL;
    AutoPtr< MyList< AutoPtr<Style> > > mStyleList; // = NULL;
    AutoPtr< MyList< AutoPtr<HyperText> > > mHyperTextList; // = NULL;

    AutoPtr<IRect> mTextBounds; // = NULL;
    String mTextChars; // = NULL;
    AutoPtr<Justification> mJustification;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CTIMEDTEXT_H__
