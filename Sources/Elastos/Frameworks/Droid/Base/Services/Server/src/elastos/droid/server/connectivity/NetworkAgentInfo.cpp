#include <elastos/droid/server/connectivity/NetworkAgentInfo.h>
#include <elastos/core/StringBuilder.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Utility::CSparseArray;

using Elastos::Utility::CArrayList;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

// Penalty applied to scores of Networks that have not been validated.
const Int32 NetworkAgentInfo::UNVALIDATED_SCORE_PENALTY = 40;

// Score for explicitly connected network.
const Int32 NetworkAgentInfo::EXPLICITLY_SELECTED_NETWORK_SCORE = 100;

const String NetworkAgentInfo::EXTRA_FEATURE_ID(String("cneFeatureId"));

const String NetworkAgentInfo::EXTRA_FEATURE_PARAMETER(String("cneFeatureParameter"));

const String NetworkAgentInfo::EXTRA_PARAMETER_VALUE(String("cneParameterValue"));

const Int32 NetworkAgentInfo::FEATURE_ID = 1;

const Int32 NetworkAgentInfo::FEATURE_PARAM = 1;

const Int32 NetworkAgentInfo::FEATURE_OFF = 1;

const Int32 NetworkAgentInfo::FEATURE_ON = FEATURE_OFF + 1;

//-----------------------------------------------------------------------
//          NetworkAgentInfo::MyBroadcastReceiver
//-----------------------------------------------------------------------
NetworkAgentInfo::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ NetworkAgentInfo* host)
    : mHost(host)
{}

ECode NetworkAgentInfo::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals("com.quicinc.cne.CNE_PREFERENCE_CHANGED")) {
        Int32 featureId;
        intent->GetInt32Extra(EXTRA_FEATURE_ID, -1, &featureId);
        Int32 featureParam;
        intent->GetInt32Extra(EXTRA_FEATURE_PARAMETER, -1, &featureParam);
        Int32 featureVal;
        intent->GetInt32Extra(EXTRA_PARAMETER_VALUE, -1, &featureVal);
        mHost->HandlePrefChange(featureId, featureParam, featureVal);
    }
}

CAR_INTERFACE_IMPL(NetworkAgentInfo, Object, INetworkAgentInfo)

NetworkAgentInfo::NetworkAgentInfo(
    /* [in] */ IMessenger* messenger,
    /* [in] */ IAsyncChannel* ac,
    /* [in] */ INetworkInfo* info,
    /* [in] */ ILinkProperties* lp,
    /* [in] */ INetworkCapabilities* nc,
    /* [in] */ Int32 score,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ INetworkMisc* misc)
    : mIsCneWqeEnabled(FALSE)
{
    CSparseArray::New((ISparseArray**)&mNetworkRequests);
    CArrayList::New((IArrayList**)&mNetworkLingered);

    mMessenger = messenger;
    mAsyncChannel = ac;
    mNetwork = NULL;
    mNetworkInfo = info;
    mLinkProperties = lp;
    mNetworkCapabilities = nc;
    mCurrentScore = score;
    mNetworkMonitor = new NetworkMonitor(context, handler, this);
    mNetworkMisc = misc;
    mCreated = FALSE;
    mValidated = FALSE;
    mContext = context;
    AutoPtr<ISystemProperties> properties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&properties);
    Int32 val;
    properties->GetInt32(String("persist.cne.feature"), 0, &val);
    if (val == 3) {
        AutoPtr<MyBroadcastReceiver> myBR = new MyBroadcastReceiver(this);
        AutoPtr<IIntentFilter> intentFilter;
        CIntentFilter::New(String("com.quicinc.cne.CNE_PREFERENCE_CHANGED"), (IIntentFilter**)&intentFilter);
        AutoPtr<IIntent> tmpIntent;
        mContext->RegisterReceiver(IBroadcastReceiver::Probe(myBR), intentFilter, (IIntent**)&tmpIntent);
    }
}

ECode NetworkAgentInfo::AddRequest(
    /* [in] */ INetworkRequest* networkRequest)
{
    Int32 id;
    networkRequest->GetRequestId(&id);
    mNetworkRequests->Put(id, TO_IINTERFACE(networkRequest));
    return NOERROR;
}

ECode NetworkAgentInfo::IsVPN(
    /* [out] */ Boolean* vpn)
{
    VALIDATE_NOT_NULL(vpn)
    return mNetworkCapabilities->HasTransport(INetworkCapabilities::TRANSPORT_VPN, vpn);
}

ECode NetworkAgentInfo::GetCurrentScore(
    /* [out] */ Int32* score)
{
    VALIDATE_NOT_NULL(score)
    *score = 0;

    // TODO: We may want to refactor this into a NetworkScore class that takes a base score from
    // the NetworkAgent and signals from the NetworkAgent and uses those signals to modify the
    // score.  The NetworkScore class would provide a nice place to centralize score constants
    // so they are not scattered about the transports.

    *score = mCurrentScore;
    if (mIsCneWqeEnabled) return NOERROR;

    if (!mValidated) *score -= UNVALIDATED_SCORE_PENALTY;
    if (*score < 0) *score = 0;

    Boolean bval;
    mNetworkMisc->GetExplicitlySelected(&bval);
    if (bval) *score = EXPLICITLY_SELECTED_NETWORK_SCORE;

    return NOERROR;
}

ECode NetworkAgentInfo::SetCurrentScore(
    /* [in] */ Int32 newScore)
{
    mCurrentScore = newScore;
    return NOERROR;
}

ECode NetworkAgentInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("NetworkAgentInfo{ ni{");
    sb += Object::ToString(mNetworkInfo);

    sb += "}  network{";
    sb += Object::ToString(mNetwork);

    sb += "}  lp{";
    sb += Object::ToString(mLinkProperties);

    sb += "}  nc{";
    sb += Object::ToString(mNetworkCapabilities);

    Int32 ival;
    GetCurrentScore(&ival);
    sb += "}  Score{";
    sb += ival;
    sb += "} ";

    sb += "validated{";
    sb += mValidated;

    sb += "} created{";
    sb += mCreated;


    Boolean bval;
    mNetworkMisc->GetExplicitlySelected(&bval);
    sb += "explicitlySelected{";
    sb += bval;
    sb += "} }";

    *str = sb.ToString();
    return NOERROR;
}

ECode NetworkAgentInfo::Name(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = Name();
    return NOERROR;
}

String NetworkAgentInfo::Name()
{
    String type, subType;
    mNetworkInfo->GetTypeName(&type);
    mNetworkInfo->GetSubtypeName(&subType);
    StringBuilder sb("NetworkAgentInfo [");
    sb += type;
    sb += " ";
    sb += subType;
    sb += ") - ";
    if (mNetwork != NULL) {
        sb += Object::ToString(mNetwork);
    }
    else {
        sb += "null";
    }
    sb += "]";
    return sb.ToString();
}

void NetworkAgentInfo::HandlePrefChange(
    /* [in] */ Int32 featureId,
    /* [in] */ Int32 featureParam,
    /* [in] */ Int32 value)
{
    if (featureId == FEATURE_ID && featureParam == FEATURE_PARAM) {
        if (value == FEATURE_ON) {
            mIsCneWqeEnabled = TRUE;
        }
        else if (value == FEATURE_OFF) {
            mIsCneWqeEnabled = FALSE;
        }
    }
}

} // Connectivity
} // Server
} // Droid
} // Elastos
