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

#include "elastos/droid/app/CNotificationManagerHelper.h"
#include "elastos/droid/app/NotificationManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CNotificationManagerHelper, Object, INotificationManagerHelper)

CAR_SINGLETON_IMPL(CNotificationManagerHelper)

/** @hide */
ECode CNotificationManagerHelper::GetService(
    /* [out] */ IINotificationManager** mgr)
{
    VALIDATE_NOT_NULL(mgr)
    AutoPtr<IINotificationManager> service = NotificationManager::GetService();
    *mgr = service;
    REFCOUNT_ADD(*mgr)
    return NOERROR;
}

/** @hide */
ECode CNotificationManagerHelper::From(
    /* [in] */ IContext* context,
    /* [out] */ INotificationManager** mgr)
{
    VALIDATE_NOT_NULL(mgr)
    AutoPtr<INotificationManager> service = NotificationManager::From(context);
    *mgr = service;
    REFCOUNT_ADD(*mgr)
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos