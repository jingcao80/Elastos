
#include "elastos/apps/dialer/calllog/DefaultVoicemailNotifier.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Provider::CCalls;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Provider::CContactsContractPhoneLookup;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

const String DefaultVoicemailNotifier::DefaultNewCallsQuery::PROJECTION[]= {
    ICalls::_ID,
    ICalls::NUMBER,
    ICalls::VOICEMAIL_URI,
    ICalls::NUMBER_PRESENTATION
};
const Int32 DefaultVoicemailNotifier::DefaultNewCallsQuery::ID_COLUMN_INDEX = 0;
const Int32 DefaultVoicemailNotifier::DefaultNewCallsQuery::NUMBER_COLUMN_INDEX = 1;
const Int32 DefaultVoicemailNotifier::DefaultNewCallsQuery::VOICEMAIL_URI_COLUMN_INDEX = 2;
const Int32 DefaultVoicemailNotifier::DefaultNewCallsQuery::NUMBER_PRESENTATION_COLUMN_INDEX = 3;

const String DefaultVoicemailNotifier::DefaultNameLookupQuery::PROJECTION[] = {
    IContactsContractPhoneLookup::DISPLAY_NAME
};

const Int32 DefaultVoicemailNotifier::DefaultNameLookupQuery::DISPLAY_NAME_COLUMN_INDEX = 0;

const String DefaultVoicemailNotifier::TAG("DefaultVoicemailNotifier)";

const String DefaultVoicemailNotifier::NOTIFICATION_TAG("DefaultVoicemailNotifier");

const Int32 DefaultVoicemailNotifier::NOTIFICATION_ID = 1;

//=================================================================
// DefaultVoicemailNotifier::NewCall
//=================================================================
CAR_INTERFACE_IMPL(DefaultVoicemailNotifier::NewCall, Object, IDefaultVoicemailNotifierNewCall);

DefaultVoicemailNotifier::NewCall::NewCall(
    /* [in] */ IUri* callsUri,
    /* [in] */ IUri* voicemailUri,
    /* [in] */ String number,
    /* [in] */ Int32 numberPresentation)
    : mCallsUri(callsUri)
    , mVoicemailUri(voicemailUri)
    , mNumberPresentation(numberPresentation)
{
    mNumber = number;
}


//=================================================================
// DefaultVoicemailNotifier::DefaultNewCallsQuery
//=================================================================
CAR_INTERFACE_IMPL(DefaultVoicemailNotifier::DefaultNewCallsQuery, Object, IDefaultVoicemailNotifierNewCallsQuery);

DefaultVoicemailNotifier::DefaultNewCallsQuery::DefaultNewCallsQuery(
    /* [in] */ IContentResolver* contentResolver)
    : mContentResolver(contentResolver)
{}

ECode DefaultVoicemailNotifier::DefaultNewCallsQuery::Query(
    /* [out, callee] */ ArrayOf<IDefaultVoicemailNotifierNewCall*>** newCalls)
{
    VALIDATE_NOT_NULL(newCalls);
    String selection = ICalls::NEW + " = 1 AND " + ICalls::TYPE + " = ?";
    String selectionArgs[] = {
        StringUtils::ParseInt32(ICalls::VOICEMAIL_TYPE)
    }

    AutoPtr<ICursor> cursor;
    // try {
    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> uri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&uri);
    mContentResolver->Query(uri, PROJECTION,
            selection, selectionArgs, ICalls::DEFAULT_SORT_ORDER, (ICursor**)&cursor);
    if (cursor == NULL) {
        *newCalls = NULL;
        return NOERROR;
    }

    Int32 count;
    cursor->GetCount(&count);
    AutoPtr<ArrayOf<IDefaultVoicemailNotifierNewCall*> > result =
            ArrayOf<IDefaultVoicemailNotifierNewCall*>::Alloc(count);

    Boolean succeeded;
    FAILE_GOTO(cursor->MoveToNext(&succeeded), exit);
    Int32 position;
    while (succeeded) {
        cursor->GetPosition(&position);
        result->Set(position, CreateNewCallsFromCursor(cursor));
    }
    *newCalls = result;
    REFCOUNT_ADD(*newCalls);
    // } finally {
    //     MoreCloseables.closeQuietly(cursor);
    // }
exit:
    assert(0 && "TODO")
    // MoreCloseables::CloseQuietly(cursor);
    return NOERROR;
}

