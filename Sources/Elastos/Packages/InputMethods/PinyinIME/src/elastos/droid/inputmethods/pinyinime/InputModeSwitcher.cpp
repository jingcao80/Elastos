
#include "InputModeSwitcher.h"
#include "R.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::IInputType;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

//===========================================================
//  InputModeSwitcher::ToggleStates
//===========================================================
InputModeSwitcher::ToggleStates::ToggleStates()
    : mQwerty(FALSE)
    , mQwertyUpperCase(FALSE)
    , mRowIdToEnable(0)
    , mKeyStatesNum(0)
{
    mKeyStates = ArrayOf<Int32>::Alloc(InputModeSwitcher::MAX_TOGGLE_STATES);
}


//===========================================================
//  InputModeSwitcher
//===========================================================
const Int32 InputModeSwitcher::USERDEF_KEYCODE_SHIFT_1 = -1;
const Int32 InputModeSwitcher::USERDEF_KEYCODE_LANG_2 = -2;
const Int32 InputModeSwitcher::USERDEF_KEYCODE_SYM_3 = -3;
const Int32 InputModeSwitcher::USERDEF_KEYCODE_MORE_SYM_5 = -5;
const Int32 InputModeSwitcher::USERDEF_KEYCODE_SMILEY_6 = -6;
const Int32 InputModeSwitcher::MASK_SKB_LAYOUT = 0xf0000000;
const Int32 InputModeSwitcher::MASK_SKB_LAYOUT_QWERTY = 0x10000000;
const Int32 InputModeSwitcher::MASK_SKB_LAYOUT_SYMBOL1 = 0x20000000;
const Int32 InputModeSwitcher::MASK_SKB_LAYOUT_SYMBOL2 = 0x30000000;
const Int32 InputModeSwitcher::MASK_SKB_LAYOUT_SMILEY = 0x40000000;
const Int32 InputModeSwitcher::MASK_SKB_LAYOUT_PHONE = 0x50000000;
const Int32 InputModeSwitcher::MASK_LANGUAGE = 0x0f000000;
const Int32 InputModeSwitcher::MASK_LANGUAGE_CN = 0x01000000;
const Int32 InputModeSwitcher::MASK_LANGUAGE_EN = 0x02000000;
const Int32 InputModeSwitcher::MASK_CASE = 0x00f00000;
const Int32 InputModeSwitcher::MASK_CASE_LOWER = 0x00100000;
const Int32 InputModeSwitcher::MASK_CASE_UPPER = 0x00200000;
const Int32 InputModeSwitcher::USERDEF_KEYCODE_PHONE_SYM_4 = -4;
const Int32 InputModeSwitcher::MODE_SKB_CHINESE = (MASK_SKB_LAYOUT_QWERTY | MASK_LANGUAGE_CN);
const Int32 InputModeSwitcher::MODE_SKB_SYMBOL1_CN = (MASK_SKB_LAYOUT_SYMBOL1 | MASK_LANGUAGE_CN);
const Int32 InputModeSwitcher::MODE_SKB_SYMBOL2_CN = (MASK_SKB_LAYOUT_SYMBOL2 | MASK_LANGUAGE_CN);
const Int32 InputModeSwitcher::MODE_SKB_ENGLISH_LOWER = (MASK_SKB_LAYOUT_QWERTY
        | MASK_LANGUAGE_EN | MASK_CASE_LOWER);

const Int32 InputModeSwitcher::MODE_SKB_ENGLISH_UPPER = (MASK_SKB_LAYOUT_QWERTY
        | MASK_LANGUAGE_EN | MASK_CASE_UPPER);

const Int32 InputModeSwitcher::MODE_SKB_SYMBOL1_EN = (MASK_SKB_LAYOUT_SYMBOL1 | MASK_LANGUAGE_EN);
const Int32 InputModeSwitcher::MODE_SKB_SYMBOL2_EN = (MASK_SKB_LAYOUT_SYMBOL2 | MASK_LANGUAGE_EN);
const Int32 InputModeSwitcher::MODE_SKB_SMILEY = (MASK_SKB_LAYOUT_SMILEY | MASK_LANGUAGE_CN);
const Int32 InputModeSwitcher::MODE_SKB_PHONE_NUM = (MASK_SKB_LAYOUT_PHONE);
const Int32 InputModeSwitcher::MODE_SKB_PHONE_SYM = (MASK_SKB_LAYOUT_PHONE | MASK_CASE_UPPER);
const Int32 InputModeSwitcher::MODE_HKB_CHINESE = (MASK_LANGUAGE_CN);
const Int32 InputModeSwitcher::MODE_HKB_ENGLISH = (MASK_LANGUAGE_EN);
const Int32 InputModeSwitcher::MODE_UNSET = 0;
const Int32 InputModeSwitcher::MAX_TOGGLE_STATES = 4;

InputModeSwitcher::InputModeSwitcher(
    /* [in] */ IPinyinIME* imeService)
    : mInputMode(MODE_UNSET)
    , mPreviousInputMode(MODE_SKB_CHINESE)
    , mRecentLauageInputMode(MODE_SKB_CHINESE)
    , mToggleStates(new ToggleStates())
    , mShortMessageField(FALSE)
    , mEnterKeyNormal(TRUE)
    , mInputIcon(R::drawable::ime_pinyin)
    , mToggleStateCn(0)
    , mToggleStateCnCand(0)
    , mToggleStateEnLower(0)
    , mToggleStateEnUpper(0)
    , mToggleStateEnSym1(0)
    , mToggleStateEnSym2(0)
    , mToggleStateSmiley(0)
    , mToggleStatePhoneSym(0)
    , mToggleStateGo(0)
    , mToggleStateSearch(0)
    , mToggleStateSend(0)
    , mToggleStateNext(0)
    , mToggleStateDone(0)
    , mToggleRowCn(0)
    , mToggleRowEn(0)
    , mToggleRowUri(0)
    , mToggleRowEmailAddress(0)
{
    mImeService = imeService;
    AutoPtr<IResources> r;
    IContext::Probe(mImeService)->GetResources((IResources**)&r);
    String strValue;
    mToggleStateCn = StringUtils::ParseInt32((r->GetString(R::string::toggle_cn, &strValue), strValue));
    mToggleStateCnCand = StringUtils::ParseInt32((r->GetString(R::string::toggle_cn_cand, &strValue), strValue));
    mToggleStateEnLower = StringUtils::ParseInt32((r->GetString(R::string::toggle_en_lower, &strValue), strValue));
    mToggleStateEnUpper = StringUtils::ParseInt32((r->GetString(R::string::toggle_en_upper, &strValue), strValue));
    mToggleStateEnSym1 = StringUtils::ParseInt32((r->GetString(R::string::toggle_en_sym1, &strValue), strValue));
    mToggleStateEnSym2 = StringUtils::ParseInt32((r->GetString(R::string::toggle_en_sym2, &strValue), strValue));
    mToggleStateSmiley = StringUtils::ParseInt32((r->GetString(R::string::toggle_smiley, &strValue), strValue));
    mToggleStatePhoneSym = StringUtils::ParseInt32((r->GetString(R::string::toggle_phone_sym, &strValue), strValue));

    mToggleStateGo = StringUtils::ParseInt32((r->GetString(R::string::toggle_enter_go, &strValue), strValue));
    mToggleStateSearch = StringUtils::ParseInt32((r->GetString(R::string::toggle_enter_search, &strValue), strValue));
    mToggleStateSend = StringUtils::ParseInt32((r->GetString(R::string::toggle_enter_send, &strValue), strValue));
    mToggleStateNext = StringUtils::ParseInt32((r->GetString(R::string::toggle_enter_next, &strValue), strValue));
    mToggleStateDone = StringUtils::ParseInt32((r->GetString(R::string::toggle_enter_done, &strValue), strValue));

    mToggleRowCn = StringUtils::ParseInt32((r->GetString(R::string::toggle_row_cn, &strValue), strValue));
    mToggleRowEn = StringUtils::ParseInt32((r->GetString(R::string::toggle_row_en, &strValue), strValue));
    mToggleRowUri = StringUtils::ParseInt32((r->GetString(R::string::toggle_row_uri, &strValue), strValue));
    mToggleRowEmailAddress = StringUtils::ParseInt32((r->GetString(R::string::toggle_row_emailaddress, &strValue), strValue));
}

Int32 InputModeSwitcher::GetInputMode()
{
    return mInputMode;
}

AutoPtr<ToggleStates> InputModeSwitcher::GetToggleStates()
{
    return mToggleStates;
}

Int32 InputModeSwitcher::GetSkbLayout()
{
    Int32 layout = (mInputMode & MASK_SKB_LAYOUT);

    switch (layout) {
        case MASK_SKB_LAYOUT_QWERTY: {
            return R::xml::skb_qwerty;
        }
        case MASK_SKB_LAYOUT_SYMBOL1: {
            return R::xml::skb_sym1;
        }
        case MASK_SKB_LAYOUT_SYMBOL2: {
            return R::xml::skb_sym2;
        }
        case MASK_SKB_LAYOUT_SMILEY: {
            return R::xml::skb_smiley;
        }
        case MASK_SKB_LAYOUT_PHONE: {
            return R::xml::skb_phone;
        }
    }
    return 0;
}

Int32 InputModeSwitcher::SwitchLanguageWithHkb()
{
    Int32 newInputMode = MODE_HKB_CHINESE;
    mInputIcon = R::drawable::ime_pinyin;

    if (MODE_HKB_CHINESE == mInputMode) {
        newInputMode = MODE_HKB_ENGLISH;
        mInputIcon = R::drawable::ime_en;
    }

    SaveInputMode(newInputMode);
    return mInputIcon;
}

Int32 InputModeSwitcher::SwitchModeForUserKey(
    /* [in] */ Int32 userKey)
{
    Int32 newInputMode = MODE_UNSET;

    if (USERDEF_KEYCODE_LANG_2 == userKey) {
        if (MODE_SKB_CHINESE == mInputMode) {
            newInputMode = MODE_SKB_ENGLISH_LOWER;
        }
        else if (MODE_SKB_ENGLISH_LOWER == mInputMode
                || MODE_SKB_ENGLISH_UPPER == mInputMode) {
            newInputMode = MODE_SKB_CHINESE;
        }
        else if (MODE_SKB_SYMBOL1_CN == mInputMode) {
            newInputMode = MODE_SKB_SYMBOL1_EN;
        }
        else if (MODE_SKB_SYMBOL1_EN == mInputMode) {
            newInputMode = MODE_SKB_SYMBOL1_CN;
        }
        else if (MODE_SKB_SYMBOL2_CN == mInputMode) {
            newInputMode = MODE_SKB_SYMBOL2_EN;
        }
        else if (MODE_SKB_SYMBOL2_EN == mInputMode) {
            newInputMode = MODE_SKB_SYMBOL2_CN;
        }
        else if (MODE_SKB_SMILEY == mInputMode) {
            newInputMode = MODE_SKB_CHINESE;
        }
    }
    else if (USERDEF_KEYCODE_SYM_3 == userKey) {
        if (MODE_SKB_CHINESE == mInputMode) {
            newInputMode = MODE_SKB_SYMBOL1_CN;
        }
        else if (MODE_SKB_ENGLISH_UPPER == mInputMode
                || MODE_SKB_ENGLISH_LOWER == mInputMode) {
            newInputMode = MODE_SKB_SYMBOL1_EN;
        }
        else if (MODE_SKB_SYMBOL1_EN == mInputMode
                || MODE_SKB_SYMBOL2_EN == mInputMode) {
            newInputMode = MODE_SKB_ENGLISH_LOWER;
        }
        else if (MODE_SKB_SYMBOL1_CN == mInputMode
                || MODE_SKB_SYMBOL2_CN == mInputMode) {
            newInputMode = MODE_SKB_CHINESE;
        }
        else if (MODE_SKB_SMILEY == mInputMode) {
            newInputMode = MODE_SKB_SYMBOL1_CN;
        }
    }
    else if (USERDEF_KEYCODE_SHIFT_1 == userKey) {
        if (MODE_SKB_ENGLISH_LOWER == mInputMode) {
            newInputMode = MODE_SKB_ENGLISH_UPPER;
        }
        else if (MODE_SKB_ENGLISH_UPPER == mInputMode) {
            newInputMode = MODE_SKB_ENGLISH_LOWER;
        }
    }
    else if (USERDEF_KEYCODE_MORE_SYM_5 == userKey) {
        Int32 sym = (MASK_SKB_LAYOUT & mInputMode);
        if (MASK_SKB_LAYOUT_SYMBOL1 == sym) {
            sym = MASK_SKB_LAYOUT_SYMBOL2;
        }
        else {
            sym = MASK_SKB_LAYOUT_SYMBOL1;
        }
        newInputMode = ((mInputMode & (~MASK_SKB_LAYOUT)) | sym);
    }
    else if (USERDEF_KEYCODE_SMILEY_6 == userKey) {
        if (MODE_SKB_CHINESE == mInputMode) {
            newInputMode = MODE_SKB_SMILEY;
        }
        else {
            newInputMode = MODE_SKB_CHINESE;
        }
    }
    else if (USERDEF_KEYCODE_PHONE_SYM_4 == userKey) {
        if (MODE_SKB_PHONE_NUM == mInputMode) {
            newInputMode = MODE_SKB_PHONE_SYM;
        }
        else {
            newInputMode = MODE_SKB_PHONE_NUM;
        }
    }

    if (newInputMode == mInputMode || MODE_UNSET == newInputMode) {
        return mInputIcon;
    }

    SaveInputMode(newInputMode);
    PrepareToggleStates(TRUE);
    return mInputIcon;
}

// Return the icon to update.
Int32 InputModeSwitcher::RequestInputWithHkb(
    /* [in] */ IEditorInfo* editorInfo)
{
    mShortMessageField = FALSE;
    Boolean english = FALSE;
    Int32 newInputMode = MODE_HKB_CHINESE;

    Int32 inputType = 0;
    editorInfo->GetInputType(&inputType);
    switch (inputType & IInputType::TYPE_MASK_CLASS) {
        case IInputType::TYPE_CLASS_NUMBER:
        case IInputType::TYPE_CLASS_PHONE:
        case IInputType::TYPE_CLASS_DATETIME:
            english = TRUE;
            break;
        case IInputType::TYPE_CLASS_TEXT:
            Int32 v = inputType & IInputType::TYPE_MASK_VARIATION;
            if (v == IInputType::TYPE_TEXT_VARIATION_EMAIL_ADDRESS
                    || v == IInputType::TYPE_TEXT_VARIATION_PASSWORD
                    || v == IInputType::TYPE_TEXT_VARIATION_VISIBLE_PASSWORD
                    || v == IInputType::TYPE_TEXT_VARIATION_URI) {
                english = TRUE;
            }
            else if (v == IInputType::TYPE_TEXT_VARIATION_SHORT_MESSAGE) {
                mShortMessageField = TRUE;
            }
            break;
        default:
            break;
    }

    if (english) {
        // If the application request English mode, we switch to it.
        newInputMode = MODE_HKB_ENGLISH;
    }
    else {
        // If the application do not request English mode, we will
        // try to keep the previous mode to input language text.
        // Because there is not soft keyboard, we need discard all
        // soft keyboard related information from the previous language
        // mode.
        if ((mRecentLauageInputMode & MASK_LANGUAGE) == MASK_LANGUAGE_CN) {
            newInputMode = MODE_HKB_CHINESE;
        }
        else {
            newInputMode = MODE_HKB_ENGLISH;
        }
    }
    mEditorInfo = editorInfo;
    SaveInputMode(newInputMode);
    PrepareToggleStates(FALSE);
    return mInputIcon;
}

