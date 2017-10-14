//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_APP_LAUNCHER_ACTIVITY_H__
#define __ELASTOS_DROID_APP_LAUNCHER_ACTIVITY_H__

#include "elastos/droid/app/ListActivity.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/Filter.h"

using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::Filter;
using Elastos::Droid::Widget::IFilter;
using Elastos::Droid::Widget::IFilterResults;
using Elastos::Droid::Widget::IFilterable;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Displays a list of all activities which can be performed
 * for a given intent. Launches when clicked.
 *
 */
class LauncherActivity
    : public ListActivity
    , public ILauncherActivity
{
public:
    /**
     * An item in the list
     */
    class ListItem
        : public Object
        , public ILauncherActivityListItem
    {
    public:
        CAR_INTERFACE_DECL()

        ListItem();

        virtual ~ListItem();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IPackageManager* pm,
            /* [in] */ IResolveInfo* resolveInfo,
            /* [in] */ ILauncherActivityIconResizer* resizer);

        CARAPI SetResolveInfo(
            /* [in] */ IResolveInfo* resolveInfo);

        CARAPI GetResolveInfo(
            /* [out] */ IResolveInfo** resolveInfo);

        CARAPI SetLabel(
            /* [in] */ ICharSequence* label);

        CARAPI GetLabel(
            /* [out] */ ICharSequence** label);

        CARAPI SetIcon(
            /* [in] */ IDrawable* icon);

        CARAPI GetIcon(
            /* [out] */ IDrawable** icon);

        CARAPI SetClassName(
            /* [in] */ const String& className);

        CARAPI GetClassName(
            /* [out] */ String* className);

        CARAPI SetPackageName(
            /* [in] */ const String& packageName);

        CARAPI GetPackageName(
            /* [out] */ String* packageName);

        CARAPI SetExtras(
            /* [in] */ IBundle* extras);

        CARAPI GetExtras(
            /* [out] */ IBundle** extras);

    public:
        AutoPtr<IResolveInfo> mResolveInfo;
        AutoPtr<ICharSequence> mLabel;
        AutoPtr<IDrawable> mIcon;
        String mPackageName;
        String mClassName;
        AutoPtr<IBundle> mExtras;
    };

    /**
     * Utility class to resize icons to match default icon size.
     */
    class IconResizer
        : public Object
        , public ILauncherActivityIconResizer
    {
    public:
        CAR_INTERFACE_DECL()

        IconResizer();

        virtual ~IconResizer();

        CARAPI constructor(
            /* [in] */ ILauncherActivity* host);

        /**
         * Returns a Drawable representing the thumbnail of the specified Drawable.
         * The size of the thumbnail is defined by the dimension
         * android.R.dimen.launcher_application_icon_size.
         *
         * This method is not thread-safe and should be invoked on the UI thread only.
         *
         * @param icon The icon to get a thumbnail of.
         *
         * @return A thumbnail for the specified icon or the icon itself if the
         *         thumbnail could not be created.
         */
        CARAPI CreateIconThumbnail(
            /* [in] */ IDrawable* icon,
            /* [out] */ IDrawable** thumbnail);

    private:
        // Code is borrowed from com.android.launcher.Utilities.
        Int32 mIconWidth;
        Int32 mIconHeight;

        AutoPtr<IRect> mOldBounds;
        AutoPtr<ICanvas> mCanvas;
        LauncherActivity* mHost;
    };

private:
    class CancelButtonOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        CancelButtonOnClickListener(
            /* [in] */ LauncherActivity* host);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        LauncherActivity* mHost;
    };

    class ArrayFilter;

    /**
     * Adapter which shows the set of activities that can be performed for a given intent.
     */
    class ActivityAdapter
        : public BaseAdapter
        , public IFilterable
    {
    public:
        CAR_INTERFACE_DECL()

        ActivityAdapter(
            /* [in] */ ILauncherActivityIconResizer* resizer,
            /* [in] */ LauncherActivity* host);

        CARAPI IntentForPosition(
            /* [in] */ Int32 position,
            /* [out] */ IIntent** intent);

        CARAPI ItemForPosition(
            /* [in] */ Int32 position,
            /* [out] */ ILauncherActivityListItem** item);

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out]　*/ IInterface** result);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* id);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI GetFilter(
            /* [out] */ IFilter** filter);

    private:
        void BindView(
            /* [in] */ IView* view,
            /* [in] */ ILauncherActivityListItem* item);

    protected:
        friend class ArrayFilter;

        AutoPtr<ILauncherActivityIconResizer> mIconResizer;
        AutoPtr<ILayoutInflater> mInflater;

        AutoPtr<IList> mActivitiesList;

    private:
        Object mLock;
        AutoPtr<IArrayList> mOriginalValues;

        AutoPtr<IFilter> mFilter;
        Boolean mShowIcons;
        LauncherActivity* mHost;
    };

    /**
     * An array filters constrains the content of the array adapter with a prefix. Each
     * item that does not start with the supplied prefix is removed from the list.
     */
    class ArrayFilter
        : public Filter
    {
    public:
        ArrayFilter(
            /* [in] */ ActivityAdapter* host);

    protected:
        // @Override
        CARAPI PerformFiltering(
            /* [in] */ ICharSequence* prefix,
            /* [out] */ IFilterResults** results);

        // @Override
        CARAPI PublishResults(
            /* [in] */ ICharSequence* constraint,
            /* [in] */ IFilterResults* results);
    private:
        ActivityAdapter* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    LauncherActivity();

    virtual ~LauncherActivity();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    //@Override
    CARAPI SetTitle(
        /* [in] */ Int32 titleId);

protected:
    /**
     * Override to call SetContentView() with your own content view to
     * customize the list layout.
     */
    CARAPI OnSetContentView();

    //@Override
    CARAPI OnListItemClick(
        /* [in] */ IListView* l,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    /**
     * Return the actual Intent for a specific position in our
     * {@link android.widget.ListView}.
     * @param position The item whose Intent to return
     */
    AutoPtr<IIntent> IntentForPosition(
        /* [in] */ Int32 position);

    /**
     * Return the {@link ListItem} for a specific position in our
     * {@link android.widget.ListView}.
     * @param position The item to return
     */
    AutoPtr<ILauncherActivityListItem> ItemForPosition(
        /* [in] */ Int32 position);

    /**
     * Get the base intent to use when running
     * {@link PackageManager#QueryIntentActivities(Intent, Int32)}.
     */
    AutoPtr<IIntent> GetTargetIntent();

    /**
     * Perform query on package manager for list items.  The default
     * implementation queries for activities.
     */
    AutoPtr<IList> OnQueryPackageManager(
        /* [in] */ IIntent* queryIntent);

    /**
     * @hide
     */
    CARAPI OnSortResultList(
        /* [in] */ IList* results);

    /**
     * Perform the query to determine which results to show and return a list of them.
     */
    CARAPI MakeListItems(
        /* [out] */ IList** items);

    /**
     * Whether or not to show icons in the list
     * @hide keeping this private for now, since only Settings needs it
     * @return TRUE to show icons beside the activity names, false otherwise
     */
    Boolean OnEvaluateShowIcons();


private:
    void UpdateAlertTitle();

    void UpdateButtonText();

public:
    AutoPtr<IIntent> mIntent;
    AutoPtr<IPackageManager> mPackageManager;
    AutoPtr<ILauncherActivityIconResizer> mIconResizer;
};


} // namespace App
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APP_LAUNCHER_ACTIVITY_H__
