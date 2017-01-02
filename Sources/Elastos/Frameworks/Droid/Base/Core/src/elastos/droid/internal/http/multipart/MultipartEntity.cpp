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
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/internal/http/multipart/MultipartEntity.h"
#include "elastos/droid/internal/http/multipart/Part.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/logging/Logger.h>
#include <org/apache/http/utility/EncodingUtils.h>

using Elastos::Core::IString;
using Elastos::Core::StringBuffer;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::CRandom;
using Elastos::Utility::IRandom;
using Elastos::Utility::Logging::Logger;
// using Org::Apache::Commons::Logging::ILogFactory;
using Org::Apache::Http::Message::CBasicHeader;
using Org::Apache::Http::Protocol::IHTTP;
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
//     factory->GetLog(String("MultipartEntity"), (ILog**)&log)
//     return log;
// }
// const AutoPtr<ILog> FilePart::LOG = InitStaticLOG();

extern AutoPtr<ArrayOf<Byte> > GetAsciiBytes(
        /* [in] */ const String& data);

const String MultipartEntity::MULTIPART_FORM_CONTENT_TYPE = String("multipart/form-data");
AutoPtr<ArrayOf<Byte> > MultipartEntity::MULTIPART_CHARS = GetAsciiBytes(String("-_1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));

CAR_INTERFACE_IMPL(MultipartEntity, AbstractHttpEntity, IMultipartEntity)

MultipartEntity::MultipartEntity()
    : mContentConsumed(FALSE)
{
}

ECode MultipartEntity::constructor(
    /* [in] */ ArrayOf<IPart*>* parts,
    /* [in] */ IHttpParams* params)
{
    if (parts == NULL) {
      Logger::E(String("MultipartEntity"), String("parts cannot be null"));
      return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
      Logger::E(String("MultipartEntity"), String("params cannot be null"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mParts = parts;
    mParams = params;
    return NOERROR;
}

ECode MultipartEntity::constructor(
    /* [in] */ ArrayOf<IPart*>* parts)
{
    SetContentType(MULTIPART_FORM_CONTENT_TYPE);
    if (parts == NULL) {
      Logger::E(String("MultipartEntity"), String("parts cannot be null"));
      return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mParts = parts;
    mParams = NULL;
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > MultipartEntity::GenerateMultipartBoundary()
{
    AutoPtr<IRandom> rand;
    CRandom::New((IRandom**)&rand);
    Int32 size;
    rand->NextInt32(11, &size);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(size + 30); // a random size from 30 to 40
    Int32 randSize;
    for (Int32 i = 0; i < bytes->GetLength(); i++) {
        rand->NextInt32(MULTIPART_CHARS->GetLength(), &randSize);
        (*bytes)[i] = (*MULTIPART_CHARS)[randSize];
    }
    return bytes;
}

AutoPtr<ArrayOf<Byte> > MultipartEntity::GetMultipartBoundary()
{
    if (mMultipartBoundary == NULL) {
        String temp;
        if (mParams != NULL) {
            AutoPtr<IInterface> obj;
            mParams->GetParameter(IMultipartEntity::MULTIPART_BOUNDARY, (IInterface**)&obj);
            if (IString::Probe(obj))
                IObject::Probe(obj)->ToString(&temp);
        }
        if (!temp.IsNull()) {
            mMultipartBoundary = GetAsciiBytes(temp);
        }
        else {
            mMultipartBoundary = GenerateMultipartBoundary();
        }
    }
    return mMultipartBoundary;
}

ECode MultipartEntity::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable);

    for (Int32 i = 0; i < mParts->GetLength(); i++) {
        Boolean repeatable;
        ((*mParts)[i])->IsRepeatable(&repeatable);
        if (!repeatable) {
            *isRepeatable = FALSE;
            return NOERROR;
        }
    }

    *isRepeatable = TRUE;
    return NOERROR;
}

ECode MultipartEntity::WriteTo(
    /* [in] */ IOutputStream* outStream)
{
    return Part::SendParts(outStream, mParts, GetMultipartBoundary());
}

ECode MultipartEntity::GetContentType(
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header);

    StringBuffer* buffer = new StringBuffer(MULTIPART_FORM_CONTENT_TYPE);
    buffer->Append("; boundary=");
    String str;
    FAIL_RETURN(EncodingUtils::GetAsciiString(GetMultipartBoundary(), &str))
    buffer->Append(str);
    String bufferStr;
    buffer->ToString(&bufferStr);
    return CBasicHeader::New(IHTTP::CONTENT_TYPE, bufferStr, header);
}

ECode MultipartEntity::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length);

    ECode ec = Part::GetLengthOfParts(mParts, GetMultipartBoundary(), length);
    if (FAILED(ec)) {
        Logger::E(String("MultipartEntity"), String("An exception occurred while getting the length of the parts"));
    }

    return NOERROR;
}

ECode MultipartEntity::GetContent(
    /* [out] */ IInputStream** stream)
{
    Boolean repeatable;
    IsRepeatable(&repeatable);
    if(!repeatable && mContentConsumed ) {
        Logger::E(String("MultipartEntity"), String("Content has been consumed"));
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mContentConsumed = TRUE;

    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&baos);
    Part::SendParts(IOutputStream::Probe(baos), mParts, mMultipartBoundary);
    AutoPtr<ArrayOf<Byte> > bytes;
    baos->ToByteArray((ArrayOf<Byte>**)&bytes);
    return CByteArrayInputStream::New(bytes, stream);
}


ECode MultipartEntity::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming);
    *isStreaming = FALSE;
    return NOERROR;
}

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
