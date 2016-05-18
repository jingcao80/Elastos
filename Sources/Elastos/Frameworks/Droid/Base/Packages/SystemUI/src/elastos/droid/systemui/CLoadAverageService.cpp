
#include "elastos/droid/systemui/CLoadAverageService.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Internal::Os::IProcessCpuTrackerStats;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CLoadAverageService::CpuTracker::CpuTracker(
    /* [in] */ IPaint* paint)
    : mLoadWidth(0)
{
    ProcessCpuTracker::constructor(FALSE);
    mPaint = paint;
}

ECode CLoadAverageService::CpuTracker::OnLoadChanged(
    /* [in] */ Float load1,
    /* [in] */ Float load5,
    /* [in] */ Float load15)
{
    mLoadText = StringUtils::ToString(load1) + String(" / ") + load5 + String(" / ") + load15;
    Float value = 0;
    mPaint->MeasureText(mLoadText, &value);
    mLoadWidth = (Int32) value;
    return NOERROR;
}

ECode CLoadAverageService::CpuTracker::OnMeasureProcessName(
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Float value = 0;
    mPaint->MeasureText(name, &value);
    *result = (Int32)value;
    return NOERROR;
}

CLoadAverageService::LoadView::LoadViewHandler::LoadViewHandler(
    /* [in] */ LoadView* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode CLoadAverageService::LoadView::LoadViewHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    if (what == 1) {
        mHost->mStats->Update();
        mHost->UpdateDisplay();
        AutoPtr<IMessage> m;
        ObtainMessage(1, (IMessage**)&m);
        Boolean result = FALSE;
        SendMessageDelayed(m, 2000, &result);
    }
    return NOERROR;
}

CLoadAverageService::LoadView::LoadView(
    /* [in] */ IContext* c)
    : mAscent(0)
    , mFH(0)
    , mNeededWidth(0)
    , mNeededHeight(0)
{
    mHandler = new LoadViewHandler(this);

    View::constructor(c);

    SetPadding(4, 4, 4, 4);
    //setBackgroundResource(com.android.internal.R.drawable.load_average_background);

    // Need to scale text size by density...  but we won't do it
    // linearly, because with higher dps it is nice to squeeze the
    // text a bit to fit more of it.  And with lower dps, trying to
    // go much smaller will result in unreadable text.
    Int32 textSize = 10;
    AutoPtr<IResources> res;
    c->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density = 0;
    dm->GetDensity(&density);
    if (density < 1) {
        textSize = 9;
    }
    else {
        textSize = (Int32)(10*density);
        if (textSize < 10) {
            textSize = 10;
        }
    }
    CPaint::New((IPaint**)&mLoadPaint);
    mLoadPaint->SetAntiAlias(TRUE);
    mLoadPaint->SetTextSize(textSize);
    mLoadPaint->SetARGB(255, 255, 255, 255);

    CPaint::New((IPaint**)&mAddedPaint);
    mAddedPaint->SetAntiAlias(TRUE);
    mAddedPaint->SetTextSize(textSize);
    mAddedPaint->SetARGB(255, 128, 255, 128);

    CPaint::New((IPaint**)&mRemovedPaint);
    mRemovedPaint->SetAntiAlias(TRUE);
    mRemovedPaint->SetStrikeThruText(TRUE);
    mRemovedPaint->SetTextSize(textSize);
    mRemovedPaint->SetARGB(255, 255, 128, 128);

    CPaint::New((IPaint**)&mShadowPaint);
    mShadowPaint->SetAntiAlias(TRUE);
    mShadowPaint->SetTextSize(textSize);
    //mShadowPaint.setFakeBoldText(TRUE);
    mShadowPaint->SetARGB(192, 0, 0, 0);
    mLoadPaint->SetShadowLayer(4, 0, 0, 0xff000000);

    CPaint::New((IPaint**)&mShadow2Paint);
    mShadow2Paint->SetAntiAlias(TRUE);
    mShadow2Paint->SetTextSize(textSize);
    //mShadow2Paint.setFakeBoldText(TRUE);
    mShadow2Paint->SetARGB(192, 0, 0, 0);
    mLoadPaint->SetShadowLayer(2, 0, 0, 0xff000000);

    CPaint::New((IPaint**)&mIrqPaint);
    mIrqPaint->SetARGB(0x80, 0, 0, 0xff);
    mIrqPaint->SetShadowLayer(2, 0, 0, 0xff000000);
    CPaint::New((IPaint**)&mSystemPaint);
    mSystemPaint->SetARGB(0x80, 0xff, 0, 0);
    mSystemPaint->SetShadowLayer(2, 0, 0, 0xff000000);
    CPaint::New((IPaint**)&mUserPaint);
    mUserPaint->SetARGB(0x80, 0, 0xff, 0);
    mSystemPaint->SetShadowLayer(2, 0, 0, 0xff000000);

    mLoadPaint->Ascent(&mAscent);
    Float descent = 0;
    mLoadPaint->Descent(&descent);
    mFH = (Int32)(descent - mAscent + .5f);

    mStats = new CpuTracker(mLoadPaint);
    mStats->Init();
    UpdateDisplay();
}

