
#include "CActivityOne.h"
#include "R.h"
#include <stdio.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CStringWrapper;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::EIID_ILayoutInflater;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IActionBarLayoutParams;
using Elastos::Droid::App::CActionBarLayoutParams;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::CPoint;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SearchViewDemo {

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ISearchViewOnQueryTextListener*)this;
    }
    else if (riid == EIID_ISearchViewOnQueryTextListener) {
        return (ISearchViewOnQueryTextListener*)this;
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

    if (pObject == (IInterface*)(ISearchViewOnQueryTextListener*)this) {
        *pIID = EIID_ISearchViewOnQueryTextListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnQueryTextSubmit(
    /* [in] */ const String& query,
    /* [out] */ Boolean* res)
{
    *res = FALSE;
    return NOERROR;
}

ECode CActivityOne::MyListener::OnQueryTextChange(
    /* [in] */ const String& newText,
    /* [out] */ Boolean* res)
{
    if (newText.IsNullOrEmpty()) {
        // Clear the text filter.
        SLOGGERD(String("CActivityOne::MyListener"), "ClearTextFilter");
        mHost->mList->ClearTextFilter();
    } else {
        // Sets the initial value for the text filter.
        mHost->mList->SetFilterText(newText);
    }
    *res = FALSE;
    return NOERROR;
}
// #include <unistd.h>

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    AutoPtr<IInterface> service;
    GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
    AutoPtr<IWindowManager> win = IWindowManager::Probe(service);
    SLOGGERD("CActivityOne", "CActivityOne::OnCreate win = %p", win.Get())
    assert(win != NULL);
    AutoPtr<IDisplay> display;
    win->GetDefaultDisplay((IDisplay**)&display);
    AutoPtr<IPoint> size;
    CPoint::New((IPoint**)&size);
    SLOGGERD("CActivityOne", "CActivityOne::OnCreate display = %p", display.Get())
    display->GetRealSize(size.Get());

SLOGGERD("CActivityOne", "CActivityOne::OnCreate Enter SetContentView")
    SetContentView(R::layout::activity_main);
SLOGGERD("CActivityOne", "CActivityOne::OnCreate Leave SetContentView")

    InitActionbar();
    mContainer = LoadData();
    mList = IListView::Probe(FindViewById(R::id::list));
    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    CArrayAdapter::New(ctx, R::layout::simple_list_item, mContainer, (IArrayAdapter**)&mAdapter);
    mList->SetAdapter(mAdapter);
    mList->SetTextFilterEnabled(TRUE);
    AutoPtr<ISearchViewOnQueryTextListener> listener = new MyListener(this);
    mSearch->SetOnQueryTextListener(listener);
    mSearch->SetSubmitButtonEnabled(FALSE);
    return NOERROR;
}

ECode CActivityOne::InitActionbar() {
    // 自定义标题栏
    AutoPtr<IActionBar> bar;
    Boolean rst;
    RequestWindowFeature(IWindow::FEATURE_ACTION_BAR, &rst);
    GetActionBar((IActionBar**)&bar);
    bar->SetDisplayShowHomeEnabled(FALSE);
    bar->SetDisplayShowTitleEnabled(FALSE);
    bar->SetDisplayShowCustomEnabled(TRUE);
    AutoPtr<IInterface> flaterTemp;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&flaterTemp);
    AutoPtr<ILayoutInflater> mInflater = ILayoutInflater::Probe(flaterTemp);
    AutoPtr<IView> mTitleView;
    mInflater->Inflate(R::layout::custom_action_bar_layout,
            NULL, (IView**)&mTitleView);
    AutoPtr<IActionBarLayoutParams> params;
    CActionBarLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT, (IActionBarLayoutParams**)&params);
    bar->SetCustomView(mTitleView, params);
    mTitleView->FindViewById(R::id::search_view, (IView**)&mSearch);
    return NOERROR;
}

AutoPtr<IObjectContainer> CActivityOne::LoadData()
{
    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);
    String str[] = {String("aa"), String("aabs"), String("jsdfasl"), String("asfas"),
         String("asdfa"), String("sdfdf"), String("fsesf") , String("rtsg")};
    for(Int32 i = 0; i < 8; i++)
    {
        AutoPtr<ICharSequence> cs;
        CStringWrapper::New(str[i], (ICharSequence**)&cs);
        dataList->Add(cs);
    }
    return dataList;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace SearchViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
