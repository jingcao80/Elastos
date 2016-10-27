
#ifndef __ELASTOS_DROID_SUPPORT_V13_APP_FRAGMENTCOMPATICSMR1_H__
#define __ELASTOS_DROID_SUPPORT_V13_APP_FRAGMENTCOMPATICSMR1_H__

#include "Elastos.Droid.Support.V13.h"
#include "Elastos.Droid.App.h"

using Elastos::Droid::App::IFragment;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V13 {
namespace App {

class FragmentCompatICSMR1
{
public:
    static CARAPI_(void) SetUserVisibleHint(
        /* [in] */ IFragment* f,
        /* [in] */ Boolean visible);
};

} // namespace App
} // namespace V13
} // namespace Support
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SUPPORT_V13_APP_FRAGMENTCOMPATICSMR1_H__
