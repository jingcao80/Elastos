
#include "CWifiDisplayManagerService.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::ServiceManager;

namespace Elastos {
namespace Droid {
namespace Server {

//===================================================================================
//          CWifiDisplayManagerService::ThreadDeathRecipient
//===================================================================================
CAR_INTERFACE_IMPL(CWifiDisplayManagerService::ThreadDeathRecipient, IProxyDeathRecipient)

CWifiDisplayManagerService::ThreadDeathRecipient::ThreadDeathRecipient(
    /* [in] */ CWifiDisplayManagerService* host,
    /* [in] */ IWifiDisplayThread* client)
    : mHost(host)
{
    mClient         = client;
    mClientBinder   = IBinder::Probe(client);
}

ECode CWifiDisplayManagerService::ThreadDeathRecipient::ProxyDied()
{
    AutoLock lock(mHost->_m_syncLock);
    Int32 ival;
    mHost->RemoveWifiDisplayClient(mClient, &ival);

    return NOERROR;
}

//===================================================================================
//          CWifiDisplayManagerService::WifiDisplayClient
//===================================================================================
const Int32 CWifiDisplayManagerService::WifiDisplayClient::WIFIDISPLAY_CLIENT_NONE       = 0;
const Int32 CWifiDisplayManagerService::WifiDisplayClient::WIFIDISPLAY_CLIENT_SENDING    = 1;
const Int32 CWifiDisplayManagerService::WifiDisplayClient::WIFIDISPLAY_CLIENT_RECEIVEING = 2;

CWifiDisplayManagerService::WifiDisplayClient::WifiDisplayClient(
    /* [in] */ CWifiDisplayManagerService* host,
    /* [in] */ IWifiDisplayThread* client)
    : mHost(host)
{
    mDeathRecipient = new ThreadDeathRecipient(mHost, client);

    mClientBinder       = IBinder::Probe(client);
    mClient             = client;
    mStatus             = WIFIDISPLAY_CLIENT_NONE;
    // try
    // {
        AutoPtr<IProxy> proxy = (IProxy*)mClient->Probe(EIID_IProxy);
        proxy->LinkToDeath(mDeathRecipient, 0);
    // }
    // catch (RemoteException e)
    // {

    // }
}

void CWifiDisplayManagerService::WifiDisplayClient::SetWifiDisplayClientStatus(
    /* [in] */ Int32 status)
{
    mStatus = status;
}

Int32 CWifiDisplayManagerService::WifiDisplayClient::GetWifiDisplayClientStatus()
{
    return mStatus;
}

AutoPtr<IWifiDisplayThread> CWifiDisplayManagerService::WifiDisplayClient::GetWifiDisplayClient()
{
    return mClient;
}

//===================================================================================
//          CWifiDisplayManagerService
//===================================================================================
const String CWifiDisplayManagerService::TAG("WifiDisplayManagerService");
const Boolean CWifiDisplayManagerService::LOCAL_LOGV = FALSE;
const Boolean CWifiDisplayManagerService::DEBUG_CLIENT = FALSE;

ECode CWifiDisplayManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIPowerManager* powerManagerService)
{
    Logger::D(TAG, "WifiDisplayManagerService Starting.......!");

    mContext = context;
    mPM      = powerManagerService;

    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::WINDOW_SERVICE);
    mWindowManager = IIWindowManager::Probe(service.Get());
    return NOERROR;
}

AutoPtr<CWifiDisplayManagerService::WifiDisplayClient> CWifiDisplayManagerService::ContainsWifiDisplayThread(
    /* [in] */ IWifiDisplayThread* client)
{
    AutoPtr<IBinder> binder = IBinder::Probe(client);
    AutoPtr<WifiDisplayClient> curClient;
    AutoPtr<IWifiDisplayThread> displayThread;
    List<AutoPtr<WifiDisplayClient> >::Iterator it = mClients.Begin();
    for (; it != mClients.End(); ++it)  {
        curClient = *it;
        displayThread = curClient->GetWifiDisplayClient();

        if (IBinder::Probe(displayThread.Get()) == binder) {
            return curClient;
        }
    }

    return NULL;
}

ECode CWifiDisplayManagerService::AddWifiDisplayClient(
    /* [in] */ IWifiDisplayThread* client,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<WifiDisplayClient> curClient = ContainsWifiDisplayThread(client);

    if (DEBUG_CLIENT) {
        Slogger::D(TAG, "AddWifiDisplayClient find = %d, IWifiDisplayThread = %p",
            curClient != NULL, client);
    }

    if (curClient == NULL) {
        AutoPtr<WifiDisplayClient> displayClient = new WifiDisplayClient(this, client);
        mClients.PushBack(displayClient);
    }

    return NOERROR;
}

ECode CWifiDisplayManagerService::RemoveWifiDisplayClient(
    /* [in] */ IWifiDisplayThread* client,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    AutoPtr<WifiDisplayClient> curClient = ContainsWifiDisplayThread(client);

    if (curClient != NULL) {
        if (DEBUG_CLIENT) {
            Slogger::D(TAG, "RemoveWifiDisplayClient find =  %d, IWifiDisplayThread = %p",
                curClient != NULL, client);
        }

        mClients.Remove(curClient);
        *result = 0;
    }
    return NOERROR;
}

ECode CWifiDisplayManagerService::StartWifiDisplaySend(
    /* [in] */ IWifiDisplayThread* client,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    AutoPtr<WifiDisplayClient> curClient = ContainsWifiDisplayThread(client);

    if (DEBUG_CLIENT) {
        Slogger::D(TAG, "StartWifiDisplaySend find = %d, IWifiDisplayThread = %p",
            curClient != NULL, client);
    }

    if (curClient != NULL) {
        curClient->SetWifiDisplayClientStatus(WifiDisplayClient::WIFIDISPLAY_CLIENT_SENDING);
        *result = 0;
    }

    return NOERROR;
}

ECode CWifiDisplayManagerService::EndWifiDisplaySend(
    /* [in] */ IWifiDisplayThread* client,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    AutoPtr<WifiDisplayClient> curClient = ContainsWifiDisplayThread(client);

    Slogger::D(TAG, "EndWifiDisplaySend mClients.size() = %d", mClients.GetSize());

    if (DEBUG_CLIENT) {
        Slogger::D(TAG, "EndWifiDisplaySend find = %d, IWifiDisplayThread = %p",
            curClient != NULL, client);
    }

    if (curClient != NULL) {
        curClient->SetWifiDisplayClientStatus(WifiDisplayClient::WIFIDISPLAY_CLIENT_NONE);
        *result = 0;
    }

    return NOERROR;
}

ECode CWifiDisplayManagerService::StartWifiDisplayReceive(
    /* [in] */ IWifiDisplayThread* client,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    AutoPtr<WifiDisplayClient> curClient = ContainsWifiDisplayThread(client);

    if (DEBUG_CLIENT) {
        Slogger::D(TAG, "StartWifiDisplayReceive find = %d, IWifiDisplayThread = %p",
            curClient != NULL, client);
    }

    if (curClient != NULL) {
        curClient->SetWifiDisplayClientStatus(WifiDisplayClient::WIFIDISPLAY_CLIENT_RECEIVEING);
        *result = 0;
    }

    return NOERROR;
}

ECode CWifiDisplayManagerService::EndWifiDisplayReceive(
    /* [in] */ IWifiDisplayThread* client,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    AutoPtr<WifiDisplayClient> curClient = ContainsWifiDisplayThread(client);

    if (DEBUG_CLIENT) {
        Slogger::D(TAG, "EndWifiDisplayReceive find = %d, IWifiDisplayThread = %p",
            curClient != NULL, client);
    }

    if (curClient != NULL) {
        curClient->SetWifiDisplayClientStatus(WifiDisplayClient::WIFIDISPLAY_CLIENT_NONE);
        *result = 0;
    }

    return NOERROR;
}

ECode CWifiDisplayManagerService::ExitWifiDisplayReceive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean  ret = FALSE;

    Slogger::D(TAG, "ExitWifiDisplayReceive mClients.size() = %d", mClients.GetSize());

    ECode ec = NOERROR;
    Int32 status;
    List<AutoPtr<WifiDisplayClient> >::Iterator it = mClients.Begin();
    for (; it != mClients.End(); ++it)  {
        status = (*it)->GetWifiDisplayClientStatus();
        if (status == WifiDisplayClient::WIFIDISPLAY_CLIENT_RECEIVEING) {
            AutoPtr<IWifiDisplayThread> displayThread = (*it)->GetWifiDisplayClient();
            ec = displayThread->ExitWifiDisplayReceive();
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                break;
            }

            ret = TRUE;
        }
    }

    *result = ret;
    return NOERROR;
}

ECode CWifiDisplayManagerService::ExitWifiDisplaySend(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean  ret = FALSE;

    Slogger::D(TAG,"ExitWifiDisplaySend mClients.size() = %d", mClients.GetSize());

    ECode ec = NOERROR;
    Int32 status;
    List<AutoPtr<WifiDisplayClient> >::Iterator it = mClients.Begin();
    for (; it != mClients.End(); ++it)  {
        status = (*it)->GetWifiDisplayClientStatus();
        if (status == WifiDisplayClient::WIFIDISPLAY_CLIENT_RECEIVEING) {
            AutoPtr<IWifiDisplayThread> displayThread = (*it)->GetWifiDisplayClient();

            ec = displayThread->ExitWifiDisplaySend();
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                break;
            }

            ret = TRUE;
        }
    }

    *result = ret;
    return NOERROR;
}

ECode CWifiDisplayManagerService::GetWifiDisplayStatus(
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    *status = 0;

    Slogger::D(TAG,"GetWifiDisplayStatus mClients.size() = %d", mClients.GetSize());

    Int32 curStatus;
    List<AutoPtr<WifiDisplayClient> >::Iterator it = mClients.Begin();
    for (; it != mClients.End(); ++it)  {
        curStatus = (*it)->GetWifiDisplayClientStatus();
        if (curStatus != WifiDisplayClient::WIFIDISPLAY_CLIENT_NONE) {
            Slogger::D(TAG, "getWifiDisplayStatus mClients.get(i).getWifiDisplayClientStatus() = %d",
                curStatus);

            *status = curStatus;
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CWifiDisplayManagerService::InjectMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    ECode ec = NOERROR;
    Int32 curStatus;
    List<AutoPtr<WifiDisplayClient> >::Iterator it = mClients.Begin();
    for (; it != mClients.End(); ++it)  {
        curStatus = (*it)->GetWifiDisplayClientStatus();

        if (curStatus == WifiDisplayClient::WIFIDISPLAY_CLIENT_RECEIVEING) {
                AutoPtr<IWifiDisplayThread> displayThread = (*it)->GetWifiDisplayClient();

            ec = displayThread->DispatchMotionEvent(event);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                break;
            }
        }
    }

    return ec;
}

} // Server
} // Droid
} // Elastos
