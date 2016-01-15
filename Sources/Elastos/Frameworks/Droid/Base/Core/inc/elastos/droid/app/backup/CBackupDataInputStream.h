
#ifndef __ELASTOS_DROID_APP_BACKUP_CBACKUPDATAINPUTSTREAM_H__
#define  __ELASTOS_DROID_APP_BACKUP_CBACKUPDATAINPUTSTREAM_H__

#include "_Elastos_Droid_App_Backup_CBackupDataInputStream.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

/**
 * Provides an {@link java.io.InputStream}-like interface for accessing an
 * entity's data during a restore operation. Used by {@link BackupHelper} classes within the {@link
 * BackupAgentHelper} mechanism.
 * <p>
 * When {@link BackupHelper#restoreEntity(BackupDataInputStream) BackupHelper.restoreEntity()}
 * is called, the current entity's header has already been read from the underlying
 * {@link BackupDataInput}.  The entity's key string and total data size are available
 * through this class's {@link #getKey()} and {@link #size()} methods, respectively.
 * <p class="note">
 * <strong>Note:</strong> The caller should take care not to seek or close the underlying data
 * source, nor read more than {@link #size()} bytes from the stream.</p>
 *
 * @see BackupAgentHelper
 * @see BackupHelper
 */
CarClass(CBackupDataInputStream) // TODO  public InputStream
    , public Object
    , public IBackupDataInputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IBackupDataInput* data);

    /**
     * Read one byte of entity data from the stream, returning it as
     * an integer value.  If more than {@link #size()} bytes of data
     * are read from the stream, the output of this method is undefined.
     *
     * @return The byte read, or undefined if the end of the stream has been reached.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Read up to {@code size} bytes of data into a byte array, beginning at position
     * {@code offset} within the array.
     *
     * @param b Byte array into which the data will be read
     * @param offset The data will be stored in {@code b} beginning at this index
     *   within the array.
     * @param size The number of bytes to read in this operation.  If insufficient
     *   data exists within the entity to fulfill this request, only as much data
     *   will be read as is available.
     * @return The number of bytes of data read, or zero if all of the entity's
     *   data has already been read.
     */
    CARAPI ReadBytes(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number);

    /**
     * Read enough entity data into a byte array to fill the array.
     *
     * @param b Byte array to fill with data from the stream.  If the stream does not
     *   have sufficient data to fill the array, then the contents of the remainder of
     *   the array will be undefined.
     * @return The number of bytes of data read, or zero if all of the entity's
     *   data has already been read.
     */
    CARAPI ReadBytes(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

    CARAPI Close();

    CARAPI Available(
        /* [out] */ Int32* result);

    CARAPI Mark(
        /* [in] */ Int32 value);

    CARAPI IsMarkSupported(
        /* [out] */ Boolean* isSupport);

    CARAPI Reset();

    CARAPI Skip(
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64* num);

    /**
     * Report the key string associated with this entity within the backup data set.
     *
     * @return The key string for this entity, equivalent to calling
     *   {@link BackupDataInput#getKey()} on the underlying {@link BackupDataInput}.
     */
    CARAPI GetKey(
        /* [out] */ String* str);

    /**
     * Report the total number of bytes of data available for the current entity.
     *
     * @return The number of data bytes available, equivalent to calling
     *   {@link BackupDataInput#getDataSize()} on the underlying {@link BackupDataInput}.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI SetKey(
        /* [in] */ const String& str);

    CARAPI SetSize(
        /* [in] */ Int32 size);

    CARAPI GetLock(
        /* [out] */ IInterface** lockobj);

    CARAPI GetData(
        /* [out] */ IBackupDataInput** data);

private:
    String mKey;
    Int32 mDataSize;

    AutoPtr<IBackupDataInput> mData;
    AutoPtr<ArrayOf<Byte> > mOneByte;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_APP_BACKUP_CBACKUPDATAINPUTSTREAM_H__
