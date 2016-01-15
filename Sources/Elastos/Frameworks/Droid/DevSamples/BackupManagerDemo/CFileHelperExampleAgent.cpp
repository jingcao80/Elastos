
#include "CFileHelperExampleAgent.h"
#include <elastos/utility/logging/Slogger.h>
#include "CActivityOne.h"

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::App::Backup::IFileBackupHelper;
using Elastos::Droid::App::Backup::CFileBackupHelper;
using Elastos::Droid::App::Backup::IBackupHelper;


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BackupManagerDemo {

const String CFileHelperExampleAgent::TAG("CFileHelperExampleAgent");
const String CFileHelperExampleAgent::FILE_HELPER_KEY("the_file");

ECode CFileHelperExampleAgent::OnCreate()
{
    AutoPtr<IFileBackupHelper> helper;
    AutoPtr<ArrayOf<String> > files = ArrayOf<String>::Alloc(1);
    (*files)[0] = CActivityOne::DATA_FILE_NAME;
    CFileBackupHelper::New(this, files, (IFileBackupHelper**)&helper);
    AddHelper(FILE_HELPER_KEY, IBackupHelper::Probe(helper));

    return NOERROR;
}

ECode CFileHelperExampleAgent::OnBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{
    Slogger::D(TAG, "OnBackup");
    {
        AutoLock lock(CActivityOne::sDatalock);
        BackupAgentHelper::OnBackup(oldState, data, newState);
    }
    return NOERROR;
}

ECode CFileHelperExampleAgent::OnRestore(
    /* [in] */ IBackupDataInput* data,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState)
{
    Slogger::D(TAG, "OnRestore");
    {
        AutoLock lock(CActivityOne::sDatalock);
        BackupAgentHelper::OnRestore(data, appVersionCode, newState);
    }
    return NOERROR;
}

}
}
}
}
