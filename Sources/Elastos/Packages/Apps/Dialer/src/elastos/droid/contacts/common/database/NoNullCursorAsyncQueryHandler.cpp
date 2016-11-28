
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
