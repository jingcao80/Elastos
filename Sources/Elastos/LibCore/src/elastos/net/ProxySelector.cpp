
#include "ProxySelector.h"
#include "ProxySelectorImpl.h"

namespace Elastos {
namespace Net {

AutoPtr<IProxySelector> ProxySelector::sDefaultSelector;

CAR_INTERFACE_IMPL(ProxySelector, Object, IProxySelector)

ProxySelector::ProxySelector()
{}

ProxySelector::~ProxySelector()
{}

ECode ProxySelector::GetDefault(
    /* [out] */ IProxySelector** defaultSelector)
{
    VALIDATE_NOT_NULL(defaultSelector);

    if (sDefaultSelector == NULL) {
        sDefaultSelector = new ProxySelectorImpl();
    }

    *defaultSelector = sDefaultSelector;
    REFCOUNT_ADD(*defaultSelector);
    return NOERROR;
}

ECode ProxySelector::SetDefault(
    /* [in] */ IProxySelector* selector)
{
    sDefaultSelector = selector;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
