
#include "elastos/droid/content/CContentProviderOperationHelper.h"
#include "elastos/droid/content/CContentProviderOperation.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CContentProviderOperationHelper, Singleton, IContentProviderOperationHelper)

CAR_SINGLETON_IMPL(CContentProviderOperationHelper)

ECode CContentProviderOperationHelper::NewInsert(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderOperationBuilder** operationBuilder)
{
    VALIDATE_NOT_NULL(operationBuilder)
    AutoPtr<IContentProviderOperationBuilder> tmp = CContentProviderOperation::NewInsert(uri);
    *operationBuilder = tmp;
    REFCOUNT_ADD(*operationBuilder);
    return NOERROR;
}

ECode CContentProviderOperationHelper::NewUpdate(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderOperationBuilder** operationBuilder)
{
    VALIDATE_NOT_NULL(operationBuilder)
    AutoPtr<IContentProviderOperationBuilder> tmp = CContentProviderOperation::NewUpdate(uri);
    *operationBuilder = tmp;
    REFCOUNT_ADD(*operationBuilder);
    return NOERROR;
}

ECode CContentProviderOperationHelper::NewDelete(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderOperationBuilder** operationBuilder)
{
    VALIDATE_NOT_NULL(operationBuilder)
    AutoPtr<IContentProviderOperationBuilder> tmp = CContentProviderOperation::NewDelete(uri);
    *operationBuilder = tmp;
    REFCOUNT_ADD(*operationBuilder);
    return NOERROR;
}

ECode CContentProviderOperationHelper::NewAssertQuery(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderOperationBuilder** operationBuilder)
{
    VALIDATE_NOT_NULL(operationBuilder)
    AutoPtr<IContentProviderOperationBuilder> tmp = CContentProviderOperation::NewAssertQuery(uri);
    *operationBuilder = tmp;
    REFCOUNT_ADD(*operationBuilder);
    return NOERROR;
}

}
}
}

