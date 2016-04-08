
#include "CToggleStates.h"
#include "CInputModeSwitcher.h"

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

CToggleStates::CToggleStates()
    : mQwerty(FALSE)
    , mQwertyUpperCase(FALSE)
    , mRowIdToEnable(0)
    , mKeyStatesNum(0)
{
    mKeyStates = ArrayOf<Int32>::Alloc(CInputModeSwitcher::MAX_TOGGLE_STATES);
}

ECode CToggleStates::SetRowIdToEnable(
    /* [in] */ Int32 id)
{
    mRowIdToEnable = id;
    return NOERROR;
}

ECode CToggleStates::GetRowIdToEnable(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mRowIdToEnable;
    return NOERROR;
}

ECode CToggleStates::SetQwerty(
    /* [in] */ Boolean qwerty)
{
    mQwerty = qwerty;
    return NOERROR;
}

ECode CToggleStates::GetQwerty(
    /* [out] */ Boolean* qwerty)
{
    VALIDATE_NOT_NULL(qwerty);
    *qwerty = mQwerty;
    return NOERROR;
}

ECode CToggleStates::SetQwertyUpperCase(
    /* [in] */ Boolean qwertyUpperCase)
{
    mQwertyUpperCase = qwertyUpperCase;
    return NOERROR;
}

ECode CToggleStates::GetQwertyUpperCase(
    /* [out] */ Boolean* qwertyUpperCase)
{
    VALIDATE_NOT_NULL(qwertyUpperCase);
    *qwertyUpperCase = mQwertyUpperCase;
    return NOERROR;
}

ECode CToggleStates::SetKeyStatesNum(
    /* [in] */ Int32 keyStatesNum)
{
    mKeyStatesNum = keyStatesNum;
    return NOERROR;
}

ECode CToggleStates::GetKeyStatesNum(
    /* [out] */ Int32* keyStatesNum)
{
    VALIDATE_NOT_NULL(keyStatesNum);
    *keyStatesNum = mKeyStatesNum;
    return NOERROR;
}

ECode CToggleStates::SetKeyStates(
    /* [in] */ ArrayOf<Int32> * keyStates)
{
    mKeyStates = keyStates;
    return NOERROR;
}

ECode CToggleStates::GetKeyStates(
    /* [out, callee] */ ArrayOf<Int32> ** keyStates)
{
    VALIDATE_NOT_NULL(keyStates);
    *keyStates = mKeyStates;
    REFCOUNT_ADD(*keyStates);
    return NOERROR;
}


} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
