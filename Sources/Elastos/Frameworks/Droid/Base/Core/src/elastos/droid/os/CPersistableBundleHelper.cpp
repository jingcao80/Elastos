
#include "elastos/droid/os/CPersistableBundleHelper.h"
#include "elastos/droid/os/CPersistableBundle.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CPersistableBundleHelper, Singleton, IPersistableBundleHelper)

CAR_SINGLETON_IMPL(CPersistableBundleHelper)

ECode CPersistableBundleHelper::GetEMPTY(
    /* [out] */ IPersistableBundle** pb)
{
    VALIDATE_NOT_NULL(pb)
    *pb = CPersistableBundle::EMPTY;
    REFCOUNT_ADD(*pb);
    return NOERROR;
}

ECode CPersistableBundleHelper::ForPair(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [out] */ IPersistableBundle** pb)
{
    VALIDATE_NOT_NULL(pb)
    AutoPtr<IPersistableBundle> b = CPersistableBundle::ForPair(key, value);
    *pb = b;
    REFCOUNT_ADD(*pb)
    return NOERROR;
}

ECode CPersistableBundleHelper::RestoreFromXml(
    /* [in] */ IXmlPullParser* in,
    /* [out] */ IPersistableBundle** pb)
{
    return CPersistableBundle::RestoreFromXml(in, pb);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
