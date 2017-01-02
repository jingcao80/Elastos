//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
