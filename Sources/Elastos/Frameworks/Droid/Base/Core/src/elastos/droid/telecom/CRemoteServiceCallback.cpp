
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CRemoteServiceCallback.h"
#include "elastos/droid/telecom/ConnectionService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telecom::EIID_IRemoteServiceCallback;
using Elastos::Droid::Internal::Telecom::IIConnectionService;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CRemoteServiceCallback::ResultRunnable::
//===============================================================
CAR_INTERFACE_IMPL(CRemoteServiceCallback::ResultRunnable, Object, IRunnable)

CRemoteServiceCallback::ResultRunnable::ResultRunnable(
    /* [in] */ IList* componentNames,
    /* [in] */ IList* services,
    /* [in] */ CRemoteServiceCallback* host)
    : mComponentNames(componentNames)
    , mServices(services)
    , mHost(host)
{}

ECode CRemoteServiceCallback::ResultRunnable::Run()
{
    Int32 cnSize = 0, svSize = 0;
    mComponentNames->GetSize(&cnSize);
    mServices->GetSize(&svSize);
    AutoPtr<ConnectionService> _host = (ConnectionService*)(mHost->mHost.Get());
    for (Int32 i = 0; i < cnSize && i < svSize; i++) {
        AutoPtr<IInterface> iName, iServ;
        mComponentNames->Get(i, (IInterface**)&iName);
        mServices->Get(i, (IInterface**)&iServ);
        _host->mRemoteConnectionManager->AddConnectionService(
                IComponentName::Probe(iName),
                IIConnectionService::Probe(iServ));
    }
    _host->OnAccountsInitialized();
    Logger::D("CRemoteServiceCallback", "remote connection services found: %p", mServices.Get());
    return NOERROR;
}

//===============================================================
// CRemoteServiceCallback::ErrorRunnable::
//===============================================================
CAR_INTERFACE_IMPL(CRemoteServiceCallback::ErrorRunnable, Object, IRunnable)

CRemoteServiceCallback::ErrorRunnable::ErrorRunnable(
    /* [in] */ CRemoteServiceCallback* host)
    : mHost(host)
{}

ECode CRemoteServiceCallback::ErrorRunnable::Run()
{
    AutoPtr<ConnectionService> _host = (ConnectionService*)(mHost->mHost.Get());
    _host->mAreAccountsInitialized = TRUE;
    return NOERROR;
}

//===============================================================
// CRemoteServiceCallback::
//===============================================================
CAR_OBJECT_IMPL(CRemoteServiceCallback)

CAR_INTERFACE_IMPL(CRemoteServiceCallback, Object, IRemoteServiceCallback)

ECode CRemoteServiceCallback::SetHost(
    /* [in] */ IConnectionService* host)
{
    mHost = host;
    return NOERROR;
}

ECode CRemoteServiceCallback::OnResult(
    /* [in] */ IList* componentNames,
    /* [in] */ IList* services)
{
    AutoPtr<IHandler> hdler;
    mHost->GetHandler((IHandler**)&hdler);
    AutoPtr<ResultRunnable> p = new ResultRunnable(componentNames, services, this);
    Boolean res = FALSE;
    hdler->Post(p, &res);
    return NOERROR;
}

ECode CRemoteServiceCallback::OnError()
{
    AutoPtr<IHandler> hdler;
    mHost->GetHandler((IHandler**)&hdler);
    AutoPtr<ErrorRunnable> p = new ErrorRunnable(this);
    Boolean res = FALSE;
    hdler->Post(p, &res);
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos