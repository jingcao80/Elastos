
#include "CDigestOutputStream.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CDigestOutputStream)
CAR_INTERFACE_IMPL(CDigestOutputStream, FilterOutputStream, IDigestOutputStream)
CDigestOutputStream::CDigestOutputStream()
    : mIsOn(TRUE)
{}

ECode CDigestOutputStream::constructor(
    /* [in] */ IOutputStream* stream,
    /* [in] */ IMessageDigest* digest)
{
    FilterOutputStream::constructor(stream);
    mDigest = digest;
    return NOERROR;
}

ECode CDigestOutputStream::GetMessageDigest(
    /* [out] */ IMessageDigest** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDigest;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CDigestOutputStream::SetMessageDigest(
    /* [in] */ IMessageDigest* digest)
{
    mDigest = digest;
    return NOERROR;
}

ECode CDigestOutputStream::Write(
    /* [in] */ Int32 b) /*throws IOException*/
{
    // update digest only if digest functionality is on
    if (mIsOn) {
        mDigest->Update((Byte)b);
    }
    // write the byte
    mOut->Write(b);
    return NOERROR;
}

ECode CDigestOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len) /*throws IOException*/
{
    // update digest only if digest functionality is on
    if (mIsOn) {
        mDigest->Update(b, off, len);
    }
    // write len bytes
    mOut->Write(b, off, len);
    return NOERROR;
}

ECode CDigestOutputStream::On(
    /* [in] */ Boolean on)
{
    mIsOn = on;
    return NOERROR;
}

ECode CDigestOutputStream::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String s;
    FilterOutputStream::ToString(&s);
    StringBuilder sb(s);
    sb.Append(", ");
    mDigest->ToString(&s);
    sb.Append(s);
    sb.Append(mIsOn ? ", is on" : ", is off");
    return sb.ToString(result);
}

} // namespace Security
} // namespace Elastos

