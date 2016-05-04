#include "elastos/droid/server/SystemServiceManager.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/utility/logging/Slogger.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::IClassLoader;
using Elastos::Core::CPathClassLoader;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

#define MANAGER_CATCH_EXCEPTION(expr) \
    do { \
        ECode ec = expr; \
        if (ec == (Int32)E_NO_SUCH_METHOD_EXCEPTION) { \
            Slogger::D(TAG, "Failed to create service  %s, Error: E_NO_SUCH_METHOD_EXCEPTION." \
                "service must have a public constructor with a \'IContext*\' argument named \'ctx\'", name.string()); \
            return ec; \
        } \
        else if (ec == (Int32)E_CLASS_NOT_FOUND_EXCEPTION) { \
            Slogger::D(TAG, "Failed to create service  %s, Error: E_CLASS_NOT_FOUND_EXCEPTION.", name.string()); \
            return ec; \
        } \
        else if (FAILED(ec)) { \
            Slogger::D(TAG, "Failed to create service  %s, Error: %08x.", name.string(), ec); \
            return ec; \
        } \
    } while(0);

const String SystemServiceManager::TAG("SystemServiceManager");
const String SystemServiceManager::sConstructorSignature("Ctx(LElastos/Droid/Content/IContext;*LIInterface;**)E");

CAR_INTERFACE_IMPL(SystemServiceManager, Object, ISystemServiceManager)

SystemServiceManager::SystemServiceManager(
    /* [in] */ IContext* context)
    : mSafeMode(FALSE)
    , mCurrentPhase(-1)
{
    mContext = context;
}

ECode SystemServiceManager::StartService(
    /* [in] */ ISystemService* systemService)
{
    AutoPtr<ISystemService> service = systemService;

    // Register it.
    mServices.PushBack(service);

    // Start it.
    ECode ec = service->OnStart();
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        Slogger::E(TAG, "Failed to start service %s: onStart threw an exception");
        return ec;
    }

    return NOERROR;
}

ECode SystemServiceManager::StartService(
    /* [in] */ const String& className,
    /* [out] */ ISystemService** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (className.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> serviceClass;

    AutoPtr<IClassLoader> cl;
    CPathClassLoader::New(String("/system/lib/Elastos.Droid.Server.eco"), NULL, (IClassLoader**)&cl);

    ECode ec = cl->LoadClass(className, (IClassInfo**)&serviceClass);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to create service %s: "
            "service class not found, usually indicates that the caller should "
            "have called PackageManager.hasSystemFeature() to check whether the "
            "feature is available on this device before trying to start the "
            "services that implement it",  className.string());
        return ec;
    }

    return StartService(serviceClass, result);
}

ECode SystemServiceManager::StartService(
    /* [in */ IClassInfo* serviceClass,
    /* [out] */ ISystemService** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(serviceClass)

    String name;
    serviceClass->GetName(&name);
    Slogger::I(TAG, "Starting %s", name.string());

    // Create the service.
    AutoPtr<IConstructorInfo> constructor;
    MANAGER_CATCH_EXCEPTION(serviceClass->GetConstructorInfoByParamNames(
        sConstructorSignature, (IConstructorInfo**)&constructor))

    AutoPtr<IArgumentList> args;
    MANAGER_CATCH_EXCEPTION(constructor->CreateArgumentList((IArgumentList**)&args))
    MANAGER_CATCH_EXCEPTION(args->SetInputArgumentOfObjectPtr(0, TO_IINTERFACE(mContext)))

    AutoPtr<IInterface> obj;
    MANAGER_CATCH_EXCEPTION(constructor->CreateObject(args, (IInterface**)&obj))

    AutoPtr<ISystemService> service = ISystemService::Probe(obj);

    FAIL_RETURN(StartService(service))

    *result = service;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SystemServiceManager::StartBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase <= mCurrentPhase) {
        Slogger::E(TAG, "Next phase must be larger than previous");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCurrentPhase = phase;

    Slogger::I(TAG, "StartBootPhase %d", mCurrentPhase);
    ECode ec = NOERROR;
    ISystemService* service;
    List<AutoPtr<ISystemService> >::Iterator it;
    for (it = mServices.Begin(); it != mServices.End(); ++it) {
        service = *it;
        ec = service->OnBootPhase(mCurrentPhase);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Failed to boot service %s: onBootPhase threw an exception"
                " during phase %d, error code: %08x",
                TO_CSTR(service), mCurrentPhase, ec);
            return E_RUNTIME_EXCEPTION;
        }
    }

    return NOERROR;
}

ECode SystemServiceManager::StartUser(
    /* [in] */ Int32 userHandle)
{
    ECode ec = NOERROR;
    ISystemService* service;
    List<AutoPtr<ISystemService> >::Iterator it;
    for (it = mServices.Begin(); it != mServices.End(); ++it) {
        service = *it;
        ec = service->OnStartUser(userHandle);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Failed reporting start of user %d: to service  %s, error code: %08x",
                userHandle, TO_CSTR(service), ec);
        }
    }

    return NOERROR;
}

ECode SystemServiceManager::SwitchUser(
    /* [in] */ Int32 userHandle)
{
    ECode ec = NOERROR;
    ISystemService* service;
    List<AutoPtr<ISystemService> >::Iterator it;
    for (it = mServices.Begin(); it != mServices.End(); ++it) {
        service = *it;
        ec = service->OnSwitchUser(userHandle);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Failed reporting switch of user %d: to service  %s, error code: %08x",
                userHandle, TO_CSTR(service), ec);
        }
    }

    return NOERROR;
}

ECode SystemServiceManager::StopUser(
    /* [in] */ Int32 userHandle)
{
    ECode ec = NOERROR;
    ISystemService* service;
    List<AutoPtr<ISystemService> >::Iterator it;
    for (it = mServices.Begin(); it != mServices.End(); ++it) {
        service = *it;
        ec = service->OnStopUser(userHandle);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Failed reporting stop of user %d: to service  %s, error code: %08x",
                userHandle, TO_CSTR(service), ec);
        }
    }

    return NOERROR;
}

ECode SystemServiceManager::CleanupUser(
    /* [in] */ Int32 userHandle)
{
    ECode ec = NOERROR;
    ISystemService* service;
    List<AutoPtr<ISystemService> >::Iterator it;
    for (it = mServices.Begin(); it != mServices.End(); ++it) {
        service = *it;
        ec = service->OnCleanupUser(userHandle);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Failed reporting cleanup of user %d: to service  %s, error code: %08x",
                userHandle, TO_CSTR(service), ec);
        }
    }

    return NOERROR;
}

ECode SystemServiceManager::SetSafeMode(
    /* [in] */ Boolean safeMode)
{
    mSafeMode = safeMode;
    return NOERROR;
}

ECode SystemServiceManager::IsSafeMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSafeMode;
    return NOERROR;
}

ECode SystemServiceManager::Dump()
{
    StringBuilder builder;
    builder.Append("Current phase: ");
    builder.Append(mCurrentPhase);
    builder.Append("\n");
    builder.Append("Services:\n");
    ISystemService* service;
    List<AutoPtr<ISystemService> >::Iterator it;
    for (it = mServices.Begin(); it != mServices.End(); ++it) {
        service = *it;
        builder.Append("\t");
        builder.Append(Object::ToString(service));
        builder.Append("\n");
    }

    Slogger::E(TAG, builder.ToString().string());
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
