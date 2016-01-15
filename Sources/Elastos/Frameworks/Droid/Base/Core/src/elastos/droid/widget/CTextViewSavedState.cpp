#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CTextViewSavedState.h"

namespace Elastos {
namespace Droid {
namespace Widget {


ECode CTextViewSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return TextViewSavedState::ReadFromParcel(source);
}

ECode CTextViewSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return TextViewSavedState::WriteToParcel(dest);
}

ECode CTextViewSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    VALIDATE_NOT_NULL(superState);
    AutoPtr<IParcelable> temp = TextViewSavedState::GetSuperState();
    *superState = temp;
    REFCOUNT_ADD(*superState);
    return NOERROR;
}

ECode CTextViewSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return TextViewSavedState::Init(superState);
}

ECode CTextViewSavedState::constructor()
{
    return TextViewSavedState::Init();
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
