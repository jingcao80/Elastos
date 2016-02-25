
#ifndef __ELASTOS_DROID_APP_BACKUP_CBACKUPDATAINPUT_H__
#define __ELASTOS_DROID_APP_BACKUP_CBACKUPDATAINPUT_H__

#include "_Elastos_Droid_App_Backup_CBackupDataInput.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CBackupDataInput)
    , public Object
    , public IBackupDataInput
{
private:
    class EntityHeader : public Object
    {
    public:
        EntityHeader();

    public:
        String mKey;
        Int32 mDataSize;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBackupDataInput();

    ~CBackupDataInput();

    /** @hide */
    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Extract the next entity header from the restore stream.  After this method
     * return success, the {@link #getKey()} and {@link #getDataSize()} methods can
     * be used to inspect the entity that is now available for processing.
     *
     * @return <code>TRUE</code> when there is an entity ready for consumption from the
     *    restore stream, <code>FALSE</code> if the restore stream has been fully consumed.
     * @throws IOException if an error occurred while reading the restore stream
     */
    CARAPI ReadNextHeader(
        /* [out] */ Boolean* next);

    /**
     * Report the key associated with the current entity in the restore stream
     * @return the current entity's key string
     * @throws IllegalStateException if the next record header has not yet been read
     */
    CARAPI GetKey(
        /* [out] */ String* key);

    /**
     * Report the size in bytes of the data associated with the current entity in the
     * restore stream.
     *
     * @return The size of the record's raw data, in bytes
     * @throws IllegalStateException if the next record header has not yet been read
     */
    CARAPI GetDataSize(
        /* [out] */ Int32* size);

    /**
     * Read a record's raw data from the restore stream.  The record's header must first
     * have been processed by the {@link #readNextHeader()} method.  Multiple calls to
     * this method may be made in order to process the data in chunks; not all of it
     * must be read in a single call.  Once all of the raw data for the current entity
     * has been read, further calls to this method will simply return zero.
     *
     * @param data An allocated byte array of at least 'size' bytes
     * @param offset Offset within the 'data' array at which the data will be placed
     *    when read from the stream
     * @param size The number of bytes to read in this pass
     * @return The number of bytes of data read.  Once all of the data for this entity
     *    has been read, further calls to this method will return zero.
     * @throws IOException if an error occurred when trying to read the restore data stream
     */

    CARAPI ReadEntityData(
        /* [in, out] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [out] */ Int32* readSize);

    /**
     * Consume the current entity's data without extracting it into a buffer
     * for further processing.  This allows a {@link android.app.backup.BackupAgent} to
     * efficiently discard obsolete or otherwise uninteresting records during the
     * restore operation.
     *
     * @throws IOException if an error occurred when trying to read the restore data stream
     */
    CARAPI SkipEntityData();

    CARAPI GetBackupReader(
        /* [out] */ Int32* backupReader);

private:
    CARAPI_(Int32) NativeCtor(
        /* [in] */ IFileDescriptor* fd);

    CARAPI_(void) NativeDtor();

    CARAPI_(Int32) NativeReadNextHeader(
        /* [in] */ EntityHeader* entity);

    CARAPI_(Int32) NativeReadEntityData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size);

    CARAPI_(Int32) NativeSkipEntityData();

private:
    static const String TAG;
    static const Boolean DEBUG;
    AutoPtr<EntityHeader> mHeader;
    Boolean mHeaderReady;
    Int32 mBackupReader;

};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_APP_BACKUP_CBACKUPDATAINPUT_H__
