
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SpinnerDemo {

class CActivityOne;

class MyListener
    : public ElRefBase
    , public IAdapterViewOnItemSelectedListener
    , public IViewOnClickListener
{
public:
    MyListener(
        /* [in] */ CActivityOne* host);

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI OnNothingSelected(
        /* [in] */ IAdapterView* parent);

    CARAPI OnClick(
        /* [in] */ IView* v);

private:
    CActivityOne* mHost;
    Int32 mRef;
};

class CActivityOne : public Activity
{
protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    friend class MyListener;

    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI ShowToast();
};

} // namespace SpinnerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
