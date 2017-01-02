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
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/CUiModeManager.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;

namespace Elastos {
namespace Droid {
namespace App {

static const String TAG("CUiModeManager");

CAR_INTERFACE_IMPL(CUiModeManager, Object, IUiModeManager)

CAR_OBJECT_IMPL(CUiModeManager)

CUiModeManager::CUiModeManager()
{
    mService = NULL;
}

CUiModeManager::~CUiModeManager()
{
}

ECode CUiModeManager::constructor()
{
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::UI_MODE_SERVICE);
    mService = IIUiModeManager::Probe(service);
    return mService != NULL ? NOERROR : E_REMOTE_EXCEPTION;
}

/**
 * Force device into car mode, like it had been placed in the car dock.
 * This will cause the device to switch to the car home UI as part of
 * the mode switch.
 * @param flags Must be 0.
 */
ECode CUiModeManager::EnableCarMode(
    /* [in] */ Int32 flags)
{
    if (mService == NULL) {
        Logger::E(TAG, "Service %s is not ready yet.", (const char*)IContext::UI_MODE_SERVICE);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    ECode ec = mService->EnableCarMode(flags);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "EnableCarMode: RemoteException");
    }
    return ec;
}

/**
 * Turn off special mode if currently in car mode.
 * @param flags May be 0 or {@link #DISABLE_CAR_MODE_GO_HOME}.
 */
ECode CUiModeManager::DisableCarMode(
    /* [in] */ Int32 flags)
{
    if (mService == NULL) {
        Logger::E(TAG, "Service %s is not ready yet.", (const char*)IContext::UI_MODE_SERVICE);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    ECode ec = mService->DisableCarMode(flags);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "DisableCarMode: RemoteException");
    }
    return ec;
}

/**
 * Return the current running mode type.  May be one of
 * {@link Configuration#UI_MODE_TYPE_NORMAL Configuration.UI_MODE_TYPE_NORMAL},
 * {@link Configuration#UI_MODE_TYPE_DESK Configuration.UI_MODE_TYPE_DESK}, or
 * {@link Configuration#UI_MODE_TYPE_CAR Configuration.UI_MODE_TYPE_CAR}, or
 * {@link Configuration#UI_MODE_TYPE_TELEVISION Configuration.UI_MODE_TYPE_APPLIANCE}.
 */
ECode CUiModeManager::GetCurrentModeType(
    /* [out] */ Int32 *type)
{
    VALIDATE_NOT_NULL(type);

    if (mService == NULL) {
        Logger::E(TAG, "Service %s is not ready yet.", (const char*)IContext::UI_MODE_SERVICE);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    ECode ec = mService->GetCurrentModeType(type);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "GetCurrentModeType: RemoteException");
    }
    return ec;
}

/**
 * Sets the night mode.  Changes to the night mode are only effective when
 * the car or desk mode is enabled on a device.
 *
 * <p>The mode can be one of:
 * <ul>
 *   <li><em>{@link #MODE_NIGHT_NO}<em> - sets the device into notnight
 *       mode.</li>
 *   <li><em>{@link #MODE_NIGHT_YES}</em> - sets the device into night mode.
 *   </li>
 *   <li><em>{@link #MODE_NIGHT_AUTO}</em> - automatic night/notnight switching
 *       depending on the location and certain other sensors.</li>
 * </ul>
 */
ECode CUiModeManager::SetNightMode(
    /* [in] */ Int32 mode)
{
    if (mService == NULL) {
        Logger::E(TAG, "Service %s is not ready yet.", (const char*)IContext::UI_MODE_SERVICE);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    ECode ec = mService->SetNightMode(mode);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "SetNightMode: RemoteException");
    }
    return ec;
}

/**
 * Returns the currently configured night mode.
 *
 * @return {@link #MODE_NIGHT_NO}, {@link #MODE_NIGHT_YES}, or
 *  {@link #MODE_NIGHT_AUTO}.  When an error occurred -1 is returned.
 */
ECode CUiModeManager::GetNightMode(
    /* [out] */ Int32 *mode)
{
    VALIDATE_NOT_NULL(mode);

    if (mService == NULL) {
        Logger::E(TAG, "Service %s is not ready yet.", (const char*)IContext::UI_MODE_SERVICE);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    ECode ec = mService->GetNightMode(mode);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "GetNightMode: RemoteException");
    }
    return ec;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
