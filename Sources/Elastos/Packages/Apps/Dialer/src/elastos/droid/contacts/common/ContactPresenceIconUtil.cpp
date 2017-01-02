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
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/contacts/common/ContactPresenceIconUtil.h"
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::IContactsContractStatusColumns;
using Elastos::Droid::Provider::IContactsContractStatusUpdates;
using Elastos::Droid::Provider::CContactsContractStatusUpdates;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

AutoPtr<IDrawable> ContactPresenceIconUtil::GetPresenceIcon(
        /* [in] */ IContext* context,
        /* [in] */ Int32 status)
{
    // We don't show the offline status in Contacts
    switch(status) {
        case IContactsContractStatusColumns::AVAILABLE:
        case IContactsContractStatusColumns::IDLE:
        case IContactsContractStatusColumns::AWAY:
        case IContactsContractStatusColumns::DO_NOT_DISTURB:
        case IContactsContractStatusColumns::INVISIBLE: {
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            AutoPtr<IContactsContractStatusUpdates> statusUpdates;
            CContactsContractStatusUpdates::AcquireSingleton((IContactsContractStatusUpdates**)&statusUpdates);
            Int32 id;
            statusUpdates->GetPresenceIconResourceId(status, &id);
            AutoPtr<IDrawable> drawable;
            res->GetDrawable(id, (IDrawable**)&drawable);
            return drawable;
        }
        case IContactsContractStatusColumns::OFFLINE:
        // The undefined status is treated as OFFLINE in getPresenceIconResourceId();
        default:
            return NULL;
    }
}

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
