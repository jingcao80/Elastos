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

#include "elastos/droid/dialer/voicemail/CVoicemailStatusHelperImplHelper.h"
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperImpl.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Voicemail {

CAR_INTERFACE_IMPL(CVoicemailStatusHelperImplHelper, Singleton, IVoicemailStatusHelperImplHelper);

CAR_SINGLETON_IMPL(CVoicemailStatusHelperImplHelper);

ECode CVoicemailStatusHelperImplHelper::GetPROJECTION(
    /* [out, callee] */ ArrayOf<String>** protection)
{
    VALIDATE_NOT_NULL(protection);
    *protection = VoicemailStatusHelperImpl::PROJECTION;
    REFCOUNT_ADD(*protection);
    return NOERROR;
}

} // Voicemail
} // Dialer
} // Droid
} // Elastos
