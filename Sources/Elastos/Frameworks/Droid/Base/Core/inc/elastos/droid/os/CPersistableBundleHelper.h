
#ifndef __ELASTOS_DROID_OS_CPERSISTABLEBUNDLEHELPER_H__
#define __ELASTOS_DROID_OS_CPERSISTABLEBUNDLEHELPER_H__

#include "_Elastos_Droid_Os_CPersistableBundleHelper.h"
#include <elastos/core/Singleton.h>

using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CPersistableBundleHelper)
    , public Singleton
    , public IPersistableBundleHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetEMPTY(
        /* [out] */ IPersistableBundle** pb);

    CARAPI ForPair(
        /* [in] */ const String& key,
        /* [in] */ const String& value,
        /* [out] */ IPersistableBundle** pb);

    CARAPI RestoreFromXml(
        /* [in] */ IXmlPullParser* in,
        /* [out] */ IPersistableBundle** pb);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_OS_CPERSISTABLEBUNDLEHELPER_H__
