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

#include "elastos/droid/telephony/CDisconnectCause.h"
#include "elastos/droid/telephony/DisconnectCause.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CDisconnectCause, Singleton, IDisconnectCause)

CAR_SINGLETON_IMPL(CDisconnectCause)

ECode CDisconnectCause::ToString(
    /* [in] */ Int32 cause,
    /* [out] */ String* str)
{
    return DisconnectCause::ToString(cause, str);
}

} //namespace Elastos
} //namespace Droid
} //namespace Telephony