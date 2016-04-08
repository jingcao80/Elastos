
#include "CContentProviderOne.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ContentProviderDemo {

ECode CContentProviderOne::Initialize()
{
    return NOERROR;
}

ECode CContentProviderOne::OnCreate(
    /* [out] */ Boolean* succeeded)
{
    return NOERROR;
}

ECode CContentProviderOne::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    return NOERROR;
}

ECode CContentProviderOne::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    *type = "GetType Succeeded!";
    return NOERROR;
}

ECode CContentProviderOne::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** insertedItemUri)
{
    return NOERROR;
}

ECode CContentProviderOne::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** cursor)
{
    PRINT_FILE_LINE;
    return NOERROR;
}

ECode CContentProviderOne::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    return NOERROR;
}

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
