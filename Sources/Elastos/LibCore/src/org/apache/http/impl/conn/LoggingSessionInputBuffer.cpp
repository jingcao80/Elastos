
#include "org/apache/http/impl/conn/LoggingSessionInputBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IO::EIID_ISessionInputBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

LoggingSessionInputBuffer::LoggingSessionInputBuffer(
    /* [in] */ ISessionInputBuffer* in,
    /* [in] */ Wire* wire)
    : mIn(in)
    , mWire(wire)
{}

CAR_INTERFACE_IMPL(LoggingSessionInputBuffer, Object, ISessionInputBuffer)

ECode LoggingSessionInputBuffer::IsDataAvailable(
    /* [in] */ Int32 timeout,
    /* [out] */ Boolean* isDataAvailable)
{
    VALIDATE_NOT_NULL(isDataAvailable)
    return mIn->IsDataAvailable(timeout, isDataAvailable);
}

ECode LoggingSessionInputBuffer::Read(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 l;
    mIn->Read(b,  off,  len, &l);
    if (mWire->Enabled() && l > 0) {
        mWire->Input(b, off, l);
    }
    *result = l;
    return NOERROR;
}

ECode LoggingSessionInputBuffer::Read(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 l;
    mIn->Read(&l);
    if (mWire->Enabled() && l > 0) {
        mWire->Input(l);
    }
    *result = l;
    return NOERROR;
}

ECode LoggingSessionInputBuffer::Read(
    /* [in] */ ArrayOf<Byte>* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 l;
    mIn->Read(b, &l);
    if (mWire->Enabled() && l > 0) {
        mWire->Input(b, 0, l);
    }
    *result = l;
    return NOERROR;
}

ECode LoggingSessionInputBuffer::ReadLine(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String s;
    mIn->ReadLine(&s);
    if (mWire->Enabled() && !s.IsNull()) {
        mWire->Input(s + String("[EOL]"));
    }
    *result = s;
    return NOERROR;
}

ECode LoggingSessionInputBuffer::ReadLine(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 l;
    mIn->ReadLine(buffer, &l);
    if (mWire->Enabled() && l > 0) {
        Int32 len;
        buffer->GetLength(&len);
        Int32 pos = len - l;
        AutoPtr< ArrayOf<Char32> > cs;
        buffer->GetBuffer((ArrayOf<Char32>**)&cs);
        String s(*cs, pos, l);
        mWire->Input(s + String("[EOL]"));
    }
    *result = l;
    return NOERROR;
}

ECode LoggingSessionInputBuffer::GetMetrics(
    /* [out] */ IHttpTransportMetrics** metrics)
{
    VALIDATE_NOT_NULL(metrics)
    return mIn->GetMetrics(metrics);
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org