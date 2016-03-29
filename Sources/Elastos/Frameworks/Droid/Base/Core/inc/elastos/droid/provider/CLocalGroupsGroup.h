#ifndef __ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUP_H__
#define __ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUP_H__

#include "_Elastos_Droid_Provider_CLocalGroupsGroup.h"
#include "_Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CLocalGroupsGroup)
    , public Object
    , public ILocalGroupsGroup
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI GetId(
        /* [out] */ Int64* id);

    CARAPI SetId(
        /* [in] */ Int64 id);

    CARAPI GetTitle(
        /* [out] */ String* title);

    CARAPI SetTitle(
        /* [in] */ const String& title);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI SetCount(
        /* [in] */ Int32 count);

    CARAPI ToContentValues(
        /* [out] */ IContentValues** contentValues);

    CARAPI Save(
        /* [in] */ IContentResolver* cr,
        /* [out] */ Boolean* result);

    CARAPI Update(
        /* [in] */ IContentResolver* cr,
        /* [out] */ Boolean* result);

    CARAPI Delete(
        /* [in] */ IContentResolver* cr,
        /* [out] */ Boolean* result);

    CLocalGroupsGroup() : mId(-1), mTitle(""), mCount(0) { }

private:
    Int64 mId;
    String mTitle;
    Int32 mCount;

};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUP_H__
