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

#include "elastos/droid/internal/utility/cm/ActionUtils.h"
#include "elastos/droid/internal/utility/cm/CActionUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CAR_INTERFACE_IMPL(CActionUtils, Singleton, IActionUtils)

CAR_SINGLETON_IMPL(CActionUtils)

ECode CActionUtils::KillForegroundApp(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    return ActionUtils::KillForegroundApp(context, userId, result);
}

ECode CActionUtils::SwitchToLastApp(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    return ActionUtils::SwitchToLastApp(context, userId, result);
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
