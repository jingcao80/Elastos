
#include "wm/StartingData.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

StartingData::StartingData(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 theme,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ ICharSequence* nonLocalizedLabel,
    /* [in] */ Int32 labelRes,
    /* [in] */ Int32 icon,
    /* [in] */ Int32 logo,
    /* [in] */ Int32 windowFlags)
    : mPkg(pkg)
    , mTheme(theme)
    , mCompatInfo(compatInfo)
    , mNonLocalizedLabel(nonLocalizedLabel)
    , mLabelRes(labelRes)
    , mIcon(icon)
    , mLogo(logo)
    , mWindowFlags(windowFlags)
{}


} // Wm
} // Server
} // Droid
} // Elastos
