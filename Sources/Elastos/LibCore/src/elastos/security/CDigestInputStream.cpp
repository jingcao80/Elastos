
#include "CDigestInputStream.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CDigestInputStream)
CAR_INTERFACE_IMPL(CDigestInputStream, FilterInputStream, IDigestInputStream)

CDigestInputStream::CDigestInputStream()
    : mIsOn(TRUE)
{}

ECode CDigestInputStream::constructor(
    /* [in] */ IInputStream* stream,
    /* [in] */ IMessageDigest* digest)
{
    FAIL_RETURN(FilterInputStream::constructor(stream))
    mDigest = digest;
    return NOERROR;
}

ECode CDigestInputStream::GetMessageDigest(
    /* [out] */ IMessageDigest** digest)
{
    VALIDATE_NOT_NULL(digest)
    *digest = mDigest;
    REFCOUNT_ADD(*digest)
    return NOERROR;
}

ECode CDigestInputStream::SetMessageDigest(
    /* [in] */ IMessageDigest* digest)
{
    mDigest = digest;
    return NOERROR;
}

ECode CDigestInputStream::Read(
    /* [out] */ Int32* byteRead)
{
    VALIDATE_NOT_NULL(byteRead)

    // read the next byte
    FAIL_RETURN(mIn->Read(byteRead))
    // update digest only if
    // - digest functionality is on
    // - eos has not been reached
    if (mIsOn && (*byteRead != -1)) {
        mDigest->Update((Byte)*byteRead);
    }
    // return byte read
    return NOERROR;
}

ECode CDigestInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* bytesRead)
{
    VALIDATE_NOT_NULL(bytesRead)

    FAIL_RETURN(in->Read(buffer, byteOffset, byteCount, bytesRead))
    // update digest only if
    // - digest functionality is on
    // - eos has not been reached
    if (isOn && (*bytesRead != -1)) {
        mDigest->Update(buffer, byteOffset, *bytesRead);
    }
    // return number of bytes read
    return NOERROR;
}

ECode CDigestInputStream::On(
    /* [in] */ Boolean on)
{
    mIsOn = on;
    return NOERROR;
}

ECode CDigestInputStream::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    FilterInputStream::ToString(str);
    StringBuilder sb(*str);
    sb += ", ";
    sb += TO_STR(mDigest);
    sb += (mIsOn ? ", is on" : ", is off");
    return sb.ToString(str);
}

} // namespace Security
} // namespace Elastos

