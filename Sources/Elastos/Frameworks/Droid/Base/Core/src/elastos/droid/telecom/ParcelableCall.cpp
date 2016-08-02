#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/ParcelableCall.h"
#include "elastos/droid/telecom/CVideoCallImpl.h"
#include "elastos/droid/telecom/CDisconnectCause.h"
#include "elastos/droid/telecom/CGatewayInfo.h"
#include "elastos/droid/telecom/CPhoneAccountHandle.h"
#include "elastos/droid/telecom/CStatusHints.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/net/CStringUri.h"
#include "elastos/droid/net/COpaqueUri.h"
#include "elastos/droid/net/CHierarchicalUri.h"
#include "elastos/droid/utility/CParcelableList.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Net::CStringUri;
using Elastos::Droid::Net::COpaqueUri;
using Elastos::Droid::Net::CHierarchicalUri;
using Elastos::Droid::Utility::CParcelableList;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// ParcelableCall::
//===============================================================
CAR_INTERFACE_IMPL_2(ParcelableCall, Object, IParcelableCall, IParcelable)

ParcelableCall::ParcelableCall()
    : mId("")
    , mState(0)
    , mCapabilities(0)
    , mProperties(0)
    , mCreateTimeMillis(0)
    , mConnectTimeMillis(0)
    , mHandlePresentation(0)
    , mCallerDisplayNamePresentation(0)
    , mVideoState(0)
    , mIsActiveSub(FALSE)
    , mCallSubstate(0)
{}

ECode ParcelableCall::constructor()
{
    return NOERROR;
}

ECode ParcelableCall::constructor(
    /* [in] */ const String& id,
    /* [in] */ Int32 state,
    /* [in] */ IDisconnectCause* disconnectCause,
    /* [in] */ IList* cannedSmsResponses,
    /* [in] */ Int32 capabilities,
    /* [in] */ Int32 properties,
    /* [in] */ Int64 createTimeMillis,
    /* [in] */ Int64 connectTimeMillis,
    /* [in] */ IUri* handle,
    /* [in] */ Int32 handlePresentation,
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 callerDisplayNamePresentation,
    /* [in] */ IGatewayInfo* gatewayInfo,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ IIVideoProvider* videoCallProvider,
    /* [in] */ const String& parentCallId,
    /* [in] */ IList* childCallIds,
    /* [in] */ IStatusHints* statusHints,
    /* [in] */ Int32 videoState,
    /* [in] */ IList* conferenceableCallIds,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean isActiveSub,
    /* [in] */ Int32 callSubstate)
{
    mId = id;
    mState = state;
    mDisconnectCause = disconnectCause;
    mCannedSmsResponses = cannedSmsResponses;
    mCapabilities = capabilities;
    mProperties = properties;
    mCreateTimeMillis = createTimeMillis;
    mConnectTimeMillis = connectTimeMillis;
    mHandle = handle;
    mHandlePresentation = handlePresentation;
    mCallerDisplayName = callerDisplayName;
    mCallerDisplayNamePresentation = callerDisplayNamePresentation;
    mGatewayInfo = gatewayInfo;
    mAccountHandle = accountHandle;
    mVideoCallProvider = videoCallProvider;
    mParentCallId = parentCallId;
    mChildCallIds = childCallIds;
    mStatusHints = statusHints;
    mVideoState = videoState;
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->UnmodifiableList(conferenceableCallIds, (IList**)&mConferenceableCallIds);
    mExtras = extras;
    mIsActiveSub = isActiveSub;
    mCallSubstate = callSubstate;
    return NOERROR;
}

ECode ParcelableCall::GetId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode ParcelableCall::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode ParcelableCall::GetDisconnectCause(
    /* [out] */ IDisconnectCause** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDisconnectCause;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetCannedSmsResponses(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCannedSmsResponses;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCapabilities;
    return NOERROR;
}

ECode ParcelableCall::GetProperties(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProperties;
    return NOERROR;
}

ECode ParcelableCall::GetConnectTimeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConnectTimeMillis;
    return NOERROR;
}

ECode ParcelableCall::GetCreateTimeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCreateTimeMillis;
    return NOERROR;
}

ECode ParcelableCall::GetHandle(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetHandlePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHandlePresentation;
    return NOERROR;
}

ECode ParcelableCall::GetCallerDisplayName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerDisplayName;
    return NOERROR;
}

