
#include "elastos/droid/hardware/camera2/legacy/LegacySizeAreaComparator.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacySizeAreaComparator.h"
#include "elastos/droid/internal/utility/Preconditions.h"

using Elastos::Droid::Hardware::Camera2::Legacy::ILegacySizeAreaComparator;
using Elastos::Droid::Hardware::Camera2::Legacy::CLegacySizeAreaComparator;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL_2(LegacySizeAreaComparator, Object, ILegacySizeAreaComparator, IComparator)

ECode LegacySizeAreaComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    FAIL_RETURN(Preconditions::CheckNotNull(lhs, String("lhs must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(rhs, String("rhs must not be null")))

    AutoPtr<ICameraSize> size = ICameraSize::Probe(size);
    AutoPtr<ICameraSize> size2 = ICameraSize::Probe(size);
    Boolean res;
    size->Equals(size2, &res);
    if (res) {
        *result = 0;
        return NOERROR;
    }

    Int32 width;
    size->GetWidth(&width);
    Int32 width2;
    size2->GetWidth(&width2);
    Int32 height;
    size->GetHeight(&height);
    Int64 area = width * height;
    Int32 height2;
    size2->GetHeight(&height2);
    Int64 area2 = width2 * height2;

    if (area == area2) {
        *result = (width > width2) ? 1 : -1;
        return NOERROR;
    }

    *result = (area > area2) ? 1 : -1;
    return NOERROR;
}

ECode LegacySizeAreaComparator::FindLargestByArea(
    /* [in] */ IList* sizes,
    /* [out] */ ICameraSize** outcs)
{
    VALIDATE_NOT_NULL(outcs);
    *outcs = NULL;
    FAIL_RETURN(Preconditions::CheckNotNull(sizes, String("sizes must not be null")))

    AutoPtr<ILegacySizeAreaComparator> com;
    CLegacySizeAreaComparator::New((ILegacySizeAreaComparator**)&com);
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    AutoPtr<IInterface> obj;
    helper->Max(ICollection::Probe(sizes), IComparator::Probe(com), (IInterface**)&obj);
    *outcs = ICameraSize::Probe(obj);
    REFCOUNT_ADD(*outcs);
    return NOERROR;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
