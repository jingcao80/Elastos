#include "MonkeyRotationEvent.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyRotationEvent::MonkeyRotationEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ROTATION)
    , mRotationDegree(0)
    , mPersist(FALSE)
{}

MonkeyRotationEvent::MonkeyRotationEvent(
    /* [in] */ Int32 degree,
    /* [in] */ Boolean persist)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ROTATION)
    , mRotationDegree(degree)
    , mPersist(persist)
{}

ECode MonkeyRotationEvent::Init(
    /* [in] */ Int32 degree,
    /* [in] */ Boolean persist)
{
    Int32 *pDegree = &mRotationDegree;
    *pDegree = degree;
    Boolean *pPersist = (Boolean*)&mPersist;
    *pPersist = persist;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ROTATION);
    return NOERROR;
}

Int32 MonkeyRotationEvent::InjectEvent(
    /* [in] */ IIWindowManager* iwm,
    /* [in] */ IIActivityManager* iam,
    /* [in] */ Int32 verbose)
{
    if (verbose > 0) {
        String str;
        AutoPtr<IBoolean> tmp;
        CBoolean::New(mPersist, (IBoolean**)&tmp);
        tmp->ToString(&str);
        PFL_EX(":Sending rotation degree= %s, persist = %s",
            StringUtils::Int32ToString(mRotationDegree).string(), str.string())
    }

    //inject rotation event
    iwm->FreezeRotation(mRotationDegree);
    if (!mPersist) {
        iwm->ThawRotation();
    }
    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

