
#ifndef __ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_CSTORAGEUTILS_H__
#define __ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_CSTORAGEUTILS_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Providers_DownloadProvider_Downloads_CStorageUtils.h"
#include <elastos/core/Object.h>
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::System::IStructStat;
using Elastos::Core::IComparator;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::ICountDownLatch;

namespace Elastos {
namespace Providers {
namespace DownloadProvider {
namespace Downloads {

/**
 * Utility methods for managing storage space related to
 * {@link DownloadManager}.
 */
CarClass(CStorageUtils)
    , public Singleton
    , public IStorageUtils
{
public:
    /**
     * Concrete file on disk that has a backing device and inode. Faster than
     * {@code realpath()} when looking for identical files.
     */
    class ConcreteFile
        : public Object
    {
    public:
        ConcreteFile(
            /* [in] */ IFile* file);

        CARAPI_(Int32) GetHashCode();

        CARAPI_(Boolean) Equals(
            /* [in] */ IInterface* o);

    public:
        AutoPtr<IFile> mFile;
        AutoPtr<IStructStat> mStat;
    };

    class ObserverLatch
        : public Object
        , public IIPackageDataObserver
    {
    public:
        CAR_INTERFACE_DECL()

        ObserverLatch();

        CARAPI OnRemoveCompleted(
            /* [in] */ const String& packageName,
            /* [in] */ Boolean succeeded);

    public:
        AutoPtr<ICountDownLatch> mLatch;
    };

    class MyComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Ensure that requested free space exists on the partition backing the
     * given {@link FileDescriptor}. If not enough space is available, it tries
     * freeing up space as follows:
     * <ul>
     * <li>If backed by the data partition (including emulated external
     * storage), then ask {@link PackageManager} to free space from cache
     * directories.
     * <li>If backed by the cache partition, then try deleting older downloads
     * to free space.
     * </ul>
     */
    CARAPI EnsureAvailableSpace(
        /* [in] */ IContext* context,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 bytes);

    /**
     * Return list of all normal files under the given directory, traversing
     * directories recursively.
     *
     * @param exclude ignore dirs with this name, or {@code null} to ignore.
     * @param uid only return files owned by this UID, or {@code -1} to ignore.
     */
    CARAPI ListFilesRecursive(
        /* [in] */ IFile* startDir,
        /* [in] */ const String& exclude,
        /* [in] */ Int32 uid,
        /* [out] */ IList** result);

private:
    /**
     * Free requested space on cache partition, deleting oldest files first.
     * We're only focused on freeing up disk space, and rely on the next orphan
     * pass to clean up database entries.
     */
    CARAPI_(void) FreeCacheStorage(
        /* [in] */ Int64 bytes);

    /**
     * Return number of available bytes on the filesystem backing the given
     * {@link FileDescriptor}, minus any {@link #RESERVED_BYTES} buffer.
     */
    CARAPI_(Int64) GetAvailableBytes(
        /* [in] */ IFileDescriptor* fd);

    CARAPI_(Int64) GetDeviceId(
        /* [in] */ IFile* file);

public:
    /**
     * Minimum age for a file to be considered for deletion.
     */
    static Int64 MIN_DELETE_AGE;

    /**
     * Reserved disk space to avoid filling disk.
     */
    static Int64 RESERVED_BYTES;

    static Boolean sForceFullEviction;
};

} // namespace Downloads
} // namespace DownloadProvider
} // namespace Providers
} // namespace Elastos

#endif //__ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_CSTORAGEUTILS_H__
