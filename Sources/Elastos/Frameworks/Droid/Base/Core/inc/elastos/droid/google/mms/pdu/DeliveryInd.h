#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_DELIVERYIND_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_DELIVERYIND_H__

#include "elastos/droid/google/mms/pdu/EncodedStringValue.h"
#include "elastos/droid/google/mms/pdu/GenericPdu.h"
#include "elastos/droid/ext/frameworkext.h"

// package com.google.android.mms.pdu;
// import com.google.android.mms.InvalidHeaderValueException;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

/**
  * M-Delivery.Ind Pdu.
  */
class DeliveryInd
    : public GenericPdu
    , public IDeliveryInd
{
public:
    CAR_INTERFACE_DECL();

    DeliveryInd();
    /**
      * Empty constructor.
      * Since the Pdu corresponding to this class is constructed
      * by the Proxy-Relay server, this class is only instantiated
      * by the Pdu Parser.
      *
      * @throws InvalidHeaderValueException if error occurs.
      */
    CARAPI constructor();

    /**
      * Constructor with given headers.
      *
      * @param headers Headers for this PDU.
      */
    CARAPI constructor(
        /* [in] */ IPduHeaders* headers);

    /**
      * Get Date value.
      *
      * @return the value
      */
    virtual CARAPI GetDate(
        /* [out] */ Int64* result);

    /**
      * Set Date value.
      *
      * @param value the value
      */
    virtual CARAPI SetDate(
        /* [in] */ Int64 value);

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
      * @param value the value, should not be null
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetMessageId(
        /* [in] */ ArrayOf<Byte>* value);

    /**
      * Get Status value.
      *
      * @return the value
      */
    virtual CARAPI GetStatus(
        /* [out] */ Int32* result);

    /**
      * Set Status value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      */
    virtual CARAPI SetStatus(
        /* [in] */ Int32 value);

    /**
      * Get To value.
      *
      * @return the value
      */
    virtual CARAPI GetTo(
        /* [out] */ ArrayOf<IEncodedStringValue*>** result);

    /**
      * set To value.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetTo(
        /* [in] */ ArrayOf<IEncodedStringValue*>* value);
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_DELIVERYIND_H__
