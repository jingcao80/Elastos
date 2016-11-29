
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/dialer/calllog/DefaultVoicemailNotifier.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/Map.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/R.h>
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "R.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Dialer::ECLSID_CCallDetailActivity;
using Elastos::Droid::Dialer::CallLog::EIID_IVoicemailNotifier;
using Elastos::Droid::Dialer::CallLog::EIID_IDefaultVoicemailNotifierNewCall;
using Elastos::Droid::Dialer::CallLog::EIID_IDefaultVoicemailNotifierNewCallsQuery;
using Elastos::Droid::Dialer::CallLog::EIID_IDefaultVoicemailNotifierNameLookupQuery;
using Elastos::Droid::Dialer::CallLog::ECLSID_CCallLogNotificationsService;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Provider::CCalls;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Provider::CContactsContractPhoneLookup;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::IO::ICloseable;
using Elastos::Utility::Etl::Map;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

//=================================================================
// DefaultVoicemailNotifier::NewCall
//=================================================================
CAR_INTERFACE_IMPL(DefaultVoicemailNotifier::NewCall, Object, IDefaultVoicemailNotifierNewCall);


//=================================================================
// DefaultVoicemailNotifier::DefaultNewCallsQuery
//=================================================================
static AutoPtr<ArrayOf<String> > InitProjection()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(4);
    (*projection)[0] = IBaseColumns::ID;
    (*projection)[1] = ICalls::NUMBER;
    (*projection)[2] = ICalls::VOICEMAIL_URI;
    (*projection)[3] = ICalls::NUMBER_PRESENTATION;
    return projection;
}
const AutoPtr<ArrayOf<String> > DefaultVoicemailNotifier::DefaultNewCallsQuery::PROJECTION= InitProjection();
const Int32 DefaultVoicemailNotifier::DefaultNewCallsQuery::ID_COLUMN_INDEX;
const Int32 DefaultVoicemailNotifier::DefaultNewCallsQuery::NUMBER_COLUMN_INDEX;
const Int32 DefaultVoicemailNotifier::DefaultNewCallsQuery::VOICEMAIL_URI_COLUMN_INDEX;
const Int32 DefaultVoicemailNotifier::DefaultNewCallsQuery::NUMBER_PRESENTATION_COLUMN_INDEX;

CAR_INTERFACE_IMPL(DefaultVoicemailNotifier::DefaultNewCallsQuery, Object, IDefaultVoicemailNotifierNewCallsQuery);

ECode DefaultVoicemailNotifier::DefaultNewCallsQuery::Query(
    /* [out, callee] */ ArrayOf<IDefaultVoicemailNotifierNewCall*>** newCalls)
{
    VALIDATE_NOT_NULL(newCalls)

    String selection("");
    selection.AppendFormat("%s = 1 AND %s = ?", ICalls::NEW.string(), ICalls::TYPE.string());
    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    (*selectionArgs)[0] = StringUtils::ToString(ICalls::VOICEMAIL_TYPE);

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
    while (cursor->MoveToNext(&succeeded), succeeded) {
        Int32 position;
        cursor->GetPosition(&position);
        result->Set(position, CreateNewCallsFromCursor(cursor));
    }
    *newCalls = result;
    REFCOUNT_ADD(*newCalls)
    // } finally {
    //     MoreCloseables.closeQuietly(cursor);
    // }
    ICloseable::Probe(cursor)->Close();
    return NOERROR;
}

AutoPtr<DefaultVoicemailNotifier::NewCall> DefaultVoicemailNotifier::DefaultNewCallsQuery::CreateNewCallsFromCursor(
    /* [in] */ ICursor* cursor)
{
    String voicemailUriString;
    cursor->GetString(VOICEMAIL_URI_COLUMN_INDEX, &voicemailUriString);

    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> uri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&uri);
    Int64 index;
    cursor->GetInt64(ID_COLUMN_INDEX, &index);
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    AutoPtr<IUri> callsUri;
    contentUris->WithAppendedId(uri, index, (IUri**)&callsUri);

    AutoPtr<IUri> voicemailUri;
    if (!voicemailUriString.IsNull()) {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        helper->Parse(voicemailUriString, (IUri**)&voicemailUri);
    }

    String number;
    cursor->GetString(NUMBER_COLUMN_INDEX, &number);
    Int32 presentation;
    cursor->GetInt32(NUMBER_PRESENTATION_COLUMN_INDEX, &presentation);
    AutoPtr<DefaultVoicemailNotifier::NewCall> newCall
            =  new DefaultVoicemailNotifier::NewCall(callsUri, voicemailUri, number, presentation);

    return newCall;
}

//=================================================================
// DefaultVoicemailNotifier::DefaultNameLookupQuery
//=================================================================
static AutoPtr<ArrayOf<String> > IntitLookupProjction()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
    (*projection)[0] = IContactsContractContactsColumns::DISPLAY_NAME;
    return projection;
}
const AutoPtr<ArrayOf<String> > DefaultVoicemailNotifier::DefaultNameLookupQuery::PROJECTION = IntitLookupProjction();
const Int32 DefaultVoicemailNotifier::DefaultNameLookupQuery::DISPLAY_NAME_COLUMN_INDEX;

CAR_INTERFACE_IMPL(DefaultVoicemailNotifier::DefaultNameLookupQuery, Object, IDefaultVoicemailNotifierNameLookupQuery)

ECode DefaultVoicemailNotifier::DefaultNameLookupQuery::Query(
    /* [in] */ const String& number,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    AutoPtr<ICursor> cursor;
    // try {
    AutoPtr<IContactsContractPhoneLookup> phoneLookup;
    CContactsContractPhoneLookup::AcquireSingleton((IContactsContractPhoneLookup**)&phoneLookup);
    AutoPtr<IUri> uri;
    phoneLookup->GetCONTENT_FILTER_URI((IUri**)&uri);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    String encode;
    helper->Encode(number, &encode);
    AutoPtr<IUri> newUri;
    helper->WithAppendedPath(uri, encode, (IUri**)&newUri);
    FAIL_GOTO(mContentResolver->Query(newUri, PROJECTION,
            String(NULL), NULL, String(NULL), (ICursor**)&cursor), exit);

    Boolean succeeded;
    if (cursor == NULL || (cursor->MoveToFirst(&succeeded), !succeeded)) {
        goto exit;
    }
    cursor->GetString(DISPLAY_NAME_COLUMN_INDEX, result);

exit:
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    return NOERROR;
    // } finally {
    //     if (cursor != null) {
    //         cursor.close();
    //     }
    // }
}

//=================================================================
// DefaultVoicemailNotifier
//=================================================================
const String DefaultVoicemailNotifier::TAG("DefaultVoicemailNotifier");
const String DefaultVoicemailNotifier::NOTIFICATION_TAG("DefaultVoicemailNotifier");
const Int32 DefaultVoicemailNotifier::NOTIFICATION_ID;
AutoPtr<DefaultVoicemailNotifier> DefaultVoicemailNotifier::sInstance;

CAR_INTERFACE_IMPL(DefaultVoicemailNotifier, Object, IVoicemailNotifier)

DefaultVoicemailNotifier::DefaultVoicemailNotifier(
    /* [in] */ IContext* context,
    /* [in] */ INotificationManager* notificationManager,
    /* [in] */ IDefaultVoicemailNotifierNewCallsQuery* newCallsQuery,
    /* [in] */ IDefaultVoicemailNotifierNameLookupQuery* nameLookupQuery,
    /* [in] */ PhoneNumberDisplayHelper* phoneNumberHelper)
    : mContext(context)
    , mNotificationManager(notificationManager)
    , mNewCallsQuery(newCallsQuery)
    , mNameLookupQuery(nameLookupQuery)
    , mPhoneNumberHelper(phoneNumberHelper)
{}

