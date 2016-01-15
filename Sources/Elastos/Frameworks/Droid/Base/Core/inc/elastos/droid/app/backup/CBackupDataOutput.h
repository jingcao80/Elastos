
#ifndef __ELASTOS_DROID_APP_BACKUP_CBACKUPDATAOUTPUT_H__
#define  __ELASTOS_DROID_APP_BACKUP_CBACKUPDATAOUTPUT_H__

#include "_Elastos_Droid_App_Backup_CBackupDataOutput.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CBackupDataOutput)
    , public Object
    , public IBackupDataOutput
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBackupDataOutput();

    ~CBackupDataOutput();

    /** @hide */
    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Mark the beginning of one record in the backup data stream. This must be called before
     * {@link #writeEntityData}.
     * @param key A string key that uniquely identifies the data record within the application
     * @param dataSize The size in bytes of this record's data.  Passing a dataSize
     *    of -1 indicates that the record under this key should be deleted.
     * @return The number of bytes written to the backup stream
     * @throws IOException if the write failed
     */
    CARAPI WriteEntityHeader(
        /* [in] */ const String& key,
        /* [in] */ Int32 dataSize,
        /* [out] */ Int32* writtenSize);

    /**
     * Write a chunk of data under the current entity to the backup transport.
     * @param data A raw data buffer to send
     * @param size The number of bytes to be sent in this chunk
     * @return the number of bytes written
     * @throws IOException if the write failed
     */
    CARAPI WriteEntityData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 size,
        /* [out] */ Int32* writtenSize);

    /** @hide */
    CARAPI SetKeyPrefix(
        /* [in] */ const String& keyPrefix);

    CARAPI_(Int32) GetNativeBackupDataWriter();

private:
    CARAPI_(Int32) NativeCtor(
        /* [in] */ IFileDescriptor* fd);

    CARAPI_(void) NativeDtor();

    CARAPI_(Int32) NativeWriteEntityHeader(
        /* [in] */ const String& key,
        /* [in] */ Int32 dataSize);

    CARAPI_(Int32) NativeWriteEntityData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 size);

    CARAPI_(void) NativeSetKeyPrefix(
        /* [in] */ const String& keyPrefix);

private:
    static const String TAG;
    static const Boolean DEBUG;
    Int32 mBackupWriter;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_APP_BACKUP_CBACKUPDATAOUTPUT_H__
