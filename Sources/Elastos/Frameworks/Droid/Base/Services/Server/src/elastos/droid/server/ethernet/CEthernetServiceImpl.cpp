
#include "elastos/droid/server/ethernet/CEthernetServiceImpl.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/core/AutoLock.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Net::EIID_IIEthernetManager;
using Elastos::Droid::Net::CIpConfiguration;
using Elastos::Core::IThread;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {

const String CEthernetServiceImpl::TAG("CEthernetServiceImpl");

CAR_INTERFACE_IMPL_2(CEthernetServiceImpl, Object, IIEthernetManager, IBinder)

CAR_OBJECT_IMPL(CEthernetServiceImpl)

ECode CEthernetServiceImpl::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mStarted);
    Logger::I(TAG, "Creating EthernetConfigStore");
    mEthernetConfigStore = new EthernetConfigStore();
    mIpConfiguration = mEthernetConfigStore->ReadIpAndProxyConfigurations();

    Logger::I(TAG, "Read stored IP configuration: %s", TO_CSTR(mIpConfiguration));

    AutoPtr<IInterface> b = ServiceManager::GetService(IContext::NETWORKMANAGEMENT_SERVICE);
    mNMService = IINetworkManagementService::Probe(b);

    mTracker = new EthernetNetworkFactory();
    mTracker->constructor();

    return NOERROR;
}

ECode CEthernetServiceImpl::EnforceAccessPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_NETWORK_STATE,
            String("EthernetService"));
}

ECode CEthernetServiceImpl::EnforceChangePermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::CHANGE_NETWORK_STATE,
            String("EthernetService"));
}

ECode CEthernetServiceImpl::EnforceConnectivityInternalPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::CONNECTIVITY_INTERNAL,
            String("ConnectivityService"));
}

ECode CEthernetServiceImpl::Start()
{
    Logger::I(TAG, "Starting Ethernet service");
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&service);
    mCM = IConnectivityManager::Probe(service);

    AutoPtr<IHandlerThread> handlerThread;
    CHandlerThread::New(String("EthernetServiceThread"), (IHandlerThread**)&handlerThread);
    IThread::Probe(handlerThread)->Start();
    AutoPtr<ILooper> looper;
    handlerThread->GetLooper((ILooper**)&looper);
    mHandler = NULL;
    CHandler::New(looper, (IHandler**)&mHandler);

    mTracker->Start(mContext, mHandler);
    mStarted->Set(TRUE);

    return NOERROR;
}

/**
 * Get Ethernet configuration
 * @return the Ethernet Configuration, contained in {@link IpConfiguration}.
 */
ECode CEthernetServiceImpl::GetConfiguration(
    /* [out] */ IIpConfiguration** config)
{
    VALIDATE_NOT_NULL(config)
    FAIL_RETURN(EnforceAccessPermission());

    AutoLock lock(mIpConfiguration);
    return CIpConfiguration::New(mIpConfiguration, config);
}

/**
 * Set Ethernet configuration
 */
ECode CEthernetServiceImpl::SetConfiguration(
    /* [in] */ IIpConfiguration* config)
{
    Boolean v;
    mStarted->Get(&v);
    if (!v) {
        Logger::W(TAG, "System isn't ready enough to change ethernet configuration");
    }

    FAIL_RETURN(EnforceChangePermission())
    FAIL_RETURN(EnforceConnectivityInternalPermission())

    AutoLock lock(mIpConfiguration);
    mEthernetConfigStore->WriteIpAndProxyConfigurations(config);

    // TODO: this does not check proxy settings, gateways, etc.
    // Fix this by making IpConfiguration a complete representation of static configuration.
    if (!Object::Equals(config, mIpConfiguration)) {
        mIpConfiguration = NULL;
        CIpConfiguration::New(config, (IIpConfiguration**)&mIpConfiguration);
        mTracker->Stop();
        mTracker->Start(mContext, mHandler);
    }
    return NOERROR;
}

// @Override
void CEthernetServiceImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    // final IndentingPrintWriter pw = new IndentingPrintWriter(writer, "  ");
    // if (mContext.checkCallingOrSelfPermission(android.Manifest.permission.DUMP)
    //         != PackageManager.PERMISSION_GRANTED) {
    //     pw.println("Permission Denial: can't dump EthernetService from pid="
    //             + Binder.getCallingPid()
    //             + ", uid=" + Binder.getCallingUid());
    //     return;
    // }

    // pw.println("Current Ethernet state: ");
    // pw.increaseIndent();
    // mTracker.dump(fd, pw, args);
    // pw.decreaseIndent();

    // pw.println();
    // pw.println("Stored Ethernet configuration: ");
    // pw.increaseIndent();
    // pw.println(mIpConfiguration);
    // pw.decreaseIndent();

    // pw.println("Handler:");
    // pw.increaseIndent();
    // mHandler.dump(new PrintWriterPrinter(pw), "EthernetServiceImpl");
    // pw.decreaseIndent();
}

ECode CEthernetServiceImpl::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos
