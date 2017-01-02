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

#include "InflaterOutputStream.h"
#include "Arrays.h"
#include "CInflater.h"

namespace Elastos {
namespace Utility {
namespace Zip {

const Int32 InflaterOutputStream::DEFAULT_BUFFER_SIZE;

CAR_INTERFACE_IMPL(InflaterOutputStream, FilterOutputStream, IInflaterOutputStream)

InflaterOutputStream::InflaterOutputStream()
    : mClosed(FALSE)
{}

InflaterOutputStream::~InflaterOutputStream()
{
}

ECode InflaterOutputStream::constructor(
    /* [in] */ IOutputStream* out)
{
    AutoPtr<IInflater> inflater;
    CInflater::New((IInflater**)&inflater);
    return constructor(out, inflater);
}

ECode InflaterOutputStream::constructor(
    /* [in] */ IOutputStream* out,
    /* [in] */ IInflater* inf)
{
    return constructor(out, inf, DEFAULT_BUFFER_SIZE);
}

ECode InflaterOutputStream::constructor(
    /* [in] */ IOutputStream* out,
    /* [in] */ IInflater* inf,
    /* [in] */ Int32 bufferSize)
{
    FAIL_RETURN(FilterOutputStream::constructor(out))

    if (out == NULL || inf == NULL) {
//        throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (inf == NULL) {
        return E_NULL_POINTER_EXCEPTION;
//        throw new NullPointerException("inf == null");
    }

    if (bufferSize <= 0) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mInf = inf;
    mBuf = ArrayOf<Byte>::Alloc(bufferSize);
    return NOERROR;
}

ECode InflaterOutputStream::Close()
{
    if (!mClosed) {
        FAIL_RETURN(Finish());
        FAIL_RETURN(mInf->End());
        ICloseable::Probe(mOut)->Close();
        mClosed = TRUE;
    }
    return NOERROR;
}

ECode InflaterOutputStream::Flush()
{
    FAIL_RETURN(Finish());
    return IFlushable::Probe(mOut)->Flush();
}

ECode InflaterOutputStream::Finish()
{
    FAIL_RETURN(CheckClosed());
    return Write();
}

ECode InflaterOutputStream::Write(
    /* [in] */ Int32 b)
{
    AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(1);
    buf->Set(0, (Byte)b);
    return Write(buf, 0, 1);
}

ECode InflaterOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    VALIDATE_NOT_NULL(bytes);
    FAIL_RETURN(CheckClosed());
    FAIL_RETURN(Arrays::CheckOffsetAndCount(bytes->GetLength(), offset, byteCount));

    FAIL_RETURN(mInf->SetInput(bytes, offset, byteCount));
    return Write();
}

ECode InflaterOutputStream::Write()
{
    Int32 inflated;
    ECode ec = mInf->Inflate(mBuf, &inflated);
    if (ec == (ECode)E_DATA_FORMAT_EXCEPTION) return E_ZIP_EXCEPTION;

    while (inflated > 0) {
        FAIL_RETURN(mOut->Write(mBuf, 0, inflated));
        ec = mInf->Inflate(mBuf, &inflated);
        if (ec == (ECode)E_DATA_FORMAT_EXCEPTION) return E_ZIP_EXCEPTION;
    }
    return NOERROR;
}

ECode InflaterOutputStream::CheckClosed()
{
    if (mClosed) {
//        throw new IOException();
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode InflaterOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    VALIDATE_NOT_NULL(buffer)
    // BEGIN android-note
    // changed array notation to be consistent with the rest of harmony
    // END android-note
    return Write(buffer, 0, buffer->GetLength());
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
