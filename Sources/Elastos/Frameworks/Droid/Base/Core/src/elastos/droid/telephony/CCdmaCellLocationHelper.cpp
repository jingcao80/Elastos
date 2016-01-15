
#include "CCdmaCellLocationHelper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CDouble;
using Elastos::Core::IDouble;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Telephony {


ECode CCdmaCellLocationHelper::ConvertQuartSecToDecDegrees(
    /* [in] */ Int32 quartSec,
    /* [out] */ Double* val)
{
    VALIDATE_NOT_NULL(val);
    AutoPtr<IDouble> adouble;
    CDouble::New(quartSec, (IDouble**)&adouble);
    Boolean isflag = FALSE;
    if((adouble->IsNaN(&isflag), isflag) || quartSec < -2592000 || quartSec > 2592000){
        // Invalid value
        //throw new IllegalArgumentException("Invalid coordiante value:" + quartSec);
        Slogger::E("CCdmaCellLocationHelper", "Invalid coordiante value:%d", quartSec);
    }
    //((double)quartSec) / (3600 * 4));
    *val = ((Double)quartSec) / (3600 * 4);

    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos