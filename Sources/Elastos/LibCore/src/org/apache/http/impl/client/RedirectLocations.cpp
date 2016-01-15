
#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/impl/client/RedirectLocations.h"
#include "elastos/utility/CHashSet.h"

using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

RedirectLocations::RedirectLocations()
{
    AutoPtr<IHashSet> hashSet;
    CHashSet::New((IHashSet**)&hashSet);
    mUris = ISet::Probe(hashSet);
}

Boolean RedirectLocations::Contains(
    /* [in] */ IURI* uri)
{
    Boolean result;
    mUris->Contains(uri, &result);
    return result;
}

void RedirectLocations::Add(
    /* [in] */ IURI* uri)
{
    mUris->Add(uri);
}

Boolean RedirectLocations::Remove(
    /* [in] */ IURI* uri)
{
    Boolean result;
    mUris->Remove(uri, &result);
    return result;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org