
#ifndef __CACTIVITYCONTROLLER_H__
#define __CACTIVITYCONTROLLER_H__

#include "_CActivityController.h"
#include "Monkey.h"

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CActivityController)
{
public:
    CAR_OBJECT_DECL()

    CARAPI ActivityStarting(
        /* [in] */ IIntent * intent,
        /* [in] */ const String& pkg,
        /* [out] */ Boolean * res);

    CARAPI ActivityResuming(
        /* [in] */ const String& pkg,
        /* [out] */ Boolean * res);

    CARAPI AppCrashed(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& shortMsg,
        /* [in] */ const String& longMsg,
        /* [in] */ Int64 timeMillis,
        /* [in] */ const String& stackTrace,
        /* [out] */ Boolean * pRes);

    CARAPI AppEarlyNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& annotation,
        /* [out] */ Int32 * res);

    CARAPI AppNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& processStats,
        /* [out] */ Int32 * res);

    CARAPI constructor(
        /* [in] */ Handle32 monkey);

private:
    Monkey* mHost;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYCONTROLLER_H__
