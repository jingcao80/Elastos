
#include "elastos/droid/server/pm/DexFile.h"
#include "elastos/droid/server/pm/dex/ElfFile.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

#if defined(__arm__)
static const InstructionSet kRuntimeISA = kArm;
#elif defined(__aarch64__)
static const InstructionSet kRuntimeISA = kArm64;
#elif defined(__mips__)
static const InstructionSet kRuntimeISA = kMips;
#elif defined(__i386__)
static const InstructionSet kRuntimeISA = kX86;
#elif defined(__x86_64__)
static const InstructionSet kRuntimeISA = kX86_64;
#else
static const InstructionSet kRuntimeISA = kNone;
#endif

static const int kPageSize = 4096;

ElfFile::ElfFile(
    /* [in] */ IFile* file,
    /* [in] */ Boolean writable,
    /* [in] */ Boolean program_header_only)
    : mFile(file)
    , mWritable(writable)
    , mProgramHeaderOnly(program_header_only)
    , mHeader(NULL)
    , mBaseAddress(NULL)
    , mProgramHeadersStart(NULL)
    , mSectionHeadersStart(NULL)
    , mDynamicProgramHeader(NULL)
    , mDynamicSectionStart(NULL)
    , mSymtabSectionStart(NULL)
    , mDynsymSectionStart(NULL)
    , mStrtabSectionStart(NULL)
    , mDynstrSectionStart(NULL)
    , mHashSectionStart(NULL)
    , mSymtabSymbolTable(NULL)
    , mDynsymSymbolTable(NULL)
    // , jit_elf_image_(NULL)
    // , jit_gdb_entry_(NULL)
{
    assert(mFile != NULL);
}

AutoPtr<ElfFile> ElfFile::Open(
    /* [in] */ IFile* file,
    /* [in] */ Boolean writable,
    /* [in] */ Boolean program_header_only,
    /* [in] */ String* error_msg)
{
    AutoPtr<ElfFile> elf_file = new ElfFile(file, writable, program_header_only);
    int prot;
    int flags;
    if (writable) {
        prot = PROT_READ | PROT_WRITE;
        flags = MAP_SHARED;
    }
    else {
        prot = PROT_READ;
        flags = MAP_PRIVATE;
    }
    if (!elf_file->Setup(prot, flags, error_msg)) {
      return NULL;
    }
    return elf_file;
}

