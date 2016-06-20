#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_USIMSERVICETABLE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_USIMSERVICETABLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/uicc/IccServiceTable.h"

// package com.android.internal.telephony.uicc;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * Wrapper class for the USIM Service Table EF.
  * See 3GPP TS 31.102 Release 10 section 4.2.8
  */
class UsimServiceTable
    : public IccServiceTable
    , public IUsimServiceTable
{
public:
    CAR_INTERFACE_DECL();

    UsimServiceTable();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* table);

    virtual CARAPI IsAvailable(
        /* [in] */ UsimService service,
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI_(String) GetTag();

    // @Override
    CARAPI_(AutoPtr<ArrayOf<IInterface*> >) GetValues();
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_USIMSERVICETABLE_H__
