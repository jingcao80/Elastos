
#include "CLayoutChangesActivity.h"
#include <R.h>
#include "R.h"
#include <elastos/core/Math.h>

using Elastos::Core::Math;
using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

const String CLayoutChangesActivity::COUNTRIES[] = {
            String("Belgium"), String("France"), String("Italy"), String("Germany"), String("Spain"),
            String("Austria"), String("Russia"), String("Poland"), String("Croatia"), String("Greece"),
            String("Ukraine")
    };

CLayoutChangesActivity::MyListener::MyListener(
    /* [in] */ CLayoutChangesActivity* host,
    /* [in] */ IView* view)
    : mHost(host)
    , mView(view)
{
}

PInterface CLayoutChangesActivity::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }

    return NULL;
}

UInt32 CLayoutChangesActivity::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CLayoutChangesActivity::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CLayoutChangesActivity::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CLayoutChangesActivity::MyListener::OnClick(
        /* [in] */ IView* newView)
{
    // Remove the row from its parent (the container view).
    // Because mContainerView has android:animateLayoutChanges set to true,
    // this removal is automatically animated.
    IViewManager::Probe(mHost->mContainerView)->RemoveView(mView);

    Int32 count = 0;
    mHost->mContainerView->GetChildCount(&count);
    // If there are no rows remaining, show the empty view.
    if (count == 0) {
        AutoPtr<IView> view;
        mHost->FindViewById(Elastos::Droid::R::id::empty, (IView**)&view);
        view->SetVisibility(IView::VISIBLE);
    }
    return NOERROR;
}

ECode CLayoutChangesActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_layout_changes);

    mContainerView = IViewGroup::Probe(FindViewById(R::id::container));
    return NOERROR;
}

ECode CLayoutChangesActivity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Activity::OnCreateOptionsMenu(menu, res);
    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::activity_layout_changes, menu);
    *res = TRUE;
    return NOERROR;
}

ECode CLayoutChangesActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 id = 0;
    item->GetItemId(&id);
    switch (id) {
        case Elastos::Droid::R::id::home: {
            // Navigate "up" the demo structure to the launchpad activity.
            // See http://developer.android.com/design/patterns/navigation.html for more.
//                NavUtils.navigateUpTo(this, new Intent(this, MainActivity.class));
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(String("AnimationsDemo"), String("AnimationsDemo.CMainActivity"));
            StartActivity(intent);
            Finish();
            *res = TRUE;
            return NOERROR;
        }
        case R::id::action_add_item: {
            // Hide the "empty" view since there is now at least one item in the list.
            FindViewById(Elastos::Droid::R::id::empty)->SetVisibility(IView::GONE);
            AddItem();
            *res = TRUE;
            return NOERROR;
        }
    }

    return Activity::OnOptionsItemSelected(item, res);
}

void CLayoutChangesActivity::AddItem()
{
    // Instantiate a new "row" view.
    AutoPtr<ILayoutInflater> inflater;
    GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inflater);

    AutoPtr<IViewGroup> newView;
    inflater->Inflate(R::layout::list_item_example, mContainerView, FALSE, (IView**)&newView);

    // Set the text in the new row to a random country.
    AutoPtr<ITextView> txtView;
    newView->FindViewById(Elastos::Droid::R::id::text1, (IView**)&txtView);

    AutoPtr<ICharSequence> text;
    CStringWrapper::New(COUNTRIES[(Int32) (Elastos::Core::Math::Random() * 11/*COUNTRIES.length*/)], (ICharSequence**)&text);
    txtView->SetText(text);

    AutoPtr<IView> delbtn;
    newView->FindViewById(R::id::delete_button, (IView**)&delbtn);
    // Set a click listener for the "X" button in the row that will remove the row.
    AutoPtr<MyListener> l = new MyListener(this, newView);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));
    delbtn->SetOnClickListener(clickListener);

    // Because mContainerView has android:animateLayoutChanges set to true,
    // adding this view is automatically animated.
    mContainerView->AddView(newView, 0);
}


} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
