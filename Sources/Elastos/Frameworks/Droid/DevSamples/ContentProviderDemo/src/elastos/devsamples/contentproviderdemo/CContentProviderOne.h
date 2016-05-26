#ifndef __Elastos_DevSamples_ContentProviderDemo_CContentProviderOne_H__
#define __Elastos_DevSamples_ContentProviderDemo_CContentProviderOne_H__

#include "_Elastos_DevSamples_ContentProviderDemo_CContentProviderOne.h"
#include <Elastos.Droid.Database.h>
#include <elastos/droid/content/ContentProvider.h>

using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

CarClass(CContentProviderOne)
    , public ContentProvider
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI OnCreate(
        /* [out] */ Boolean* succeeded);

    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** insertedItemUri);

    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

private:
    static AutoPtr<IUriMatcher> GetUriMatcher();

private:
    static AutoPtr<IUriMatcher> sUriMatcher;

    AutoPtr<ISQLiteOpenHelper> mDBOpenHelper;
};

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos

#endif //__Elastos_DevSamples_ContentProviderDemo_CContentProviderOne_H__
