
#include "elastos/droid/dialer/calllog/IntentProvider.h"
#include "elastos/droid/contacts/common/CallUtil.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Contacts::Common::CallUtil;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Dialer::CallLog::EIID_IIntentProvider;
// using Elastos::Droid::Dialer::ECLSID_CCallDetailActivity;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Provider::CCalls;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

const String IntentProvider::TAG("IntentProvider");

//=================================================================
// IntentProvider::ReturnCallIntentProvider
//=================================================================
CAR_INTERFACE_IMPL(IntentProvider::ReturnCallIntentProvider, Object, IIntentProvider);

IntentProvider::ReturnCallIntentProvider::ReturnCallIntentProvider(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle)
    : mNumber(number)
    , mAccountHandle(accountHandle)
{}

ECode IntentProvider::ReturnCallIntentProvider::GetIntent(
    /* [in] */ IContext* context,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> result = CallUtil::GetCallIntent(mNumber, mAccountHandle);
    *intent = result;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}


//=================================================================
// IntentProvider::ReturnVideoCallIntentProvider
//=================================================================
CAR_INTERFACE_IMPL(IntentProvider::ReturnVideoCallIntentProvider, Object, IIntentProvider)

IntentProvider::ReturnVideoCallIntentProvider::ReturnVideoCallIntentProvider(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle)
    : mNumber(number)
    , mAccountHandle(accountHandle)
{}

ECode IntentProvider::ReturnVideoCallIntentProvider::GetIntent(
    /* [in] */ IContext* context,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> result = CallUtil::GetVideoCallIntent(mNumber, mAccountHandle);
    *intent = result;
    REFCOUNT_ADD(*intent)
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
    , mVoicemailUri(voicemailUri)
{}

ECode IntentProvider::PlayVoicemailIntentProvider::GetIntent(
    /* [in] */ IContext* context,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)

    AutoPtr<IIntent> result;
    assert(0);
    // CIntent::New(context, ECLSID_CCallDetailActivity, (IIntent**)&result);

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
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->Parse(mVoicemailUri, (IUri**)&uri);
        result->PutExtra(ICallDetailActivity::EXTRA_VOICEMAIL_URI,
                IParcelable::Probe(uri));
    }
    result->PutBooleanExtra(ICallDetailActivity::EXTRA_VOICEMAIL_START_PLAYBACK, TRUE);
    *intent = result;
    REFCOUNT_ADD(*intent)
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
    , mVoicemailUri(voicemailUri)
{}

ECode IntentProvider::CallDetailIntentProvider::GetIntent(
    /* [in] */ IContext* context,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)

    AutoPtr<IIntent> result;
    assert(0);
    // CIntent::New(context, ECLSID_CCallDetailActivity, (IIntent**)&result);
    // Check if the first item is a voicemail.
    if (!mVoicemailUri.IsNull()) {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->Parse(mVoicemailUri, (IUri**)&uri);
        result->PutExtra(ICallDetailActivity::EXTRA_VOICEMAIL_URI,
                IParcelable::Probe(uri));
    }
    result->PutBooleanExtra(ICallDetailActivity::EXTRA_VOICEMAIL_START_PLAYBACK, FALSE);

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
    REFCOUNT_ADD(*intent)
    return NOERROR;
}


//=================================================================
// IntentProvider
//=================================================================
CAR_INTERFACE_IMPL(IntentProvider, Object, IIntentProvider);

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
} // Droid
} // Elastos
