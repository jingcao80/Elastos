
#include "elastos/droid/app/CFragmentSavedState.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CParcel.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CFragmentSavedState, Object, IFragmentSavedState, IParcelable)

CAR_OBJECT_IMPL(CFragmentSavedState)


ECode CFragmentSavedState::constructor()
{
    return NOERROR;
}

ECode CFragmentSavedState::constructor(
    /* [in] */ IBundle* state)
{
    mState = state;
    return NOERROR;
}

ECode CFragmentSavedState::constructor(
    /* [in] */ IParcel* in,
    /* [in] */ IClassLoader* loader)
{
    mLoader = loader;
    ReadFromParcel(in);
    return NOERROR;
}

ECode CFragmentSavedState::GetState(
    /* [out] */ IBundle** state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode CFragmentSavedState::SetState(
    /* [in] */ IBundle* state)
{
    mState = state;
    return NOERROR;
}

ECode CFragmentSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    mState = Elastos::Droid::Os::CParcel::ReadBundle(source);
    if (mLoader != NULL && mState != NULL) {
        mState->SetClassLoader(mLoader);
    }
    return NOERROR;
}

ECode CFragmentSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Elastos::Droid::Os::CParcel::WriteBundle(dest, mState);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
