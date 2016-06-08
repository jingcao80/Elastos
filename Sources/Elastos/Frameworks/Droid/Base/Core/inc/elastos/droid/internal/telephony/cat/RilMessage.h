#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_RILMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_RILMESSAGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

class RilMessage
    : public Object
{
public:
    RilMessage(
        /* [in] */ Int32 msgId,
        /* [in] */ const String& rawData);

    RilMessage(
        /* [in] */ RilMessage* other);

public:
    Int32 mId;
    AutoPtr<IInterface> mData;
    ResultCode mResCode;
};


} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_RILMESSAGE_H__
