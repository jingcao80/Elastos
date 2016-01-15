#include "elastos/droid/view/CEmptyAbsSavedState.h"

namespace Elastos {
namespace Droid {
namespace View {

CEmptyAbsSavedState::CEmptyAbsSavedState()
{}

ECode CEmptyAbsSavedState::constructor()
{
    return NOERROR;
}

ECode CEmptyAbsSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    VALIDATE_NOT_NULL(superState);
    AutoPtr<IParcelable> temp = AbsSavedState::GetSuperState();
    *superState = temp;
    return NOERROR;
}

ECode CEmptyAbsSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return AbsSavedState::WriteToParcel(dest);
}

ECode CEmptyAbsSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return AbsSavedState::ReadFromParcel(source);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
