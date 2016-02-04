
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/AbsSavedState.h"
// #include "elastos/droid/view/CEmptyAbsSavedState.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL_2(AbsSavedState, Object, IAbsSavedState, IParcelable)

static AutoPtr<IAbsSavedState> InitEmptySs()
{
    // AutoPtr<CEmptyAbsSavedState> rst;
    // CEmptyAbsSavedState::NewByFriend((CEmptyAbsSavedState**)&rst);
    // return rst;
    return NULL;
}

const AutoPtr<IAbsSavedState> AbsSavedState::EMPTY_STATE = InitEmptySs();

/**
 * Constructor used to make the EMPTY_STATE singleton
 */
AbsSavedState::AbsSavedState()
{
}

AbsSavedState::~AbsSavedState()
{
}

AutoPtr<IParcelable> AbsSavedState::GetSuperState()
{
    return mSuperState;
}

ECode AbsSavedState::GetSuperState(
    /* [out] */ IParcelable** p)
{
    VALIDATE_NOT_NULL(p)
    *p = mSuperState;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode AbsSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mSuperState);
    //dest.writeParcelable(mSuperState, flags);
    return NOERROR;
}

ECode AbsSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IParcelable> superState;
    source->ReadInterfacePtr((Handle32*)&superState);
    if(superState != NULL)
    {
        mSuperState = superState;
    }else{
        mSuperState = IParcelable::Probe(EMPTY_STATE.Get());
    }

    return NOERROR;
}

ECode AbsSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    if (superState == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    //    throw new IllegalArgumentException("superState must not be null");
    }
    mSuperState = superState != IParcelable::Probe(EMPTY_STATE.Get()) ? superState : NULL;

    return NOERROR;
}

ECode AbsSavedState::constructor()
{
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
