
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/CRemoteServiceCallback.h"
#include "elastos/droid/telecomm/telecom/ConnectionService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telecomm::Internal::EIID_IRemoteServiceCallback;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telecomm {
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
    for (Int32 i = 0; i < cnSize && i < svSize; i++) {
        assert(0 && "TODO");
        // mRemoteConnectionManager.addConnectionService(
        //         mComponentNames.get(i),
        //         IConnectionService.Stub.asInterface(mServices.get(i)));
    }
    AutoPtr<ConnectionService> _host = (ConnectionService*)(mHost->mHost.Get());
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
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos