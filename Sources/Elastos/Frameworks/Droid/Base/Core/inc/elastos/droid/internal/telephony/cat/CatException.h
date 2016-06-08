#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATEXCEPTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.util.AndroidException;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Base class for all the exceptions in CAT service.
  *
  * {@hide}
  */
class CatException
    : public Object
    //, public IAndroidException
{
public:
    CatException();
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATEXCEPTION_H__

