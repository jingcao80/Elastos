
#include "elastos/droid/server/connectivity/Vpn.h"
#include "elastos/droid/server/connectivity/CVpnObserver.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/FileUtils.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/app/AppGlobals.h>
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.KeyStore.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>

#include <cutils/log.h>
#include "netutils/ifc.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/route.h>
#include <linux/ipv6_route.h>

using Elastos::Droid::R;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ISystemService;
using Elastos::Droid::Os::CSystemService;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTING;
using Elastos::Droid::Net::NetworkInfoDetailedState_FAILED;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_IDLE;
using Elastos::Droid::Net::NetworkInfoDetailedState_AUTHENTICATING;
using Elastos::Droid::Net::IIpPrefix;
using Elastos::Droid::Net::CIpPrefix;
using Elastos::Droid::Net::CRouteInfo;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::INetworkMisc;
using Elastos::Droid::Net::CNetworkMisc;
using Elastos::Droid::Net::CNetworkCapabilities;
using Elastos::Droid::Net::CUidRange;
using Elastos::Droid::Net::IUidRangeHelper;
using Elastos::Droid::Net::CUidRangeHelper;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::LocalSocketAddressNamespace_RESERVED;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Internal::Net::CLegacyVpnInfo;
using Elastos::Droid::Internal::Net::IVpnConfig;
using Elastos::Droid::Internal::Net::CVpnConfig;
using Elastos::Droid::Internal::Net::IVpnConfigHelper;
using Elastos::Droid::Internal::Net::CVpnConfigHelper;
using Elastos::Droid::Internal::Net::ILegacyVpnInfoHelper;
using Elastos::Droid::Internal::Net::CLegacyVpnInfoHelper;
using Elastos::Droid::KeyStore::Security::ICredentials;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::ICloseable;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInet6Address;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInet4AddressHelper;
using Elastos::Net::CInet4AddressHelper;
using Elastos::Net::IInet6AddressHelper;
using Elastos::Net::CInet6AddressHelper;
using Elastos::Utility::ICollection;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::CTreeSet;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

static Int32 inet4 = -1;
static Int32 inet6 = -1;

static inline in_addr_t *as_in_addr(sockaddr *sa) {
    return &((sockaddr_in *)sa)->sin_addr.s_addr;
}

//=============================================================================
// Vpn::Connection
//=============================================================================
CAR_INTERFACE_IMPL(Vpn::Connection, Object, IServiceConnection);

ECode Vpn::Connection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    mService = service;
    return NOERROR;
}

ECode Vpn::Connection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    mService = NULL;
    return NOERROR;
}


//=============================================================================
// Vpn::LegacyVpnRunner
//=============================================================================
const String Vpn::LegacyVpnRunner::TAG("LegacyVpnRunner");
Object Vpn::LegacyVpnRunner::sTAGLock;

Vpn::LegacyVpnRunner::LegacyVpnRunner()
    : mTimer(-1)
{}

ECode Vpn::LegacyVpnRunner::constructor(
    /* [in] */ IVpnConfig* config,
    /* [in] */ ArrayOf<String>* racoon,
    /* [in] */ ArrayOf<String>* mtpd,
    /* [in] */ Vpn* host)
{
    Thread::constructor(TAG);

    CAtomicInteger32::New(IConnectivityManager::TYPE_NONE, (IAtomicInteger32**)&mOuterConnection);

    mHost->mConfig = config;
    mDaemons = ArrayOf<String>::Alloc(2);
    (*mDaemons)[0] = "racoon";
    (*mDaemons)[1] = "mtpd";
    // TODO: clear arguments from memory once launched
    mArguments = ArrayOf<StringArray>::Alloc(2);
    mArguments->Set(0, racoon);
    mArguments->Set(1, mtpd);
    mSockets = ArrayOf<ILocalSocket*>::Alloc(mDaemons->GetLength());

    // This is the interface which VPN is running on,
    // mConfig.interfaze will change to point to OUR
    // internal interface soon. TODO - add inner/outer to mconfig
    // TODO - we have a race - if the outer iface goes away/disconnects before we hit this
    // we will leave the VPN up.  We should check that it's still there/connected after
    // registering
    mHost->mConfig->GetInterfaze(&mOuterInterface);
    Int32 ival;
    ECode ec = mHost->mConnService->FindConnectionTypeForIface(mOuterInterface, &ival);
    if (FAILED(ec)) {
        mOuterConnection->Set(IConnectivityManager::TYPE_NONE);
    }
    else {
        mOuterConnection->Set(ival);
    }

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IConnectivityManager::CONNECTIVITY_ACTION);
    AutoPtr<IIntent> intent;
    return mHost->mContext->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&intent);
}

ECode Vpn::LegacyVpnRunner::Check(
    /* [in] */ const String& interfaze)
{
    if (interfaze.Equals(mOuterInterface)){
        Logger::I(TAG, "Legacy VPN is going down with %s", interfaze.string());
        Exit();
    }
    return NOERROR;
}

ECode Vpn::LegacyVpnRunner::Exit()
{
    // We assume that everything is reset after stopping the daemons.
    Interrupt();
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    for (Int32 i = 0; i < mSockets->GetLength(); i++) {
        ioUtils->CloseQuietly(ICloseable::Probe((*mSockets)[i]));
    }
    mHost->AgentDisconnect();
    mHost->mContext->UnregisterReceiver(mBroadcastReceiver);

    return NOERROR;
}

ECode Vpn::LegacyVpnRunner::Run()
{
    // Wait for the previous thread since it has been interrupted.
    Logger::V(TAG, "Waiting");
    {
        AutoLock lock(sTAGLock);
        Logger::V(TAG, "Executing");
        Execute();
        MonitorDaemons();
    }
    return NOERROR;
}

ECode Vpn::LegacyVpnRunner::Checkpoint(
    /* [in] */ Boolean yield)
{
    Int64 now = SystemClock::GetElapsedRealtime();
    if (mTimer == -1) {
        mTimer = now;
        Thread::Sleep(1);
    }
    else if (now - mTimer <= 60000) {
        Thread::Sleep(yield ? 200 : 1);
    }
    else {
        mHost->UpdateState(NetworkInfoDetailedState_FAILED, String("checkpoint"));
        // throw new IllegalStateException("Time is up");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode Vpn::LegacyVpnRunner::Execute()
{
    AutoPtr<Elastos::Droid::Os::ISystemService> systemService;
    AutoPtr<IFile> state, state2;
    AutoPtr< ArrayOf<String> > parameters;
    String readfile, interfaze, endpoint;
    AutoPtr<IList> dnsServers, searchDomains, routes;
    // Catch all exceptions so we can clean up few things.
    Boolean initFinished = FALSE, bval;
    Boolean restart = FALSE, result;
    Int32 count;

    // try {
    // Initialize the timer.
    ECode ec = Checkpoint(FALSE);
    if(FAILED(ec)) goto ERROR;

    CSystemService::AcquireSingleton((Elastos::Droid::Os::ISystemService**)&systemService);
    // Wait for the daemons to stop.
    for (Int32 i = 0; i < mDaemons->GetLength(); i++){
        String daemon = (*mDaemons)[i];
        Boolean isStopped = FALSE;
        systemService->IsStopped(daemon, &isStopped);
        while(!isStopped){
            ec = Checkpoint(TRUE);
            if(FAILED(ec)) goto ERROR;
        }
    }

    // Clear the previous state.
    CFile::New(String("/data/misc/vpn/state"), (IFile**)&state);
    state->Delete(&result);
    if(state->Exists(&result), result) {
        //throw new IllegalStateException("Cannot delete the state");
        ec = E_ILLEGAL_STATE_EXCEPTION;
        goto ERROR;
    }

    CFile::New(String("/data/misc/vpn/abort"), (IFile**)&state2);
    state2->Delete(&result);
    initFinished = TRUE;

    // Check if we need to restart any of the daemons.
    for (Int32 i = 0; i < mArguments->GetLength(); i++){
        restart = restart || ((*mArguments)[i] != NULL);
    }
    if (!restart) {
        mHost->AgentDisconnect();
        ec = NOERROR;
        goto RETURN;
    }
    mHost->UpdateState(NetworkInfoDetailedState_CONNECTING, String("execute"));

    // Start the daemon with arguments.
    for (Int32 i = 0; i < mDaemons->GetLength(); ++i) {
        AutoPtr<ArrayOf<String> > arguments = (*mArguments)[i];
        if (arguments == NULL) {
            continue;
        }

        // Start the daemon.
        String daemon = (*mDaemons)[i];
        systemService->Start(daemon);

        // Wait for the daemon to start.
        systemService->IsRunning(daemon, &result);
        while (!result) {
            ec = Checkpoint(TRUE);
            if(FAILED(ec)) goto ERROR;
        }

        // Create the control socket.
        AutoPtr<ILocalSocket> socket;
        CLocalSocket::New((ILocalSocket**)&socket);
        mSockets->Set(i, socket);
        AutoPtr<ILocalSocketAddress> address;
        CLocalSocketAddress::New(daemon, LocalSocketAddressNamespace_RESERVED, (ILocalSocketAddress**)&address);

        // Wait for the socket to connect.
        while (TRUE) {
            ec = (*mSockets)[i]->Connect(address);
            if(FAILED(ec)){
                ec = Checkpoint(TRUE);
                if(FAILED(ec)) goto ERROR;
            }
            else {
                break;
            }
        }
        (*mSockets)[i]->SetSoTimeout(500);

        // Send over the arguments.
        AutoPtr<IOutputStream> out;
        (*mSockets)[i]->GetOutputStream((IOutputStream**)&out);
        for (Int32 j = 0; j < arguments->GetLength(); ++j){
            String argument = (*arguments)[j];
            AutoPtr< ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(argument.GetLength());
            bytes->Copy((Byte*)argument.string(), bytes->GetLength());
            if (bytes->GetLength() >= 0xFFFF) {
                //throw new IllegalArgumentException("Argument is too large");
                ec = E_ILLEGAL_ARGUMENT_EXCEPTION;
                goto ERROR;
            }
            out->Write(bytes->GetLength() >> 8);
            out->Write(bytes->GetLength());
            out->Write(bytes);
            Checkpoint(FALSE);
        }
        out->Write(0xFF);
        out->Write(0xFF);
        IFlushable::Probe(out)->Flush();

        // Wait for End-of-File.
        AutoPtr<IInputStream> in;
        (*mSockets)[i]->GetInputStream((IInputStream**)&in);
        while (TRUE) {
            Int32 value;
            ec = in->Read(&value);
            if (SUCCEEDED(ec) && value == -1) {
                break;
            }
            ec = Checkpoint(TRUE);
            if(FAILED(ec)) goto ERROR;
        }
    }

    // Wait for the daemons to create the new state.
    while (state->Exists(&result), !result) {
        // Check if a running daemon is dead.
        for (Int32 i = 0; i < mDaemons->GetLength(); ++i) {
            String daemon = (*mDaemons)[i];
            Boolean isRunning;
            if ((*mArguments)[i] != NULL &&
                    (systemService->IsRunning(daemon, &isRunning), !isRunning)) {
                //throw new IllegalStateException(daemon + " is dead");
                ec = E_ILLEGAL_STATE_EXCEPTION;
                goto ERROR;
            }
        }
        ec = Checkpoint(TRUE);
        if(FAILED(ec)) goto ERROR;
    }

    // Now we are connected. Read and parse the new state.
    FileUtils::ReadTextFile(state, 0, String(NULL), &readfile);
    StringUtils::Split(readfile, String("\n"), -1, (ArrayOf<String>**)&parameters);
    if (parameters->GetLength() != 7) {
        //throw new IllegalStateException("Cannot parse the state");
        ec = E_ILLEGAL_STATE_EXCEPTION;
        goto ERROR;
    }

    // Set the interface and the addresses in the config.
    mHost->mConfig->SetInterfaze((*parameters)[0].Trim());
    mHost->mConfig->AddLegacyAddresses((*parameters)[1]);

    mHost->mConfig->GetRoutes((IList**)&routes);
    // Set the routes if they are not set in the config.
    if (routes == NULL || (routes->IsEmpty(&bval), bval)) {
        mHost->mConfig->AddLegacyRoutes((*parameters)[2]);
    }

    // Set the DNS servers if they are not set in the config.
    mHost->mConfig->GetDnsServers((IList**)&dnsServers);

    if (dnsServers == NULL || (dnsServers->GetSize(&count), count == 0)) {
        String dnsServersStr = (*parameters)[3].Trim();
        if (!dnsServersStr.IsEmpty()) {
            AutoPtr< ArrayOf<String> > servers;
            StringUtils::Split(dnsServersStr, String(" "), (ArrayOf<String>**)&servers);
            dnsServers = NULL;
            CArrayList::New((IList**)&dnsServers);
            for (Int32 i = 0; i < servers->GetLength(); i++) {
                AutoPtr<ICharSequence> server;
                CString::New((*servers)[i], (ICharSequence**)&server);
                dnsServers->Add(server.Get());
            }
            mHost->mConfig->SetDnsServers(dnsServers);
        }
    }

    // Set the search domains if they are not set in the config.
    mHost->mConfig->GetSearchDomains((IList**)&searchDomains);
    if (searchDomains == NULL || (searchDomains->GetSize(&count), count == 0)) {
        String searchDomainsStr = (*parameters)[4].Trim();
        if (!searchDomainsStr.IsEmpty()) {
            AutoPtr< ArrayOf<String> > domains;
            StringUtils::Split(searchDomainsStr, String(" "), (ArrayOf<String>**)&domains);
            searchDomains = NULL;
            CArrayList::New((IList**)&searchDomains);
            for (Int32 i = 0; i < domains->GetLength(); i++) {
                AutoPtr<ICharSequence> domain;
                CString::New((*domains)[i], (ICharSequence**)&domain);
                searchDomains->Add(domain.Get());
            }
            mHost->mConfig->SetSearchDomains(searchDomains);
        }
    }

    // Add a throw route for the VPN server endpoint, if one was specified.
    endpoint = (*parameters)[5];
    if (!endpoint.IsEmpty()) {
        AutoPtr<IInetAddressHelper> helper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
        AutoPtr<IInetAddress> addr;
        ECode ec = helper->ParseNumericAddress(endpoint, (IInetAddress**)&addr);
        if (FAILED(ec)) {
            Logger::E(TAG, "Exception constructing throw route to %s", endpoint.string());
        }
        else {
            if (IInet4Address::Probe(addr) || IInet6Address::Probe(addr)) {
                Int32 value = 128;
                if (IInet4Address::Probe(addr)) value = 32;
                AutoPtr<IIpPrefix> prefix;
                CIpPrefix::New(addr, value, (IIpPrefix**)&prefix);
                AutoPtr<IRouteInfo> ri;
                CRouteInfo::New(prefix, IRouteInfo::RTN_THROW, (IRouteInfo**)&ri);
                routes->Add(ri.Get());
            }
            else {
                Logger::E(TAG, "Unknown IP address family for VPN endpoint: %s", endpoint.string());
            }
        }
    }

    {
        Object& lockObj = mHost->mLock;
        AutoLock lock(lockObj);
        // Set the start time
        mHost->mConfig->SetStartTime(SystemClock::GetElapsedRealtime());

        // Check if the thread is interrupted while we are waiting.
        ec = Checkpoint(FALSE);
        if(FAILED(ec)) goto ERROR;

        // Check if the interface is gone while we are waiting.
        if (Check(interfaze) == 0) {
            //throw new IllegalStateException(mConfig.interfaze + " is gone");
            ec = E_ILLEGAL_STATE_EXCEPTION;
            goto ERROR;
        }

        // Now NetworkManagementEventObserver is watching our back.
        mHost->mConfig->GetInterfaze(&mHost->mInterface);
        mHost->mVpnUsers = NULL;
        CArrayList::New((IList**)&mHost->mVpnUsers);

        mHost->AgentConnect();

        Logger::I(TAG, "Connected!");
    }
    // } catch (Exception e) {
    //     Log.i(TAG, "Aborting", e);
    //     exit();
    // }
ERROR:
    mHost->UpdateState(NetworkInfoDetailedState_FAILED, String("execute"));
    // Log.i(TAG, "Aborting", e);
    Exit();
    // } finally {
RETURN:
    if (!initFinished) {
        for (Int32 i = 0; i < mDaemons->GetLength(); ++i){
            String daemon = (*mDaemons)[i];
            systemService->Stop(daemon);
        }
    }

    // Do not leave an unstable state.
    NetworkInfoDetailedState niState;
    if (!initFinished ||
        (mHost->mNetworkInfo->GetDetailedState(&niState), niState == NetworkInfoDetailedState_CONNECTING)) {
        mHost->AgentDisconnect();
    }
    // }
    return ec;
}

void Vpn::LegacyVpnRunner::MonitorDaemons()
{
    Boolean result;
    if (mHost->mNetworkInfo->IsConnected(&result), !result) {
        return;
    }

    AutoPtr<Elastos::Droid::Os::ISystemService> systemService;
    CSystemService::AcquireSingleton((Elastos::Droid::Os::ISystemService**)&systemService);
    // try {
    while (TRUE) {
        Thread::Sleep(2000);
        for (Int32 i = 0; i < mDaemons->GetLength(); i++) {
            Boolean result;
            if ((*mArguments)[i] != NULL &&
                    (systemService->IsStopped((*mDaemons)[i], &result), result)) {
                goto RETURN;
            }
        }
    }
    // } catch (InterruptedException e) {
    //     Log.d(TAG, "interrupted during monitorDaemons(); stopping services");
    // } finally {
RETURN:
    for (Int32 i = 0; i < mDaemons->GetLength(); i++) {
        String daemon = (*mDaemons)[i];
        systemService->Stop(daemon);
    }

    mHost->AgentDisconnect();
    // }
}

ECode Vpn::UserIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Int32 userHandle;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE,
        IUserHandle::USER_NULL, &userHandle);
    if (userHandle == IUserHandle::USER_NULL) return NOERROR;

    if (IIntent::ACTION_USER_ADDED.Equals(action)) {
        mHost->OnUserAdded(userHandle);
    }
    else if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        mHost->OnUserRemoved(userHandle);
    }
    return NOERROR;
}


ECode Vpn::LegacyVpnRunnerReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (!mHost->mEnableTeardown) return NOERROR;

    String action;
    intent->GetAction(&action);
    if (action.Equals(IConnectivityManager::CONNECTIVITY_ACTION)) {
        Int32 ival, oc;
        intent->GetInt32Extra(IConnectivityManager::EXTRA_NETWORK_TYPE,
            IConnectivityManager::TYPE_NONE, &ival);
        mLegacyVpnRunner->mOuterConnection->Get(&oc);
        if (ival == oc) {
            AutoPtr<IInterface> obj;
            intent->GetExtra(IConnectivityManager::EXTRA_NETWORK_INFO, (IInterface**)&obj);
            INetworkInfo* info = INetworkInfo::Probe(obj);
            Boolean bval;
            if (info != NULL && (info->IsConnectedOrConnecting(&bval), !bval)) {
                mHost->mObserver->InterfaceStatusChanged(mLegacyVpnRunner->mOuterInterface, FALSE);
            }
        }
    }
    return NOERROR;
}

////////////////////////////////////////////////////////////////////////////////
// Vpn
const String Vpn::NETWORKTYPE("VPN");
const String Vpn::TAG("Vpn");
const Boolean Vpn::LOGD = TRUE;

Vpn::Vpn(
    /* [in] */ ILooper* looper,
    /* [in] */ IContext* context,
    /* [in] */ IINetworkManagementService* netService,
    /* [in] */ IIConnectivityManager* connService,
    /* [in] */ Int32 userHandle)
    : mContext(context)
    , mOwnerUID(0)
    , mHostUID(0)
    , mEnableTeardown(TRUE)
    , mUserHandle(userHandle)
{
    mNetd = netService;
    mConnService = connService;
    mUserHandle = userHandle;
    mLooper = looper;

    mPackage = IVpnConfig::LEGACY_VPN;
    mHostUID = GetAppUid(mPackage, mUserHandle);

    CVpnObserver::New((Handle32)this, (IINetworkManagementEventObserver**)&mObserver);
    ECode ec = netService->RegisterObserver(mObserver);
    if (FAILED(ec)) {
        Logger::W(TAG, "Problem registering observer");
    }

    if (userHandle == IUserHandle::USER_OWNER) {
        // Owner's VPN also needs to handle restricted users
        mUserIntentReceiver = new UserIntentReceiver(this);

        AutoPtr<IIntentFilter> intentFilter;
        CIntentFilter::New((IIntentFilter**)&intentFilter);
        intentFilter->AddAction(IIntent::ACTION_USER_ADDED);
        intentFilter->AddAction(IIntent::ACTION_USER_REMOVED);
        AutoPtr<IIntent> intent;
        mContext->RegisterReceiverAsUser(
            mUserIntentReceiver, UserHandle::ALL, intentFilter,
            String(NULL), NULL, (IIntent**)&intent);
    }

    CNetworkInfo::New(IConnectivityManager::TYPE_VPN, 0, NETWORKTYPE, String(""),
        (INetworkInfo**)&mNetworkInfo);
    // TODO: Copy metered attribute and bandwidths from physical transport, b/16207332
    CNetworkCapabilities::New((INetworkCapabilities**)&mNetworkCapabilities);
    mNetworkCapabilities->AddTransportType(INetworkCapabilities::TRANSPORT_VPN);
    mNetworkCapabilities->RemoveCapability(INetworkCapabilities::NET_CAPABILITY_NOT_VPN);
}

ECode Vpn::SetEnableTeardown(
    /* [in] */ Boolean enableTeardown)
{
    mEnableTeardown = enableTeardown;
    return NOERROR;
}

void Vpn::UpdateState(
    /* [in] */ NetworkInfoDetailedState detailedState,
    /* [in] */ const String& reason)
{
    if (LOGD) {
        Logger::D(TAG, "setting state=%d, reason=%s", detailedState, reason.string());
    }
    mNetworkInfo->SetDetailedState(detailedState, reason, String(NULL));

    if (mNetworkAgent != NULL) {
        mNetworkAgent->SendNetworkInfo(mNetworkInfo);
    }
}

ECode Vpn::Prepare(
    /* [in] */ const String& oldPackage,
    /* [in] */ const String& newPackage,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoLock lock(mLock);
    // Return FALSE if the package does not match.
    if (!oldPackage.IsNull() && !oldPackage.Equals(mPackage)) {

        // The package doesn't match. If this VPN was not previously authorized, return FALSE
        // to force user authorization. Otherwise, revoke the VPN anyway.
        if (!oldPackage.Equals(IVpnConfig::LEGACY_VPN) && IsVpnUserPreConsented(oldPackage)) {
            Int64 token = Binder::ClearCallingIdentity();

            // This looks bizarre, but it is what ConfirmDialog in VpnDialogs is doing when
            // the user clicks through to allow the VPN to consent. So we are emulating the
            // action of the dialog without actually showing it.
            Boolean bval;
            Prepare(String(NULL), oldPackage, &bval);

            Binder::RestoreCallingIdentity(token);
            *result = TRUE;
            return NOERROR;
        }
        return NOERROR;
    }

    // Return TRUE if we do not need to revoke.
    if (newPackage.IsNull() ||
            (newPackage.Equals(mPackage) && !newPackage.Equals(IVpnConfig::LEGACY_VPN))) {
        *result = TRUE;
        return NOERROR;
    }

    // Check if the caller is authorized.
    FAIL_RETURN(EnforceControlPermission());

    // Reset the interface.
    if (mInterface != NULL) {
        mStatusIntent = NULL;
        AgentDisconnect();
        NativeReset(mInterface);
        mInterface = NULL;
        mVpnUsers = NULL;
    }

    // Revoke the connection or stop LegacyVpnRunner.
    //alex maybe have return value:Parcel::Obtain()
    if (mConnection != NULL) {
        assert(0);
//        mConnection->mService->Transact(IBinder::LAST_CALL_TRANSACTION,
//                            Parcel::Obtain(), NULL, IBinder::FLAG_ONEWAY);
        mContext->UnbindService(mConnection);
        mConnection = NULL;
    }
    else if (mLegacyVpnRunner != NULL) {
        mLegacyVpnRunner->Exit();
        mLegacyVpnRunner = NULL;
    }

    Int64 token = Binder::ClearCallingIdentity();

    ECode ec = mNetd->DenyProtect(mOwnerUID);
    if (FAILED(ec)) {
        Logger::W(TAG, "Failed to disallow UID %d to call protect() ", mOwnerUID);
    }

    Binder::RestoreCallingIdentity(token);

//    Logger::I(TAG, StringBuffer("Switched from ") + mPackage + " to " + newPackage);
    mPackage = newPackage;

    mOwnerUID = GetAppUid(newPackage, mUserHandle);
    token = Binder::ClearCallingIdentity();

    ec = mNetd->AllowProtect(mOwnerUID);
    if (FAILED(ec)) {
        Logger::W(TAG, "Failed to allow UID %d to call protect() ", mOwnerUID);
    }
    Binder::RestoreCallingIdentity(token);

    mConfig = NULL;

    UpdateState(NetworkInfoDetailedState_IDLE, String("prepare"));
    *result = TRUE;
    return NOERROR;
}

ECode Vpn::SetPackageAuthorization(
    /* [in] */ Boolean authorized)
{
    // Check if the caller is authorized.
    EnforceControlPermission();

    if (mPackage == NULL || IVpnConfig::LEGACY_VPN.Equals(mPackage)) {
        return NOERROR;
    }

    Int64 token = Binder::ClearCallingIdentity();
    ECode ec = NOERROR;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    IAppOpsManager* appOps = IAppOpsManager::Probe(obj);
    ec = appOps->SetMode(IAppOpsManager::OP_ACTIVATE_VPN, mOwnerUID, mPackage,
        authorized ? IAppOpsManager::MODE_ALLOWED : IAppOpsManager::MODE_IGNORED);
    if (FAILED(ec)) {
        Logger::W(TAG, "Failed to set app ops for package %s", mPackage.string());
    }

    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

Boolean Vpn::IsVpnUserPreConsented(
    /* [in] */ const String& packageName)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    IAppOpsManager* appOps = IAppOpsManager::Probe(obj);

    // Verify that the caller matches the given package and has permission to activate VPNs.
    Int32 result;
    appOps->NoteOpNoThrow(IAppOpsManager::OP_ACTIVATE_VPN,
        Binder::GetCallingUid(), packageName, &result);
    return result == IAppOpsManager::MODE_ALLOWED;
}

Int32 Vpn::GetAppUid(
    /* [in] */ const String& app,
    /* [in] */ Int32 userHandle)
{
    if (IVpnConfig::LEGACY_VPN.Equals(app)) {
        return Process::MyUid();
    }
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 result = -1;
    ECode ec = pm->GetPackageUid(app, userHandle, &result);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        result = -1;
    }
    return result;
}

AutoPtr<INetworkInfo> Vpn::GetNetworkInfo()
{
    return mNetworkInfo;
}

AutoPtr<ILinkProperties> Vpn::MakeLinkProperties()
{
    Boolean allowIPv4, allowIPv6;
    mConfig->GetAllowIPv4(&allowIPv4);
    mConfig->GetAllowIPv6(&allowIPv6);

    AutoPtr<ILinkProperties> lp;
    CLinkProperties::New((ILinkProperties**)&lp);

    lp->SetInterfaceName(mInterface);

    Boolean hasNext, bval;
    AutoPtr<IList> list;
    mConfig->GetAddresses((IList**)&list);
    if (list != NULL) {
        AutoPtr<IIterator> it;
        list->GetIterator((IIterator**)&it);
        AutoPtr<IInetAddress> ia;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            ILinkAddress* address = ILinkAddress::Probe(obj);
            lp->AddLinkAddress(address, &bval);

            ia = NULL;
            address->GetAddress((IInetAddress**)&ia);

            allowIPv4 |= IInet4Address::Probe(ia) != NULL;
            allowIPv6 |= IInet6Address::Probe(ia) != NULL;
        }
    }

    list = NULL;
    mConfig->GetRoutes((IList**)&list);
    if (list != NULL) {
        AutoPtr<IIterator> it;
        list->GetIterator((IIterator**)&it);
        AutoPtr<IInetAddress> address;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IRouteInfo* route = IRouteInfo::Probe(obj);
            lp->AddRoute(route, &bval);

            AutoPtr<IIpPrefix> prefix;
            route->GetDestination((IIpPrefix**)&prefix);
            address = NULL;
            prefix->GetAddress((IInetAddress**)&address);

            allowIPv4 |= IInet4Address::Probe(address) != NULL;
            allowIPv6 |= IInet6Address::Probe(address) != NULL;
        }
    }

    list = NULL;
    mConfig->GetDnsServers((IList**)&list);
    if (list != NULL) {
        AutoPtr<IIterator> it;
        list->GetIterator((IIterator**)&it);
        AutoPtr<IInetAddress> address;
        AutoPtr<IInetAddressHelper> helper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            String dnsServer = Object::ToString(obj);
            address = NULL;
            helper->ParseNumericAddress(dnsServer, (IInetAddress**)&address);
            lp->AddDnsServer(address, &bval);

            allowIPv4 |= IInet4Address::Probe(address) != NULL;
            allowIPv6 |= IInet6Address::Probe(address) != NULL;
        }
    }

    if (!allowIPv4) {
        AutoPtr<IInet4AddressHelper> helper;
        CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&helper);
        AutoPtr<IInetAddress> any;
        helper->GetANY((IInetAddress**)&any);
        AutoPtr<IIpPrefix> prefix;
        CIpPrefix::New(any, 0, (IIpPrefix**)&prefix);
        AutoPtr<IRouteInfo> ri;
        CRouteInfo::New(prefix, IRouteInfo::RTN_UNREACHABLE, (IRouteInfo**)&ri);
        lp->AddRoute(ri, &bval);
    }
    if (!allowIPv6) {
        AutoPtr<IInet6AddressHelper> helper;
        CInet6AddressHelper::AcquireSingleton((IInet6AddressHelper**)&helper);
        AutoPtr<IInetAddress> any;
        helper->GetANY((IInetAddress**)&any);
        AutoPtr<IIpPrefix> prefix;
        CIpPrefix::New(any, 0, (IIpPrefix**)&prefix);
        AutoPtr<IRouteInfo> ri;
        CRouteInfo::New(prefix, IRouteInfo::RTN_UNREACHABLE, (IRouteInfo**)&ri);
        lp->AddRoute(ri, &bval);
    }

    // Concatenate search domains into a string.
    StringBuilder buffer;
    list = NULL;
    mConfig->GetSearchDomains((IList**)&list);
    if (list != NULL) {
        AutoPtr<IIterator> it;
        list->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            String domain = Object::ToString(obj);
            buffer.Append(domain);
            buffer.Append(" ");
        }
    }
    lp->SetDomains(buffer.ToString().Trim());

    // TODO: Stop setting the MTU in NativeCreate and set it here.

    return lp;
}

ECode Vpn::AgentConnect()
{
    AutoPtr<ILinkProperties> lp = MakeLinkProperties();

    Boolean bval1, bval2;
    lp->HasIPv4DefaultRoute(&bval1);
    lp->HasIPv6DefaultRoute(&bval2);
    if (bval1 || bval2) {
        mNetworkCapabilities->AddCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET);
    }
    else {
        mNetworkCapabilities->RemoveCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET);
    }

    String nullStr;
    mNetworkInfo->SetIsAvailable(TRUE);
    mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_CONNECTED, nullStr, nullStr);

    Boolean bval;
    AutoPtr<INetworkMisc> networkMisc;
    CNetworkMisc::New((INetworkMisc**)&networkMisc);
    mConfig->GetAllowBypass(&bval);
    networkMisc->SetAllowBypass(bval);

    Int64 token = Binder::ClearCallingIdentity();
    AutoPtr<MyNetworkAgent> na = new MyNetworkAgent();
    na->constructor(mLooper, mContext, NETWORKTYPE,
        mNetworkInfo, mNetworkCapabilities, lp, 0, networkMisc);
    mNetworkAgent = (INetworkAgent*)na.Get();

    Binder::RestoreCallingIdentity(token);

    FAIL_RETURN(AddVpnUserLocked(mUserHandle))
    // If we are owner assign all Restricted Users to this VPN
    if (mUserHandle == IUserHandle::USER_OWNER) {
        token = Binder::ClearCallingIdentity();

        AutoPtr<IUserManagerHelper> helper;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
        AutoPtr<IUserManager> userMgr;
        helper->Get(mContext, (IUserManager**)&userMgr);
        AutoPtr<IList> users;
        userMgr->GetUsers((IList**)&users);

        Binder::RestoreCallingIdentity(token);

        AutoPtr<IIterator> it;
        users->GetIterator((IIterator**)&it);
        Boolean hasNext;
        Int32 id;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IUserInfo* user = IUserInfo::Probe(obj);
            if (user->IsRestricted(&bval), bval) {
                user->GetId(&id);
                FAIL_RETURN(AddVpnUserLocked(id))
            }
        }
    }
    Int32 size;
    mVpnUsers->GetSize(&size);
    AutoPtr<ArrayOf<IUidRange*> > array = ArrayOf<IUidRange*>::Alloc(size);
    AutoPtr<IIterator> it;
    mVpnUsers->GetIterator((IIterator**)&it);
    Boolean hasNext;
    Int32 i = 0;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IUidRange* ur = IUidRange::Probe(obj);
        array->Set(i++, ur);
    }

    mNetworkAgent->AddUidRanges(array);
    return NOERROR;
}

ECode Vpn::AgentDisconnect(
    /* [in] */ INetworkInfo* networkInfo,
    /* [in] */ INetworkAgent* networkAgent)
{
    networkInfo->SetIsAvailable(FALSE);
    String nullStr;
    networkInfo->SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, nullStr, nullStr);
    if (networkAgent != NULL) {
        networkAgent->SendNetworkInfo(networkInfo);
    }
    return NOERROR;
}

ECode Vpn::AgentDisconnect(
    /* [in] */ INetworkAgent* networkAgent)
{
    AutoPtr<INetworkInfo> networkInfo;
    CNetworkInfo::New(mNetworkInfo, (INetworkInfo**)&networkInfo);
    return AgentDisconnect(networkInfo, networkAgent);
}

ECode Vpn::AgentDisconnect()
{
    Boolean bval;
    mNetworkInfo->IsConnected(&bval);
    if (bval) {
        AgentDisconnect(mNetworkInfo, mNetworkAgent);
        mNetworkAgent = NULL;
    }
    return NOERROR;
}

ECode Vpn::Establish(
   /* [in] */ IVpnConfig* config,
   /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoLock lock(mLock);

    // Check if the caller is already prepared.
    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    AutoPtr<IUserManager> mgr;
    helper->Get(mContext, (IUserManager**)&mgr);

    if (Binder::GetCallingUid() != mOwnerUID) {
        return NOERROR;
    }

    // Check if the service is properly declared.
    AutoPtr<IIntent> intent;
    CIntent::New(IVpnConfig::SERVICE_INTERFACE, (IIntent**)&intent);
    String str;
    config->GetUser(&str);
    intent->SetClassName(mPackage, str);
    Int64 token = Binder::ClearCallingIdentity();

    // Restricted users are not allowed to create VPNs, they are tied to Owner
    Boolean bval, bval1;
    AutoPtr<IUserInfo> user;
    ECode ec = mgr->GetUserInfo(mUserHandle, (IUserInfo**)&user);
    if (FAILED(ec)) {
        Logger::E(TAG, "Cannot find %s", str.string());
        Binder::RestoreCallingIdentity(token);
        return E_SECURITY_EXCEPTION;
    }

    user->IsRestricted(&bval);
    if (bval || (mgr->HasUserRestriction(IUserManager::DISALLOW_CONFIG_VPN, &bval1), bval1)) {
        Logger::E(TAG, "Restricted users cannot establish VPNs");
        Binder::RestoreCallingIdentity(token);
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    AutoPtr<IResolveInfo> info;
    pm->ResolveService(intent, String(NULL), 0, mUserHandle, (IResolveInfo**)&info);
    if (FAILED(ec)) {
        Logger::E(TAG, "Cannot find %s", str.string());
        Binder::RestoreCallingIdentity(token);
        return E_SECURITY_EXCEPTION;
    }

    if (info == NULL) {
        Logger::E(TAG, "Cannot find %s", str.string());
        Binder::RestoreCallingIdentity(token);
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<IServiceInfo> si;
    info->GetServiceInfo((IServiceInfo**)&si);
    String permission;
    si->GetPermission(&permission);
    if (!Manifest::permission::BIND_VPN_SERVICE.Equals(permission)) {
        Logger::E(TAG, "%s  does not require %s", str.string(),
            Manifest::permission::BIND_VPN_SERVICE.string());
        Binder::RestoreCallingIdentity(token);
        return E_SECURITY_EXCEPTION;
    }

    Binder::RestoreCallingIdentity(token);

    // Save the old config in case we need to go back.
    AutoPtr<IVpnConfig> oldConfig = mConfig;
    String oldInterface = mInterface;
    AutoPtr<Connection> oldConnection = mConnection;
    AutoPtr<INetworkAgent> oldNetworkAgent = mNetworkAgent;
    mNetworkAgent = NULL;
    AutoPtr<IList> oldUsers = mVpnUsers;
    AutoPtr<IList> addresses;
    AutoPtr<IIterator> it;
    Boolean hasNext;
    StringBuilder builder;
    AutoPtr<Connection> connection;
    AutoPtr<IUserHandle> userHandle;
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    // Configure the interface. Abort if any of these steps fails.
    Int32 mtu, ival, fd;
    config->GetMtu(&mtu);
    String interfaze;

    NativeCreate(mtu, &ival);
    AutoPtr<IParcelFileDescriptorHelper> pfdHelper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfdHelper);
    AutoPtr<IParcelFileDescriptor> tun;
    pfdHelper->AdoptFd(ival, (IParcelFileDescriptor**)&tun);
    AutoPtr<IFileDescriptor> ifd;
    tun->GetFileDescriptor((IFileDescriptor**)&ifd);
    UpdateState(NetworkInfoDetailedState_CONNECTING, String("establish"));

    tun->GetFd(&fd);
    FAIL_GOTO(NativeGetName(fd, &interfaze), _EXIT_)

    // TEMP use the old jni calls until there is support for netd address setting
    config->GetAddresses((IList**)&addresses);
    addresses->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        // ILinkAddress* address = ILinkAddress::Probe(obj);
        builder.Append(" ");
        builder.Append(Object::ToString(obj)) ;
    }

    if (NativeSetAddresses(interfaze, builder.ToString()) < 1) {
        Logger::E(TAG, "At least one address must be specified");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    connection = new Connection(this);
    CUserHandle::New(mUserHandle, (IUserHandle**)&userHandle);
    ec = mContext->BindServiceAsUser(intent, connection.Get(),
        IContext::BIND_AUTO_CREATE, userHandle, &bval);
    FAIL_GOTO(ec, _EXIT_)

    if (!bval) {
        Logger::E(TAG, "Cannot bind %s", str.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mConnection = connection;
    mInterface = interfaze;

    // Fill more values.
    config->SetUser(mPackage);
    config->SetInterfaze(mInterface);
    config->SetStartTime(SystemClock::GetElapsedRealtime());
    mConfig = config;

    // Set up forwarding and DNS rules.
    mVpnUsers = NULL;
    CArrayList::New((IList**)&mVpnUsers);

    AgentConnect();

    if (oldConnection != NULL) {
        ec = mContext->UnbindService(oldConnection.Get());
        FAIL_GOTO(ec, _EXIT_)
    }
    // Remove the old tun's user forwarding rules
    // The new tun's user rules have already been added so they will take over
    // as rules are deleted. This prevents data leakage as the rules are moved over.
    AgentDisconnect(oldNetworkAgent);
    if (oldInterface != NULL && !oldInterface.Equals(interfaze)) {
        NativeReset(oldInterface);
    }

    config->GetBlocking(&bval);
    ec = ioUtils->SetBlocking(ifd, bval);
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::E(TAG, "Cannot set tunnel's fd as blocking=%d", bval);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

_EXIT_:
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        ioUtils->CloseQuietly(IFileDescriptor::Probe(tun));
        AgentDisconnect();
        // restore old state
        mConfig = oldConfig;
        mConnection = oldConnection;
        mVpnUsers = oldUsers;
        mNetworkAgent = oldNetworkAgent;
        mInterface = oldInterface;
        return ec;
    }

    Logger::I(TAG, "Established by %s on %s", str.string(), mInterface.string());
    *result = tun;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Boolean Vpn::IsRunningLocked()
{
    return mVpnUsers != NULL;
}

AutoPtr<ISortedSet> Vpn::GetAppsUids(
    /* [in] */ IList* packageNames,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<ISortedSet> uids;
    CTreeSet::New((ISortedSet**)&uids);
    AutoPtr<IIterator> it;
    packageNames->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String app = Object::ToString(obj);

        Int32 uid = GetAppUid(app, userHandle);

        if (uid != -1) {
            AutoPtr<IInteger32> iobj = CoreUtils::Convert(uid);
            uids->Add(iobj.Get());
        }
    }
    return uids;
}

ECode Vpn::AddVpnUserLocked(
    /* [in] */ Int32 userHandle)
{
    if (!IsRunningLocked()) {
        Logger::E(TAG, "VPN is not active");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IUidRangeHelper> helper;
    CUidRangeHelper::AcquireSingleton((IUidRangeHelper**)&helper);
    AutoPtr<IList> allowedList, disallowedList;
    mConfig->GetAllowedApplications((IList**)&allowedList);
    mConfig->GetDisallowedApplications((IList**)&disallowedList);
    if (allowedList != NULL) {
        // Add ranges covering all UIDs for allowedApplications.
        Int32 start = -1, stop = -1;

        AutoPtr<ISortedSet> set = GetAppsUids(allowedList, userHandle);
        AutoPtr<IIterator> it;
        set->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IInteger32* iobj = IInteger32::Probe(obj);
            Int32 uid;
            iobj->GetValue(&uid);

            if (start == -1) {
                start = uid;
            }
            else if (uid != stop + 1) {
                AutoPtr<IUidRange> ur;
                CUidRange::New(start, stop, (IUidRange**)&ur);
                mVpnUsers->Add(ur);
                start = uid;
            }
            stop = uid;
        }
        if (start != -1) {
            AutoPtr<IUidRange> ur;
            CUidRange::New(start, stop, (IUidRange**)&ur);
            mVpnUsers->Add(ur);
        }
    }
    else if (disallowedList != NULL) {
        // Add all ranges for user skipping UIDs for disallowedApplications.
        AutoPtr<IUidRange> userRange;
        helper->CreateForUser(userHandle, (IUidRange**)&userRange);
        Int32 start, stop;
        userRange->GetStart(&start);

        AutoPtr<ISortedSet> set = GetAppsUids(disallowedList, userHandle);
        AutoPtr<IIterator> it;
        set->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IInteger32* iobj = IInteger32::Probe(obj);
            Int32 uid;
            iobj->GetValue(&uid);

            if (uid == start) {
                start++;
            }
            else {
                AutoPtr<IUidRange> ur;
                CUidRange::New(start, uid - 1, (IUidRange**)&ur);
                mVpnUsers->Add(ur);
                start = uid + 1;
            }
        }

        userRange->GetStop(&stop);
        if (start <= stop) {
            AutoPtr<IUidRange> ur;
            CUidRange::New(start, stop, (IUidRange**)&ur);
            mVpnUsers->Add(ur);
        }
    }
    else {
        // Add all UIDs for the user.
        AutoPtr<IUidRange> userRange;
        helper->CreateForUser(userHandle, (IUidRange**)&userRange);
        mVpnUsers->Add(userRange);
    }

    return PrepareStatusIntent();
}

AutoPtr< ArrayOf<IUidRange*> > Vpn::UidRangesForUser(
    /* [in] */ Int32 userHandle,
    /* [out] */ IList** list)
{
    AutoPtr<IUidRange> userRange;
    AutoPtr<IUidRangeHelper> helper;
    CUidRangeHelper::AcquireSingleton((IUidRangeHelper**)&helper);
    helper->CreateForUser(userHandle, (IUidRange**)&userRange);

    AutoPtr<IList> ranges;
    CArrayList::New((IList**)&ranges);
    AutoPtr<IIterator> it;
    mVpnUsers->GetIterator((IIterator**)&it);
    Boolean hasNext;
    Int32 ls, le, rs, re;
    userRange->GetStart(&rs);
    userRange->GetStop(&re);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IUidRange* range = IUidRange::Probe(obj);
        range->GetStart(&ls);
        range->GetStop(&le);

        if (ls >= ls && le <= re) {
            ranges->Add(range);
        }
    }

    Int32 size;
    ranges->GetSize(&size);
    AutoPtr< ArrayOf<IUidRange*> > result = ArrayOf<IUidRange*>::Alloc(size);
    it = NULL;
    ranges->GetIterator((IIterator**)&it);
    Int32 i = 0;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IUidRange* range = IUidRange::Probe(obj);
        result->Set(i++, range);
    }

    if (list != NULL) {
        *list = ranges;
        REFCOUNT_ADD(*list)
    }

    return result;
}

ECode Vpn::RemoveVpnUserLocked(
    /* [in] */ Int32 userHandle)
{
    if (!IsRunningLocked()) {
        Logger::E(TAG, "VPN is not active");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IList> list;
    AutoPtr<ArrayOf<IUidRange*> > ranges = UidRangesForUser(userHandle, (IList**)&list);
    if (mNetworkAgent != NULL) {
        mNetworkAgent->RemoveUidRanges(ranges);
    }
    mVpnUsers->RemoveAll(ICollection::Probe(list));
    mStatusIntent = NULL;
    return NOERROR;
}

ECode Vpn::OnUserAdded(
    /* [in] */ Int32 userHandle)
{
    // If the user is restricted tie them to the owner's VPN
    synchronized(this) {
        AutoPtr<IUserManagerHelper> helper;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
        AutoPtr<IUserManager> mgr;
        helper->Get(mContext, (IUserManager**)&mgr);

        AutoPtr<IUserInfo> user;
        mgr->GetUserInfo(userHandle, (IUserInfo**)&user);
        Boolean bval;
        if (user->IsRestricted(&bval), bval) {
            ECode ec = AddVpnUserLocked(userHandle);
            if (FAILED(ec)) {
                Logger::E(TAG, "Failed to add restricted user to owner");
                return ec;
            }
            else {
                if (mNetworkAgent != NULL) {
                    AutoPtr<ArrayOf<IUidRange*> > ranges = UidRangesForUser(userHandle, NULL);
                    mNetworkAgent->AddUidRanges(ranges);
                }
            }
        }
    }
    return NOERROR;
}

ECode Vpn::OnUserRemoved(
    /* [in] */ Int32 userHandle)
{
    // clean up if restricted
    ECode ec = NOERROR;
    synchronized(this) {
        AutoPtr<IUserManagerHelper> helper;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
        AutoPtr<IUserManager> mgr;
        helper->Get(mContext, (IUserManager**)&mgr);

        AutoPtr<IUserInfo> user;
        mgr->GetUserInfo(userHandle, (IUserInfo**)&user);
        Boolean bval;
        if (user->IsRestricted(&bval), bval) {
            ec = RemoveVpnUserLocked(userHandle);
            if (FAILED(ec)) {
                Logger::E(TAG, "Failed to remove restricted user to owner");
            }
        }
    }
    return ec;
}

AutoPtr<IVpnConfig> Vpn::GetVpnConfig()
{
    EnforceControlPermission();
    return mConfig;
}

ECode Vpn::InterfaceStatusChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    AutoLock lock(mLock);
    return mObserver->InterfaceStatusChanged(iface, up);
}

