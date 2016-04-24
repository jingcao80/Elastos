
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_INPUTMODESWITCHER_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_INPUTMODESWITCHER_H__

#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

class CPinyinIME;

/**
 * Switcher used to switching input mode between Chinese, English, symbol,etc.
 */
class InputModeSwitcher : public Object
{
public:
    class ToggleStates : public Object
    {
    public:
        ToggleStates();

    public:
        /**
         * If it is true, this soft keyboard is a QWERTY one.
         */
        Boolean mQwerty;

        /**
         * If {@link #mQwerty} is true, this variable is used to decide the
         * letter case of the QWERTY keyboard.
         */
        Boolean mQwertyUpperCase;

        /**
         * The id of enabled row in the soft keyboard. Refer to
         * {@link com.android.inputmethod.pinyin.SoftKeyboard.KeyRow} for
         * details.
         */
        Int32 mRowIdToEnable;

        /**
         * Used to store all other toggle states for the current input mode.
         */
        AutoPtr<ArrayOf<Int32> > mKeyStates;

        /**
         * Number of states to toggle.
         */
        Int32 mKeyStatesNum;
    };

public:
    InputModeSwitcher(
        /* [in] */ CPinyinIME* imeService);

    CARAPI_(Int32) GetInputMode();

    CARAPI_(AutoPtr<ToggleStates>) GetToggleStates();

    CARAPI_(Int32) GetSkbLayout();

    // Return the icon to update.
    CARAPI_(Int32) SwitchLanguageWithHkb();

    // Return the icon to update.
    CARAPI_(Int32) SwitchModeForUserKey(
        /* [in] */ Int32 userKey);

    // Return the icon to update.
    CARAPI_(Int32) RequestInputWithHkb(
        /* [in] */ IEditorInfo* editorInfo);

    // Return the icon to update.
    CARAPI_(Int32) RequestInputWithSkb(
        /* [in] */ IEditorInfo* editorInfo);

    // Return the icon to update.
    CARAPI_(Int32) RequestBackToPreviousSkb();

    CARAPI_(Int32) GetTooggleStateForCnCand();

    CARAPI_(Boolean) IsEnglishWithHkb();

    CARAPI_(Boolean) IsEnglishWithSkb();

    CARAPI_(Boolean) IsEnglishUpperCaseWithSkb();

    CARAPI_(Boolean) IsChineseText();

    CARAPI_(Boolean) IsChineseTextWithHkb();

    CARAPI_(Boolean) IsChineseTextWithSkb();

    CARAPI_(Boolean) IsSymbolWithSkb();

    CARAPI_(Boolean) IsEnterNoramlState();

    CARAPI_(Boolean) TryHandleLongPressSwitch(
        /* [in] */ Int32 keyCode);

private:
    CARAPI_(void) SaveInputMode(
        /* [in] */ Int32 newInputMode);

    CARAPI_(void) PrepareToggleStates(
        /* [in] */ Boolean needSkb);

public:
    /**
     * User defined key code, used by soft keyboard.
     */
    static const Int32 USERDEF_KEYCODE_PHONE_SYM_4;

    /**
     * Mode for inputing Chinese with soft keyboard.
     */
    static const Int32 MODE_SKB_CHINESE;

    /**
     * Mode for inputing basic symbols for Chinese mode with soft keyboard.
     */
    static const Int32 MODE_SKB_SYMBOL1_CN;

    /**
     * Mode for inputing more symbols for Chinese mode with soft keyboard.
     */
    static const Int32 MODE_SKB_SYMBOL2_CN;

    /**
     * Mode for inputing English lower characters with soft keyboard.
     */
    static const Int32 MODE_SKB_ENGLISH_LOWER;

    /**
     * Mode for inputing English upper characters with soft keyboard.
     */
    static const Int32 MODE_SKB_ENGLISH_UPPER;

