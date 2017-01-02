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

#include "elastos/droid/launcher2/CWorkspaceHelper.h"
#include "elastos/droid/launcher2/Workspace.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CWorkspaceHelper)

CAR_INTERFACE_IMPL(CWorkspaceHelper, Singleton, IWorkspaceHelper)

ECode CWorkspaceHelper::GetCellLayoutMetrics(
    /* [in] */ ILauncher* launcher,
    /* [in] */ Int32 orientation,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    return Workspace::GetCellLayoutMetrics(launcher, orientation, rect);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos