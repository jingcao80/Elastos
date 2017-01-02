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

#include "elastos/droid/server/pm/LauncherAppsService.h"
#include "elastos/droid/server/pm/CLauncherAppsImpl.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

ECode LauncherAppsService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    return CLauncherAppsImpl::New(context, (IILauncherApps**)&mLauncherAppsImpl);
}

ECode LauncherAppsService::OnStart()
{
    PublishBinderService(IContext::LAUNCHER_APPS_SERVICE, IBinder::Probe(mLauncherAppsImpl));
    return NOERROR;
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