AutoPtr<DefaultVoicemailNotifier> DefaultVoicemailNotifier::GetInstance(
    /* [in] */ IContext* context)
{
    AutoLock syncLock(sInstance);
    if (sInstance == NULL) {
        AutoPtr<IInterface> service;
        context->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&service);
        AutoPtr<INotificationManager> notificationManager = INotificationManager::Probe(service);
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);
        sInstance = new DefaultVoicemailNotifier(
                context, notificationManager,
                CreateNewCallsQuery(contentResolver),
                CreateNameLookupQuery(contentResolver),
                CreatePhoneNumberHelper(context));
    }
    return sInstance;
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
    Map<String, String> names;

    // Determine the call corresponding to the new voicemail we have to notify about.
    AutoPtr<NewCall> callToNotify;

    // Iterate over the new voicemails to determine all the information above.
    for (Int32 i = 0; i < newCalls->GetLength(); i++) {
        AutoPtr<NewCall> newCall = (NewCall*)(*newCalls)[i];
        // Check if we already know the name associated with this number.
        String name;
        Map<String, String>::Iterator it = names.Find(newCall->mNumber);
        if (it != names.End()) {
            name = it->mSecond;
        }
        if (name.IsNull()) {
            AutoPtr<ICharSequence> cs;
            CString::New(newCall->mNumber, (ICharSequence**)&cs);
            mPhoneNumberHelper->GetDisplayName(cs,
                    newCall->mNumberPresentation)->ToString(&name);
            // If we cannot lookup the contact, use the number instead.
            if (TextUtils::IsEmpty(name)) {
                // Look it up in the database.
                mNameLookupQuery->Query(newCall->mNumber, &name);
                if (TextUtils::IsEmpty(name)) {
                    name = newCall->mNumber;
                }
            }
            names[newCall->mNumber] = name;
            // This is a new caller. Add it to the back of the list of callers.
            if (TextUtils::IsEmpty(callers)) {
                callers = name;
            }
            else {
                AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
                args->Set(0, CoreUtils::Convert(callers));
                args->Set(1, CoreUtils::Convert(name));
                resources->GetString(R::string::notification_voicemail_callers_list,
                         args, &callers);
            }
        }
        // Check if this is the new call we need to notify about.
        Boolean equals;
        if (newCallUri != NULL &&
                (IObject::Probe(newCallUri)->Equals(newCall->mVoicemailUri, &equals), equals)) {
            callToNotify = newCall;
        }
    }

    if (newCallUri != NULL && callToNotify == NULL) {
        Logger::E(TAG, "The new call could not be found in the call log: %s", TO_CSTR(newCallUri));
    }

    // Determine the title of the notification and the icon for it.
    String title;
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(newCalls->GetLength()));
    resources->GetQuantityString(R::plurals::notification_voicemail_title,
             newCalls->GetLength(), args, &title);
    // TODO: Use the photo of contact if all calls are from the same person.
    Int32 icon = Elastos::Droid::R::drawable::stat_notify_voicemail;

    AutoPtr<INotificationBuilder> notificationBuilder;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&notificationBuilder);
    notificationBuilder->SetSmallIcon(icon);
    notificationBuilder->SetContentTitle(CoreUtils::Convert(title));
    notificationBuilder->SetContentText(CoreUtils::Convert(callers));
    Int32 color;
    resources->GetColor(Elastos::Droid::Dialer::R::color::dialer_theme_color, &color);
    notificationBuilder->SetColor(color);
    notificationBuilder->SetDefaults(callToNotify != NULL ? INotification::DEFAULT_ALL : 0);
    notificationBuilder->SetDeleteIntent(CreateMarkNewVoicemailsAsOldIntent());
    notificationBuilder->SetAutoCancel(TRUE);

    // Determine the intent to fire when the notification is clicked on.
    AutoPtr<IIntent> contentIntent;
    if (newCalls->GetLength() == 1) {
        // Open the voicemail directly.
        AutoPtr<NewCall> newCall = (NewCall*)(*newCalls)[0];
        CIntent::New(mContext, ECLSID_CCallDetailActivity, (IIntent**)&contentIntent);
        contentIntent->SetData(newCall->mCallsUri);
        contentIntent->PutExtra(ICallDetailActivity::EXTRA_VOICEMAIL_URI,
                IParcelable::Probe(newCall->mVoicemailUri));

        AutoPtr<IIntent> playIntent;
        CIntent::New(mContext, ECLSID_CCallDetailActivity, (IIntent**)&playIntent);
        playIntent->SetData(newCall->mCallsUri);
        playIntent->PutExtra(ICallDetailActivity::EXTRA_VOICEMAIL_URI,
                IParcelable::Probe(newCall->mVoicemailUri));
        playIntent->PutBooleanExtra(ICallDetailActivity::EXTRA_VOICEMAIL_START_PLAYBACK, TRUE);
        playIntent->PutBooleanExtra(ICallDetailActivity::EXTRA_FROM_NOTIFICATION, TRUE);

        String action;
        resources->GetString(R::string::notification_action_voicemail_play, &action);
        AutoPtr<IPendingIntentHelper> helper;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
        AutoPtr<IPendingIntent> pendingIntent;
        helper->GetActivity(mContext, 0, playIntent, 0, (IPendingIntent**)&pendingIntent);
        notificationBuilder->AddAction(Elastos::Droid::Dialer::R::drawable::ic_play_holo_dark,
                CoreUtils::Convert(action), pendingIntent);
    }
    else {
        // Open the call log.
        AutoPtr<ICalls> calls;
        CCalls::AcquireSingleton((ICalls**)&calls);
        AutoPtr<IUri> uri;
        calls->GetCONTENT_URI((IUri**)&uri);
        CIntent::New(IIntent::ACTION_VIEW, uri, (IIntent**)&contentIntent);
        contentIntent->PutExtra(ICalls::EXTRA_CALL_TYPE_FILTER, ICalls::VOICEMAIL_TYPE);
    }
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetActivity(mContext, 0, contentIntent, 0, (IPendingIntent**)&pendingIntent);
    notificationBuilder->SetContentIntent(pendingIntent);

    // The text to show in the ticker, describing the new event.
    if (callToNotify != NULL) {
        String value;
        Map<String, String>::Iterator it = names.Find(callToNotify->mNumber);
        if (it != names.End()) {
            value = it->mSecond;
        }
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(value));
        String str;
        resources->GetString(
                Elastos::Droid::Dialer::R::string::notification_new_voicemail_ticker, args, &str);
        notificationBuilder->SetTicker(CoreUtils::Convert(str));
    }

    AutoPtr<INotification> notification;
    notificationBuilder->Build((INotification**)&notification);
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

AutoPtr<PhoneNumberDisplayHelper> DefaultVoicemailNotifier::CreatePhoneNumberHelper(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<PhoneNumberDisplayHelper> helper = new PhoneNumberDisplayHelper(resources);
    return helper;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
