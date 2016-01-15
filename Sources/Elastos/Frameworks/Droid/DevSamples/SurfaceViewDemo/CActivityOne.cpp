#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
//#include "Uri.h"
#include <stdio.h>

using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;
using Elastos::Droid::View::CSurfaceView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SurfaceViewDemo {


CActivityOne::CMySurfaceHolderCallback::CMySurfaceHolderCallback(
    /* [in] */ CActivityOne* host) :
    mHost(host)
{
}

PInterface CActivityOne::CMySurfaceHolderCallback::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ISurfaceHolderCallback*)this;
    }
    else if (riid == EIID_ISurfaceHolderCallback) {
        return (ISurfaceHolderCallback*)this;
    }

    return NULL;
}

UInt32 CActivityOne::CMySurfaceHolderCallback::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::CMySurfaceHolderCallback::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::CMySurfaceHolderCallback::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(ISurfaceHolderCallback*)this) {
        *pIID = EIID_ISurfaceHolderCallback;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

//In SurfaceCreated function, you could create a thread to draw your image.
ECode CActivityOne::CMySurfaceHolderCallback::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    PRINT_ENTER_LEAVE("CMySurfaceHolderCallback::SurfaceCreated");
    AutoPtr<ICanvas> canvas;
    holder->LockCanvas(NULL, (ICanvas**)&canvas);
    assert(canvas != NULL);

    Int32 canvasWidth, canvasHeight;
    canvas->GetWidth(&canvasWidth);
    canvas->GetWidth(&canvasHeight);
    PRINT_FILE_LINE_EX(" Canvas size %d:%d", canvasWidth, canvasHeight);
    holder->UnlockCanvasAndPost(canvas);
    return NOERROR;
}

ECode CActivityOne::CMySurfaceHolderCallback::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    PRINT_FILE_LINE_EX("CMySurfaceHolderCallback::SurfaceChanged %d:%d, format: %d", width, height, format);

    AutoPtr<ICanvas> canvas;
    holder->LockCanvas(NULL, (ICanvas**)&canvas);
    assert(canvas != NULL);

    Int32 canvasWidth, canvasHeight;
    canvas->GetWidth(&canvasWidth);
    canvas->GetWidth(&canvasHeight);
    PRINT_FILE_LINE_EX(" Canvas size %d:%d", canvasWidth, canvasHeight);
    canvas->DrawColor(0xFF008080);

    AutoPtr<IPaint> mPaint;
    CPaint::New((IPaint**)&mPaint);
    mPaint->SetColor(0xFFFF0000);

    Int32 left = 50;
    Int32 top = 60;
    Int32 baseW = 40;
    Int32 baseH = 20;
    Int32 interval = 60;
    Int32 extW = 80;

    left = top = 0;

    AutoPtr<IRectF> rectf;
    CRectF::New(left, top, left + baseW, top + 180, (IRectF**)&rectf);
    //Draw  |
    canvas->DrawRect(rectf, mPaint);

    baseW += 5;
    //Draw ￣
    rectf->Set(left + baseW, top, left + baseW + extW, top + baseH);
    canvas->DrawRect(rectf, mPaint);

    //Draw －
    rectf->Set(left + baseW, top + interval + baseH, left + baseW + extW, top + interval + baseH * 2);
    canvas->DrawRect(rectf, mPaint);

    //Draw _
    rectf->Set(left + baseW, top + interval * 2 + baseH * 2, left + baseW + extW, top + interval * 2 + baseH * 3);
    canvas->DrawRect(rectf, mPaint);

//    rectf->Set(0, 0, baseW, baseW);
//    canvas->DrawRect(rectf, mPaint);
//
//    rectf->Set(width - baseW, height - baseW, width, height);
//    canvas->DrawRect(rectf, mPaint);

    holder->UnlockCanvasAndPost(canvas);

    return NOERROR;
}

ECode CActivityOne::CMySurfaceHolderCallback::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    PRINT_FILE_LINE_EX("CMySurfaceHolderCallback::SurfaceDestroyed");
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    PRINT_ENTER_LEAVE("CActivityOne::OnCreate");
    //SetContentView(0x7f030000);
    ECode ec = CSurfaceView::New((IContext*)this->Probe(EIID_IContext),
            (ISurfaceView**)&mSurfaceView);
    assert(mSurfaceView != NULL);

    mSurfaceView->GetHolder((ISurfaceHolder**)&mSurfaceHolder);
    assert(mSurfaceHolder != NULL);
    mSurfaceHolderCallback = new CMySurfaceHolderCallback(this);

    assert(mSurfaceHolderCallback != NULL);
    mSurfaceHolder->AddCallback(mSurfaceHolderCallback);

    return SetContentView(mSurfaceView.Get());
}

ECode CActivityOne::OnStart()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnStart");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnResume");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnPause");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    PRINT_ENTER_LEAVE("OnStop");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    PRINT_ENTER_LEAVE("OnDestroy");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    PRINT_ENTER_LEAVE("OnActivityResult");
    return NOERROR;
}

} // namespace SurfaceViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
