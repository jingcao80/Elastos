
#include "elastos/droid/server/pm/DexFile.h"
#include "elastos/droid/server/pm/dex/OatFile.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <dlfcn.h>

using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

Boolean ReadFileToString(const String& file_name, String* result)
{
    FILE* fp = fopen(file_name.string(), "r");
    if (fp == NULL) return FALSE;

    char buf[8192];
    while (TRUE) {
        size_t n = fread(buf, 1, 8192, fp);
        if (n == -1) {
            fclose(fp);
            return FALSE;
        }
        if (n == 0) {
            fclose(fp);
            return TRUE;
        }
        result->Append(String(buf, n));
    }
}


OatFile::OatDexFile::OatDexFile(
    /* [in] */ const OatFile* oat_file,
    /* [in] */ const String& dex_file_location,
    /* [in] */ const String& canonical_dex_file_location,
    /* [in] */ uint32_t dex_file_location_checksum,
    /* [in] */ const Byte* dex_file_pointer,
    /* [in] */ const uint32_t* oat_class_offsets_pointer)
    : mOatFile(oat_file)
    , mDexFileLocation(dex_file_location)
    , mCanonicalDexFileLocation(canonical_dex_file_location)
    , mDexFileLocationChecksum(dex_file_location_checksum)
    , mDexFilePointer(dex_file_pointer)
    , mOatClassOffsetsPointer(oat_class_offsets_pointer)
{}


const Boolean OatFile::sIsDebug = FALSE;

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
    /* [in] */ Byte* requested_base,
    /* [in] */ Boolean executable,
    /* [out] */ String* error_msg)
{
    CheckLocation(filename);
    AutoPtr<OatFile> ret;
    if (sUsePortableCompiler && executable) {
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
        AutoPtr<IFile> file;
        CFile::New(filename, (IFile**)&file);
        Boolean res;
        file->SetReadOnly(&res);
        if (file == NULL) {
            String msg("");
            msg.AppendFormat("Failed to open oat filename for reading: %s", strerror(errno));
            *error_msg = msg;
            return NULL;
        }
        ret = OpenElfFile(file, location, requested_base, FALSE, executable, error_msg);
        // It would be nice to unlink here. But we might have opened the file created by the
        // ScopedLock, which we better not delete to avoid races. TODO: Investigate how to fix the API
        // to allow removal when we know the ELF must be borked.
    }
    return ret;
}