ECode CLoadAverageService::LoadView::OnAttachedToWindow()
{
    View::OnAttachedToWindow();
    Boolean result = FALSE;
    mHandler->SendEmptyMessage(1, &result);
    return NOERROR;
}

ECode CLoadAverageService::LoadView::OnDetachedFromWindow()
{
    View::OnDetachedFromWindow();
    mHandler->RemoveMessages(1);
    return NOERROR;
}

void CLoadAverageService::LoadView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    SetMeasuredDimension(ResolveSize(mNeededWidth, widthMeasureSpec),
            ResolveSize(mNeededHeight, heightMeasureSpec));
}

void CLoadAverageService::LoadView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    View::OnDraw(canvas);
    const Int32 W = mNeededWidth;
    Int32 width = 0;
    GetWidth(&width);
    const Int32 RIGHT = width - 1;

    AutoPtr<CpuTracker> stats = mStats;
    Int32 userTime = 0;
    stats->GetLastUserTime(&userTime);
    Int32 systemTime = 0;
    stats->GetLastSystemTime(&systemTime);
    Int32 iowaitTime = 0;
    stats->GetLastIoWaitTime(&iowaitTime);
    Int32 irqTime = 0;
    stats->GetLastIrqTime(&irqTime);
    Int32 softIrqTime = 0;
    stats->GetLastSoftIrqTime(&softIrqTime);
    Int32 idleTime = 0;
    stats->GetLastIdleTime(&idleTime);

    const Int32 totalTime = userTime + systemTime + iowaitTime + irqTime + softIrqTime + idleTime;
    if (totalTime == 0) {
        return;
    }
    Int32 userW = (userTime * W) / totalTime;
    Int32 systemW = (systemTime * W) / totalTime;
    Int32 irqW = ((iowaitTime + irqTime + softIrqTime) * W) / totalTime;

    Int32 paddingRight = 0;
    GetPaddingRight(&paddingRight);
    Int32 x = RIGHT - paddingRight;
    Int32 value = 0;
    GetPaddingTop(&value);
    Int32 top = value + 2;
    Int32 bottom = value + mFH - 2;

    if (irqW > 0) {
        canvas->DrawRect(x - irqW, top, x, bottom, mIrqPaint);
        x -= irqW;
    }
    if (systemW > 0) {
        canvas->DrawRect(x - systemW, top, x, bottom, mSystemPaint);
        x -= systemW;
    }
    if (userW > 0) {
        canvas->DrawRect(x - userW, top, x, bottom, mUserPaint);
        x -= userW;
    }

    Int32 y = value - (Int32)mAscent;
    canvas->DrawText(stats->mLoadText, RIGHT - paddingRight - stats->mLoadWidth - 1,
            y - 1, mShadowPaint);
    canvas->DrawText(stats->mLoadText, RIGHT - paddingRight - stats->mLoadWidth - 1,
            y + 1, mShadowPaint);
    canvas->DrawText(stats->mLoadText, RIGHT - paddingRight - stats->mLoadWidth + 1,
            y - 1, mShadow2Paint);
    canvas->DrawText(stats->mLoadText, RIGHT - paddingRight - stats->mLoadWidth + 1,
            y + 1, mShadow2Paint);
    canvas->DrawText(stats->mLoadText, RIGHT - paddingRight - stats->mLoadWidth,
            y, mLoadPaint);

    Int32 N = 0;
    stats->CountWorkingStats(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IProcessCpuTrackerStats> s;
        stats->GetWorkingStats(i, (IProcessCpuTrackerStats**)&s);
        AutoPtr<CpuTracker::Stats> st = (CpuTracker::Stats*)s.Get();
        y += mFH;
        top += mFH;
        bottom += mFH;

        userW = (st->mRel_utime * W) / totalTime;
        systemW = (st->mRel_stime * W) / totalTime;
        x = RIGHT - paddingRight;
        if (systemW > 0) {
            canvas->DrawRect(x - systemW, top, x, bottom, mSystemPaint);
            x -= systemW;
        }
        if (userW > 0) {
            canvas->DrawRect(x - userW, top, x, bottom, mUserPaint);
            x -= userW;
        }

        canvas->DrawText(st->mName, RIGHT - paddingRight - st->mNameWidth - 1,
                y - 1, mShadowPaint);
        canvas->DrawText(st->mName, RIGHT - paddingRight - st->mNameWidth - 1,
                y + 1, mShadowPaint);
        canvas->DrawText(st->mName, RIGHT - paddingRight - st->mNameWidth + 1,
                y - 1, mShadow2Paint);
        canvas->DrawText(st->mName, RIGHT - paddingRight - st->mNameWidth + 1,
                y + 1, mShadow2Paint);
        AutoPtr<IPaint> p = mLoadPaint;
        if (st->mAdded) p = mAddedPaint;
        if (st->mRemoved) p = mRemovedPaint;
        canvas->DrawText(st->mName, RIGHT - paddingRight - st->mNameWidth, y, p);
    }
}

