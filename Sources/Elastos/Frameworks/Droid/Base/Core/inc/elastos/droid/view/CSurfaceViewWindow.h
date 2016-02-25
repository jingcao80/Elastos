
#ifndef __ELASTOS_DROID_VIEW_CSURFACEVIEWWINDOW_H__
#define __ELASTOS_DROID_VIEW_CSURFACEVIEWWINDOW_H__

#include "_Elastos_Droid_View_CSurfaceViewWindow.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/view/BaseIWindow.h"

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::View::IBaseIWindow;
using Elastos::Droid::Internal::View::BaseIWindow;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CSurfaceViewWindow)
    , public BaseIWindow
{
public:
    CAR_OBJECT_DECL()

    CSurfaceViewWindow();

    CARAPI constructor(
        /* [in] */ ISurfaceView* surfaceView);

    CARAPI Resized(
        /* [in] */ IRect* frame,
        /* [in] */ IRect* overscanInsets,
        /* [in] */ IRect* coveredInsets,
        /* [in] */ IRect* visibleInsets,
        /* [in] */ IRect* stableInsets,
        /* [in] */ Boolean reportDraw,
        /* [in] */ IConfiguration* newConfig);


    CARAPI DispatchGetNewSurface();

    /**
     * Tell the window that it is either gaining or losing focus.  Keep it up
     * to date on the current state showing navigational focus (touch mode) too.
     */
    CARAPI WindowFocusChanged(
        /* [in] */ Boolean hasFocus,
        /* [in] */ Boolean touchEnabled);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    friend class SurfaceView;

    AutoPtr<IWeakReference> mSurfaceView;

    Int32 mCurWidth;// = -1;
    Int32 mCurHeight;// = -1;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CSURFACEVIEWWINDOW_H__