    /**
     * Mode for inputing basic symbols for English mode with soft keyboard.
     */
    static const Int32 MODE_SKB_SYMBOL1_EN;

    /**
     * Mode for inputing more symbols for English mode with soft keyboard.
     */
    static const Int32 MODE_SKB_SYMBOL2_EN;

    /**
     * Mode for inputing smileys with soft keyboard.
     */
    static const Int32 MODE_SKB_SMILEY;

    /**
     * Mode for inputing phone numbers.
     */
    static const Int32 MODE_SKB_PHONE_NUM;

    /**
     * Mode for inputing phone numbers.
     */
    static const Int32 MODE_SKB_PHONE_SYM;

    /**
     * Mode for inputing Chinese with a hardware keyboard.
     */
    static const Int32 MODE_HKB_CHINESE;

    /**
     * Mode for inputing English with a hardware keyboard
     */
    static const Int32 MODE_HKB_ENGLISH;

    /**
     * Unset mode.
     */
    static const Int32 MODE_UNSET;

    /**
     * Maximum toggle states for a soft keyboard.
     */
    static const Int32 MAX_TOGGLE_STATES;

private:
    /**
     * User defined key code, used by soft keyboard.
     */
    static const Int32 USERDEF_KEYCODE_SHIFT_1;

    /**
     * User defined key code, used by soft keyboard.
     */
    static const Int32 USERDEF_KEYCODE_LANG_2;

    /**
     * User defined key code, used by soft keyboard.
     */
    static const Int32 USERDEF_KEYCODE_SYM_3;

    /**
     * User defined key code, used by soft keyboard.
     */
    static const Int32 USERDEF_KEYCODE_MORE_SYM_5;

    /**
     * User defined key code, used by soft keyboard.
     */
    static const Int32 USERDEF_KEYCODE_SMILEY_6;

    /**
     * Bits used to indicate soft keyboard layout. If none bit is set, the
     * current input mode does not require a soft keyboard.
     **/
    static const Int32 MASK_SKB_LAYOUT;

    /**
     * A kind of soft keyboard layout. An input mode should be anded with
     * {@link #MASK_SKB_LAYOUT} to get its soft keyboard layout.
     */
    static const Int32 MASK_SKB_LAYOUT_QWERTY;

    /**
     * A kind of soft keyboard layout. An input mode should be anded with
     * {@link #MASK_SKB_LAYOUT} to get its soft keyboard layout.
     */
    static const Int32 MASK_SKB_LAYOUT_SYMBOL1;

    /**
     * A kind of soft keyboard layout. An input mode should be anded with
     * {@link #MASK_SKB_LAYOUT} to get its soft keyboard layout.
     */
    static const Int32 MASK_SKB_LAYOUT_SYMBOL2;

    /**
     * A kind of soft keyboard layout. An input mode should be anded with
     * {@link #MASK_SKB_LAYOUT} to get its soft keyboard layout.
     */
    static const Int32 MASK_SKB_LAYOUT_SMILEY;

    /**
     * A kind of soft keyboard layout. An input mode should be anded with
     * {@link #MASK_SKB_LAYOUT} to get its soft keyboard layout.
     */
    static const Int32 MASK_SKB_LAYOUT_PHONE;

    /**
     * Used to indicate which language the current input mode is in. If the
     * current input mode works with a none-QWERTY soft keyboard, these bits are
     * also used to get language information. For example, a Chinese symbol soft
     * keyboard and an English one are different in an icon which is used to
     * tell user the language information. BTW, the smiley soft keyboard mode
     * should be set with {@link #MASK_LANGUAGE_CN} because it can only be
     * launched from Chinese QWERTY soft keyboard, and it has Chinese icon on
     * soft keyboard.
     */
    static const Int32 MASK_LANGUAGE;

    /**
     * Used to indicate the current language. An input mode should be anded with
     * {@link #MASK_LANGUAGE} to get this information.
     */
    static const Int32 MASK_LANGUAGE_CN;

