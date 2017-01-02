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

#include "elastos/droid/server/pm/CUserStopUserCallback.h"

using Elastos::Droid::App::EIID_IStopUserCallback;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CAR_INTERFACE_IMPL_2(CUserStopUserCallback, Object, IStopUserCallback, IBinder)

CAR_OBJECT_IMPL(CUserStopUserCallback)

CARAPI CUserStopUserCallback::constructor(
    /* [in] */ IIUserManager* um)
{
    mUm = (CUserManagerService*)um;
    return NOERROR;
}

ECode CUserStopUserCallback::UserStopped(
    /* [in] */ Int32 userId)
{
    mUm->FinishRemoveUser(userId);
    return NOERROR;
}

ECode CUserStopUserCallback::UserStopAborted(
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode CUserStopUserCallback::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
