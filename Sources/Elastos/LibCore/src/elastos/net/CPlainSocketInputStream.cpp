
#include "CPlainSocketInputStream.h"
#include "PlainSocketImpl.h"

namespace Elastos {
namespace Net {

CPlainSocketInputStream::CPlainSocketInputStream()
{
}

ECode CPlainSocketInputStream::constructor(
    /* [in] */ ISocketImpl* impl)
{
    if (IPlainSocketImpl::Probe(impl) == NULL) {
        return E_INVALID_ARGUMENT;
    }

    mSocket = impl;
    return NOERROR;
}

ECode CPlainSocketInputStream::Available(
    /* [out] */ Int32* number)
{
    return mSocket->Available(number);
}

ECode CPlainSocketInputStream::Close()
{
    return mSocket->Close();;
}

ECode CPlainSocketInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    PlainSocketImpl* psi = (PlainSocketImpl*)IPlainSocketImpl::Probe(mSocket);

    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1);
    Int32 result;
    psi->Read(buffer, 0, 1, &result);
    if (result == -1) {
        *value = -1;
    }
    else {
        *value = (*buffer)[0] & 0xFF;
    }
    return NOERROR;
}

ECode CPlainSocketInputStream::Read(
    /* [out] */ ArrayOf<Byte> * buffer,
    /* [out] */ Int32* number)
{
    return Read(buffer, 0, buffer->GetLength(), number);
}

ECode CPlainSocketInputStream::Read(
    /* [out] */ ArrayOf<Byte> * buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;

    if (buffer == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (length == 0) {
        *number = 0;
        return NOERROR;
    }

    if (0 > offset || offset >= buffer->GetLength()) {
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
    if (0 > length || offset + length > buffer->GetLength()) {
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }

    PlainSocketImpl* psi = (PlainSocketImpl*)IPlainSocketImpl::Probe(mSocket);
    return psi->Read(buffer, offset, length, number);
}

} // namespace Net
} // namespace Elastos
