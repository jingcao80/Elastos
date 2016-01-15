#include <elastos/droid/server/connectivity/NetworkAgentInfo.h>
#include <elastos/core/StringBuilder.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Net::CLinkProperties;
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

} // Connectivity
} // Server
} // Droid
} // Elastos
