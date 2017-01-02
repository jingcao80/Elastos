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

#include "elastos/droid/dialer/contact/ContactUpdateService.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Contact {

CAR_INTERFACE_IMPL(ContactUpdateService, IntentService, IContactUpdateService);

ECode ContactUpdateService::constructor()
{
    IntentService::constructor(String("ContactUpdateService"));
    SetIntentRedelivery(TRUE);
    return NOERROR;
}

AutoPtr<IIntent> ContactUpdateService::CreateSetSuperPrimaryIntent(
    /* [in] */ IContext* context,
    /* [in] */ Int64 dataId)
{
    AutoPtr<IIntent> serviceIntent;
    CIntent::New(context, ECLSID_CContactUpdateService, (IIntent**)&serviceIntent);
    serviceIntent->PutExtra(IContactUpdateService::EXTRA_PHONE_NUMBER_DATA_ID, dataId);
    return serviceIntent;
}

ECode ContactUpdateService::OnHandleIntent(
    /* [in] */ IIntent* intent)
{
    // Currently this service only handles one type of update.
    Int64 dataId;
    intent->GetInt64Extra(IContactUpdateService::EXTRA_PHONE_NUMBER_DATA_ID, -1, &dataId);

    assert(0 && "TODO") ;
    // ContactUpdateUtils::SetSuperPrimary((IContext*)this, dataId);
    return NOERROR;
}


} // Contact
} // Dialer
} // Droid
} // Elastos
