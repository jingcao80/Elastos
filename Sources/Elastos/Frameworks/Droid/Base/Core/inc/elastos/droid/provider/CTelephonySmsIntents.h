#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSINTENTS_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSINTENTS_H__

#include "_Elastos_Droid_Provider_CTelephonySmsIntents.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonySmsIntents)
    , public Singleton
    , public ITelephonySmsIntents
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    /**
      * Read the PDUs out of an {@link #SMS_RECEIVED_ACTION} or a
      * {@link #DATA_SMS_RECEIVED_ACTION} intent.
      *
      * @param intent the intent to read from
      * @return an array of SmsMessages for the PDUs
      */
    CARAPI GetMessagesFromIntent(
        /* [in] */ IIntent* intent,
        /* [out, callee] */ ArrayOf<ISmsMessage*>** result);

    /**
      * Read the normalized addresses out of PDUs
      * @param pdus bytes for PDUs
      * @param format the format of the message
      * @return a list of Addresses for the PDUs
      * @hide
      */
    CARAPI GetNormalizedAddressesFromPdus(
        /* [in] */ ArrayOf<ArrayOf<Byte>*>* pdus,
        /* [in] */ const String& format,
        /* [out] */ IList** result);

};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSINTENTS_H__
