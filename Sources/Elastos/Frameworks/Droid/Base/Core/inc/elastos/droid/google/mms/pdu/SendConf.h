#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_SENDCONF_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_SENDCONF_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/pdu/GenericPdu.h"

// package com.google.android.mms.pdu;
// import com.google.android.mms.InvalidHeaderValueException;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class SendConf
    : public GenericPdu
    , public ISentConf
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Empty constructor.
      * Since the Pdu corresponding to this class is constructed
      * by the Proxy-Relay server, this class is only instantiated
      * by the Pdu Parser.
      *
      * @throws InvalidHeaderValueException if error occurs.
      */
    SendConf();

    /**
      * Constructor with given headers.
      *
      * @param headers Headers for this PDU.
      */
    SendConf(
        /* [in] */ IPduHeaders* headers);

    /**
      * Get Message-ID value.
      *
      * @return the value
      */
    virtual CARAPI GetMessageId(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Set Message-ID value.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetMessageId(
        /* [in] */ ArrayOf<Byte>* value);

    /**
      * Get X-Mms-Response-Status.
      *
      * @return the value
      */
    virtual CARAPI GetResponseStatus(
        /* [out] */ Int32* result);

    /**
      * Set X-Mms-Response-Status.
      *
      * @param value the values
      * @throws InvalidHeaderValueException if the value is invalid.
      */
    virtual CARAPI SetResponseStatus(
        /* [in] */ Int32 value);

    /**
      * Get X-Mms-Transaction-Id field value.
      *
      * @return the X-Mms-Report-Allowed value
      */
    virtual CARAPI GetTransactionId(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Set X-Mms-Transaction-Id field value.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetTransactionId(
        /* [in] */ ArrayOf<Byte>* value);
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_SENDCONF_H__

