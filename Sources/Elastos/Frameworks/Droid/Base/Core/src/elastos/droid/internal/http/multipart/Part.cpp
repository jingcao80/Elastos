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

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/http/multipart/Part.h"
#include <elastos/utility/logging/Logger.h>
#include <org/apache/http/utility/EncodingUtils.h>

using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::Utility::Logging::Logger;
// using Org::Apache::Commons::Logging::ILogFactory;
using Org::Apache::Http::Utility::EncodingUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

// TODO:
// static AutoPtr<ILog> InitStaticLOG()
// {
//     AutoPtr<ILogFactory> factory;
//     CLogFactory::AcquireSingleton((ILogFactory**)&factory);
//     AutoPtr<ILog> log;
//     factory->GetLog(String("Part"), (ILog**)&log)
//     return log;
// }
// const AutoPtr<ILog> Part::LOG = InitStaticLOG();

AutoPtr<ArrayOf<Byte> > GetAsciiBytes(
        /* [in] */ const String& data)
{
    AutoPtr<ArrayOf<Byte> > bytes;
    EncodingUtils::GetAsciiBytes(data, (ArrayOf<Byte>**)&bytes);
    return bytes;
}

const String Part::BOUNDARY = String("----------------314159265358979323846");
const AutoPtr<ArrayOf<Byte> > Part::BOUNDARY_BYTES = GetAsciiBytes(BOUNDARY);
const AutoPtr<ArrayOf<Byte> > Part::DEFAULT_BOUNDARY_BYTES = BOUNDARY_BYTES;
const String Part::CRLF = String("\r\n");
const AutoPtr<ArrayOf<Byte> > Part::CRLF_BYTES = GetAsciiBytes(CRLF);
const String Part::QUOTE = String("\"");
const AutoPtr<ArrayOf<Byte> > Part::QUOTE_BYTES = GetAsciiBytes(QUOTE);
const String Part::EXTRA = String("--");
const AutoPtr<ArrayOf<Byte> > Part::EXTRA_BYTES = GetAsciiBytes(EXTRA);
const String Part::CONTENT_DISPOSITION = String("Content-Disposition: form-data; name=");
const AutoPtr<ArrayOf<Byte> > Part::CONTENT_DISPOSITION_BYTES = GetAsciiBytes(CONTENT_DISPOSITION);
const String Part::CONTENT_TYPE = String("Content-Type: ");
const AutoPtr<ArrayOf<Byte> > Part::CONTENT_TYPE_BYTES = GetAsciiBytes(CONTENT_TYPE);
const String Part::CHARSET = String("; charset=");
const AutoPtr<ArrayOf<Byte> > Part::CHARSET_BYTES = GetAsciiBytes(CHARSET);
const String Part::CONTENT_TRANSFER_ENCODING = String("Content-Transfer-Encoding: ");
const AutoPtr<ArrayOf<Byte> > Part::CONTENT_TRANSFER_ENCODING_BYTES = GetAsciiBytes(CONTENT_TRANSFER_ENCODING);

CAR_INTERFACE_IMPL(Part, Object, IPart)

ECode Part::GetBoundary(
        /* [out] */ String* boundary)
{
    VALIDATE_NOT_NULL(boundary);
    *boundary = BOUNDARY;
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > Part::GetPartBoundary()
{
    if (mBoundaryBytes == NULL) {
        // custom boundary bytes have not been set, use the default.
        return DEFAULT_BOUNDARY_BYTES;
    }

    return mBoundaryBytes;
}

ECode Part::SetPartBoundary(
    /* [in] */ ArrayOf<Byte>* boundaryBytes)
{
    mBoundaryBytes = boundaryBytes;
    return NOERROR;
}

ECode Part::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable);
    *isRepeatable = TRUE;
    return NOERROR;
}

ECode Part::SendStart(
    /* [in] */ IOutputStream* outStream)
{
    // LOG->Trace(String("enter sendStart(OutputStream out)"));
    FAIL_RETURN(outStream->Write(EXTRA_BYTES));
    FAIL_RETURN(outStream->Write(GetPartBoundary()));
    FAIL_RETURN(outStream->Write(CRLF_BYTES));

    return NOERROR;
}

ECode Part::SendDispositionHeader(
    /* [in] */ IOutputStream* outStream)
{
    // LOG->Trace(String("enter sendDispositionHeader(OutputStream out)"));
    FAIL_RETURN(outStream->Write(CONTENT_DISPOSITION_BYTES));
    FAIL_RETURN(outStream->Write(QUOTE_BYTES));
    String name;
    GetName(&name);
    FAIL_RETURN(outStream->Write(GetAsciiBytes(name)));
    FAIL_RETURN(outStream->Write(QUOTE_BYTES));

    return NOERROR;
}

ECode Part::SendContentTypeHeader(
    /* [in] */ IOutputStream* outStream)
{
    // LOG->Trace(String("enter sendContentTypeHeader(OutputStream out)"));
    String contentType;
    GetContentType(&contentType);
    if (contentType != NULL) {
        FAIL_RETURN(outStream->Write(CRLF_BYTES));
        FAIL_RETURN(outStream->Write(CONTENT_TYPE_BYTES));
        FAIL_RETURN(outStream->Write(GetAsciiBytes(contentType)));
        String charSet;
        GetCharSet(&charSet);
        if (charSet != NULL) {
            FAIL_RETURN(outStream->Write(CHARSET_BYTES));
            FAIL_RETURN(outStream->Write(GetAsciiBytes(charSet)));
        }
    }

    return NOERROR;
}

