#include "CFragmentOne.h"
#include "CFragmentTwo.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace FragmentDemo {

static const String TAG("FragmentDemo::CFragmentOne");

//=======================================================================
// CFragmentOne::MyListener
//=======================================================================

CAR_INTERFACE_IMPL(CFragmentOne::MyListener, Object, IViewOnClickListener)

CFragmentOne::MyListener::MyListener(
    /* [in] */ CFragmentOne* host)
    : mHost(host)
{
}

ECode CFragmentOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Logger::I(TAG, "MyListener::OnClick");
    AutoPtr<IFragmentManager> fm;
    mHost->GetFragmentManager((IFragmentManager**)&fm);
    AutoPtr<IFragmentTransaction> trans;
    fm->BeginTransaction((IFragmentTransaction**)&trans);

    AutoPtr<IFragment> fragment;
    CFragmentTwo::New((IFragment**)&fragment);
    Logger::I(TAG, " >> Replace");
    trans->Replace(R::id::id_content, fragment, String("TWO"));
    Logger::I(TAG, " >> AddToBackStack");
    trans->AddToBackStack(String(NULL));
    Int32 ival;
    Logger::I(TAG, " >> Commit");
    trans->Commit(&ival);
    return NOERROR;
}

//=======================================================================
// CFragmentOne
//=======================================================================
CAR_OBJECT_IMPL(CFragmentOne)

ECode CFragmentOne::constructor()
{
    return NOERROR;
}

ECode CFragmentOne::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    Logger::I(TAG, " >> OnCreateView");
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::fragment_one, container, FALSE, (IView**)&view);
    AutoPtr<IView> btnView;
    view->FindViewById(R::id::fragment_one_btn, (IView**)&btnView);

    AutoPtr<IViewOnClickListener> l = new MyListener(this);
    btnView->SetOnClickListener(l);
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace FragmentDemo
} // namespace DevSamples
} // namespace Elastos

