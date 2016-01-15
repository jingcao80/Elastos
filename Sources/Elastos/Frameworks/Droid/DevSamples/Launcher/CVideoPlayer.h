
#ifndef __CVIDEOPLAYER_H__
#define __CVIDEOPLAYER_H__

#include "elastos/droid/app/Activity.h"
#include "_CVideoPlayer.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Widget::IVideoView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

#define VideoPlayer_VIDEO_ACTION "org.mozilla.gecko.PLAY_VIDEO"

class CVideoPlayer : public Activity
{
public:

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    ECode GetSpecFromYouTubeVideoID(
        /* [in] */ const String& id,
        /* [out] */ String* spec);

private:
    AutoPtr<IVideoView> mVideoView;
};

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CVIDEOPLAYER_H__
