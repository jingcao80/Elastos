
#include "CCheckedOutputStream.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CAR_INTERFACE_IMPL(CCheckedOutputStream, FilterOutputStream, ICheckedOutputStream)

CAR_OBJECT_IMPL(CCheckedOutputStream)

ECode CCheckedOutputStream::constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ IChecksum* cs)
{
    FAIL_RETURN(FilterOutputStream::constructor(os));
    mCheck = cs;
    return NOERROR;
}

ECode CCheckedOutputStream::GetChecksum(
    /* [out] */ IChecksum** checksum)
{
    VALIDATE_NOT_NULL(checksum);
    *checksum = mCheck;
    REFCOUNT_ADD(*checksum);
    return NOERROR;
}

ECode CCheckedOutputStream::Write(
    /* [in] */ Int32 val)
{
    FAIL_RETURN(mOut->Write(val));
    return mCheck->Update(val);
}

ECode CCheckedOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 off,
    /* [in] */ Int32 nbytes)
{
    FAIL_RETURN(mOut->Write(buf, off, nbytes));
    return mCheck->Update(buf, off, nbytes);
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
