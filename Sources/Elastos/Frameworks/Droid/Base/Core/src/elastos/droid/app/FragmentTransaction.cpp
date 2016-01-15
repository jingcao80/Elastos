
#include "elastos/droid/app/FragmentTransaction.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(FragmentTransaction, Object, IFragmentTransaction)

FragmentTransaction::FragmentTransaction()
{
}

FragmentTransaction::~FragmentTransaction()
{}

ECode FragmentTransaction::SetCustomTransition(
    /* [in] */ Int32 sceneRootId,
    /* [in] */ Int32 transitionId)
{
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos