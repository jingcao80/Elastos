
#ifndef  __ELASTOS_DROID_SYSTEMUI_CLOADAVERAGESERVICE_H__
#define  __ELASTOS_DROID_SYSTEMUI_CLOADAVERAGESERVICE_H__

#include "_Elastos_Droid_SystemUI_CLoadAverageService.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/app/Service.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/view/View.h>
#include <elastos/droid/internal/os/ProcessCpuTracker.h>

using Elastos::Droid::Internal::Os::ProcessCpuTracker;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CLoadAverageService)
    , public Elastos::Droid::App::Service
    , public ILoadAverageService
{
private:
    class CpuTracker
        : public ProcessCpuTracker
    {
    public:
        CpuTracker(
            /* [in] */ IPaint* paint);

        // @Override
        CARAPI OnLoadChanged(
            /* [in] */ Float load1,
            /* [in] */ Float load5,
            /* [in] */ Float load15);

        // @Override
        CARAPI OnMeasureProcessName(
            /* [in] */ const String& name,
            /* [out] */ Int32* result);

    public:
        String mLoadText;
        Int32 mLoadWidth;

    private:
        AutoPtr<IPaint> mPaint;
    };

    class LoadView : public Elastos::Droid::View::View
    {
    private:
        class LoadViewHandler : public Handler
        {
        public:
            TO_STRING_IMPL("CLoadAverageService::LoadViewHandler")

            LoadViewHandler(
                /* [in] */ LoadView* host);

            // @Override
            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            LoadView* mHost;
        };

    public:
        LoadView(
            /* [in] */ IContext* c);

        // @Override
        CARAPI_(void) OnDraw(
            /* [in] */ ICanvas* canvas);

    protected:
        // @Override
        CARAPI OnAttachedToWindow();

        // @Override
        CARAPI OnDetachedFromWindow();

        // @Override
        CARAPI OnMeasure(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

        void UpdateDisplay();

    private:
        AutoPtr<IHandler> mHandler;

        AutoPtr<CpuTracker> mStats;

        AutoPtr<IPaint> mLoadPaint;
        AutoPtr<IPaint> mAddedPaint;
        AutoPtr<IPaint> mRemovedPaint;
        AutoPtr<IPaint> mShadowPaint;
        AutoPtr<IPaint> mShadow2Paint;
        AutoPtr<IPaint> mIrqPaint;
        AutoPtr<IPaint> mSystemPaint;
        AutoPtr<IPaint> mUserPaint;
        Float mAscent;
        Int32 mFH;

        Int32 mNeededWidth;
        Int32 mNeededHeight;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnCreate();

    // @Override
    CARAPI OnDestroy();

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

private:
    AutoPtr<IView> mView;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CLOADAVERAGESERVICE_H__
