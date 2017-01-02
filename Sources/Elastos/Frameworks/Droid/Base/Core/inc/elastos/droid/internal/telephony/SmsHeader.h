//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    AutoPtr<PortAddrs> mPortAddrs;
    AutoPtr<ConcatRef> mConcatRef;
    //ArrayList<SpecialSmsMsg> specialSmsMsgList = new ArrayList<SpecialSmsMsg>();
    AutoPtr<IArrayList> mSpecialSmsMsgList;
    //ArrayList<MiscElt> miscEltList = new ArrayList<MiscElt>();
    AutoPtr<IArrayList> mMiscEltList;

    /** 7 bit national language locking shift table, or 0 for GSM default 7 bit alphabet. */
    Int32 mLanguageTable;

    /** 7 bit national language single shift table, or 0 for GSM default 7 bit extension table. */
    Int32 mLanguageShiftTable;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSHEADER_H__
