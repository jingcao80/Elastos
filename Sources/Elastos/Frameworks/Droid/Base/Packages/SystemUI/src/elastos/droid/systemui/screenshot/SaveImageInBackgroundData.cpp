#include "elastos/droid/systemui/screenshot/SaveImageInBackgroundData.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

SaveImageInBackgroundData::SaveImageInBackgroundData()
    : mIconSize(0)
    , mResult(0)
    , mPreviewWidth(0)
    , mPreviewheight(0)
{}

void SaveImageInBackgroundData::ClearImage()
{
    mImage = NULL;
    mImageUri = NULL;
    mIconSize = 0;
}

void SaveImageInBackgroundData::ClearContext() {
    mContext = NULL;
}

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos