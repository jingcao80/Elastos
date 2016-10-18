
#include "elastos/droid/hardware/camera2/utils/TaskSingleDrainer.h"
#include "elastos/droid/hardware/camera2/utils/CTaskDrainer.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(TaskSingleDrainer, Object, ITaskSingleDrainer)

TaskSingleDrainer::TaskSingleDrainer()
{
}

TaskSingleDrainer::~TaskSingleDrainer()
{
}

ECode TaskSingleDrainer::constructor()
{
    return NOERROR;
}

ECode TaskSingleDrainer::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ ITaskDrainerDrainListener* listener)
{
    return CTaskDrainer::New(handler, listener, (ITaskDrainer**)&mTaskDrainer);
}

ECode TaskSingleDrainer::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ ITaskDrainerDrainListener* listener,
    /* [in] */ const String& name)
{
    return CTaskDrainer::New(handler, listener, name, (ITaskDrainer**)&mTaskDrainer);
}

ECode TaskSingleDrainer::TaskStarted()
{
    return mTaskDrainer->TaskStarted(TO_IINTERFACE((IObject*)&mSingleTask));
}

ECode TaskSingleDrainer::TaskFinished()
{

    return mTaskDrainer->TaskFinished(TO_IINTERFACE((IObject*)&mSingleTask));
}

ECode TaskSingleDrainer::BeginDrain()
{
    return mTaskDrainer->BeginDrain();
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos