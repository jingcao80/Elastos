
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_BOOTANIMATIONVIEW_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_BOOTANIMATIONVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/view/View.h"


using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class BootAnimationView
    : public Elastos::Droid::View::View
    , public IView
    , public ElRefBase
{
// Inner class of global variable.
private:
    class BootHandler
        : public HandlerBase
    {
    public:
        BootHandler(
            /* [in] */ BootAnimationView* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        BootAnimationView* mHost;
    };

public:
    BootAnimationView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL);

    CAR_INTERFACE_DECL();

    IVIEW_METHODS_DECL();

    CARAPI_(void) StartShowBatteryCharge(
        /* [in] */ Int32 percent);

    CARAPI_(void) HideScreen(
        /* [in] */ Boolean enable);

    CARAPI_(void) ShowBootInitLogo(
        /* [in] */ Int32 logo);

    CARAPI_(void) ShowInitLogo();

    CARAPI_(void) HideBootInitLogo();

    CARAPI_(void) ShowBootAnimation();

    CARAPI_(void) HideBootAnimation();

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(AutoPtr<IBitmap>) GetImageFromAssetsFile(
        /* [in] */ IContext* context,
        /* [in] */ const String& filepath);

    CARAPI_(AutoPtr<IBitmap>) GetImageFromSystemPath(
        /* [in] */ IContext* context,
        /* [in] */ const String& filepath);

    CARAPI_(AutoPtr<IBitmap>) ZoomBitmap(
        /* [in] */ IBitmap* bmp,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI_(void) DrawBatteryFrame();

    CARAPI_(void) InitMemCanvas();

private:
     static const Boolean DEBUG;
     static const String TAG;
     static const String BOOT_LOGO_PATH;
     static const String INIT_LOGO_PATH;

     const static Int32 nsize;

     const static Int32 FRAME_RATE;
     const static Int32 MSG_UPDATE_FRAME;
     const static Int32 MSG_UPDATE;

private:
    AutoPtr<IBitmap> mBootLogo;
    AutoPtr<IBitmap> mInitLogo;
    AutoPtr< ArrayOf< AutoPtr<IBitmap> > > mBat;

    AutoPtr<IBitmap> mMemBitmap;
    AutoPtr<ICanvas> mMemCanvas;
    AutoPtr<IPaint> mMemPaint;

    Int32 mW,mH;

    Int32 cframe;
    Int32 sframe;

    AutoPtr<BootAnimationView> mSlef;

    Int32 drawWhat;

private:
     AutoPtr<IHandler> mHandler;

};

} // namespace Elastos
} // namespace Droid
} // namespace Internal
} // namespace Policy
} // namespace Impl

#endif // __GLOBALACTIONS_H__
