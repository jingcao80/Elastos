
#ifndef  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SOFTKEYTOGGLE_H__
#define  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SOFTKEYTOGGLE_H__

#include "SoftKey.h"

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

/**
 * Class for soft keys which defined in the keyboard xml file. A soft key can be
 * a basic key or a toggling key.
 *
 * @see com.android.inputmethod.pinyin.SoftKey
 */
class SoftKeyToggle
    : public SoftKey
    , public ISoftKeyToggle
{
public:
    class ToggleState : public Object
    {
    public:
        ToggleState();

        void SetStateId(
            /* [in] */ Int32 stateId);

        void SetStateFlags(
            /* [in] */ Boolean repeat,
            /* [in] */ Boolean balloon);

    public:
        // The id should be bigger than 0;
        Int32 mIdAndFlags;
        AutoPtr<SoftKeyType> mKeyType;
        Int32 mKeyCode;
        AutoPtr<IDrawable> mKeyIcon;
        AutoPtr<IDrawable> mKeyIconPopup;
        String mKeyLabel;
        AutoPtr<ToggleState> mNextState;
    };

public:
    CAR_INTERFACE_DECL();

    Int32 GetToggleStateId();

    // The state id should be valid, and less than 255.
    // If resetIfNotFound is TRUE and there is no such toggle state with the
    // given id, the key state will be reset.
    // If the key state is newly changed (enabled to the given state, or
    // reseted) and needs re-draw, return TRUE.
    Boolean EnableToggleState(
        /* [in] */ Int32 stateId,
        /* [in] */ Boolean resetIfNotFound);

    // The state id should be valid, and less than 255.
    // If resetIfNotFound is TRUE and there is no such toggle state with the
    // given id, the key state will be reset.
    // If the key state is newly changed and needs re-draw, return TRUE.
    Boolean DisableToggleState(
        /* [in] */ Int32 stateId,
        /* [in] */ Boolean resetIfNotFound);

    // Clear any toggle state. If the key needs re-draw, return TRUE.
    Boolean DisableAllToggleStates();

    AutoPtr<IDrawable> GetKeyIcon();

    AutoPtr<IDrawable> GetKeyIconPopup();

    CARAPI GetKeyCode(
        /* [out] */ Int32* code);

    CARAPI GetKeyLabel(
        /* [out] */ String* label);

    AutoPtr<IDrawable> GetKeyBg();

    AutoPtr<IDrawable> GetKeyHlBg();

    Int32 GetColor();

    Int32 GetColorHl();

    Int32 GetColorBalloon();

    virtual CARAPI IsKeyCodeKey(
        /* [out] */ Boolean* result);

    virtual CARAPI IsUserDefKey(
        /* [out] */ Boolean* result);

    virtual CARAPI IsUniStrKey(
        /* [out] */ Boolean* result);

    Boolean NeedBalloon();

    Boolean Repeatable();

    void ChangeCase(
        /* [in] */ Boolean lowerCase);

    AutoPtr<ToggleState> CreateToggleState();

    Boolean SetToggleStates(
        /* [in] */ ToggleState* rootState);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    AutoPtr<ToggleState> GetToggleState();

private:
    /**
     * The current state number is stored in the lowest 8 bits of mKeyMask, this
     * mask is used to get the state number. If the current state is 0, the
     * normal state is enabled; if the current state is more than 0, a toggle
     * state in the toggle state chain will be enabled.
     */
    static const Int32 KEYMASK_TOGGLE_STATE;

    AutoPtr<ToggleState> mToggleState;
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODS_PINYINIME_SOFTKEYTOGGLE_H__
