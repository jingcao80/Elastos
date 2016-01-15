#include "elastos/droid/view/CDisplayManagerAw.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/CBinder.h"

using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::CBinder;

namespace Elastos {
namespace Droid {
namespace View {

const String CDisplayManagerAw::Tab("DislayManagerAw");

CDisplayManagerAw::CDisplayManagerAw()
{
    CBinder::New((IBinder**)&mToken);
    mService = GetService();
}

ECode CDisplayManagerAw::constructor()
{
    return NOERROR;
}

ECode CDisplayManagerAw::GetDisplayCount(
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        if(mService == NULL)
        {
            mService = GetService();
        }
        return  mService->GetDisplayCount(result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayOpenStatus(
    /* [in] */ Int32 mDisplay,
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    // try
    // {
        if(mService == NULL)
        {
            mService = GetService();
        }
        return  mService->GetDisplayOpenStatus(mDisplay, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayHotPlugStatus(
    /* [in] */ Int32 mDisplay,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        if(mService == NULL)
        {
            mService = GetService();
        }
        return  mService->GetDisplayHotPlugStatus(mDisplay, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayOutputType(
    /* [in] */ Int32 mDisplay,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        if(mService == NULL)
        {
            mService = GetService();
        }
        return  mService->GetDisplayOutputType(mDisplay, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayOutputFormat(
    /* [in] */ Int32 mDisplay,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        if(mService == NULL)
        {
            mService = GetService();
        }
        return  mService->GetDisplayOutputFormat(mDisplay, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayWidth(
    /* [in] */ Int32 mDisplay,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplayWidth(mDisplay, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayHeight(
    /* [in] */ Int32 mDisplay,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplayHeight(mDisplay, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayPixelFormat(
    /* [in] */ Int32 mDisplay,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplayPixelFormat(mDisplay, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::SetDisplayParameter(
    /* [in] */ Int32 mDisplay,
    /* [in] */ Int32 param0,
    /* [in] */ Int32 param1,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->SetDisplayParameter(mDisplay, param0, param1, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::SetDisplayMode(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        Int32 r;
        SetDisplayBacklightMode(1, &r);
        return  mService->SetDisplayMode(mode, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayMode(
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplayMode(result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::SetDisplayOutputType(
    /* [in] */ Int32 mDisplay,
    /* [in] */ Int32 type,
    /* [in] */ Int32 format,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->SetDisplayOutputType(mDisplay, type, format, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::OpenDisplay(
    /* [in] */ Int32 mDisplay,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->OpenDisplay(mDisplay, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::CloseDisplay(
    /* [in] */ Int32 mDisplay,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->CloseDisplay(mDisplay, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::SetDisplayBacklightMode(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->SetDisplayBacklightMode(mode, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::SetDisplayMaster(
    /* [in] */ Int32 mDisplay,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->SetDisplayMaster(mDisplay, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayMaster(
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplayMaster(result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetMaxWidthDisplay(
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetMaxWidthDisplay(result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetMaxHdmiMode(
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetMaxHdmiMode(result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayBacklightMode(
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplayBacklightMode(result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::IsSupportHdmiMode(
    /* [in] */ Int32 mode,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->IsSupportHdmiMode(mode, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::IsSupport3DMode(
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->IsSupport3DMode(result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetHdmiHotPlugStatus(
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetHdmiHotPlugStatus(result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetTvHotPlugStatus(
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetTvHotPlugStatus(result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::SetDisplayAreaPercent(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 percent,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->SetDisplayAreaPercent(displayno, percent, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayAreaPercent(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplayAreaPercent(displayno, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::SetDisplayBright(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 bright,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->SetDisplayBright(displayno, bright, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayBright(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplayBright(displayno, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::SetDisplayContrast(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 contrast,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->SetDisplayContrast(displayno, contrast, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayContrast(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplayContrast(displayno, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::SetDisplaySaturation(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 saturation,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->SetDisplaySaturation(displayno, saturation, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplaySaturation(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplaySaturation(displayno, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::SetDisplayHue(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 hue,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->SetDisplayHue(displayno, hue, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::GetDisplayHue(
    /* [in] */ Int32 displayno,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->GetDisplayHue(displayno, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

ECode CDisplayManagerAw::Set3DLayerOffset(
    /* [in] */ Int32 displayno,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right,
    /* [out] */ Int32* result)
{
    *result = -1;
    // try
    // {
        return  mService->Set3DLayerOffset(displayno, left, right, result);
    // }
    // catch (RemoteException ex)
    // {
    //     return -1;
    // }
}

AutoPtr<IIDisplayManagerAw> CDisplayManagerAw::GetService()
{
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> sTemp;
    sm->GetService(String("display_aw"), (IInterface**)&sTemp);
    return (IIDisplayManagerAw*)(sTemp->Probe(EIID_IIDisplayManagerAw));
}

}// namespace Elastos
}// namespace Droid
}// namespace View