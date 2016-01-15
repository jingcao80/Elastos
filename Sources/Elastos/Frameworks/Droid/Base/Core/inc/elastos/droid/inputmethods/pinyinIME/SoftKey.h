
#ifndef  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SOFTKEY_H__
#define  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SOFTKEY_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Inputmethods::PinyinIME::ISoftKey;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

class SoftKeyType;
class SoftKeyboard;

/**
 * Class for soft keys which defined in the keyboard xml file. A soft key can be
 * a basic key or a toggling key.
 *
 * @see com.android.inputmethod.pinyin.SoftKeyToggle
 */
class SoftKey
    : public Object
    , public ISoftKey
{
public:
    CAR_INTERFACE_DECL();

    SoftKey();

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    virtual  CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    void SetKeyType(
        /* [in] */ SoftKeyType* keyType,
        /* [in] */ IDrawable* keyIcon,
        /* [in] */ IDrawable* keyIconPopup);

    // The caller guarantees that all parameters are in [0, 1]
    void SetKeyDimensions(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom);

    void SetKeyAttribute(
        /* [in] */ Int32 keyCode,
        /* [in] */ const String& label,
        /* [in] */ Boolean repeat,
        /* [in] */ Boolean balloon);

    void SetPopupSkbId(
        /* [in] */ Int32 popupSkbId);

    // Call after setKeyDimensions(). The caller guarantees that the
    // keyboard with and height are valid.
    void SetSkbCoreSize(
        /* [in] */ Int32 skbWidth,
        /* [in] */ Int32 skbHeight);

    virtual AutoPtr<IDrawable> GetKeyIcon();

    virtual AutoPtr<IDrawable> GetKeyIconPopup();

    virtual CARAPI GetKeyCode(
        /* [out] */ Int32* code);

    virtual CARAPI GetKeyLabel(
        /* [out] */ String* label);

    virtual void ChangeCase(
        /* [in] */ Boolean upperCase);

    virtual AutoPtr<IDrawable> GetKeyBg();

    virtual AutoPtr<IDrawable> GetKeyHlBg();

    virtual Int32 GetColor();

    virtual Int32 GetColorHl();

    virtual Int32 GetColorBalloon();

    virtual CARAPI IsKeyCodeKey(
        /* [out] */ Boolean* result);

    virtual CARAPI IsUserDefKey(
        /* [out] */ Boolean* result);

    virtual CARAPI IsUniStrKey(
        /* [out] */ Boolean* result);

    virtual Boolean NeedBalloon();

    virtual Boolean Repeatable();

    Int32 GetPopupResId();

    Int32 GetWidth();

    Int32 GetHeight();

    Boolean MoveWithinKey(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    // @Override
    // public String toString() {
    //     String str = "\n";
    //     str += "  keyCode: " + String.valueOf(mKeyCode) + "\n";
    //     str += "  keyMask: " + String.valueOf(mKeyMask) + "\n";
    //     str += "  keyLabel: " + (mKeyLabel == NULL ? "NULL" : mKeyLabel) + "\n";
    //     str += "  popupResId: " + String.valueOf(mPopupSkbId) + "\n";
    //     str += "  Position: " + String.valueOf(mLeftF) + ", "
    //             + String.valueOf(mTopF) + ", " + String.valueOf(mRightF) + ", "
    //             + String.valueOf(mBottomF) + "\n";
    //     return str;
    // }

    CARAPI ToString(
        /* [out] */ String* info);

public:
    /**
     * For a finger touch device, after user presses a key, there will be some
     * consequent moving events because of the changing in touching pressure. If
     * the moving distance in x is within this threshold, the moving events will
     * be ignored.
     */
    static const Int32 MAX_MOVE_TOLERANCE_X;

    /**
     * For a finger touch device, after user presses a key, there will be some
     * consequent moving events because of the changing in touching pressure. If
     * the moving distance in y is within this threshold, the moving events will
     * be ignored.
     */
    static const Int32 MAX_MOVE_TOLERANCE_Y;

    /**
     * If this value is not 0, this key can be used to popup a sub soft keyboard
     * when user presses it for some time.
     */
    Int32 mPopupSkbId;

    Float mLeftF;
    Float mRightF;
    Float mTopF;
    Float mBottomF;
    Int32 mLeft;
    Int32 mRight;
    Int32 mTop;
    Int32 mBottom;

protected:
    static const Int32 KEYMASK_REPEAT;
    static const Int32 KEYMASK_BALLOON;

    /**
     * Used to indicate the type and attributes of this key. the lowest 8 bits
     * should be reserved for SoftkeyToggle.
     */
    Int32 mKeyMask;

    AutoPtr<SoftKeyType> mKeyType;

    AutoPtr<IDrawable> mKeyIcon;

    AutoPtr<IDrawable> mKeyIconPopup;

    String mKeyLabel;

    Int32 mKeyCode;

private:
    friend class SoftKeyboard;
    friend class SkbContainer;
    friend class SoftKeyboardView;
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SOFTKEY_H__
