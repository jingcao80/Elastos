
#include "CCloseGuard.h"
#include "CThrowable.h"
#include "StringBuilder.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

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
const String CCloseGuard::TAG("CCloseGuard");

CAR_INTERFACE_IMPL(CCloseGuard::DefaultReporter, Object, ICloseGuardReporter)

ECode CCloseGuard::DefaultReporter::Report (
    /* [in] */ const String& message,
    /* [in] */ IThrowable* allocationSite)
{
    Logger::W(TAG, "%s, %s", message.string(), TO_CSTR(allocationSite));

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
        Logger::E(TAG, "SetReporter: reporter == null");
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
        Logger::E(TAG, "Open: closer == null");
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

    REPORTER->Report(String("A resource was acquired but never released."
            " See ICloseable for information on avoiding resource leaks."), mAllocationSite);
    return NOERROR;
}

} // namespace Core
} // namespace Elastos