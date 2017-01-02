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

#include "org/apache/http/impl/io/ChunkedOutputStream.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

ChunkedOutputStream::ChunkedOutputStream(
    /* [in] */ ISessionOutputBuffer* out,
    /* [in] */ Int32 bufferSize)
    : OutputStream()
    , mOut(out)
    , mCachePosition(0)
    , mWroteLastChunk(FALSE)
    , mClosed(FALSE)
{
    mCache = ArrayOf<Byte>::Alloc(bufferSize);
}

ChunkedOutputStream::ChunkedOutputStream(
    /* [in] */ ISessionOutputBuffer* out)
    : OutputStream()
    , mOut(out)
    , mCachePosition(0)
    , mWroteLastChunk(FALSE)
    , mClosed(FALSE)
{
    mCache = ArrayOf<Byte>::Alloc(2048);
}

ECode ChunkedOutputStream::FlushCache()
{
    if (mCachePosition > 0) {
        mOut->WriteLine(StringUtils::ToHexString(mCachePosition));
        mOut->Write(mCache, 0, mCachePosition);
        mOut->WriteLine(String(""));
        mCachePosition = 0;
    }
    return NOERROR;
}

ECode ChunkedOutputStream::FlushCacheWithAppend(
    /* [in] */ ArrayOf<Byte>* bufferToAppend,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    mOut->WriteLine(StringUtils::ToHexString(mCachePosition + len));
    mOut->Write(mCache, 0, mCachePosition);
    mOut->Write(bufferToAppend, off, len);
    mOut->WriteLine(String(""));
    mCachePosition = 0;
    return NOERROR;
}

ECode ChunkedOutputStream::WriteClosingChunk()
{
    // Write the final chunk.
    mOut->WriteLine(String("0"));
    mOut->WriteLine(String(""));
    return NOERROR;
}

ECode ChunkedOutputStream::Finish()
{
    if (!mWroteLastChunk) {
        FlushCache();
        WriteClosingChunk();
        mWroteLastChunk = TRUE;
    }
    return NOERROR;
}

ECode ChunkedOutputStream::Write(
    /* [in] */ Int32 b)
{
    if (mClosed) {
        Logger::E("ChunkedOutputStream", "Attempted write to closed stream.");
        return E_IO_EXCEPTION;
    }
    (*mCache)[mCachePosition] = (Byte)b;
    mCachePosition++;
    if (mCachePosition == mCache->GetLength()) {
        FlushCache();
    }
    return NOERROR;
}

ECode ChunkedOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* b)
{
    return Write(b, 0, b->GetLength());
}

ECode ChunkedOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* src,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (mClosed) {
        Logger::E("ChunkedOutputStream", "Attempted write to closed stream.");
        return E_IO_EXCEPTION;
    }
    if (len >= mCache->GetLength() - mCachePosition) {
        return FlushCacheWithAppend(src, off, len);
    }
    else {
        mCache->Copy(mCachePosition, src, off, len);
        mCachePosition += len;
    }
    return NOERROR;
}

ECode ChunkedOutputStream::Flush()
{
    FlushCache();
    return mOut->Flush();
}

ECode ChunkedOutputStream::Close()
{
    if (!mClosed) {
        mClosed = TRUE;
        Finish();
        mOut->Flush();
    }
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org