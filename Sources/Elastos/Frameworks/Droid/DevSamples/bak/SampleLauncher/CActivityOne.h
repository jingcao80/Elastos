
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SampleLauncher {

class CActivityOne : public Activity
{
public:
    class MyListener
        : public ElRefBase
        , public IAdapterViewOnItemClickListener
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

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        AutoPtr<CActivityOne> mHost;
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
    CARAPI Reboot();

private:
    AutoPtr<IListView> mListView;
    List<AutoPtr<IResolveInfo> > mApps;
};

} // namespace SampleLauncher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