AutoPtr<IDefaultVoicemailNotifierNewCall> DefaultVoicemailNotifier::DefaultNewCallsQuery::CreateNewCallsFromCursor(
    /* [in] */ ICursor* cursor)
{
    String voicemailUriString;
    cursor->GetString(VOICEMAIL_URI_COLUMN_INDEX, &voicemailUriString);

    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> uri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&uri);
    Int64 index;
    cursor->GetLong(ID_COLUMN_INDEX, &index);
    AutoPtr<IUri> callsUri;
    contentUris->WithAppendedId(uri, index, (IUri**)&callsUri);

    AutoPtr<IUri> voicemailUri = voicemailUriString.IsNull() ? NULL : Uri::Parse(voicemailUriString);

    String number;
    cursor->GetString(NUMBER_COLUMN_INDEX, &number);
    Int32 presentation;
    cursor->GetInt32(NUMBER_PRESENTATION_COLUMN_INDEX, &presentation);
    AutoPtr<NewCall> newCall =  new NewCall(callsUri, voicemailUri, number, presentation);

    return (IDefaultVoicemailNotifierNewCall*)newCall;
}

//=================================================================
// DefaultVoicemailNotifier::DefaultNameLookupQuery
//=================================================================
CAR_INTERFACE_IMPL(DefaultVoicemailNotifier::DefaultNameLookupQuery, Object, IDefaultVoicemailNotifierNameLookupQuery);

DefaultVoicemailNotifier::DefaultNameLookupQuery::DefaultNameLookupQuery(
    /* [in] */ IContentResolver* contentResolver)
    : mContentResolver(contentResolver)
{}

ECode DefaultVoicemailNotifier::DefaultNameLookupQuery::Query(
    /* [out, callee] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICursor> cursor;
    // try {
    AutoPtr<IContactsContractPhoneLookup> phoneLookup;
    CContactsContractPhoneLookup::AcquireSingleton(
            (IContactsContractPhoneLookup**)&phoneLookup);
    AutoPtr<IUri> uri;
    phoneLookup->GetCONTENT_FILTER_URI((IUri**)&uri);

    String encode;
    Uri::Encode(number, &encode);
    AutoPtr<IUri> newUri;
    Uri::WithAppendedPath(uri, encode, (IUri**)&newUri);
    FAILE_GOTO(mContentResolver->Query(newUri,
            PROJECTION, NULL, NULL, NULL, (ICursor**)&cursor), exit);

    Boolean succeeded = FALSE;
    if (cursor == NULL || cursor->MoveToFirst(&succeeded), !succeeded) {
        *result = String(NULL);
        goto exit;
    }
    cursor->GetString(DISPLAY_NAME_COLUMN_INDEX, result);

exit:
    if (cursor != NULL) {
        cursor->Close();
    }
    return ec;
    // } finally {
    //     if (cursor != null) {
    //         cursor.close();
    //     }
    // }
}

//=================================================================
// DefaultVoicemailNotifier
//=================================================================
CAR_INTERFACE_IMPL_2(DefaultVoicemailNotifier, Object, IDefaultVoicemailNotifier, IVoicemailNotifier);

DefaultVoicemailNotifier::DefaultVoicemailNotifier(
    /* [in] */ IContext* context,
    /* [in] */ INotificationManager* notificationManager,
    /* [in] */ IDefaultVoicemailNotifierNewCallsQuery* newCallsQuery,
    /* [in] */ IDefaultVoicemailNotifierNameLookupQuery* nameLookupQuery,
    /* [in] */ IPhoneNumberDisplayHelper* phoneNumberHelper)
    : mContext(context)
    , mNotificationManager(notificationManager)
    , mNewCallsQuery(newCallsQuery)
    , mNameLookupQuery(nameLookupQuery)
    , mPhoneNumberHelper(phoneNumberHelper)
{}

AutoPtr<IDefaultVoicemailNotifier> DefaultVoicemailNotifier::GetInstance(
    /* [in] */ IContext* context)
{
    {    AutoLock syncLock(sLock);
        if (sInstance == NULL) {
            AutoPtr<IInterface> service;
            context->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&service);
            INotificationManager* notificationManager = INotificationManager::Probe(service);
            AutoPtr<IContentResolver> contentResolver;
            context->getContentResolver((IContentResolver**)&contentResolver);
            AutoPtr<DefaultVoicemailNotifier> notifier =
                    new DefaultVoicemailNotifier(context, notificationManager,
                    CreateNewCallsQuery(contentResolver),
                    CreateNameLookupQuery(contentResolver),
                    CreatePhoneNumberHelper(context));
            sInstance = (IDefaultVoicemailNotifier*)notifier;
        }
        return sInstance;
    }
}

