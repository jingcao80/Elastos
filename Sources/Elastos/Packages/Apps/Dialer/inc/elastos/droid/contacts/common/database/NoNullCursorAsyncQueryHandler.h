
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
