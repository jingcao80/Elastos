#include "CActivityOne.h"
#include "R.h"
#include <Elastos.Droid.Core.h>
#include <Org.Xmlpull.V1.h>
#include <unistd.h>
#include <util/Xml.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/System.h>
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Etl::List;
using Elastos::Core::System;
using Elastos::Core::CObjectContainer;
using Elastos::Core::CStringWrapper;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Widget::EIID_IViewFactory;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Droid::Widget::IRelativeLayoutLayoutParams;
using Elastos::Droid::Widget::CRelativeLayoutLayoutParams;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::CDatePicker;
using Elastos::Droid::Widget::ICalendarView;
using Elastos::Droid::Widget::CCalendarView;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;


using Elastos::Droid::Os::IDebug;

using Elastos::Droid::Os::CDebug;

using Elastos::IO::IFile;

using Elastos::IO::CFile;

using Elastos::IO::IFileDescriptor;

using Elastos::IO::CFileDescriptor;

using Libcore::IO::IOsConstants;

using Libcore::IO::COsConstants;

using Libcore::IO::ILibcore;

using Libcore::IO::CLibcore;

using Libcore::IO::IOs;

using Libcore::IO::IIoBridge;

using Libcore::IO::CIoBridge;



static void MyDumpMemery()

{

     AutoPtr<IOsConstants> osConstans;

     COsConstants::AcquireSingleton((IOsConstants**)&osConstans);

     Int32 m1, m2, m3;

     osConstans->GetOsConstant(String("O_RDWR"), &m1);

     osConstans->GetOsConstant(String("O_CREAT"), &m2);

     osConstans->GetOsConstant(String("O_TRUNC"), &m3);



     AutoPtr<IFile> file;

     CFile::New(String("/data/debug"), (IFile**)&file);

     Boolean bval;

     file->Exists(&bval);

     if (!bval) {

         file->Mkdirs(&bval);

     }



     AutoPtr<IFileDescriptor> ifd;

     CFileDescriptor::New((IFileDescriptor**)&ifd);

     AutoPtr<IIoBridge> ioBridge;

     CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);

     Int32 fd;

     ioBridge->Open(String("/data/debug/1.txt"), m1 | m2 | m3, &fd);

     ifd->SetDescriptor(fd);



     AutoPtr<IDebug> dbg;

     CDebug::AcquireSingleton((IDebug**)&dbg);

     dbg->DumpHeap(ifd);

}

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ImageButtonDemo {

Int32 CActivityOne::count = 0;
Int32 CActivityOne::index = 0;

ECode CActivityOne::OnCreate(
        /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
Slogger::D("Test", "IGallery process ID is : %d", gettid());
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
    //MyDumpMemery();
    //Test();
    /*AutoPtr<IView> clock = FindViewById(R::id::digitalClock);
    mClock = IDigitalClock::Probe(clock);*/

    /*AutoPtr<IView> num = FindViewById(R::id::NumberPicker);
    mNumber = INumberPicker::Probe(num);
    assert (mNumber);
    mNumber->SetMinValue(0);
    mNumber->SetMaxValue(12);

    AutoPtr<IView> view = FindViewById(R::id::button);
    mButton = IButton::Probe(view);

    AutoPtr<IViewOnClickListener> listerer = new ImageSwitcherListener(this);
    mButton->SetOnClickListener(listerer);*/



    return NOERROR;
}

ECode CActivityOne::Test()
{
Slogger::D("jiazhenjiang", "====================BEGIN=====================");
    AutoPtr<ICharSequence> title, text;
    AutoPtr<INotification> targetNotification;
    AutoPtr<INotificationBuilder> mNotificationBuilder;
    AutoPtr<INotificationManager> mNotificationManager;

    GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&mNotificationManager);
    CNotificationBuilder::New(IContext::Probe(this), (INotificationBuilder**)&mNotificationBuilder);

    AutoPtr<IResources> res;
    AutoPtr<IBitmap> bm;
    AutoPtr<IBitmapFactory> bf;

    CStringWrapper::New(String("notification"), (ICharSequence**)&title);
    CStringWrapper::New(String("send notification"), (ICharSequence**)&text);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    mNotificationBuilder->SetWhen(now);
    mNotificationBuilder->SetShowWhen(true);
    mNotificationBuilder->SetContentTitle(title.Get());
    mNotificationBuilder->SetContentText(text.Get());
    mNotificationBuilder->SetSmallIcon(R::drawable::test0);
    mNotificationBuilder->SetContentIntent(NULL/*mPendingIntent.Get()*/);

    GetResources((IResources**)&res);
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bf);
    bf->DecodeResource(res.Get(), R::drawable::test1, (IBitmap**)&bm);
    mNotificationBuilder->SetLargeIcon(bm.Get());

    mNotificationBuilder->Build((INotification**)&targetNotification);
    mNotificationManager->Notify(0, targetNotification.Get());
Slogger::D("jiazhenjiang", "====================END=====================");
}

ECode CActivityOne::OnStart()
{
    PEL("CActivityOne::OnStart");
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    PEL("CActivityOne::OnResume");
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    PEL("CActivityOne::OnPause");
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    PEL("CActivityOne::OnStop");
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    PEL("CActivityOne::OnDestroy");
    return NOERROR;
}

//====================================================
CActivityOne::ActivityListener::ActivityListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{

}

PInterface CActivityOne::ActivityListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ISeekBarOnSeekBarChangeListener*)this;
    } else  if (riid == EIID_ISeekBarOnSeekBarChangeListener) {
        return (ISeekBarOnSeekBarChangeListener*)this;
    } else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }
    return NULL;
}

UInt32 CActivityOne::ActivityListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::ActivityListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::ActivityListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pObject == (ISeekBarOnSeekBarChangeListener*)this) {
        *pIID = EIID_ISeekBarOnSeekBarChangeListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    PRINT_FILE_LINE_EX("OnProgressChanged progress = %d", progress);
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    PRINT_FILE_LINE_EX("CActivityOne::ActivityListener::OnStartTrackingTouch");
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    PRINT_FILE_LINE_EX("CActivityOne::ActivityListener::OnStopTrackingTouch");
    return NOERROR;
}

ECode CActivityOne::ActivityListener::OnClick(
    /* [in] */ IView* v)
{
    /*mHost->mSwitcher->SetVisibility(IView::VISIBLE);
    mHost->mImageSwitch->SetVisibility(IView::GONE);
    if (v == mHost->mPrev) {
        mHost->mSwitcher->ShowPrevious();
    } else if ( v == mHost->mNext) {
        mHost->mSwitcher->ShowNext();
    }*/
}

//============================================================
//              CActivityOne::Switcher
//============================================================

CActivityOne::Switcher::Switcher(
    /* [in] */ IContext* context)
    : mContext(context)
{

}

ECode CActivityOne::Switcher::MakeView(
    /* [out] */ IView** view)
{
    AutoPtr<IImageView> v;
    CImageView::New(mContext, (IImageView**)&v);
    v->SetBackgroundColor(IColor::WHITE);
    *view = v;
    if (*view) {
        (*view)->AddRef();
    }
}

PInterface CActivityOne::Switcher::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ISeekBarOnSeekBarChangeListener*)this;
    } else if (riid == EIID_IViewFactory) {
        return (IViewFactory*)this;
    }
    return NULL;
}

UInt32 CActivityOne::Switcher::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::Switcher::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::Switcher::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{

}

//============================================================
//              CActivityOne::ImageSwitcherListener
//============================================================

CActivityOne::ImageSwitcherListener::ImageSwitcherListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{

}

PInterface CActivityOne::ImageSwitcherListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ISeekBarOnSeekBarChangeListener*)this;
    } else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }
    return NULL;
}

UInt32 CActivityOne::ImageSwitcherListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::ImageSwitcherListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::ImageSwitcherListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{

}

ECode CActivityOne::ImageSwitcherListener::OnClick(
    /* [in] */ IView* v)
{
    if(v = mHost->mButton) {
        Slogger::D("jiazhenjiang", "-------------------Begin--------------------");

        Slogger::D("jiazhenjiang", "--------------------End-------------------");
    }
}

//============================================================
//              CActivityOne::TextSwitcherListener
//============================================================

CActivityOne::TextSwitcherListener::TextSwitcherListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{

}

PInterface CActivityOne::TextSwitcherListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ISeekBarOnSeekBarChangeListener*)this;
    } else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }
    return NULL;
}

UInt32 CActivityOne::TextSwitcherListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::TextSwitcherListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::TextSwitcherListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{

}

ECode CActivityOne::TextSwitcherListener::OnClick(
    /* [in] */ IView* v)
{
    /*mHost->mImageSwitch->SetVisibility(IView::GONE);
    mHost->mSwitcher->SetVisibility(IView::GONE);
    AutoPtr<ICharSequence> csq;
    if (v == mHost->mImagePrev) {
        CStringWrapper::New(String("jiazhenjiang========================================================"), (ICharSequence**)&csq);
        mHost->mTextSwitch->SetText(csq);
    } else if (v == mHost->mImageNext) {
        CStringWrapper::New(String("wangguangming!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!"), (ICharSequence**)&csq);
        mHost->mTextSwitch->SetText(csq);
    }*/
}

//============================================================
//     CActivityOne::ActivityOneCheckedChangeListener
//============================================================

CActivityOne::ActivityOneCheckedChangeListener::ActivityOneCheckedChangeListener()
{

}

PInterface CActivityOne::ActivityOneCheckedChangeListener::Probe(
    /* [in]  */ REIID riid)
{

}

UInt32 CActivityOne::ActivityOneCheckedChangeListener::AddRef()
{

}

UInt32 CActivityOne::ActivityOneCheckedChangeListener::Release()
{

}

ECode CActivityOne::ActivityOneCheckedChangeListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{

}

ECode CActivityOne::ActivityOneCheckedChangeListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    PFL_EX("CActivityOne::ActivityOneCheckedChangeListener::OnCheckedChanged is running");
}

} // namespace SurfaceViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
