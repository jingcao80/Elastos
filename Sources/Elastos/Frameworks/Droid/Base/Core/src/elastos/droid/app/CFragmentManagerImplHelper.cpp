
#include "elastos/droid/app/CFragmentManagerImplHelper.h"
#include "elastos/droid/app/FragmentManagerImpl.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CFragmentManagerImplHelper, Singleton, IFragmentManagerImplHelper)

CAR_SINGLETON_IMPL(CFragmentManagerImplHelper)

ECode CFragmentManagerImplHelper::ReverseTransit(
    /* [in] */ Int32 transit,
    /* [out] */ Int32* reverse)
{
    return FragmentManagerImpl::ReverseTransit(transit, reverse);
}

ECode CFragmentManagerImplHelper::TransitToStyleIndex(
    /* [in] */ Int32 transit,
    /* [in] */ Boolean enter,
    /* [out] */ Int32* index)
{
    return FragmentManagerImpl::TransitToStyleIndex(transit, enter, index);
}

ECode CFragmentManagerImplHelper::SetDEBUG(
    /* [in] */ Boolean debug)
{
    FragmentManagerImpl::DEBUG = debug;
    return NOERROR;
}

ECode CFragmentManagerImplHelper::GetDEBUG(
    /* [out] */ Boolean* debug)
{
    VALIDATE_NOT_NULL(debug);
    *debug = FragmentManagerImpl::DEBUG;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
