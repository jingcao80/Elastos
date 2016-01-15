#include "MonkeyNetworkMonitor.h"
#include "elastos/droid/os/SystemClock.h"

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::NetworkInfoState;
using Elastos::Droid::Net::NetworkInfoState_CONNECTED;
using Elastos::Droid::Net::NetworkInfoState_DISCONNECTED;
using Elastos::Droid::Net::EIID_INetworkInfo;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

const Boolean MonkeyNetworkMonitor::LDEBUG = FALSE;

MonkeyNetworkMonitor::MonkeyNetworkMonitor()
 : mCollectionStartTime(0)
 , mEventTime(0)
 , mLastNetworkType(-1)
 , mWifiElapsedTime(0)
 , mMobileElapsedTime(0)
 , mElapsedTime(0)
{
    ASSERT_SUCCEEDED(CIntentFilter::New(IConnectivityManager::CONNECTIVITY_ACTION, (IIntentFilter**)&mFilter));
}

ECode MonkeyNetworkMonitor::PerformReceive(
    /* [in] */ IIntent * intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle * extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    AutoPtr<IParcelable> parcel;
    intent->GetParcelableExtra(
        IConnectivityManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcel);
    AutoPtr<INetworkInfo> ni = INetworkInfo::Probe(parcel);
    if (LDEBUG) {
        Int32 type;
        ni->GetType(&type);
        NetworkInfoState state;
        ni->GetState(&state);
        PFL_EX("Network state changed: type=%d, state=%d", type, state);
    }

    UpdateNetworkStats();
    NetworkInfoState state;
    ni->GetState(&state);
    if(NetworkInfoState_CONNECTED == state) {
        if (LDEBUG) {
            PFL_EX("Network connected");
        }
        ni->GetType(&mLastNetworkType);
    }
    else if (NetworkInfoState_DISCONNECTED == state) {
        if (LDEBUG) {
            PFL_EX("Network not connected");
        }
        mLastNetworkType = -1; // unknown since we're disconnected
    }

    mEventTime = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode MonkeyNetworkMonitor::Start()
{
    mWifiElapsedTime = 0;
    mMobileElapsedTime = 0;
    mElapsedTime = 0;
    mEventTime = mCollectionStartTime = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode MonkeyNetworkMonitor::Register(
    /* [in] */ IIActivityManager * am)
{
    if (LDEBUG) {
        PFL_EX("registering Receiver");
    }
    AutoPtr<IIntent> intent;
    return am->RegisterReceiver(NULL, String(), THIS_PROBE(IIntentReceiver), mFilter, String(), IUserHandle::USER_ALL
        , (IIntent**)&intent);
}

ECode MonkeyNetworkMonitor::Unregister(
    /* [in] */ IIActivityManager * am)
{
    if (LDEBUG) {
        PFL_EX("unregistering Receiver");
    }
    return am->UnregisterReceiver(THIS_PROBE(IIntentReceiver));
}

ECode MonkeyNetworkMonitor::Stop()
{
    return UpdateNetworkStats();
}

ECode MonkeyNetworkMonitor::Dump()
{
    PFL_EX("Network stats: elapsed time=%lld ms (%lldms mobile, %lldms wifi, %lldms not connected)",
        mElapsedTime, mMobileElapsedTime, mWifiElapsedTime,
        mElapsedTime - mMobileElapsedTime - mWifiElapsedTime);
    return NOERROR;
}

ECode MonkeyNetworkMonitor::UpdateNetworkStats()
{
    Int64 timeNow = SystemClock::GetElapsedRealtime();
    Int64 delta = timeNow - mEventTime;
    switch (mLastNetworkType) {
        case IConnectivityManager::TYPE_MOBILE:
            if (LDEBUG) {
                PFL_EX("Adding to mobile: %lld", delta);
            }
            mMobileElapsedTime += delta;
            break;
        case IConnectivityManager::TYPE_WIFI:
            if (LDEBUG) {
                PFL_EX("Adding to wifi: %lld", delta);
            }
            mWifiElapsedTime += delta;
            break;
        default:
            if (LDEBUG) {
                PFL_EX("Unaccounted for: %lld", delta);
            }
            break;
    }
    mElapsedTime = timeNow - mCollectionStartTime;
    return NOERROR;
}


} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
