#ifndef __ELASTOS_DROID_GOOGLE_MMS_MMSEXCEPTION_H__
#define __ELASTOS_DROID_GOOGLE_MMS_MMSEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

/**
  * A generic exception that is thrown by the Mms client.
  */
class MmsException
    : public Object
    //, public Exception
{
public:
    /**
      * Creates a new MmsException.
      */
    MmsException();

    /**
      * Creates a new MmsException with the specified detail message.
      *
      * @param message the detail message.
      */
    MmsException(
        /* [in] */ const String& message);

    /**
      * Creates a new MmsException with the specified cause.
      *
      * @param cause the cause.
      */
    //MmsException(
        ///* [in] */ Throwable* cause);

    /**
      * Creates a new MmsException with the specified detail message and cause.
      *
      * @param message the detail message.
      * @param cause the cause.
      */
    //MmsException(
        ///* [in] */ const String& message,
        ///* [in] */ Throwable* cause);

private:
    static const Int64 serialVersionUID = -7323249827281485390L;
};

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_MMSEXCEPTION_H__
