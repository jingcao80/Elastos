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

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsPresence.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsPresence)

CAR_INTERFACE_IMPL_4(CContactsPresence, Singleton, IContactsPresence, IBaseColumns, IContactsPresenceColumns, IContactsPeopleColumns)

ECode CContactsPresence::constructor()
{
    return NOERROR;
}

ECode CContactsPresence::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/presence"), uri);
}

ECode CContactsPresence::GetPresenceIconResourceId(
    /* [in] */ Int32 status,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    switch (status) {
        case IContactsPresenceColumns::AVAILABLE:
            *id = Elastos::Droid::R::drawable::presence_online;
            break;

        case IContactsPresenceColumns::IDLE:
        case IContactsPresenceColumns::AWAY:
            *id = Elastos::Droid::R::drawable::presence_away;
            break;

        case IContactsPresenceColumns::DO_NOT_DISTURB:
            *id = Elastos::Droid::R::drawable::presence_busy;
            break;

        case IContactsPresenceColumns::INVISIBLE:
            *id = Elastos::Droid::R::drawable::presence_invisible;
            break;

        case IContactsPresenceColumns::OFFLINE:
        default:
            *id = Elastos::Droid::R::drawable::presence_offline;
            break;
    }
    return NOERROR;
}

ECode CContactsPresence::SetPresenceIcon(
    /* [in] */ IImageView* icon,
    /* [in] */ Int32 serverStatus)
{
    Int32 id;
    FAIL_RETURN(GetPresenceIconResourceId(serverStatus, &id))
    return icon->SetImageResource(id);
}

} // Provider
} // Droid
} // Elastos