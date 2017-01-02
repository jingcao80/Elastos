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

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/impl/io/AbstractMessageWriter.h"
#include "org/apache/http/message/BasicLineFormatter.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeaderIterator;
using Org::Apache::Http::IO::EIID_IHttpMessageWriter;
using Org::Apache::Http::Message::BasicLineFormatter;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

AbstractMessageWriter::AbstractMessageWriter(
    /* [in] */ ISessionOutputBuffer* buffer,
    /* [in] */ ILineFormatter* formatter,
    /* [in] */ IHttpParams* params)
{
    if (buffer == NULL) {
        Logger::E("AbstractMessageWriter", "Session output buffer may not be null");
        assert(0);
        // throw new IllegalArgumentException("Session output buffer may not be null");
    }
    mSessionBuffer = buffer;
    CCharArrayBuffer::New(128, (ICharArrayBuffer**)&mLineBuffer);
    mLineFormatter = (formatter != NULL) ? formatter : ILineFormatter::Probe(BasicLineFormatter::DEFAULT);
}

CAR_INTERFACE_IMPL(AbstractMessageWriter, Object, IHttpMessageWriter)

ECode AbstractMessageWriter::Write(
    /* [in] */ IHttpMessage* message)
{
    if (message == NULL) {
        Logger::E("AbstractMessageWriter", "HTTP message may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    WriteHeadLine(message);
    AutoPtr<IHeaderIterator> hi;
    message->GetHeaderIterator((IHeaderIterator**)&hi);
    AutoPtr<IIterator> it = IIterator::Probe(hi);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<IHeader> header = IHeader::Probe(value);
        AutoPtr<ICharArrayBuffer> buffer;
        mLineFormatter->FormatHeader(mLineBuffer, header, (ICharArrayBuffer**)&buffer);
        mSessionBuffer->WriteLine(buffer);
    }
    mLineBuffer->Clear();
    mSessionBuffer->WriteLine(mLineBuffer);
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org