
#include "CCloseGuard.h"
#include "CThrowable.h"

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL(CCloseGuard, Object, ICloseGuard)

CAR_OBJECT_IMPL(CCloseGuard)

static AutoPtr<ICloseGuard> initNOOP()
{
    AutoPtr<CCloseGuard> cg;
    CCloseGuard::NewByFriend((CCloseGuard**)&cg);
    return (ICloseGuard*)cg.Get();
}

static AutoPtr<ICloseGuardReporter> initREPORTER()
{
    AutoPtr<ICloseGuardReporter> report;
    report = new CCloseGuard::DefaultReporter();
    return (ICloseGuardReporter*)report.Get();
}

Boolean CCloseGuard::ENABLED = TRUE;
const AutoPtr<ICloseGuard> CCloseGuard::NOOP = initNOOP();
AutoPtr<ICloseGuardReporter> CCloseGuard::REPORTER = initREPORTER();

CAR_INTERFACE_IMPL(CCloseGuard::DefaultReporter, Object, ICloseGuardReporter)

ECode CCloseGuard::DefaultReporter::Report (
    /* [in] */ const String& message,
    /* [in] */ IThrowable* allocationSite)
{
    String str = Object::ToString(allocationSite);
    ALOGW(message);
    ALOGW(str);

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
    AutoPtr<CCloseGuard> res;
    CCloseGuard::NewByFriend((CCloseGuard**)&res);
    return (ICloseGuard*)res.Get();
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
        //throw new NullPointerException("reporter == null");
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
        //throw new NullPointerException("closer == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // ...but avoid allocating an allocationSite if disabled
    if (this == NOOP || !ENABLED) {
        return NOERROR;
    }
    String message = String("Explicit termination method '") + closer + String("' not called");
    mAllocationSite = NULL;
    CThrowable::New(message, (IThrowable**)&mAllocationSite);
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

    String message =
            String("A resource was acquired at attached stack trace but never released. ")
             + String("See java.io.Closeable for information on avoiding resource leaks.");

    REPORTER->Report(message, mAllocationSite);
    return NOERROR;
}

} // namespace Core
} // namespace Elastos