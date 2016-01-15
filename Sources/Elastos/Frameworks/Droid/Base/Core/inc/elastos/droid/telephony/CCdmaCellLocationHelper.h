
#ifndef __ELASTOS_DROID_TELEPHONY_CCDMACELLLOCATIONHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CCDMACELLLOCATIONHELPER_H__

#include "_Elastos_Droid_Telephony_CCdmaCellLocationHelper.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCdmaCellLocationHelper)
{
public:
    CARAPI ConvertQuartSecToDecDegrees(
        /* [in] */ Int32 quartSec,
        /* [out] */ Double* val);

};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCDMACELLLOCATIONHELPER_H__
