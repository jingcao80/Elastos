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

#include "FilterWriter.h"
#include "AutoLock.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FilterWriter, Writer, IFilterWriter)

FilterWriter::FilterWriter()
{
}

FilterWriter::~FilterWriter()
{
}

ECode FilterWriter::constructor(
    /* [in] */ IWriter* out)
{
    FAIL_RETURN(Writer::constructor(ISynchronize::Probe(out)))

    mOut = out;

    return NOERROR;
}

ECode FilterWriter::Close()
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return ICloseable::Probe(mOut)->Close();
}

ECode FilterWriter::Flush()
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return IFlushable::Probe(mOut)->Flush();
}

ECode FilterWriter::Write(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mOut->Write(buffer, offset, count);
}

ECode FilterWriter::Write(
    /* [in] */ Int32 oneChar32)
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mOut->Write(oneChar32);
}

ECode FilterWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    return mOut->Write(str, offset, count);
}

} // namespace IO
} // namespace Elastos
