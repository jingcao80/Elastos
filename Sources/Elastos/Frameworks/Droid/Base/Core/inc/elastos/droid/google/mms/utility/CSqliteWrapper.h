#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CSQLITEWRAPPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CSQLITEWRAPPER_H__

#include "_Elastos_Droid_Google_Mms_Utility_CSqliteWrapper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Database::ICursor;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

CarClass(CSqliteWrapper)
    , public Singleton
    , public ISqliteWrapper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI Query(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** result);

    CARAPI Requery(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [out] */ Boolean* result);

    CARAPI Update(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* result);

    CARAPI Delete(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* result);

    CARAPI Insert(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** result);

};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CSQLITEWRAPPER_H__
