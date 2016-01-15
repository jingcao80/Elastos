
#include "CStructTimeval.h"

namespace Elastos {
namespace Droid {
namespace System {
CAR_OBJECT_IMPL(CStructTimeval)

CAR_INTERFACE_IMPL(CStructTimeval, Object, IStructTimeval)

ECode CStructTimeval::GetSec(
    /* [out] */ Int64* sec)
{
    VALIDATE_NOT_NULL(sec);

    *sec = mTv_sec;
    return NOERROR;
}

ECode CStructTimeval::GetUsec(
    /* [out] */ Int64* usec)
{
    VALIDATE_NOT_NULL(usec);

    *usec = mTv_usec;
    return NOERROR;
}

ECode CStructTimeval::ToMillis(
    /* [out] */ Int64* millis)
{
    VALIDATE_NOT_NULL(millis);

    *millis = (mTv_sec * 1000) + (mTv_usec / 1000);
    return NOERROR;
}

ECode CStructTimeval::constructor(
    /* [in] */ Int64 sec,
    /* [in] */ Int64 usec)
{
    mTv_sec = sec;
    mTv_usec = usec;
    return NOERROR;
}

ECode CStructTimeval::FromMillis(
    /* [in] */ Int64 millis,
    /* [out] */ IStructTimeval** tv)
{
    VALIDATE_NOT_NULL(tv);

    Int64 mTv_sec = millis / 1000;
    Int64 mTv_usec = (millis - (mTv_sec * 1000)) * 1000;
    return CStructTimeval::New(mTv_sec, mTv_usec, tv);
}

} // namespace System
} // namespace Droid
} // namespace Elastos
