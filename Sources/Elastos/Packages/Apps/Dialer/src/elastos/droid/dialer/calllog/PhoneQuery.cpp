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

#include "elastos/droid/dialer/calllog/PhoneQuery.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Provider::IContactsContractPhoneLookupColumns;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

static AutoPtr<ArrayOf<String> > InitProjection()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(9);
    (*projection)[0] = IBaseColumns::ID;
    (*projection)[1] = IContactsContractContactsColumns::DISPLAY_NAME;
    (*projection)[2] = IContactsContractPhoneLookupColumns::TYPE;
    (*projection)[3] = IContactsContractPhoneLookupColumns::LABEL;
    (*projection)[4] = IContactsContractPhoneLookupColumns::NUMBER;
    (*projection)[5] = IContactsContractPhoneLookupColumns::NORMALIZED_NUMBER;
    (*projection)[6] = IContactsContractContactsColumns::PHOTO_ID;
    (*projection)[7] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*projection)[8] = IContactsContractContactsColumns::PHOTO_URI;
    return projection;
}
const AutoPtr<ArrayOf<String> > PhoneQuery::_PROJECTION = InitProjection();
const Int32 PhoneQuery::PERSON_ID;
const Int32 PhoneQuery::NAME;
const Int32 PhoneQuery::PHONE_TYPE;
const Int32 PhoneQuery::LABEL;
const Int32 PhoneQuery::MATCHED_NUMBER;
const Int32 PhoneQuery::NORMALIZED_NUMBER;
const Int32 PhoneQuery::PHOTO_ID;
const Int32 PhoneQuery::LOOKUP_KEY;
const Int32 PhoneQuery::PHOTO_URI;

} // CallLog
} // Dialer
} // Droid
} // Elastos
