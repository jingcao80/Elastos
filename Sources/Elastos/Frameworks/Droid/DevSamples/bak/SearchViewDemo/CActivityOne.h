
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include <elautoptr.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ISearchViewOnQueryTextListener;
using Elastos::Droid::Widget::EIID_ISearchViewOnQueryTextListener;
using Elastos::Droid::Widget::ISearchView;
using Elastos::Droid::Widget::EIID_ISearchView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::EIID_IListView;
using Elastos::Droid::Widget::IArrayAdapter;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SearchViewDemo {

class CActivityOne : public Activity
{
private:

    class MyListener
            : public ISearchViewOnQueryTextListener
            , public ElRefBase
    {
    public:

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnQueryTextSubmit(
            /* [in] */ const String& query,
            /* [out] */ Boolean* res);

        CARAPI OnQueryTextChange(
            /* [in] */ const String& newText,
            /* [out] */ Boolean* res);
    private:
        CActivityOne* mHost;
    };
public:

    CARAPI_(AutoPtr<IObjectContainer>) LoadData();

    CARAPI InitActionbar();
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

    AutoPtr<IListView> mList;
    AutoPtr<IObjectContainer> mContainer;
    AutoPtr<IArrayAdapter> mAdapter;
    AutoPtr<ISearchView> mSearch;
};

} // namespace SearchViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
