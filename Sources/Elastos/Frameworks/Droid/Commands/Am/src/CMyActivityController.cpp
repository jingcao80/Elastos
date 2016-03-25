
#include "CMyActivityController.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

PInterface CMyActivityController::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_MyActivityController) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CMyActivityController::Probe(riid);
}

ECode CMyActivityController::ActivityStarting(
    /* [in] */ IIntent * intent,
    /* [in] */ const String& pkg,
    /* [out] */ Boolean * res)
{
    VALIDATE_NOT_NULL(res);
    return MyActivityController::ActivityStarting(intent, pkg, res);
}

ECode CMyActivityController::ActivityResuming(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean * res)
{
    VALIDATE_NOT_NULL(res);
    return MyActivityController::ActivityResuming(pkg, res);
}

ECode CMyActivityController::AppCrashed(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg,
    /* [in] */ Int64 timeMillis,
    /* [in] */ const String& stackTrace,
    /* [out] */ Boolean * res)
{
    VALIDATE_NOT_NULL(res);
    return MyActivityController::AppCrashed(
        processName, pid, shortMsg, longMsg, timeMillis, stackTrace, res);
}

ECode CMyActivityController::AppEarlyNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& annotation,
    /* [out] */ Int32 * res)
{
    VALIDATE_NOT_NULL(res);
    return MyActivityController::AppEarlyNotResponding(
        processName, pid, annotation, res);
}

ECode CMyActivityController::AppNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& processStats,
    /* [out] */ Int32 * res)
{
    VALIDATE_NOT_NULL(res);
    return MyActivityController::AppNotResponding(
        processName, pid, processStats, res);
}

ECode CMyActivityController::constructor(
    /* [in] */ Handle32 am)
{
    return MyActivityController::Init(am);
}

ECode CMyActivityController::constructor(
    /* [in] */ Handle32 am,
    /* [in] */ const String& gdbPort)
{
    return MyActivityController::Init(am, gdbPort);
}

}
}
}
}

