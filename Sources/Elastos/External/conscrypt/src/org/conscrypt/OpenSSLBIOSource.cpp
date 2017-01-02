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

#include "org/conscrypt/OpenSSLBIOSource.h"
#include "org/conscrypt/OpenSSLBIOInputStream.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/Math.h>

using Elastos::IO::IBuffer;

namespace Org {
namespace Conscrypt {

//==============================================================================
//  OpenSSLBIOSource::ByteBufferInputStream
//==============================================================================

OpenSSLBIOSource::ByteBufferInputStream::ByteBufferInputStream(
    /* [in] */ IByteBuffer* source)
{
    mSource = source;
}

ECode OpenSSLBIOSource::ByteBufferInputStream::Read(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 val;
    IBuffer::Probe(mSource)->GetRemaining(&val);
    if (val > 0) {
        return mSource->GetInt32(result);
    }
    else {
        *result = -1;
    }
    return NOERROR;
}

ECode OpenSSLBIOSource::ByteBufferInputStream::Available(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 limit, position;
    IBuffer::Probe(mSource)->GetLimit(&limit);
    IBuffer::Probe(mSource)->GetPosition(&position);
    *result = limit - position;
    return NOERROR;
}

ECode OpenSSLBIOSource::ByteBufferInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 originalPosition;
    IBuffer::Probe(mSource)->GetPosition(&originalPosition);
    mSource->Get(buffer);
    Int32 position;
    IBuffer::Probe(mSource)->GetPosition(&position);
    *result = position - originalPosition;
    return NOERROR;
}

ECode OpenSSLBIOSource::ByteBufferInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 remaining;
    IBuffer::Probe(mSource)->GetRemaining(&remaining);
    Int32 toRead = Elastos::Core::Math::Min(remaining, byteCount);
    Int32 originalPosition;
    IBuffer::Probe(mSource)->GetPosition(&originalPosition);
    mSource->Get(buffer, byteOffset, toRead);
    Int32 position;
    IBuffer::Probe(mSource)->GetPosition(&position);
    *result = position - originalPosition;
    return NOERROR;
}

ECode OpenSSLBIOSource::ByteBufferInputStream::Reset()
{
    return IBuffer::Probe(mSource)->Reset();
}

ECode OpenSSLBIOSource::ByteBufferInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 originalPosition;
    IBuffer::Probe(mSource)->GetPosition(&originalPosition);
    IBuffer::Probe(mSource)->SetPosition((Int32) (originalPosition + byteCount));
    Int32 position;
    IBuffer::Probe(mSource)->GetPosition(&position);
    *result = position - originalPosition;
    return NOERROR;
}

//==============================================================================
//  OpenSSLBIOSource
//==============================================================================

CAR_INTERFACE_IMPL(OpenSSLBIOSource, Object, IOpenSSLBIOSource)

OpenSSLBIOSource::OpenSSLBIOSource()
{}

OpenSSLBIOSource::~OpenSSLBIOSource()
{
    // try {
    ReleaseResources();
    // } finally {
    //     super.finalize();
    // }
}

ECode OpenSSLBIOSource::constructor(
    /* [in] */ IOpenSSLBIOInputStream* source)
{
    mSource = source;
    return NOERROR;
}

ECode OpenSSLBIOSource::GetContext(
    /* [out] */  Int64* result)
{
    return mSource->GetBioContext(result);
}

ECode OpenSSLBIOSource::ReleaseResources()
{
    if (mSource != NULL) {
        Int64 val;
        mSource->GetBioContext(&val);
        NativeCrypto::BIO_free_all(val);
        mSource = NULL;
    }
    return NOERROR;
}

ECode OpenSSLBIOSource::Wrap(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IOpenSSLBIOSource** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ByteBufferInputStream> bbis = new ByteBufferInputStream(buffer);
    AutoPtr<OpenSSLBIOInputStream> osis = new OpenSSLBIOInputStream();
    osis->constructor(IInputStream::Probe(bbis));
    AutoPtr<OpenSSLBIOSource> oss = new OpenSSLBIOSource();
    oss->constructor(IOpenSSLBIOInputStream::Probe(osis));
    *result = IOpenSSLBIOSource::Probe(oss);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
