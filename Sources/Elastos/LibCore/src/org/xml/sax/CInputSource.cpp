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

#include "Elastos.CoreLibrary.IO.h"
#include "CInputSource.h"

namespace Org {
namespace Xml {
namespace Sax {

CAR_INTERFACE_IMPL(CInputSource, Object, IInputSource)

CAR_OBJECT_IMPL(CInputSource)

ECode CInputSource::constructor()
{
    return NOERROR;
}

ECode CInputSource::constructor(
    /* [in] */ const String& systemId)
{
    return SetSystemId(systemId);
}

ECode CInputSource::constructor(
    /* [in] */ IInputStream* byteStream)
{
    return SetByteStream(byteStream);
}

ECode CInputSource::constructor(
    /* [in] */ IReader* characterStream)
{
    return SetCharacterStream(characterStream);;
}

ECode CInputSource::SetPublicId(
    /* [in] */ const String& publicId)
{
    mPublicId = publicId;
    return NOERROR;
}

ECode CInputSource::GetPublicId(
    /* [out] */ String* publicId)
{
    VALIDATE_NOT_NULL(publicId);

    *publicId = mPublicId;

    return NOERROR;
}

ECode CInputSource::SetSystemId(
    /* [in] */ const String& systemId)
{
    mSystemId = systemId;

    return NOERROR;
}

ECode CInputSource::GetSystemId(
    /* [out] */ String* systemId)
{
    VALIDATE_NOT_NULL(systemId);

    *systemId = mSystemId;

    return NOERROR;
}

ECode CInputSource::SetByteStream(
    /* [in] */ IInputStream* byteStream)
{
    mByteStream = byteStream;

    return NOERROR;
}

ECode CInputSource::GetByteStream(
    /* [out] */ IInputStream** byteStream)
{
    VALIDATE_NOT_NULL(byteStream);

    *byteStream = (mByteStream).Get();
    REFCOUNT_ADD(*byteStream);

    return NOERROR;
}

ECode CInputSource::SetEncoding(
    /* [in] */ const String& encoding)
{
    mEncoding = encoding;

    return NOERROR;
}

ECode CInputSource::GetEncoding(
    /* [out] */ String* encoding)
{
    VALIDATE_NOT_NULL(encoding);

    *encoding = mEncoding;

    return NOERROR;
}

ECode CInputSource::SetCharacterStream(
    /* [in] */ IReader* characterStream)
{
    mCharacterStream = characterStream;

    return NOERROR;
}

ECode CInputSource::GetCharacterStream(
    /* [out] */ IReader** characterStream)
{
    VALIDATE_NOT_NULL(characterStream);

    *characterStream = mCharacterStream.Get();
    REFCOUNT_ADD(*characterStream);

    return NOERROR;
}

} // namespace Sax
} // namespace Xml
} // namespace Org