    /**
     * Used to indicate the current language. An input mode should be anded with
     * {@link #MASK_LANGUAGE} to get this information.
     */
    static const Int32 MASK_LANGUAGE_EN;

    /**
     * Used to indicate which case the current input mode is in. For example,
     * English QWERTY has lowercase and uppercase. For the Chinese QWERTY, these
     * bits are ignored. For phone keyboard layout, these bits can be
     * {@link #MASK_CASE_UPPER} to request symbol page for phone soft keyboard.
     */
    static const Int32 MASK_CASE;

    /**
     * Used to indicate the current case information. An input mode should be
     * anded with {@link #MASK_CASE} to get this information.
     */
    static const Int32 MASK_CASE_LOWER;

    /**
     * Used to indicate the current case information. An input mode should be
     * anded with {@link #MASK_CASE} to get this information.
     */
    static const Int32 MASK_CASE_UPPER;

    /**
     * The input mode for the current edit box.
     */
    Int32 mInputMode;

    /**
     * Used to remember previous input mode. When user enters an edit field, the
     * previous input mode will be tried. If the previous mode can not be used
     * for the current situation (For example, previous mode is a soft keyboard
     * mode to input symbols, and we have a hardware keyboard for the current
     * situation), {@link #mRecentLauageInputMode} will be tried.
     **/
    Int32 mPreviousInputMode;

    /**
     * Used to remember recent mode to input language.
     */
    Int32 mRecentLauageInputMode;

    /**
     * Editor information of the current edit box.
     */
    AutoPtr<IEditorInfo> mEditorInfo;

    /**
     * Used to indicate required toggling operations.
     */
    AutoPtr<ToggleStates> mToggleStates;

    /**
     * The current field is a short message field?
     */
    Boolean mShortMessageField;

    /**
     * Is return key in normal state?
     */
    Boolean mEnterKeyNormal;

    /**
     * Current icon. 0 for none icon.
     */
    Int32 mInputIcon;

    /**
     * IME service.
     */
    CPinyinIME* mImeService;

    /**
     * Key toggling state for Chinese mode.
     */
    Int32 mToggleStateCn;

    /**
     * Key toggling state for Chinese mode with candidates.
     */
    Int32 mToggleStateCnCand;

    /**
     * Key toggling state for English lowwercase mode.
     */
    Int32 mToggleStateEnLower;

    /**
     * Key toggling state for English upppercase mode.
     */
    Int32 mToggleStateEnUpper;

    /**
     * Key toggling state for English symbol mode for the first page.
     */
    Int32 mToggleStateEnSym1;

    /**
     * Key toggling state for English symbol mode for the second page.
     */
    Int32 mToggleStateEnSym2;

    /**
     * Key toggling state for smiley mode.
     */
    Int32 mToggleStateSmiley;

    /**
     * Key toggling state for phone symbol mode.
     */
    Int32 mToggleStatePhoneSym;

    /**
     * Key toggling state for GO action of ENTER key.
     */
    Int32 mToggleStateGo;

    /**
     * Key toggling state for SEARCH action of ENTER key.
     */
    Int32 mToggleStateSearch;

    /**
     * Key toggling state for SEND action of ENTER key.
     */
    Int32 mToggleStateSend;

    /**
     * Key toggling state for NEXT action of ENTER key.
     */
    Int32 mToggleStateNext;

    /**
     * Key toggling state for SEND action of ENTER key.
     */
    Int32 mToggleStateDone;

    /**
     * QWERTY row toggling state for Chinese input.
     */
    Int32 mToggleRowCn;

    /**
     * QWERTY row toggling state for English input.
     */
    Int32 mToggleRowEn;

    /**
     * QWERTY row toggling state for URI input.
     */
    Int32 mToggleRowUri;

    /**
     * QWERTY row toggling state for email address input.
     */
    Int32 mToggleRowEmailAddress;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_INPUTMODESWITCHER_H__
