#ifndef __ELASTOS_DROID_DIALER_CALLDETAILACTIVITYQUERYHANDLER_H__
#define __ELASTOS_DROID_DIALER_CALLDETAILACTIVITYQUERYHANDLER_H__

#include "elastos/droid/contacts/common/database/NoNullCursorAsyncQueryHandler.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Contacts::Common::Database::NoNullCursorAsyncQueryHandler;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Dialer {

class CCallDetailActivity;

/**
 * Class used by {@link CallDetailActivity} to fire async content resolver queries.
 */
class CallDetailActivityQueryHandler
    : public NoNullCursorAsyncQueryHandler
{
public:
    CallDetailActivityQueryHandler();

    CARAPI constructor(
        /* [in] */ CCallDetailActivity* callDetailActivity);

    /**
     * Fires a query to update voicemail status for the given voicemail record. On completion of the
     * query a call to {@link CallDetailActivity#updateVoicemailStatusMessage(Cursor)} is made.
     * <p>
     * if this is a voicemail record then it makes up to two asynchronous content resolver queries.
     * The first one to fetch voicemail content details and check if the voicemail record has audio.
     * If the voicemail record does not have an audio yet then it fires the second query to get the
     * voicemail status of the associated source.
     */
    CARAPI StartVoicemailStatusQuery(
        /* [in] */ IUri* voicemailUri);

protected:
    // @Override
    CARAPI OnNotNullableQueryComplete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ ICursor* cursor);

private:
    /** Check that the cursor is non-null and can be moved to first. */
    CARAPI_(Boolean) MoveToFirst(
        /* [in] */ ICursor* cursor);

    CARAPI_(Boolean) HasNoAudio(
        /* [in] */ ICursor* voicemailCursor);

    CARAPI_(String) GetSourcePackage(
        /* [in] */ ICursor* voicemailCursor);

private:
    static const String TAG;
    static const Int32 QUERY_VOICEMAIL_CONTENT_TOKEN = 101;
    static const Int32 QUERY_VOICEMAIL_STATUS_TOKEN = 102;

    AutoPtr<ArrayOf<String> > VOICEMAIL_CONTENT_PROJECTION;

    static const Int32 SOURCE_PACKAGE_COLUMN_INDEX = 0;
    static const Int32 HAS_CONTENT_COLUMN_INDEX = 1;

    AutoPtr<CCallDetailActivity> mCallDetailActivity;
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLDETAILACTIVITYQUERYHANDLER_H__
