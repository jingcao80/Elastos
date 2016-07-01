#ifndef __ELASTOS_APPS_CONTACTS_COMMON_DATABASE_NONULLCURSORASYNCQUERYHANDLER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_DATABASE_NONULLCURSORASYNCQUERYHANDLER_H__

#include <AsyncQueryHandler.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Content::IContentResolver;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Database {

class ECO_PUBLIC NoNullCursorAsyncQueryHandler
    : public AsyncQueryHandler
{
private:
    class CookieWithProjection
        : public Object
    {
    public:
        CookieWithProjection(
            /* [in] */ IInterface* cookie,
            /* [in] */ ArrayOf<String>* projection);

        CAR_INTERFACE_DECL();

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        AutoPtr<IInterface> mOriginalCookie;
        AutoPtr<ArrayOf<String> > mProjection;
    };

public:
    NoNullCursorAsyncQueryHandler();

    virtual ~NoNullCursorAsyncQueryHandler();

    CARAPI constructor(
        /* [in] */ IContentResolver* cr);

    CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& orderBy);

protected:
    CARAPI OnQueryComplete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ ICursor* cursor);

    virtual CARAPI OnNotNullableQueryComplete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ ICursor* cursor) = 0;
};

} // Database
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_DATABASE_NONULLCURSORASYNCQUERYHANDLER_H__
