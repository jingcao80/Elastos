
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_XMLKEYBOARDLOADER_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_XMLKEYBOARDLOADER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/inputmethod/pinyin/SoftKeyToggle.h"
#include "elastos/droid/inputmethod/pinyin/SoftKeyboard.h"
#include "elastos/droid/inputmethod/pinyin/SkbTemplate.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Class used to load a soft keyboard or a soft keyboard template from xml
 * files.
 */
class XmlKeyboardLoader: public Object
{
public:
    class KeyCommonAttributes : public Object
    {
    public:
        KeyCommonAttributes(
            /* [in] */ IXmlResourceParser* xrp,
            /* [in] */ XmlKeyboardLoader* host);

        // Make sure the default object is not NULL.
        CARAPI_(Boolean) GetAttributes(
            /* [in] */ KeyCommonAttributes* defAttr);

    public:
        AutoPtr<IXmlResourceParser> mXrp;
        Int32 mKeyType;
        Float mKeyWidth;
        Float mKeyHeight;
        Boolean mRepeat;
        Boolean mBalloon;
        XmlKeyboardLoader* mHost;
    };

public:
    XmlKeyboardLoader(
        /* [in] */ IContext* context);

    CARAPI_(AutoPtr<SkbTemplate>) LoadSkbTemplate(
        /* [in] */ Int32 resourceId);

    CARAPI_(AutoPtr<SoftKeyboard>) LoadKeyboard(
        /* [in] */ Int32 resourceId,
        /* [in] */ Int32 skbWidth,
        /* [in] */ Int32 skbHeight);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    // Caller makes sure xrp and r are valid.
    CARAPI GetSoftKey(
        /* [in] */ IXmlResourceParser* xrp,
        /* [in] */ KeyCommonAttributes* attrKey,
        /* [out] */ SoftKey** key);

    CARAPI GetToggleStates(
        /* [in] */ KeyCommonAttributes* attrKey,
        /* [in] */ SoftKeyToggle* softKey,
        /* [in] */ Int32 defKeyCode,
        /* [out] */ SoftKeyToggle::ToggleState** state);

    CARAPI_(Int32) GetInteger(
        /* [in] */ IXmlResourceParser* xrp,
        /* [in] */ const String& name,
        /* [in] */ Int32 defValue);

    CARAPI_(Int32) GetColor(
        /* [in] */ IXmlResourceParser* xrp,
        /* [in] */ const String& name,
        /* [in] */ Int32 defValue);

    CARAPI_(String) GetString(
        /* [in] */ IXmlResourceParser* xrp,
        /* [in] */ const String& name,
        /* [in] */ const String& defValue);

    CARAPI_(Float) GetFloat(
        /* [in] */ IXmlResourceParser* xrp,
        /* [in] */ const String& name,
        /* [in] */ Float defValue);

    CARAPI_(Boolean) GetBoolean(
        /* [in] */ IXmlResourceParser* xrp,
        /* [in] */ const String& name,
        /* [in] */ Boolean defValue);

    CARAPI_(AutoPtr<IDrawable>) GetDrawable(
        /* [in] */ IXmlResourceParser* xrp,
        /* [in] */ const String& name,
        /* [in] */ IDrawable* defValue);

private:
    /**
     * The tag used to define an xml-based soft keyboard template.
     */
    static String XMLTAG_SKB_TEMPLATE;

    /**
     * The tag used to indicate the soft key type which is defined inside the
     * {@link #XMLTAG_SKB_TEMPLATE} element in the xml file. file.
     */
    static String XMLTAG_KEYTYPE;

    /**
     * The tag used to define a default key icon for enter/delete/space keys. It
     * is defined inside the {@link #XMLTAG_SKB_TEMPLATE} element in the xml
     * file.
     */
    static String XMLTAG_KEYICON;

    /**
     * Attribute tag of the left and right margin for a key. A key's width
     * should be larger than double of this value. Defined inside
     * {@link #XMLTAG_SKB_TEMPLATE} and {@link #XMLTAG_KEYBOARD}.
     */
    static String XMLATTR_KEY_XMARGIN;

    /**
     * Attribute tag of the top and bottom margin for a key. A key's height
     * should be larger than double of this value. Defined inside
     * {@link #XMLTAG_SKB_TEMPLATE} and {@link #XMLTAG_KEYBOARD}.
     */
    static String XMLATTR_KEY_YMARGIN;

    /**
     * Attribute tag of the keyboard background image. Defined inside
     * {@link #XMLTAG_SKB_TEMPLATE} and {@link #XMLTAG_KEYBOARD}.
     */
    static String XMLATTR_SKB_BG;

    /**
     * Attribute tag of the balloon background image for key press. Defined
     * inside {@link #XMLTAG_SKB_TEMPLATE} and {@link #XMLTAG_KEYBOARD}.
     */
    static String XMLATTR_BALLOON_BG;

    /**
     * Attribute tag of the popup balloon background image for key press or
     * popup mini keyboard. Defined inside {@link #XMLTAG_SKB_TEMPLATE} and
     * {@link #XMLTAG_KEYBOARD}.
     */
    static String XMLATTR_POPUP_BG;

    /**
     * Attribute tag of the color to draw key label. Defined inside
     * {@link #XMLTAG_SKB_TEMPLATE} and {@link #XMLTAG_KEYTYPE}.
     */
    static String XMLATTR_COLOR;

    /**
     * Attribute tag of the color to draw key's highlighted label. Defined
     * inside {@link #XMLTAG_SKB_TEMPLATE} and {@link #XMLTAG_KEYTYPE}.
     */
    static String XMLATTR_COLOR_HIGHLIGHT;

    /**
     * Attribute tag of the color to draw key's label in the popup balloon.
     * Defined inside {@link #XMLTAG_SKB_TEMPLATE} and {@link #XMLTAG_KEYTYPE}.
     */
    static String XMLATTR_COLOR_BALLOON;

    /**
     * Attribute tag of the id of {@link #XMLTAG_KEYTYPE} and
     * {@link #XMLTAG_KEY}. Key types and keys defined in a soft keyboard
     * template should have id, because a soft keyboard needs the id to refer to
     * these default definitions. If a key defined in {@link #XMLTAG_KEYBOARD}
     * does not id, that means the key is newly defined; if it has id (and only
     * has id), the id is used to find the default definition from the soft
     * keyboard template.
     */
    static String XMLATTR_ID;

    /**
     * Attribute tag of the key background for a specified key type. Defined
     * inside {@link #XMLTAG_KEYTYPE}.
     */
    static String XMLATTR_KEYTYPE_BG;

    /**
     * Attribute tag of the key high-light background for a specified key type.
     * Defined inside {@link #XMLTAG_KEYTYPE}.
     */
    static String XMLATTR_KEYTYPE_HLBG;

    /**
     * Attribute tag of the starting x-position of an element. It can be defined
     * in {@link #XMLTAG_ROW} and {@link #XMLTAG_KEY} in {XMLTAG_SKB_TEMPLATE}.
     * If not defined, 0 will be used. For a key defined in
     * {@link #XMLTAG_KEYBOARD}, it always use its previous keys information to
     * calculate its own position.
     */
    static String XMLATTR_START_POS_X;

    /**
     * Attribute tag of the starting y-position of an element. It can be defined
     * in {@link #XMLTAG_ROW} and {@link #XMLTAG_KEY} in {XMLTAG_SKB_TEMPLATE}.
     * If not defined, 0 will be used. For a key defined in
     * {@link #XMLTAG_KEYBOARD}, it always use its previous keys information to
     * calculate its own position.
     */
    static String XMLATTR_START_POS_Y;

    /**
     * Attribute tag of a row's id. Defined {@link #XMLTAG_ROW}. If not defined,
     * -1 will be used. Rows with id -1 will be enabled always, rows with same
     * row id will be enabled when the id is the same to the activated id of the
     * soft keyboard.
     */
    static String XMLATTR_ROW_ID;

    /** The tag used to indicate the keyboard element in the xml file. */
    static String XMLTAG_KEYBOARD;

    /** The tag used to indicate the row element in the xml file. */
    static String XMLTAG_ROW;

    /** The tag used to indicate key-array element in the xml file. */
    static String XMLTAG_KEYS;

