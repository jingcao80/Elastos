
#include "CActivityTwo.h"
#include "R.h"
#include <elautoptr.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::View::EIID_IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Interacting {

CActivityTwo::MyListener::MyListener(
    /* [in] */ CActivityTwo* host) :
    mHost(host),
    mRef(0)
{
}

PInterface CActivityTwo::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return this;
    }

    return NULL;
}

UInt32 CActivityTwo::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityTwo::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityTwo::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityTwo::MyListener::OnClick(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode CActivityTwo::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main2);

    return NOERROR;
}

ECode CActivityTwo::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityTwo::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityTwo::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityTwo::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityTwo::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityTwo::OnBackPressed()
{
    AutoPtr<IIntent> data;
    CIntent::New((IIntent**)&data);
    data->PutExtra(String("Interacting.at.key1"), String("elastos"));
    data->PutExtra(String("Interacting.at.key2"), String("elastos.org"));

    SetResult(10, data);
    return Activity::OnBackPressed();
}

} // namespace Interacting
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
