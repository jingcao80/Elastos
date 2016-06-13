#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCIORESULT_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCIORESULT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.android.internal.telephony.uicc;

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
    Int32 sw1;
    Int32 sw2;
    AutoPtr<ArrayOf<Byte> > payload;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCIORESULT_H__

