
#ifndef __ELASTOS_DROID_SERVER_PM_DEX_OATFILE_H__
#define __ELASTOS_DROID_SERVER_PM_DEX_OATFILE_H__

#include <elastos/core/Object.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

class OatFile : public Object
{
public:
    class OatDexFile : public Object
    {
    public:
        // Returns original path of DexFile that was the source of this OatDexFile.
        CARAPI_(String) GetDexFileLocation() const;

        // Returns checksum of original DexFile that was the source of this OatDexFile;
        CARAPI_(uint32_t) GetDexFileLocationChecksum() const;
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

    // const CARAPI_(OatHeader&) GetOatHeader() const;

    CARAPI_(AutoPtr<OatDexFile>) GetOatDexFile(
        /* [in] */ const char* dex_location,
        /* [in] */ const uint32_t* const dex_location_checksum,
        /* [in] */ Boolean exception_if_not_found = TRUE) const;

    CARAPI_(Vector< AutoPtr<OatDexFile> >&) GetOatDexFiles() const;

    CARAPI_(Byte*) Begin() const;

    CARAPI_(Byte*) End() const;

private:
    static CARAPI_(void) CheckLocation(
        /* [in] */ const String& location);

    static CARAPI_(AutoPtr<OatFile>) OpenDlopen(
        /* [in] */ const String& elf_filename,
        /* [in] */ const String& location,
        /* [in] */ Byte* requested_base,
        /* [out] */ String* error_msg);

    explicit OatFile(
        /* [in] */ const String& filename,
        /* [in] */ Boolean executable);

    CARAPI_(Boolean) Dlopen(
        /* [in] */ const String& elf_filename,
        /* [in] */ Byte* requested_base,
        /* [out] */ String* error_msg);

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

    // dlopen handle during runtime.
    void* mDlopenHandle;

    // Owning storage for the OatDexFile objects.
    Vector< AutoPtr<OatDexFile> > mOatDexFilesStorage;

    // Lock guarding all members needed for secondary lookup in GetOatDexFile().
    // mutable Mutex secondary_lookup_lock_ DEFAULT_MUTEX_ACQUIRED_AFTER;
};

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_DEX_OATFILE_H__
