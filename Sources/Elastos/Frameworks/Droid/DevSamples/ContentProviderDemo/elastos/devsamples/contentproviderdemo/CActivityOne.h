
#ifndef __Elastos_DevSamples_ContentProviderDemo_CActivityOne_H__
#define __Elastos_DevSamples_ContentProviderDemo_CActivityOne_H__

#include "_Elastos_DevSamples_ContentProviderDemo_CActivityOne.h"
#include <Elastos.Droid.Database.h>
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

CarClass(CActivityOne)
    , public Activity
{
public:
    class MyListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

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

    CARAPI Create();

    CARAPI Add();

    CARAPI Query();

    CARAPI Update();

    CARAPI Delete();

private:
    AutoPtr<ISQLiteOpenHelper> mDBOpenHelper;
};

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ContentProviderDemo_CActivityOne_H__
