
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIDEOVIEWCLIENT_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIDEOVIEWCLIENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
 *  Main callback class used by ContentVideoView.
 *
 *  This contains the superset of callbacks that must be implemented by the embedder.
 *
 *  onShowCustomView and onDestoryContentVideoView must be implemented,
 *  getVideoLoadingProgressView() is optional, and may return null if not required.
 *
 *  The implementer is responsible for displaying the Android view when
 *  {@link #onShowCustomView(View)} is called.
 */
class ContentVideoViewClient : public Object
{
public:
    /**
     * Called when the video view is ready to be shown. Must be implemented.
     * @param view The view to show.
     * @return whether the video is actually shown.
     */
    virtual CARAPI_(Boolean) OnShowCustomView(
        /* [in] */ IView* view) = 0;

    /**
     * Called when it's time to destroy the video view. Must be implemented.
     */
    virtual CARAPI_(void) OnDestroyContentVideoView() = 0;

    /**
     * Allows the embedder to replace the view indicating that the video is loading.
     * If null is returned, the default video loading view is used.
     */
    virtual CARAPI_(AutoPtr<IView>) GetVideoLoadingProgressView() = 0;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIDEOVIEWCLIENT_H__
