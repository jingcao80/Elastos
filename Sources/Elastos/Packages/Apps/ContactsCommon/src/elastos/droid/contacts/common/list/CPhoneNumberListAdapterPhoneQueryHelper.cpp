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

#include "CPhoneNumberListAdapterPhoneQueryHelper.h"
#include "CPhoneNumberListAdapter.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

CAR_INTERFACE_IMPL(CPhoneNumberListAdapterPhoneQueryHelper, Singleton, IPhoneNumberListAdapterPhoneQueryHelper)

CAR_SINGLETON_DECL(CPhoneNumberListAdapterPhoneQueryHelper)

ECode CPhoneNumberListAdapterPhoneQueryHelper::GetPROJECTION_PRIMARY(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALUE_NOT_NULL(result);

    *result = ArrayOf<String>::Alloc(
            CPhoneNumberListAdapter::PhoneQuery::PROJECTION_PRIMARY,
            sizeof(CPhoneNumberListAdapter::PhoneQuery::PROJECTION_PRIMARY) / sizeof(String));
    return NOERROR;
}

ECode CPhoneNumberListAdapterPhoneQueryHelper::GetPROJECTION_ALTERNATIVE(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALUE_NOT_NULL(result);

    *result = ArrayOf<String>::Alloc(
            CPhoneNumberListAdapter::PhoneQuery::PROJECTION_ALTERNATIVE,
            sizeof(CPhoneNumberListAdapter::PhoneQuery::PROJECTION_ALTERNATIVE) / sizeof(String));
    return NOERROR;
}

} // List
} // Common
} // Contacts
} // Apps
} // Elastos