ECode ParcelableCall::GetCallerDisplayNamePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerDisplayNamePresentation;
    return NOERROR;
}

ECode ParcelableCall::GetGatewayInfo(
    /* [out] */ IGatewayInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGatewayInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetAccountHandle(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAccountHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetVideoCall(
    /* [out] */ IInCallServiceVideoCall** result)
{
    VALIDATE_NOT_NULL(result)
    if (mVideoCall == NULL && mVideoCallProvider != NULL) {
        CVideoCallImpl::New(mVideoCallProvider, (IInCallServiceVideoCall**)&mVideoCall);
    }

    *result = mVideoCall;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetParentCallId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mParentCallId;
    return NOERROR;
}

ECode ParcelableCall::GetChildCallIds(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChildCallIds;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetConferenceableCallIds(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConferenceableCallIds;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetStatusHints(
    /* [out] */ IStatusHints** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatusHints;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetVideoState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoState;
    return NOERROR;
}

ECode ParcelableCall::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetIsActiveSub(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsActiveSub;
    return NOERROR;
}

ECode ParcelableCall::GetCallSubstate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallSubstate;
    return NOERROR;
}

ECode ParcelableCall::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mId);
    source->ReadInt32(&mState);

    Int32 value;
    source->ReadInt32(&value);
    if (value != 0) {
        mDisconnectCause = NULL;
        CDisconnectCause::New((IDisconnectCause**)&mDisconnectCause);
        IParcelable::Probe(mDisconnectCause)->ReadFromParcel(source);
    }
    else {
        mDisconnectCause = NULL;
    }

    source->ReadInt32(&value);
    if (value != 0) {
        mCannedSmsResponses = NULL;
        AutoPtr<IList> array;
        CArrayList::New((IList**)&array);
        CParcelableList::New(array, (IList**)&mCannedSmsResponses);

        IParcelable::Probe(mCannedSmsResponses)->ReadFromParcel(source);
    }
    else {
        mCannedSmsResponses = NULL;
    }

    source->ReadInt32(&mCapabilities);
    source->ReadInt32(&mProperties);
    source->ReadInt64(&mCreateTimeMillis);
    source->ReadInt64(&mConnectTimeMillis);

    source->ReadInt32(&value);
    if (value != 0) {
        mHandle = NULL;

        //Int32 id = 0;
        //source->ReadInt32(&id);
        //if (id == 1) {
        //    CStringUri::New((IUri**)&mHandle);
        //    IParcelable::Probe(mHandle)->ReadFromParcel(source);
        //}
        //else if (id == 2) {
        //    COpaqueUri::New((IUri**)&mHandle);
        //    IParcelable::Probe(mHandle)->ReadFromParcel(source);
        //}
        //else if (id == 3) {
        //    CHierarchicalUri::New((IUri**)&mHandle);
        //    IParcelable::Probe(mHandle)->ReadFromParcel(source);
        //}
        //else {
        //    Logger::E("GatewayInfo", "Wrong URI id:%d, only 1, 2, 3 accepted", id);
        //}
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mHandle = IUri::Probe(obj);
    }
    else {
        mHandle = NULL;
    }

    source->ReadInt32(&mHandlePresentation);
    source->ReadString(&mCallerDisplayName);
    source->ReadInt32(&mCallerDisplayNamePresentation);

    source->ReadInt32(&value);
    if (value != 0) {
        mGatewayInfo = NULL;
        CGatewayInfo::New((IGatewayInfo**)&mGatewayInfo);
        IParcelable::Probe(mGatewayInfo)->ReadFromParcel(source);
    }
    else {
        mGatewayInfo = NULL;
    }

    source->ReadInt32(&value);
    if (value != 0) {
        mAccountHandle = NULL;
        CPhoneAccountHandle::New((IPhoneAccountHandle**)&mAccountHandle);
        IParcelable::Probe(mAccountHandle)->ReadFromParcel(source);
    }
    else {
        mAccountHandle = NULL;
    }

    source->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mVideoCallProvider = IIVideoProvider::Probe(obj);
    }
    else {
        mVideoCallProvider = NULL;
    }

    mParentCallId = NULL;
    source->ReadString(&mParentCallId);

    source->ReadInt32(&value);
    if (value != 0) {
        mChildCallIds = NULL;
        AutoPtr<IList> array;
        CArrayList::New((IList**)&array);
        CParcelableList::New(array, (IList**)&mChildCallIds);

        IParcelable::Probe(mChildCallIds)->ReadFromParcel(source);
    }
    else {
        mChildCallIds = NULL;
    }

    source->ReadInt32(&value);
    if (value != 0) {
        mStatusHints = NULL;
        CStatusHints::New((IStatusHints**)&mStatusHints);
        IParcelable::Probe(mStatusHints)->ReadFromParcel(source);
    }
    else {
        mStatusHints = NULL;
    }

    source->ReadInt32(&mVideoState);

    source->ReadInt32(&value);
    if (value != 0) {
        mConferenceableCallIds = NULL;
        AutoPtr<IList> array;
        CArrayList::New((IList**)&array);
        CParcelableList::New(array, (IList**)&mConferenceableCallIds);

        IParcelable::Probe(mConferenceableCallIds)->ReadFromParcel(source);
    }
    else {
        mConferenceableCallIds = NULL;
    }

    source->ReadInt32(&value);
    if (value != 0) {
        CBundle::New((IBundle**)&mExtras);
        IParcelable::Probe(mExtras)->ReadFromParcel(source);
    }
    else {
        mExtras = NULL;
    }

    source->ReadBoolean(&mIsActiveSub);
    source->ReadInt32(&mCallSubstate);
    return NOERROR;
}

