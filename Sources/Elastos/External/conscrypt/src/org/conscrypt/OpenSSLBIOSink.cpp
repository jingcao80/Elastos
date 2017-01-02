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

#include "org/conscrypt/OpenSSLBIOSink.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/Math.h>

using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IOutputStream;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLBIOSink, Object, IOpenSSLBIOSink)

OpenSSLBIOSink::OpenSSLBIOSink()
    : mCtx(0)
    , mPosition(0)
{}

OpenSSLBIOSink::~OpenSSLBIOSink()
{
    // try {
    NativeCrypto::BIO_free_all(mCtx);
    // } finally {
    //     super.finalize();
    // }
}

ECode OpenSSLBIOSink::constructor(
    /* [in] */ IByteArrayOutputStream* buffer)
{
    NativeCrypto::Create_BIO_OutputStream(IOutputStream::Probe(buffer), &mCtx);
    mBuffer = buffer;
    return NOERROR;
}

ECode OpenSSLBIOSink::Available(
    /* [out] */  Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 size;
    mBuffer->GetSize(&size);
    *result = size - mPosition;
    return NOERROR;
}

ECode OpenSSLBIOSink::Reset()
{
    mBuffer->Reset();
    mPosition = 0;
    return NOERROR;
}

ECode OpenSSLBIOSink::Skip(
    /* [in] */  Int64 byteCount,
    /* [out] */  Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 val;
    Available(&val);
    Int32 maxLength = Elastos::Core::Math::Min(val, (Int32) byteCount);
    mPosition += maxLength;
    Int32 size;
    mBuffer->GetSize(&size);
    if (mPosition == size) {
        Reset();
    }
    *result = maxLength;
    return NOERROR;
}

ECode OpenSSLBIOSink::GetContext(
    /* [out] */  Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCtx;
    return NOERROR;
}

ECode OpenSSLBIOSink::ToByteArray(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return mBuffer->ToByteArray(result);
}

ECode OpenSSLBIOSink::Position(
    /* [out] */  Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPosition;
    return NOERROR;
}

ECode OpenSSLBIOSink::Create(
    /* [out] */  IOpenSSLBIOSink** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IByteArrayOutputStream> buffer;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&buffer);
    AutoPtr<OpenSSLBIOSink> sink = new OpenSSLBIOSink();
    sink->constructor(buffer);
    *result = IOpenSSLBIOSink::Probe(sink);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
