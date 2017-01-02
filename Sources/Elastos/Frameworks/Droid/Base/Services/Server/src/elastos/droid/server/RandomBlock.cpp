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

#include <elastos/droid/server/RandomBlock.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::IDataOutput;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CRandomAccessFile;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String RandomBlock::TAG("RandomBlock");
const Boolean RandomBlock::DEBUG = FALSE;
const Int32 RandomBlock::BLOCK_SIZE = 512;

RandomBlock::RandomBlock()
{
    mBlock = ArrayOf<Byte>::Alloc(BLOCK_SIZE);
}

ECode RandomBlock::FromFile(
    /* [in] */ const String& filename,
    /* [out] */ RandomBlock** block)
{
    VALIDATE_NOT_NULL(block)
    *block = NULL;

    if (DEBUG) Slogger::V(TAG, "reading from file %s", filename.string());
    AutoPtr<IInputStream> stream;
    FAIL_RETURN(CFileInputStream::New(filename, (IInputStream**)&stream));
    ECode ec = FromStream(stream, block);
    Close(ICloseable::Probe(stream));
    return ec;
}

ECode RandomBlock::FromStream(
    /* [in] */ IInputStream* in,
    /* [out] */ RandomBlock** block)
{
    VALIDATE_NOT_NULL(block)
    *block = NULL;
    VALIDATE_NOT_NULL(in)

    AutoPtr<RandomBlock> retval = new RandomBlock();
    Int32 total = 0, result;
    while(total < BLOCK_SIZE) {
        FAIL_RETURN(in->Read(retval->mBlock, total, BLOCK_SIZE - total, &result))
        if (result == -1) {
            return E_EOF_EXCEPTION;
        }
        total += result;
    }

    *block = retval;
    REFCOUNT_ADD(*block)
    return NOERROR;
}

ECode RandomBlock::ToFile(
    /* [in] */ const String& filename,
    /* [in] */ Boolean sync)
{
    if (DEBUG) Slogger::V(TAG, "writing to file %s", filename.string());
    AutoPtr<IRandomAccessFile> out;
    CRandomAccessFile::New(filename, String(sync ? "rws" : "rw"), (IRandomAccessFile**)&out);
    FAIL_RETURN(ToDataOut(IDataOutput::Probe(out)))
    TruncateIfPossible(out);
    Close(ICloseable::Probe(out));
    return NOERROR;
}

void RandomBlock::TruncateIfPossible(
    /* [in] */ IRandomAccessFile* f)
{
    // try {
    f->SetLength(BLOCK_SIZE);
    // } catch (IOException e) {
        // ignore this exception.  Sometimes, the file we're trying to
        // write is a character device, such as /dev/urandom, and
        // these character devices do not support setting the length.
    // }
}

ECode RandomBlock::ToDataOut(
    /* [in] */ IDataOutput* out)
{
    return out->Write(mBlock);
}

void RandomBlock::Close(
    /* [in] */ ICloseable* c)
{
    // try {
    if (c == NULL) {
        return;
    }
    ECode ec = c->Close();
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG, "IOException thrown while closing Closeable");
    }
}



} // Server
} // Droid
} // Elastos