Boolean ElfFile::Setup(
    /* [in] */ int prot,
    /* [in] */ int flags,
    /* [in] */ String* error_msg)
{
    Int64 temp_file_length;
    mFile->GetLength(&temp_file_length);
    String path;
    mFile->GetPath(&path);
    if (temp_file_length < 0) {
        errno = -temp_file_length;
        String msg("");
        msg.AppendFormat("Failed to get length of file: '%s' : %s",
                path.string(), strerror(errno));
        *error_msg = msg;
        return FALSE;
    }
    size_t file_length = static_cast<size_t>(temp_file_length);
    if (file_length < sizeof(Elf32_Ehdr)) {
        String msg("");
        msg.AppendFormat("File size of %zd bytes not large enough to contain ELF header of "
                "%zd bytes: '%s'", file_length, sizeof(Elf32_Ehdr),
                path.string());
        return FALSE;
    }

    AutoPtr<IFileInputStream> fis;
    CFileInputStream::New(mFile, (IFileInputStream**)&fis);
    AutoPtr<IFileDescriptor> fdObj;
    fis->GetFD((IFileDescriptor**)&fdObj);
    Int32 fd;
    fdObj->GetDescriptor(&fd);
    if (mProgramHeaderOnly) {
        // first just map ELF header to get program header size information
        size_t elf_header_size = sizeof(Elf32_Ehdr);
        if (!SetMap(MemMap::MapFile(elf_header_size, prot, flags, fd, 0,
                path.string(), error_msg), error_msg)) {
            return FALSE;
        }
        // then remap to cover program header
        size_t program_header_size = mHeader->e_phoff + (mHeader->e_phentsize * mHeader->e_phnum);
        if (file_length < program_header_size) {
            String msg("");
            msg.AppendFormat("File size of %zd bytes not large enough to contain ELF program "
                    "header of %zd bytes: '%s'", file_length,
                    sizeof(Elf32_Ehdr), path.string());
            *error_msg = msg;
            return FALSE;
        }
        if (!SetMap(MemMap::MapFile(program_header_size, prot, flags, fd, 0,
                path.string(), error_msg), error_msg)) {
            *error_msg = String("Failed to map ELF program headers: ") + *error_msg;
            return FALSE;
        }
    }
    else {
        // otherwise map entire file
        if (!SetMap(MemMap::MapFile(temp_file_length, prot, flags, fd, 0,
                path.string(), error_msg), error_msg)) {
            *error_msg = String("Failed to map ELF file: ") + *error_msg;
            return FALSE;
        }
    }

    if (mProgramHeaderOnly) {
        mProgramHeadersStart = Begin() + GetHeader().e_phoff;
    }
    else {
        assert(0);
        // if (!CheckAndSet(GetHeader().e_phoff, "program headers", &program_headers_start_, error_msg)) {
        //   return false;
        // }

        // // Setup section headers.
        // if (!CheckAndSet(GetHeader().e_shoff, "section headers", &section_headers_start_, error_msg)) {
        //   return false;
        // }

        // // Find shstrtab.
        // Elf32_Shdr* shstrtab_section_header = GetSectionNameStringSection();
        // if (shstrtab_section_header == nullptr) {
        //   *error_msg = StringPrintf("Failed to find shstrtab section header in ELF file: '%s'",
        //                             file_->GetPath().c_str());
        //   return false;
        // }

        // // Find .dynamic section info from program header
        // dynamic_program_header_ = FindProgamHeaderByType(PT_DYNAMIC);
        // if (dynamic_program_header_ == nullptr) {
        //   *error_msg = StringPrintf("Failed to find PT_DYNAMIC program header in ELF file: '%s'",
        //                             file_->GetPath().c_str());
        //   return false;
        // }

        // if (!CheckAndSet(GetDynamicProgramHeader().p_offset, "dynamic section",
        //                  reinterpret_cast<byte**>(&dynamic_section_start_), error_msg)) {
        //   return false;
        // }

        // // Find other sections from section headers
        // for (Elf32_Word i = 0; i < GetSectionHeaderNum(); i++) {
        //   Elf32_Shdr* section_header = GetSectionHeader(i);
        //   if (section_header == nullptr) {
        //     *error_msg = StringPrintf("Failed to find section header for section %d in ELF file: '%s'",
        //                               i, file_->GetPath().c_str());
        //     return false;
        //   }
        //   switch (section_header->sh_type) {
        //     case SHT_SYMTAB: {
        //       if (!CheckAndSet(section_header->sh_offset, "symtab",
        //                        reinterpret_cast<byte**>(&symtab_section_start_), error_msg)) {
        //         return false;
        //       }
        //       break;
        //     }
        //     case SHT_DYNSYM: {
        //       if (!CheckAndSet(section_header->sh_offset, "dynsym",
        //                        reinterpret_cast<byte**>(&dynsym_section_start_), error_msg)) {
        //         return false;
        //       }
        //       break;
        //     }
        //     case SHT_STRTAB: {
        //       // TODO: base these off of sh_link from .symtab and .dynsym above
        //       if ((section_header->sh_flags & SHF_ALLOC) != 0) {
        //         // Check that this is named ".dynstr" and ignore otherwise.
        //         const char* header_name = GetString(*shstrtab_section_header, section_header->sh_name);
        //         if (strncmp(".dynstr", header_name, 8) == 0) {
        //           if (!CheckAndSet(section_header->sh_offset, "dynstr",
        //                            reinterpret_cast<byte**>(&dynstr_section_start_), error_msg)) {
        //             return false;
        //           }
        //         }
        //       } else {
        //         // Check that this is named ".strtab" and ignore otherwise.
        //         const char* header_name = GetString(*shstrtab_section_header, section_header->sh_name);
        //         if (strncmp(".strtab", header_name, 8) == 0) {
        //           if (!CheckAndSet(section_header->sh_offset, "strtab",
        //                            reinterpret_cast<byte**>(&strtab_section_start_), error_msg)) {
        //             return false;
        //           }
        //         }
        //       }
        //       break;
        //     }
        //     case SHT_DYNAMIC: {
        //       if (reinterpret_cast<byte*>(dynamic_section_start_) !=
        //           Begin() + section_header->sh_offset) {
        //         LOG(WARNING) << "Failed to find matching SHT_DYNAMIC for PT_DYNAMIC in "
        //                      << file_->GetPath() << ": " << std::hex
        //                      << reinterpret_cast<void*>(dynamic_section_start_)
        //                      << " != " << reinterpret_cast<void*>(Begin() + section_header->sh_offset);
        //         return false;
        //       }
        //       break;
        //     }
        //     case SHT_HASH: {
        //       if (!CheckAndSet(section_header->sh_offset, "hash section",
        //                        reinterpret_cast<byte**>(&hash_section_start_), error_msg)) {
        //         return false;
        //       }
        //       break;
        //     }
        //   }
        // }

        // // Check for the existence of some sections.
        // if (!CheckSectionsExist(error_msg)) {
        //   return false;
        // }
    }

    return TRUE;
}

Boolean ElfFile::CheckSectionsLinked(
    /* [in] */ const Byte* source,
    /* [in] */ const Byte* target) const
{
    // Only works in whole-program mode, as we need to iterate over the sections.
    // Note that we normally can't search by type, as duplicates are allowed for most section types.
    if (mProgramHeaderOnly) {
        return TRUE;
    }

    Elf32_Shdr* source_section = NULL;
    Elf32_Word target_index = 0;
    Boolean target_found = FALSE;
    for (Elf32_Word i = 0; i < GetSectionHeaderNum(); i++) {
        Elf32_Shdr* section_header = GetSectionHeader(i);

        if (Begin() + section_header->sh_offset == source) {
            // Found the source.
            source_section = section_header;
            if (target_index) {
                break;
            }
        }
        else if (Begin() + section_header->sh_offset == target) {
            target_index = i;
            target_found = true;
            if (source_section != NULL) {
                break;
            }
        }
    }

    return target_found && source_section != NULL && source_section->sh_link == target_index;
}

Boolean ElfFile::CheckSectionsExist(
    /* [in] */ String* error_msg) const
{
    if (!mProgramHeaderOnly) {
        // If in full mode, need section headers.
        if (mSectionHeadersStart == NULL) {
            String path;
            mFile->GetPath(&path);
            String msg("");
            msg.AppendFormat("No section headers in ELF file: '%s'", path.string());
            *error_msg = msg;
            return FALSE;
        }
    }

    // This is redundant, but defensive.
    if (mDynamicProgramHeader == NULL) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find PT_DYNAMIC program header in ELF file: '%s'", path.string());
        *error_msg = msg;
        return FALSE;
    }

    // Need a dynamic section. This is redundant, but defensive.
    if (mDynamicSectionStart == NULL) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find dynamic section in ELF file: '%s'", path.string());
        *error_msg = msg;
        return FALSE;
    }

    // Symtab validation. These is not really a hard failure, as we are currently not using the
    // symtab internally, but it's nice to be defensive.
    if (mSymtabSectionStart != NULL) {
        // When there's a symtab, there should be a strtab.
        if (mStrtabSectionStart == NULL) {
            String path;
            mFile->GetPath(&path);
            String msg("");
            msg.AppendFormat("No strtab for symtab in ELF file: '%s'", path.string());
            *error_msg = msg;
            return FALSE;
        }

        // The symtab should link to the strtab.
        if (!CheckSectionsLinked(reinterpret_cast<const Byte*>(mSymtabSectionStart),
                               reinterpret_cast<const Byte*>(mStrtabSectionStart))) {
            String path;
            mFile->GetPath(&path);
            String msg("");
            msg.AppendFormat("Symtab is not linked to the strtab in ELF file: '%s'", path.string());
            *error_msg = msg;
            return FALSE;
        }
    }

    // We always need a dynstr & dynsym.
    if (mDynstrSectionStart == NULL) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("No dynstr in ELF file: '%s'", path.string());
        *error_msg = msg;
        return FALSE;
    }
    if (mDynsymSectionStart == NULL) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("No dynsym in ELF file: '%s'", path.string());
        *error_msg = msg;
        return FALSE;
    }

    // Need a hash section for dynamic symbol lookup.
    if (mHashSectionStart == NULL) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find hash section in ELF file: '%s'", path.string());
        *error_msg = msg;
        return FALSE;
    }

    // And the hash section should be linking to the dynsym.
    if (!CheckSectionsLinked(reinterpret_cast<const Byte*>(mHashSectionStart),
                             reinterpret_cast<const Byte*>(mDynsymSectionStart))) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Hash section is not linked to the dynstr in ELF file: '%s'", path.string());
        *error_msg = msg;
        return FALSE;
    }

    return TRUE;
}

Boolean ElfFile::SetMap(
    /* [in] */ MemMap* map,
    /* [in] */ String* error_msg)
{
    if (map == NULL) {
        // MemMap::Open should have already set an error.
        assert(!error_msg->IsEmpty());
        return FALSE;
    }
    mMap = map;
    assert(mMap != NULL);
    assert(mMap->Begin() != NULL);

    mHeader = reinterpret_cast<Elf32_Ehdr*>(mMap->Begin());
    if ((ELFMAG0 != mHeader->e_ident[EI_MAG0])
        || (ELFMAG1 != mHeader->e_ident[EI_MAG1])
        || (ELFMAG2 != mHeader->e_ident[EI_MAG2])
        || (ELFMAG3 != mHeader->e_ident[EI_MAG3])) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find ELF magic value %d %d %d %d in %s, found %d %d %d %d",
                ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3,
                path.string(),
                mHeader->e_ident[EI_MAG0],
                mHeader->e_ident[EI_MAG1],
                mHeader->e_ident[EI_MAG2],
                mHeader->e_ident[EI_MAG3]);
        *error_msg = msg;
        return FALSE;
    }
    if (ELFCLASS32 != mHeader->e_ident[EI_CLASS]) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find expected EI_CLASS value %d in %s, found %d",
                ELFCLASS32,
                path.string(),
                mHeader->e_ident[EI_CLASS]);
        *error_msg = msg;
        return FALSE;
    }
    if (ELFDATA2LSB != mHeader->e_ident[EI_DATA]) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find expected EI_DATA value %d in %s, found %d",
                ELFDATA2LSB,
                path.string(),
                mHeader->e_ident[EI_CLASS]);
        *error_msg = msg;
        return FALSE;
    }
    if (EV_CURRENT != mHeader->e_ident[EI_VERSION]) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find expected EI_VERSION value %d in %s, found %d",
                EV_CURRENT,
                path.string(),
                mHeader->e_ident[EI_CLASS]);
        *error_msg = msg;
        return FALSE;
    }
    if (ET_DYN != mHeader->e_type) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find expected e_type value %d in %s, found %d",
                ET_DYN,
                path.string(),
                mHeader->e_type);
        *error_msg = msg;
        return FALSE;
    }
    if (EV_CURRENT != mHeader->e_version) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find expected e_version value %d in %s, found %d",
                EV_CURRENT,
                path.string(),
                mHeader->e_version);
        *error_msg = msg;
        return FALSE;
    }
    if (0 != mHeader->e_entry) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find expected e_entry value %d in %s, found %d",
                0,
                path.string(),
                mHeader->e_entry);
        *error_msg = msg;
        return FALSE;
    }
    if (0 == mHeader->e_phoff) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find non-zero e_phoff value in %s",
                path.string());
        *error_msg = msg;
        return FALSE;
    }
    if (0 == mHeader->e_shoff) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find non-zero e_shoff value in %s",
                path.string());
        *error_msg = msg;
        return FALSE;
    }
    if (0 == mHeader->e_ehsize) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find non-zero e_ehsize value in %s",
                path.string());
        *error_msg = msg;
        return FALSE;
    }
    if (0 == mHeader->e_phentsize) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find non-zero e_phentsize value in %s",
                path.string());
        *error_msg = msg;
        return FALSE;
    }
    if (0 == mHeader->e_phnum) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find non-zero e_phnum value in %s",
                path.string());
        *error_msg = msg;
        return FALSE;
    }
    if (0 == mHeader->e_shentsize) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find non-zero e_shentsize value in %s",
                path.string());
        *error_msg = msg;
        return FALSE;
    }
    if (0 == mHeader->e_shnum) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find non-zero e_shnum value in %s",
                path.string());
        *error_msg = msg;
        return FALSE;
    }
    if (0 == mHeader->e_shstrndx) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find non-zero e_shstrndx value in %s",
                path.string());
        *error_msg = msg;
        return FALSE;
    }
    if (mHeader->e_shstrndx >= mHeader->e_shnum) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find e_shnum value %d less than %d in %s",
                mHeader->e_shstrndx,
                mHeader->e_shnum,
                path.string());
        *error_msg = msg;
        return FALSE;
    }

    if (!mProgramHeaderOnly) {
        if (mHeader->e_phoff >= Size()) {
            String path;
            mFile->GetPath(&path);
            String msg("");
            msg.AppendFormat("Failed to find e_phoff value %d less than %zd in %s",
                    mHeader->e_phoff,
                    Size(),
                    path.string());
            *error_msg = msg;
            return FALSE;
        }
        if (mHeader->e_shoff >= Size()) {
            String path;
            mFile->GetPath(&path);
            String msg("");
            msg.AppendFormat("Failed to find e_shoff value %d less than %zd in %s",
                    mHeader->e_shoff,
                    Size(),
                    path.string());
            *error_msg = msg;
            return FALSE;
        }
    }
    return TRUE;
}

Elf32_Ehdr& ElfFile::GetHeader() const
{
    assert(mHeader != NULL);  // Header has been checked in SetMap. This is a sanity check.
    return *mHeader;
}

Byte* ElfFile::GetProgramHeadersStart() const
{
    assert(mProgramHeadersStart != NULL);  // Header has been set in Setup. This is a sanity
                                             // check.
    return mProgramHeadersStart;
}

Byte* ElfFile::GetSectionHeadersStart() const
{
    assert(!mProgramHeaderOnly);              // Only used in "full" mode.
    assert(mSectionHeadersStart != NULL);  // Is checked in CheckSectionsExist. Sanity check.
    return mSectionHeadersStart;
}

Elf32_Phdr& ElfFile::GetDynamicProgramHeader() const
{
    assert(mDynamicProgramHeader != NULL);  // Is checked in CheckSectionsExist. Sanity check.
    return *mDynamicProgramHeader;
}

Elf32_Dyn* ElfFile::GetDynamicSectionStart() const
{
    assert(mDynamicSectionStart != NULL);  // Is checked in CheckSectionsExist. Sanity check.
    return mDynamicSectionStart;
}

static Boolean IsSymbolSectionType(Elf32_Word section_type)
{
    return ((section_type == SHT_SYMTAB) || (section_type == SHT_DYNSYM));
}

Elf32_Sym* ElfFile::GetSymbolSectionStart(
    /* [in] */ Elf32_Word section_type) const
{
    assert(IsSymbolSectionType(section_type));
    switch (section_type) {
        case SHT_SYMTAB: {
            return mSymtabSectionStart;
            break;
        }
        case SHT_DYNSYM: {
            return mDynsymSectionStart;
            break;
        }
        default: {
            Slogger::E("ElfFile", "section_type %d is wrong.", section_type);
            return NULL;
        }
    }
}

const char* ElfFile::GetStringSectionStart(
    /* [in] */ Elf32_Word section_type) const
{
    assert(IsSymbolSectionType(section_type));
    switch (section_type) {
        case SHT_SYMTAB: {
            return mStrtabSectionStart;
        }
        case SHT_DYNSYM: {
            return mDynstrSectionStart;
        }
        default: {
            Slogger::E("ElfFile", "section_type %d is wrong.", section_type);
            return NULL;
        }
    }
}

const char* ElfFile::GetString(
    /* [in] */ Elf32_Word section_type,
    /* [in] */ Elf32_Word i) const
{
    assert(IsSymbolSectionType(section_type));
    if (i == 0) {
        return NULL;
    }
    const char* string_section_start = GetStringSectionStart(section_type);
    if (string_section_start == NULL) {
        return NULL;
    }
    return string_section_start + i;
}

Elf32_Word* ElfFile::GetHashSectionStart() const
{
    return mHashSectionStart;
}

Elf32_Word ElfFile::GetHashBucketNum() const
{
    return GetHashSectionStart()[0];
}

Elf32_Word ElfFile::GetHashChainNum() const
{
    return GetHashSectionStart()[1];
}

Elf32_Word ElfFile::GetHashBucket(
    /* [in] */ size_t i,
    /* [in] */ Boolean* ok) const
{
    if (i >= GetHashBucketNum()) {
        *ok = FALSE;
        return 0;
    }
    *ok = TRUE;
    // 0 is nbucket, 1 is nchain
    return GetHashSectionStart()[2 + i];
}

Elf32_Word ElfFile::GetHashChain(
    /* [in] */ size_t i,
    /* [in] */ Boolean* ok) const
{
    if (i >= GetHashBucketNum()) {
        *ok = FALSE;
        return 0;
    }
    *ok = TRUE;
    // 0 is nbucket, 1 is nchain, & chains are after buckets
    return GetHashSectionStart()[2 + GetHashBucketNum() + i];
}

Elf32_Word ElfFile::GetProgramHeaderNum() const
{
    return GetHeader().e_phnum;
}

Elf32_Phdr* ElfFile::GetProgramHeader(
    /* [in] */ Elf32_Word i) const
{
    assert(i < GetProgramHeaderNum());  // Sanity check for caller.
    Byte* program_header = GetProgramHeadersStart() + (i * GetHeader().e_phentsize);
    if (program_header >= End()) {
        return NULL;  // Failure condition.
    }
    return reinterpret_cast<Elf32_Phdr*>(program_header);
}

Elf32_Word ElfFile::GetSectionHeaderNum() const
{
    return GetHeader().e_shnum;
}

Elf32_Shdr* ElfFile::GetSectionHeader(
    /* [in] */ Elf32_Word i) const
{
    // Can only access arbitrary sections when we have the whole file, not just program header.
    // Even if we Load(), it doesn't bring in all the sections.
    assert(!mProgramHeaderOnly);
    if (i >= GetSectionHeaderNum()) {
        return NULL;  // Failure condition.
    }
    byte* section_header = GetSectionHeadersStart() + (i * GetHeader().e_shentsize);
    if (section_header >= End()) {
        return NULL;  // Failure condition.
    }
    return reinterpret_cast<Elf32_Shdr*>(section_header);
}

// from bionic
static unsigned elfhash(const char *_name)
{
    const unsigned char *name = (const unsigned char *) _name;
    unsigned h = 0, g;

    while (*name) {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }
    return h;
}

const Byte* ElfFile::FindDynamicSymbolAddress(
    /* [in] */ const String& symbol_name) const
{
    // Check that we have a hash section.
    if (GetHashSectionStart() == NULL) {
        return NULL;  // Failure condition.
    }
    const Elf32_Sym* sym = FindDynamicSymbol(symbol_name);
    if (sym != NULL) {
        return mBaseAddress + sym->st_value;
    }
    else {
        return NULL;
    }
}

const Elf32_Sym* ElfFile::FindDynamicSymbol(
    /* [in] */ const String& symbol_name) const
{
    if (GetHashBucketNum() == 0) {
        // No dynamic symbols at all.
        return NULL;
    }
    Elf32_Word hash = elfhash(symbol_name.string());
    Elf32_Word bucket_index = hash % GetHashBucketNum();
    Boolean ok;
    Elf32_Word symbol_and_chain_index = GetHashBucket(bucket_index, &ok);
    if (!ok) {
        return NULL;
    }
    while (symbol_and_chain_index != 0 /* STN_UNDEF */) {
        Elf32_Sym* symbol = GetSymbol(SHT_DYNSYM, symbol_and_chain_index);
        if (symbol == NULL) {
            return NULL;  // Failure condition.
        }
        const char* name = GetString(SHT_DYNSYM, symbol->st_name);
        if (symbol_name == name) {
            return symbol;
        }
        symbol_and_chain_index = GetHashChain(symbol_and_chain_index, &ok);
        if (!ok) {
            return NULL;
        }
    }
    return NULL;
}

Elf32_Sym* ElfFile::GetSymbol(
    /* [in] */ Elf32_Word section_type,
    /* [in] */ Elf32_Word i) const
{
    Elf32_Sym* sym_start = GetSymbolSectionStart(section_type);
    if (sym_start == NULL) {
        return NULL;
    }
    return sym_start + i;
}

Elf32_Word ElfFile::GetDynamicNum() const
{
    return GetDynamicProgramHeader().p_filesz / sizeof(Elf32_Dyn);
}

Elf32_Dyn& ElfFile::GetDynamic(
    /* [in] */ Elf32_Word i) const
{
    assert(i < GetDynamicNum());
    return *(GetDynamicSectionStart() + i);
}

// Base on bionic phdr_table_get_load_size
size_t ElfFile::GetLoadedSize() const
{
    Elf32_Addr min_vaddr = 0xFFFFFFFFu;
    Elf32_Addr max_vaddr = 0x00000000u;
    for (Elf32_Word i = 0; i < GetProgramHeaderNum(); i++) {
        Elf32_Phdr* program_header = GetProgramHeader(i);
        if (program_header->p_type != PT_LOAD) {
            continue;
        }
        Elf32_Addr begin_vaddr = program_header->p_vaddr;
        if (begin_vaddr < min_vaddr) {
            min_vaddr = begin_vaddr;
        }
        Elf32_Addr end_vaddr = program_header->p_vaddr + program_header->p_memsz;
        if (end_vaddr > max_vaddr) {
            max_vaddr = end_vaddr;
        }
    }
    min_vaddr = RoundDown(min_vaddr, kPageSize);
    max_vaddr = RoundUp(max_vaddr, kPageSize);
    assert(min_vaddr < max_vaddr);
    size_t loaded_size = max_vaddr - min_vaddr;
    return loaded_size;
}

