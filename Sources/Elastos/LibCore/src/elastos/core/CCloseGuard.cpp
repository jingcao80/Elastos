
#include "CCloseGuard.h"
#include "CThrowable.h"
#include "StringBuilder.h"

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL(CCloseGuard, Object, ICloseGuard)

CAR_OBJECT_IMPL(CCloseGuard)

static AutoPtr<ICloseGuard> InitNOOP()
{
    AutoPtr<ICloseGuard> cg;
    CCloseGuard::New((ICloseGuard**)&cg);
    return cg;
}

static AutoPtr<ICloseGuardReporter> InitREPORTER()
{
    AutoPtr<ICloseGuardReporter> report = new CCloseGuard::DefaultReporter();
    return report;
}

Boolean CCloseGuard::ENABLED = TRUE;
const AutoPtr<ICloseGuard> CCloseGuard::NOOP = InitNOOP();
AutoPtr<ICloseGuardReporter> CCloseGuard::REPORTER = InitREPORTER();

CAR_INTERFACE_IMPL(CCloseGuard::DefaultReporter, Object, ICloseGuardReporter)

ECode CCloseGuard::DefaultReporter::Report (
    /* [in] */ const String& message,
    /* [in] */ IThrowable* allocationSite)
{
    String str = Object::ToString(allocationSite);
    StringBuilder sb("CCloseGuard :");
    sb += str;
    ALOGW(message);
    ALOGW(sb.ToString());

    // AutoPtr<ISystem> system;
    // CSystem::AcquireSingleton((ISystem**)&system);
    // system->LogW(message, str);
    return NOERROR;
}

AutoPtr<ICloseGuard> CCloseGuard::Get()
{
    if (!ENABLED) {
        return NOOP;
    }
    AutoPtr<ICloseGuard> res;
    CCloseGuard::New((ICloseGuard**)&res);
    return res;
}

ECode CCloseGuard::SetEnabled(
    /* [in] */ Boolean enabled)
{
    ENABLED = enabled;
    return NOERROR;
}

ECode CCloseGuard::SetReporter(
    /* [in] */ ICloseGuardReporter* reporter)
{
    if (reporter == NULL) {
        ALOGE("CCloseGuard::SetReporter: reporter == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    REPORTER = reporter;
    return NOERROR;
}

AutoPtr<ICloseGuardReporter> CCloseGuard::GetReporter()
{
    return REPORTER;
}

ECode CCloseGuard::Open(
    /* [in] */ const String& closer)
{
    // always perform the check for valid API usage...
    if (closer.IsNull()) {
        ALOGE("CCloseGuard::Open: closer == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // ...but avoid allocating an allocationSite if disabled
    if (this == NOOP || !ENABLED) {
        return NOERROR;
    }

    StringBuilder sb("Explicit termination method '");
    sb += closer;
    sb += "' not called";

    mAllocationSite = NULL;
    CThrowable::New(sb.ToString(), (IThrowable**)&mAllocationSite);
    return NOERROR;
}

ECode CCloseGuard::Close()
{
    mAllocationSite = NULL;
    return NOERROR;
}

ECode CCloseGuard::WarnIfOpen()
{
    if (mAllocationSite == NULL || !ENABLED) {
        return NOERROR;
    }

    StringBuilder sb("CCloseGuard: A resource was acquired but never released.");
    sb += "See ICloseable for information on avoiding resource leaks.";
    REPORTER->Report(sb.ToString(), mAllocationSite);
    assert(0 && "Please review code!!");
    return NOERROR;
}

} // namespace Core
} // namespace Elastos