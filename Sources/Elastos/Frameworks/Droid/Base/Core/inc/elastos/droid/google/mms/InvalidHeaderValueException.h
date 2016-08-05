#ifndef __ELASTOS_DROID_GOOGLE_MMS_INVALIDHEADERVALUEEXCEPTION_H__
#define __ELASTOS_DROID_GOOGLE_MMS_INVALIDHEADERVALUEEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/google/mms/MmsException.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

/**
  * Thrown when an invalid header value was set.
  */
class InvalidHeaderValueException
    : public MmsException
{
public:
    /**
      * Constructs an InvalidHeaderValueException with no detailed message.
      */
    InvalidHeaderValueException();

    /**
      * Constructs an InvalidHeaderValueException with the specified detailed message.
      *
      * @param message the detailed message.
      */
    InvalidHeaderValueException(
        /* [in] */ const String& message);

private:
    static const Int64 serialVersionUID = -2053384496042052262L;
};

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_INVALIDHEADERVALUEEXCEPTION_H__

