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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/AbsSavedState.h"
#include "elastos/droid/view/CAbsSavedState.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL_2(AbsSavedState, Object, IAbsSavedState, IParcelable)

const AutoPtr<IAbsSavedState> AbsSavedState::EMPTY_STATE;

AutoPtr<IAbsSavedState> AbsSavedState::GetEMPTY_STATE()
{
    if (EMPTY_STATE == NULL) {
        CAbsSavedState::New((IAbsSavedState**)&EMPTY_STATE);
    }
    return EMPTY_STATE;
}

AbsSavedState::AbsSavedState()
{}

AbsSavedState::~AbsSavedState()
{}

ECode AbsSavedState::constructor()
{
    return NOERROR;
}

ECode AbsSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    if (superState == NULL) {
        Logger::E("AbsSavedState", "superState must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mSuperState = superState != IParcelable::Probe(GetEMPTY_STATE()) ? superState : NULL;
    return NOERROR;
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
    return NOERROR;
}

ECode AbsSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mSuperState = IParcelable::Probe(obj);
    if (mSuperState == NULL) {
        AutoPtr<IAbsSavedState> empty = GetEMPTY_STATE();
        mSuperState = IParcelable::Probe(empty);
    }

    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
