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

#ifndef __LIBCORE_IO_CMEMORYMAPPEDFILE_H__
#define __LIBCORE_IO_CMEMORYMAPPEDFILE_H__

#include "_Libcore_IO_CMemoryMappedFile.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ICloseable;

namespace Libcore {
namespace IO {

CarClass(CMemoryMappedFile)
    , public Object
    , public IMemoryMappedFile
    , public ICloseable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int64 address,
        /* [in] */ Int64 size);

    /**
     * Returns a new iterator that treats the mapped data as big-endian.
     */
    CARAPI BigEndianIterator(
        /* [out] */ IBufferIterator** it);

    /**
     * Returns a new iterator that treats the mapped data as little-endian.
     */
    CARAPI LittleEndianIterator(
        /* [out] */ IBufferIterator** it);

    /**
     * Returns the size in bytes of the memory-mapped region.
     */
    CARAPI Size(
        /* [out] */ Int64* size);

    CARAPI Close();

    static CARAPI MmapRO(
        /* [in] */ const String& path,
        /* [in] */ IMemoryMappedFile** rst);

private:
    Int64 mAddress;
    Int64 mSize;
};

} // namespace IO
} // namespace Libcore

#endif
