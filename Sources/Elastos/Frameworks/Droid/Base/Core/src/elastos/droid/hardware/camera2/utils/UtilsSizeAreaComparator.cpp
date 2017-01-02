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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/hardware/camera2/utils/UtilsSizeAreaComparator.h"
#include "elastos/droid/hardware/camera2/utils/CUtilsSizeAreaComparator.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Hardware::Camera2::Utils::IUtilsSizeAreaComparator;
using Elastos::Droid::Hardware::Camera2::Utils::CUtilsSizeAreaComparator;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL_2(UtilsSizeAreaComparator, Object, IUtilsSizeAreaComparator, IComparator)

ECode UtilsSizeAreaComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<ISize> size = ISize::Probe(lhs);
    AutoPtr<ISize> size2 = ISize::Probe(rhs);

    FAIL_RETURN(Preconditions::CheckNotNull(size.Get(), String("size must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(size2.Get(), String("size2 must not be null")))

    Boolean _result;
    size->Equals(size2, &_result);
    if (_result) {
        *result = 0;
        return NOERROR;
    }

    Int32 width1, width2, height1, height2;
    size->GetWidth(&width1);
    size2->GetWidth(&width2);
    size->GetHeight(&height1);
    Int64 area = width1 * height1;
    size2->GetHeight(&height2);
    Int64 area2 = width2 * height2;

    if (area == area2) {
        *result = (width1 > width2) ? 1 : -1;
        return NOERROR;
    }

    *result = (area > area2) ? 1 : -1;
    return NOERROR;
}

ECode UtilsSizeAreaComparator::FindLargestByArea(
    /* [in] */ IList* sizes,
    /* [out] */ ISize** outsz)
{
    VALIDATE_NOT_NULL(outsz);
    *outsz = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(sizes, String("sizes must not be null")))

    AutoPtr<IUtilsSizeAreaComparator> comparator;
    CUtilsSizeAreaComparator::New((IUtilsSizeAreaComparator**)&comparator);

    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    AutoPtr<IInterface> obj;
    helper->Max(ICollection::Probe(sizes), IComparator::Probe(comparator), (IInterface**)&obj);
    *outsz = ISize::Probe(obj);
    REFCOUNT_ADD(*outsz);
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos