
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::ElRefBase;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IRadioGroup;
using Elastos::Droid::Widget::IRadioGroupOnCheckedChangeListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace TextViewTest {

class CActivityOne : public Activity
{
public:
    class MyListener
        : public ElRefBase
        , public IRadioGroupOnCheckedChangeListener
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

        CARAPI OnCheckedChanged(
            /* [in] */ IRadioGroup* group,
            /* [in] */ Int32 checkedId);

    private:
        CActivityOne* mHost;
        Int32 mRef;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

private:
    AutoPtr<ITextView> mTextView;
    AutoPtr<IRadioGroup> mRadioGroupLanguage;
    AutoPtr<IRadioGroup> mRadioGroupGravity;

    AutoPtr<IResources> resources;
};

} // namespace ImageViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
