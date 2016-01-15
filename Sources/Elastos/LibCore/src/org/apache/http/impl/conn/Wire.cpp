
#include "org/apache/http/impl/conn/Wire.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/StringUtils.h"
#include "elastos/io/CByteArrayInputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

void Wire::DoWire(
    /* [in] */ const String& header,
    /* [in] */ IInputStream* instream)
{
    StringBuilder buffer;
    Int32 ch;
    while (instream->Read(&ch), ch != -1) {
        if (ch == 13) {
            buffer.Append("[\\r]");
        }
        else if (ch == 10) {
            buffer.Append("[\\n]\"");
            buffer.Insert(0, String("\""));
            buffer.Insert(0, header);
            // log.debug(buffer.toString());
            buffer.SetLength(0);
        }
        else if ((ch < 32) || (ch > 127)) {
            buffer.Append("[0x");
            buffer.Append(StringUtils::ToHexString(ch));
            buffer.Append("]");
        }
        else {
            buffer.Append(StringUtils::ToString(ch));
        }
    }
    if (buffer.GetLength() > 0) {
        buffer.AppendChar('\"');
        buffer.Insert(0, '\"');
        buffer.Insert(0, header);
        // log.debug(buffer.toString());
    }
}

Boolean Wire::Enabled()
{
    // return log.isDebugEnabled();
    assert(0);
}

ECode Wire::Output(
    /* [in] */ IInputStream* outstream)
{
    if (outstream == NULL) {
        Logger::E("Wire", "Output may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    DoWire(String(">> "), outstream);
    return NOERROR;
}

ECode Wire::Input(
    /* [in] */ IInputStream* instream)
{
    if (instream == NULL) {
        Logger::E("Wire", "Input may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    DoWire(String("<< "), instream);
    return NOERROR;
}

ECode Wire::Output(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (b == NULL) {
        Logger::E("Wire", "Output may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IByteArrayInputStream> ins;
    CByteArrayInputStream::New(b, off, len, (IByteArrayInputStream**)&ins);
    AutoPtr<IInputStream> in = IInputStream::Probe(ins);
    DoWire(String(">> "), in);
    return NOERROR;
}

ECode Wire::Input(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (b == NULL) {
        Logger::E("Wire", "Input may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IByteArrayInputStream> ins;
    CByteArrayInputStream::New(b, off, len, (IByteArrayInputStream**)&ins);
    AutoPtr<IInputStream> in = IInputStream::Probe(ins);
    DoWire(String("<< "), in);
    return NOERROR;
}

ECode Wire::Output(
    /* [in] */ ArrayOf<Byte>* b)
{
    if (b == NULL) {
        Logger::E("Wire", "Output may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IByteArrayInputStream> ins;
    CByteArrayInputStream::New(b, (IByteArrayInputStream**)&ins);
    AutoPtr<IInputStream> in = IInputStream::Probe(ins);
    DoWire(String(">> "), in);
    return NOERROR;
}

ECode Wire::Input(
    /* [in] */ ArrayOf<Byte>* b)
{
    if (b == NULL) {
        Logger::E("Wire", "Input may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IByteArrayInputStream> ins;
    CByteArrayInputStream::New(b, (IByteArrayInputStream**)&ins);
    AutoPtr<IInputStream> in = IInputStream::Probe(ins);
    DoWire(String("<< "), in);
    return NOERROR;
}

ECode Wire::Output(
    /* [in] */ Int32 b)
{
    AutoPtr< ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(1);
    (*array)[0] = (Byte)b;
    return Output(array);
}

ECode Wire::Input(
    /* [in] */ Int32 b)
{
    AutoPtr< ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(1);
    (*array)[0] = (Byte)b;
    return Input(array);
}

ECode Wire::Output(
    /* [in] */ const String& s)
{
    if (s.IsNull()) {
        Logger::E("Wire", "Output may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr< ArrayOf<Byte> > b = s.GetBytes();
    return Output(b);
}

ECode Wire::Input(
    /* [in] */ const String& s)
{
    if (s.IsNull()) {
        Logger::E("Wire", "Input may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr< ArrayOf<Byte> > b = s.GetBytes();
    return Input(b);
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org