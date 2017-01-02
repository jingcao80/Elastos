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

#include "FilterOutputStream.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FilterOutputStream, OutputStream, IFilterOutputStream)

FilterOutputStream::FilterOutputStream()
{
}

FilterOutputStream::~FilterOutputStream()
{
}

ECode FilterOutputStream::constructor(
    /* [in] */ IOutputStream* outs)
{
    VALIDATE_NOT_NULL(outs);
    mOut = outs;
    return NOERROR;
}

ECode FilterOutputStream::Close()
{
    ECode ec = IFlushable::Probe(mOut)->Flush();
    ECode ec2 = ICloseable::Probe(mOut)->Close();
    if(ec != NOERROR || ec2 != NOERROR){
        return ec != NOERROR ? ec : ec2;
    }
    return NOERROR;
}

ECode FilterOutputStream::Flush()
{
    return IFlushable::Probe(mOut)->Flush();
}

ECode FilterOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    return mOut->Write(oneByte);
}

ECode FilterOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    VALIDATE_NOT_NULL(buffer)

    // BEGIN android-note
    // changed array notation to be consistent with the rest of harmony
    // END android-note
    // Force null buffer check first!
    if (offset > buffer->GetLength() || offset < 0) {
//      throw new ArrayIndexOutOfBoundsException("Offset out of bounds: " + offset);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (count < 0 || count > buffer->GetLength() - offset) {
//      throw new ArrayIndexOutOfBoundsException("Length out of bounds: " + length);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    for (Int32 i = 0; i < count; i++) {
        // Call write() instead of out.write() since subclasses could
        // override the write() method.
        FAIL_RETURN(Write((Int32)(*buffer)[offset + i]));
    }

    return NOERROR;
}

} // namespace IO
} // namespace Elastos
