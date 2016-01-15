#include "CDisplayManagerServiceAw.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include <gui/SurfaceComposerClient.h>
#include <ui/DisplayCommand.h>
#include <hardware/display.h>

using android::SurfaceComposerClient;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::View::IDisplayManagerAw;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::App::ActivityManagerNative;

namespace Elastos {
namespace Droid {
namespace Server {

const String CDisplayManagerServiceAw::TAG("DisplayManagerServiceAw");
const Boolean CDisplayManagerServiceAw::LOCAL_LOGV = TRUE;
AutoPtr<CDisplayManagerServiceAw::DisplayThread> CDisplayManagerServiceAw::sThread;
Boolean CDisplayManagerServiceAw::sThreadStarted = FALSE;

//===================================================================================
//          CDisplayManagerServiceAw::DisplayThread
//===================================================================================
CDisplayManagerServiceAw::DisplayThread::DisplayThread(
    /* [in] */ IContext* context,
    /* [in] */ CDisplayManagerServiceAw* service,
    /* [in] */ IIPowerManager* pm)
    : mHdmihotplug(0)
    , mTvdachotplug(0)
{
    String superPara("DisplayManagerPolicy");
    Thread::constructor(superPara);
    mService = service;
    mContext = context;
    mPM = pm;
}

ECode CDisplayManagerServiceAw::DisplayThread::Run()
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_FOREGROUND);
    Process::SetCanSelfBackground(FALSE);
    {
        AutoLock lock(mLock);
        NotifyAll();
    }
    while(TRUE)
    {
        Int32 hotplug;
        hotplug = mService->NativeGetHdmiHotPlug();
        if(hotplug != mHdmihotplug)
        {
            mHdmihotplug = hotplug;
            Int32 rst;
            mService->SetHdmiHotplugStatus(mHdmihotplug, &rst);
            mService->SendHdmiIntent();
        }

        hotplug = mService->NativeGetTvDacHotPlug();
        if(hotplug != mTvdachotplug)
        {
            mTvdachotplug = hotplug;
            Int32 rst;
            mService->SettvHotplugStatus(mTvdachotplug, &rst);

            mService->SendTvDacIntent();
        }

        Int32 type = mService->NativeGetDisplayOutputType(0);
        Int32 mode = mService->NativeGetDisplayOutputFormat(0);
        if (type != mService->mCurrentType || mode != mService->mCurrentMode) {
            mService->mCurrentType = type;
            mService->mCurrentMode = mode;
            mService->SendOutputChangedIntent();
        }

        FAIL_RETURN(Thread::Sleep(500));
    }
    return NOERROR;
}

Object* CDisplayManagerServiceAw::DisplayThread::GetSelfLock()
{
    return &mLock;
}

//===================================================================================
//          CDisplayManagerServiceAw
//===================================================================================
CDisplayManagerServiceAw::CDisplayManagerServiceAw()
        : mHdmiPlugin(0)
        , mTvDacPlugin(0)
        , mBacklightMode(0)
        , mCurrentType(-1)
        , mCurrentMode(-1)
{}

ECode CDisplayManagerServiceAw::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIPowerManager* pm)
{
    mContext = context;
    mPm = pm;
    NativeInit();
    // set initial hotplug status
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    if(sysProp->Get(String("ro.display.switch"), &value), value == "1")
    {
        if (sThreadStarted == FALSE)
        {
            sThread = new DisplayThread(mContext,this,mPm);
            sThread->Start();
            sThreadStarted = TRUE;
        }
    }

    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> sTemp;
    sm->GetService(IContext::WINDOW_SERVICE, (IInterface**)&sTemp);
    mWindowManager = (IIWindowManager*)sTemp->Probe(EIID_IIWindowManager);
    //boolean enable = Settings.System.getInt(mContext.getContentResolver(),Settings.System.SMART_BRIGHTNESS_ENABLE,0) != 0 ? true : false;
    Boolean enable = FALSE;
    Int32 rst;
    SetDisplayBacklightMode(enable ? 1 : 0, &rst);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SystemReady()
{
    // init bright
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 brightness;
    Int32 rst;
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    settingsSystem->GetInt32(cr, ISettingsSystem::COLOR_BRIGHTNESS, IDisplayManagerAw::DEF_BRIGHTNESS, &brightness);
    SetDisplayBright(0, brightness, &rst);
    // init saturation
    Int32 saturation;
    settingsSystem->GetInt32(cr, ISettingsSystem::COLOR_SATURATION, IDisplayManagerAw::DEF_SATURATION, &saturation);
    SetDisplaySaturation(0, saturation, &rst);
    // init contrast
    Int32 contrast;
    settingsSystem->GetInt32(cr, ISettingsSystem::COLOR_CONTRAST, IDisplayManagerAw::DEF_CONTRAST, &contrast);
    SetDisplayContrast(0, contrast, &rst);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayCount(
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayCount();
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayOpenStatus(
    /* [in] */ Int32 displayno,
    /* [out] */ Boolean* rst)
{
    Int32 ret;
    ret = NativeGetDisplayOpen(displayno);
    if(ret == 0)
    {
        *rst = FALSE;
        return NOERROR;
    }
    *rst = TRUE;
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayHotPlugStatus(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayHotPlug(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayOutputType(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayOutputType(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayOutputFormat(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayOutputFormat(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayWidth(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayWidth(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayHeight(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayHeight(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetDisplayParameter(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 param0,
    /* [in] */ Int32 param1,
    /* [out] */ Int32* rst)
{
    *rst = NativeSetDisplayParameter(displayno, param0, param1);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayPixelFormat(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayPixelFormat(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetDisplayMode(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* rst)
{
    *rst = -1;
    if(mode == 3 || mode == 4)
    {
        Int32 hwrotation = 0;

        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        sysProp->GetInt32(String("ro.sf.hwrotation"),0, &hwrotation);
        if((hwrotation==90) || (hwrotation==270))
        {
            if(FAILED(mWindowManager->FreezeRotation(1)));
            {
                return NOERROR;
            }
        }
        else
        {
            if(NativeGetDisplayWidth(0) < NativeGetDisplayHeight(0) )
            {
                if(FAILED(mWindowManager->FreezeRotation(1)));
                {
                    return NOERROR;
                }
            }
            else
            {
                if(FAILED(mWindowManager->FreezeRotation(0)));
                {
                    return NOERROR;
                }
            }
            //mWindowManager.freezeRotation(1);
        }
    }
    else
    {
        if(FAILED(mWindowManager->ThawRotation()))
        {
            return NOERROR;
        }
    }
    // catch (Exception e)
    // {
    //     Log.d(TAG,"freezeRotation or thawRotation failed!");
    //     return -1;
    // }
    *rst = NativeSetDisplayMode(mode);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayMode(
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayMode();
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetDisplayOutputType(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 type,
    /* [in] */ Int32 format,
    /* [out] */ Int32* rst)
{
    *rst = NativeSetDisplayOutputType(displayno, type, format);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::OpenDisplay(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeOpenDisplay(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::CloseDisplay(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeCloseDisplay(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetDisplayMaster(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeSetDisplayMaster(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayMaster(
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayMaster();
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetMaxWidthDisplay(
    /* [out] */ Int32* rst)
{
    *rst = NativeGetMaxWidthDisplay();
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetMaxHdmiMode(
    /* [out] */ Int32* rst)
{
    *rst =NativeGetMaxHdmiMode();
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetDisplayBacklightMode(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* rst)
{
    mBacklightMode = mode;
    *rst = NativeSetDisplayBacklihgtMode(mode);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayBacklightMode(
    /* [out] */ Int32* rst)
{
    *rst = mBacklightMode;
    return NOERROR;
}

ECode CDisplayManagerServiceAw::IsSupportHdmiMode(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* rst)
{
    *rst = NativeIsSupportHdmiMode(mode);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::IsSupport3DMode(
    /* [out] */ Int32* rst)
{
    *rst = NativeIsSupport3DMode();
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetHdmiHotPlugStatus(
    /* [out] */ Int32* rst)
{
    *rst = NativeGetHdmiHotPlug();
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetTvHotPlugStatus(
    /* [out] */ Int32* rst)
{
    *rst = NativeGetTvDacHotPlug();
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetDisplayAreaPercent(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 percent,
    /* [out] */ Int32* rst)
{
    *rst = NativeSetDisplayAreaPercent(displayno, percent);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayAreaPercent(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayAreaPercent(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetDisplayBright(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 bright,
    /* [out] */ Int32* rst)
{
    *rst = NativeSetDisplayBright(displayno, bright);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayBright(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayBright(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetDisplayContrast(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 contrast,
    /* [out] */ Int32* rst)
{
    *rst = NativeSetDisplayContrast(displayno, contrast);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayContrast(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayContrast(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetDisplaySaturation(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 saturation,
    /* [out] */ Int32* rst)
{
    *rst = NativeSetDisplaySaturation(displayno, saturation);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplaySaturation(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplaySaturation(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetDisplayHue(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 hue,
    /* [out] */ Int32* rst)
{
    *rst = NativeSetDisplayHue(displayno, hue);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::GetDisplayHue(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeGetDisplayHue(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SetHdmiHotplugStatus(
    /* [in] */ Int32 status,
    /* [out] */ Int32* rst)
{
    mHdmiPlugin = status;
    *rst = 0;
    return NOERROR;
}

ECode CDisplayManagerServiceAw::SettvHotplugStatus(
    /* [in] */ Int32 status,
    /* [in] */ Int32* rst)
{
    mTvDacPlugin = status;
    *rst = 0;
    return NOERROR;
}

ECode CDisplayManagerServiceAw::StartWifiDisplaySend(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* rst)
{
    *rst = NativeStartWifiDisplaySend(displayno, mode);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::EndWifiDisplaySend(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeEndWifiDisplaySend(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::StartWifiDisplayReceive(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* rst)
{
    *rst = NativeStartWifiDisplayReceive(displayno, mode);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::EndWifiDisplayReceive(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* rst)
{
    *rst = NativeEndWifiDisplayReceive(displayno);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::UpdateSendClient(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* rst)
{
    *rst = NativeUpdateSendClient(mode);
    return NOERROR;
}

ECode CDisplayManagerServiceAw::ToString(
        /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

ECode CDisplayManagerServiceAw::Set3DLayerOffset(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [out] */ Int32* rst)
{
    *rst = NativeSet3DLayerOffset(displayno, left, right);
    return NOERROR;
}

void CDisplayManagerServiceAw::SendHdmiIntent()
{
    //  Pack up the values and broadcast them to everyone
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_HDMISTATUS_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
            | IIntent::FLAG_RECEIVER_REPLACE_PENDING);

    intent->PutExtra(IDisplayManagerAw::EXTRA_HDMISTATUS, mHdmiPlugin);

    ActivityManagerNative::BroadcastStickyIntent(intent, String(NULL), IUserHandle::USER_ALL);
}

void CDisplayManagerServiceAw::SendTvDacIntent()
{
    //  Pack up the values and broadcast them to everyone
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.intent.action.TVDACSTATUS_CHANGED")/* IIntent::ACTION_TVDACSTATUS_CHANGED */, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
            | IIntent::FLAG_RECEIVER_REPLACE_PENDING);

    intent->PutExtra(IDisplayManagerAw::EXTRA_TVSTATUS, mTvDacPlugin);

    ActivityManagerNative::BroadcastStickyIntent(intent, String(NULL), IUserHandle::USER_ALL);
}

void CDisplayManagerServiceAw::SendOutputChangedIntent()
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.intent.action.TVDACSTATUS_CHANGED")/* IIntent::ACTION_TVDACSTATUS_CHANGED */, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY
            | IIntent::FLAG_RECEIVER_REPLACE_PENDING);

    intent->PutExtra(IDisplayManagerAw::EXTRA_DISPLAY_TYPE, mCurrentType);
    intent->PutExtra(IDisplayManagerAw::EXTRA_DISPLAY_MODE, mCurrentMode);

    ActivityManagerNative::BroadcastStickyIntent(intent, String(NULL), IUserHandle::USER_ALL);
}

void CDisplayManagerServiceAw::NativeInit()
{}

Int32 CDisplayManagerServiceAw::NativeGetDisplayCount()
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPCOUNT,0,0,0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayOutputType(
    /* [in] */ Int32 displayno)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPPARA, displayno, DISPLAY_OUTPUT_TYPE, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayOutputFormat(
    /* [in] */ Int32 displayno)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPPARA, displayno, DISPLAY_OUTPUT_FORMAT, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayWidth(
    /* [in] */ Int32 displayno)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPPARA, displayno, DISPLAY_OUTPUT_WIDTH, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayHeight(
    /* [in] */ Int32 displayno)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPPARA, displayno, DISPLAY_OUTPUT_HEIGHT, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayPixelFormat(
    /* [in] */ Int32 displayno)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPPARA, displayno, DISPLAY_OUTPUT_PIXELFORMAT, 0);
}

Int32 CDisplayManagerServiceAw::NativeSetDisplayMode(
    /* [in] */ Int32 mode)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETDISPMODE,mode,0,0);
}

Int32 CDisplayManagerServiceAw::NativeSetDisplayOutputType(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 type,
    /* [in] */ Int32 format)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_CHANGEDISPMODE,displayno, type, format);
}

Int32 CDisplayManagerServiceAw::NativeOpenDisplay(
    /* [in] */ Int32 displayno)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_OPENDISP, displayno, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeCloseDisplay(
    /* [in] */ Int32 displayno)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_CLOSEDISP, displayno, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeSetDisplayMaster(
    /* [in] */ Int32 displayno)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETMASTERDISP, displayno, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayOpen(
    /* [in] */ Int32 displayno)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPPARA, displayno, DISPLAY_OUTPUT_ISOPEN, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayHotPlug(
    /* [in] */ Int32 displayno)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPPARA, displayno, DISPLAY_OUTPUT_HOTPLUG, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetHdmiHotPlug()
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETHDMISTATUS, 0, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetTvDacHotPlug()
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETTVSTATUS, 0, 0,  0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayMode()
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPLAYMODE, 0, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayMaster()
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETMASTERDISP, 0, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetMaxWidthDisplay()
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETMAXWIDTHDISP, 0, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetMaxHdmiMode()
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETMAXHDMIMODE, 0, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeSetDisplayParameter(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 para0,
    /* [in] */ Int32 para1)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETDISPPARA,displayno,para0,para1);
}

Int32 CDisplayManagerServiceAw::NativeSetDisplayBacklihgtMode(Int32 mode)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETBACKLIGHTMODE,mode,0,0);
}

Int32 CDisplayManagerServiceAw::NativeSetDisplayAreaPercent(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 percent)
{
    return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETAREAPERCENT,displayno,percent,0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayAreaPercent(
    /* [in] */ Int32 displayno)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETAREAPERCENT,displayno,0,0);
}

Int32 CDisplayManagerServiceAw::NativeSetDisplayBright(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 bright)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETBRIGHT,displayno,bright,0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayBright(
    /* [in] */ Int32 displayno)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETBRIGHT,displayno,0,0);
}

Int32 CDisplayManagerServiceAw::NativeSetDisplayContrast(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 contrast)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETCONTRAST,displayno,contrast,0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayContrast(
    /* [in] */ Int32 displayno)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETCONTRAST,displayno,0,0);
}

Int32 CDisplayManagerServiceAw::NativeSetDisplaySaturation(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 saturation)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETSATURATION, displayno, saturation, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplaySaturation(
    /* [in] */ Int32 displayno)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETSATURATION, displayno, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeSetDisplayHue(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 saturation)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETHUE, displayno, saturation, 0);
}

Int32 CDisplayManagerServiceAw::NativeGetDisplayHue(
    /* [in] */ Int32 displayno)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETHUE, displayno, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeIsSupportHdmiMode(Int32 mode)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_ISSUPPORTHDMIMODE, mode, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeIsSupport3DMode()
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_ISSUPPORTHDMIMODE,DISPLAY_TVFORMAT_1080P_24HZ_3D_FP, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeSet3DLayerOffset(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SET3DLAYEROFFSET, displayno, left, right);
}

Int32 CDisplayManagerServiceAw::NativeStartWifiDisplaySend(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 mode)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_STARTWIFIDISPLAYSEND, displayno, mode, 0);
}

Int32 CDisplayManagerServiceAw::NativeEndWifiDisplaySend(
    /* [in] */ Int32 displayno)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_ENDWIFIDISPLAYSEND, displayno, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeStartWifiDisplayReceive(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 mode)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_STARTWIFIDISPLAYRECEIVE, displayno, mode, 0);
}

Int32 CDisplayManagerServiceAw::NativeEndWifiDisplayReceive(
    /* [in] */ Int32 displayno)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_ENDWIFIDISPLAYRECEIVE, displayno, 0, 0);
}

Int32 CDisplayManagerServiceAw::NativeUpdateSendClient(Int32 mode)
{
    return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_UPDATESENDCLIENT, mode, 0, 0);
}

}// namespace Server
}// namespace Droid
}// namespace Elastos
