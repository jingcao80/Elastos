
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSHEADER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSHEADER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SmsHeader
    : public Object
    , public ISmsHeader
{
public:
    class PortAddrs : public Object
    {
    public:
        Int32 destPort;
        Int32 origPort;
        Boolean areEightBits;
    };

    class ConcatRef : public Object
    {
    public:
        Int32 refNumber;
        Int32 seqNumber;
        Int32 msgCount;
        Boolean isEightBits;
    };

    class SpecialSmsMsg : public Object
    {
    public:
        Int32 msgIndType;
        Int32 msgCount;
    };

    /**
     * A header element that is not explicitly parsed, meaning not
     * PortAddrs or ConcatRef or SpecialSmsMsg.
     */
    class MiscElt : public Object
    {
    public:
          Int32 id;
          AutoPtr<ArrayOf<Byte> > data;
    };


public:
    CAR_INTERFACE_DECL()

    SmsHeader();

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Create structured SmsHeader object from serialized byte array representation.
     * (see TS 23.040 9.2.3.24)
     * @param data is user data header bytes
     * @return SmsHeader object
     */
    static CARAPI FromByteArray(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ ISmsHeader** result);

    /**
     * Create serialized Byte array representation from structured SmsHeader object.
     * (see TS 23.040 9.2.3.24)
     * @return Byte array representing the SmsHeader
     */
    static CARAPI ToByteArray(
        /* [in] */ ISmsHeader* smsHeader,
        /* [out, callee] */ ArrayOf<Byte>** result);

public:
    AutoPtr<PortAddrs> portAddrs;
    AutoPtr<ConcatRef> concatRef;
    //ArrayList<SpecialSmsMsg> specialSmsMsgList = new ArrayList<SpecialSmsMsg>();
    AutoPtr<IArrayList> specialSmsMsgList;
    //ArrayList<MiscElt> miscEltList = new ArrayList<MiscElt>();
    AutoPtr<IArrayList> miscEltList;

    /** 7 bit national language locking shift table, or 0 for GSM default 7 bit alphabet. */
    Int32 languageTable;

    /** 7 bit national language single shift table, or 0 for GSM default 7 bit extension table. */
    Int32 languageShiftTable;

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSHEADER_H__
