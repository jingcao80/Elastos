
#ifndef __ELASTOS_DROID_SERVER_PM_DEX_ELFFILE_H__
#define __ELASTOS_DROID_SERVER_PM_DEX_ELFFILE_H__

#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/server/pm/dex/elf.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::Object;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::HashMap;

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

private:
    ElfFile(
        /* [in] */ IFile* file,
        /* [in] */ Boolean writable,
        /* [in] */ Boolean program_header_only);

    CARAPI_(Boolean) Setup(
        /* [in] */ int prot,
        /* [in] */ int flags,
        /* [in] */ String* error_msg);

    typedef HashMap<String, Elf32_Sym*> SymbolTable;

private:

    const AutoPtr<IFile> mFile;
    const Boolean mWritable;
    const Boolean mProgramHeaderOnly;

    Elf32_Ehdr* mHeader;

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
