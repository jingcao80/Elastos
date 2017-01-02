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

#include "CEngineHelper.h"
#include <CEngine.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Fortress {

CAR_SINGLETON_IMPL(CEngineHelper)

CAR_INTERFACE_IMPL(CEngineHelper, Singleton, IEngineHelper)

ECode CEngineHelper::SetDoor(
    /* [in] */ ISecurityAccess* door)
{
    CEngine::mDoor = door;
    return NOERROR;
}

ECode CEngineHelper::GetDoor(
    /* [out] */ ISecurityAccess** door)
{
    VALIDATE_NOT_NULL(door)
    *door = CEngine::mDoor;
    REFCOUNT_ADD(*door)
    return NOERROR;
}

} // namespace Fortress
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

