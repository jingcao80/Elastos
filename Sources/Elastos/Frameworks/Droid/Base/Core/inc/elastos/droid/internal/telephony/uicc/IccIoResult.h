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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCIORESULT_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCIORESULT_H__

#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class IccIoResult
    : public Object
    , public IIccIoResult
{
public:
    CAR_INTERFACE_DECL();

    IccIoResult();

    CARAPI constructor(
        /* [in] */ Int32 sw1,
        /* [in] */ Int32 sw2,
        /* [in] */ ArrayOf<Byte>* payload);

    CARAPI constructor(
        /* [in] */ Int32 sw1,
        /* [in] */ Int32 sw2,
        /* [in] */ const String& hexString);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
      * true if this operation was successful
      * See GSM 11.11 Section 9.4
      * (the fun stuff is absent in 51.011)
      */
    virtual CARAPI Success(
        /* [out] */ Boolean* result);

    /**
      * Returns exception on error or null if success
      */
    virtual CARAPI GetException(
        /* [out] */ IIccException** result);

public:
    Int32 mSw1;
    Int32 mSw2;
    AutoPtr<ArrayOf<Byte> > mPayload;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCIORESULT_H__