void CLoadAverageService::LoadView::UpdateDisplay()
{
    AutoPtr<CpuTracker> stats = mStats;
    Int32 NW = 0;
    stats->CountWorkingStats(&NW);

    Int32 maxWidth = stats->mLoadWidth;
    for (Int32 i = 0; i < NW; i++) {
        AutoPtr<IProcessCpuTrackerStats> s;
        stats->GetWorkingStats(i, (IProcessCpuTrackerStats**)&s);
        AutoPtr<CpuTracker::Stats> st = (CpuTracker::Stats*)s.Get();
        if (st->mNameWidth > maxWidth) {
            maxWidth = st->mNameWidth;
        }
    }

    Int32 v1 = 0, v2 = 0;
    Int32 neededWidth = (GetPaddingLeft(&v1), v1) + (GetPaddingRight(&v2), v2) + maxWidth;
    Int32 neededHeight = (GetPaddingTop(&v1), v1) + (GetPaddingBottom(&v2), v2) + (mFH * (1 + NW));
    if (neededWidth != mNeededWidth || neededHeight != mNeededHeight) {
        mNeededWidth = neededWidth;
        mNeededHeight = neededHeight;
        RequestLayout();
    }
    else {
        Invalidate();
    }
}


CAR_OBJECT_IMPL(CLoadAverageService)
CAR_INTERFACE_IMPL(CLoadAverageService, Service, ILoadAverageService)
ECode CLoadAverageService::OnCreate()
{
    Service::OnCreate();
    mView = new LoadView(this);
    AutoPtr<IWindowManagerLayoutParams> params;
    CWindowManagerLayoutParams::New(
        IViewGroupLayoutParams::MATCH_PARENT,
        IViewGroupLayoutParams::WRAP_CONTENT,
        IWindowManagerLayoutParams::TYPE_SECURE_SYSTEM_OVERLAY,
        IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE|
        IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE,
        IPixelFormat::TRANSLUCENT, (IWindowManagerLayoutParams**)&params);
    params->SetGravity(IGravity::END | IGravity::TOP);
    AutoPtr<ICharSequence> title;
    CString::New(String("Load Average"), (ICharSequence**)&title);
    params->SetTitle(title);
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    AutoPtr<IWindowManager> wm = IWindowManager::Probe(obj);
    IViewManager::Probe(wm)->AddView(mView, IViewGroupLayoutParams::Probe(params));
    return NOERROR;
}

ECode CLoadAverageService::OnDestroy()
{
    Service::OnDestroy();
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    IViewManager::Probe(obj)->RemoveView(mView);
    mView = NULL;
    return NOERROR;
}

ECode CLoadAverageService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);
    *binder = NULL;
    return NOERROR;
}


} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
