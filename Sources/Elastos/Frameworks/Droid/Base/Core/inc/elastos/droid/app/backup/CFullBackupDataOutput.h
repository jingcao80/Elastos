#ifndef __ELASTOS_DROID_APP_BACKUP_CFULLBACKUPDATAOUTPUT_H__
#define __ELASTOS_DROID_APP_BACKUP_CFULLBACKUPDATAOUTPUT_H__

#include "_Elastos_Droid_App_Backup_CFullBackupDataOutput.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CFullBackupDataOutput)
    , public Object
    , public IFullBackupDataOutput
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IParcelFileDescriptor* fd);

    CARAPI GetData(
        /* [out] */ IBackupDataOutput** data);

private:
    // Currently a name-scoping shim around BackupDataOutput
    AutoPtr<IBackupDataOutput> mData;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_BACKUP_CFULLBACKUPDATAOUTPUT_H__
