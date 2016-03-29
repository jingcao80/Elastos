
#include "elastos/droid/provider/LocalGroups.h"
#include "elastos/droid/provider/CLocalGroupsGroup.h"
#include "elastos/droid/provider/ContactsContractData.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_OBJECT_IMPL(CLocalGroupsGroup)

CAR_INTERFACE_IMPL(CLocalGroupsGroup, Object, ILocalGroupsGroup)

ECode CLocalGroupsGroup::constructor()
{
    return NOERROR;
}

ECode CLocalGroupsGroup::GetId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode CLocalGroupsGroup::SetId(
    /* [in] */ Int64 id)
{
    mId = id;
    return NOERROR;
}

ECode CLocalGroupsGroup::GetTitle(
    /* [out] */ String* title)
{
    VALIDATE_NOT_NULL(title);
    *title = mTitle;
    return NOERROR;
}

ECode CLocalGroupsGroup::SetTitle(
    /* [in] */ const String& title)
{
    mTitle = title;
    return NOERROR;
}

ECode CLocalGroupsGroup::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mCount;
    return NOERROR;
}

ECode CLocalGroupsGroup::SetCount(
    /* [in] */ Int32 count)
{
    mCount = count;
    return NOERROR;
}

ECode CLocalGroupsGroup::ToContentValues(
    /* [out] */ IContentValues** contentValues)
{
    VALIDATE_NOT_NULL(contentValues);

    AutoPtr<IContentValues> values;
    FAIL_RETURN(CContentValues::New((IContentValues**)&values));

    values->Put(ILocalGroupsGroupColumns::TITLE, mTitle);
    values->Put(ILocalGroupsGroupColumns::COUNT, mCount);

    *contentValues = values;
    REFCOUNT_ADD(*contentValues);
    return NOERROR;
}

ECode CLocalGroupsGroup::Save(
    /* [in] */ IContentResolver* cr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (!cr) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IContentValues> contentValues;
    ToContentValues((IContentValues**)&contentValues);

    AutoPtr<IUri> uri;
    ECode ec = cr->Insert(
            LocalGroups::CONTENT_URI, contentValues, (IUri**)&uri);
    if (SUCCEEDED(ec) && uri != NULL) {
        Int64 id;
        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        contentUris->ParseId(uri, &id);
        SetId(id);
        *result = TRUE;
    }
    else {
        *result = FALSE;
    }

    return NOERROR;
}

ECode CLocalGroupsGroup::Update(
    /* [in] */ IContentResolver* cr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (!cr) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IContentValues> contentValues;
    ToContentValues((IContentValues**)&contentValues);

    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = StringUtils::ToString(mId);

    Int32 rowsAffected = 0;
    cr->Update(LocalGroups::CONTENT_URI, contentValues,
            String(ILocalGroupsGroupColumns::_ID + "=?"), args, &rowsAffected);

    *result = rowsAffected > 0;
    return NOERROR;
}

ECode CLocalGroupsGroup::Delete(
    /* [in] */ IContentResolver* cr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (!cr) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IUri> uri;
    ContactsContractData::GetCONTENT_URI((IUri**)&uri);

    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = IContactsContractCommonDataKindsLocalGroup::CONTENT_ITEM_TYPE;
    (*args)[1] = StringUtils::ToString(mId);

    Int32 rowsAffected = 0;
    cr->Delete(uri, String(IContactsContractDataColumns::MIMETYPE + "=? and "
            + IContactsContractCommonDataKindsLocalGroup::GROUP + "=?"), args,
            &rowsAffected);

    (*args)[0] = StringUtils::ToString(mId);
    rowsAffected = 0;
    cr->Delete(LocalGroups::CONTENT_URI,
            String(ILocalGroupsGroupColumns::_ID + "=?"), args, &rowsAffected);

    *result = rowsAffected > 0;
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos
