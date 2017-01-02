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
#include "CipherSpi.h"

using Elastos::IO::IBuffer;

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CipherSpi, Object, ICipherSpi)

CipherSpi::CipherSpi()
{
}

CipherSpi::~CipherSpi()
{
}

ECode CipherSpi::constructor()
{
    return NOERROR;
}

ECode CipherSpi::EngineUpdate(
    /* [in] */ IByteBuffer* input,
    /* [in] */ IByteBuffer* output,
    /* [out] */ Int32* number)
{
    if (input == NULL) {
        // throw new NullPointerException("input == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (output == NULL) {
        // throw new NullPointerException("output == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 position;
    IBuffer::Probe(input)->GetPosition(&position);
    Int32 limit;
    IBuffer::Probe(input)->GetLimit(&limit);
    if ((limit - position) <= 0) {
        *number = 0;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > bInput;
    AutoPtr<ArrayOf<Byte> > bOutput;
    Boolean b;
    IBuffer::Probe(input)->HasArray(&b);
    if (b) {
        input->GetArray((ArrayOf<Byte>**)&bInput);
        Int32 offset;
        IBuffer::Probe(input)->GetArrayOffset(&offset);
        EngineUpdate(bInput, offset + position, limit - position, (ArrayOf<Byte>**)&bOutput);
        IBuffer::Probe(input)->SetPosition(limit);
    } else {
        bInput = ArrayOf<Byte>::Alloc(limit - position);
        input->Get(bInput);
        EngineUpdate(bInput, 0, limit - position, (ArrayOf<Byte>**)&bOutput);
    }
    if (bOutput == NULL) {
        *number = 0;
        return NOERROR;
    }
    Int32 remaining;
    IBuffer::Probe(output)->GetRemaining(&remaining);
    if (remaining < bOutput->GetLength()) {
        // throw new ShortBufferException("output buffer too small");
        return E_SHORT_BUFFER_EXCEPTION;
    }
    // try {
        output->Put(bOutput);
    // } catch (java.nio.BufferOverflowException e) {
    //     // throw new ShortBufferException("output buffer too small");
    //     return E_SHORT_BUFFER_EXCEPTION;
    // }
    *number = bOutput->GetLength();
    return NOERROR;
}

ECode CipherSpi::EngineUpdateAAD(
    /* [in] */ ArrayOf<Byte> * input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen)
{
    // throw new UnsupportedOperationException(
    //     "This cipher does not support Authenticated Encryption with Additional Data");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CipherSpi::EngineUpdateAAD(
    /* [in] */ IByteBuffer* input)
{
    if (input == NULL) {
        // throw new NullPointerException("input == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 position;
    IBuffer::Probe(input)->GetPosition(&position);
    Int32 limit;
    IBuffer::Probe(input)->GetLimit(&limit);
    if ((limit - position) <= 0) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > bInput;
    Boolean b;
    IBuffer::Probe(input)->HasArray(&b);
    if (b) {
        input->GetArray((ArrayOf<Byte>**)&bInput);
        Int32 offset;
        IBuffer::Probe(input)->GetArrayOffset(&offset);
        EngineUpdateAAD(bInput, offset + position, limit - position);
        IBuffer::Probe(input)->SetPosition(limit);
    } else {
        Int32 len = limit - position;
        bInput = ArrayOf<Byte>::Alloc(len);
        input->Get(bInput);
        EngineUpdateAAD(bInput, 0, len);
    }
    return NOERROR;
}

ECode CipherSpi::EngineDoFinal(
    /* [in] */ IByteBuffer* input,
    /* [in] */ IByteBuffer* output,
    /* [out] */ Int32* number)
{
    if (input == NULL) {
        // throw new NullPointerException("input == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (output == NULL) {
        // throw new NullPointerException("output == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 position;
    IBuffer::Probe(input)->GetPosition(&position);
    Int32 limit;
    IBuffer::Probe(input)->GetLimit(&limit);

    if ((limit - position) <= 0) {
        *number = 0;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > bInput;
    AutoPtr<ArrayOf<Byte> > bOutput;

    Boolean b;
    IBuffer::Probe(input)->HasArray(&b);
    if (b) {
        input->GetArray((ArrayOf<Byte>**)&bInput);
        Int32 offset;
        IBuffer::Probe(input)->GetArrayOffset(&offset);
        EngineDoFinal(bInput, offset + position, limit - position, (ArrayOf<Byte>**)&bOutput);
        IBuffer::Probe(input)->SetPosition(limit);
    } else {
        bInput = ArrayOf<Byte>::Alloc(limit - position);
        input->Get(bInput);
        EngineDoFinal(bInput, 0, limit - position, (ArrayOf<Byte>**)&bOutput);
    }

    Int32 remaining;
    IBuffer::Probe(output)->GetRemaining(&remaining);
    if (remaining < bOutput->GetLength()) {
        // throw new ShortBufferException("output buffer too small");
        return E_SHORT_BUFFER_EXCEPTION;
    }
    // try {
        output->Put(bOutput);
    // } catch (java.nio.BufferOverflowException e) {
    //     // throw new ShortBufferException("output buffer too small");
    //     return E_SHORT_BUFFER_EXCEPTION;
    // }
    *number = bOutput->GetLength();
    return NOERROR;
}

ECode CipherSpi::EngineWrap(
    /* [in] */ IKey* keyToWrap,
    /* [out, callee] */ ArrayOf<Byte>** wrappedKey)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CipherSpi::EngineUnwrap(
    /* [in] */ ArrayOf<Byte>* wrappedKey,
    /* [in] */ const String& wrappedKeyAlgorithm,
    /* [in] */ Int32 wrappedKeyType,
    /* [out] */ IKey** key)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CipherSpi::EngineGetKeySize(
    /* [in] */ IKey* key,
    /* [out] */ Int32* size)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}
} // Crypto
} // Elastosx