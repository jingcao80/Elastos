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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_DATABASE_NONULLCURSORASYNCQUERYHANDLER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_DATABASE_NONULLCURSORASYNCQUERYHANDLER_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/content/AsyncQueryHandler.h"

using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Database {

/**
 * An {@AsyncQueryHandler} that will never return a null cursor.
 * <p>
 * Instead, will return a {@link Cursor} with 0 records.
 */
class NoNullCursorAsyncQueryHandler
    : public AsyncQueryHandler
    , public INoNullCursorAsyncQueryHandler
{
private:
    /**
     * Class to add projection to an existing cookie.
     */
    class CookieWithProjection : public Object
    {
    public:
        TO_STRING_IMPL("NoNullCursorAsyncQueryHandler::CookieWithProjection")

        CookieWithProjection(
            /* [in] */ IInterface* cookie,
            /* [in] */ ArrayOf<String>* projection)
            : mOriginalCookie(cookie)
            , mProjection(projection)
        {}

    public:
        AutoPtr<IInterface> mOriginalCookie;
        AutoPtr<ArrayOf<String> > mProjection;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContentResolver* cr);

    // @Override
    CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& orderBy);

protected:
    // @Override
    CARAPI OnQueryComplete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ ICursor* cursor);
};

} // Database
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_DATABASE_NONULLCURSORASYNCQUERYHANDLER_H__
