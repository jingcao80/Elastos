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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_GENERICPDU_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_GENERICPDU_H__

#include "elastos/droid/google/mms/pdu/EncodedStringValue.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class GenericPdu
    : public Object
    , public IGenericPdu
{
public:
    CAR_INTERFACE_DECL();

    GenericPdu();
    /**
      * Constructor.
      */
    CARAPI constructor();

    /**
      * Constructor.
      *
      * @param headers Headers for this PDU.
      */
    CARAPI constructor(
        /* [in] */ IPduHeaders* headers);

    /**
      * Get the headers of this PDU.
      *
      * @return A PduHeaders of this PDU.
      */
    virtual CARAPI GetPduHeaders(
        /* [out] */ IPduHeaders** result);

    /**
      * Get X-Mms-Message-Type field value.
      *
      * @return the X-Mms-Report-Allowed value
      */
    virtual CARAPI GetMessageType(
        /* [out] */ Int32* result);

    /**
      * Set X-Mms-Message-Type field value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      *         RuntimeException if field's value is not Octet.
      */
    virtual CARAPI SetMessageType(
        /* [in] */ Int32 value);

    /**
      * Get X-Mms-MMS-Version field value.
      *
      * @return the X-Mms-MMS-Version value
      */
    virtual CARAPI GetMmsVersion(
        /* [out] */ Int32* result);

    /**
      * Set X-Mms-MMS-Version field value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      *         RuntimeException if field's value is not Octet.
      */
    virtual CARAPI SetMmsVersion(
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

public:
    /**
      * The headers of pdu.
      */
    AutoPtr<IPduHeaders> mPduHeaders;
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_GENERICPDU_H__

