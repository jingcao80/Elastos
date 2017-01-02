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

#include "elastos/droid/webkit/CookieManager.h"
#include "elastos/droid/webkit/CookieSyncManager.h"
#include "elastos/droid/webkit/WebViewDatabase.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

AutoPtr<ICookieSyncManager> CookieSyncManager::sRef;
Boolean CookieSyncManager::sGetInstanceAllowed = FALSE;

CAR_INTERFACE_IMPL(CookieSyncManager, Object, ICookieSyncManager);

CookieSyncManager::CookieSyncManager()
    : WebSyncManager(NULL, String(NULL))
{
}

ECode CookieSyncManager::constructor()
{
    return NOERROR;
}

ECode CookieSyncManager::Sync()
{
    return CookieManager::GetInstance()->Flush();
}

ECode CookieSyncManager::ResetSync()
{
    return NOERROR;
}

ECode CookieSyncManager::StartSync()
{
    return NOERROR;
}

ECode CookieSyncManager::StopSync()
{
    return NOERROR;
}

ECode CookieSyncManager::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "CookieSyncManager";
    return NOERROR;
}

void CookieSyncManager::SyncFromRamToFlash()
{
    CookieManager::GetInstance()->Flush();
}

ECode CookieSyncManager::SetGetInstanceIsAllowed()
{
    sGetInstanceAllowed = TRUE;
    return NOERROR;
}

ECode CookieSyncManager::CheckInstanceIsAllowed()
{
    if (!sGetInstanceAllowed) {
        // throw new IllegalStateException(
        //             "CookieSyncManager::createInstance() needs to be called "
        //                     + "before CookieSyncManager::getInstance()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos