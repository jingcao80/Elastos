
#include "elastos/droid/server/pm/dex/ElfFile.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

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
    if (program_header_only_) {
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
        if (!CheckAndSet(GetHeader().e_phoff, "program headers", &program_headers_start_, error_msg)) {
          return false;
        }

        // Setup section headers.
        if (!CheckAndSet(GetHeader().e_shoff, "section headers", &section_headers_start_, error_msg)) {
          return false;
        }

        // Find shstrtab.
        Elf32_Shdr* shstrtab_section_header = GetSectionNameStringSection();
        if (shstrtab_section_header == nullptr) {
          *error_msg = StringPrintf("Failed to find shstrtab section header in ELF file: '%s'",
                                    file_->GetPath().c_str());
          return false;
        }

        // Find .dynamic section info from program header
        dynamic_program_header_ = FindProgamHeaderByType(PT_DYNAMIC);
        if (dynamic_program_header_ == nullptr) {
          *error_msg = StringPrintf("Failed to find PT_DYNAMIC program header in ELF file: '%s'",
                                    file_->GetPath().c_str());
          return false;
        }

        if (!CheckAndSet(GetDynamicProgramHeader().p_offset, "dynamic section",
                         reinterpret_cast<byte**>(&dynamic_section_start_), error_msg)) {
          return false;
        }

        // Find other sections from section headers
        for (Elf32_Word i = 0; i < GetSectionHeaderNum(); i++) {
          Elf32_Shdr* section_header = GetSectionHeader(i);
          if (section_header == nullptr) {
            *error_msg = StringPrintf("Failed to find section header for section %d in ELF file: '%s'",
                                      i, file_->GetPath().c_str());
            return false;
          }
          switch (section_header->sh_type) {
            case SHT_SYMTAB: {
              if (!CheckAndSet(section_header->sh_offset, "symtab",
                               reinterpret_cast<byte**>(&symtab_section_start_), error_msg)) {
                return false;
              }
              break;
            }
            case SHT_DYNSYM: {
              if (!CheckAndSet(section_header->sh_offset, "dynsym",
                               reinterpret_cast<byte**>(&dynsym_section_start_), error_msg)) {
                return false;
              }
              break;
            }
            case SHT_STRTAB: {
              // TODO: base these off of sh_link from .symtab and .dynsym above
              if ((section_header->sh_flags & SHF_ALLOC) != 0) {
                // Check that this is named ".dynstr" and ignore otherwise.
                const char* header_name = GetString(*shstrtab_section_header, section_header->sh_name);
                if (strncmp(".dynstr", header_name, 8) == 0) {
                  if (!CheckAndSet(section_header->sh_offset, "dynstr",
                                   reinterpret_cast<byte**>(&dynstr_section_start_), error_msg)) {
                    return false;
                  }
                }
              } else {
                // Check that this is named ".strtab" and ignore otherwise.
                const char* header_name = GetString(*shstrtab_section_header, section_header->sh_name);
                if (strncmp(".strtab", header_name, 8) == 0) {
                  if (!CheckAndSet(section_header->sh_offset, "strtab",
                                   reinterpret_cast<byte**>(&strtab_section_start_), error_msg)) {
                    return false;
                  }
                }
              }
              break;
            }
            case SHT_DYNAMIC: {
              if (reinterpret_cast<byte*>(dynamic_section_start_) !=
                  Begin() + section_header->sh_offset) {
                LOG(WARNING) << "Failed to find matching SHT_DYNAMIC for PT_DYNAMIC in "
                             << file_->GetPath() << ": " << std::hex
                             << reinterpret_cast<void*>(dynamic_section_start_)
                             << " != " << reinterpret_cast<void*>(Begin() + section_header->sh_offset);
                return false;
              }
              break;
            }
            case SHT_HASH: {
              if (!CheckAndSet(section_header->sh_offset, "hash section",
                               reinterpret_cast<byte**>(&hash_section_start_), error_msg)) {
                return false;
              }
              break;
            }
          }
        }

        // Check for the existence of some sections.
        if (!CheckSectionsExist(error_msg)) {
          return false;
        }
    }

    return true;
}

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
