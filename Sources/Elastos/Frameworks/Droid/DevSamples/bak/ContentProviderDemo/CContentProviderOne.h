
#ifndef __CCONTENTPROVIDERONE_H__
#define __CCONTENTPROVIDERONE_H__

#include <content/ContentProvider.h>
#include "_CContentProviderOne.h"

using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ContentProviderDemo {

class CContentProviderOne : public ContentProvider
{
public:
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

    CARAPI Initialize();
};

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__CCONTENTPROVIDERONE_H__
