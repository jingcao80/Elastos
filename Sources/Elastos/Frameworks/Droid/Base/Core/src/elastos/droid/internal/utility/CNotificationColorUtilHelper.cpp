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

#include "elastos/droid/internal/utility/CNotificationColorUtilHelper.h"
#include "elastos/droid/internal/utility/NotificationColorUtil.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CNotificationColorUtilHelper, Singleton, INotificationColorUtilHelper)

CAR_SINGLETON_IMPL(CNotificationColorUtilHelper)

ECode CNotificationColorUtilHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ INotificationColorUtil** instance)
{
    VALIDATE_NOT_NULL(instance)
    AutoPtr<INotificationColorUtil> temp = NotificationColorUtil::GetInstance(context);
    *instance = temp.Get();
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
