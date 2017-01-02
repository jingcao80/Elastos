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

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/contacts/common/ContactStatusUtil.h"
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::IContactsContractStatusColumns;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

const String ContactStatusUtil::TAG("ContactStatusUtil");

String ContactStatusUtil::GetStatusString(
    /* [in] */ IContext* context,
    /* [in] */ Int32 presence)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    String str;
    switch (presence) {
        case IContactsContractStatusColumns::AVAILABLE:
            resources->GetString(Elastos::Droid::Dialer::R::string::status_available, &str);
            return str;
        case IContactsContractStatusColumns::IDLE:
        case IContactsContractStatusColumns::AWAY:
            resources->GetString(Elastos::Droid::Dialer::R::string::status_away, &str);
            return str;
        case IContactsContractStatusColumns::DO_NOT_DISTURB:
            resources->GetString(Elastos::Droid::Dialer::R::string::status_busy, &str);
            return str;
        case IContactsContractStatusColumns::OFFLINE:
        case IContactsContractStatusColumns::INVISIBLE:
        default:
            return String(NULL);
    }
}

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
