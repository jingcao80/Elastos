//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "org/apache/http/impl/conn/LoggingSessionOutputBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IO::EIID_ISessionOutputBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

LoggingSessionOutputBuffer::LoggingSessionOutputBuffer(
    /* [in] */ ISessionOutputBuffer* out,
    /* [in] */ Wire* wire)
    : mOut(out)
    , mWire(wire)
{}

CAR_INTERFACE_IMPL(LoggingSessionOutputBuffer, Object, ISessionOutputBuffer)

ECode LoggingSessionOutputBuffer::Write(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    mOut->Write(b,  off,  len);
    if (mWire->Enabled()) {
        mWire->Output(b, off, len);
    }
    return NOERROR;
}

ECode LoggingSessionOutputBuffer::Write(
    /* [in] */ Int32 b)
{
    mOut->Write(b);
    if (mWire->Enabled()) {
        mWire->Output(b);
    }
    return NOERROR;
}

ECode LoggingSessionOutputBuffer::Write(
    /* [in] */ ArrayOf<Byte>* b)
{
    mOut->Write(b);
    if (mWire->Enabled()) {
        mWire->Output(b);
    }
    return NOERROR;
}

ECode LoggingSessionOutputBuffer::Flush()
{
    return mOut->Flush();
}

ECode LoggingSessionOutputBuffer::WriteLine(
    /* [in] */ ICharArrayBuffer* buffer)
{
    mOut->WriteLine(buffer);
    if (mWire->Enabled()) {
        Int32 len;
        buffer->GetLength(&len);
        AutoPtr< ArrayOf<Char32> > cs;
        buffer->GetBuffer((ArrayOf<Char32>**)&cs);
        String s(*cs, 0, len);
        mWire->Output(s + String("[EOL]"));
    }
    return NOERROR;
}

ECode LoggingSessionOutputBuffer::WriteLine(
    /* [in] */ const String& s)
{
    mOut->WriteLine(s);
    if (mWire->Enabled()) {
        mWire->Output(s + String("[EOL]"));
    }
    return NOERROR;
}

ECode LoggingSessionOutputBuffer::GetMetrics(
    /* [out] */ IHttpTransportMetrics** metrics)
{
    VALIDATE_NOT_NULL(metrics)
    return mOut->GetMetrics(metrics);
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org