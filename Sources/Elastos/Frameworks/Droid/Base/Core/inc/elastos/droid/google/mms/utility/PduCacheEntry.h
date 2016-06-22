#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHEENTRY_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHEENTRY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.google.android.mms.util;
// import com.google.android.mms.pdu.GenericPdu;

using Elastos::Droid::Google::Mms::Pdu::IGenericPdu;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

class PduCacheEntry
    : public Object
    , public IPduCacheEntry
{
public:
    CAR_INTERFACE_DECL();

    PduCacheEntry();

    CARAPI constructor(
        /* [in] */ IGenericPdu* pdu,
        /* [in] */ Int32 msgBox,
        /* [in] */ Int64 threadId);

    virtual CARAPI GetPdu(
        /* [out] */ IGenericPdu** result);

    virtual CARAPI GetMessageBox(
        /* [out] */ Int32* result);

    virtual CARAPI GetThreadId(
        /* [out] */ Int64* result);

private:
    /*const*/ AutoPtr<IGenericPdu> mPdu;
    /*const*/ Int32 mMessageBox;
    /*const*/ Int64 mThreadId;
};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHEENTRY_H__