    /**
     * The tag used to indicate a key element in the xml file. If the element is
     * defined in a soft keyboard template, it should have an id. If it is
     * defined in a soft keyboard, id is not required.
     */
    static String XMLTAG_KEY;

    /** The tag used to indicate a key's toggle element in the xml file. */
    static String XMLTAG_TOGGLE_STATE;

    /**
     * Attribute tag of the toggle state id for toggle key. Defined inside
     * {@link #XMLTAG_TOGGLE_STATE}
     */
    static String XMLATTR_TOGGLE_STATE_ID;

    /** Attribute tag of key template for the soft keyboard. */
    static String XMLATTR_SKB_TEMPLATE;

    /**
     * Attribute tag used to indicate whether this soft keyboard needs to be
     * cached in memory for future use. {@link #DEFAULT_SKB_CACHE_FLAG}
     * specifies the default value.
     */
    static String XMLATTR_SKB_CACHE_FLAG;

    /**
     * Attribute tag used to indicate whether this soft keyboard is sticky. A
     * sticky soft keyboard will keep the current layout unless user makes a
     * switch explicitly. A none sticky soft keyboard will automatically goes
     * back to the previous keyboard after click a none-function key.
     * {@link #DEFAULT_SKB_STICKY_FLAG} specifies the default value.
     */
    static String XMLATTR_SKB_STICKY_FLAG;

    /** Attribute tag to indicate whether it is a QWERTY soft keyboard. */
    static String XMLATTR_QWERTY;

    /**
     * When the soft keyboard is a QWERTY one, this attribute tag to get the
     * information that whether it is defined in upper case.
     */
    static String XMLATTR_QWERTY_UPPERCASE;

    /** Attribute tag of key type. */
    static String XMLATTR_KEY_TYPE;

    /** Attribute tag of key width. */
    static String XMLATTR_KEY_WIDTH;

    /** Attribute tag of key height. */
    static String XMLATTR_KEY_HEIGHT;

    /** Attribute tag of the key's repeating ability. */
    static String XMLATTR_KEY_REPEAT;

    /** Attribute tag of the key's behavior for balloon. */
    static String XMLATTR_KEY_BALLOON;

    /** Attribute tag of the key splitter in a key array. */
    static String XMLATTR_KEY_SPLITTER;

    /** Attribute tag of the key labels in a key array. */
    static String XMLATTR_KEY_LABELS;

    /** Attribute tag of the key codes in a key array. */
    static String XMLATTR_KEY_CODES;

    /** Attribute tag of the key label in a key. */
    static String XMLATTR_KEY_LABEL;

    /** Attribute tag of the key code in a key. */
    static String XMLATTR_KEY_CODE;

    /** Attribute tag of the key icon in a key. */
    static String XMLATTR_KEY_ICON;

    /** Attribute tag of the key's popup icon in a key. */
    static String XMLATTR_KEY_ICON_POPUP;

    /** The id for a mini popup soft keyboard. */
    static String XMLATTR_KEY_POPUP_SKBID;

    static Boolean DEFAULT_SKB_CACHE_FLAG;

    static Boolean DEFAULT_SKB_STICKY_FLAG;

    /**
     * The key type id for invalid key type. It is also used to generate next
     * valid key type id by adding 1.
     */
    static const Int32 KEYTYPE_ID_LAST;

    AutoPtr<IContext> mContext;

    AutoPtr<IResources> mResources;

    /** The event type in parsing the xml file. */
    Int32 mXmlEventType;

    /**
     * The current soft keyboard template used by the current soft keyboard
     * under loading.
     **/
    AutoPtr<SkbTemplate> mSkbTemplate;

    /** The x position for the next key. */
    Float mKeyXPos;

    /** The y position for the next key. */
    Float mKeyYPos;

    /** The width of the keyboard to load. */
    Int32 mSkbWidth;

    /** The height of the keyboard to load. */
    Int32 mSkbHeight;

    /** Key margin in x-way. */
    Float mKeyXMargin;

    /** Key margin in y-way. */
    Float mKeyYMargin;

    /**
     * Used to indicate whether next event has been fetched during processing
     * the the current event.
     */
    Boolean mNextEventFetched;

    String mAttrTmp;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_XMLKEYBOARDLOADER_H__