ECode Part::SendTransferEncodingHeader(
    /* [in] */ IOutputStream* outStream)
{
    // LOG->Trace(String("enter sendTransferEncodingHeader(OutputStream out)"));
    String transferEncoding;
    GetTransferEncoding(&transferEncoding);
    if (transferEncoding != NULL) {
        FAIL_RETURN(outStream->Write(CRLF_BYTES));
        FAIL_RETURN(outStream->Write(CONTENT_TRANSFER_ENCODING_BYTES));
        FAIL_RETURN(outStream->Write(GetAsciiBytes(transferEncoding)));
    }

    return NOERROR;
}

ECode Part::SendEndOfHeader(
    /* [in] */ IOutputStream* outStream)
{
    // LOG->Trace(String("enter sendEndOfHeader(OutputStream out)"));
    FAIL_RETURN(outStream->Write(CRLF_BYTES));
    FAIL_RETURN(outStream->Write(CRLF_BYTES));

    return NOERROR;
}

ECode Part::SendEnd(
    /* [in] */ IOutputStream* outStream)
{
    // LOG->Tace(String("enter sendEnd(OutputStream out)"));
    FAIL_RETURN(outStream->Write(CRLF_BYTES));

    return NOERROR;
}

ECode Part::Send(
    /* [in] */ IOutputStream* outStream)
{
    // LOG->Trace(String("enter send(OutputStream out)"));
    FAIL_RETURN(SendStart(outStream));
    FAIL_RETURN(SendDispositionHeader(outStream));
    FAIL_RETURN(SendContentTypeHeader(outStream));
    FAIL_RETURN(SendTransferEncodingHeader(outStream));
    FAIL_RETURN(SendEndOfHeader(outStream));
    FAIL_RETURN(SendData(outStream));
    FAIL_RETURN(SendEnd(outStream));

    return NOERROR;
}

ECode Part::Length(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length);

    // LOG->Trace(String("enter length()"));
    FAIL_RETURN(LengthOfData(length));
    if (*length < 0) {
        *length = -1;
        return NOERROR;
    }

    AutoPtr<IOutputStream> overhead;
    CByteArrayOutputStream::New((IOutputStream**)&overhead);
    FAIL_RETURN(SendStart(overhead));
    FAIL_RETURN(SendDispositionHeader(overhead));
    FAIL_RETURN(SendContentTypeHeader(overhead));
    FAIL_RETURN(SendTransferEncodingHeader(overhead));
    FAIL_RETURN(SendEndOfHeader(overhead));
    FAIL_RETURN(SendEnd(overhead));

    FAIL_RETURN(LengthOfData(length));
    Int32 ovSize;
    IByteArrayOutputStream::Probe(overhead)->GetSize(&ovSize);
    *length += ovSize;

    return NOERROR;
}

ECode Part::ToString(
     /* [out] */ String* str)
{
    return GetName(str);
}

ECode Part::SendParts(
    /* [in] */ IOutputStream* outStream,
    /* [in] */ ArrayOf<IPart*>* parts)
{
    return SendParts(outStream, parts, DEFAULT_BOUNDARY_BYTES);
}

ECode Part::SendParts(
    /* [in] */ IOutputStream* outStream,
    /* [in] */ ArrayOf<IPart*>* parts,
    /* [in] */ ArrayOf<Byte>* partBoundary)
{
    if (parts == NULL) {
        Logger::E(String("Part"), String("Parts may not be null"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (partBoundary == NULL || partBoundary->GetLength() == 0) {
        Logger::E(String("Part"), String("partBoundary may not be empty"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    for (Int32 i = 0; i < parts->GetLength(); i++) {
        // set the part boundary before the part is sent
        FAIL_RETURN(((Part*)(*parts)[i])->SetPartBoundary(partBoundary));
        FAIL_RETURN(((*parts)[i])->Send(outStream));
    }
    FAIL_RETURN(outStream->Write(EXTRA_BYTES));
    FAIL_RETURN(outStream->Write(partBoundary));
    FAIL_RETURN(outStream->Write(EXTRA_BYTES));
    FAIL_RETURN(outStream->Write(CRLF_BYTES));

    return NOERROR;
}

ECode Part::GetLengthOfParts(
    /* [in] */ ArrayOf<IPart*>* parts,
    /* [out] */ Int64* length)
{
    return GetLengthOfParts(parts, DEFAULT_BOUNDARY_BYTES, length);
}

ECode Part::GetLengthOfParts(
    /* [in] */ ArrayOf<IPart*>* parts,
    /* [in] */ ArrayOf<Byte>* partBoundary,
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length);

    // LOG->Trace(String("getLengthOfParts(Parts[])"));
    if (parts == NULL) {
        Logger::E(String("Part"), String("Parts may not be null"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 total = 0;
    for (Int32 i = 0; i < parts->GetLength(); i++) {
        // set the part boundary before we calculate the part's length
        FAIL_RETURN(((Part*)(*parts)[i])->SetPartBoundary(partBoundary));
        Int64 l;
        FAIL_RETURN(((*parts)[i])->Length(&l));
        if (l < 0) {
            *length = -1;
            return NOERROR;
        }
        total += l;
    }
    total += EXTRA_BYTES->GetLength();
    total += partBoundary->GetLength();
    total += EXTRA_BYTES->GetLength();
    total += CRLF_BYTES->GetLength();
    *length = total;

    return NOERROR;
}

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
