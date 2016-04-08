
#include "CActivityOne.h"
#include "R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::Widget::CPopupMenu;
using Elastos::Droid::Widget::EIID_IPopupMenuOnMenuItemClickListener;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace DevSamples {
namespace PopupMenuDemo {

//=====================================
// CActivityOne::MyListener
//=====================================

CAR_INTERFACE_IMPL(CActivityOne::MyListener, Object, IViewOnClickListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IPopupMenu> popup;
    CPopupMenu::New(mHost, v, (IPopupMenu**)&popup);
    AutoPtr<IMenuInflater> inflater;
    popup->GetMenuInflater((IMenuInflater**)&inflater);
    AutoPtr<IMenu> mn;
    popup->GetMenu((IMenu**)&mn);
    inflater->Inflate(R::menu::action, mn);
    AutoPtr<IPopupMenuOnMenuItemClickListener> ml = new MemuListener();
    popup->SetOnMenuItemClickListener(ml);
    popup->Show();
    return NOERROR;
}

//=====================================
// CActivityOne::MemuListener
//=====================================

CAR_INTERFACE_IMPL(CActivityOne::MemuListener, Object, IPopupMenuOnMenuItemClickListener)

ECode CActivityOne::MemuListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    Slogger::D("CActivityOne", "CActivityOne::MemuListener::OnMenuItemClick");
    return NOERROR;
}

//=====================================
// CActivityOne
//=====================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
    mButton = IButton::Probe(FindViewById(R::id::button));
    AutoPtr<IViewOnClickListener> l = new MyListener(this);
    IView::Probe(mButton)->SetOnClickListener(l);
    return NOERROR;
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

} // namespace AutoCompleteTextViewDemo
} // namespace DevSamples
} // namespace Elastos
