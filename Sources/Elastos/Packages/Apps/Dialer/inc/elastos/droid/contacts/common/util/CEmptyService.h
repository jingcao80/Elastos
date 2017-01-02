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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_CEMPTYSERVICE_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_CEMPTYSERVICE_H__

#include "_Elastos_Droid_Contacts_Common_Util_CEmptyService.h"
#include <elastos/droid/app/Service.h>

using Elastos::Droid::App::Service;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

CarClass(CEmptyService)
    , public Elastos::Droid::App::Service
{
public:
    CAR_OBJECT_DECL()

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);
};

} // Util
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_CEMPTYSERVICE_H__
