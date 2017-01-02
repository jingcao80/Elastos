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

#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_KEYBOARD_H__
#define __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_KEYBOARD_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.InputMethodService.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::InputMethodService::IKeyboard;
using Elastos::Core::Object;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

/**
 * Loads an XML description of a keyboard and stores the attributes of the keys. A keyboard
 * consists of rows of keys.
 * <p>The layout file for a keyboard contains XML that looks like the following snippet:</p>
 * <pre>
 * &lt;Keyboard
 *         android:keyWidth="%10p"
 *         android:keyHeight="50px"
 *         android:horizontalGap="2px"
 *         android:verticalGap="2px" &gt;
 *     &lt;Row android:keyWidth="32px" &gt;
 *         &lt;Key android:keyLabel="A" /&gt;
 *         ...
 *     &lt;/Row&gt;
 *     ...
 * &lt;/Keyboard&gt;
 * </pre>
 * @attr ref android.R.styleable#Keyboard_keyWidth
 * @attr ref android.R.styleable#Keyboard_keyHeight
 * @attr ref android.R.styleable#Keyboard_horizontalGap
 * @attr ref android.R.styleable#Keyboard_verticalGap
 */
class Keyboard
    : public Object
    , public IKeyboard
{
public:
    class Key;
    /**
     * Container for keys in the keyboard. All keys in a row are at the same Y-coordinate.
     * Some of the key size defaults can be overridden per row from what the {@link Keyboard}
     * defines.
     * @attr ref android.R.styleable#Keyboard_keyWidth
     * @attr ref android.R.styleable#Keyboard_keyHeight
     * @attr ref android.R.styleable#Keyboard_horizontalGap
     * @attr ref android.R.styleable#Keyboard_verticalGap
     * @attr ref android.R.styleable#Keyboard_Row_rowEdgeFlags
     * @attr ref android.R.styleable#Keyboard_Row_keyboardMode
     */
    class Row
        : public Object
        , public IKeyboardRow
    {
    public:
        CAR_INTERFACE_DECL()

        Row(
            /* [in] */ Keyboard* parent);

        Row(
            /* [in] */ IResources* res,
            /* [in] */ Keyboard* parent,
            /* [in] */ IXmlResourceParser* parser);

        virtual CARAPI GetDefaultWidth(
            /* [out] */ Int32* value);

        virtual CARAPI SetDefaultWidth(
            /* [in] */ Int32 value);

        virtual CARAPI GetDefaultHeight(
            /* [out] */ Int32* value);

        virtual CARAPI SetDefaultHeight(
            /* [in] */ Int32 value);

        virtual CARAPI GetDefaultHorizontalGap(
            /* [out] */ Int32* value);

        virtual CARAPI SetDefaultHorizontalGap(
            /* [in] */ Int32 value);

        virtual CARAPI GetVerticalGap(
            /* [out] */ Int32* value);

        virtual CARAPI SetVerticalGap(
            /* [in] */ Int32 value);

        virtual CARAPI GetRowEdgeFlags(
            /* [out] */ Int32* value);

        virtual CARAPI SetRowEdgeFlags(
            /* [in] */ Int32 value);

        virtual CARAPI GetMode(
            /* [out] */ Int32* value);

        virtual CARAPI SetMode(
            /* [in] */ Int32 value);

    public:
        /** Default width of a key in this row. */
        Int32 mDefaultWidth;
        /** Default height of a key in this row. */
        Int32 mDefaultHeight;
        /** Default horizontal gap between keys in this row. */
        Int32 mDefaultHorizontalGap;
        /** Vertical gap following this row. */
        Int32 mVerticalGap;

        AutoPtr<IArrayList> mKeys;// List<<Key>

        /**
         * Edge flags for this row of keys. Possible values that can be assigned are
         * {@link Keyboard#EDGE_TOP EDGE_TOP} and {@link Keyboard#EDGE_BOTTOM EDGE_BOTTOM}
         */
        Int32 mRowEdgeFlags;

        /** The keyboard mode for this row */
        Int32 mMode;

        Keyboard* mParent;
    };

    /**
     * Class for describing the position and characteristics of a single key in the keyboard.
     *
     * @attr ref android.R.styleable#Keyboard_keyWidth
     * @attr ref android.R.styleable#Keyboard_keyHeight
     * @attr ref android.R.styleable#Keyboard_horizontalGap
     * @attr ref android.R.styleable#Keyboard_Key_codes
     * @attr ref android.R.styleable#Keyboard_Key_keyIcon
     * @attr ref android.R.styleable#Keyboard_Key_keyLabel
     * @attr ref android.R.styleable#Keyboard_Key_iconPreview
     * @attr ref android.R.styleable#Keyboard_Key_isSticky
     * @attr ref android.R.styleable#Keyboard_Key_isRepeatable
     * @attr ref android.R.styleable#Keyboard_Key_isModifier
     * @attr ref android.R.styleable#Keyboard_Key_popupKeyboard
     * @attr ref android.R.styleable#Keyboard_Key_popupCharacters
     * @attr ref android.R.styleable#Keyboard_Key_keyOutputText
     * @attr ref android.R.styleable#Keyboard_Key_keyEdgeFlags
     */
    class Key
        : public Object
        , public IKeyboardKey
    {
    public:
        CAR_INTERFACE_DECL()

        /** Create an empty key with no attributes. */
        Key(
            /* [in] */ Row* parent);

        /** Create a key with the given top-left coordinate and extract its attributes from
         * the XML parser.
         * @param res resources associated with the caller's context
         * @param parent the row that this key belongs to. The row must already be attached to
         * a {@link Keyboard}.
         * @param x the x coordinate of the top-left
         * @param y the y coordinate of the top-left
         * @param parser the XML parser containing the attributes for this key
         */
        Key(
            /* [in] */ IResources* res,
            /* [in] */ Row* parent,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [in] */ IXmlResourceParser* parser);

        virtual ~Key();

        /**
         * Informs the key that it has been pressed, in case it needs to change its appearance or
         * state.
         * @see #onReleased(Boolean)
         */
        virtual CARAPI OnPressed();

        /**
         * Changes the pressed state of the key. If it is a sticky key, it will also change the
         * toggled state of the key if the finger was release inside.
         * @param inside whether the finger was released inside the key
         * @see #onPressed()
         */
        virtual CARAPI OnReleased(
            /* [in] */ Boolean inside);

        /* package */ virtual CARAPI_(AutoPtr< ArrayOf<Int32> >) ParseCSV(
            /* [in] */ const String& value);

        /**
         * Detects if a point falls inside this key.
         * @param x the x-coordinate of the point
         * @param y the y-coordinate of the point
         * @return whether or not the point falls inside the key. If the key is attached to an edge,
         * it will assume that all points between the key and the edge are considered to be inside
         * the key.
         */
        virtual CARAPI IsInside(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [out] */ Boolean* isInside);

        /**
         * Returns the square of the distance between the center of the key and the given point.
         * @param x the x-coordinate of the point
         * @param y the y-coordinate of the point
         * @return the square of the distance of the point from the center of the key
         */
        virtual CARAPI SquaredDistanceFrom(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [out] */ Int32* dis);

        /**
         * Returns the drawable state for the key, based on the current state and type of the key.
         * @return the drawable state of the key.
         * @see android.graphics.drawable.StateListDrawable#setState(Int32[])
         */
        virtual CARAPI GetCurrentDrawableState(
            /* [out, callee] */ ArrayOf<Int32>** drawableStates);

        virtual CARAPI GetCodes(
            /* [out, callee] */ ArrayOf<Int32>** codes);

        virtual CARAPI SetCodes(
            /* [in] */ ArrayOf<Int32>* codes);

        virtual CARAPI GetLabel(
            /* [in] */ ICharSequence** label);

        virtual CARAPI SetLabel(
            /* [in] */ ICharSequence* label);

        virtual CARAPI GetIcon(
            /* [out] */ IDrawable** icon);

        virtual CARAPI SetIcon(
            /* [in] */ IDrawable* icon);

        virtual CARAPI GetIconPreview(
            /* [out] */ IDrawable** preView);

        virtual CARAPI SetIconPreview(
            /* [in] */ IDrawable* preView);

        virtual CARAPI GetWidth(
            /* [out] */ Int32* value);

        virtual CARAPI SetWidth(
            /* [in] */ Int32 value);

        virtual CARAPI GetHeight(
            /* [out] */ Int32* value);

        virtual CARAPI SetHeight(
            /* [in] */ Int32 value);

        virtual CARAPI GetGap(
            /* [out] */ Int32* value);

        virtual CARAPI SetGap(
            /* [in] */ Int32 value);

        virtual CARAPI IsSticky(
            /* [out] */ Boolean* value);

        virtual CARAPI SetSticky(
            /* [in] */ Boolean value);

        virtual CARAPI GetX(
            /* [out] */ Int32* value);

        virtual CARAPI SetX(
            /* [in] */ Int32 value);

        virtual CARAPI GetY(
            /* [out] */ Int32* value);

        virtual CARAPI SetY(
            /* [in] */ Int32 value);

        virtual CARAPI GetText(
            /* [out] */ ICharSequence** text);

        virtual CARAPI GetPopupCharacters(
            /* [out] */ ICharSequence** popupCharacters);

        virtual CARAPI GetPopupResId(
            /* [out] */ Int32* popupResId);

        virtual CARAPI IsRepeatable(
            /* [out] */ Boolean* repeatable);

    public:
        /**
         * All the key codes (unicode or custom code) that this key could generate, zero'th
         * being the most important.
         */
        AutoPtr< ArrayOf<Int32> > mCodes;

        /** Label to display */
        AutoPtr<ICharSequence> mLabel;

        /** Icon to display instead of a label. Icon takes precedence over a label */
        AutoPtr<IDrawable> mIcon;
        /** Preview version of the icon, for the preview popup */
        AutoPtr<IDrawable> mIconPreview;
        /** Width of the key, not including the gap */
        Int32 mWidth;
        /** Height of the key, not including the gap */
        Int32 mHeight;
        /** The horizontal gap before this key */
        Int32 mGap;
        /** Whether this key is sticky, i.e., a toggle key */
        Boolean mSticky;
        /** X coordinate of the key in the keyboard layout */
        Int32 mX;
        /** Y coordinate of the key in the keyboard layout */
        Int32 mY;
        /** The current pressed state of this key */
        Boolean mPressed;
        /** If this is a sticky key, is it on? */
        Boolean mOn;
        /** Text to output when pressed. This can be multiple characters, like ".com" */
        AutoPtr<ICharSequence> mText;
        /** Popup characters */
        AutoPtr<ICharSequence> mPopupCharacters;

        /**
         * Flags that specify the anchoring to edges of the keyboard for detecting touch events
         * that are just out of the boundary of the key. This is a bit mask of
         * {@link Keyboard#EDGE_LEFT}, {@link Keyboard#EDGE_RIGHT}, {@link Keyboard#EDGE_TOP} and
         * {@link Keyboard#EDGE_BOTTOM}.
         */
        Int32 mEdgeFlags;
        /** Whether this is a modifier key, such as Shift or Alt */
        Boolean mModifier;
        /**
         * If this key pops up a mini keyboard, this is the resource id for the XML layout for that
         * keyboard.
         */
        Int32 mPopupResId;
        /** Whether this key repeats itself when held down */
        Boolean mRepeatable;

    private:
        /** The keyboard that this key belongs to */
        Keyboard* mKeyboard;

        static Int32 KEY_STATE_NORMAL_ON[];
        static Int32 KEY_STATE_PRESSED_ON[];
        static Int32 KEY_STATE_NORMAL_OFF[];
        static Int32 KEY_STATE_PRESSED_OFF[];
        static Int32 KEY_STATE_NORMAL[];
        static Int32 KEY_STATE_PRESSED[];
    };

public:
    CAR_INTERFACE_DECL()

    Keyboard();

    /**
     * Creates a keyboard from the given xml key layout file.
     * @param context the application or service context
     * @param xmlLayoutResId the resource file that contains the keyboard layout and keys.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 xmlLayoutResId);

    /**
     * Creates a keyboard from the given xml key layout file. Weeds out rows
     * that have a keyboard mode defined but don't match the specified mode.
     * @param context the application or service context
     * @param xmlLayoutResId the resource file that contains the keyboard layout and keys.
     * @param modeId keyboard mode identifier
     * @param width sets width of keyboard
     * @param height sets height of keyboard
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 xmlLayoutResId,
        /* [in] */ Int32 modeId,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Creates a keyboard from the given xml key layout file. Weeds out rows
     * that have a keyboard mode defined but don't match the specified mode.
     * @param context the application or service context
     * @param xmlLayoutResId the resource file that contains the keyboard layout and keys.
     * @param modeId keyboard mode identifier
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 xmlLayoutResId,
        /* [in] */ Int32 modeId);

    /**
     * <p>Creates a blank keyboard from the given resource file and populates it with the specified
     * characters in left-to-right, top-to-bottom fashion, using the specified number of columns.
     * </p>
     * <p>If the specified number of columns is -1, then the keyboard will fit as many keys as
     * possible in each row.</p>
     * @param context the application or service context
     * @param layoutTemplateResId the layout template file, containing no keys.
     * @param characters the list of characters to display on the keyboard. One key will be created
     * for each character.
     * @param columns the number of columns of keys to display. If this number is greater than the
     * number of keys that can fit in a row, it will be ignored. If this number is -1, the
     * keyboard will fit as many keys as possible in each row.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layoutTemplateResId,
        /* [in] */ ICharSequence* characters,
        /* [in] */ Int32 columns,
        /* [in] */ Int32 horizontalPadding);

    virtual ~Keyboard();

    virtual CARAPI_(void) Resize(
        /* [in] */ Int32 newWidth,
        /* [in] */ Int32 newHeight);

    virtual CARAPI GetKeys(
        /* [out] */ IList** keys);

    virtual CARAPI GetModifierKeys(
        /* [out] */ IList** keys);

    /**
     * Returns the total height of the keyboard
     * @return the total height of the keyboard
     */
    virtual CARAPI GetHeight(
        /* [out] */ Int32* h);

    virtual CARAPI GetMinWidth(
        /* [out] */ Int32* minW);

    virtual CARAPI SetShifted(
        /* [in] */ Boolean shiftState,
        /* [out] */ Boolean* res);

    virtual CARAPI IsShifted(
        /* [out] */ Boolean* res);

    /**
     * @hide
     */
    virtual CARAPI GetShiftKeyIndices(
        /* [out, callee] */ ArrayOf<Int32>** keyIndices);

    virtual CARAPI GetShiftKeyIndex(
        /* [out] */ Int32* index);

    /**
     * Returns the indices of the keys that are closest to the given point.
     * @param x the x-coordinate of the point
     * @param y the y-coordinate of the point
     * @return the array of integer indices for the nearest keys to the given point. If the given
     * point is out of range, then an array of size zero is returned.
     */
    virtual CARAPI GetNearestKeys(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out, callee] */ ArrayOf<Int32>** nearestKeys);

    static CARAPI_(Int32) GetDimensionOrFraction(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 index,
        /* [in] */ Int32 base,
        /* [in] */ Int32 defValue);

