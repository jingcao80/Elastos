#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_READRECIND_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_READRECIND_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/pdu/GenericPdu.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class ReadRecInd
    : public GenericPdu
    , public IReadRecInd
{
public:
    CAR_INTERFACE_DECL();

    ReadRecInd();
    /**
      * Constructor, used when composing a M-ReadRec.ind pdu.
      *
      * @param from the from value
      * @param messageId the message ID value
      * @param mmsVersion current viersion of mms
      * @param readStatus the read status value
      * @param to the to value
      * @throws InvalidHeaderValueException if parameters are invalid.
      *         NullPointerException if messageId or to is null.
      */
    CARAPI constructor(
        /* [in] */ IEncodedStringValue* from,
        /* [in] */ ArrayOf<Byte>* messageId,
        /* [in] */ Int32 mmsVersion,
        /* [in] */ Int32 readStatus,
        /* [in] */ ArrayOf<IEncodedStringValue*>* to);

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
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetMessageId(
        /* [in] */ ArrayOf<Byte>* value);

    /**
      * Get To value.
      *
      * @return the value
      */
    virtual CARAPI GetTo(
        /* [out] */ ArrayOf<IEncodedStringValue*>** result);

    /**
      * Set To value.
      *
      * @param value the value
      * @throws NullPointerException if the value is null.
      */
    virtual CARAPI SetTo(
        /* [in] */ ArrayOf<IEncodedStringValue*>* value);

    /**
      * Get X-MMS-Read-status value.
      *
      * @return the value
      */
    virtual CARAPI GetReadStatus(
        /* [out] */ Int32* result);

    /**
      * Set X-MMS-Read-status value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      */
    virtual CARAPI SetReadStatus(
        /* [in] */ Int32 value);
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_READRECIND_H__
