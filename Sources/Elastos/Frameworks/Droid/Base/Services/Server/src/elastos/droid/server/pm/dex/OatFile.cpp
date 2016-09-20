
#include "elastos/droid/server/pm/dex/OatFile.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

void OatFile::CheckLocation(
    /* [in] */ const String& location)
{
    if (location.IsNullOrEmpty()) {
        Slogger::D("OatFile", "Check failed: location is null or empty.");
    }
}

AutoPtr<OatFile> OatFile::Open(
    /* [in] */ const String& filename,
    /* [in] */ const String& location,
    /* [in] */ byte* requested_base,
    /* [in] */ Boolean executable,
    /* [in] */ const String& error_msg)
{
    CheckLocation(filename);
    AutoPtr<OatFile> ret;
    if (kUsePortableCompiler && executable) {
      // If we are using PORTABLE, use dlopen to deal with relocations.
      //
      // We use our own ELF loader for Quick to deal with legacy apps that
      // open a generated dex file by name, remove the file, then open
      // another generated dex file with the same name. http://b/10614658
      ret = OpenDlopen(filename, location, requested_base, error_msg);
    }
    else {
      // If we aren't trying to execute, we just use our own ElfFile loader for a couple reasons:
      //
      // On target, dlopen may fail when compiling due to selinux restrictions on installd.
      //
      // On host, dlopen is expected to fail when cross compiling, so fall back to OpenElfFile.
      // This won't work for portable runtime execution because it doesn't process relocations.
      std::unique_ptr<File> file(OS::OpenFileForReading(filename.c_str()));
      if (file.get() == NULL) {
        *error_msg = StringPrintf("Failed to open oat filename for reading: %s", strerror(errno));
        return nullptr;
      }
      ret.reset(OpenElfFile(file.get(), location, requested_base, false, executable, error_msg));
      // It would be nice to unlink here. But we might have opened the file created by the
      // ScopedLock, which we better not delete to avoid races. TODO: Investigate how to fix the API
      // to allow removal when we know the ELF must be borked.
    }
    return ret;
}

AutoPtr<OatFile> OatFile::OpenDlopen(
    /* [in] */ const String& elf_filename,
    /* [in] */ const String& location,
    /* [in] */ byte* requested_base,
    /* [out] */ String* error_msg)
{
    AutoPtr<OatFile> oat_file = new OatFile(location, TRUE);
    Boolean success = oat_file->Dlopen(elf_filename, requested_base, error_msg);
    if (!success) {
        return NULL;
    }
    return oat_file;
}

OatFile::OatFile(
    /* [in] */ const String& location,
    /* [in] */ Boolean is_executable)
    : mLocation(location)
    , mBegin(NULL)
    , mEnd(NULL)
    , mIsExecutable(is_executable)
    , mDlopenHandle(NULL)
    , secondary_lookup_lock_("OatFile secondary lookup lock", kOatFileSecondaryLookupLock)
{
    CheckLocation(mLocation);
}

Boolean OatFile::Dlopen(
    /* [in] */ const String& elf_filename,
    /* [in] */ Byte* requested_base,
    /* [in] */ String* error_msg)
{
    char* absolute_path = realpath(elf_filename.string(), NULL);
    if (absolute_path == NULL) {
        StringBuilder sb(512);
        sb += "Failed to find absolute path for '";
        sb += elf_filename;
        sb += "'";
        *error_msg = sb.ToString();
        return FALSE;
    }
    mDlopenHandle = dlopen(absolute_path, RTLD_NOW);
    free(absolute_path);
    if (mDlopenHandle == NULL) {
        StringBuilder sb(512);
        sb += "Failed to dlopen '";
        sb += elf_filename;
        sb += "': ";
        sb += dlerror();
        *error_msg = sb.ToString();
        return FALSE;
    }
    mBegin = reinterpret_cast<Byte*>(dlsym(mDlopenHandle, "oatdata"));
    if (mBegin == NULL) {
        StringBuilder sb(512);
        sb += "Failed to find oatdata symbol in '";
        sb += elf_filename;
        sb += "': ";
        sb += dlerror();
        *error_msg = sb.ToString();
        return FALSE;
    }
    if (requested_base != NULL && mBegin != requested_base) {
        StringBuilder sb(512);
        sb += "Failed to find oatdata symbol at expected address: oatdata=";
        sb += mBegin;
        sb += " != expected=";
        sb += requested_base;
        sb += " /proc/self/maps:\n"
        *error_msg = sb.ToString();
        ReadFileToString("/proc/self/maps", error_msg);
        return FALSE;
    }
    mEnd = reinterpret_cast<byte*>(dlsym(mDlopenHandle, "oatlastword"));
    if (mEnd == NULL) {
        StringBuilder sb(512);
        sb += "Failed to find oatlastword symbol in '";
        sb += elf_filename;
        sb += "': ";
        sb += dlerror();
        *error_msg = sb.ToString();
        return FALSE;
    }
    // Readjust to be non-inclusive upper bound.
    mEnd += sizeof(uint32_t);
    return Setup(error_msg);
}

