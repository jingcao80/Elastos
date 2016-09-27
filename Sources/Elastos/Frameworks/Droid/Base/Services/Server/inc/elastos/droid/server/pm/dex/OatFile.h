
#ifndef __ELASTOS_DROID_SERVER_PM_DEX_OATFILE_H__
#define __ELASTOS_DROID_SERVER_PM_DEX_OATFILE_H__

#include <elastos/droid/server/pm/dex/ElfFile.h>
#include <elastos/droid/server/pm/dex/Oat.h>
#include "Elastos.CoreLibrary.IO.h"
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

class OatHeader;

class OatFile : public Object
{
public:
    class OatDexFile : public Object
    {
    public:
        // Returns original path of DexFile that was the source of this OatDexFile.
        CARAPI_(String) GetDexFileLocation() const
        {
            return mDexFileLocation;
        }

        // Returns the canonical location of DexFile that was the source of this OatDexFile.
        CARAPI_(String) GetCanonicalDexFileLocation() const
        {
            return mCanonicalDexFileLocation;
        }

        // Returns checksum of original DexFile that was the source of this OatDexFile;
        CARAPI_(uint32_t) GetDexFileLocationChecksum() const
        {
            return mDexFileLocationChecksum;
        }

    private:
        OatDexFile(
            /* [in] */ const OatFile* oat_file,
            /* [in] */ const String& dex_file_location,
            /* [in] */ const String& canonical_dex_file_location,
            /* [in] */ uint32_t dex_file_checksum,
            /* [in] */ const Byte* dex_file_pointer,
            /* [in] */ const uint32_t* oat_class_offsets_pointer);

        friend class OatFile;

    private:
        const OatFile* const mOatFile;
        String mDexFileLocation;
        String mCanonicalDexFileLocation;
        const uint32_t mDexFileLocationChecksum;
        const Byte* const mDexFilePointer;
        const uint32_t* const mOatClassOffsetsPointer;
    };

public:
    // Open an oat file. Returns NULL on failure.  Requested base can
    // optionally be used to request where the file should be loaded.
    static CARAPI_(AutoPtr<OatFile>) Open(
        /* [in] */ const String& filename,
        /* [in] */ const String& location,
        /* [in] */ Byte* requested_base,
        /* [in] */ Boolean executable,
        /* [out] */ String* error_msg);

    CARAPI_(String) GetLocation() const;

    CARAPI_(const OatHeader&) GetOatHeader() const;

    CARAPI_(AutoPtr<OatDexFile>) GetOatDexFile(
        /* [in] */ const char* dex_location,
        /* [in] */ const uint32_t* const dex_location_checksum,
        /* [in] */ Boolean exception_if_not_found = TRUE) const;

    CARAPI_(const Vector< AutoPtr<OatDexFile> >&) GetOatDexFiles() const;

    CARAPI_(size_t) Size() const
    {
        return End() - Begin();
    }

    CARAPI_(const Byte*) Begin() const;

    CARAPI_(const Byte*) End() const;

private:
    static CARAPI_(void) CheckLocation(
        /* [in] */ const String& location);

    static CARAPI_(AutoPtr<OatFile>) OpenDlopen(
        /* [in] */ const String& elf_filename,
        /* [in] */ const String& location,
        /* [in] */ Byte* requested_base,
        /* [out] */ String* error_msg);

    static CARAPI_(AutoPtr<OatFile>) OpenElfFile(
        /* [in] */ IFile* file,
        /* [in] */ const String& location,
        /* [in] */ Byte* requested_base,
        /* [in] */ Boolean writable,
        /* [in] */ Boolean executable,
        /* [in] */ String* error_msg);

    explicit OatFile(
        /* [in] */ const String& filename,
        /* [in] */ Boolean executable);

    CARAPI_(Boolean) Dlopen(
        /* [in] */ const String& elf_filename,
        /* [in] */ Byte* requested_base,
        /* [out] */ String* error_msg);

    CARAPI_(Boolean) ElfFileOpen(
        /* [in] */ IFile* file,
        /* [in] */ Byte* requested_base,
        /* [in] */ Boolean writable,
        /* [in] */ Boolean executable,
        /* [in] */ String* error_msg);

    CARAPI_(Boolean) Setup(
        /* [out] */ String* error_msg);

private:
    // The oat file name.
    //
    // The image will embed this to link its associated oat file.
    const String mLocation;

    // Pointer to OatHeader.
    const Byte* mBegin;

    // Pointer to end of oat region for bounds checking.
    const Byte* mEnd;

    // Was this oat_file loaded executable?
    const Boolean mIsExecutable;

    // Backing memory map for oat file during cross compilation.
    AutoPtr<ElfFile> mElfFile;

    // dlopen handle during runtime.
    void* mDlopenHandle;

    // Owning storage for the OatDexFile objects.
    Vector< AutoPtr<OatDexFile> > mOatDexFilesStorage;

    // Map each location and canonical location (if different) retrieved from the
    // oat file to its OatDexFile. This map doesn't change after it's constructed in Setup()
    // and therefore doesn't need any locking and provides the cheapest dex file lookup
    // for GetOatDexFile() for a very frequent use case. Never contains a nullptr value.
    HashMap<String, AutoPtr<OatDexFile> > mOatDexFiles;

    // Lock guarding all members needed for secondary lookup in GetOatDexFile().
    mutable Object mSecondaryLookupLock;

    // If the primary oat_dex_files_ lookup fails, use a secondary map. This map stores
    // the results of all previous secondary lookups, whether successful (non-null) or
    // failed (null). If it doesn't contain an entry we need to calculate the canonical
    // location and use oat_dex_files_by_canonical_location_.
    mutable HashMap<String, AutoPtr<OatDexFile> > mSecondaryOatDexFiles;

    // from globals.h in art
#if defined(ART_USE_PORTABLE_COMPILER)
    static const Boolean sUsePortableCompiler = TRUE;
#else
    static const Boolean sUsePortableCompiler = FALSE;
#endif

    static const Boolean sIsDebug;
};

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_DEX_OATFILE_H__