ECode Vpn::EnforceControlPermission()
{
    // System user is allowed to control VPN.
    if (Binder::GetCallingUid() == IProcess::SYSTEM_UID) {
       return NOERROR;
    }

    // // System dialogs are also allowed to control VPN.
    // AutoPtr<IPackageManager> pm;
    // mContext->GetPackageManager((IPackageManager**)&pm);
    // AutoPtr<IApplicationInfo> app;
    // pm->GetApplicationInfo(IVpnConfig::DIALOGS_PACKAGE, 0, (IApplicationInfo**)&app);
    // Int32 uid;
    // app->GetUid(&uid);
    // if (Binder::GetCallingUid() == uid) {
    //     return NOERROR;
    // }

    Int32 appId = UserHandle::GetAppId(Binder::GetCallingUid());
    Int64 token = Binder::ClearCallingIdentity();

    // System VPN dialogs are also allowed to control VPN.
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IApplicationInfo> app;
    pm->GetApplicationInfo(IVpnConfig::DIALOGS_PACKAGE, 0, (IApplicationInfo**)&app);
    Int32 flags, uid;
    app->GetFlags(&flags);
    app->GetUid(&uid);
    if (((flags & IApplicationInfo::FLAG_SYSTEM) != 0) && (appId == uid)) {
        Binder::RestoreCallingIdentity(token);
        return NOERROR;
    }

    // SystemUI dialogs are also allowed to control VPN.
    AutoPtr<IApplicationInfo> sysUiApp;
    pm->GetApplicationInfo(String("Elastos.Droid.SystemUI"), 0, (IApplicationInfo**)&sysUiApp);
    app->GetFlags(&flags);
    app->GetUid(&uid);
    if (((flags & IApplicationInfo::FLAG_SYSTEM) != 0) && (appId == uid)) {
        Binder::RestoreCallingIdentity(token);
        return NOERROR;
    }

    Logger::E(TAG, "Unauthorized Caller");
    return E_SECURITY_EXCEPTION;
}

ECode Vpn::PrepareStatusIntent()
{
    Int64 token = Binder::ClearCallingIdentity();
    AutoPtr<IVpnConfigHelper> helper;
    CVpnConfigHelper::AcquireSingleton((IVpnConfigHelper**)&helper);

    mStatusIntent = NULL;
    helper->GetIntentForStatusPanel(mContext, (IPendingIntent**)&mStatusIntent);

    Binder::RestoreCallingIdentity(token);

    return NOERROR;
}

Boolean Vpn::AddAddress(
    /* [in] */ const String& address,
    /* [in] */ Int32 prefixLength)
{
    AutoLock lock(this);
    if (Binder::GetCallingUid() != mOwnerUID || mInterface == NULL || mNetworkAgent == NULL) {
        return FALSE;
    }
    if (address.IsNull() || mInterface.IsNull()) {
        Logger::E(TAG, "AddAddress:: E_ILLEGAL_ARGUMENT_EXCEPTION");
        return FALSE;
    }
    Boolean success = NativeAddAddress(mInterface, address, prefixLength);
    if (mNetworkAgent != NULL) {
        mNetworkAgent->SendLinkProperties(MakeLinkProperties());
    }
    return success;
}

Boolean Vpn::RemoveAddress(
    /* [in] */ const String& address,
    /* [in] */ Int32 prefixLength)
{
    AutoLock lock(this);
    if (Binder::GetCallingUid() != mOwnerUID || mInterface == NULL || mNetworkAgent == NULL) {
        return FALSE;
    }
    if (address.IsNull() || mInterface.IsNull()) {
        Logger::E(TAG, "AddAddress:: E_ILLEGAL_ARGUMENT_EXCEPTION");
        return FALSE;
    }
    Boolean success = NativeDelAddress(mInterface, address, prefixLength);
    if (mNetworkAgent != NULL) {
        mNetworkAgent->SendLinkProperties(MakeLinkProperties());
    }
    return success;
}

#define SYSTEM_ERROR -1
#define BAD_ARGUMENT -2

static Int32 create_interface(Int32 mtu)
{
    Int32 tun = open("/dev/tun", O_RDWR | O_NONBLOCK);

    ifreq ifr4;
    memset(&ifr4, 0, sizeof(ifr4));

    // Allocate interface.
    ifr4.ifr_flags = IFF_TUN | IFF_NO_PI;
    if (ioctl(tun, TUNSETIFF, &ifr4)) {
        Logger::E("Vpn", "Cannot allocate TUN: %s", strerror(errno));
        goto error;
    }

    // Activate interface.
    ifr4.ifr_flags = IFF_UP;
    if (ioctl(inet4, SIOCSIFFLAGS, &ifr4)) {
        Logger::E("Vpn", "Cannot activate %s: %s", ifr4.ifr_name, strerror(errno));
        goto error;
    }

    // Set MTU if it is specified.
    ifr4.ifr_mtu = mtu;
    if (mtu > 0 && ioctl(inet4, SIOCSIFMTU, &ifr4)) {
        Logger::E("Vpn", "Cannot set MTU on %s: %s", ifr4.ifr_name, strerror(errno));
        goto error;
    }

    return tun;

error:
    close(tun);
    return SYSTEM_ERROR;
}

static Int32 get_interface_name(char *name, Int32 tun)
{
    ifreq ifr4;
    if (ioctl(tun, TUNGETIFF, &ifr4)) {
        Logger::E("Vpn", "Cannot get interface name: %s", strerror(errno));
        return SYSTEM_ERROR;
    }
    strncpy(name, ifr4.ifr_name, IFNAMSIZ);
    return 0;
}

static Int32 get_interface_index(const char *name)
{
    ifreq ifr4;
    strncpy(ifr4.ifr_name, name, IFNAMSIZ);
    if (ioctl(inet4, SIOGIFINDEX, &ifr4)) {
        Logger::E("Vpn", "Cannot get index of %s: %s", name, strerror(errno));
        return SYSTEM_ERROR;
    }
    return ifr4.ifr_ifindex;
}

static Int32 set_addresses(const char *name, const char *addresses)
{
    Int32 index = get_interface_index(name);
    if (index < 0) {
        return index;
    }

    ifreq ifr4;
    memset(&ifr4, 0, sizeof(ifr4));
    strncpy(ifr4.ifr_name, name, IFNAMSIZ);
    ifr4.ifr_addr.sa_family = AF_INET;
    ifr4.ifr_netmask.sa_family = AF_INET;

    in6_ifreq ifr6;
    memset(&ifr6, 0, sizeof(ifr6));
    ifr6.ifr6_ifindex = index;

    char address[65];
    Int32 prefix;
    Int32 chars;
    Int32 count = 0;

    while (sscanf(addresses, " %64[^/]/%d %n", address, &prefix, &chars) == 2) {
        addresses += chars;

        if (strchr(address, ':')) {
            // Add an IPv6 address.
            if (inet_pton(AF_INET6, address, &ifr6.ifr6_addr) != 1 ||
                    prefix < 0 || prefix > 128) {
                count = BAD_ARGUMENT;
                break;
            }

            ifr6.ifr6_prefixlen = prefix;
            if (ioctl(inet6, SIOCSIFADDR, &ifr6)) {
                count = (errno == EINVAL) ? BAD_ARGUMENT : SYSTEM_ERROR;
                break;
            }
        } else {
            // Add an IPv4 address.
            if (inet_pton(AF_INET, address, as_in_addr(&ifr4.ifr_addr)) != 1 ||
                    prefix < 0 || prefix > 32) {
                count = BAD_ARGUMENT;
                break;
            }

            if (count) {
                sprintf(ifr4.ifr_name, "%s:%d", name, count);
            }
            if (ioctl(inet4, SIOCSIFADDR, &ifr4)) {
                count = (errno == EINVAL) ? BAD_ARGUMENT : SYSTEM_ERROR;
                break;
            }

            in_addr_t mask = prefix ? (~0 << (32 - prefix)) : 0;
            *as_in_addr(&ifr4.ifr_netmask) = htonl(mask);
            if (ioctl(inet4, SIOCSIFNETMASK, &ifr4)) {
                count = (errno == EINVAL) ? BAD_ARGUMENT : SYSTEM_ERROR;
                break;
            }
        }
        Logger::D("Vpn", "Address added on %s: %s/%d", name, address, prefix);
        ++count;
    }

    if (count == BAD_ARGUMENT) {
        Logger::E("Vpn", "Invalid address: %s/%d", address, prefix);
    } else if (count == SYSTEM_ERROR) {
        Logger::E("Vpn", "Cannot add address: %s/%d: %s", address, prefix, strerror(errno));
    } else if (*addresses) {
        Logger::E("Vpn", "Invalid address: %s", addresses);
        count = BAD_ARGUMENT;
    }

    return count;
}

// static Int32 set_routes(const char *name, const char *routes)
// {
//     Int32 index = get_interface_index(name);
//     if (index < 0) {
//         return index;
//     }

//     rtentry rt4;
//     memset(&rt4, 0, sizeof(rt4));
//     rt4.rt_dev = (char *)name;
//     rt4.rt_flags = RTF_UP;
//     rt4.rt_dst.sa_family = AF_INET;
//     rt4.rt_genmask.sa_family = AF_INET;

//     in6_rtmsg rt6;
//     memset(&rt6, 0, sizeof(rt6));
//     rt6.rtmsg_ifindex = index;
//     rt6.rtmsg_flags = RTF_UP;

//     char address[65];
//     Int32 prefix;
//     Int32 chars;
//     Int32 count = 0;

//     while (sscanf(routes, " %64[^/]/%d %n", address, &prefix, &chars) == 2) {
//         routes += chars;

//         if (strchr(address, ':')) {
//             // Add an IPv6 route.
//             if (inet_pton(AF_INET6, address, &rt6.rtmsg_dst) != 1 ||
//                     prefix < 0 || prefix > 128) {
//                 count = BAD_ARGUMENT;
//                 break;
//             }

//             rt6.rtmsg_dst_len = prefix ? prefix : 1;
//             if (ioctl(inet6, SIOCADDRT, &rt6) && errno != EEXIST) {
//                 count = (errno == EINVAL) ? BAD_ARGUMENT : SYSTEM_ERROR;
//                 break;
//             }

