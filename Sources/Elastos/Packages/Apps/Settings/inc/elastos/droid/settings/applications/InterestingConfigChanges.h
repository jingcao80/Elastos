#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_INTERESTINGCONFIGCHANGES_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_INTERESTINGCONFIGCHANGES_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

class InterestingConfigChanges
    : public Object
{
public:
    InterestingConfigChanges();

    ~InterestingConfigChanges();

    virtual CARAPI_(Boolean) ApplyNewConfig(
        /* [in] */ IResources* res);

protected:
    AutoPtr<IConfiguration> mLastConfiguration;
    Int32 mLastDensity;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_INTERESTINGCONFIGCHANGES_H__