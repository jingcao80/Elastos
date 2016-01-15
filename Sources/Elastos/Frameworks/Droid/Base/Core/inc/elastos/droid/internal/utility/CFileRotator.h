
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CFILEROTATOR_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CFILEROTATOR_H__

#include <Elastos.CoreLibrary.IO.h>
#include "_Elastos_Droid_Internal_Utility_CFileRotator.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CFileRotator)
    , public Object
    , public IFileRotator
{
private:
    class FileInfo : public Object
    {
    public:
        FileInfo(
            /* [in] */ const String& prefix);

        /**
         * Attempt parsing the given filename.
         *
         * @return Whether parsing was successful.
         */
        CARAPI_(Boolean) Parse(
            /* [in] */ const String& name);

        /**
         * Build current state into filename.
         */
        CARAPI_(String) Build();

        /**
         * Test if current file is active (no end timestamp).
         */
        CARAPI_(Boolean) IsActive();

    public:
        String mPrefix;

        Int64 mStartMillis;
        Int64 mEndMillis;
    };

    class CombineActiveRewriter
        : public Object
        , public IFileRotatorReader
        , public IFileRotatorWriter
        , public IFileRotatorRewriter
    {
    public:
        CombineActiveRewriter(
            /* [in] */ IFileRotatorReader* reader,
            /* [in] */ IFileRotatorWriter* writer);

        CAR_INTERFACE_DECL()

        CARAPI Reset();

        CARAPI Read(
            /* [in] */ IInputStream* in);

        CARAPI ShouldWrite(
            /* [out] */ Boolean* result);

        CARAPI Write(
            /* [in] */ IOutputStream* out);

    private:
        AutoPtr<IFileRotatorReader> mReader;
        AutoPtr<IFileRotatorWriter> mWriter;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Create a file rotator.
     *
     * @param basePath Directory under which all files will be placed.
     * @param prefix Filename prefix used to identify this rotator.
     * @param rotateAgeMillis Age in milliseconds beyond which an active file
     *            may be rotated into a historical file.
     * @param deleteAgeMillis Age in milliseconds beyond which a rotated file
     *            may be deleted.
     */
    CARAPI constructor(
        /* [in] */ IFile* basePath,
        /* [in] */ const String& prefix,
        /* [in] */ Int64 rotateAgeMillis,
        /* [in] */ Int64 deleteAgeMillis);

    /**
     * Delete all files managed by this rotator.
     */
    CARAPI DeleteAll();

    /**
     * Dump all files managed by this rotator for debugging purposes.
     */
    CARAPI DumpAll(
        /* [in] */ IOutputStream* os);

    /**
     * Process currently active file, first reading any existing data, then
     * writing modified data. Maintains a backup during write, which is restored
     * if the write fails.
     */
    CARAPI RewriteActive(
        /* [in] */ IFileRotatorRewriter* rewriter,
        /* [in] */ Int64 currentTimeMillis);

    CARAPI CombineActive(
        /* [in] */ IFileRotatorReader* reader,
        /* [in] */ IFileRotatorWriter* writer,
        /* [in] */ Int64 currentTimeMillis);

    /**
     * Process all files managed by this rotator, usually to rewrite historical
     * data. Each file is processed atomically.
     */
    CARAPI RewriteAll(
        /* [in] */ IFileRotatorRewriter* rewriter);

    /**
     * Read any rotated data that overlap the requested time range.
     */
    CARAPI ReadMatching(
        /* [in] */ IFileRotatorReader* reader,
        /* [in] */ Int64 matchStartMillis,
        /* [in] */ Int64 matchEndMillis);

    /**
     * Examine all files managed by this rotator, renaming or deleting if their
     * age matches the configured thresholds.
     */
    CARAPI MaybeRotate(
        /* [in] */ Int64 currentTimeMillis);

private:
    /**
     * Process a single file atomically, first reading any existing data, then
     * writing modified data. Maintains a backup during write, which is restored
     * if the write fails.
     */
    CARAPI RewriteSingle(
        /* [in] */ IFileRotatorRewriter* rewriter,
        /* [in] */ const String& name);

    /**
     * Return the currently active file, which may not exist yet.
     */
    CARAPI_(String) GetActiveName(
        /* [in] */ Int64 currentTimeMillis);

    static CARAPI ReadFile(
        /* [in] */ IFile* file,
        /* [in] */ IFileRotatorReader* reader);

    static CARAPI WriteFile(
        /* [in] */ IFile* file,
        /* [in] */ IFileRotatorWriter* writer);

private:
    static const String TAG;
    static const Boolean LOGD;

    AutoPtr<IFile> mBasePath;
    String mPrefix;
    Int64 mRotateAgeMillis;
    Int64 mDeleteAgeMillis;

    static const String SUFFIX_BACKUP;
    static const String SUFFIX_NO_BACKUP;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_UTILITY_CFILEROTATOR_H__
