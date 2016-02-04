#ifndef __ELASTOS_DROID_SERVER_WM_STARTINGDATA_H__
#define __ELASTOS_DROID_SERVER_WM_STARTINGDATA_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.Content.h>
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::Res::ICompatibilityInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class StartingData : public Object
{
public:
    StartingData(
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

public:
    String mPkg;
    Int32 mTheme;
    AutoPtr<ICompatibilityInfo> mCompatInfo;
    AutoPtr<ICharSequence> mNonLocalizedLabel;
    Int32 mLabelRes;
    Int32 mIcon;
    Int32 mLogo;
    Int32 mWindowFlags;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_STARTINGDATA_H__
