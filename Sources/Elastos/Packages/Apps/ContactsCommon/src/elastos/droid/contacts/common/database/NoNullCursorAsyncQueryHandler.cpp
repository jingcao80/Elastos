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

#include "NoNullCursorAsyncQueryHandler.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Database {

//=================================================================
// NoNullCursorAsyncQueryHandler::CookieWithProjection
//=================================================================
NoNullCursorAsyncQueryHandler::CookieWithProjection::CookieWithProjection(
    /* [in] */ IInterface* cookie,
    /* [in] */ ArrayOf<String>* projection)
    : mOriginalCookie(cookie)
    , mProjection(projection)
{}

ECode NoNullCursorAsyncQueryHandler::CookieWithProjection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "NoNullCursorAsyncQueryHandler::CookieWithProjection";
    return NOERROR;
}

//=================================================================
// NoNullCursorAsyncQueryHandler
//=================================================================
NoNullCursorAsyncQueryHandler::NoNullCursorAsyncQueryHandler()
    : AsyncQueryHandler()
{}

NoNullCursorAsyncQueryHandler::~NoNullCursorAsyncQueryHandler()
{}

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
    return AsyncQueryHandler::StartQuery(token, (IObject*)projectionCookie,
            uri, projection, selection, selectionArgs, orderBy);
}

ECode NoNullCursorAsyncQueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<CookieWithProjection> projectionCookie = (CookieWithProjection*)cookie;
    AsyncQueryHandler::OnQueryComplete(token, projectionCookie->mOriginalCookie, cursor);

    assert(0 && "TODO");
    // if (cursor == NULL) {
    //     cursor = new EmptyCursor(projectionCookie->mProjection);
    // }
    OnNotNullableQueryComplete(token, projectionCookie->mOriginalCookie, cursor);

    return NOERROR;
}

} // Database
} // Common
} // Contacts
} // Apps
} // Elastos
