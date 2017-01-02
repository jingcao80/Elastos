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

#ifndef __ELASTOS_DROID_SERVER_PM_DEX_ELFFILE_H__
#define __ELASTOS_DROID_SERVER_PM_DEX_ELFFILE_H__

#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/server/pm/dex/elf.h"
#include "elastos/droid/server/pm/dex/MemMap.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Core::Object;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

class ElfFile : public Object
{
public:
    static CARAPI_(AutoPtr<ElfFile>) Open(
        /* [in] */ IFile* file,
        /* [in] */ Boolean writable,
        /* [in] */ Boolean program_header_only,
        /* [in] */ String* error_msg);

    CARAPI_(Byte*) Begin() const
    {
        return mMap->Begin();
    }

    CARAPI_(Byte*) End() const
    {
        return mMap->End();
    }

    CARAPI_(size_t) Size() const
    {
        return mMap->Size();
    }

    CARAPI_(Elf32_Ehdr&) GetHeader() const;

    CARAPI_(Elf32_Word) GetProgramHeaderNum() const;

    CARAPI_(Elf32_Phdr*) GetProgramHeader(
        /* [in] */ Elf32_Word) const;

    CARAPI_(Elf32_Word) GetSectionHeaderNum() const;

    CARAPI_(Elf32_Shdr*) GetSectionHeader(
        /* [in] */ Elf32_Word) const;

    // Find .dynsym using .hash for more efficient lookup than FindSymbolAddress.
    CARAPI_(const Byte*) FindDynamicSymbolAddress(
        /* [in] */ const String& symbol_name) const;

    CARAPI_(Elf32_Sym*) GetSymbol(
        /* [in] */ Elf32_Word section_type,
        /* [in] */ Elf32_Word i) const;

    // Lookup a string given string section and offset. Returns nullptr for
    // special 0 offset.
    CARAPI_(const char*) GetString(
        /* [in] */ Elf32_Shdr&,
        /* [in] */ Elf32_Word) const;

    CARAPI_(Elf32_Word) GetDynamicNum() const;

    CARAPI_(Elf32_Dyn&) GetDynamic(
        /* [in] */ Elf32_Word) const;

    // Returns the expected size when the file is loaded at runtime
    CARAPI_(size_t) GetLoadedSize() const;

    // Load segments into memory based on PT_LOAD program headers.
    // executable is true at run time, false at compile time.
    CARAPI_(Boolean) Load(
        /* [in] */ Boolean executable,
        /* [in] */ String* error_msg);

private:
    ElfFile(
        /* [in] */ IFile* file,
        /* [in] */ Boolean writable,
        /* [in] */ Boolean program_header_only);

    CARAPI_(Boolean) Setup(
        /* [in] */ int prot,
        /* [in] */ int flags,
        /* [in] */ String* error_msg);

    CARAPI_(Boolean) SetMap(
        /* [in] */ MemMap* map,
        /* [in] */ String* error_msg);

    CARAPI_(Byte*) GetProgramHeadersStart() const;

    CARAPI_(Byte*) GetSectionHeadersStart() const;

    CARAPI_(Elf32_Phdr&) GetDynamicProgramHeader() const;

    CARAPI_(Elf32_Dyn*) GetDynamicSectionStart() const;

    CARAPI_(Elf32_Sym*) GetSymbolSectionStart(
        /* [in] */ Elf32_Word section_type) const;

    CARAPI_(const char*) GetStringSectionStart(
        /* [in] */ Elf32_Word section_type) const;

    CARAPI_(Elf32_Word*) GetHashSectionStart() const;

    CARAPI_(Elf32_Word) GetHashBucketNum() const;

    CARAPI_(Elf32_Word) GetHashChainNum() const;

    CARAPI_(Elf32_Word) GetHashBucket(
        /* [in] */ size_t i,
        /* [in] */ Boolean* ok) const;

    CARAPI_(Elf32_Word) GetHashChain(
        /* [in] */ size_t i,
        /* [in] */ Boolean* ok) const;

    typedef HashMap<String, Elf32_Sym*> SymbolTable;

    CARAPI_(Boolean) ValidPointer(
        /* [in] */ const Byte* start) const;

    CARAPI_(const Elf32_Sym*) FindDynamicSymbol(
        /* [in] */ const String& symbol_name) const;

    // Check that certain sections and their dependencies exist.
    CARAPI_(Boolean) CheckSectionsExist(
        /* [in] */ String* error_msg) const;

    // Check that the link of the first section links to the second section.
    CARAPI_(Boolean) CheckSectionsLinked(
        /* [in] */ const Byte* source,
        /* [in] */ const Byte* target) const;

    // Lookup a string by section type. Returns nullptr for special 0 offset.
    CARAPI_(const char*) GetString(
        /* [in] */ Elf32_Word section_type,
        /* [in] */ Elf32_Word) const;

private:
    const AutoPtr<IFile> mFile;
    const Boolean mWritable;
    const Boolean mProgramHeaderOnly;

    // ELF header mapping. If program_header_only_ is false, will
    // actually point to the entire elf file.
    AutoPtr<MemMap> mMap;
    Elf32_Ehdr* mHeader;
    Vector< AutoPtr<MemMap> > mSegments;

    // Pointer to start of first PT_LOAD program segment after Load()
    // when program_header_only_ is true.
    Byte* mBaseAddress;

    // The program header should always available but use GetProgramHeadersStart() to be sure.
    Byte* mProgramHeadersStart;

    // Conditionally available values. Use accessors to ensure they exist if they are required.
    Byte* mSectionHeadersStart;
    Elf32_Phdr* mDynamicProgramHeader;
    Elf32_Dyn* mDynamicSectionStart;
    Elf32_Sym* mSymtabSectionStart;
    Elf32_Sym* mDynsymSectionStart;
    char* mStrtabSectionStart;
    char* mDynstrSectionStart;
    Elf32_Word* mHashSectionStart;

    SymbolTable* mSymtabSymbolTable;
    SymbolTable* mDynsymSymbolTable;
};

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_DEX_ELFFILE_H__
