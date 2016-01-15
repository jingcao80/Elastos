
#ifndef __ELASTOS_DROID_PRIVACY_CPRIVACYSETTINGSSTUB_H__
#define __ELASTOS_DROID_PRIVACY_CPRIVACYSETTINGSSTUB_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Privacy {

class PrivacySettingsStub
    : public ElRefBase
    , public IInterface
{
public:
    CAR_INTERFACE_DECL();

    CARAPI_(Boolean) IsStub();

private:
    static const Boolean sIsStub;
};

} // namespace Privacy
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PRIVACY_CPRIVACYSETTINGSSTUB_H__
