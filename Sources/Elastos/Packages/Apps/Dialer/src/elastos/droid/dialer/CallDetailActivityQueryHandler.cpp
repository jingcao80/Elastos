//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/dialer/CallDetailActivityQueryHandler.h"
#include "elastos/droid/dialer/CCallDetailActivity.h"
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperImpl.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Dialer::Voicemail::VoicemailStatusHelperImpl;
using Elastos::Droid::Provider::IVoicemailContractStatus;
using Elastos::Droid::Provider::CVoicemailContractStatus;
using Elastos::Droid::Provider::IVoicemails;
using Elastos::Core::AutoLock;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {

const String CallDetailActivityQueryHandler::TAG("CallDetail");
const Int32 CallDetailActivityQueryHandler::QUERY_VOICEMAIL_CONTENT_TOKEN;
const Int32 CallDetailActivityQueryHandler::QUERY_VOICEMAIL_STATUS_TOKEN;
const Int32 CallDetailActivityQueryHandler::SOURCE_PACKAGE_COLUMN_INDEX;
const Int32 CallDetailActivityQueryHandler::HAS_CONTENT_COLUMN_INDEX;

CallDetailActivityQueryHandler::CallDetailActivityQueryHandler()
{
    VOICEMAIL_CONTENT_PROJECTION = ArrayOf<String>::Alloc(2);
    VOICEMAIL_CONTENT_PROJECTION->Set(0, IVoicemails::SOURCE_PACKAGE);
    VOICEMAIL_CONTENT_PROJECTION->Set(1, IVoicemails::HAS_CONTENT);
}

ECode CallDetailActivityQueryHandler::constructor(
    /* [in] */ CCallDetailActivity* callDetailActivity)
{
    AutoPtr<IContentResolver> resolver;
    callDetailActivity->GetContentResolver((IContentResolver**)&resolver);
    NoNullCursorAsyncQueryHandler::constructor(resolver);
    mCallDetailActivity = callDetailActivity;
    return NOERROR;
}

ECode CallDetailActivityQueryHandler::StartVoicemailStatusQuery(
    /* [in] */ IUri* voicemailUri)
{
    return StartQuery(QUERY_VOICEMAIL_CONTENT_TOKEN, NULL, voicemailUri,
            VOICEMAIL_CONTENT_PROJECTION, String(NULL), NULL, String(NULL));
}

ECode CallDetailActivityQueryHandler::OnNotNullableQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    // try {
    AutoLock lock(this);
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

    ICloseable::Probe(cursor)->Close();
    return NOERROR;
}

Boolean CallDetailActivityQueryHandler::MoveToFirst(
    /* [in] */ ICursor* cursor)
{
    Boolean succeeded;
    if (cursor == NULL || (cursor->MoveToFirst(&succeeded), !succeeded)) {
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
} // Droid
} // Elastos
