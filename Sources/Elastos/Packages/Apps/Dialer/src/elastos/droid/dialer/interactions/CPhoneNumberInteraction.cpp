
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
