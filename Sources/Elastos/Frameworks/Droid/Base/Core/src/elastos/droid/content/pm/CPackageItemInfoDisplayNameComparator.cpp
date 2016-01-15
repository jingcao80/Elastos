
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/content/pm/CPackageItemInfoDisplayNameComparator.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::EIID_IComparator;
using Elastos::Text::ICollatorHelper;
using Elastos::Text::CCollatorHelper;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

AutoPtr<ICollator> CPackageItemInfoDisplayNameComparator::sCollator;

CAR_INTERFACE_IMPL(CPackageItemInfoDisplayNameComparator, Object, IComparator)

CAR_OBJECT_IMPL(CPackageItemInfoDisplayNameComparator)

ECode CPackageItemInfoDisplayNameComparator::constructor(
    /* [in] */ IPackageManager* pm)
{
    mPM = pm;
    return NOERROR;
}

ECode CPackageItemInfoDisplayNameComparator::Compare(
    /* [in] */ IInterface* aa,
    /* [in] */ IInterface* ab,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    IPackageItemInfo* pa = IPackageItemInfo::Probe(aa);
    IPackageItemInfo* pb = IPackageItemInfo::Probe(ab);
    VALIDATE_NOT_NULL(pa)
    VALIDATE_NOT_NULL(pb)

    String a, b;
    AutoPtr<ICharSequence> sa, sb;
    pa->LoadLabel(mPM, (ICharSequence**)&sa);
    if (sa == NULL) {
        pa->GetName(&a);
    }
    else {
        a = Object::ToString(sa);
    }

    pb->LoadLabel(mPM, (ICharSequence**)&sb);
    if (sb == NULL) {
        pb->GetName(&b);
    }
    else {
        b = Object::ToString(sb);
    }

    if (sCollator == NULL) {
        AutoPtr<ICollatorHelper> helper;
        CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
        helper->GetInstance((ICollator**)&sCollator);
    }

    return sCollator->Compare(a, b, result);
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
