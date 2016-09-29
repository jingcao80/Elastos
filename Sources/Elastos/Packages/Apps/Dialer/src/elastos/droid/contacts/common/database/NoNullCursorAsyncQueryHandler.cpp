
#include "elastos/droid/contacts/common/database/NoNullCursorAsyncQueryHandler.h"
#include "elastos/droid/contacts/common/database/EmptyCursor.h"

using Elastos::Droid::Contacts::Common::Database::EIID_INoNullCursorAsyncQueryHandler;

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
    return AsyncQueryHandler::StartQuery(token, (IObject*)projectionCookie, uri, projection, selection, selectionArgs,
            orderBy);
}

ECode NoNullCursorAsyncQueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<CookieWithProjection> projectionCookie = (CookieWithProjection*)(IObject*)cookie;

    AsyncQueryHandler::OnQueryComplete(token, projectionCookie->mOriginalCookie, cursor);

    if (cursor == NULL) {
        cursor = new EmptyCursor();
        cursor->constructor(projectionCookie->mProjection)
    }
    return OnNotNullableQueryComplete(token, projectionCookie->mOriginalCookie, cursor);
}

} // namespace Database
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