ECode ParcelableCall::WriteToParcel(
    /* [in] */ IParcel* destination)
{
    destination->WriteString(mId);
    destination->WriteInt32(mState);

    if (mDisconnectCause != NULL) {
        destination->WriteInt32(1);
        IParcelable::Probe(mDisconnectCause)->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }

    if (mCannedSmsResponses != NULL) {
        destination->WriteInt32(1);

        AutoPtr<IParcelable> parcelable;
        CParcelableList::New(mCannedSmsResponses, (IParcelable**)&parcelable);
        parcelable->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }

    destination->WriteInt32(mCapabilities);
    destination->WriteInt32(mProperties);
    destination->WriteInt64(mCreateTimeMillis);
    destination->WriteInt64(mConnectTimeMillis);

    if (mHandle != NULL) {
        destination->WriteInt32(1);
        //IParcelable::Probe(mHandle)->WriteToParcel(destination);
        destination->WriteInterfacePtr(mHandle);//TODO should update
    }
    else {
        destination->WriteInt32(0);
    }

    destination->WriteInt32(mHandlePresentation);
    destination->WriteString(mCallerDisplayName);
    destination->WriteInt32(mCallerDisplayNamePresentation);

    if (mGatewayInfo != NULL) {
        destination->WriteInt32(1);
        IParcelable::Probe(mGatewayInfo)->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }

    if (mAccountHandle != NULL) {
        destination->WriteInt32(1);
        IParcelable::Probe(mAccountHandle)->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }

    if (NULL != mVideoCallProvider) {
        destination->WriteInt32(1);
        destination->WriteInterfacePtr(mVideoCallProvider);
    }
    else {
        destination->WriteInt32(0);
    }

    destination->WriteString(mParentCallId);

    if (mChildCallIds != NULL) {
        destination->WriteInt32(1);

        AutoPtr<IParcelable> parcelable;
        CParcelableList::New(mChildCallIds, (IParcelable**)&parcelable);
        parcelable->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }

    if (mStatusHints != NULL) {
        destination->WriteInt32(1);
        IParcelable::Probe(mStatusHints)->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }

    destination->WriteInt32(mVideoState);

    if (mConferenceableCallIds != NULL) {
        destination->WriteInt32(1);

        AutoPtr<IParcelable> parcelable;
        CParcelableList::New(mConferenceableCallIds, (IParcelable**)&parcelable);
        parcelable->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }

    if (mExtras != NULL) {
        destination->WriteInt32(1);
        IParcelable::Probe(mExtras)->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }

    destination->WriteBoolean(mIsActiveSub);
    destination->WriteInt32(mCallSubstate);
    return NOERROR;
}

ECode ParcelableCall::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(3);
    AutoPtr<ICharSequence> pId;
    CString::New(mId, (ICharSequence**)&pId);
    arr->Set(0, pId);
    AutoPtr<ICharSequence> pParentCallId;
    CString::New(mParentCallId, (ICharSequence**)&pParentCallId);
    arr->Set(1, pParentCallId);
    arr->Set(2, mChildCallIds);
    *result = StringUtils::Format(String("[%s, parent:%s, children:%s]"), arr);
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
