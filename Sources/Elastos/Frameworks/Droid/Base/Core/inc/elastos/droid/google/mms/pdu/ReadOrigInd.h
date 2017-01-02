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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_READORIGIND_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_READORIGIND_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/pdu/GenericPdu.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class ReadOrigInd
    : public GenericPdu
    , public IReadOrigInd
{
public:
    CAR_INTERFACE_DECL();

    ReadOrigInd();
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
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_READORIGIND_H__