protected:
    virtual CARAPI GetHorizontalGap(
        /* [out] */ Int32* hGap);

    virtual CARAPI SetHorizontalGap(
        /* [in] */ Int32 gap);

    virtual CARAPI GetVerticalGap(
        /* [out] */ Int32* vGap);

    virtual CARAPI SetVerticalGap(
        /* [in] */ Int32 gap);

    virtual CARAPI GetKeyHeight(
        /* [out] */ Int32* h);

    virtual CARAPI SetKeyHeight(
        /* [in] */ Int32 height);

    virtual CARAPI GetKeyWidth(
        /* [out] */ Int32* w);

    virtual CARAPI SetKeyWidth(
        /* [in] */ Int32 width);

    virtual CARAPI_(AutoPtr<Row>) CreateRowFromXml(
        /* [in] */ IResources* res,
        /* [in] */ IXmlResourceParser* parser);

    virtual CARAPI_(AutoPtr<Key>) CreateKeyFromXml(
        /* [in] */ IResources* res,
        /* [in] */ Row* parent,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IXmlResourceParser* parser);

private:
    CARAPI_(void) ComputeNearestNeighbors();

    CARAPI_(void) LoadKeyboard(
        /* [in] */ IContext* context,
        /* [in] */ IXmlResourceParser* parser);

    //throws XmlPullParserException, IOException
    CARAPI_(void) SkipToEndOfRow(
        /* [in] */ IXmlResourceParser* parser);

    CARAPI_(void) ParseKeyboardAttributes(
        /* [in] */ IResources* res,
        /* [in] */ IXmlResourceParser* parser);

