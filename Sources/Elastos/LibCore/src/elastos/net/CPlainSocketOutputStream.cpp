
#include "CPlainSocketOutputStream.h"
#include "PlainSocketImpl.h"

namespace Elastos {
namespace Net {

CPlainSocketOutputStream::CPlainSocketOutputStream()
{
}

ECode CPlainSocketOutputStream::constructor(
    /* [in] */ ISocketImpl * impl)
{
    if (IPlainSocketImpl::Probe(impl) == NULL) {
        return E_INVALID_ARGUMENT;
    }

    mSocket = impl;
    return NOERROR;
}

ECode CPlainSocketOutputStream::Close()
{
    return mSocket->Close();
}

ECode CPlainSocketOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1);
    (*buffer)[0] = (Byte) (oneByte & 0xFF);

    return Write(buffer, 0, 1);
}

ECode CPlainSocketOutputStream::Write(
    /* [in] */ ArrayOf<Byte> * buffer)
{
    if (buffer == NULL || buffer->GetLength() == 0) {
        return E_INVALID_ARGUMENT;
    }
    return Write(buffer, 0, buffer->GetLength());
}

ECode CPlainSocketOutputStream::Write(
    /* [in] */ ArrayOf<Byte> * buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    if (buffer == NULL || buffer->GetLength() == 0) {
        return E_INVALID_ARGUMENT;
    }

    if (0 <= offset && offset <= buffer->GetLength() && 0 <= count && count <= buffer->GetLength() - offset) {
        PlainSocketImpl* psi = (PlainSocketImpl*)IPlainSocketImpl::Probe(mSocket);
        Int32 number;
        return psi->Write(buffer, offset, count, &number);
    }
    else {
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
}

} // namespace Net
} // namespace Elastos
