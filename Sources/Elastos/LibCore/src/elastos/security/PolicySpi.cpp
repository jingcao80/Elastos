
#include "PolicySpi.h"
#include "Policy.h"

namespace Elastos {
namespace Security {

PolicySpi::~PolicySpi()
{}

PolicySpi::PolicySpi()
{
    // default constructor
}

void PolicySpi::EngineRefresh()
{
    // do nothing in default implementation
}

AutoPtr<PermissionCollection> PolicySpi::EngineGetPermissions(
    /* [in] */ ICodeSource* codesource)
{
    return Policy::UNSUPPORTED_EMPTY_COLLECTION;
}

AutoPtr<PermissionCollection> PolicySpi::EngineGetPermissions(
    /* [in] */ IProtectionDomain* domain)
{
    return Policy::UNSUPPORTED_EMPTY_COLLECTION;
}

} // namespace Security
} // namespace Elastos
