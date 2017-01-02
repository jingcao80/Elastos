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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_SENDCONF_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_SENDCONF_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/pdu/GenericPdu.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class SendConf
    : public GenericPdu
    , public ISendConf
{
public:
    CAR_INTERFACE_DECL();

    SendConf();
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
      * Get X-Mms-Response-Status.
      *
      * @return the value
      */
    virtual CARAPI GetResponseStatus(
        /* [out] */ Int32* result);

    /**
      * Set X-Mms-Response-Status.
      *
      * @param value the values
      * @throws InvalidHeaderValueException if the value is invalid.
      */
    virtual CARAPI SetResponseStatus(
        /* [in] */ Int32 value);

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
      */
    virtual CARAPI SetTransactionId(
        /* [in] */ ArrayOf<Byte>* value);
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_SENDCONF_H__

