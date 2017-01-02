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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_NOTIFYRESPIND_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_NOTIFYRESPIND_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/pdu/GenericPdu.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

/**
  * M-NofifyResp.ind PDU.
  */
class NotifyRespInd
    : public GenericPdu
    , public INotifyRespInd
{
public:
    CAR_INTERFACE_DECL();

    NotifyRespInd();
    /**
      * Constructor, used when composing a M-NotifyResp.ind pdu.
      *
      * @param mmsVersion current version of mms
      * @param transactionId the transaction-id value
      * @param status the status value
      * @throws InvalidHeaderValueException if parameters are invalid.
      *         NullPointerException if transactionId is null.
      *         RuntimeException if an undeclared error occurs.
      */
    CARAPI constructor(
        /* [in] */ Int32 mmsVersion,
        /* [in] */ ArrayOf<Byte>* transactionId,
        /* [in] */ Int32 status);

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
      *         RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetReportAllowed(
        /* [in] */ Int32 value);

    /**
      * Set X-Mms-Status field value.
      *
      * @param value the value
      * @throws InvalidHeaderValueException if the value is invalid.
      *         RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetStatus(
        /* [in] */ Int32 value);

    /**
      * GetX-Mms-Status field value.
      *
      * @return the X-Mms-Status value
      */
    virtual CARAPI GetStatus(
        /* [out] */ Int32* result);

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
      *         RuntimeException if an undeclared error occurs.
      */
    virtual CARAPI SetTransactionId(
        /* [in] */ ArrayOf<Byte>* value);
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_NOTIFYRESPIND_H__
