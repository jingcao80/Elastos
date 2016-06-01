#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_RETRIEVECONF_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_RETRIEVECONF_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/pdu/RetrieveConf/MultimediaMessagePdu.h"

// package com.google.android.mms.pdu;
// import com.google.android.mms.InvalidHeaderValueException;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

/**
  * M-Retrive.conf Pdu.
  */
class RetrieveConf
    : public MultimediaMessagePdu
    : public IRetrieveConf
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
    RetrieveConf();

    /**
      * Constructor with given headers.
      *
      * @param headers Headers for this PDU.
      */
    RetrieveConf(
        /* [in] */ IPduHeaders* headers);

    /**
      * Constructor with given headers and body
      *
      * @param headers Headers for this PDU.
      * @param body Body of this PDu.
      */
    RetrieveConf(
        /* [in] */ IPduHeaders* headers,
        /* [in] */ IPduBody* body);

    /**
      * Get CC value.
      *
      * @return the value
      */
    virtual CARAPI GetCc(
        /* [out] */ ArrayOf<IEncodedStringValue*>** result);

    /**
      * Add a "CC" value.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI AddCc(
        /* [in] */ IEncodedStringValue* value);

    /**
      * Get Content-type value.
      *
      * @return the value
      */
    virtual CARAPI GetContentType(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Set Content-type value.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetContentType(
        /* [in] */ ArrayOf<Byte>* value);

    /**
      * Get X-Mms-Delivery-Report value.
      *
      * @return the value
      */
    virtual CARAPI GetDeliveryReport(
        /* [out] */ Int32* result);

    /**
      * Set X-Mms-Delivery-Report value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      */
    virtual CARAPI SetDeliveryReport(
        /* [in] */ Int32 value);

    /**
      * Get From value.
      * From-value = Value-length
      *      (Address-present-token Encoded-string-value | Insert-address-token)
      *
      * @return the value
      */
    virtual CARAPI GetFrom(
        /* [out] */ IEncodedStringValue** result);

    /**
      * Set From value.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetFrom(
        /* [in] */ IEncodedStringValue* value);

    /**
      * Get X-Mms-Message-Class value.
      * Message-class-value = Class-identifier | Token-text
      * Class-identifier = Personal | Advertisement | Informational | Auto
      *
      * @return the value
      */
    virtual CARAPI GetMessageClass(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Set X-Mms-Message-Class value.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetMessageClass(
        /* [in] */ ArrayOf<Byte>* value);

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
      * Get X-Mms-Read-Report value.
      *
      * @return the value
      */
    virtual CARAPI GetReadReport(
        /* [out] */ Int32* result);

    /**
      * Set X-Mms-Read-Report value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      */
    virtual CARAPI SetReadReport(
        /* [in] */ Int32 value);

    /**
      * Get X-Mms-Retrieve-Status value.
      *
      * @return the value
      */
    virtual CARAPI GetRetrieveStatus(
        /* [out] */ Int32* result);

    /**
      * Set X-Mms-Retrieve-Status value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      */
    virtual CARAPI SetRetrieveStatus(
        /* [in] */ Int32 value);

    /**
      * Get X-Mms-Retrieve-Text value.
      *
      * @return the value
      */
    virtual CARAPI GetRetrieveText(
        /* [out] */ IEncodedStringValue** result);

    /**
      * Set X-Mms-Retrieve-Text value.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetRetrieveText(
        /* [in] */ IEncodedStringValue* value);

    /**
      * Get X-Mms-Transaction-Id.
      *
      * @return the value
      */
    virtual CARAPI GetTransactionId(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Set X-Mms-Transaction-Id.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetTransactionId(
        /* [in] */ ArrayOf<Byte>* value);

private:
    CARAPI_(AutoPtr<ArrayOf<Byte> >) GenerateTransactionId();
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_RETRIEVECONF_H__

