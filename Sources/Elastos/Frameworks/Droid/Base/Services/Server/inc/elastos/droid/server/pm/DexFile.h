#ifndef __ELASTOS_DROID_SERVER_PM_DEXFILE_H__
#define __ELASTOS_DROID_SERVER_PM_DEXFILE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/pm/dex/MemMap.h"
#include "elastos/droid/server/pm/dex/OatFile.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Server::Pm::Dex::MemMap;
using Elastos::Droid::Server::Pm::Dex::OatFile;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class DexFile : public Object
{
public:
    static const size_t sSha1DigestSize = 20;

    // Raw header_item.
    struct Header {
        uint8_t mMagic[8];
        uint32_t mChecksum;  // See also location_checksum_
        uint8_t mSignature[sSha1DigestSize];
        uint32_t mFileSize;  // size of entire file
        uint32_t mHeaderSize;  // offset to start of next section
        uint32_t mEndianTag;
        uint32_t mLinkSize;  // unused
        uint32_t mLinkOff;  // unused
        uint32_t mMapOff;  // unused
        uint32_t mStringIdsSize;  // number of StringIds
        uint32_t mStringIdsOff;  // file offset of StringIds array
        uint32_t mTypeIdsSize;  // number of TypeIds, we don't support more than 65535
        uint32_t mTypeIdsOff;  // file offset of TypeIds array
        uint32_t mProtoIdsSize;  // number of ProtoIds, we don't support more than 65535
        uint32_t mProtoIdsOff;  // file offset of ProtoIds array
        uint32_t mFieldIdsSize;  // number of FieldIds
        uint32_t mFieldIdsOff;  // file offset of FieldIds array
        uint32_t mMethodIdsSize;  // number of MethodIds
        uint32_t mMethodIdsOff;  // file offset of MethodIds array
        uint32_t mClassDefsSize;  // number of ClassDefs
        uint32_t mClassDefsOff;  // file offset of ClassDef array
        uint32_t mDataSize;  // unused
        uint32_t mDataOff;  // unused
    };

    enum InstructionSet {
        kNone,
        kArm,
        kArm64,
        kThumb2,
        kX86,
        kX86_64,
        kMips,
        kMips64
    };

public:
    /**
     * Returns UP_TO_DATE if the VM believes that the apk/jar file
     * is up to date, PATCHOAT_NEEDED if it believes that the file is up
     * to date but it must be relocated to match the base address offset,
     * and DEXOPT_NEEDED if it believes that it is out of date and should
     * be passed through "dexopt" again.
     *
     * @param fileName the absolute path to the apk/jar file to examine.
     * @return DEXOPT_NEEDED if dexopt should be called on the file,
     *         PATCHOAT_NEEDED if we need to run "patchoat" on it and
     *         UP_TO_DATE otherwise.
     * @throws java.io.FileNotFoundException if fileName is not readable,
     *         not a file, or not present.
     * @throws java.io.IOException if fileName is not a valid apk/jar file or
     *         if problems occur while parsing it.
     * @throws java.lang.NullPointerException if fileName is null.
     * @throws dalvik.system.StaleDexCacheError if the optimized dex file
     *         is stale but exists on a read-only partition.
     *
     * @hide
     */
    static CARAPI_(Byte) IsDexOptNeededInternal(
        /* [in] */ const String& fileName,
        /* [in] */ const String& pkgname,
        /* [in] */ const String& instructionSet,
        /* [in] */ Boolean defer);

    // from dex_file.h in art

    // Returns the checksum of a file for comparison with GetLocationChecksum().
    // For .dex files, this is the header checksum.
    // For zip files, this is the classes.dex zip entry CRC32 checksum.
    // Return true if the checksum could be found, false otherwise.
    static CARAPI_(Boolean) GetChecksum(
        /* [in] */ const char* filename,
        /* [in] */ uint32_t* checksum,
        /* [out] */ String* error_msg);

    // For normal dex files, location and base location coincide. If a dex file is part of a multidex
    // archive, the base location is the name of the originating jar/apk, stripped of any internal
    // classes*.dex path.
    static CARAPI_(String) GetBaseLocation(
        /* [in] */ const char* location);

    CARAPI_(Header&) GetHeader();

    // Returns true if the Byte string points to the magic value.
    static CARAPI_(Boolean) IsMagicValid(
        /* [in] */ const Byte* magic);

    CARAPI_(Byte*) Begin() const;

    CARAPI_(size_t) Size() const;

    // from class_linker.h in art

    // Returns true if oat file contains the dex file with the given location and checksum.
    static CARAPI_(Boolean) VerifyOatAndDexFileChecksums(
        /* [in] */ const OatFile* oat_file,
        /* [in] */ const char* dex_location,
        /* [in] */ uint32_t dex_location_checksum,
        /* [in] */ InstructionSet instruction_set,
        /* [out] */ String* error_msg);

    // from utils.h in art
    // Check whether the given magic matches a known file type.
    static CARAPI_(Boolean) IsZipMagic(
        /* [in] */ uint32_t magic);

    static CARAPI_(Boolean) IsDexMagic(
        /* [in] */ uint32_t magic);

private:
    // Opens a .dex file
    static CARAPI_(AutoPtr<DexFile>) OpenFile(
        /* [in] */ Int32 fd,
        /* [in] */ const char* location,
        /* [in] */ Boolean verify,
        /* [out] */ String* error_msg);

    // Opens a .dex file at the given address backed by a MemMap
    static CARAPI_(AutoPtr<DexFile>) OpenMemory(
        /* [in] */ const String& location,
        /* [in] */ uint32_t location_checksum,
        /* [in] */ MemMap* mem_map,
        /* [out] */ String* error_msg);

    // Check whether a location denotes a multidex dex file. This is a very simple check: returns
    // whether the string contains the separator character.
    static CARAPI_(Boolean) IsMultiDexLocation(
        /* [in] */ const char* location);

public:
    /**
     * See {@link #isDexOptNeededInternal(String, String, String, boolean)}.
     *
     * @hide
     */
    static const Byte UP_TO_DATE = 0;

    /**
     * See {@link #isDexOptNeededInternal(String, String, String, boolean)}.
     *
     * @hide
     */
    static const Byte PATCHOAT_NEEDED = 1;

    /**
     * See {@link #isDexOptNeededInternal(String, String, String, boolean)}.
     *
     * @hide
     */
    static const Byte DEXOPT_NEEDED = 2;

    // from dex_file.h in art
    static const Byte sDexMagic[];

    // name of the DexFile entry within a zip archive
    static const char* sClassesDex;

    // The separator charactor in MultiDex locations.
    static const char sMultiDexSeparator;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_DEXFILE_H__
