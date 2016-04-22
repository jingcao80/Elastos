
#include "CMyActivityController.h"

using Elastos::Droid::App::EIID_IIActivityController;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CAR_INTERFACE_IMPL_2(CMyActivityController, Object, IIActivityController, IBinder);

ECode CMyActivityController::constructor(
    /* [in] */ IIActivityManager* am,
    /* [in] */ const String& gdbPort)
{
    return Am::MyActivityController::constructor(am, gdbPort);
}

ECode CMyActivityController::ActivityResuming(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    return Am::MyActivityController::ActivityResuming(pkg, result);
}

ECode CMyActivityController::ActivityStarting(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    return Am::MyActivityController::ActivityStarting(intent, pkg, result);
}

ECode CMyActivityController::AppCrashed(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg,
    /* [in] */ Int64 timeMillis,
    /* [in] */ const String& stackTrace,
    /* [out] */ Boolean* result)
{
    return Am::MyActivityController::AppCrashed(
            processName, pid, shortMsg, longMsg, timeMillis, stackTrace, result);
}

ECode CMyActivityController::AppEarlyNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& annotation,
    /* [out] */ Int32* result)
{
    return Am::MyActivityController::AppEarlyNotResponding(
            processName, pid, annotation, result);
}

ECode CMyActivityController::AppNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& processStats,
    /* [out] */ Int32* result)
{
    return Am::MyActivityController::AppNotResponding(
            processName, pid, processStats, result);
}

ECode CMyActivityController::SystemNotResponding(
    /* [in] */ const String& message,
    /* [out] */ Int32* result)
{
    return Am::MyActivityController::SystemNotResponding(message, result);
}

ECode CMyActivityController::ToString(
    /* [out] */ String* string)
{
    return Object::ToString(string);
}

ECode CMyActivityController::Run()
{
    return Am::MyActivityController::Run();
}

}
}
}
}

