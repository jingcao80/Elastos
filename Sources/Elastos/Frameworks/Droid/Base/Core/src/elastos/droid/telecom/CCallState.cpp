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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CCallState.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CCallState::
//===============================================================
CAR_SINGLETON_IMPL(CCallState)

CAR_INTERFACE_IMPL(CCallState, Singleton, ICallState)

ECode CCallState::ToString(
    /* [in] */ Int32 callState,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    switch (callState) {
        case NEW:
            *result = String("NEW");
            return NOERROR;
        case CONNECTING:
            *result = String("CONNECTING");
            return NOERROR;
        case PRE_DIAL_WAIT:
            *result = String("PRE_DIAL_WAIT");
            return NOERROR;
        case DIALING:
            *result = String("DIALING");
            return NOERROR;
        case RINGING:
            *result = String("RINGING");
            return NOERROR;
        case ACTIVE:
            *result = String("ACTIVE");
            return NOERROR;
        case ON_HOLD:
            *result = String("ON_HOLD");
            return NOERROR;
        case DISCONNECTED:
            *result = String("DISCONNECTED");
            return NOERROR;
        case ABORTED:
            *result = String("ABORTED");
            return NOERROR;
        case DISCONNECTING:
            *result = String("DISCONNECTING");
            return NOERROR;
        default:
            *result = String("UNKNOWN");
            return NOERROR;
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos