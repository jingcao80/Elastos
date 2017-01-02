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

#include "FilterInputStream.h"
#include "AutoLock.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FilterInputStream, InputStream, IFilterInputStream)

FilterInputStream::FilterInputStream()
{}

FilterInputStream::~FilterInputStream()
{}

ECode FilterInputStream::constructor(
    /* [in] */ IInputStream* in)
{
    mIn = in;
    return NOERROR;
}

ECode FilterInputStream::Available(
    /* [out] */ Int32* number)
{
    return mIn->Available(number);
}

ECode FilterInputStream::Close()
{
    return ICloseable::Probe(mIn)->Close();
}

ECode FilterInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    AutoLock lock(this);
    return mIn->Mark(readLimit);
}

ECode FilterInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    return mIn->IsMarkSupported(supported);
}

ECode FilterInputStream::Read(
    /* [out] */ Int32* value)
{
    return mIn->Read(value);
}

ECode FilterInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    return mIn->Read(buffer, byteOffset, byteCount, number);
}

ECode FilterInputStream::Reset()
{
    AutoLock lock(this);
    return mIn->Reset();
}

ECode FilterInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    return mIn->Skip(byteCount, number);
}

} // namespace IO
} // namespace Elastos
