#ifndef __CFILEHELPEREXAMPLEAGENT_H__
#define __CFILEHELPEREXAMPLEAGENT_H__

#include "elastos/droid/app/backup/BackupAgentHelper.h"
#include "_CFileHelperExampleAgent.h"

using Elastos::Droid::App::Backup::BackupAgentHelper;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BackupManagerDemo {

class CFileHelperExampleAgent
    : public BackupAgentHelper
{
public:
    CARAPI OnCreate();

    CARAPI OnBackup(
     /* [in] */ IParcelFileDescriptor* oldState,
     /* [in] */ IBackupDataOutput* data,
     /* [in] */ IParcelFileDescriptor* newState);

    CARAPI OnRestore(
     /* [in] */ IBackupDataInput* data,
     /* [in] */ Int32 appVersionCode,
     /* [in] */ IParcelFileDescriptor* newState);

private:
    static const String TAG;
    static const String FILE_HELPER_KEY;
};
}
}
}
}

#endif
