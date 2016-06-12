
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_INBOUNDSMSTRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_INBOUNDSMSTRACKER_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::ICursor;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Tracker for an incoming SMS message ready to broadcast to listeners.
 * This is similar to {@link com.android.internal.telephony.SMSDispatcher.SmsTracker} used for
 * outgoing messages.
 */
class InboundSmsTracker
    : public Object
{
public:
    /**
     * Create a tracker for a single-part SMS.
     * @param pdu the message PDU
     * @param timestamp the message timestamp
     * @param destPort the destination port
     * @param is3gpp2 true for 3GPP2 format; false for 3GPP format
     * @param is3gpp2WapPdu true for 3GPP2 format WAP PDU; false otherwise
     */
    InboundSmsTracker(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int64 timestamp,
        /* [in] */ Int32 destPort,
        /* [in] */ Boolean is3gpp2,
        /* [in] */ Boolean is3gpp2WapPdu);

    /**
     * Create a tracker for a multi-part SMS. Sequence numbers start at 1 for 3GPP and regular
     * concatenated 3GPP2 messages, but CDMA WAP push sequence numbers start at 0. The caller
     * will subtract 1 if necessary so that the sequence number is always 0-based. When loading
     * and saving to the raw table, the sequence number is adjusted if necessary for backwards
     * compatibility.
     *
     * @param pdu the message PDU
     * @param timestamp the message timestamp
     * @param destPort the destination port
     * @param is3gpp2 true for 3GPP2 format; false for 3GPP format
     * @param address the originating address
     * @param referenceNumber the concatenated reference number
     * @param sequenceNumber the sequence number of this segment (0-based)
     * @param messageCount the total number of segments
     * @param is3gpp2WapPdu true for 3GPP2 format WAP PDU; false otherwise
     */
    InboundSmsTracker(
        /* [in] */ ArrayOf<Byte>* pdu,
        /* [in] */ Int64 timestamp,
        /* [in] */ Int32 destPort,
        /* [in] */ Boolean is3gpp2,
        /* [in] */ const String& address,
        /* [in] */ Int32 referenceNumber,
        /* [in] */ Int32 sequenceNumber,
        /* [in] */ Int32 messageCount,
        /* [in] */ Boolean is3gpp2WapPdu);

    /**
     * Create a new tracker from the row of the raw table pointed to by Cursor.
     * Since this constructor is used only for recovery during startup, the Dispatcher is null.
     * @param cursor a Cursor pointing to the row to construct this SmsTracker for
     */
    InboundSmsTracker(
        /* [in] */ ICursor* cursor,
        /* [in] */ Boolean isCurrentFormat3gpp2);

    CARAPI_(AutoPtr<IContentValues>) GetContentValues();

    /**
     * Get the port number, or -1 if there is no destination port.
     * @param destPort the destination port value, with flags
     * @return the real destination port, or -1 for no port
     */
    static CARAPI_(Int32) GetRealDestPort(
        /* [in] */ Int32 destPort);

    /**
     * Update the values to delete all rows of the message from raw table.
     * @param deleteWhere the selection to use
     * @param deleteWhereArgs the selection args to use
     */
    CARAPI_(void) SetDeleteWhere(
        /* [in] */ const String& deleteWhere,
        /* [in] */ ArrayOf<String>* deleteWhereArgs);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) GetPdu();

    CARAPI_(Int64) GetTimestamp();

    CARAPI_(Int32) GetDestPort();

    CARAPI_(Boolean) Is3gpp2();

    CARAPI_(String) GetFormat();

    /**
     * Sequence numbers for concatenated messages start at 1. The exception is CDMA WAP PDU
     * messages, which use a 0-based index.
     * @return the offset to use to convert between mIndex and the sequence number
     */
    CARAPI_(Int32) GetIndexOffset();

    CARAPI_(String) GetAddress();

    CARAPI_(Int32) GetReferenceNumber();

    CARAPI_(Int32) GetSequenceNumber();

    CARAPI_(Int32) GetMessageCount();

    CARAPI_(String) GetDeleteWhere();

    CARAPI_(AutoPtr<ArrayOf<String> >) GetDeleteWhereArgs();

private:
    // Fields for single and multi-part messages
    AutoPtr<ArrayOf<Byte> > mPdu;
    Int64 mTimestamp;
    Int32 mDestPort;
    Boolean mIs3gpp2;
    Boolean mIs3gpp2WapPdu;

    // Fields for concatenating multi-part SMS messages
    String mAddress;
    Int32 mReferenceNumber;
    Int32 mSequenceNumber;
    Int32 mMessageCount;

    // Fields for deleting this message after delivery
    String mDeleteWhere;
    AutoPtr<ArrayOf<String> > mDeleteWhereArgs;

    /** Destination port flag bit for no destination port. */
    static const Int32 DEST_PORT_FLAG_NO_PORT;

    /** Destination port flag bit to indicate 3GPP format message. */
    static const Int32 DEST_PORT_FLAG_3GPP;

    /** Destination port flag bit to indicate 3GPP2 format message. */
    static const Int32 DEST_PORT_FLAG_3GPP2;

    /** Destination port flag bit to indicate 3GPP2 format WAP message. */
    static const Int32 DEST_PORT_FLAG_3GPP2_WAP_PDU;

    /** Destination port mask (16-bit unsigned value on GSM and CDMA). */
    static const Int32 DEST_PORT_MASK;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_INBOUNDSMSTRACKER_H__
