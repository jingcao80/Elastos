
#ifndef __CINSTRUMENTATIONWATCHER_H__
#define __CINSTRUMENTATIONWATCHER_H__

#include "_CInstrumentationWatcher.h"
#include <elastos/core/Object.h>
#include "CAm.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CInstrumentationWatcher)
{
public:
    CInstrumentationWatcher();

    CARAPI InstrumentationStatus(
        /* [in] */ IComponentName * name,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle * results);

    CARAPI InstrumentationFinished(
        /* [in] */ IComponentName * name,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle * results);

/**
* Set or reset "raw mode".  In "raw mode", all bundles are dumped.  In "pretty mode",
* if a bundle includes Instrumentation.REPORT_KEY_STREAMRESULT, just print that.
* @param rawMode true for raw mode, false for pretty mode.
*/
    CARAPI SetRawOutput(
        /* [in] */ Boolean rawMode);

    CARAPI WaitForFinish(
        /* [out] */ Boolean* result);

    CARAPI constructor(
        /* [in] */ Handle32 am);

private:
    Boolean mFinished;
    Boolean mRawMode;
    Object mLock;
    CAm* mHost;
};

}
}
}
}

#endif // __CINSTRUMENTATIONWATCHER_H__
