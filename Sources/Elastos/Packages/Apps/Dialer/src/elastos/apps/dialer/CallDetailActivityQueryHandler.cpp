
#include "elastos/apps/dialer/CallDetailActivityQueryHandler.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Provider::IVoicemailContractStatus;
using Elastos::Droid::Provider::CVoicemailContractStatus;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Apps {
namespace Dialer {

const String CallDetailActivityQueryHandler::TAG("CallDetail");
const Int32 CallDetailActivityQueryHandler::QUERY_VOICEMAIL_CONTENT_TOKEN = 101;
const Int32 CallDetailActivityQueryHandler::QUERY_VOICEMAIL_STATUS_TOKEN = 102;
const Int32 CallDetailActivityQueryHandler::SOURCE_PACKAGE_COLUMN_INDEX = 0;
const Int32 CallDetailActivityQueryHandler::HAS_CONTENT_COLUMN_INDEX = 1;

CAR_INTERFACE_IMPL(CallDetailActivityQueryHandler,
        /*NoNullCursorAsyncQueryHandler*/Object, ICallDetailActivityQueryHandler);

CallDetailActivityQueryHandler::CallDetailActivityQueryHandler()
{
    VOICEMAIL_CONTENT_PROJECTION = ArrayOf<String>::Alloc(2);
    VOICEMAIL_CONTENT_PROJECTION->Set(0, IVoicemailContractVoicemails::SOURCE_PACKAGE);
    VOICEMAIL_CONTENT_PROJECTION->Set(1, IVoicemailContractVoicemails::HAS_CONTENT);
}

ECode CallDetailActivityQueryHandler::constructor(
    /* [in] */ ICallDetailActivity* callDetailActivity)
{
    AutoPtr<IContentResolver> resolver;
    callDetailActivity->GetContentResolver((IContentResolver**)&resolver);
    asser(0 && "TODO");
    // NoNullCursorAsyncQueryHandler::constructor(resolver);
    mCallDetailActivity = callDetailActivity;
    return NOERROR;
}

ECode CallDetailActivityQueryHandler::StartVoicemailStatusQuery(
    /* [in] */ IUri* voicemailUri)
{
    return StartQuery(QUERY_VOICEMAIL_CONTENT_TOKEN, NULL, voicemailUri,
            VOICEMAIL_CONTENT_PROJECTION, String(NULL), NULL, String(NULL));
}

ECode CallDetailActivityQueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    // try {
    if (token == QUERY_VOICEMAIL_CONTENT_TOKEN) {
        // Query voicemail status only if this voicemail record does not have audio.
        if (MoveToFirst(cursor) && HasNoAudio(cursor)) {
            AutoPtr<IVoicemailContractStatus> status;
            CVoicemailContractStatus::AcquireSingleton((IVoicemailContractStatus**)&status);
            AutoPtr<IUri> uri;
            status->BuildSourceUri(GetSourcePackage(cursor), (IUri**)&uri);
            StartQuery(QUERY_VOICEMAIL_STATUS_TOKEN, NULL, uri,
                    VoicemailStatusHelperImpl::PROJECTION, String(NULL), NULL, String(NULL));
        }
        else {
            // nothing to show in status
            mCallDetailActivity->UpdateVoicemailStatusMessage(NULL);
        }
    }
    else if (token == QUERY_VOICEMAIL_STATUS_TOKEN) {
        mCallDetailActivity->UpdateVoicemailStatusMessage(cursor);
    }
    else {
        Logger::W(TAG, "Unknown query completed: ignoring: %d", token);
    }
    // } finally {
    //     MoreCloseables.closeQuietly(cursor);
    // }

    assert(0 && "TODO");
    // MoreCloseables.closeQuietly(cursor);
    return NOERROR;
}

Boolean CallDetailActivityQueryHandler::MoveToFirst(
    /* [in] */ ICursor* cursor)
{
    Boolean succeeded;
    if (cursor == NULL || cursor->MoveToFirst(&succeeded), !succeeded) {
        Logger::E(TAG, "Cursor not valid, could not move to first");
        return FALSE;
    }
    return TRUE;
}

Boolean CallDetailActivityQueryHandler::HasNoAudio(
    /* [in] */ ICursor* voicemailCursor)
{
    Int32 value;
    voicemailCursor->GetInt32(HAS_CONTENT_COLUMN_INDEX, &value);
    return value == 0;
}

String CallDetailActivityQueryHandler::GetSourcePackage(
    /* [in] */ ICursor* voicemailCursor)
{
    String str;
    voicemailCursor->GetString(SOURCE_PACKAGE_COLUMN_INDEX, &str);
    return str;
}

} // Dialer
} // Apps
} // Elastos
