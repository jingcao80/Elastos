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

#include "elastos/droid/telephony/CVoLteServiceState.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CVoLteServiceState::TAG("CVoLteServiceState");
const Boolean CVoLteServiceState::DBG;

CAR_INTERFACE_IMPL_2(CVoLteServiceState, Object, IVoLteServiceState, IParcelable)

CAR_OBJECT_IMPL(CVoLteServiceState)

CVoLteServiceState::CVoLteServiceState()
    : mSrvccState(IVoLteServiceState::INVALID)
{
}

CVoLteServiceState::~CVoLteServiceState()
{
}

ECode CVoLteServiceState::constructor()
{
    Initialize();
    return NOERROR;
}

ECode CVoLteServiceState::constructor(
    /* [in] */ Int32 srvccState)
{
    Initialize();
    mSrvccState = srvccState;
    return NOERROR;
}

ECode CVoLteServiceState::constructor(
    /* [in] */ IVoLteServiceState* s)
{
    return s->GetSrvccState(&mSrvccState);
}

ECode CVoLteServiceState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mSrvccState);
    return NOERROR;
}

ECode CVoLteServiceState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mSrvccState);
    return NOERROR;
}

ECode CVoLteServiceState::ValidateInput()
{
    return NOERROR;
}

ECode CVoLteServiceState::FillInNotifierBundle(
    /* [in] */ IBundle* m)
{
    return m->PutInt32(String("mSrvccState"), mSrvccState);
}

ECode CVoLteServiceState::GetSrvccState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mSrvccState;
    return NOERROR;
}

ECode CVoLteServiceState::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 primeNum = 31;
    *hashCode = (mSrvccState * primeNum);
    return NOERROR;
}

ECode CVoLteServiceState::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<IVoLteServiceState> s;

    // try {
    s = IVoLteServiceState::Probe(other);
    // } catch (ClassCastException ex) {
    //     return false;
    // }

    if (s == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 state;
    s->GetSrvccState(&state);
    *res = (mSrvccState == state);
    return NOERROR;
}

ECode CVoLteServiceState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("VoLteServiceState: ");
    sb.Append(mSrvccState);
    *str = sb.ToString();
    return NOERROR;
}

ECode CVoLteServiceState::NewFromBundle(
    /* [in] */ IBundle* m,
    /* [out] */ IVoLteServiceState** state)
{
    AutoPtr<CVoLteServiceState> ret;
    CVoLteServiceState::NewByFriend((CVoLteServiceState**)&ret);
    ret->SetFromNotifierBundle(m);
    *state = IVoLteServiceState::Probe(ret);
    REFCOUNT_ADD(*state)
    return NOERROR;
}

void CVoLteServiceState::CopyFrom(
    /* [in] */ IVoLteServiceState* s)
{
    s->GetSrvccState(&mSrvccState);
}

void CVoLteServiceState::Initialize()
{
    mSrvccState = IVoLteServiceState::INVALID;
}

void CVoLteServiceState::SetFromNotifierBundle(
    /* [in] */ IBundle* m)
{
    m->GetInt32(String("mSrvccState"), &mSrvccState);
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
