
#ifndef __ELASTOS_DROID_SERVER_AM_ACTIVITYRESULT_H__
#define __ELASTOS_DROID_SERVER_AM_ACTIVITYRESULT_H__

#include "Elastos.Droid.App.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IResultInfo;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityRecord;

class ActivityResult : public Object
{
public:
    ActivityResult(
        /* [in] */ ActivityRecord* from,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    ~ActivityResult();

public:
    AutoPtr<IResultInfo> mResultInfo;
    ActivityRecord* mFrom;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_ACTIVITYRESULT_H__
