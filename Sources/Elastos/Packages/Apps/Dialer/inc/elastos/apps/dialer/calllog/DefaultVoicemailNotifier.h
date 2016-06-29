
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_DEFAULTVOICEMAILNOTIFIER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_DEFAULTVOICEMAILNOTIFIER_H__

#include "_Elastos.Apps.Dialer.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

/**
 * Implementation of {@link VoicemailNotifier} that shows a notification in the
 * status bar.
 */
class DefaultVoicemailNotifier
    : public Object
    , public IDefaultVoicemailNotifier
    , public IVoicemailNotifier
{
private:
    /** Information about a new voicemail. */
    class NewCall
        : public Object
        , public IDefaultVoicemailNotifierNewCall
    {
    public:
        CAR_INTERFACE_DECL();

        NewCall(
            /* [in] */ IUri* callsUri,
            /* [in] */ IUri* voicemailUri,
            /* [in] */ const String& number,
            /* [in] */ Int32 numberPresentation);

    public:
        AutoPtr<IUri> mCallsUri;
        AutoPtr<IUri> mVoicemailUri;
        String mNumber;
        Int32 mNumberPresentation;
    };

    /**
     * Default implementation of {@link NewCallsQuery} that looks up the list of new calls to
     * notify about in the call log.
     */
    class DefaultNewCallsQuery
        : public Object
        , public IDefaultVoicemailNotifierNewCallsQuery
    {
    public:
        CAR_INTERFACE_DECL();

        DefaultNewCallsQuery(
            /* [in] */ IContentResolver* contentResolver);

        CARAPI Query(
            /* [out, callee] */ ArrayOf<IDefaultVoicemailNotifierNewCall*>** newCalls);

    private:
         /** Returns an instance of {@link NewCall} created by using the values of the cursor. */
        AutoPtr<IDefaultVoicemailNotifierNewCall> CreateNewCallsFromCursor(
            /* [in] */ ICursor* cursor);

    private:
        static const String PROJECTION[];
        static const Int32 ID_COLUMN_INDEX; // = 0;
        static const Int32 NUMBER_COLUMN_INDEX; // = 1;
        static const Int32 VOICEMAIL_URI_COLUMN_INDEX; // = 2;
        static const Int32 NUMBER_PRESENTATION_COLUMN_INDEX; // = 3;

        AutoPtr<IContentResolver> mContentResolver;
    };

    /**
     * Default implementation of {@link NameLookupQuery} that looks up the name of a contact in the
     * contacts database.
     */
    class DefaultNameLookupQuery
        : public Object
        , public IDefaultVoicemailNotifierNameLookupQuery
    {
    public:
        CAR_INTERFACE_DECL();

        DefaultNameLookupQuery(
            /* [in] */ IContentResolver* contentResolver);

        CARAPI Query(
            /* [in] */ const String& number,
            /* [out] */ String* NewCalls);

    private:
        static const String PROJECTION[]; // = { PhoneLookup.DISPLAY_NAME };
        static const Int32 DISPLAY_NAME_COLUMN_INDEX; // = 0;

        AutoPtr<IContentResolver> mContentResolver;
    };

public:
    CAR_INTERFACE_DECL();

    /** Returns the singleton instance of the {@link DefaultVoicemailNotifier}. */
    static CARAPI_(AutoPtr<IDefaultVoicemailNotifier>) GetInstance(
        /* [in] */ IContext* context);

    /** Updates the notification and notifies of the call with the given URI. */
    // @Override
    CARAPI UpdateNotification(
        /* [in] */ IUri* newCallUri);

    // @Override
    CARAPI ClearNotification();

    /** Create a new instance of {@link NewCallsQuery}. */
    static CARAPI_(AutoPtr<IDefaultVoicemailNotifierNewCallsQuery>) CreateNewCallsQuery(
        /* [in] */ IContentResolver* contentResolver);

    /** Create a new instance of {@link NameLookupQuery}. */
    static CARAPI_(AutoPtr<IDefaultVoicemailNotifierNameLookupQuery>) CreateNameLookupQuery(
        /* [in] */ IContentResolver* contentResolver);

    /**
     * Create a new PhoneNumberHelper.
     * <p>
     * This will cause some Disk I/O, at least the first time it is created, so it should not be
     * called from the main thread.
     */
    static CARAPI_(AutoPtr<IPhoneNumberDisplayHelper>) CreatePhoneNumberHelper(
        /* [in] */ IContext* context);

private:
    DefaultVoicemailNotifier(
        /* [in] */ IContext* context,
        /* [in] */ INotificationManager* notificationManager,
        /* [in] */ IDefaultVoicemailNotifierNewCallsQuery* newCallsQuery,
        /* [in] */ IDefaultVoicemailNotifierNameLookupQuery* nameLookupQuery,
        /* [in] */ IPhoneNumberDisplayHelper* phoneNumberHelper);

    /** Creates a pending intent that marks all new voicemails as old. */
    AutoPtr<IPendingIntent> CreateMarkNewVoicemailsAsOldIntent();

public:
    static const String TAG; // = "DefaultVoicemailNotifier";

private:
    static Object sLock;

    /** The tag used to identify notifications from this class. */
    static const String NOTIFICATION_TAG; // = "DefaultVoicemailNotifier";
    /** The identifier of the notification of new voicemails. */
    static const Int32 NOTIFICATION_ID; // = 1;

    /** The singleton instance of {@link DefaultVoicemailNotifier}. */
    static AutoPtr<IDefaultVoicemailNotifier> sInstance;

    AutoPtr<IContext> mContext;
    AutoPtr<INotificationManager> mNotificationManager;
    AutoPtr<IDefaultVoicemailNotifierNewCallsQuery> mNewCallsQuery;
    AutoPtr<IDefaultVoicemailNotifierNameLookupQuery> mNameLookupQuery;
    AutoPtr<IPhoneNumberDisplayHelper> mPhoneNumberHelper;
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_DEFAULTVOICEMAILNOTIFIER_H__

