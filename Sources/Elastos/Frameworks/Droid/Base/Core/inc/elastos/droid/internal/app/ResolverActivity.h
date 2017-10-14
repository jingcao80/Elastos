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

#ifndef __ELASTOS_DROID_INTERNAL_APP_RESOLVERACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_RESOLVERACTIVITY_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/widget/BaseAdapter.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::Usage::IUsageStatsManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Internal::Content::EIID_IPackageMonitor;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemLongClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;
using Elastos::Core::IComparator;
using Elastos::Text::ICollator;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class ECO_PUBLIC ResolverActivity
    : public Activity
    , public IResolverActivity
{
    friend class ResolveListAdapter;

public:
    class ViewHolder : public Object
    {
    public:
        ViewHolder(
            /* [in] */ IView* view);

    public:
        AutoPtr<ITextView> mText;
        AutoPtr<ITextView> mText2;
        AutoPtr<IImageView> mIcon;
    };

    class ItemLongClickListener
        : public Object
        , public IAdapterViewOnItemLongClickListener
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ItemLongClickListener(
            /* [in] */ ResolverActivity* host)
            : mHost(host)
        {}

        CARAPI OnItemLongClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id,
            /* [out] */ Boolean* clicked);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        ResolverActivity* mHost;
    };

    class LoadIconTask : public AsyncTask
    {
    public:
        LoadIconTask(
            /* [in] */ ResolverActivity* host);

    protected:
        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* info);

    private:
        ResolverActivity* mHost;
    };

    class LoadIconIntoViewTask : public AsyncTask
    {
    public:
        LoadIconIntoViewTask(
            /* [in] */ IImageView* target,
            /* [in] */ ResolverActivity* host);

    protected:
        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* info);

    public:
        AutoPtr<IImageView> mTargetView;

    private:
        ResolverActivity* mHost;
    };

    class ResolverComparator
        : public Object
        , public IComparator
    {
    public:
        ResolverComparator(
            /* [in] */ ResolverActivity* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        CARAPI_(Int64) GetPackageTimeSpent(
            /* [in] */ const String& packageName);

    private:
        AutoPtr<ICollator> mCollator;
        ResolverActivity* mHost;
    };

private:
    class _PackageMonitor
        : public PackageMonitor
    {
    public:
        _PackageMonitor(
            /* [in] */ ResolverActivity* host)
            : mHost(host)
        {}

        CARAPI OnSomePackagesChanged();

    private:
        ResolverActivity* mHost;
    };

    class ActionTitle : public Object
    {
    public:
        ActionTitle(
            /* [in] */ const String& action,
            /* [in] */ Int32 titleRes,
            /* [in] */ Int32 namedTitleRes);

        static CARAPI_(AutoPtr<ActionTitle>) ForAction(
            /* [in] */ const String& action);

    private:
        static CARAPI_(AutoPtr<ArrayOf<ActionTitle*> >) InitArray();

    public:
        static const AutoPtr<ActionTitle> VIEW;
        static const AutoPtr<ActionTitle> EDIT;
        static const AutoPtr<ActionTitle> SEND;
        static const AutoPtr<ActionTitle> SENDTO;
        static const AutoPtr<ActionTitle> SEND_MULTIPLE;
        static const AutoPtr<ActionTitle> DEFAULT;
        static const AutoPtr<ActionTitle> HOME;

    public:
        const String mAction;
        const Int32 mTitleRes;
        const Int32 mNamedTitleRes;

    private:
        static const AutoPtr<ArrayOf<ActionTitle*> > sArray;
    };

    class ViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        ViewOnClickListener(
            /* [in] */ ResolverActivity* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        ResolverActivity* mHost;
    };

    class DisplayResolveInfo : public Object
    {
    public:
        DisplayResolveInfo(
            /* [in] */ IResolveInfo* pri,
            /* [in] */ ICharSequence* pLabel,
            /* [in] */ ICharSequence* pInfo,
            /* [in] */ IIntent* pOrigIntent)
            : mRi(pri)
            , mDisplayLabel(pLabel)
            , mExtendedInfo(pInfo)
            , mOrigIntent(pOrigIntent)
        {}

    public:
        AutoPtr<IResolveInfo> mRi;
        AutoPtr<ICharSequence> mDisplayLabel;
        AutoPtr<IDrawable> mDisplayIcon;
        AutoPtr<ICharSequence> mExtendedInfo;
        AutoPtr<IIntent> mOrigIntent;
    };

    class ResolveListAdapter
        : public BaseAdapter
    {
    public:
        friend class ResolverActivity;

        ResolveListAdapter(
            /* [in] */ ResolverActivity* host,
            /* [in] */ IIntent* intent,
            /* [in] */ ArrayOf<IIntent*>* initialIntents,
            /* [in] */ IList* rList,
            /* [in] */ Int32 launchedFromUid,
            /* [in] */ Boolean filterLastUsed);

        CARAPI HandlePackagesChanged();

        CARAPI_(AutoPtr<DisplayResolveInfo>) GetFilteredItem();

        CARAPI_(Int32) GetFilteredPosition();

        CARAPI_(Boolean) HasFilteredItem();

        CARAPI_(AutoPtr<IResolveInfo>) ResolveInfoForPosition(
            /* [in] */ Int32 position,
            /* [in] */ Boolean filtered);

        CARAPI_(AutoPtr<IIntent>) IntentForPosition(
            /* [in] */ Int32 position,
            /* [in] */ Boolean filtered);

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* id);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** bkView);

    private:
        CARAPI_(void) RebuildList();

        CARAPI_(void) ProcessGroup(
            /* [in] */ IList* rList,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ IResolveInfo* ro,
            /* [in] */ ICharSequence* roLabel);

        CARAPI_(void) BindView(
            /* [in] */ IView* view,
            /* [in] */ DisplayResolveInfo* info);

    public:
        List<AutoPtr<DisplayResolveInfo> > mList;
        AutoPtr<IList> mOrigResolveList;

    private:
        ResolverActivity* mHost;
        AutoPtr<ArrayOf<IIntent*> > mInitialIntents;
        AutoPtr<IList> mBaseResolveList;
        AutoPtr<IResolveInfo> mLastChosen;
        AutoPtr<IIntent> mIntent;
        Int32 mLaunchedFromUid;
        AutoPtr<ILayoutInflater> mInflater;

        Int32 mLastChosenPosition;
        Boolean mFilterLastUsed;
    };

    class FilteredItemContainerOnLongCliskListener
        : public Object
        , public IViewOnLongClickListener
    {
    public:
        FilteredItemContainerOnLongCliskListener(
            /* [in] */ ResolverActivity* host,
            /* [in] */ ResolveListAdapter* adapter);

        CAR_INTERFACE_DECL()

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        ResolverActivity* mHost;
        ResolveListAdapter* mAdapter;
    };