//             if (!prefix) {
//                 // Split the route instead of replacing the default route.
//                 rt6.rtmsg_dst.s6_addr[0] ^= 0x80;
//                 if (ioctl(inet6, SIOCADDRT, &rt6) && errno != EEXIST) {
//                     count = SYSTEM_ERROR;
//                     break;
//                 }
//             }
//         } else {
//             // Add an IPv4 route.
//             if (inet_pton(AF_INET, address, as_in_addr(&rt4.rt_dst)) != 1 ||
//                     prefix < 0 || prefix > 32) {
//                 count = BAD_ARGUMENT;
//                 break;
//             }

//             in_addr_t mask = prefix ? (~0 << (32 - prefix)) : 0x80000000;
//             *as_in_addr(&rt4.rt_genmask) = htonl(mask);
//             if (ioctl(inet4, SIOCADDRT, &rt4) && errno != EEXIST) {
//                 count = (errno == EINVAL) ? BAD_ARGUMENT : SYSTEM_ERROR;
//                 break;
//             }

//             if (!prefix) {
//                 // Split the route instead of replacing the default route.
//                 *as_in_addr(&rt4.rt_dst) ^= htonl(0x80000000);
//                 if (ioctl(inet4, SIOCADDRT, &rt4) && errno != EEXIST) {
//                     count = SYSTEM_ERROR;
//                     break;
//                 }
//             }
//         }
//         Logger::D("Vpn", "Route added on %s: %s/%d", name, address, prefix);
//         ++count;
//     }

//     if (count == BAD_ARGUMENT) {
//         Logger::E("Vpn", "Invalid route: %s/%d", address, prefix);
//     } else if (count == SYSTEM_ERROR) {
//         Logger::E("Vpn", "Cannot add route: %s/%d: %s",
//                 address, prefix, strerror(errno));
//     } else if (*routes) {
//         Logger::E("Vpn", "Invalid route: %s", routes);
//         count = BAD_ARGUMENT;
//     }

//     return count;
// }

static Int32 reset_interface(const char *name)
{
    ifreq ifr4;
    strncpy(ifr4.ifr_name, name, IFNAMSIZ);
    ifr4.ifr_flags = 0;

    if (ioctl(inet4, SIOCSIFFLAGS, &ifr4) && errno != ENODEV) {
        Logger::E("Vpn", "Cannot reset %s: %s", name, strerror(errno));
        return SYSTEM_ERROR;
    }
    return 0;
}

static Int32 check_interface(const char *name)
{
    ifreq ifr4;
    strncpy(ifr4.ifr_name, name, IFNAMSIZ);
    ifr4.ifr_flags = 0;

    if (ioctl(inet4, SIOCGIFFLAGS, &ifr4) && errno != ENODEV) {
        Logger::E("Vpn", "Cannot check %s: %s", name, strerror(errno));
    }
    return ifr4.ifr_flags;
}

// static Int32 bind_to_interface(Int32 socket, const char *name)
// {
//     if (setsockopt(socket, SOL_SOCKET, SO_BINDTODEVICE, name, strlen(name))) {
//         Logger::E("Vpn", "Cannot bind socket to %s: %s", name, strerror(errno));
//         return SYSTEM_ERROR;
//     }
//     return 0;
// }

static bool modifyAddress(const char * name, const char * address, Int32 jPrefixLength, bool add)
{
    int error = SYSTEM_ERROR;

    if (add) {
        error = ifc_add_address(name, address, jPrefixLength);
        if (error == 0) {
            ALOGE("Cannot add address %s/%d on interface %s (%s)", address, jPrefixLength, name,
                  strerror(-error));
        }
    } else {
        error = ifc_del_address(name, address, jPrefixLength);
        if (error == 0) {
            ALOGE("Cannot del address %s/%d on interface %s (%s)", address, jPrefixLength, name,
                  strerror(-error));
        }
    }

    return !error;
}

