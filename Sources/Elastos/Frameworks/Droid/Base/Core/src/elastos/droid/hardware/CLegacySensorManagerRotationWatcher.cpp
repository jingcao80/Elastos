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

#include "elastos/droid/hardware/CLegacySensorManagerRotationWatcher.h"
#include "elastos/droid/hardware/LegacySensorManager.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CAR_INTERFACE_IMPL(CLegacySensorManagerRotationWatcher, Object, IRotationWatcher);

CAR_OBJECT_IMPL(CLegacySensorManagerRotationWatcher);

CLegacySensorManagerRotationWatcher::CLegacySensorManagerRotationWatcher()
{}

ECode CLegacySensorManagerRotationWatcher::constructor()
{
    return NOERROR;
}

ECode CLegacySensorManagerRotationWatcher::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    LegacySensorManager::OnRotationChanged(rotation);
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos