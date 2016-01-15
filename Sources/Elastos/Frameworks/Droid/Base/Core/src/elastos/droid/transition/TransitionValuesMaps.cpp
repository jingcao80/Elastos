
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/transition/TransitionValuesMaps.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CInt64SparseArray.h"
#include "elastos/droid/utility/CSparseArray.h"

using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CInt64SparseArray;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// TransitionValuesMaps::
//===============================================================
CAR_INTERFACE_IMPL(TransitionValuesMaps, Object, ITransitionValuesMaps)

TransitionValuesMaps::TransitionValuesMaps()
{
    CArrayMap::New((IArrayMap**)&mViewValues);

    CSparseArray::New((ISparseArray**)&mIdValues);

    CInt64SparseArray::New((IInt64SparseArray**)&mItemIdValues);

    CArrayMap::New((IArrayMap**)&mNameValues);
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
