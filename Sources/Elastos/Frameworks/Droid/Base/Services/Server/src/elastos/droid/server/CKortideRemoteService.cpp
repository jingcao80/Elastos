
#include "CKortideRemoteService.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Net::CServerSocket;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::SystemClock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CKortideRemoteService::TAG("KortideRemoteService");
const Boolean CKortideRemoteService::DBG = FALSE;
const Int32 CKortideRemoteService::SLEEPTIME_MS = 5000;
const Int32 CKortideRemoteService::RETRYMAX = 5;
const Int32 CKortideRemoteService::LANPORT = 49527;
const Int32 CKortideRemoteService::NETPORT = 40000;
Int32 CKortideRemoteService::sRetryCounter = 0;

ECode CKortideRemoteService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<ISystemProperties> systemProperties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProperties);
    systemProperties->Get(String("ro.product.name"), String("KortideDevice"), &mDevName);
    mLanDispatcher = new LanDispatcher(mContext);
    mLanDispatcher->Start();
    InitServerSocket();
    if (DBG) Slogger::D(TAG, "constructor %s", mDevName.string());
    return NOERROR;
}

void CKortideRemoteService::InitServerSocket()
{
    ECode ec = NOERROR;
    do {
        ec = CServerSocket::New(LANPORT, (IServerSocket**)&mLanServerSocket);
        if (FAILED(ec))
            break;
        ec = CServerSocket::New(NETPORT, (IServerSocket**)&mNetServerSocket);
    } while (0);

    if (FAILED(ec)) {
        Slogger::E(TAG, "ServerSocket init fail, retry");
        // ex.printStackTrace();
        SystemClock::Sleep(SLEEPTIME_MS);
        sRetryCounter++;
        if (sRetryCounter <= RETRYMAX) {
            InitServerSocket();
        }
        else {
            Slogger::E(TAG, "Stop init ServerSocket");
            sRetryCounter = 0;
        }
        return;
    }

    if (mLanServerSocket != NULL && mLanCmdListener == NULL) {
        mLanCmdListener = new RemoteCmdListener(mContext, mLanServerSocket);
        mLanCmdListener->Start();
    }
    if (mNetServerSocket != NULL && mNetCmdListener == NULL) {
        mNetCmdListener = new RemoteCmdListener(mContext, mNetServerSocket);
        mNetCmdListener->Start();
    }
}

ECode CKortideRemoteService::SetDeviceName(
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    mDevName = name;
    *result = 0;
    return NOERROR;
}

ECode CKortideRemoteService::GetDeviceName(
    /* [out] */ String* deviceName)
{
    VALIDATE_NOT_NULL(deviceName)
    *deviceName = mDevName;
    return NOERROR;
}
}//Server
}//Droid
}//Elastos
