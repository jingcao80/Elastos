
#ifndef __ELASTOS_DROID_WIDGET_SHAREACTIONPROVIDER_H__
#define __ELASTOS_DROID_WIDGET_SHAREACTIONPROVIDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/view/ActionProvider.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ISubMenu;
using Elastos::Droid::View::ActionProvider;
using Elastos::Droid::View::IOnMenuItemClickListener;
using Elastos::Droid::Widget::IOnChooseActivityListener;
using Elastos::Droid::Widget::IActivityChooserModel;
using Elastos::Droid::Widget::IOnShareTargetSelectedListener;

namespace Elastos {
namespace Droid {
namespace Widget {

class ShareActionProvider
    : public ActionProvider
    , public IShareActionProvider
{
private:
    class ShareMenuItemOnMenuItemClickListener
        : public Object
        , public IOnMenuItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ShareMenuItemOnMenuItemClickListener(
            /* [in] */ ShareActionProvider* host);

        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* isConsumed);

    private:
        ShareActionProvider* mHost;
    };

    class ShareActivityChooserModelPolicy
        : public Object
        , public IOnChooseActivityListener
    {
    public:
        CAR_INTERFACE_DECL()

        ShareActivityChooserModelPolicy(
            /* [in] */ ShareActionProvider* host);

        CARAPI OnChooseActivity(
            /* [in] */  IActivityChooserModel* host,
            /* [in] */  IIntent* intent,
            /* [out] */  Boolean* res);

    private:
        ShareActionProvider* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ShareActionProvider();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetOnShareTargetSelectedListener(
        /* [in] */ IOnShareTargetSelectedListener* listener);

    CARAPI OnCreateActionView(
        /* [out] */ IView** view);

    CARAPI HasSubMenu(
        /* [out] */ Boolean* res);

    CARAPI OnPrepareSubMenu(
        /* [in] */ ISubMenu* subMenu);

    CARAPI SetShareHistoryFileName(
        /* [in] */ const String& shareHistoryFile);

    CARAPI SetShareIntent(
        /* [in] */ IIntent* shareIntent);

private:
    CARAPI_(void) SetActivityChooserPolicyIfNeeded();

public:
    /**
     * The default name for storing share history.
     */
    static const String DEFAULT_SHARE_HISTORY_FILE_NAME;

private:
    /**
     * The default for the maximal number of activities shown in the sub-menu.
     */
    static const Int32 DEFAULT_INITIAL_ACTIVITY_COUNT;

    /**
     * The the maximum number activities shown in the sub-menu.
     */
    Int32 mMaxShownActivityCount;

    /**
     * Listener for handling menu item clicks.
     */
    AutoPtr<ShareMenuItemOnMenuItemClickListener> mOnMenuItemClickListener;

    /**
     * Context for accessing resources.
     */
    AutoPtr<IContext> mContext;

    /**
     * The name of the file with share history data.
     */
    String mShareHistoryFileName;

    AutoPtr<IOnShareTargetSelectedListener> mOnShareTargetSelectedListener;

    AutoPtr<IOnChooseActivityListener> mOnChooseActivityListener;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_SHAREACTIONPROVIDER_H__
