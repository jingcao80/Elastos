#include "elastos/droid/internal/os/WrapperInit.h"
#include "elastos/droid/internal/os/RuntimeInit.h"
#include "elastos/droid/internal/os/CZygoteInit.h"
#include "elastos/droid/internal/os/Zygote.h"
#include "elastos/droid/os/Process.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>

using Elastos::Droid::Os::Process;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::ICloseable;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

static const String TAG("DroidRuntime");

ECode WrapperInit::Main(
    /* [in] */ ArrayOf<String>* args)
{
    // try {
    // Parse our mandatory arguments.
    Int32 fdNum = StringUtils::ParseInt32((*args)[0], 10);
    Int32 targetSdkVersion = StringUtils::ParseInt32((*args)[1], 10);

    // Tell the Zygote what our actual PID is (since it only knows about the
    // wrapper that it directly forked).
    if (fdNum != 0) {
        // try {
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        AutoPtr<IFileDescriptor> fd = CZygoteInit::CreateFileDescriptor(fdNum);
        AutoPtr<IDataOutputStream> os;
        AutoPtr<IOutputStream> fos;
        CFileOutputStream::New(fd, (IOutputStream**)&fos);
        CDataOutputStream::New(fos, (IDataOutputStream**)&os);
        ECode ec = os->WriteInt32(Process::MyPid());
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::D(TAG, "Could not write pid of wrapped process to Zygote pipe.");
        }
        ICloseable::Probe(os)->Close();
        ioUtils->CloseQuietly(fd);
        // } catch (IOException ex) {
        // Slogger::D(TAG, "Could not write pid of wrapped process to Zygote pipe.");
        // }
    }

    // Mimic Zygote preloading.
    CZygoteInit::Preload();

    // Launch the application.
    AutoPtr<ArrayOf<String> > runtimeArgs = ArrayOf<String>::Alloc(args->GetLength() - 2);
    for (Int32 i = 0; i < runtimeArgs->GetLength(); ++i) {
        runtimeArgs->Set(i, (*args)[i + 2]);
    }

    AutoPtr<IRunnable> task;
    ECode ec = RuntimeInit::WrapperInit(targetSdkVersion, runtimeArgs, (IRunnable**)&task);
    FAIL_RETURN(ec)

    return task->Run();
    // } catch (ZygoteInit.MethodAndArgsCaller caller) {
    //     caller.run();
    // }
}

ECode WrapperInit::ExecApplication(
    /* [in] */ const String& invokeWith,
    /* [in] */ const String& niceName,
    /* [in] */ Int32 targetSdkVersion,
    /* [in] */ IFileDescriptor* pipeFd,
    /* [in] */ ArrayOf<String>* args)
{
    StringBuilder command(invokeWith);
    command += " /system/bin/app_process /system/bin --application";
    if (niceName != NULL) {
        command += " '--nice-name=";
        command += niceName;
        command += "'";
    }
    command += " com.android.internal.os.WrapperInit ";
    Int32 id = 0;
    if (pipeFd != NULL) {
        pipeFd->GetInt(&id);
    }
    command += id;
    command += ' ';
    command += targetSdkVersion;
    Zygote::AppendQuotedShellArgs(&command, args);
    return Zygote::ExecShell(command.ToString());
}

ECode WrapperInit::ExecStandalone(
    /* [in] */ const String& invokeWith,
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [in] */ ArrayOf<String>* args)
{
    StringBuilder command(invokeWith);
    command += " /system/bin/dalvikvm -classpath '";
    command += moduleName;
    command += "' ";
    command += className;
    Zygote::AppendQuotedShellArgs(&command, args);
    return Zygote::ExecShell(command.ToString());
}


} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos