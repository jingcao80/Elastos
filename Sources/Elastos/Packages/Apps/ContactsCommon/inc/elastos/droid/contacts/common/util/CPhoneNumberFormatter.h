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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CPHONENUMBERFORMATTER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CPHONENUMBERFORMATTER_H__

#include "_Elastos_Apps_Contacts_Common_Util_CPhoneNumberFormatter.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CarClass(CPhoneNumberFormatter)
    , public singleton
    , public IPhoneNumberFormatter
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetPhoneNumberFormattingTextWatcher(
        /* [in] */ IContext* context,
        /* [in] */ ITextView* textView);
};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_CPHONENUMBERFORMATTER_H__
