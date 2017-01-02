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

#include "org/apache/http/entity/CSerializableEntity.h"
#include "elastos/io/CByteArrayInputStream.h"
#include "elastos/io/CByteArrayOutputStream.h"
#include "elastos/io/CObjectOutputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CObjectOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IObjectOutput;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IFlushable;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_OBJECT_IMPL(CSerializableEntity)

ECode CSerializableEntity::CreateBytes(
    /* [in] */ ISerializable* ser)
{
    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&baos);
    AutoPtr<IObjectOutputStream> out;
    CObjectOutputStream::New(IOutputStream::Probe(baos), (IObjectOutputStream**)&out);
    IObjectOutput::Probe(out)->WriteObject(IObject::Probe(ser));
    IFlushable::Probe(out)->Flush();
    return baos->ToByteArray((ArrayOf<Byte>**)&mObjSer);
}

ECode CSerializableEntity::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    if (mObjSer == NULL) {
        CreateBytes(mObjRef);
    }
    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(mObjSer, (IByteArrayInputStream**)&stream);
    *inputStream = IInputStream::Probe(stream);
    REFCOUNT_ADD(*inputStream)
    return NOERROR;
}

ECode CSerializableEntity::GetContentLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length)
    if (mObjSer ==  NULL) {
        *length = -1;
    }
    else {
        *length = mObjSer->GetLength();
    }
    return NOERROR;
}

ECode CSerializableEntity::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    *isRepeatable = TRUE;
    return NOERROR;
}

ECode CSerializableEntity::IsStreaming(
    /* [out] */ Boolean* isStreaming)
{
    VALIDATE_NOT_NULL(isStreaming)
    *isStreaming = mObjSer == NULL;
    return NOERROR;
}

ECode CSerializableEntity::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    if (outstream == NULL) {
        Logger::E("CSerializableEntity", "Output stream may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mObjSer == NULL) {
        AutoPtr<IObjectOutputStream> out;
        CObjectOutputStream::New(outstream, (IObjectOutputStream**)&out);
        IObjectOutput::Probe(out)->WriteObject(IObject::Probe(mObjRef));
        IFlushable::Probe(out)->Flush();
    }
    else {
        outstream->Write(mObjSer);
        IFlushable::Probe(outstream)->Flush();
    }
    return NOERROR;
}

ECode CSerializableEntity::constructor(
    /* [in] */ ISerializable* ser,
    /* [in] */ Boolean bufferize)
{
    if (ser == NULL) {
        Logger::E("CSerializableEntity", "Source object may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (bufferize) {
        return CreateBytes(ser);
    }
    else {
        mObjRef = ser;
    }
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org
