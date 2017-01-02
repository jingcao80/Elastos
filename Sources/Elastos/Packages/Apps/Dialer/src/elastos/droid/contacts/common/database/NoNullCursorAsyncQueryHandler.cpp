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

#include "elastos/droid/contacts/common/database/NoNullCursorAsyncQueryHandler.h"
#include "elastos/droid/contacts/common/database/EmptyCursor.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Contacts::Common::Database::EIID_INoNullCursorAsyncQueryHandler;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Database {

CAR_INTERFACE_IMPL(NoNullCursorAsyncQueryHandler, AsyncQueryHandler, INoNullCursorAsyncQueryHandler)

ECode NoNullCursorAsyncQueryHandler::constructor(
    /* [in] */ IContentResolver* cr)
{
    return AsyncQueryHandler::constructor(cr);
}

ECode NoNullCursorAsyncQueryHandler::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& orderBy)
{
    AutoPtr<CookieWithProjection> projectionCookie = new CookieWithProjection(cookie, projection);
    return AsyncQueryHandler::StartQuery(token, TO_IINTERFACE(projectionCookie), uri, projection, selection, selectionArgs,
            orderBy);
}

ECode NoNullCursorAsyncQueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* inCursor)
{
    AutoPtr<ICursor> cursor = inCursor;
    AutoPtr<CookieWithProjection> projectionCookie = (CookieWithProjection*)IObject::Probe(cookie);
    AutoPtr<IInterface> originalCookie = projectionCookie->mOriginalCookie;
    AsyncQueryHandler::OnQueryComplete(token, originalCookie, cursor);

    if (cursor == NULL) {
        AutoPtr<EmptyCursor> emptyCursor = new EmptyCursor();
        emptyCursor->constructor(projectionCookie->mProjection);
        cursor = (ICursor*)emptyCursor;
    }
    return OnNotNullableQueryComplete(token, originalCookie, cursor);
}

} // namespace Database
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
