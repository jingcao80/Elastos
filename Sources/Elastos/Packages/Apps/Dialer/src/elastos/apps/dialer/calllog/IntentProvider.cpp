
#include "IntentProvider.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

const String IntentProvider::TAG("IntentProvider");

//=================================================================
// IntentProvider::ReturnCallIntentProvider
//=================================================================
CAR_INTERFACE_IMPL(IntentProvider::ReturnCallIntentProvider, Object, IIntentProvider)

IntentProvider::ReturnCallIntentProvider::ReturnCallIntentProvider(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle)
    : mAccountHandle(accountHandle)
{
    mNumber = number;
}

ECode IntentProvider::ReturnCallIntentProvider::GetIntent(
    /* [in] */ IContext* context,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    AutoPtr<IIntent> result = CallUtil::GetCallIntent(mNumber, mAccountHandle);
    *intent = result;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

//=================================================================
// IntentProvider::ReturnVideoCallIntentProvider
//=================================================================
CAR_INTERFACE_IMPL(IntentProvider::ReturnVideoCallIntentProvider, Object, IIntentProvider)

IntentProvider::ReturnVideoCallIntentProvider::ReturnVideoCallIntentProvider(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle)
    : mAccountHandle(accountHandle)
{
    mNumber = number;
}

ECode IntentProvider::ReturnVideoCallIntentProvider::GetIntent(
    /* [in] */ IContext* context,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);
    AutoPtr<IIntent> result = CallUtil::GetVideoCallIntent(mNumber, mAccountHandle);
    *intent = result;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

//=================================================================
// IntentProvider::PlayVoicemailIntentProvider
//=================================================================
CAR_INTERFACE_IMPL(IntentProvider::PlayVoicemailIntentProvider, Object, IIntentProvider)

IntentProvider::PlayVoicemailIntentProvider::PlayVoicemailIntentProvider(
    /* [in] */ Int64 rowId,
    /* [in] */ const String& voicemailUri)
    : mRowId(rowId)
{
    mVoicemailUri = voicemailUri;
}

ECode IntentProvider::PlayVoicemailIntentProvider::GetIntent(
    /* [in] */ IContext* context,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);

    AutoPtr<IIntent> result;
    CIntent::New(context, ECLSID_CCallDetailActivity, (IIntent**)&result);

    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> uri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&uri);
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    AutoPtr<IUri> callsUri;
    contentUris->WithAppendedId(uri, mRowId, (IUri**)&callsUri);
    result->SetData(callsUri);

    if (!mVoicemailUri.IsNull()) {
        result->PutExtra(ICallDetailActivity::EXTRA_VOICEMAIL_URI,
                Uri::Parse(voicemailUri));
    }
    result->putExtra(ICallDetailActivity::EXTRA_VOICEMAIL_START_PLAYBACK, TRUE);
    *intent = result;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

//=================================================================
// IntentProvider::CallDetailIntentProvider
//=================================================================
CAR_INTERFACE_IMPL(IntentProvider::CallDetailIntentProvider, Object, IIntentProvider)

IntentProvider::CallDetailIntentProvider::CallDetailIntentProvider(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* extraIds,
    /* [in] */ const String& voicemailUri)
    : mId(id)
    , mExtraIds(extraIds)
{
    mVoicemailUri = voicemailUri;
}

ECode IntentProvider::CallDetailIntentProvider::GetIntent(
    /* [in] */ IContext* context,
    /* [out] */ IIntent** intent)
{
    VALUE_NOT_NULL(intent);

    AutoPtr<IIntent> result;
    CIntent::New(context, ECLSID_CCallDetailActivity, (IIntent**)&result);
    // Check if the first item is a voicemail.
    if (!mVoicemailUri.IsNull()) {
        result->PutExtra(ICallDetailActivity::EXTRA_VOICEMAIL_URI,
                Uri::Parse(mVoicemailUri));
    }
    result->PutExtra(ICallDetailActivity::EXTRA_VOICEMAIL_START_PLAYBACK, FALSE);

    if (mExtraIds != NULL && mExtraIds->GetLength() > 0) {
        result->PutExtra(ICallDetailActivity::EXTRA_CALL_LOG_IDS, mExtraIds);
    }
    else {
        // If there is a single item, use the direct URI for it.
        AutoPtr<ICalls> calls;
        CCalls::AcquireSingleton((ICalls**)&calls);
        AutoPtr<IUri> uri;
        calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&uri);
        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        AutoPtr<IUri> callsUri;
        contentUris->WithAppendedId(uri, mId, (IUri**)&callsUri);
        result->SetData(callsUri);
    }

    *intent = result;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}


//=================================================================
// IntentProvider
//=================================================================
CAR_INTERFACE_IMPL(IntentProvider, Object, IIntentProvider)

AutoPtr<IIntentProvider> IntentProvider::GetReturnCallIntentProvider(
    /* [in] */ const String& number)
{
    return GetReturnCallIntentProvider(number, NULL);
}

AutoPtr<IIntentProvider> IntentProvider::GetReturnCallIntentProvider(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    AutoPtr<ReturnCallIntentProvider> provider = new ReturnCallIntentProvider(
            number, accountHandle);

    return (IIntentProvider*)provider;
}

AutoPtr<IIntentProvider> IntentProvider::GetReturnVideoCallIntentProvider(
    /* [in] */ const String& number)
{
    return GetReturnVideoCallIntentProvider(number, NULL);
}

AutoPtr<IIntentProvider> IntentProvider::GetReturnVideoCallIntentProvider(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    AutoPtr<ReturnVideoCallIntentProvider> provider = new ReturnVideoCallIntentProvider(
            number, accountHandle);

    return (IIntentProvider*)provider;
}

AutoPtr<IIntentProvider> IntentProvider::GetPlayVoicemailIntentProvider(
    /* [in] */ Int64 rowId,
    /* [in] */ const String& voicemailUri)
{
    AutoPtr<PlayVoicemailIntentProvider> provider = new PlayVoicemailIntentProvider(
            rowId, voicemailUri);

    return (IIntentProvider*)provider;
}

AutoPtr<IIntentProvider> IntentProvider::GetCallDetailIntentProvider(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* extraIds,
    /* [in] */ const String& voicemailUri)
{
    AutoPtr<CallDetailIntentProvider> provider = new CallDetailIntentProvider(
            id, extraIds, voicemailUri);

    return (IIntentProvider*)provider;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