Int32 InputModeSwitcher::RequestInputWithSkb(
    /* [in] */ IEditorInfo* editorInfo)
{
    mShortMessageField = FALSE;

    Int32 newInputMode = MODE_SKB_CHINESE;

    Int32 inputType = 0;
    editorInfo->GetInputType(&inputType);
    switch (inputType & IInputType::TYPE_MASK_CLASS) {
        case IInputType::TYPE_CLASS_NUMBER:
        case IInputType::TYPE_CLASS_DATETIME:
            newInputMode = MODE_SKB_SYMBOL1_EN;
            break;
        case IInputType::TYPE_CLASS_PHONE:
            newInputMode = MODE_SKB_PHONE_NUM;
            break;
        case IInputType::TYPE_CLASS_TEXT: {
            Int32 v = inputType & IInputType::TYPE_MASK_VARIATION;
            if (v == IInputType::TYPE_TEXT_VARIATION_EMAIL_ADDRESS
                    || v == IInputType::TYPE_TEXT_VARIATION_PASSWORD
                    || v == IInputType::TYPE_TEXT_VARIATION_VISIBLE_PASSWORD
                    || v == IInputType::TYPE_TEXT_VARIATION_URI) {
                // If the application request English mode, we switch to it.
                newInputMode = MODE_SKB_ENGLISH_LOWER;
            }
            else {
                if (v == IInputType::TYPE_TEXT_VARIATION_SHORT_MESSAGE) {
                    mShortMessageField = TRUE;
                }
                // If the application do not request English mode, we will
                // try to keep the previous mode.
                Int32 skbLayout = (mInputMode & MASK_SKB_LAYOUT);
                newInputMode = mInputMode;
                if (0 == skbLayout) {
                    if ((mInputMode & MASK_LANGUAGE) == MASK_LANGUAGE_CN) {
                        newInputMode = MODE_SKB_CHINESE;
                    }
                    else {
                        newInputMode = MODE_SKB_ENGLISH_LOWER;
                    }
                }
            }
            break;
        }
        default: {
            // Try to keep the previous mode.
            Int32 skbLayout = (mInputMode & MASK_SKB_LAYOUT);
            newInputMode = mInputMode;
            if (0 == skbLayout) {
                if ((mInputMode & MASK_LANGUAGE) == MASK_LANGUAGE_CN) {
                    newInputMode = MODE_SKB_CHINESE;
                }
                else {
                    newInputMode = MODE_SKB_ENGLISH_LOWER;
                }
            }
            break;
        }
    }

    mEditorInfo = editorInfo;
    SaveInputMode(newInputMode);
    PrepareToggleStates(TRUE);
    return mInputIcon;
}

Int32 InputModeSwitcher::RequestBackToPreviousSkb()
{
    Int32 layout = (mInputMode & MASK_SKB_LAYOUT);
    Int32 lastLayout = (mPreviousInputMode & MASK_SKB_LAYOUT);
    if (0 != layout && 0 != lastLayout) {
        mInputMode = mPreviousInputMode;
        SaveInputMode(mInputMode);
        PrepareToggleStates(TRUE);
        return mInputIcon;
    }
    return 0;
}

Int32 InputModeSwitcher::GetTooggleStateForCnCand()
{
    return mToggleStateCnCand;
}

Boolean InputModeSwitcher::IsEnglishWithHkb()
{
    return MODE_HKB_ENGLISH == mInputMode;
}

Boolean InputModeSwitcher::IsEnglishWithSkb()
{
    return MODE_SKB_ENGLISH_LOWER == mInputMode
            || MODE_SKB_ENGLISH_UPPER == mInputMode;
}

