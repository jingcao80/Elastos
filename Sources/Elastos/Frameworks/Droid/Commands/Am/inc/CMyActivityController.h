
#ifndef __CMYACTIVITYCONTROLLER_H__
#define __CMYACTIVITYCONTROLLER_H__

#include "_CMyActivityController.h"
#include "MyActivityController.h"

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CMyActivityController), public MyActivityController
{
public:

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

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
        /* [out] */ Boolean * res);

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
    /* [in] */ Handle32 am);

    CARAPI constructor(
        /* [in] */ Handle32 am,
        /* [in] */ const String& gdbPort);
};

}
}
}
}

#endif // __CMYACTIVITYCONTROLLER_H__
