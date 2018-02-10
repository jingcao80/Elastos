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

#include "CWMSUserSwitchObserver.h"
#include "CWallpaperManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {

ECode CWMSUserSwitchObserver::constructor(
    /* [in] */ HANDLE owner)
{
    mOwner = (CWallpaperManagerService*)owner;
    return NOERROR;
}

ECode CWMSUserSwitchObserver::OnUserSwitching(
    /* [in] */ Int32 newUserId,
    /* [in] */ IIRemoteCallback* reply)
{
    mOwner->SwitchUser(newUserId, reply);
    return NOERROR;
}

ECode CWMSUserSwitchObserver::OnUserSwitchComplete(
    /* [in] */ Int32 newUserId)
{
    return NOERROR;
}

ECode CWMSUserSwitchObserver::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}


} // Server
} // Droid
} // Elastos
