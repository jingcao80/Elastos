
#ifndef __ELASTOS_DROID_SERVICE_PERSISTENTDATA_PERSISTENTDATABLOCKMANAGER_H__
#define __ELASTOS_DROID_SERVICE_PERSISTENTDATA_PERSISTENTDATABLOCKMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Service::Persistentdata::IIPersistentDataBlockService;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Persistentdata {

/**
 * Interface for reading and writing data blocks to a persistent partition.
 *
 * Allows writing one block at a time. Namely, each time
 * {@link PersistentDataBlockManager#write(byte[])}
 * is called, it will overwite the data that was previously written on the block.
 *
 * Clients can query the size of the currently written block via
 * {@link PersistentDataBlockManager#getDataBlockSize()}.
 *
 * Clients can query the maximum size for a block via
 * {@link PersistentDataBlockManager#getMaximumDataBlockSize()}
 *
 * Clients can read the currently written block by invoking
 * {@link PersistentDataBlockManager#read()}.
 *
 * @hide
 */
// @SystemApi
class PersistentDataBlockManager
    : public Object
    , public IPersistentDataBlockManager
{
public:
    CAR_INTERFACE_DECL()

    PersistentDataBlockManager();

    CARAPI constructor(
        /* [in] */ IIPersistentDataBlockService* service);

    /**
     * Writes {@code data} to the persistent partition. Previously written data
     * will be overwritten. This data will persist across factory resets.
     *
     * Returns the number of bytes written or -1 on error. If the block is too big
     * to fit on the partition, returns -MAX_BLOCK_SIZE.
     *
     * @param data the data to write
     */
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ Int32* ret);

    /**
     * Returns the data block stored on the persistent partition.
     */
    CARAPI Read(
        /* [out, callee] */ ArrayOf<Byte>** data);

    /**
     * Retrieves the size of the block currently written to the persistent partition.
     *
     * Return -1 on error.
     */
    CARAPI GetDataBlockSize(
        /* [out] */ Int32* size);

    /**
     * Retrieves the maximum size allowed for a data block.
     *
     * Returns -1 on error.
     */
    CARAPI GetMaximumDataBlockSize(
        /* [out] */ Int64* size);

    /**
     * Zeroes the previously written block in its entirety. Calling this method
     * will erase all data written to the persistent data partition.
     */
    CARAPI Wipe();

    /**
     * Writes a byte enabling or disabling the ability to "OEM unlock" the device.
     */
    CARAPI SetOemUnlockEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Returns whether or not "OEM unlock" is enabled or disabled on this device.
     */
    CARAPI GetOemUnlockEnabled(
        /* [out] */ Boolean* isenabled);

private:
    CARAPI_(void) OnError(
        /* [in] */ const String& msg);

private:
    const static String TAG;//PersistentDataBlockManager.class.getSimpleName();
    AutoPtr<IIPersistentDataBlockService> sService;

};

} // namespace Persistentdata
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_PERSISTENTDATA_PERSISTENTDATABLOCKMANAGER_H__