Boolean ElfFile::Load(
    /* [in] */ Boolean executable,
    /* [in] */ String* error_msg)
{
    assert(mProgramHeaderOnly);

    if (executable) {
        InstructionSet elf_ISA = kNone;
        switch (GetHeader().e_machine) {
            case EM_ARM: {
                elf_ISA = kArm;
                break;
            }
            case EM_AARCH64: {
                elf_ISA = kArm64;
                break;
            }
            case EM_386: {
                elf_ISA = kX86;
                break;
            }
            case EM_X86_64: {
                elf_ISA = kX86_64;
                break;
            }
            case EM_MIPS: {
                elf_ISA = kMips;
                break;
            }
        }

        if (elf_ISA != kRuntimeISA) {
            String msg("");
            msg.AppendFormat("Expected ISA %d,  but found %d",
                    kRuntimeISA, elf_ISA);
            *error_msg = msg;
            return FALSE;
        }
    }

    Boolean reserved = FALSE;
    for (Elf32_Word i = 0; i < GetProgramHeaderNum(); i++) {
        Elf32_Phdr* program_header = GetProgramHeader(i);
        if (program_header == NULL) {
            String path;
            mFile->GetPath(&path);
            String msg("");
            msg.AppendFormat("No program header for entry %d in ELF file %s.",
                    i, path.string());
            *error_msg = msg;
            return FALSE;
        }

        // Record .dynamic header information for later use
        if (program_header->p_type == PT_DYNAMIC) {
            mDynamicProgramHeader = program_header;
            continue;
        }

        // Not something to load, move on.
        if (program_header->p_type != PT_LOAD) {
            continue;
        }

        // Found something to load.

        // Before load the actual segments, reserve a contiguous chunk
        // of required size and address for all segments, but with no
        // permissions. We'll then carve that up with the proper
        // permissions as we load the actual segments. If p_vaddr is
        // non-zero, the segments require the specific address specified,
        // which either was specified in the file because we already set
        // base_address_ after the first zero segment).
        Int64 temp_file_length;
        mFile->GetLength(&temp_file_length);
        if (temp_file_length < 0) {
            errno = -temp_file_length;
            String path;
            mFile->GetPath(&path);
            String msg("");
            msg.AppendFormat("Failed to get length of file: '%s' : %s",
                    path.string(), strerror(errno));
            *error_msg = msg;
            return FALSE;
        }
        size_t file_length = static_cast<size_t>(temp_file_length);
        if (!reserved) {
            Byte* reserve_base = ((program_header->p_vaddr != 0) ?
                                reinterpret_cast<byte*>(program_header->p_vaddr) : NULL);
            String reservation_name("ElfFile reservation for ");
            String path;
            mFile->GetPath(&path);
            reservation_name += path;
            AutoPtr<MemMap> reserve = MemMap::MapAnonymous(reservation_name.string(),
                                                           reserve_base,
                                                           GetLoadedSize(), PROT_NONE, FALSE,
                                                           error_msg);
            if (reserve == NULL) {
                String msg("");
                msg.AppendFormat("Failed to allocate %s: %s",
                        reservation_name.string(), error_msg->string());
                *error_msg = msg;
                return FALSE;
            }
            reserved = TRUE;
            if (reserve_base == NULL) {
                mBaseAddress = reserve->Begin();
            }
            mSegments.PushBack(reserve);
        }
        // empty segment, nothing to map
        if (program_header->p_memsz == 0) {
            continue;
        }
        Byte* p_vaddr = mBaseAddress + program_header->p_vaddr;
        int prot = 0;
        if (executable && ((program_header->p_flags & PF_X) != 0)) {
            prot |= PROT_EXEC;
        }
        if ((program_header->p_flags & PF_W) != 0) {
            prot |= PROT_WRITE;
        }
        if ((program_header->p_flags & PF_R) != 0) {
            prot |= PROT_READ;
        }
        int flags = 0;
        if (mWritable) {
            prot |= PROT_WRITE;
            flags |= MAP_SHARED;
        }
        else {
            flags |= MAP_PRIVATE;
        }
        if (file_length < (program_header->p_offset + program_header->p_memsz)) {
            String path;
            mFile->GetPath(&path);
            String msg("");
            msg.AppendFormat("File size of %zd bytes not large enough to contain ELF segment "
                    "%d of %d bytes: '%s'", file_length, i,
                    program_header->p_offset + program_header->p_memsz,
                    path.string());
            *error_msg = msg;
            return FALSE;
        }
        AutoPtr<IFileInputStream> fis;
        CFileInputStream::New(mFile, (IFileInputStream**)&fis);
        AutoPtr<IFileDescriptor> fdObj;
        fis->GetFD((IFileDescriptor**)&fdObj);
        Int32 fd;
        fdObj->GetDescriptor(&fd);
        String path;
        mFile->GetPath(&path);
        AutoPtr<MemMap> segment = MemMap::MapFileAtAddress(p_vaddr,
                                                           program_header->p_memsz,
                                                           prot, flags, fd,
                                                           program_header->p_offset,
                                                           TRUE,  // implies MAP_FIXED
                                                           path.string(),
                                                           error_msg);
        if (segment == NULL) {
            String path;
            mFile->GetPath(&path);
            String msg("");
            msg.AppendFormat("Failed to map ELF file segment %d from %s: %s",
                                    i, path.string(), error_msg->string());
            *error_msg = msg;
            return FALSE;
        }
        if (segment->Begin() != p_vaddr) {
            String path;
            mFile->GetPath(&path);
            String msg("");
            msg.AppendFormat("Failed to map ELF file segment %d from %s at expected address %p, "
                                    "instead mapped to %p",
                                    i, path.string(), p_vaddr, segment->Begin());
            *error_msg = msg;
            return FALSE;
        }
        mSegments.PushBack(segment);
    }

    // Now that we are done loading, .dynamic should be in memory to find .dynstr, .dynsym, .hash
    Byte* dsptr = mBaseAddress + GetDynamicProgramHeader().p_vaddr;
    if ((dsptr < Begin() || dsptr >= End()) && !ValidPointer(dsptr)) {
        String path;
        mFile->GetPath(&path);
        String msg("");
        msg.AppendFormat("dynamic section address invalid in ELF file %s",
                            path.string());
        *error_msg = msg;
        return FALSE;
    }
    mDynamicSectionStart = reinterpret_cast<Elf32_Dyn*>(dsptr);

    for (Elf32_Word i = 0; i < GetDynamicNum(); i++) {
        Elf32_Dyn& elf_dyn = GetDynamic(i);
        Byte* d_ptr = mBaseAddress + elf_dyn.d_un.d_ptr;
        switch (elf_dyn.d_tag) {
            case DT_HASH: {
                if (!ValidPointer(d_ptr)) {
                    String path;
                    mFile->GetPath(&path);
                    String msg("");
                    msg.AppendFormat("DT_HASH value %p does not refer to a loaded ELF segment of %s",
                                            d_ptr, path.string());
                    *error_msg = msg;
                    return FALSE;
                }
                mHashSectionStart = reinterpret_cast<Elf32_Word*>(d_ptr);
                break;
            }
            case DT_STRTAB: {
                if (!ValidPointer(d_ptr)) {
                    String path;
                    mFile->GetPath(&path);
                    String msg("");
                    msg.AppendFormat("DT_HASH value %p does not refer to a loaded ELF segment of %s",
                                            d_ptr, path.string());
                    *error_msg = msg;
                    return FALSE;
                }
                mDynstrSectionStart = reinterpret_cast<char*>(d_ptr);
                break;
            }
            case DT_SYMTAB: {
                if (!ValidPointer(d_ptr)) {
                    String path;
                    mFile->GetPath(&path);
                    String msg("");
                    msg.AppendFormat("DT_HASH value %p does not refer to a loaded ELF segment of %s",
                                          d_ptr, path.string());
                    *error_msg = msg;
                    return FALSE;
                }
                mDynsymSectionStart = reinterpret_cast<Elf32_Sym*>(d_ptr);
                break;
            }
            case DT_NULL: {
                if (GetDynamicNum() != i+1) {
                    String path;
                    mFile->GetPath(&path);
                    String msg("");
                    msg.AppendFormat("DT_NULL found after %d .dynamic entries, "
                                            "expected %d as implied by size of PT_DYNAMIC segment in %s",
                                            i + 1, GetDynamicNum(), path.string());
                    *error_msg = msg;
                    return FALSE;
                }
                break;
            }
        }
    }

    // Check for the existence of some sections.
    if (!CheckSectionsExist(error_msg)) {
        return FALSE;
    }

    // TODO:
    // Use GDB JIT support to do stack backtrace, etc.
    // if (executable) {
    //     GdbJITSupport();
    // }

    return TRUE;
}

Boolean ElfFile::ValidPointer(
    /* [in] */ const Byte* start) const
{
    for (size_t i = 0; i < mSegments.GetSize(); ++i) {
        const AutoPtr<MemMap> segment = mSegments[i];
        if (segment->Begin() <= start && start < segment->End()) {
            return TRUE;
        }
    }
    return FALSE;
}

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