Boolean InputModeSwitcher::IsEnglishUpperCaseWithSkb()
{
    return MODE_SKB_ENGLISH_UPPER == mInputMode;
}

Boolean InputModeSwitcher::IsChineseText()
{
    VALIDATE_NOT_NULL(result);
    Int32 skbLayout = (mInputMode & MASK_SKB_LAYOUT);
    if (MASK_SKB_LAYOUT_QWERTY == skbLayout || 0 == skbLayout) {
        Int32 language = (mInputMode & MASK_LANGUAGE);
        if (MASK_LANGUAGE_CN == language) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean InputModeSwitcher::IsChineseTextWithHkb()
{
    Int32 skbLayout = (mInputMode & MASK_SKB_LAYOUT);
    if (0 == skbLayout) {
        Int32 language = (mInputMode & MASK_LANGUAGE);
        if (MASK_LANGUAGE_CN == language) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean InputModeSwitcher::IsChineseTextWithSkb()
{
    Int32 skbLayout = (mInputMode & MASK_SKB_LAYOUT);
    if (MASK_SKB_LAYOUT_QWERTY == skbLayout) {
        Int32 language = (mInputMode & MASK_LANGUAGE);
        if (MASK_LANGUAGE_CN == language) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean InputModeSwitcher::IsSymbolWithSkb()
{
    Int32 skbLayout = (mInputMode & MASK_SKB_LAYOUT);
    if (MASK_SKB_LAYOUT_SYMBOL1 == skbLayout
            || MASK_SKB_LAYOUT_SYMBOL2 == skbLayout) {
        return TRUE;
    }
    return FALSE;
}

Boolean InputModeSwitcher::IsEnterNoramlState()
{
    return mEnterKeyNormal;
}

Boolean InputModeSwitcher::TryHandleLongPressSwitch(
    /* [in] */ Int32 keyCode)
{
    if (USERDEF_KEYCODE_LANG_2 == keyCode
            || USERDEF_KEYCODE_PHONE_SYM_4 == keyCode) {
        mImeService->ShowOptionsMenu();
        return TRUE;
    }
    return FALSE;
}

void InputModeSwitcher::SaveInputMode(
    /* [in] */ Int32 newInputMode)
{
    mPreviousInputMode = mInputMode;
    mInputMode = newInputMode;

    Int32 skbLayout = (mInputMode & MASK_SKB_LAYOUT);
    if (MASK_SKB_LAYOUT_QWERTY == skbLayout || 0 == skbLayout) {
        mRecentLauageInputMode = mInputMode;
    }

    mInputIcon = R::drawable::ime_pinyin;
    Boolean result = FALSE;
    if (IsEnglishWithHkb(&result), result) {
        mInputIcon = R::drawable::ime_en;
    }
    else if (IsChineseTextWithHkb(&result), result) {
        mInputIcon = R::drawable::ime_pinyin;
    }

    if (!Environment::GetInstance()->HasHardKeyboard()) {
        mInputIcon = 0;
    }
}

void InputModeSwitcher::PrepareToggleStates(
    /* [in] */ Boolean needSkb)
{
    mEnterKeyNormal = TRUE;
    if (!needSkb) return;

    mToggleStates->SetQwerty(FALSE);
    mToggleStates->SetKeyStatesNum(0);

    AutoPtr<ArrayOf<Int32> > states;
    mToggleStates->GetKeyStates((ArrayOf<Int32>**)&states);
    Int32 statesNum = 0;
    // Toggle state for language.
    Int32 language = (mInputMode & MASK_LANGUAGE);
    Int32 layout = (mInputMode & MASK_SKB_LAYOUT);
    Int32 charcase = (mInputMode & MASK_CASE);
    Int32 inputType = 0;
    Int32 variation = (mEditorInfo->GetInputType(&inputType), inputType) & IInputType::TYPE_MASK_VARIATION;

    if (MASK_SKB_LAYOUT_PHONE != layout) {
        if (MASK_LANGUAGE_CN == language) {
            // Chinese and Chinese symbol are always the default states,
            // do not add a toggling operation.
            if (MASK_SKB_LAYOUT_QWERTY == layout) {
                mToggleStates->SetQwerty(TRUE);
                mToggleStates->SetQwertyUpperCase(TRUE);
                if (mShortMessageField) {
                    (*states)[statesNum] = mToggleStateSmiley;
                    statesNum++;
                }
            }
        }
        else if (MASK_LANGUAGE_EN == language) {
            if (MASK_SKB_LAYOUT_QWERTY == layout) {
                mToggleStates->SetQwerty(TRUE);
                mToggleStates->SetQwertyUpperCase(FALSE);
                (*states)[statesNum] = mToggleStateEnLower;
                if (MASK_CASE_UPPER == charcase) {
                    mToggleStates->SetQwertyUpperCase(TRUE);
                    (*states)[statesNum] = mToggleStateEnUpper;
                }
                statesNum++;
            }
            else if (MASK_SKB_LAYOUT_SYMBOL1 == layout) {
                (*states)[statesNum] = mToggleStateEnSym1;
                statesNum++;
            }
            else if (MASK_SKB_LAYOUT_SYMBOL2 == layout) {
                (*states)[statesNum] = mToggleStateEnSym2;
                statesNum++;
            }
        }

        // Toggle rows for QWERTY.
        //TODO : KeyRow::DEFAULT_ROW_ID
        mToggleStates->SetRowIdToEnable(0/*KeyRow::DEFAULT_ROW_ID*/);
        if (variation == IInputType::TYPE_TEXT_VARIATION_EMAIL_ADDRESS) {
            mToggleStates->SetRowIdToEnable(mToggleRowEmailAddress);
        }
        else if (variation == IInputType::TYPE_TEXT_VARIATION_URI) {
            mToggleStates->SetRowIdToEnable(mToggleRowUri);
        }
        else if (MASK_LANGUAGE_CN == language) {
            mToggleStates->SetRowIdToEnable(mToggleRowCn);
        }
        else if (MASK_LANGUAGE_EN == language) {
            mToggleStates->SetRowIdToEnable(mToggleRowEn);
        }
    }
    else {
        if (MASK_CASE_UPPER == charcase) {
            (*states)[statesNum] = mToggleStatePhoneSym;
            statesNum++;
        }
    }

    // Toggle state for enter key.
    Int32 imeOptions = 0;
    Int32 action = (mEditorInfo->GetImeOptions(&imeOptions), imeOptions)
            & (IEditorInfo::IME_MASK_ACTION | IEditorInfo::IME_FLAG_NO_ENTER_ACTION);

    if (action == IEditorInfo::IME_ACTION_GO) {
        (*states)[statesNum] = mToggleStateGo;
        statesNum++;
        mEnterKeyNormal = FALSE;
    }
    else if (action == IEditorInfo::IME_ACTION_SEARCH) {
        (*states)[statesNum] = mToggleStateSearch;
        statesNum++;
        mEnterKeyNormal = FALSE;
    }
    else if (action == IEditorInfo::IME_ACTION_SEND) {
        (*states)[statesNum] = mToggleStateSend;
        statesNum++;
        mEnterKeyNormal = FALSE;
    }
    else if (action == IEditorInfo::IME_ACTION_NEXT) {
        Int32 inputType = 0;
        mEditorInfo->GetInputType(&inputType);
        Int32 f = inputType & IInputType::TYPE_MASK_FLAGS;
        if (f != IInputType::TYPE_TEXT_FLAG_MULTI_LINE) {
            (*states)[statesNum] = mToggleStateNext;
            statesNum++;
            mEnterKeyNormal = FALSE;
        }
    }
    else if (action == IEditorInfo::IME_ACTION_DONE) {
        (*states)[statesNum] = mToggleStateDone;
        statesNum++;
        mEnterKeyNormal = FALSE;
    }
    mToggleStates->SetKeyStatesNum(statesNum);
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

