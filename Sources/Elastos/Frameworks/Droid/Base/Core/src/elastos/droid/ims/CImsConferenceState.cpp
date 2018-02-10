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

#include "elastos/droid/ims/CImsConferenceState.h"

using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Ims {

CAR_INTERFACE_IMPL_2(CImsConferenceState, Object, IImsConferenceState, IParcelable)

CAR_OBJECT_IMPL(CImsConferenceState)

CImsConferenceState::CImsConferenceState()
{
}

CImsConferenceState::~CImsConferenceState()
{
}

ECode CImsConferenceState::constructor()
{
    CHashMap::New((IHashMap**)&mParticipants);
    return NOERROR;
}

ECode CImsConferenceState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mParticipants = IHashMap::Probe(obj);
    return NOERROR;
}

ECode CImsConferenceState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mParticipants);
    return NOERROR;
}

} // namespace Ims
} // namespace Droid
} // namespace Elastos