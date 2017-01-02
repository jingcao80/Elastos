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

#ifndef __ELASTOS_DROID_SERVER_PM_DEX_MEMMAP_H__
#define __ELASTOS_DROID_SERVER_PM_DEX_MEMMAP_H__

#include <elastos/core/Object.h>
#include <sys/mman.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

class MemMap : public Object
{
public:
    // Request an anonymous region of length 'byte_count' and a requested base address.
    // Use NULL as the requested base address if you don't care.
    //
    // The word "anonymous" in this context means "not backed by a file". The supplied
    // 'ashmem_name' will be used -- on systems that support it -- to give the mapping
    // a name.
    //
    // On success, returns returns a MemMap instance.  On failure, returns a NULL;
    static CARAPI_(AutoPtr<MemMap>) MapAnonymous(
        /* [in] */ const char* ashmem_name,
        /* [in] */ Byte* addr,
        /* [in] */ size_t byte_count,
        /* [in] */ int prot,
        /* [in] */ Boolean low_4gb, String* error_msg);

    // Map part of a file, taking care of non-page aligned offsets.  The
    // "start" offset is absolute, not relative.
    //
    // On success, returns returns a MemMap instance.  On failure, returns a NULL;
    static CARAPI_(AutoPtr<MemMap>) MapFile(
        /* [in] */ size_t byte_count,
        /* [in] */ int prot,
        /* [in] */ int flags,
        /* [in] */ int fd,
        /* [in] */ off_t start,
        /* [in] */ const char* filename,
        /* [out] */ String* error_msg);

    // Map part of a file, taking care of non-page aligned offsets.  The
    // "start" offset is absolute, not relative. This version allows
    // requesting a specific address for the base of the
    // mapping. "reuse" allows us to create a view into an existing
    // mapping where we do not take ownership of the memory.
    //
    // On success, returns returns a MemMap instance.  On failure, returns a
    // nullptr;
    static CARAPI_(AutoPtr<MemMap>) MapFileAtAddress(
        /* [in] */ Byte* addr,
        /* [in] */ size_t byte_count,
        /* [in] */ int prot,
        /* [in] */ int flags,
        /* [in] */ int fd,
        /* [in] */ off_t start,
        /* [in] */ Boolean reuse,
        /* [in] */ const char* filename,
        /* [out] */ String* error_msg);

    CARAPI_(Byte*) Begin()
    {
        return mBegin;
    }

    CARAPI_(size_t) Size()
    {
        return mSize;
    }

    CARAPI_(Byte*) End()
    {
        return Begin() + Size();
    }

private:
    MemMap(
        /* [in] */ const String& name,
        /* [in] */ Byte* begin,
        /* [in] */ size_t size,
        /* [in] */ void* base_begin,
        /* [in] */ size_t base_size,
        /* [in] */ int prot,
        /* [in] */ Boolean reuse);

private:
    const String mName;
    Byte* const mBegin;  // Start of data.
    size_t mSize;  // Length of data.

    void* const mBaseBegin;  // Page-aligned base address.
    size_t mBaseSize;  // Length of mapping. May be changed by RemapAtEnd (ie Zygote).
    int mProt;  // Protection of the map.

    // When reuse_ is true, this is just a view of an existing mapping
    // and we do not take ownership and are not responsible for
    // unmapping.
    const Boolean mReuse;
};

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_DEX_MEMMAP_H__
