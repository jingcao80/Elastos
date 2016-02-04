
#ifndef __ELASTOS_DROID_SERVER_PERSISTENT_DATA_BLOCK_SERVICE_H__
#define __ELASTOS_DROID_SERVER_PERSISTENT_DATA_BLOCK_SERVICE_H__

#include "elastos/droid/server/SystemService.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Service.h>

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Service::Persistentdata::IIPersistentDataBlockService;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Service for reading and writing blocks to a persistent partition.
 * This data will live across factory resets not initiated via the Settings UI.
 * When a device is factory reset through Settings this data is wiped.
 *
 * Allows writing one block at a time. Namely, each time
 * {@link android.service.persistentdata.IPersistentDataBlockService}.Write(Byte[] data)
 * is called, it will overwite the data that was previously written on the block.
 *
 * Clients can query the size of the currently written block via
 * {@link android.service.persistentdata.IPersistentDataBlockService}.GetTotalDataSize().
 *
 * Clients can any number of bytes from the currently written block up to its total size by invoking
 * {@link android.service.persistentdata.IPersistentDataBlockService}.Read(Byte[] data)
 */
class PersistentDataBlockService
    : public SystemService
{
public:

    class BinderService
        : public Object
        , public IIPersistentDataBlockService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ISystemService* service);

        //@Override
        CARAPI Write(
            /* [in] */ ArrayOf<Byte>* data,
            /* [out] */ Int32* result);

        //@Override
        CARAPI Read(
            /* [out, callee] */ ArrayOf<Byte>** data);

        //@Override
        CARAPI Wipe();

        //@Override
        CARAPI SetOemUnlockEnabled(
            /* [in] */ Boolean enabled);

        //@Override
        CARAPI GetOemUnlockEnabled(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI GetDataBlockSize(
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetMaximumDataBlockSize(
            /* [out] */ Int64* result);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        PersistentDataBlockService* mHost;
    };

public:
    PersistentDataBlockService();

    ~PersistentDataBlockService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnStart();

private:
    Int32 GetAllowedAppId(
        /* [in] */ Int32 userHandle);

    CARAPI EnforceOemUnlockPermission();

    CARAPI EnforceUid(
        /* [in] */ Int32 callingUid);

    CARAPI GetTotalDataSizeLocked(
        /* [in] */ IDataInputStream* inputStream,
        /* [out] */ Int32* result);

    Int64 GetBlockDeviceSize();

    Int64 NativeGetBlockDeviceSize(
        /* [in] */ const String& path);

    Int32 NativeWipe(
        /* [in] */ const String& path);

private:
    static const String TAG;

    static const String PERSISTENT_DATA_BLOCK_PROP;
    static const Int32 HEADER_SIZE;
    // Magic number to mark block device as adhering to the format consumed by this service
    static const Int32 PARTITION_TYPE_MARKER;
    // Limit to 100k as blocks larger than this might cause strain on Binder.
    // TODO(anmorales): Consider splitting up too-large blocks in PersistentDataBlockManager
    static const Int32 MAX_DATA_BLOCK_SIZE;

    AutoPtr<IContext> mContext;
    String mDataBlockFile;
    Object mLock;

    Int32 mAllowedAppId;
    /*
     * Separate lock for OEM unlock related operations as they can happen in parallel with regular
     * block operations.
     */
    Object mOemLock;

    Int64 mBlockDeviceSize;

    AutoPtr<IBinder> mService;
};

}// Server
}// Droid
}// Elastos

#endif //__ELASTOS_DROID_SERVER_PERSISTENT_DATA_BLOCK_SERVICE_H__