Boolean OatFile::Setup(
    /* [out] */ String* error_msg)
{
    if (!GetOatHeader().IsValid()) {
        StringBuilder sb(512);
        sb += "Invalid oat magic for '";
        sb += GetLocation();
        sb += "'";
        *error_msg = sb.ToString();
        return FALSE;
    }
    const Byte* oat = Begin();
    oat += sizeof(OatHeader);
    if (oat > End()) {
        StringBuilder sb(512);
        sb += "In oat file '";
        sb += GetLocation();
        sb += "' found truncated OatHeader";
        *error_msg = sb.ToString();
        return FALSE;
    }

    oat += GetOatHeader().GetKeyValueStoreSize();
    if (oat > End()) {
        String msg("");
        msg.AppendFormat("In oat file '%s' found truncated variable-size data: "
                "%p + %zd + %ud <= %p", GetLocation().string(),
                Begin(), sizeof(OatHeader), GetOatHeader().GetKeyValueStoreSize(),
                End());
        *error_msg = msg;
        return FALSE;
    }

    uint32_t dexFileCount = GetOatHeader().GetDexFileCount();
    mOatDexFilesStorage.Reserve(dexFileCount);
    for (size_t i = 0; i < dexFileCount; i++) {
        uint32_t dexFileLocationSize = *reinterpret_cast<const uint32_t*>(oat);
        if (dexFileLocationSize == 0U) {
            String msg("");
            msg.AppendFormat("In oat file '%s' found OatDexFile #%zd with empty location name",
                    GetLocation().string(), i);
            *error_msg = msg;
            return FALSE;
        }
        oat += sizeof(dexFileLocationSize);
        if (oat > End()) {
            String msg("");
            msg.AppendFormat("In oat file '%s' found OatDexFile #%zd truncated after dex file "
                    "location size", GetLocation().string(), i);
            *error_msg = msg;
            return FALSE;
        }

        const char* dexFileLocationData = reinterpret_cast<const char*>(oat);
        oat += dexFileLocationSize;
        if (oat > End()) {
            String msg("");
            msg.AppendFormat("In oat file '%s' found OatDexFile #%zd with truncated dex file "
                    "location", GetLocation().string(), i);
            *error_msg = msg;
            return FALSE;
        }

        String dexFileLocation(dexFileLocationData, dexFileLocationSize);

        uint32_t dexFileChecksum = *reinterpret_cast<const uint32_t*>(oat);
        oat += sizeof(dexFileChecksum);
        if (oat > End()) {
            String msg("");
            msg.AppendFormat("In oat file '%s' found OatDexFile #%zd for '%s' truncated after "
                    "dex file checksum", GetLocation().string(), i,
                    dexFileLocation.string());
            *error_msg = msg;
            return FALSE;
        }

        uint32_t dexFileOffset = *reinterpret_cast<const uint32_t*>(oat);
        if (dexFileOffset == 0U) {
            String msg("");
            msg.AppendFormat("In oat file '%s' found OatDexFile #%zd for '%s' with zero dex "
                    "file offset", GetLocation().string(), i, dexFileLocation.string());
            *error_msg = msg;
            return FALSE;
        }
        if (dexFileOffset > Size()) {
            String msg("");
            msg.AppendFormat("In oat file '%s' found OatDexFile #%zd for '%s' with dex file "
                    "offset %ud > %zd", GetLocation().string(), i,
                    dexFileLocation.string(), dexFileOffset, Size());
            *error_msg = msg;
            return FALSE;
        }
        oat += sizeof(dexFileOffset);
        if (oat > End()) {
            String msg("");
            msg.AppendFormat("In oat file '%s' found OatDexFile #%zd for '%s' truncated "
                    " after dex file offsets", GetLocation().string(), i,
                    dexFileLocation.string());
            *error_msg = msg;
            return FALSE;
        }

        const uint8_t* dexFilePointer = Begin() + dexFileOffset;
        if (!DexFile::IsMagicValid(dexFilePointer)) {
            String msg("");
            msg.AppendFormat("In oat file '%s' found OatDexFile #%zd for '%s' with invalid "
                    " dex file magic '%s'", GetLocation().string(), i,
                    dexFileLocation.string(), dexFilePointer);
            *error_msg = msg;
            return FALSE;
        }
        if (!DexFile::IsVersionValid(dexFilePointer)) {
            String msg("");
            msg.AppendFormat("In oat file '%s' found OatDexFile #%zd for '%s' with invalid "
                    " dex file version '%s'", GetLocation().string(), i,
                    dexFileLocation.string(), dexFilePointer);
            *error_msg = msg;
            return FALSE;
        }
        const DexFile::Header* header = reinterpret_cast<const DexFile::Header*>(dexFilePointer);
        const uint32_t* methodsOffsetsPointer = reinterpret_cast<const uint32_t*>(oat);

        oat += (sizeof(*methodsOffsetsPointer) * header->mClassDefsSize);
        if (oat > End()) {
            String msg("");
            msg.AppendFormat("In oat file '%s' found OatDexFile #%zd for '%s' with truncated "
                    " method offsets", GetLocation().string(), i,
                    dexFileLocation.string());
            *error_msg = msg;
            return FALSE;
        }

        String canonicalLocation = DexFile::GetDexCanonicalLocation(dexFileLocation.string());

        // Create the OatDexFile and add it to the owning container.
        OatDexFile* oat_dex_file = new OatDexFile(this,
                                                  dex_file_location,
                                                  canonical_location,
                                                  dex_file_checksum,
                                                  dex_file_pointer,
                                                  methods_offsets_pointer);
        oat_dex_files_storage_.push_back(oat_dex_file);

        // Add the location and canonical location (if different) to the oat_dex_files_ table.
        StringPiece key(oat_dex_file->GetDexFileLocation());
        oat_dex_files_.Put(key, oat_dex_file);
        if (canonical_location != dex_file_location) {
          StringPiece canonical_key(oat_dex_file->GetCanonicalDexFileLocation());
          oat_dex_files_.Put(canonical_key, oat_dex_file);
        }
    }
    return TRUE;
}

const OatHeader& OatFile::GetOatHeader() const
{
    return *reinterpret_cast<const OatHeader*>(Begin());
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
