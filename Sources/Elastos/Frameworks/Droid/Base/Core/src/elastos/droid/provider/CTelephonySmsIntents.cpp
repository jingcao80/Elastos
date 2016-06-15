#include "elastos/droid/provider/CTelephonySmsIntents.h"
#include "elastos/droid/provider/Telephony.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL(CTelephonySmsIntents, Singleton, ITelephonySmsIntents);
CAR_SINGLETON_IMPL(CTelephonySmsIntents);

ECode CTelephonySmsIntents::GetMessagesFromIntent(
    /* [in] */ IIntent* intent,
    /* [out, callee] */ ArrayOf<ISmsMessage*>** result)
{
    return Telephony::Sms::Intents::GetMessagesFromIntent(intent, result);
}

ECode CTelephonySmsIntents::GetNormalizedAddressesFromPdus(
    /* [in] */ ArrayOf<ArrayOf<Byte>*>* pdus,
    /* [in] */ const String& format,
    /* [out] */ IList** result)
{
    return Telephony::Sms::Intents::GetNormalizedAddressesFromPdus(pdus, format, result);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
