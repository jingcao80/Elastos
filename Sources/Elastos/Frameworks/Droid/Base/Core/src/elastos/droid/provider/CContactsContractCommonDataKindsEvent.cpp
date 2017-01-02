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

#include "elastos/droid/provider/CContactsContractCommonDataKindsEvent.h"
#include "elastos/droid/R.h"
#include <elastos/coredef.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractCommonDataKindsEvent)

CAR_INTERFACE_IMPL_3(CContactsContractCommonDataKindsEvent, Singleton
    , IContactsContractCommonDataKindsEvent
    , IContactsContractDataColumnsWithJoins
    , IContactsContractCommonDataKindsCommonColumns)

ECode CContactsContractCommonDataKindsEvent::GetTypeResource(
    /* [in] */ IInteger32* type,
    /* [out] */ Int32* resource)
{
    VALIDATE_NOT_NULL(resource);

    if (type == NULL) {
        *resource = Elastos::Droid::R::string::eventTypeOther;
        return NOERROR;
    }
    Int32 _type;
    FAIL_RETURN(type->GetValue(&_type))
    switch (_type) {
        case TYPE_ANNIVERSARY:
            *resource = Elastos::Droid::R::string::eventTypeAnniversary;
            break;
        case TYPE_BIRTHDAY:
            *resource = Elastos::Droid::R::string::eventTypeBirthday;
            break;
        case TYPE_OTHER:
            *resource = Elastos::Droid::R::string::eventTypeOther;
            break;
        default:
            *resource = Elastos::Droid::R::string::eventTypeCustom;
            break;
    }
     return NOERROR;
}

} //Provider
} //Droid
} //Elastos