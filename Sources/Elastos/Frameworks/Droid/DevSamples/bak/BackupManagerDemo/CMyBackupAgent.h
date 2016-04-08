#ifndef __CMYBACKUPAGENT_H__
#define __CMYBACKUPAGENT_H__

#include "elastos/droid/app/backup/BackupAgent.h"
#include "_CMyBackupAgent.h"

using Elastos::Droid::App::Backup::BackupAgent;
using Elastos::Droid::App::Backup::IBackupDataOutput;
using Elastos::Droid::App::Backup::IBackupDataInput;
namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BackupManagerDemo {

class CMyBackupAgent
    : public BackupAgent
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
     CARAPI_(Boolean) CompareStateFile(
         /* [in] */ IParcelFileDescriptor* oldState);

     CARAPI_(void) WriteStateFile(
         /* [in] */ IParcelFileDescriptor* stateFile);

private:
     static const String TAG;
     static const Int32 AGENT_VERSION;
     static const String APP_DATA_KEY;

    /** The app's current data, read from the live disk file */
    Boolean mAddMayo;
    Boolean mAddTomato;
    Int32 mFilling;

    /** The location of the application's persistent data file */
    AutoPtr<IFile> mDataFile;

};
}
}
}
}
#endif
