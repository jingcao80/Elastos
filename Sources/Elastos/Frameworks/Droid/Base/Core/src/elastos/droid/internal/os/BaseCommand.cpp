
#include "elastos/droid/internal/os/BaseCommand.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::IO::IPrintStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CAR_INTERFACE_IMPL(BaseCommand, Object, IBaseCommand)

ECode BaseCommand::Run(
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    if (args == NULL || args->GetLength() < 1) {
        AutoPtr<IPrintStream> out;
        system->GetOut((IPrintStream**)&out);
        OnShowUsage(out);
        return NOERROR;
    }

    mArgs = args;
    mNextArg = 0;
    mCurArgData = NULL;

    ECode ec = OnRun();
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        AutoPtr<IPrintStream> err;
        system->GetErr((IPrintStream**)&err);
        OnShowUsage(err);
        err->Println();
        err->Println(String("Error: ")/* + e.getMessage()*/);
    }
    else if (FAILED(ec)) {
        // e.printStackTrace(System.err);
        system->Exit(1);
    }
    return NOERROR;
}

ECode BaseCommand::ShowUsage()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    AutoPtr<IPrintStream> err;
    system->GetErr((IPrintStream**)&err);
    return OnShowUsage(err);
}

ECode BaseCommand::ShowError(
    /* [in] */ const String& message)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    AutoPtr<IPrintStream> err;
    system->GetErr((IPrintStream**)&err);
    OnShowUsage(err);
    err->Println();
    err->Println(message);
    return NOERROR;
}

ECode BaseCommand::NextOption(
    /* [out] */ String* nextOption)
{
    VALIDATE_NOT_NULL(nextOption)
    *nextOption = NULL;
    if (mCurArgData != NULL) {
        String prev = (*mArgs)[mNextArg - 1];
        Logger::E("BaseCommand", "No argument expected after \"%s\"", prev.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mNextArg >= mArgs->GetLength()) {
        return NOERROR;
    }

    String arg = (*mArgs)[mNextArg];
    if (!arg.StartWith("-")) {
        return NOERROR;
    }

    mNextArg++;
    if (arg.Equals("--")) {
        return NOERROR;
    }

    if (arg.GetLength() > 1 && arg.GetChar(1) != '-') {
        if (arg.GetLength() > 2) {
            mCurArgData = arg.Substring(2);
            *nextOption = arg.Substring(0, 2);
            return NOERROR;
        }
    }
    mCurArgData = NULL;
    *nextOption = arg;
    return NOERROR;
}

ECode BaseCommand::NextArg(
    /* [out] */ String* nextArg)
{
    VALIDATE_NOT_NULL(nextArg)
    if (mCurArgData != NULL) {
        *nextArg = mCurArgData;
        mCurArgData = NULL;
    }
    else if (mNextArg < mArgs->GetLength()) {
        *nextArg = (*mArgs)[mNextArg++];
    }
    else {
        *nextArg = NULL;
    }
    return NOERROR;
}

ECode BaseCommand::NextArgRequired(
    /* [out] */ String* nextArg)
{
    VALIDATE_NOT_NULL(nextArg)
    NextArg(nextArg);
    if (*nextArg == NULL) {
        String prev = (*mArgs)[mNextArg - 1];
        Logger::E("BaseCommand", "Argument expected after \"%s\"", prev.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
