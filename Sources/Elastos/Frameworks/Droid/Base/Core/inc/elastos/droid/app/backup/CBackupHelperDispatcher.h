
#ifndef __ELASTOS_DROID_APP_BACKUP_CBACKUPHELPERDISPATCHER_H__
#define  __ELASTOS_DROID_APP_BACKUP_CBACKUPHELPERDISPATCHER_H__

#include "_Elastos_Droid_App_Backup_CBackupHelperDispatcher.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::App::Backup::IBackupDataInput;
using Elastos::Droid::App::Backup::IBackupHelper;
using Elastos::Droid::App::Backup::IBackupDataOutput;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

/** @hide */
CarClass(CBackupHelperDispatcher)
    , public Object
    , public IBackupHelperDispatcher
{
private:
    class Header : public ElRefBase
    {
    public:
        Int32 chunkSize; // not including the header
        String keyPrefix;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI AddHelper(
        /* [in] */ const String& keyPrefix,
        /* [in] */ IBackupHelper* helper);

    CARAPI PerformBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ IParcelFileDescriptor* newState);

    CARAPI PerformRestore(
        /* [in] */ IBackupDataInput* input,
        /* [in] */ Int32 appVersionCode,
        /* [in] */ IParcelFileDescriptor* newState);

private:
    CARAPI DoOneBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ IParcelFileDescriptor* newState,
        /* [in] */ Header* header,
        /* [in] */ IBackupHelper* helper);

    static CARAPI_(Int32) NativeReadHeader(
        /* [in] */ Header* h,
        /* [in] */ IFileDescriptor* fd);

    static CARAPI_(Int32) NativeSkipChunk(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 bytesToSkip);

    static CARAPI_(Int32) NativeAllocateHeader(
        /* [in] */ Header* h,
        /* [in] */ IFileDescriptor* fd);

    static CARAPI_(Int32) NativeWriteHeader(
        /* [in] */ Header* h,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 pos);

private:
    static const String TAG;

    HashMap<String, AutoPtr<IBackupHelper> > mHelpers;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_APP_BACKUP_CBACKUPHELPERDISPATCHER_H__
