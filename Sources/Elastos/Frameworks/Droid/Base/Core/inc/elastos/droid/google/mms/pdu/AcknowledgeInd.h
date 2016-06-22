#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_ACKNOWLEDGEIND_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_ACKNOWLEDGEIND_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/pdu/GenericPdu.h"

// package com.google.android.mms.pdu;
// import com.google.android.mms.InvalidHeaderValueException;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

/**
  * M-Acknowledge.ind PDU.
  */
class AcknowledgeInd
    : public GenericPdu
    , public IAcknowledgeInd
{
public:
    CAR_INTERFACE_DECL();

    AcknowledgeInd();
    /**
      * Constructor, used when composing a M-Acknowledge.ind pdu.
      *
      * @param mmsVersion current viersion of mms
      * @param transactionId the transaction-id value
      * @throws InvalidHeaderValueException if parameters are invalid.
      *         NullPointerException if transactionId is null.
      */
    CARAPI constructor(
        /* [in] */ Int32 mmsVersion,
        /* [in] */ ArrayOf<Byte>* transactionId);

    /**
      * Constructor with given headers.
      *
      * @param headers Headers for this PDU.
      */
    CARAPI constructor(
        /* [in] */ IPduHeaders* headers);

    /**
      * Get X-Mms-Report-Allowed field value.
      *
      * @return the X-Mms-Report-Allowed value
      */
    virtual CARAPI GetReportAllowed(
        /* [out] */ Int32* result);

    /**
      * Set X-Mms-Report-Allowed field value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      */
    virtual CARAPI SetReportAllowed(
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

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_ACKNOWLEDGEIND_H__