ECode DefaultVoicemailNotifier::UpdateNotification(
    /* [in] */ IUri* newCallUri)
{
    // Lookup the list of new voicemails to include in the notification.
    // TODO: Move this into a service, to avoid holding the receiver up.
    AutoPtr<ArrayOf<IDefaultVoicemailNotifierNewCall*> > newCalls;
    mNewCallsQuery->Query((ArrayOf<IDefaultVoicemailNotifierNewCall*>**)&newCalls);

    if (newCalls == NULL) {
        // Query failed, just return.
        return NOERROR;
    }

    if (newCalls->GetLength() == 0) {
        // No voicemails to notify about: clear the notification.
        ClearNotification();
        return NOERROR;
    }

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);

    // This represents a list of names to include in the notification.
    String callers;

    // Maps each number into a name: if a number is in the map, it has already left a more
    // recent voicemail.
    AutoPtr<IHashMap> names;
    CHashMap::New((IHashMap**)&names);
    // final Map<String, String> names = Maps.newHashMap();

    // Determine the call corresponding to the new voicemail we have to notify about.
    AutoPtr<IDefaultVoicemailNotifierNewCall> callToNotify;

    // Iterate over the new voicemails to determine all the information above.
    for (Int32 i = 0; i < newCalls->GetLength(); i++) {
        NewCall* newCall = (NewCall*)(*newCalls)[i];
        // Check if we already know the name associated with this number.
        AutoPtr<IInterface> value;
        names->Get(CoreUtils::Convert(newCall->mNumber), (IInterface**)&value);
        String name = CoreUtils::Unbox(value);
        if (name.IsNull()) {
            AutoPtr<ICharSequence> displayName = mPhoneNumberHelper->GetDisplayName(
                    newCall->mNumber, newCall->mNumberPresentation);
            displayName->ToString(&name);
            // If we cannot lookup the contact, use the number instead.
            if (TextUtils::IsEmpty(name)) {
                // Look it up in the database.
                mNameLookupQuery->Query(newCall->mNumber, &name);
                if (TextUtils::IsEmpty(name)) {
                    name = newCall->mNumber;
                }
            }
            names->Put(CoreUtils::Convert(newCall->mNumber), CoreUtils::Convert(name));
            // This is a new caller. Add it to the back of the list of callers.
            if (TextUtils::IsEmpty(callers)) {
                callers = name;
            }
            else {
                resources->GetString(R::string::notification_voicemail_callers_list,
                         callers, name, &callers);
            }
        }
        // Check if this is the new call we need to notify about.
        if (newCallUri != NULL && IObject::Probe(newCallUri)->Equals(newCall->mVoicemailUri)) {
            callToNotify = (IDefaultVoicemailNotifierNewCall*)newCall;
        }
    }

    if (newCallUri != NULL && callToNotify == NULL) {
        String str;
        newCallUri->ToString(&str);
        Logger::E(TAG, "The new call could not be found in the call log: %s", str.string());
    }

    // Determine the title of the notification and the icon for it.
    String title;
    resources->GetQuantityString(R::plurals::notification_voicemail_title,
             newCalls->GetLength(), newCalls->GetLength(), &the);
    // TODO: Use the photo of contact if all calls are from the same person.
    Int32 icon = Elastos::R::drawable::stat_notify_voicemail;

    AutoPtr<INotificationBuilder> notificationBuilder;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&notificationBuilder);
    notificationBuilder->SetSmallIcon(icon);
    notificationBuilder->SetContentTitle(title);
    notificationBuilder->SetContentText(callers);
    Int32 color;
    resources->GetColor(R::color::dialer_theme_color, &color);
    notificationBuilder->SetColor(color);
    notificationBuilder->SetDefaults(callToNotify != NULL ? INotification::DEFAULT_ALL : 0);
    notificationBuilder->SsetDeleteIntent(CreateMarkNewVoicemailsAsOldIntent());
    notificationBuilder->SetAutoCancel(TRUE);

    // Determine the intent to fire when the notification is clicked on.
    AutoPtr<IIntent> contentIntent;
    if (newCalls->GetLength() == 1) {
        // Open the voicemail directly.
        CIntent::New(mContext, ECLSID_CCallDetailActivity, (IIntent**)&contentIntent);
        contentIntent->SetData((*newCalls)[0]->mCallsUri);
        contentIntent->PutExtra(ICallDetailActivity::EXTRA_VOICEMAIL_URI,
                (*newCalls)[0]->mVoicemailUri);

        AutoPtr<IIntent> playIntent;
        CIntent::New(mContext, ECLSID_CCallDetailActivity, (IIntent**)&playIntent);
        playIntent.setData((*newCalls)[0]->callsUri);
        playIntent.putExtra(CallDetailActivity.EXTRA_VOICEMAIL_URI,
                (*newCalls)[0]->mVoicemailUri);
        playIntent->PutBooleanExtra(ICallDetailActivity::EXTRA_VOICEMAIL_START_PLAYBACK, TRUE);
        playIntent->PutBooleanExtra(ICallDetailActivity::EXTRA_FROM_NOTIFICATION, TRUE);

        String action;
        resources->GetString(R::string::notification_action_voicemail_play, &action);
        AutoPtr<IPendingIntentHelper> helper;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
        AutoPtr<IPendingIntent> pendingIntent;
        helper->GetActivity(mContext, 0, playIntent, 0, (IPendingIntent**)&pendingIntent);
        notificationBuilder->AddAction(R::drawable::ic_play_holo_dark,
                action, pendingIntent);
    }
    else {
        // Open the call log.
        AutoPtr<ICalls> calls;
        CCalls::AcquireSingleton((ICalls**)&calls);
        AutoPtr<IUri> uri;
        calls->GetCONTENT_URI((IUri**)&uri);
        CIntent::New(IIntent::ACTION_VIEW, uri, (IIntent**)&contentIntent);
        contentIntent->PutExtra(ICalls::EXTRA_CALL_TYPE_FILTER, IIntent::ACTION_VIEWVOICEMAIL_TYPE);
    }
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetActivity(mContext, 0, contentIntent, 0, (IPendingIntent**)&pendingIntent);
    notificationBuilder->SetContentIntent(pendingIntent);

    // The text to show in the ticker, describing the new event.
    if (callToNotify != NULL) {
        AutoPtr<IInterface> value;
        names->Get(callToNotify->mNumber, (IInterface**)&value);
        String str;
        resources->GetString(R::string::notification_new_voicemail_ticker,
                CoreUtils::Unbox(value), &str);
        notificationBuilder->SetTicker(str);
    }

    AutoPtr<INotification> notification;
    notificationBuilder::Build((INotification**)&notification);
    mNotificationManager->Notify(NOTIFICATION_TAG, NOTIFICATION_ID, notification);

    return NOERROR;
}

