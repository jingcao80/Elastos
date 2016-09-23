
#ifndef __ORG_CONSCRYPT_FILECLIENTSESSIONCACHE_H__
#define __ORG_CONSCRYPT_FILECLIENTSESSIONCACHE_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include "elastos/io/File.h"
#include "_Org.Conscrypt.h"

using Elastos::Core::IThrowable;
using Elastos::IO::File;
using Elastos::IO::IFile;
using Elastos::Utility::IMap;
using Elastosx::Net::Ssl::ISSLSession;

namespace Org {
namespace Conscrypt {

class FileClientSessionCache
{
public:
    /**
     * This cache creates one file per SSL session using "host.port" for
     * the file name. Files are created or replaced when session data is put
     * in the cache (see {@link #putSessionData}). Files are read on
     * cache hits, but not on cache misses.
     *
     * <p>When the number of session files exceeds MAX_SIZE, we delete the
     * least-recently-used file. We don't current persist the last access time,
     * so the ordering actually ends up being least-recently-modified in some
     * cases and even just "not accessed in this process" if the filesystem
     * doesn't track last modified times.
     */
    class Impl
        : public Object
        , public ISSLClientSessionCache
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Constructs a new cache backed by the given directory.
         */
        Impl(
            /* [in] */ IFile* directory);

        // @Override
        /*synchronized*/
        CARAPI GetSessionData(
            /* [in] */ const String& host,
            /* [in] */ Int32 port,
            /* [out] */ ArrayOf<Byte>** result);

        static CARAPI LogReadError(
            /* [in] */ const String& host,
            /* [in] */ IFile* file,
            /* [in] */ IThrowable* t);

        // @Override
        /*synchronized*/
        CARAPI PutSessionData(
            /* [in] */ ISSLSession* session,
            /* [in] */ ArrayOf<Byte>* sessionData);

        static CARAPI LogWriteError(
            /* [in] */ const String& host,
            /* [in] */ IFile* file,
            /* [in] */ IThrowable* t);

    private:
        /**
         * Creates a new access-ordered linked hash map.
         */
        static CARAPI_(AutoPtr<IMap/*<String, File>*/>) NewAccessOrder();

        /**
         * Gets the file name for the given host and port.
         */
        static CARAPI FileName(
            /* [in] */ const String& host,
            /* [in] */ Int32 port,
            /* [out] */ String* result);

        /**
         * Deletes old files if necessary.
         */
        CARAPI_(void) MakeRoom();

        /**
         * Lazily updates accessOrder to know about all files as opposed to
         * just the files accessed since this process started.
         */
        CARAPI_(void) IndexFiles();

        // @SuppressWarnings("ThrowableInstanceNeverThrown")
        CARAPI Delete(
            /* [in] */ IFile* file);

    public:
        /** Directory to store session files in. */
        AutoPtr<IFile> mDirectory;

        /**
         * Map of name -> File. Keeps track of the order files were accessed in.
         */
        AutoPtr<IMap/*<String, File>*/> mAccessOrder; // = newAccessOrder();

        /** The number of files on disk. */
        Int32 mSize;

        /**
         * The initial set of files. We use this to defer adding information
         * about all files to accessOrder until necessary.
         */
        AutoPtr<ArrayOf<String> > mInitialFiles;
    };

    /** A file containing a piece of cached data. */
    class CacheFile
        : public File
    {
    public:
        CacheFile(
            /* [in] */ IFile* dir,
            /* [in] */ const String& name);

        // @Override
        CARAPI LastModified(
            /* [out] */ Int64* result);

        // @Override
        CARAPI CompareTo(
            /* [in] */ IFile* another,
            /* [out] */ Int32* result);

    public:
        String mName;

        Int64 mLastModified;
    };

public:
    /**
     * Returns a cache backed by the given directory. Creates the directory
     * (including parent directories) if necessary. This cache should have
     * exclusive access to the given directory.
     *
     * @param directory to store files in
     * @return a cache backed by the given directory
     * @throws IOException if the file exists and is not a directory or if
     *  creating the directories fails
     */
    static CARAPI UsingDirectory(
        /* [in] */ IFile* directory,
        /* [out] */ ISSLClientSessionCache** result);

    /** For testing. */
    static CARAPI Reset();

public:
    /**
     * Maps directories to the cache instances that are backed by those
     * directories. We synchronize access using the cache instance, so it's
     * important that everyone shares the same instance.
     */
    static AutoPtr<IMap/*<File, FileClientSessionCache.Impl>*/> caches; // = new HashMap<File, FileClientSessionCache.Impl>();
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_FILECLIENTSESSIONCACHE_H__