public:
    ResolverActivity();

    CAR_INTERFACE_DECL()

    /**
     * Turn on launch mode that is safe to use when forwarding intents received from
     * applications and running in system processes.  This mode uses Activity.startActivityAsCaller
     * instead of the normal Activity.startActivity for launching the activity selected
     * by the user.
     *
     * <p>This mode is set to true by default if the activity is initialized through
     * {@link #onCreate(android.os.Bundle)}.  If a subclass calls one of the other onCreate
     * methods, it is set to false by default.  You must set it before calling one of the
     * more detailed onCreate methods, so that it will be set correctly in the case where
     * there is only one intent to resolve and it is thus started immediately.</p>
     */
    CARAPI SetSafeForwardingMode(
        /* [in] */ Boolean safeForwarding);

    virtual CARAPI Dismiss();

    CARAPI GetIcon(
        /* [in] */ IResources* res,
        /* [in] */ Int32 resId,
        /* [out] */ IDrawable** drawable);

    CARAPI LoadIconForResolveInfo(
        /* [in] */ IResolveInfo* ri,
        /* [out] */ IDrawable** drawable);

    virtual CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI OnButtonClick(
        /* [in] */ IView* v);

    CARAPI StartSelected(
        /* [in] */ Int32 which,
        /* [in] */ Boolean always,
        /* [in] */ Boolean filtered);

    /**
     * Replace me in subclasses!
     */
    CARAPI GetReplacementIntent(
        /* [in] */ const String& packageName,
        /* [in] */ IIntent* defIntent,
        /* [out] */ IIntent** outIntent);

    CARAPI SafelyStartActivity(
        /* [in] */ IIntent* intent);

    CARAPI ShowAppDetails(
        /* [in] */ IResolveInfo* ri);

protected:
    using Activity::OnCreate;

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    virtual CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState,
        /* [in] */ IIntent* intent,
        /* [in] */ ICharSequence* title,
        /* [in] */ ArrayOf<IIntent*>* initialIntents,
        /* [in] */ IList* rList,
        /* [in] */ Boolean alwaysUseOption);

    virtual CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState,
        /* [in] */ IIntent* intent,
        /* [in] */ ICharSequence* title,
        /* [in] */ Int32 defaultTitleRes,
        /* [in] */ ArrayOf<IIntent*>* initialIntents,
        /* [in] */ IList* rList,
        /* [in] */ Boolean alwaysUseOption);

    CARAPI_(AutoPtr<ICharSequence>) GetTitleForAction(
        /* [in] */ const String& action,
        /* [in] */ Int32 defaultTitleRes);

    CARAPI OnRestart();

    CARAPI OnStop();

    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnIntentSelected(
        /* [in] */ IResolveInfo* ri,
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean alwaysCheck);

private:
    CARAPI_(AutoPtr<IIntent>) MakeMyIntent();


    CARAPI_(Boolean) HasManagedProfile();

    CARAPI_(Boolean) SupportsManagedProfiles(
        /* [in] */ IResolveInfo* resolveInfo);

    CARAPI_(Boolean) VersionNumberAtLeastL(
        /* [in] */ Int32 versionNumber);

    CARAPI_(void) SetAlwaysButtonEnabled(
        /* [in] */ Boolean hasValidSelection,
        /* [in] */ Int32 checkedPos,
        /* [in] */ Boolean filtered);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Int64 USAGE_STATS_PERIOD;

    Int32 mLaunchedFromUid;
    AutoPtr<ResolveListAdapter> mAdapter;
    AutoPtr<IPackageManager> mPm;
    Boolean mSafeForwardingMode;
    Boolean mAlwaysUseOption;
    Boolean mShowExtended;
    AutoPtr<IListView> mListView;
    AutoPtr<IViewGroup> mFilteredItemContainer;
    AutoPtr<IButton> mAlwaysButton;
    AutoPtr<IButton> mOnceButton;
    Int32 mIconDpi;
    Int32 mIconSize;
    Int32 mMaxColumns;
    Int32 mLastSelected;
    Boolean mResolvingHome;

    AutoPtr<IUsageStatsManager> mUsm;
    AutoPtr<IMap> mStats;

    Boolean mRegistered;

    AutoPtr<_PackageMonitor> mPackageMonitor;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::Internal::App::ResolverActivity::ActionTitle*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_INTERNAL_APP_RESOLVERACTIVITY_H__
