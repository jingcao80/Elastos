#include "CEnumSetHelper.h"
#include "EnumSet.h"

namespace Elastos{
namespace Utility{

CAR_SINGLETON_IMPL(CEnumSetHelper)

CAR_INTERFACE_IMPL(CEnumSetHelper, Singleton, IEnumSetHelper)

ECode CEnumSetHelper::NoneOf(
    /* [in] */ const InterfaceID& type,
    /* [out] */ IEnumSet** res)
{
    return EnumSet::NoneOf(type, res);
}

ECode CEnumSetHelper::AllOf(
    /* [in] */ const InterfaceID& type,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::AllOf(type);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::CopyOf(
    /* [in] */ IEnumSet* s,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::CopyOf(s);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::CopyOf(
    /* [in] */ ICollection* c,
    /* [out] */ IEnumSet** set)
{
    return EnumSet::CopyOf(c, set);
}

ECode CEnumSetHelper::ComplementOf(
    /* [in] */ IEnumSet* s,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::ComplementOf(s);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ IInterface* i,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(i);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(i1, i2);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [in] */ IInterface* i3,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(i1, i2, i3);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [in] */ IInterface* i3,
    /* [in] */ IInterface* i4,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(i1, i2, i3, i4);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [in] */ IInterface* i3,
    /* [in] */ IInterface* i4,
    /* [in] */ IInterface* i5,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(i1, i2, i3, i4, i5);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Of(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IEnumSet> temp = EnumSet::Of(array);
    *res = temp;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CEnumSetHelper::Range(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end,
    /* [out] */ IEnumSet** set)
{
    return EnumSet::Range(start, end, set);
}

} // namespace Utility
} // namespace Elastos