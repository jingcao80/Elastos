#include "_Elastos_Droid_View_CDisplayManagerAw.h"

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CDisplayManagerAw)
{
public:
    CDisplayManagerAw();

    CARAPI constructor();

    CARAPI GetDisplayCount(
        /* [out] */ Int32* result);

    CARAPI GetDisplayOpenStatus(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Boolean* result);

    CARAPI GetDisplayHotPlugStatus(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayOutputType(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayOutputFormat(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayWidth(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayHeight(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayPixelFormat(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI SetDisplayParameter(
        /* [in] */ Int32 mDisplay,
        /* [in] */ Int32 param0,
        /* [in] */ Int32 param1,
        /* [out] */ Int32* result);

    CARAPI SetDisplayMode(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    CARAPI GetDisplayMode(
        /* [out] */ Int32* result);

    CARAPI SetDisplayOutputType(
        /* [in] */ Int32 mDisplay,
        /* [in] */ Int32 type,
        /* [in] */ Int32 format,
        /* [out] */ Int32* result);

    CARAPI OpenDisplay(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI CloseDisplay(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI SetDisplayBacklightMode(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    CARAPI SetDisplayMaster(
        /* [in] */ Int32 mDisplay,
        /* [out] */ Int32* result);

    CARAPI GetDisplayMaster(
        /* [out] */ Int32* result);

    CARAPI GetMaxWidthDisplay(
        /* [out] */ Int32* result);

    CARAPI GetMaxHdmiMode(
        /* [out] */ Int32* result);

    CARAPI GetDisplayBacklightMode(
        /* [out] */ Int32* result);

    CARAPI IsSupportHdmiMode(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    CARAPI IsSupport3DMode(
        /* [out] */ Int32* result);

    CARAPI GetHdmiHotPlugStatus(
        /* [out] */ Int32* result);

    CARAPI GetTvHotPlugStatus(
        /* [out] */ Int32* result);

    CARAPI SetDisplayAreaPercent(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 percent,
        /* [out] */ Int32* result);

    CARAPI GetDisplayAreaPercent(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* result);

    CARAPI SetDisplayBright(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 bright,
        /* [out] */ Int32* result);

    CARAPI GetDisplayBright(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* result);

    CARAPI SetDisplayContrast(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 contrast,
        /* [out] */ Int32* result);

    CARAPI GetDisplayContrast(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* result);

    CARAPI SetDisplaySaturation(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 saturation,
        /* [out] */ Int32* result);

    CARAPI GetDisplaySaturation(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* result);

    CARAPI SetDisplayHue(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 hue,
        /* [out] */ Int32* result);

    CARAPI GetDisplayHue(
        /* [in] */ Int32 displayno,
        /* [out] */ Int32* result);

    CARAPI Set3DLayerOffset(
        /* [in] */ Int32 displayno,
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [out] */ Int32* result);
private:

    CARAPI_(AutoPtr<IIDisplayManagerAw>) GetService();
    const static String Tab;
    AutoPtr<IIDisplayManagerAw> mService;
    AutoPtr<IBinder> mToken;

};

}// namespace Elastos
}// namespace Droid
}// namespace View
