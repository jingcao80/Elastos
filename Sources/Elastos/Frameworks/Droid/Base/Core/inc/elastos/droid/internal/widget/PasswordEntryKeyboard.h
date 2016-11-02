
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARD_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARD_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/inputmethodservice/Keyboard.h"

using Elastos::Droid::InputMethodService::Keyboard;

/**
 * A basic, embed-able keyboard designed for password entry. Allows entry of all Latin-1 characters.
 *
 * It has two modes: alpha and numeric. In alpha mode, it allows all Latin-1 characters and enables
 * an additional keyboard with symbols.  In numeric mode, it shows a 12-key DTMF dialer-like
 * keypad with alpha characters hints.
 */
namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class PasswordEntryKeyboard
    : public Keyboard
    , public IPasswordEntryKeyboard
{
public:
    class LatinKey
        : public Keyboard::Key
        , public IPasswordEntryKeyboardLatinKey
    {
    public:
        CAR_INTERFACE_DECL()

        LatinKey(
            /* [in] */ IResources* res,
            /* [in] */ Row* parent,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [in] */ IXmlResourceParser* parser);

        CARAPI_(void) SetEnabled(
            /* [in] */ Boolean enabled);

        CARAPI_(void) EnableShiftLock();

        CARAPI OnReleased(
            /* [in] */ Boolean inside);

        /**
         * Overriding this method so that we can reduce the target area for certain keys.
         */
        CARAPI IsInside(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [out] */ Boolean* isInside);

    private:
        Boolean mShiftLockEnabled;
        Boolean mEnabled;
    };

public:
    CAR_INTERFACE_DECL()

    PasswordEntryKeyboard();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 xmlLayoutResId);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 xmlLayoutResId,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 xmlLayoutResId,
        /* [in] */ Int32 mode);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 xmlLayoutResId,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 layoutTemplateResId,
        /* [in] */ ICharSequence* characters,
        /* [in] */ Int32 columns,
        /* [in] */ Int32 horizontalPadding);

    /**
     * Allows enter key resources to be overridden
     * @param res resources to grab given items from
     * @param previewId preview drawable shown on enter key
     * @param iconId normal drawable shown on enter key
     * @param labelId string shown on enter key
     */
    CARAPI SetEnterKeyResources(
        /* [in] */ IResources* res,
        /* [in] */ Int32 previewId,
        /* [in] */ Int32 iconId,
        /* [in] */ Int32 labelId);

    /**
     * Allows shiftlock to be turned on.  See {@link #setShiftLocked(Boolean)}
     *
     */
    CARAPI EnableShiftLock();

    /**
     * Turn on shift lock. This turns on the LED for this key, if it has one.
     * It should be followed by a call to {@link KeyboardView#invalidateKey(Int32)}
     * or {@link KeyboardView#invalidateAllKeys()}
     *
     * @param shiftLocked
     */
    CARAPI SetShiftLocked(
        /* [in] */ Boolean shiftLocked);

    /**
     * Turn on shift mode. Sets shift mode and turns on icon for shift key.
     * It should be followed by a call to {@link KeyboardView#invalidateKey(Int32)}
     * or {@link KeyboardView#invalidateAllKeys()}
     *
     * @param shiftLocked
     */
    CARAPI SetShifted(
        /* [in] */ Boolean shiftState,
        /* [out] */ Boolean* rst);

    /**
     * Whether or not keyboard is shifted.
     * @return TRUE if keyboard state is shifted.
     */
    CARAPI IsShifted(
        /* [out] */ Boolean* rst);

    virtual CARAPI_(AutoPtr<Key>) CreateKeyFromXml(
        /* [in] */ IResources* res,
        /* [in] */ Row* parent,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IXmlResourceParser* parser);

private:
    CARAPI_(void) Init(
        /* [in] */ IContext* context);

public:
    static const Int32 KEYCODE_SPACE;

    static Int32 sSpacebarVerticalCorrection;

private:
    static const String TAG;
    static const Int32 SHIFT_OFF;
    static const Int32 SHIFT_ON;
    static const Int32 SHIFT_LOCKED;

    AutoPtr<IDrawable> mShiftIcon;
    AutoPtr<IDrawable> mShiftLockIcon;

    // These two arrays must be the same length
    AutoPtr<ArrayOf<IDrawable*> > mOldShiftIcons;// = { NULL, NULL };
    AutoPtr<ArrayOf<Key*> > mShiftKeys;// = { NULL, NULL };

    AutoPtr<Key> mEnterKey;
    AutoPtr<Key> mF1Key;
    AutoPtr<Key> mSpaceKey;
    Int32 mShiftState;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARD_H__
