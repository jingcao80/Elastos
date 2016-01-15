#ifndef __ELASTOS_DROID_SERVER_CDISPLAYMANAGERSERVICEAW_H__
#define __ELASTOS_DROID_SERVER_CDISPLAYMANAGERSERVICEAW_H__

#include "_Elastos_Droid_Server_CDisplayManagerServiceAw.h"

using Elastos::Core::IClassLoader;
using Elastos::Core::IRunnable;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::EIID_IIWindowManager;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CDisplayManagerServiceAw)
{
public:

    class DisplayThread
        : public ThreadBase
    {
    public:
        DisplayThread(
            /* [in] */ IContext* context,
            /* [in] */ CDisplayManagerServiceAw* service,
            /* [in] */ IIPowerManager* pm);
        virtual CARAPI Run();

    private:
        CARAPI_(Object*) GetSelfLock();

    private:
        Object mLock;
        CDisplayManagerServiceAw* mService;
        AutoPtr<IContext> mContext;
        AutoPtr<IIPowerManager> mPM;
        Int32 mHdmihotplug;
        Int32 mTvdachotplug;
    };

public:
    CDisplayManagerServiceAw();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIPowerManager* pm);

    CARAPI SystemReady();

    CARAPI GetDisplayCount(
        /* [out] */ Int32* rst);

    CARAPI GetDisplayOpenStatus(
        /* [in] */ Int32 displayno,
        /* [out] */ Boolean* rst);

    CARAPI GetDisplayHotPlugStatus(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayOutputType(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayOutputFormat(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayWidth(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayHeight(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI SetDisplayParameter(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 param0,
        /* [in] */ Int32 param1,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayPixelFormat(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI SetDisplayMode(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayMode(
        /* [out] */ Int32* rst);

    CARAPI SetDisplayOutputType(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 type,
        /* [in] */ Int32 format,
        /* [out] */ Int32* rst);

    CARAPI OpenDisplay(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI CloseDisplay(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI SetDisplayMaster(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayMaster(
        /* [out] */ Int32* rst);

    CARAPI GetMaxWidthDisplay(
        /* [out] */ Int32* rst);

    CARAPI GetMaxHdmiMode(
        /* [out] */ Int32* rst);

    CARAPI SetDisplayBacklightMode(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayBacklightMode(
        /* [out] */ Int32* rst);

    CARAPI IsSupportHdmiMode(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* rst);

    CARAPI IsSupport3DMode(
        /* [out] */ Int32* rst);

    CARAPI GetHdmiHotPlugStatus(
        /* [out] */ Int32* rst);

    CARAPI GetTvHotPlugStatus(
        /* [out] */ Int32* rst);

    CARAPI SetDisplayAreaPercent(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 percent,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayAreaPercent(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI SetDisplayBright(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 bright,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayBright(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI SetDisplayContrast(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 contrast,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayContrast(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI SetDisplaySaturation(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 saturation,
        /* [out] */ Int32* rst);

    CARAPI GetDisplaySaturation(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI SetDisplayHue(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 hue,
        /* [out] */ Int32* rst);

    CARAPI GetDisplayHue(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI SetHdmiHotplugStatus(
        /* [in] */ Int32 status,
        /* [out] */ Int32* rst);

    CARAPI SettvHotplugStatus(
        /* [in] */ Int32 status,
        /* [in] */ Int32* rst);

    CARAPI StartWifiDisplaySend(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 mode,
        /* [out] */ Int32* rst);

    CARAPI EndWifiDisplaySend(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI StartWifiDisplayReceive(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 mode,
        /* [out] */ Int32* rst);

    CARAPI EndWifiDisplayReceive(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* rst);

    CARAPI UpdateSendClient(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* rst);

    CARAPI Set3DLayerOffset(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [out] */ Int32* rst);

    CARAPI ToString(
        /* [out] */ String* str);

private:

    CARAPI_(void) SendHdmiIntent();

    CARAPI_(void) SendTvDacIntent();

    CARAPI_(void) SendOutputChangedIntent();

    CARAPI_(void)  NativeInit();

    CARAPI_(Int32) NativeGetDisplayCount();

    CARAPI_(Int32) NativeGetDisplayOutputType(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeGetDisplayOutputFormat(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeGetDisplayWidth(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeGetDisplayHeight(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeGetDisplayPixelFormat(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeSetDisplayMode(Int32 mode);

    CARAPI_(Int32) NativeSetDisplayOutputType(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 type,
        /* [in] */ Int32 format);

    CARAPI_(Int32) NativeOpenDisplay(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeCloseDisplay(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeSetDisplayMaster(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeGetDisplayOpen(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeGetDisplayHotPlug(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeGetHdmiHotPlug();

    CARAPI_(Int32) NativeGetTvDacHotPlug();

    CARAPI_(Int32) NativeGetDisplayMode();

    CARAPI_(Int32) NativeGetDisplayMaster();

    CARAPI_(Int32) NativeGetMaxWidthDisplay();

    CARAPI_(Int32) NativeGetMaxHdmiMode();

    CARAPI_(Int32) NativeSetDisplayParameter(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 para0,
        /* [in] */ Int32 para1);

    CARAPI_(Int32) NativeSetDisplayBacklihgtMode(Int32 mode);

    CARAPI_(Int32) NativeSetDisplayAreaPercent(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 percent);

    CARAPI_(Int32) NativeGetDisplayAreaPercent(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeSetDisplayBright(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 bright);

    CARAPI_(Int32) NativeGetDisplayBright(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeSetDisplayContrast(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 contrast);

    CARAPI_(Int32) NativeGetDisplayContrast(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeSetDisplaySaturation(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 saturation);

    CARAPI_(Int32) NativeGetDisplaySaturation(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeSetDisplayHue(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 saturation);

    CARAPI_(Int32) NativeGetDisplayHue(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeIsSupportHdmiMode(Int32 mode);

    CARAPI_(Int32) NativeIsSupport3DMode();

    CARAPI_(Int32) NativeSet3DLayerOffset(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 left,
        /* [in] */ Int32 right);

    CARAPI_(Int32) NativeStartWifiDisplaySend(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 mode);

    CARAPI_(Int32) NativeEndWifiDisplaySend(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeStartWifiDisplayReceive(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 mode);

    CARAPI_(Int32) NativeEndWifiDisplayReceive(
        /* [in] */ Int32 displayno);

    CARAPI_(Int32) NativeUpdateSendClient(Int32 mode);
private:
    const static String TAG;
    const static Boolean LOCAL_LOGV;
    static AutoPtr<DisplayThread> sThread;
    static Boolean sThreadStarted;

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IIPowerManager> mPm;
    AutoPtr<IIWindowManager> mWindowManager;
    Int32 mHdmiPlugin;
    Int32 mTvDacPlugin;
    Int32 mBacklightMode;
    Int32 mCurrentType;
    Int32 mCurrentMode;
};

}// namespace Server
}// namespace Droid
}// namespace Elastos

#endif
