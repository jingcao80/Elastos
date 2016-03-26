
#include "CActivityOne.h"
#include "R.h"
#include <elautoptr.h>

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Sharing {

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host),
    mRef(0)
{
}

PInterface CActivityOne::MyListener::Probe(
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

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);
    if (id == R::id::sharingsimpledata) {
        AutoPtr<IIntent> sendIntent;
        CIntent::New((IIntent**)&sendIntent);
        sendIntent->SetAction(IIntent::ACTION_SEND);
        sendIntent->PutExtra(IIntent::EXTRA_TEXT, String("This is my text to send from Sharing app."));
        sendIntent->SetType(String("text/plain"));
        mHost->StartActivity(sendIntent);
    }
    else if (id == R::id::sharingfiles) {

    }
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));

    mSharingSimpleData = FindViewById(R::id::sharingsimpledata);
    assert(mSharingSimpleData != NULL);
    mSharingSimpleData->SetOnClickListener(clickListener);

    mSharingFiles = FindViewById(R::id::sharingfiles);
    assert(mSharingFiles != NULL);
    mSharingFiles->SetOnClickListener(clickListener);

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

ECode CActivityOne::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    // Inflate the menu items for use in the action bar
    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::action_menu, menu);
    return Activity::OnCreateOptionsMenu(menu, allowToShow);
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace Sharing
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