private:
    static const String TAG;

    // Keyboard XML Tags
    static const String TAG_KEYBOARD;
    static const String TAG_ROW;
    static const String TAG_KEY;

    /** Keyboard label **/
    AutoPtr<ICharSequence> mLabel;

    /** Horizontal gap default for all rows */
    Int32 mDefaultHorizontalGap;

    /** Default key width */
    Int32 mDefaultWidth;

    /** Default key height */
    Int32 mDefaultHeight;

    /** Default gap between rows */
    Int32 mDefaultVerticalGap;

    /** Is the keyboard in the shifted state */
    Boolean mShifted;

    /** Key instance for the shift key, if present */
    AutoPtr< ArrayOf<Key*> > mShiftKeys;

    /** Key index for the shift key, if present */
    AutoPtr< ArrayOf<Int32> > mShiftKeyIndices;

    /** Current key width, while loading the keyboard */
    Int32 mKeyWidth;

    /** Current key height, while loading the keyboard */
    Int32 mKeyHeight;

    /** Total height of the keyboard, including the padding and keys */
    Int32 mTotalHeight;

    /**
     * Total width of the keyboard, including left side gaps and keys, but not any gaps on the
     * right side.
     */
    Int32 mTotalWidth;

    /** List of keys in this keyboard */
    AutoPtr<IList> mKeys;// List<Key>

    /** List of modifier keys such as Shift & Alt, if any */
    AutoPtr<IList> mModifierKeys; //List<Key>

    /** Width of the screen available to fit the keyboard */
    Int32 mDisplayWidth;

    /** Height of the screen */
    Int32 mDisplayHeight;

    /** Keyboard mode, or zero, if none.  */
    Int32 mKeyboardMode;

    // Variables for pre-computing nearest keys.

    static const Int32 GRID_WIDTH;
    static const Int32 GRID_HEIGHT;
    static const Int32 GRID_SIZE;
    Int32 mCellWidth;
    Int32 mCellHeight;
    AutoPtr<ArrayOf< AutoPtr<ArrayOf<Int32> > > > mGridNeighbors;
    Int32 mProximityThreshold;
    /** Number of key widths from current touch point to search for nearest keys. */
    static Float SEARCH_DISTANCE;

    AutoPtr<IArrayList> mRows; //ArrayList<Row>

    friend class KeyboardView;
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::InputMethodService::Keyboard::Key, IInterface)

#endif  // __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_KEYBOARD_H__
