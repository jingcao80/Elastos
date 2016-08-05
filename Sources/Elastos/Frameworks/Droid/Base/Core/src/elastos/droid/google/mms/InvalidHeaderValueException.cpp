
#include "elastos/droid/google/mms/InvalidHeaderValueException.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

//=====================================================================
//                     InvalidHeaderValueException
//=====================================================================
const Int64 InvalidHeaderValueException::serialVersionUID;

InvalidHeaderValueException::InvalidHeaderValueException()
{
}

InvalidHeaderValueException::InvalidHeaderValueException(
    /* [in] */ const String& message)
    : MmsException(message)
{
}

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos


