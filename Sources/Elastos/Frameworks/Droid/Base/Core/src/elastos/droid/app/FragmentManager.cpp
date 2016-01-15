
#include "elastos/droid/app/FragmentManager.h"
#include "elastos/droid/app/FragmentManagerImpl.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(FragmentManager, Object, IFragmentManager)

FragmentManager::FragmentManager()
{
}

FragmentManager::~FragmentManager()
{
}

ECode FragmentManager::OpenTransaction(
    /* [out] */ IFragmentTransaction** transaction)
{
    return BeginTransaction(transaction);
}

ECode FragmentManager::EnableDebugLogging(
    /* [in] */ Boolean enabled)
{
    FragmentManagerImpl::DEBUG = enabled;
    return NOERROR;
}

ECode FragmentManager::InvalidateOptionsMenu()
{
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos