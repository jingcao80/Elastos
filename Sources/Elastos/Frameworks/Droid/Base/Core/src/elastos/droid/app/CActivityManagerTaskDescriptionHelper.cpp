#include "elastos/droid/app/CActivityManagerTaskDescriptionHelper.h"
#include "elastos/droid/app/CActivityManagerTaskDescription.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CActivityManagerTaskDescriptionHelper, Singleton, IActivityManagerTaskDescriptionHelper)

CAR_SINGLETON_IMPL(CActivityManagerTaskDescriptionHelper)

ECode CActivityManagerTaskDescriptionHelper::LoadTaskDescriptionIcon(
    /* [in] */ const String& iconFilename,
    /* [out] */ IBitmap** bm)
{
    return CActivityManagerTaskDescription::LoadTaskDescriptionIcon(iconFilename, bm);
}

}// namespace App
}// namespace Droid
}// namespace Elastos