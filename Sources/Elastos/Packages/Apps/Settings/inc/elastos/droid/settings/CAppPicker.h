#ifndef __ELASTOS_DROID_SETTINGS_CAPPPICKER_H__
#define __ELASTOS_DROID_SETTINGS_CAPPPICKER_H__

#include "_Elastos_Droid_Settings_CAppPicker.h"
#include "elastos/droid/app/ListActivity.h"
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::App::ListActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::IComparator;
using Elastos::Text::ICollator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CAppPicker), public ListActivity
{
public:
    class AppListAdapter
        : public ArrayAdapter
    {
    public:
        TO_STRING_IMPL("CAppPicker::AppListAdapter")

        AppListAdapter(
            /* [in] */ CAppPicker* host);

        CARAPI constructor(
            /* [in] */ IContext* context);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* _convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

    private:
        CAppPicker* mHost;

        // AutoPtr<IList<MyApplicationInfo> mPackageInfoList = new ArrayList<MyApplicationInfo>();
        AutoPtr<IList> mPackageInfoList;
        AutoPtr<ILayoutInflater> mInflater;
    };

protected:
    class MyApplicationInfo
        : public Object
    {
    public:
        TO_STRING_IMPL("CAppPicker::MyApplicationInfo")

        AutoPtr<IApplicationInfo> mInfo;
        AutoPtr<ICharSequence> mLabel;
    };

private:
    class MyComparator
        : public Object
        , public IComparator
    {
    public:
        TO_STRING_IMPL("CAppPicker::MyComparator")

        CAR_INTERFACE_DECL()

        MyComparator();

        CARAPI Compare(
            /* [in] */ IInterface* left,
            /* [in] */ IInterface* right,
            /* [out] */ Int32* result);

    private:
        AutoPtr<ICollator> mCollator;
    };

public:
    CAR_OBJECT_DECL()

    CAppPicker();

    virtual ~CAppPicker();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnStop();

    //@Override
    CARAPI OnListItemClick(
        /* [in] */ IListView* l,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

private:
    AutoPtr<AppListAdapter> mAdapter;

    static AutoPtr<IComparator> sDisplayNameComparator;

};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CAPPPICKER_H__