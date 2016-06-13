#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCVMFIXEDEXCEPTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCVMFIXEDEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/uicc/IccException.h"

// package com.android.internal.telephony.uicc;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class IccVmFixedException
    : public IccException
    , public IIccVmFixedException
{
public:
    CAR_INTERFACE_DECL();

    IccVmFixedException();

    IccVmFixedException(
        /* [in] */ const String& s);
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCVMFIXEDEXCEPTION_H__

