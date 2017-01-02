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

#include "elastos/droid/dialer/interactions/CPhoneNumberInteraction.h"
#include "elastos/droid/dialer/interactions/PhoneNumberInteraction.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Interactions {

CAR_INTERFACE_IMPL(CPhoneNumberInteraction, Singleton, IPhoneNumberInteraction);

CAR_SINGLETON_IMPL(CPhoneNumberInteraction);

ECode CPhoneNumberInteraction::StartInteractionForPhoneCall(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri)
{
    PhoneNumberInteraction::StartInteractionForPhoneCall(activity, uri);
    return NOERROR;
}

ECode CPhoneNumberInteraction::StartInteractionForPhoneCall(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri,
    /* [in] */ Boolean useDefault)
{
    PhoneNumberInteraction::StartInteractionForPhoneCall(activity, uri, useDefault);
    return NOERROR;
}

ECode CPhoneNumberInteraction::StartInteractionForPhoneCall(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri,
    /* [in] */ const String& callOrigin)
{
    PhoneNumberInteraction::StartInteractionForPhoneCall(activity, uri, callOrigin);
    return NOERROR;
}

ECode CPhoneNumberInteraction::StartInteractionForTextMessage(
    /* [in] */ ITransactionSafeActivity* activity,
    /* [in] */ IUri* uri)
{
    PhoneNumberInteraction::StartInteractionForTextMessage(activity, uri);
    return NOERROR;
}

} // Interactions
} // Dialer
} // Droid
} // Elastos
