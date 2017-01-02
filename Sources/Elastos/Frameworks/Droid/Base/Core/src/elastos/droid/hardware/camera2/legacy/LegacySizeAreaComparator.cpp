//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

    ICameraSize* size = ICameraSize::Probe(lhs);
    ICameraSize* size2 = ICameraSize::Probe(rhs);
    if (Object::Equals(size, size2)) {
        *result = 0;
        return NOERROR;
    }

    Int32 width, width2, height, height2;
    size->GetWidth(&width);
    size->GetHeight(&height);
    size2->GetWidth(&width2);
    size2->GetHeight(&height2);
    Int64 area = width * height;
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
