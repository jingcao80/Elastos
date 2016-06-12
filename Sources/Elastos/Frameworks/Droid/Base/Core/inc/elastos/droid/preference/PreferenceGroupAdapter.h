
#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEGROUPADAPTER_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEGROUPADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Drawable:: IDrawable;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Core::IComparable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceGroupAdapter
    : public BaseAdapter
    , public IPreferenceGroupAdapter
    , public IPreferenceOnPreferenceChangeInternalListener
{
private:
    class PreferenceLayout
        : public Object
        , public IComparable
        , public IPreferenceLayout
    {
    public:
        CAR_INTERFACE_DECL()

        PreferenceLayout();

        //@Override
        CARAPI CompareTo(
            /* [in]  */ IInterface* other,
            /* [out]  */ Int32* result);

        CARAPI GetResId(
            /* [out]  */ Int32* resId);

        CARAPI SetResId(
            /* [in]  */ Int32 resId);

        CARAPI GetWidgetResId(
            /* [out]  */ Int32* resId);

        CARAPI SetWidgetResId(
            /* [in]  */ Int32 resId);

        CARAPI GetName(
            /* [out]  */ String* name);

        CARAPI SetName(
            /* [in]  */ const String & name);

    public:
        Int32 mResId;
        Int32 mWidgetResId;
        String mName;
    };

    class SyncRunnable
        : public Runnable
    {
    public:
        SyncRunnable(
            /* [in]  */ PreferenceGroupAdapter* host);

        CARAPI Run();

    private:
        PreferenceGroupAdapter* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    PreferenceGroupAdapter();

    CARAPI constructor(
        /* [in] */ IPreferenceGroup* preferenceGroup);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IPreference** item);

    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    /**
     * @hide
     */
    CARAPI SetHighlighted(
        /* [in] */ Int32 position);

    /**
     * @hide
     */
    CARAPI SetHighlightedDrawable(
        /* [in] */ IDrawable* drawable);

    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    CARAPI IsEnabled(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* enabled);

    CARAPI AreAllItemsEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference);

    CARAPI OnPreferenceHierarchyChange(
        /* [in] */ IPreference* preference);

    CARAPI HasStableIds(
        /* [out] */ Boolean* hasStableIds);

    CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [out] */ Int32* type);

    CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

private:
    CARAPI GetHighlightItemViewType(
        /* [out] */ Int32* type);

    CARAPI_(void) SyncMyPreferences();

    CARAPI_(void) FlattenPreferenceGroup(
        /* [in] */ IList* preferences,
        /* [in] */ IPreferenceGroup* group);
    /**
     * Creates a string that includes the preference name, layout id and widget layout id.
     * If a particular preference type uses 2 different resources, they will be treated as
     * different view types.
     */
    CARAPI_(AutoPtr<IPreferenceLayout>) CreatePreferenceLayout(
        /* [in] */ IPreference* preference,
        /* [in] */ IPreferenceLayout* in);

    CARAPI_(void) AddPreferenceClassName(
        /* [in] */ IPreference* preference);

private:
    const static String TAG;

    /**
     * The group that we are providing data from.
     */
    // context usually holds adapter, we use weak-reference here.
    AutoPtr<IWeakReference> mWeakPreferenceGroup;
    // AutoPtr<IPreferenceGroup> mPreferenceGroup;

    /**
     * Maps a position into this adapter -> {@link Preference}. These
     * {@link Preference}s don't have to be direct children of this
     * {@link PreferenceGroup}, they can be grand children or younger)
     */
    AutoPtr<IList> mPreferenceList;

    /**
     * List of unique Preference and its subclasses' names. This is used to find
     * out how many types of views this adapter can return. Once the count is
     * returned, this cannot be modified (since the ListView only checks the
     * count once--when the adapter is being set). We will not recycle views for
     * Preference subclasses seen after the count has been returned.
     */
    AutoPtr<IList> mPreferenceLayouts;

    AutoPtr<IPreferenceLayout> mTempPreferenceLayout;

    /**
     * Blocks the mPreferenceClassNames from being changed anymore.
     */
    Boolean mHasReturnedViewTypeCount;

    Boolean mIsSyncing;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IRunnable> mSyncRunnable;
    Object mLock;

    Int32 mHighlightedPosition;
    AutoPtr<IDrawable> mHighlightedDrawable;

    static AutoPtr<IViewGroupLayoutParams> sWrapperLayoutParams;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_PREFERENCEGROUPADAPTER_H__