AutoPtr<IPendingIntent> DefaultVoicemailNotifier::CreateMarkNewVoicemailsAsOldIntent()
{
    AutoPtr<IIntent> intent;
    CIntent::New(mContext, ECLSID_CCallLogNotificationsService, (IIntent**)&intent);
    intent->SetAction(ICallLogNotificationsService::ACTION_MARK_NEW_VOICEMAILS_AS_OLD);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetService(mContext, 0, intent, 0, (IPendingIntent**)&pendingIntent);

    return pendingIntent;
}

ECode DefaultVoicemailNotifier::ClearNotification()
{
    return mNotificationManager->Cancel(NOTIFICATION_TAG, NOTIFICATION_ID);
}

AutoPtr<IDefaultVoicemailNotifierNewCallsQuery> DefaultVoicemailNotifier::CreateNewCallsQuery(
    /* [in] */ IContentResolver* contentResolver)
{
    AutoPtr<DefaultNewCallsQuery> query = new DefaultNewCallsQuery(contentResolver);
    return (IDefaultVoicemailNotifierNewCallsQuery*)query;
}

AutoPtr<IDefaultVoicemailNotifierNameLookupQuery> DefaultVoicemailNotifier::CreateNameLookupQuery(
    /* [in] */ IContentResolver* contentResolver)
{
    AutoPtr<DefaultNameLookupQuery> query = new DefaultNameLookupQuery(contentResolver);
    return (IDefaultVoicemailNotifierNameLookupQuery*)query;
}

AutoPtr<IPhoneNumberDisplayHelper> DefaultVoicemailNotifier::CreatePhoneNumberHelper(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<PhoneNumberDisplayHelper> helper = new PhoneNumberDisplayHelper(resources);
    return (IPhoneNumberDisplayHelper*)helper;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
