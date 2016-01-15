
#include "CActivityOne.h"
#include "R.h"
#include <stdio.h>
#include <utils/SystemClock.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CStringWrapper;
using Elastos::Core::CObjectContainer;
using Elastos::Droid::AppWidget::CAppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::CAppWidgetManagerHelper;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemLongClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AppwidgetTest {


CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnTouchListener*)this;
    }
    else if (riid == EIID_IViewOnTouchListener) {
        return (IViewOnTouchListener*)this;
    }
    else if (riid == EIID_IViewOnKeyListener) {
        return (IViewOnKeyListener*)this;
    }
    else if (riid == EIID_IAdapterViewOnItemClickListener) {
        return (IAdapterViewOnItemClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnTouchListener*)this) {
        *pIID = EIID_IRunnable;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnTouch(
    /* [in] */ IView* view,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    PRINT_FILE_LINE_EX("OnTouch");

    return NOERROR;
}

ECode CActivityOne::MyListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    if (mHost->mSelectedView.Get() == view) {
        PRINT_FILE_LINE_EX("OnItemClick [the same view]");
        return NOERROR;
    }

    mHost->mSelectedView = view;

    assert(mHost->m_Providers != NULL);
    AutoPtr<IObjectEnumerator> ator;
    mHost->m_Providers->GetObjectEnumerator((IObjectEnumerator**)&ator);
    assert(ator != NULL);
    Boolean hasNext = FALSE;
    AutoPtr<IComponentName> provider;
    while (ator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> data;
        ator->Current((IInterface**)&data);
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(data);
        assert(info != NULL);

        info->GetProvider((IComponentName**)&provider);
    }

    assert(provider != NULL);

    //add
    Int32 appWidgetId = 0;
    mHost->m_appWidgetHost->AllocateAppWidgetId(&appWidgetId);
printf("File=[%s], Func=[%s], line=[%d], appWidgetId=[%d]. \n", __FILE__, __FUNCTION__, __LINE__, appWidgetId);
    mHost->m_appwidgetmgr->BindAppWidgetId(appWidgetId, provider);
    AutoPtr<IAppWidgetProviderInfo> appWidget;
    mHost->m_appwidgetmgr->GetAppWidgetInfo(appWidgetId, (IAppWidgetProviderInfo**)&appWidget);
    assert(appWidget != NULL);
    AutoPtr<IAppWidgetHostView> hostView = mHost->GetHostView(appWidgetId, appWidget);

    if (hostView != NULL) {
        hostView->SetVisibility(IView::VISIBLE);
        Int32 count = 0;
        mHost->m_appContainer->GetChildCount(&count);
        if (count > 0) {
            mHost->m_appContainer->RemoveAllViews();
        }
        mHost->m_appContainer->AddView(hostView);
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnItemLongClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id,
            /* [out] */ Boolean* result)
{
    PRINT_FILE_LINE_EX("OnItemLongClick position = %d, id = %lld", position, id);
    *result = TRUE;
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
        /* [in] */ IView* view)
{
    PRINT_ENTER_LEAVE("CActivityOne::MyListener::OnClick");

    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    PRINT_ENTER_LEAVE("CActivityOne::OnCreate");

    // Setup ContentView
    //
    SetContentView(R::layout::main);

    AutoPtr<IView> temp;
    // Setup ListView
    //
    temp = FindViewById(R::id::ListView01);
    mListView = IListView::Probe(temp);
    assert(mListView != NULL);

    //Appwidget
    temp = FindViewById(R::id::appContainer);
    m_appContainer = ILinearLayout::Probe(temp);
    assert(m_appContainer != NULL);

    CAppWidgetHost::New(this, HOST_ID, (IAppWidgetHost**)&m_appWidgetHost);
printf("File=[%s], Func=[%s], line=[%d]. \n", __FILE__, __FUNCTION__, __LINE__);
    m_appWidgetHost->StartListening();
printf("File=[%s], Func=[%s], line=[%d]. \n", __FILE__, __FUNCTION__, __LINE__);
    AutoPtr<IAppWidgetManagerHelper> helper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
    assert(helper != NULL);

    helper->GetInstance(this, (IAppWidgetManager**)&m_appwidgetmgr);
    assert(m_appwidgetmgr != NULL);

    CObjectContainer::New((IObjectContainer**)&m_Providers);
    m_appwidgetmgr->GetInstalledProviders((IObjectContainer**)&m_Providers);
    assert(m_Providers != NULL);
    Int32 count = 0;
    m_Providers->GetObjectCount(&count);
printf("File=[%s], Func=[%s], line=[%d], count=[%d]. \n", __FILE__, __FUNCTION__, __LINE__, count);

    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);

    AutoPtr<IObjectEnumerator> ator;
    m_Providers->GetObjectEnumerator((IObjectEnumerator**)&ator);
    assert(ator != NULL);
    Boolean hasNext = FALSE;
    while (ator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> data;
        ator->Current((IInterface**)&data);
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(data);
        assert(info != NULL);

        String label;
        info->GetLabel(&label);
printf("File=[%s], Func=[%s], line=[%d], label=[%s]. \n", __FILE__, __FUNCTION__, __LINE__, label.string());
        StringBuilder sb(label);
        AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        dataList->Add(seq->Probe(EIID_IInterface));
    }

    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(this, R::layout::listitem, dataList, (IArrayAdapter**)&adapter);
    assert(adapter != NULL);
    mListView->SetAdapter(adapter.Get());

    AutoPtr<IColorDrawable> drawable;
    CColorDrawable::New(0xFF0000FF, (IColorDrawable**)&drawable);
    assert(drawable != NULL);
    mListView->SetDivider(drawable);
    mListView->SetDividerHeight(1);

    // Create Listener
    //
    AutoPtr<MyListener> l = new MyListener(this);
    IAdapterViewOnItemClickListener* itemClickListener = (IAdapterViewOnItemClickListener*)(l->Probe(EIID_IAdapterViewOnItemClickListener));
    mListView->SetOnItemClickListener(itemClickListener);
    mListView->SetOnItemLongClickListener(l);

    return NOERROR;
}

AutoPtr<IAppWidgetHostView> CActivityOne::GetHostView(
    /* [in] */ Int32 appwidgetId,
    /* [in] */ IAppWidgetProviderInfo* appwidgetInfo)
{
printf("File=[%s], Func=[%s], line=[%d]. \n", __FILE__, __FUNCTION__, __LINE__);
    AutoPtr<IAppWidgetHostView> hostView;
    m_appWidgetHost->CreateView((IContext*)this->Probe(EIID_IContext), appwidgetId,
            appwidgetInfo, (IAppWidgetHostView**)&hostView);
printf("File=[%s], Func=[%s], line=[%d]. \n", __FILE__, __FUNCTION__, __LINE__);
    assert(hostView != NULL);
    hostView->SetAppWidget(appwidgetId, appwidgetInfo);
printf("File=[%s], Func=[%s], line=[%d]. \n", __FILE__, __FUNCTION__, __LINE__);
    return hostView;
}

ECode CActivityOne::OnStart()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnStart");
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnResume");
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnPause");
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnStop");
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnDestroy");
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace AppwidgetTest
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
