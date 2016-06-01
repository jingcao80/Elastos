#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_NOTIFICATIONIND_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_NOTIFICATIONIND_H__

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
  * M-Notification.ind PDU.
  */
class NotificationInd
    : public GenericPdu
    , public INotificationInd
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
      *         RuntimeException if an undeclared error occurs.
      */
    NotificationInd();

    /**
      * Constructor with given headers.
      *
      * @param headers Headers for this PDU.
      */
    NotificationInd(
        /* [in] */ IPduHeaders* headers);

    /**
      * Get X-Mms-Content-Class Value.
      *
      * @return the value
      */
    virtual CARAPI GetContentClass(
        /* [out] */ Int32* result);

    /**
      * Set X-Mms-Content-Class Value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      *         RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetContentClass(
        /* [in] */ Int32 value);

    /**
      * Get X-Mms-Content-Location value.
      * When used in a PDU other than M-Mbox-Delete.conf and M-Delete.conf:
      * Content-location-value = Uri-value
      *
      * @return the value
      */
    virtual CARAPI GetContentLocation(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Set X-Mms-Content-Location value.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      *         RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetContentLocation(
        /* [in] */ ArrayOf<Byte>* value);

    /**
      * Get X-Mms-Expiry value.
      *
      * Expiry-value = Value-length
      *      (Absolute-token Date-value | Relative-token Delta-seconds-value)
      *
      * @return the value
      */
    virtual CARAPI GetExpiry(
        /* [out] */ Int64* result);

    /**
      * Set X-Mms-Expiry value.
      *
      * @param value the value
      * @throws RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetExpiry(
        /* [in] */ Int64 value);

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
      *         RuntimeException if an undeclared error occurs.
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
      *         RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetMessageClass(
        /* [in] */ ArrayOf<Byte>* value);

    /**
      * Get X-Mms-Message-Size value.
      * Message-size-value = Long-integer
      *
      * @return the value
      */
    virtual CARAPI GetMessageSize(
        /* [out] */ Int64* result);

    /**
      * Set X-Mms-Message-Size value.
      *
      * @param value the value
      * @throws RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetMessageSize(
        /* [in] */ Int64 value);

    /**
      * Get subject.
      *
      * @return the value
      */
    virtual CARAPI GetSubject(
        /* [out] */ IEncodedStringValue** result);

    /**
      * Set subject.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      *         RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetSubject(
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
      *         RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetTransactionId(
        /* [in] */ ArrayOf<Byte>* value);

    /**
      * Get X-Mms-Delivery-Report Value.
      *
      * @return the value
      */
    virtual CARAPI GetDeliveryReport(
        /* [out] */ Int32* result);

    /**
      * Set X-Mms-Delivery-Report Value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      *         RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetDeliveryReport(
        /* [in] */ Int32 value);
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_NOTIFICATIONIND_H__
