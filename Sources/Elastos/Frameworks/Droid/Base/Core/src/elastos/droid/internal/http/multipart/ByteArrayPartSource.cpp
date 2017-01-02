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
#include "elastos/droid/internal/http/multipart/ByteArrayPartSource.h"

using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

CAR_INTERFACE_IMPL_2(ByteArrayPartSource, Object, IByteArrayPartSource, IPartSource)

ECode ByteArrayPartSource::constructor(
    /* [in] */ const String& fileName,
    /* [in] */ ArrayOf<Byte>* bytes)
{
    mFileName = fileName;
    mBytes = bytes;
    return NOERROR;
}

ECode ByteArrayPartSource::GetLength(
    /* [out] */ Int64* length)
{
    VALIDATE_NOT_NULL(length);
    *length = mBytes->GetLength();
    return NOERROR;
}

ECode ByteArrayPartSource::GetFileName(
    /* [out] */ String* fileName)
{
    VALIDATE_NOT_NULL(fileName);
    *fileName = mFileName;
    return NOERROR;
}

ECode ByteArrayPartSource::CreateInputStream(
    /* [out] */ IInputStream** stream)
{
    return CByteArrayInputStream::New(mBytes, stream);
}

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