String OatFile::GetLocation() const
{
    return mLocation;
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

AutoPtr<OatFile> OatFile::OpenElfFile(
    /* [in] */ IFile* file,
    /* [in] */ const String& location,
    /* [in] */ Byte* requested_base,
    /* [in] */ Boolean writable,
    /* [in] */ Boolean executable,
    /* [in] */ String* error_msg)
{
    AutoPtr<OatFile> oat_file = new OatFile(location, executable);
    Boolean success = oat_file->ElfFileOpen(file, requested_base, writable, executable, error_msg);
    if (!success) {
        assert(!error_msg->IsEmpty());
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
    // , secondary_lookup_lock_("OatFile secondary lookup lock", kOatFileSecondaryLookupLock)
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
        String msg("");
        msg.AppendFormat("Failed to find absolute path for '%s'", elf_filename.string());
        *error_msg = msg;
        return FALSE;
    }
    mDlopenHandle = dlopen(absolute_path, RTLD_NOW);
    free(absolute_path);
    if (mDlopenHandle == NULL) {
        String msg("");
        msg.AppendFormat("Failed to dlopen '%s': %s", elf_filename.string(), dlerror());
        *error_msg = msg;
        return FALSE;
    }
    mBegin = reinterpret_cast<Byte*>(dlsym(mDlopenHandle, "oatdata"));
    if (mBegin == NULL) {
        String msg("");
        msg.AppendFormat("Failed to find oatdata symbol in '%s': %s", elf_filename.string(),
                dlerror());
        *error_msg = msg;
        return FALSE;
    }
    if (requested_base != NULL && mBegin != requested_base) {
        String msg("");
        msg.AppendFormat("Failed to find oatdata symbol at expected address: "
                "oatdata=%p != expected=%p /proc/self/maps:\n",
                mBegin, requested_base);
        *error_msg = msg;
        ReadFileToString(String("/proc/self/maps"), error_msg);
        return FALSE;
    }
    mEnd = reinterpret_cast<byte*>(dlsym(mDlopenHandle, "oatlastword"));
    if (mEnd == NULL) {
        String msg("");
        msg.AppendFormat("Failed to find oatlastword symbol in '%s': %s", elf_filename.string(),
                dlerror());
        *error_msg = msg;
        return FALSE;
    }
    // Readjust to be non-inclusive upper bound.
    mEnd += sizeof(uint32_t);
    return Setup(error_msg);
}

Boolean OatFile::ElfFileOpen(
    /* [in] */ IFile* file,
    /* [in] */ Byte* requested_base,
    /* [in] */ Boolean writable,
    /* [in] */ Boolean executable,
    /* [in] */ String* error_msg)
{
    mElfFile = ElfFile::Open(file, writable, TRUE, error_msg);
    if (mElfFile == NULL) {
        assert(!error_msg->IsEmpty());
        return FALSE;
    }
    Boolean loaded = mElfFile->Load(executable, error_msg);
    if (!loaded) {
        assert(!error_msg->IsEmpty());
        return FALSE;
    }
    mBegin = mElfFile->FindDynamicSymbolAddress(String("oatdata"));
    if (mBegin == NULL) {
        String path;
        file->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find oatdata symbol in '%s'", path.string());
        *error_msg = msg;
        return FALSE;
    }
    if (requested_base != NULL && mBegin != requested_base) {
        String msg("");
        msg.AppendFormat("Failed to find oatdata symbol at expected address: "
                "oatdata=%p != expected=%p /proc/self/maps:\n",
                mBegin, requested_base);
        *error_msg = msg;
        ReadFileToString(String("/proc/self/maps"), error_msg);
        return FALSE;
    }
    mEnd = mElfFile->FindDynamicSymbolAddress(String("oatlastword"));
    if (mEnd == NULL) {
        String path;
        file->GetPath(&path);
        String msg("");
        msg.AppendFormat("Failed to find oatlastword symbol in '%s'", path.string());
        *error_msg = msg;
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
        AutoPtr<OatDexFile> oatDexFile = new OatDexFile(this,
                                                  dexFileLocation,
                                                  canonicalLocation,
                                                  dexFileChecksum,
                                                  dexFilePointer,
                                                  methodsOffsetsPointer);
        mOatDexFilesStorage.PushBack(oatDexFile);

        // Add the location and canonical location (if different) to the oat_dex_files_ table.
        mOatDexFiles[oatDexFile->GetDexFileLocation()] = oatDexFile;
        if (!canonicalLocation.Equals(dexFileLocation)) {
            mOatDexFiles[oatDexFile->GetCanonicalDexFileLocation()] = oatDexFile;
        }
    }
    return TRUE;
}

const OatHeader& OatFile::GetOatHeader() const
{
    return *reinterpret_cast<const OatHeader*>(Begin());
}

const Byte* OatFile::Begin() const
{
    assert(mBegin != NULL);
    return mBegin;
}

const Byte* OatFile::End() const
{
    assert(mEnd != NULL);
    return mEnd;
}

AutoPtr<OatFile::OatDexFile> OatFile::GetOatDexFile(
    /* [in] */ const char* dex_location,
    /* [in] */ const uint32_t* const dex_location_checksum,
    /* [in] */ Boolean exception_if_not_found) const
{
    // NOTE: We assume here that the canonical location for a given dex_location never
    // changes. If it does (i.e. some symlink used by the filename changes) we may return
    // an incorrect OatDexFile. As long as we have a checksum to check, we shall return
    // an identical file or fail; otherwise we may see some unpredictable failures.

    // TODO: Additional analysis of usage patterns to see if this can be simplified
    // without any performance loss, for example by not doing the first lock-free lookup.

    AutoPtr<OatDexFile> oat_dex_file;
    String key(dex_location);
    // Try to find the key cheaply in the oat_dex_files_ map which holds dex locations
    // directly mentioned in the oat file and doesn't require locking.
    HashMap<String, AutoPtr<OatDexFile> >::ConstIterator primary_it = mOatDexFiles.Find(key);
    if (primary_it != mOatDexFiles.End()) {
        oat_dex_file = primary_it->mSecond;
        assert(oat_dex_file != NULL);
    }
    else {
        // This dex_location is not one of the dex locations directly mentioned in the
        // oat file. The correct lookup is via the canonical location but first see in
        // the secondary_oat_dex_files_ whether we've looked up this location before.
        AutoLock lock(mSecondaryLookupLock);
        HashMap<String, AutoPtr<OatDexFile> >::Iterator secondary_lb = mSecondaryOatDexFiles.Find(key);
        if (secondary_lb != mSecondaryOatDexFiles.End()) {
            oat_dex_file = secondary_lb->mSecond;  // May be nullptr.
        }
        else {
            // We haven't seen this dex_location before, we must check the canonical location.
            String dex_canonical_location = DexFile::GetDexCanonicalLocation(dex_location);
            if (!dex_canonical_location.Equals(dex_location)) {
                HashMap<String, AutoPtr<OatDexFile> >::ConstIterator canonical_it = mOatDexFiles.Find(dex_canonical_location);
                if (canonical_it != mOatDexFiles.End()) {
                    oat_dex_file = canonical_it->mSecond;
                }  // else keep nullptr.
            }  // else keep nullptr.

            mSecondaryOatDexFiles[key] = oat_dex_file;
        }
    }
    if (oat_dex_file != NULL &&
            (dex_location_checksum == NULL ||
            oat_dex_file->GetDexFileLocationChecksum() == *dex_location_checksum)) {
        return oat_dex_file;
    }

    if (exception_if_not_found) {
        String dex_canonical_location = DexFile::GetDexCanonicalLocation(dex_location);
        String checksum("<unspecified>");
        if (dex_location_checksum != NULL) {
            checksum = "";
            checksum.AppendFormat("0x%08x", *dex_location_checksum);
        }
        Slogger::W("OatFile", "Failed to find OatDexFile for DexFile %s ( canonical path %s) with checksum %s in OatFile %s",
                dex_location, dex_canonical_location.string(), checksum.string(), GetLocation().string());
        if (sIsDebug) {
            Vector< AutoPtr<OatDexFile> >::ConstIterator it;
            for (it = mOatDexFilesStorage.Begin(); it != mOatDexFilesStorage.End(); ++it) {
                OatDexFile* odf = *it;
                Slogger::W("OatFile", "OatFile %s contains OatDexFile %s (canonical path %s) with checksum 0x%08x",
                        GetLocation().string(), odf->GetDexFileLocation().string(),
                        odf->GetCanonicalDexFileLocation().string(), odf->GetDexFileLocationChecksum());
            }
        }
    }

    return NULL;
}

const Vector< AutoPtr<OatFile::OatDexFile> >& OatFile::GetOatDexFiles() const
{
    return mOatDexFilesStorage;
}

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
