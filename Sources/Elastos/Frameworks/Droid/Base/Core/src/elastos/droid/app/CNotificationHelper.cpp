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

#include "elastos/droid/app/CNotificationHelper.h"
#include "elastos/droid/app/CNotification.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CNotificationHelper, Object, INotificationHelper)

CAR_SINGLETON_IMPL(CNotificationHelper)

ECode CNotificationHelper::GetAUDIO_ATTRIBUTES_DEFAULT(
    /* [out] */ IAudioAttributes** attr)
{
    VALIDATE_NOT_NULL(attr);
    *attr = CNotification::AUDIO_ATTRIBUTES_DEFAULT;
    REFCOUNT_ADD(*attr);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
