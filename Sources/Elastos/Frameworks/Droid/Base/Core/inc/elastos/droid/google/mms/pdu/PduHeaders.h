#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUHEADERS_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUHEADERS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class PduHeaders
    : public Object
    , public IPduHeaders
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Constructor of PduHeaders.
      */
    PduHeaders();

protected:
    /**
      * Get octet value by header field.
      *
      * @param field the field
      * @return the octet value of the pdu header
      *          with specified header field. Return 0 if
      *          the value is not set.
      */
    CARAPI GetOctet(
        /* [in] */ Int32 field,
        /* [out] */ Int32* result);

    /**
      * Set octet value to pdu header by header field.
      *
      * @param value the value
      * @param field the field
      * @throws InvalidHeaderValueException if the value is invalid.
      */
    CARAPI SetOctet(
        /* [in] */ Int32 value,
        /* [in] */ Int32 field);

    /**
      * Get TextString value by header field.
      *
      * @param field the field
      * @return the TextString value of the pdu header
      *          with specified header field
      */
    CARAPI GetTextString(
        /* [in] */ Int32 field,
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Set TextString value to pdu header by header field.
      *
      * @param value the value
      * @param field the field
      * @return the TextString value of the pdu header
      *          with specified header field
      * @throws NullPointerException if the value is null.
      */
    CARAPI SetTextString(
        /* [in] */ ArrayOf<Byte>* value,
        /* [in] */ Int32 field);

    /**
      * Get EncodedStringValue value by header field.
      *
      * @param field the field
      * @return the EncodedStringValue value of the pdu header
      *          with specified header field
      */
    CARAPI GetEncodedStringValue(
        /* [in] */ Int32 field,
        /* [out] */ IEncodedStringValue** result);

    /**
      * Get TO, CC or BCC header value.
      *
      * @param field the field
      * @return the EncodeStringValue array of the pdu header
      *          with specified header field
      */
    CARAPI GetEncodedStringValues(
        /* [in] */ Int32 field,
        /* [out] */ ArrayOf<IEncodedStringValue*>** result);

    /**
      * Set EncodedStringValue value to pdu header by header field.
      *
      * @param value the value
      * @param field the field
      * @return the EncodedStringValue value of the pdu header
      *          with specified header field
      * @throws NullPointerException if the value is null.
      */
    CARAPI SetEncodedStringValue(
        /* [in] */ IEncodedStringValue* value,
        /* [in] */ Int32 field);

    /**
      * Set TO, CC or BCC header value.
      *
      * @param value the value
      * @param field the field
      * @return the EncodedStringValue value array of the pdu header
      *          with specified header field
      * @throws NullPointerException if the value is null.
      */
    CARAPI SetEncodedStringValues(
        /* [in] */ ArrayOf<IEncodedStringValue*>* value,
        /* [in] */ Int32 field);

    /**
      * Append one EncodedStringValue to another.
      *
      * @param value the EncodedStringValue to append
      * @param field the field
      * @throws NullPointerException if the value is null.
      */
    CARAPI AppendEncodedStringValue(
        /* [in] */ IEncodedStringValue* value,
        /* [in] */ Int32 field);

    /**
      * Get LongInteger value by header field.
      *
      * @param field the field
      * @return the LongInteger value of the pdu header
      *          with specified header field. if return -1, the
      *          field is not existed in pdu header.
      */
    CARAPI GetLongInteger(
        /* [in] */ Int32 field,
        /* [out] */ Int64* result);

    /**
      * Set LongInteger value to pdu header by header field.
      *
      * @param value the value
      * @param field the field
      */
    CARAPI SetLongInteger(
        /* [in] */ Int64 value,
        /* [in] */ Int32 field);

private:
    /**
      * The map contains the value of all headers.
      */
    AutoPtr<IHashMap> mHeaderMap; // Integer, Object
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUHEADERS_H__