ECode Vpn::NativeCreate(
    /* [in] */ Int32 mtu,
    /* [out] */ Int32* fd)
{
    Int32 tun = create_interface(mtu);
    if (tun < 0) {
        // throwException(env, tun, "Cannot create interface");
        *fd = -1;
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *fd = tun;
    return NOERROR;
}

ECode Vpn::NativeGetName(
    /* [in] */ Int32 tun,
    /* [out] */ String* vpnName)
{
    char name[IFNAMSIZ];
    if (get_interface_name(name, tun) < 0) {
        // throwException(env, SYSTEM_ERROR, "Cannot get interface name");
        *vpnName = NULL;
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *vpnName = name;
    return NOERROR;
}

Int32 Vpn::NativeSetAddresses(
    /* [in] */ const String& interfaze,
    /* [in] */ const String& addresses)
{
    Int32 count = -1;

    if (interfaze.IsNull() || addresses.IsNull()) return count;

    count = set_addresses(interfaze.string(), addresses.string());
    if (count < 0) count = -1;
    return count;
}

ECode Vpn::NativeReset(
    /* [in] */ const String& interfaze)
{
    if (interfaze.IsNull()) return E_NULL_POINTER_EXCEPTION;
    if (reset_interface(interfaze.string()) < 0) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

Int32 Vpn::NativeCheck(
    /* [in] */ const String& interfaze)
{
    const char *name = interfaze.string();
    Int32 flags = check_interface(name);
    return flags;
}

Boolean Vpn::NativeAddAddress(
    /* [in] */ const String& interfaze,
    /* [in] */ const String& addresses,
    /* [in] */ Int32 prefixLength)
{
    return modifyAddress(interfaze.string(), addresses.string(), prefixLength, TRUE);
}

Boolean Vpn::NativeDelAddress(
    /* [in] */ const String& interfaze,
    /* [in] */ const String& addresses,
    /* [in] */ Int32 prefixLength)
{
    return modifyAddress(interfaze.string(), addresses.string(), prefixLength, FALSE);
}

ECode Vpn::FindIPv4DefaultRoute(
    /* [in] */ ILinkProperties* prop,
    /* [out] */ IRouteInfo** routeInfo)
{
    VALIDATE_NOT_NULL(routeInfo);
    *routeInfo = NULL;

    AutoPtr<IList> routes;
    prop->GetRoutes((IList**)&routes);
    AutoPtr<IIterator> it;
    routes->GetIterator((IIterator**)&it);
    Boolean hasNext, bval;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IRouteInfo* route = IRouteInfo::Probe(obj);

        // Currently legacy VPN only works on IPv4.
        route->IsDefaultRoute(&bval);
        AutoPtr<IInetAddress> addr;
        route->GetGateway((IInetAddress**)&addr);
        if (bval && IInet4Address::Probe(addr)) {
            *routeInfo = route;
            REFCOUNT_ADD(*routeInfo)
            return NOERROR;
        }
    }

    Logger::E(TAG, "Unable to find IPv4 default gateway");
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode Vpn::StartLegacyVpn(
    /* [in] */ IVpnProfile* profile,
    /* [in] */ IKeyStore* keyStore,
    /* [in] */ ILinkProperties* egress)
{
    EnforceControlPermission();
    Boolean bval, bval2;
    keyStore->IsUnlocked(&bval);
    if (!bval) {
        Logger::E(TAG, "KeyStore isn't unlocked");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    AutoPtr<IUserManager> mgr;
    helper->Get(mContext, (IUserManager**)&mgr);
    AutoPtr<IUserInfo> user;
    mgr->GetUserInfo(mUserHandle, (IUserInfo**)&user);
    user->IsRestricted(&bval);
    if (bval || (mgr->HasUserRestriction(IUserManager::DISALLOW_CONFIG_VPN, &bval2))) {
        Logger::E(TAG, "Restricted users cannot establish VPNs");
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<IRouteInfo> ipv4DefaultRoute;
    FindIPv4DefaultRoute(egress, (IRouteInfo**)&ipv4DefaultRoute);
    AutoPtr<IInetAddress> gatewayAddr;
    ipv4DefaultRoute->GetGateway((IInetAddress**)&gatewayAddr);
    String gateway, iface;
    gatewayAddr->GetHostAddress(&gateway);
    ipv4DefaultRoute->GetInterface(&iface);

    // Load certificates.
    String privateKey("");
    String userCert("");
    String caCert("");
    String serverCert("");
    String ipsecSecret, ipsecCaCert, ipsecServerCert;

    profile->GetIpsecUserCert(&ipsecSecret);
    if (!ipsecSecret.IsEmpty()) {
        privateKey = ICredentials::USER_PRIVATE_KEY + ipsecSecret;
        String key(ICredentials::USER_CERTIFICATE);
        key += ipsecSecret;
        AutoPtr<ArrayOf<Byte> > value;
        keyStore->Get(key, (ArrayOf<Byte>**)&value);
        if (value == NULL) {
            userCert = String(NULL);
        }
        else {
            String str(*value);
            userCert =str;
        }
    }

   profile->GetIpsecCaCert(&ipsecCaCert);
   if (!ipsecCaCert.IsEmpty()) {
        String key(ICredentials::CA_CERTIFICATE);
        key += ipsecCaCert;
        AutoPtr<ArrayOf<Byte> > value;
        keyStore->Get(key, (ArrayOf<Byte>**)&value);
        if (value == NULL) {
            caCert = String(NULL);
        }
        else {
            String str(*value);
            caCert =str;
        }
   }

   profile->GetIpsecServerCert(&ipsecServerCert);
   if (!ipsecServerCert.IsEmpty()) {
        String key(ICredentials::USER_CERTIFICATE);
        key += ipsecServerCert;
        AutoPtr<ArrayOf<Byte> > value;
        keyStore->Get(key, (ArrayOf<Byte>**)&value);
        if (value == NULL) {
            serverCert = String(NULL);
        }
        else {
            String str(*value);
            serverCert =str;
        }
   }

   if (privateKey == NULL || userCert == NULL || caCert == NULL || serverCert == NULL) {
       Logger::E(TAG, "Cannot load credentials");
       return E_ILLEGAL_STATE_EXCEPTION;
   }

   // Prepare arguments for racoon.
   AutoPtr<ArrayOf<String> > racoon;
   Int32 type;
   profile->GetType(&type);
   String server, ipsecIdentifier, username, password, l2tpSecret, name;
   profile->GetServer(&server);
   profile->GetIpsecIdentifier(&ipsecIdentifier);
   profile->GetL2tpSecret(&l2tpSecret);
   profile->GetUsername(&username);
   profile->GetName(&name);
   profile->GetPassword(&password);
   switch (type) {
       case IVpnProfile::TYPE_L2TP_IPSEC_PSK: {
                racoon = ArrayOf<String>::Alloc(6);
                racoon->Set(0, iface);
                racoon->Set(1, server);
                racoon->Set(2, String("udppsk"));
                racoon->Set(3, ipsecIdentifier);
                racoon->Set(4, ipsecSecret);
                racoon->Set(5, String("1701"));
            };
            break;

       case IVpnProfile::TYPE_L2TP_IPSEC_RSA: {
                racoon = ArrayOf<String>::Alloc(8);
                racoon->Set(0, iface);
                racoon->Set(1, server);
                racoon->Set(2, String("udprsa"));
                racoon->Set(3, privateKey);
                racoon->Set(4, userCert);
                racoon->Set(5, caCert);
                racoon->Set(6, serverCert);
                racoon->Set(7, String("1701"));
            };
            break;

       case IVpnProfile::TYPE_IPSEC_XAUTH_PSK: {
                racoon = ArrayOf<String>::Alloc(10);
                racoon->Set(0, iface);
                racoon->Set(1, server);
                racoon->Set(2, String("xauthpsk"));
                racoon->Set(3, ipsecIdentifier);
                racoon->Set(4, ipsecSecret);
                racoon->Set(5, username);
                racoon->Set(6, password);
                racoon->Set(7, String(""));
                racoon->Set(8, gateway);
                racoon->Set(9, String("1701"));
            };
            break;

       case IVpnProfile::TYPE_IPSEC_XAUTH_RSA: {
                racoon = ArrayOf<String>::Alloc(12);
                racoon->Set(0, iface);
                racoon->Set(1, server);
                racoon->Set(2, String("xauthrsa"));
                racoon->Set(3, privateKey);
                racoon->Set(4, userCert);
                racoon->Set(5, caCert);
                racoon->Set(6, serverCert);
                racoon->Set(7, username);
                racoon->Set(8, password);
                racoon->Set(9, String(""));
                racoon->Set(10, gateway);
                racoon->Set(11, String("1701"));
            };
            break;

       case IVpnProfile::TYPE_IPSEC_HYBRID_RSA: {
                racoon = ArrayOf<String>::Alloc(10);
                racoon->Set(0, iface);
                racoon->Set(1, server);
                racoon->Set(2, String("hybridrsa"));
                racoon->Set(3, caCert);
                racoon->Set(4, serverCert);
                racoon->Set(5, username);
                racoon->Set(6, password);
                racoon->Set(7, String(""));
                racoon->Set(8, gateway);
                racoon->Set(9, String("1701"));
            };
            break;
   }

    // Prepare arguments for mtpd.
   Boolean mppe;
   profile->GetMppe(&mppe);
    AutoPtr<ArrayOf<String> > mtpd;
    switch (type) {
        case IVpnProfile::TYPE_PPTP: {
                mtpd = ArrayOf<String>::Alloc(20);
                mtpd->Set(0, iface);
                mtpd->Set(1, String("pptp"));
                mtpd->Set(2, server);
                mtpd->Set(3, String("1723"));
                mtpd->Set(4, String("name"));
                mtpd->Set(5, username);
                mtpd->Set(6, String("password"));
                mtpd->Set(7, password);
                mtpd->Set(8, String("linkname")),
                mtpd->Set(9, String("vpn"));
                mtpd->Set(10, String("refuse-eap"));
                mtpd->Set(11, String("nodefaultroute"));
                mtpd->Set(12, String("usepeerdns"));
                mtpd->Set(13, String("idle"));
                mtpd->Set(14, String("1800"));
                mtpd->Set(15, String("mtu"));
                mtpd->Set(16, String("1400"));
                mtpd->Set(17, String("mru"));
                mtpd->Set(18, String("1400"));
                mtpd->Set(19, mppe ? String("+mppe") : String("nomppe"));
            };
            break;

        case IVpnProfile::TYPE_L2TP_IPSEC_PSK:
        case IVpnProfile::TYPE_L2TP_IPSEC_RSA: {
                mtpd = ArrayOf<String>::Alloc(20);
                mtpd->Set(0, iface);
                mtpd->Set(1, String("l2tp"));
                mtpd->Set(2, server);
                mtpd->Set(3, String("1701"));
                mtpd->Set(4, l2tpSecret);
                mtpd->Set(5, String("name"));
                mtpd->Set(6, username);
                mtpd->Set(7, String("password"));
                mtpd->Set(8, password);
                mtpd->Set(9, String("linkname")),
                mtpd->Set(10, String("vpn"));
                mtpd->Set(11, String("refuse-eap"));
                mtpd->Set(12, String("nodefaultroute"));
                mtpd->Set(13, String("usepeerdns"));
                mtpd->Set(14, String("idle"));
                mtpd->Set(15, String("1800"));
                mtpd->Set(16, String("mtu"));
                mtpd->Set(17, String("1400"));
                mtpd->Set(18, String("mru"));
                mtpd->Set(19, String("1400"));
            };
           break;
    }

    String routes;
    profile->GetRoutes(&routes);

    AutoPtr<IVpnConfig> config;
    CVpnConfig::New((IVpnConfig**)&config);
    config->SetLegacy(TRUE);
    String key;
    profile->GetKey(&key);
    config->SetUser(key);
    config->SetInterfaze(iface);
    config->SetSession(name);
    config->AddLegacyRoutes(routes);

    String strVal;
    profile->GetDnsServers(&strVal);
    if (!strVal.IsEmpty()) {
        AutoPtr<ArrayOf<String> > splits;
        StringUtils::Split(strVal, " +", (ArrayOf<String>**)&splits);
        AutoPtr<IList> list;
        CArrayList::New((IList**)&list);
        for (Int32 i = 0; i < splits->GetLength(); ++i) {
            AutoPtr<ICharSequence> csq = CoreUtils::Convert((*splits)[i]);
            list->Add(csq.Get());
        }
        config->SetDnsServers(list);
    }


    profile->GetDnsServers(&strVal);
    if (!strVal.IsEmpty()) {
        AutoPtr<ArrayOf<String> > splits;
        StringUtils::Split(strVal, " +", (ArrayOf<String>**)&splits);
        AutoPtr<IList> list;
        CArrayList::New((IList**)&list);
        for (Int32 i = 0; i < splits->GetLength(); ++i) {
            AutoPtr<ICharSequence> csq = CoreUtils::Convert((*splits)[i]);
            list->Add(csq.Get());
        }
        config->SetSearchDomains(list);
    }

    StartLegacyVpn(config, racoon, mtpd);
    return NOERROR;
}

void Vpn::StartLegacyVpn(
    /* [in] */ IVpnConfig* config,
    /* [in] */ ArrayOf<String>* racoon,
    /* [in] */ ArrayOf<String>* mtpd)
{
    AutoLock lock(mLock);
    StopLegacyVpn();

    // Prepare for the new request. This also checks the caller.
    Boolean result;
    Prepare(String(NULL), IVpnConfig::LEGACY_VPN, &result);
    UpdateState(NetworkInfoDetailedState_CONNECTING, String("startLegacyVpn"));

    // Start a new LegacyVpnRunner and we are done!
    mLegacyVpnRunner = new LegacyVpnRunner();
    mLegacyVpnRunner->constructor(config, racoon, mtpd, this);
    mLegacyVpnRunner->Start();
}

ECode Vpn::StopLegacyVpn()
{
    AutoLock lock(mLock);
    if (mLegacyVpnRunner != NULL) {
        mLegacyVpnRunner->Exit();
        mLegacyVpnRunner = NULL;

        {
            AutoLock lock(LegacyVpnRunner::sTAGLock);
            // wait for old thread to completely finish before spinning up
            // new instance, otherwise state updates can be out of order.
        }
    }
    return NOERROR;
}

ECode Vpn::GetLegacyVpnInfo(
    /* [out] */ ILegacyVpnInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    AutoLock lock(mLock);
    // Check if the caller is authorized.
    FAIL_RETURN(EnforceControlPermission());
    if (mLegacyVpnRunner == NULL) {
        return NOERROR;
    }

    CLegacyVpnInfo::New(info);
    String user;
    mConfig->GetUser(&user);
    (*info)->SetKey(user);

    AutoPtr<ILegacyVpnInfoHelper> helper;
    CLegacyVpnInfoHelper::AcquireSingleton((ILegacyVpnInfoHelper**)&helper);
    Int32 state;
    helper->StateFromNetworkInfo(mNetworkInfo, &state);
    (*info)->SetState(state);

    Boolean result;
    if (mNetworkInfo->IsConnected(&result), result) {
       (*info)->SetIntent(mStatusIntent);
    }
    return NOERROR;
}

ECode Vpn::GetLegacyVpnConfig(
    /* [out] */ IVpnConfig** config)
{
    VALIDATE_NOT_NULL(config);
    *config = NULL;

    if (mLegacyVpnRunner != NULL) {
        *config = mConfig;
        REFCOUNT_ADD(*config);
    }

    return NOERROR;
}

} // namespace Connectivity
} // namespace Server
} // namespace Droid
} // namespace Elastos
