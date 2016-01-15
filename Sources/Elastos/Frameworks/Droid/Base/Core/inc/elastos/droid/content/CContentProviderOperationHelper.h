
#ifndef __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATIONHELPER_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATIONHELPER_H__

#include "_Elastos_Droid_Content_CContentProviderOperationHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentProviderOperationHelper)
    , public Singleton
    , public IContentProviderOperationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create a {@link Builder} suitable for building an insert {@link ContentProviderOperation}.
     * @param uri The {@link Uri} that is the target of the insert.
     * @return a {@link Builder}
     */
    CARAPI NewInsert(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderOperationBuilder** operationBuilder);

    /**
     * Create a {@link Builder} suitable for building an update {@link ContentProviderOperation}.
     * @param uri The {@link Uri} that is the target of the update.
     * @return a {@link Builder}
     */
    CARAPI NewUpdate(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderOperationBuilder** operationBuilder);

    /**
     * Create a {@link Builder} suitable for building a delete {@link ContentProviderOperation}.
     * @param uri The {@link Uri} that is the target of the delete.
     * @return a {@link Builder}
     */
    CARAPI NewDelete(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderOperationBuilder** operationBuilder);

    /**
     * Create a {@link Builder} suitable for building a
     * {@link ContentProviderOperation} to assert a set of values as provided
     * through {@link Builder#withValues(ContentValues)}.
     */
    CARAPI NewAssertQuery(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderOperationBuilder** operationBuilder);

private:
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATIONHELPER_H__
