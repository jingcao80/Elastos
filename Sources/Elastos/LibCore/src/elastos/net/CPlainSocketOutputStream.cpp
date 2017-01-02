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

#include "CPlainSocketOutputStream.h"
#include "PlainSocketImpl.h"

namespace Elastos {
namespace Net {

CAR_OBJECT_IMPL(CPlainSocketOutputStream)

CPlainSocketOutputStream::CPlainSocketOutputStream()
{
}

ECode CPlainSocketOutputStream::constructor(
    /* [in] */ ISocketImpl * impl)
{
    if (IPlainSocketImpl::Probe(impl) == NULL) {
        return E_INVALID_ARGUMENT;
    }

    mSocket = impl;
    return NOERROR;
}

ECode CPlainSocketOutputStream::Close()
{
    return mSocket->Close();
}

ECode CPlainSocketOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1);
    (*buffer)[0] = (Byte) (oneByte & 0xFF);

    return Write(buffer, 0, 1);
}

ECode CPlainSocketOutputStream::Write(
    /* [in] */ ArrayOf<Byte> * buffer)
{
    if (buffer == NULL || buffer->GetLength() == 0) {
        return E_INVALID_ARGUMENT;
    }
    return Write(buffer, 0, buffer->GetLength());
}

ECode CPlainSocketOutputStream::Write(
    /* [in] */ ArrayOf<Byte> * buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    if (buffer == NULL || buffer->GetLength() == 0) {
        return E_INVALID_ARGUMENT;
    }

    if (0 <= offset && offset <= buffer->GetLength() && 0 <= count && count <= buffer->GetLength() - offset) {
        PlainSocketImpl* psi = (PlainSocketImpl*)IPlainSocketImpl::Probe(mSocket);
        Int32 number;
        return psi->Write(buffer, offset, count, &number);
    }
    else {
        return E_BUFFER_UNDER_FLOW_EXCEPTION;
    }
}

} // namespace Net
} // namespace Elastos
