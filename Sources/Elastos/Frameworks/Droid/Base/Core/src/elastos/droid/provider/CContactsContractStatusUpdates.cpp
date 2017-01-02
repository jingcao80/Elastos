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

#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/provider/ContactsContractProfile.h"
#include "elastos/droid/provider/CContactsContractStatusUpdates.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractStatusUpdates)

CAR_INTERFACE_IMPL_3(CContactsContractStatusUpdates, Singleton
    , IContactsContractStatusUpdates
    , IContactsContractStatusColumns
    , IContactsContractPresenceColumns)

ECode CContactsContractStatusUpdates::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), String("status_updates"), uri);
}

ECode CContactsContractStatusUpdates::GetPROFILE_CONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> conUri;
    FAIL_RETURN(ContactsContractProfile::GetCONTENT_URI((IUri**)&conUri))
    return Uri::WithAppendedPath(conUri, String("status_updates"), uri);
}

ECode CContactsContractStatusUpdates::GetPresenceIconResourceId(
    /* [in] */ Int32 status,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    switch (status) {
        case AVAILABLE:
            *id = Elastos::Droid::R::drawable::presence_online;
            break;
        case IDLE:
        case AWAY:
            *id = Elastos::Droid::R::drawable::presence_away;
            break;
        case DO_NOT_DISTURB:
            *id = Elastos::Droid::R::drawable::presence_busy;
            break;
        case INVISIBLE:
            *id = Elastos::Droid::R::drawable::presence_invisible;
            break;
        case OFFLINE:
        default:
            *id = Elastos::Droid::R::drawable::presence_offline;
            break;
    }
    return NOERROR;
}

ECode CContactsContractStatusUpdates::GetPresencePrecedence(
    /* [in] */ Int32 status,
    /* [out] */ Int32* precedence)
{
    VALIDATE_NOT_NULL(precedence);
    // Keep this function here incase we want to enforce a different precedence than the
    // natural order of the status constants.
    *precedence = status;
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos