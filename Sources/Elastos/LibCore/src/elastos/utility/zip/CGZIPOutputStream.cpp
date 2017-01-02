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

#include "CGZIPOutputStream.h"
#include "CDeflater.h"
#include "CCRC32.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CAR_INTERFACE_IMPL(CGZIPOutputStream, DeflaterOutputStream, IGZIPOutputStream)

CAR_OBJECT_IMPL(CGZIPOutputStream)

CGZIPOutputStream::CGZIPOutputStream()
{
    CCRC32::NewByFriend((CCRC32**)&mCrc);
}

ECode CGZIPOutputStream::constructor(
    /* [in] */ IOutputStream* os)
{
    return constructor(os, BUF_SIZE, FALSE);
}

ECode CGZIPOutputStream::constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ Int32 size)
{
    return constructor(os, size, FALSE);
}

ECode CGZIPOutputStream::constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ Boolean syncFlush)
{
    return constructor(os, BUF_SIZE, syncFlush);
}

ECode CGZIPOutputStream::constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ Int32 size,
    /* [in] */ Boolean syncFlush)
{
    AutoPtr<CDeflater> deflater;
    CDeflater::NewByFriend(IDeflater::DEFAULT_COMPRESSION, TRUE, (CDeflater**)&deflater);
    FAIL_RETURN(DeflaterOutputStream::constructor(os, deflater.Get(), size, syncFlush));
    WriteInt32(IGZIPInputStream::GZIP_MAGIC);
    FAIL_RETURN(mOut->Write(IDeflater::DEFLATED));
    FAIL_RETURN(mOut->Write(0)); // flags
    WriteInt64(0); // mod time
    FAIL_RETURN(mOut->Write(0)); // extra flags
    return mOut->Write(0); // operating system
}

ECode CGZIPOutputStream::Finish()
{
    FAIL_RETURN(DeflaterOutputStream::Finish());
    Int64 checksum;
    mCrc->GetValue(&checksum);
    WriteInt64(checksum);
    WriteInt64(mCrc->mTbytes);
    return NOERROR;
}

ECode CGZIPOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 off,
    /* [in] */ Int32 nbytes)
{
    FAIL_RETURN(DeflaterOutputStream::Write(buffer, off, nbytes));
    return mCrc->Update(buffer, off, nbytes);
}

Int64 CGZIPOutputStream::WriteInt64(
    /* [in] */ Int64 i)
{
    // Write out the long value as an unsigned int
    Int32 ui = (Int32)i;
    mOut->Write(ui & 0xFF);
    mOut->Write((ui >> 8) & 0xFF);
    mOut->Write((ui >> 16) & 0xFF);
    mOut->Write((ui >> 24) & 0xFF);
    return i;
}

Int32 CGZIPOutputStream::WriteInt32(
    /* [in] */ Int32 i)
{
    mOut->Write(i & 0xFF);
    mOut->Write((i >> 8) & 0xFF);
    return i;
}

} // namespace Zip
} // namespace Utility
} // namespace Elastos
