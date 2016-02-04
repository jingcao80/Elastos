
#include <Elastos.CoreLibrary.Text.h>
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/internal/app/ResolverActivity.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/widget/ListView.h"
#include "elastos/droid/widget/Toast.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::Usage::IUsageStats;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilterAuthorityEntry;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::ILabeledIntent;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Widget::IResolverDrawerLayout;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemLongClickListener;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::ListView;
using Elastos::Droid::Widget::Toast;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Text::CCollatorHelper;
using Elastos::Text::ICollatorHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

const String ResolverActivity::TAG("ResolverActivity");
const Boolean ResolverActivity::DEBUG = FALSE;
const Int64 ResolverActivity::USAGE_STATS_PERIOD = 1000 * 60 * 60 * 24 * 14;

ECode ResolverActivity::_PackageMonitor::OnSomePackagesChanged()
{
    return mHost->mAdapter->HandlePackagesChanged();
}

const AutoPtr<ResolverActivity::ActionTitle> ResolverActivity::ActionTitle::VIEW
    = new ResolverActivity::ActionTitle(IIntent::ACTION_VIEW,
    R::string::whichViewApplication, R::string::whichViewApplicationNamed);

const AutoPtr<ResolverActivity::ActionTitle> ResolverActivity::ActionTitle::EDIT
    = new ResolverActivity::ActionTitle(IIntent::ACTION_EDIT,
    R::string::whichEditApplication, R::string::whichEditApplicationNamed);

const AutoPtr<ResolverActivity::ActionTitle> ResolverActivity::ActionTitle::SEND
    = new ResolverActivity::ActionTitle(IIntent::ACTION_SEND,
    R::string::whichSendApplication, R::string::whichSendApplicationNamed);

const AutoPtr<ResolverActivity::ActionTitle> ResolverActivity::ActionTitle::SENDTO
    = new ResolverActivity::ActionTitle(IIntent::ACTION_SENDTO,
    R::string::whichSendApplication, R::string::whichSendApplicationNamed);

const AutoPtr<ResolverActivity::ActionTitle> ResolverActivity::ActionTitle::SEND_MULTIPLE
    = new ResolverActivity::ActionTitle(IIntent::ACTION_SEND_MULTIPLE,
    R::string::whichSendApplication, R::string::whichSendApplicationNamed);

const AutoPtr<ResolverActivity::ActionTitle> ResolverActivity::ActionTitle::DEFAULT
    = new ResolverActivity::ActionTitle(String(NULL),
    R::string::whichApplication, R::string::whichApplicationNamed);

const AutoPtr<ResolverActivity::ActionTitle> ResolverActivity::ActionTitle::HOME
    = new ResolverActivity::ActionTitle(IIntent::ACTION_MAIN,
    R::string::whichHomeApplication, R::string::whichHomeApplicationNamed);

const AutoPtr<ArrayOf<ResolverActivity::ActionTitle*> > ResolverActivity::ActionTitle::sArray =
    ResolverActivity::ActionTitle::InitArray();

ResolverActivity::ActionTitle::ActionTitle(
    /* [in] */ const String& action,
    /* [in] */ Int32 titleRes,
    /* [in] */ Int32 namedTitleRes)
    : mAction(action)
    , mTitleRes(titleRes)
    , mNamedTitleRes(namedTitleRes)
{
}

AutoPtr<ResolverActivity::ActionTitle> ResolverActivity::ActionTitle::ForAction(
    /* [in] */ const String& action)
{
    for (Int32 i = 0; i < sArray->GetLength(); i++) {
        AutoPtr<ActionTitle> title = (*sArray)[i];
        if (title != HOME && action != NULL && action.Equals(title->mAction)) {
            return title;
        }
    }
    return DEFAULT;
}

AutoPtr<ArrayOf<ResolverActivity::ActionTitle*> > ResolverActivity::ActionTitle::InitArray()
{
    AutoPtr<ArrayOf<ActionTitle*> > array = ArrayOf<ActionTitle*>::Alloc(7);
    array->Set(0, VIEW);
    array->Set(1, EDIT);
    array->Set(2, SEND);
    array->Set(3, SENDTO);
    array->Set(4, SEND_MULTIPLE);
    array->Set(5, DEFAULT);
    array->Set(6, HOME);
    return array;
}

CAR_INTERFACE_IMPL(ResolverActivity::ViewOnClickListener, Object, IViewOnClickListener)

ResolverActivity::ViewOnClickListener::ViewOnClickListener(
    /* [in] */ ResolverActivity* host)
    : mHost(host)
{}

ECode ResolverActivity::ViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->Finish();
}

ResolverActivity::ResolveListAdapter::ResolveListAdapter(
    /* [in] */ ResolverActivity* host,
    /* [in] */ IIntent* intent,
    /* [in] */ ArrayOf<IIntent*>* initialIntents,
    /* [in] */ IList* rList,
    /* [in] */ Int32 launchedFromUid,
    /* [in] */ Boolean filterLastUsed)
    : mHost(host)
    , mLastChosenPosition(-1)
    , mFilterLastUsed(FALSE)
{
    CIntent::New(intent, (IIntent**)&mIntent);
    mInitialIntents = initialIntents;
    mBaseResolveList = rList;
    mLaunchedFromUid = launchedFromUid;
    LayoutInflater::From(host, (ILayoutInflater**)&mInflater);
    mFilterLastUsed = filterLastUsed;
    RebuildList();
}

ECode ResolverActivity::ResolveListAdapter::HandlePackagesChanged()
{
    Int32 oldItemCount;
    GetCount(&oldItemCount);
    RebuildList();
    NotifyDataSetChanged();
    Int32 newItemCount;
    GetCount(&newItemCount);
    if (newItemCount == 0) {
        // We no longer have any items...  just finish the activity.
        mHost->Finish();
    }
    return NOERROR;
}

AutoPtr<ResolverActivity::DisplayResolveInfo> ResolverActivity::ResolveListAdapter::GetFilteredItem()
{
    if (mFilterLastUsed && mLastChosenPosition >= 0) {
        // Not using getItem since it offsets to dodge this position for the list
        return mList[mLastChosenPosition];
    }
    return NULL;
}

Int32 ResolverActivity::ResolveListAdapter::GetFilteredPosition()
{
    if (mFilterLastUsed && mLastChosenPosition >= 0) {
        return mLastChosenPosition;
    }
    return IAdapterView::INVALID_POSITION;
}

Boolean ResolverActivity::ResolveListAdapter::HasFilteredItem()
{
    return mFilterLastUsed && mLastChosenPosition >= 0;
}

void ResolverActivity::ResolveListAdapter::RebuildList()
{
    AutoPtr<IList> currentResolveList;

    AutoPtr<IContentResolver> resolver;
    mHost->GetContentResolver((IContentResolver**)&resolver);
    String type;
    mIntent->ResolveTypeIfNeeded(resolver, &type);
    if (FAILED(AppGlobals::GetPackageManager()->GetLastChosenActivity(
        mIntent, type, IPackageManager::MATCH_DEFAULT_ONLY, (IResolveInfo**)&mLastChosen))) {
        Logger::D(TAG, "Error calling GetLastChosenActivity\n");
    }

    mList.Clear();
    if (mBaseResolveList != NULL) {
        currentResolveList = mOrigResolveList = mBaseResolveList;
    }
    else {
        mHost->mPm->QueryIntentActivities(
                mIntent, IPackageManager::MATCH_DEFAULT_ONLY
                | (mFilterLastUsed ? IPackageManager::GET_RESOLVED_FILTER : 0),
                (IList**)&currentResolveList);
        mOrigResolveList = currentResolveList;
        // Filter out any activities that the launched uid does not
        // have permission for.  We don't do this when we have an explicit
        // list of resolved activities, because that only happens when
        // we are being subclassed, so we can safely launch whatever
        // they gave us.
        if (currentResolveList != NULL) {
            String permission;
            Int32 uid, granted;
            Boolean exported;
            Int32 size;
            currentResolveList->GetSize(&size);
            for (Int32 i = size - 1; i >= 0; i--) {
                AutoPtr<IInterface> item;
                currentResolveList->Get(i, (IInterface**)&item);
                AutoPtr<IActivityInfo> ai;
                IResolveInfo::Probe(item)->GetActivityInfo((IActivityInfo**)&ai);
                ai->GetPermission(&permission);
                AutoPtr<IApplicationInfo> appInfo;
                IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                appInfo->GetUid(&uid);
                IComponentInfo::Probe(ai)->GetExported(&exported);
                granted = CActivityManager::CheckComponentPermission(
                    permission, mLaunchedFromUid, uid, exported);
                if (granted != IPackageManager::PERMISSION_GRANTED) {
                    // Access not allowed!
                    if (mOrigResolveList == currentResolveList) {
                        mOrigResolveList = NULL;
                        CArrayList::New(ICollection::Probe(currentResolveList), (IList**)&mOrigResolveList);
                    }
                    currentResolveList->Remove(i);
                }
            }
        }
    }

    Int32 N;
    if ((currentResolveList != NULL) && (currentResolveList->GetSize(&N), N) > 0) {
        // Only display the first matches that are either of equal
        // priority or have asked to be default options.
        AutoPtr<IInterface> item0;
        currentResolveList->Get(0, (IInterface**)&item0);
        AutoPtr<IResolveInfo> r0 = IResolveInfo::Probe(item0);
        for (Int32 i = 1; i < N; i++) {
            AutoPtr<IInterface> item;
            currentResolveList->Get(i, (IInterface**)&item);
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(item);

            AutoPtr<IActivityInfo> r0ActivityInfo;
            r0->GetActivityInfo((IActivityInfo**)&r0ActivityInfo);
            String r0Name;
            IPackageItemInfo::Probe(r0ActivityInfo)->GetName(&r0Name);
            Int32 r0Priority;
            r0->GetPriority(&r0Priority);
            Boolean r0IsDefault;
            r0->GetIsDefault(&r0IsDefault);

            AutoPtr<IActivityInfo> riActivityInfo;
            ri->GetActivityInfo((IActivityInfo**)&riActivityInfo);
            String riName;
            IPackageItemInfo::Probe(riActivityInfo)->GetName(&riName);
            Int32 riPriority;
            ri->GetPriority(&riPriority);
            Boolean riIsDefault;
            ri->GetIsDefault(&riIsDefault);

            if (DEBUG)
                Logger::V(TAG, "%s=%d/%d vs %s=%d/%d", r0Name.string(), r0Priority, r0IsDefault,
                    riName.string(), riPriority, riIsDefault);

            if (r0Priority != riPriority || r0IsDefault != riIsDefault) {
                while (i < N) {
                    if (mOrigResolveList == currentResolveList) {
                        mOrigResolveList = NULL;
                        CArrayList::New(ICollection::Probe(currentResolveList), (IList**)&mOrigResolveList);
                    }
                    currentResolveList->Remove(i);
                    N--;
                }
            }
        }

        if (N > 1) {
            AutoPtr<IComparator> rComparator = new ResolverComparator(mHost);
            AutoPtr<ICollections> collections;
            CCollections::AcquireSingleton((ICollections**)&collections);
            collections->Sort(currentResolveList, rComparator);
        }

        // First put the initial items at the top.
        if (mInitialIntents != NULL) {
            for (Int32 i = 0; i < mInitialIntents->GetLength(); i++) {
                AutoPtr<IIntent> ii = (*mInitialIntents)[i];
                if (ii == NULL) {
                    continue;
                }
                AutoPtr<IPackageManager> pkgMgr;
                mHost->GetPackageManager((IPackageManager**)&pkgMgr);
                AutoPtr<IActivityInfo> ai;
                ii->ResolveActivityInfo(pkgMgr, 0, (IActivityInfo**)&ai);
                if (ai == NULL) {
                    String str;
                    IObject::Probe(ii)->ToString(&str);
                    Slogger::W(TAG, "No activity found for %p", str.string());
                    continue;
                }
                AutoPtr<IResolveInfo> ri;
                CResolveInfo::New((IResolveInfo**)&ri);
                ri->SetActivityInfo(ai);
                if (ILabeledIntent::Probe(ii)) {
                    AutoPtr<ILabeledIntent> li = ILabeledIntent::Probe(ii);
                    String sourcePkg;
                    li->GetSourcePackage(&sourcePkg);
                    ri->SetResolvePackageName(sourcePkg);
                    Int32 labelRes;
                    li->GetLabelResource(&labelRes);
                    ri->SetLabelRes(labelRes);
                    AutoPtr<ICharSequence> label;
                    li->GetNonLocalizedLabel((ICharSequence**)&label);
                    ri->SetNonLocalizedLabel(label);
                    Int32 iconRes;
                    li->GetIconResource(&iconRes);
                    ri->SetIcon(iconRes);
                }
                AutoPtr<ICharSequence> label;
                IPackageItemInfo::Probe(ri)->LoadLabel(pkgMgr, (ICharSequence**)&label);
                AutoPtr<DisplayResolveInfo> info = new DisplayResolveInfo(ri, label, NULL, ii);
                mList.PushBack(info);
            }
        }

        // Check for applications with same name and use application name or
        // package name if necessary
        item0 = NULL;
        currentResolveList->Get(0, (IInterface**)&item0);
        r0 = IResolveInfo::Probe(item0);
        Int32 start = 0;
        AutoPtr<ICharSequence> r0Label;
        IPackageItemInfo::Probe(r0)->LoadLabel(mHost->mPm, (ICharSequence**)&r0Label);
        mHost->mShowExtended = FALSE;
        for (Int32 i = 1; i < N; i++) {
            String r0pkgName, ripkgName;
            if (r0Label == NULL) {
                AutoPtr<IActivityInfo> aInfo;
                r0->GetActivityInfo((IActivityInfo**)&aInfo);
                IPackageItemInfo::Probe(aInfo)->GetPackageName(&r0pkgName);
                r0Label = CoreUtils::Convert(r0pkgName);
            }
            AutoPtr<IInterface> item;
            currentResolveList->Get(i, (IInterface**)&item);
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(item);
            AutoPtr<ICharSequence> riLabel;
            IPackageItemInfo::Probe(ri)->LoadLabel(mHost->mPm, (ICharSequence**)&riLabel);
            if (riLabel == NULL) {
                AutoPtr<IActivityInfo> aInfo;
                ri->GetActivityInfo((IActivityInfo**)&aInfo);
                IPackageItemInfo::Probe(aInfo)->GetPackageName(&ripkgName);
                riLabel = CoreUtils::Convert(ripkgName);
            }
            if (ripkgName.Equals(r0pkgName)) {
                continue;
            }
            ProcessGroup(currentResolveList, start, (i-1), r0, r0Label);
            r0 = ri;
            r0Label = riLabel;
            start = i;
            i++;
        }
        // Process last group
        ProcessGroup(currentResolveList, start, (N-1), r0, r0Label);
    }
}

void ResolverActivity::ResolveListAdapter::ProcessGroup(
    /* [in] */ IList* rList,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IResolveInfo* ro,
    /* [in] */ ICharSequence* roLabel)
{
    // Process labels from start to i
    Int32 num = end - start+1;
    if (num == 1) {
        if (mLastChosen != NULL) {
            AutoPtr<IActivityInfo> aInfo;
            mLastChosen->GetActivityInfo((IActivityInfo**)&aInfo);
            String lpkgName, lName;
            IPackageItemInfo::Probe(aInfo)->GetPackageName(&lpkgName);
            IPackageItemInfo::Probe(aInfo)->GetName(&lName);
            aInfo = NULL;
            ro->GetActivityInfo((IActivityInfo**)&aInfo);
            String rpkgName, rName;
            IPackageItemInfo::Probe(aInfo)->GetPackageName(&rpkgName);
            IPackageItemInfo::Probe(aInfo)->GetName(&rName);
            if (lpkgName.Equals(rpkgName) && lName.Equals(rName))
                mLastChosenPosition = mList.GetSize();
        }
        // No duplicate labels. Use label for entry at start
        AutoPtr<DisplayResolveInfo> info = new DisplayResolveInfo(ro, roLabel, NULL, NULL);
        mList.PushBack(info);
    }
    else {
        mHost->mShowExtended = TRUE;
        Boolean usePkg = FALSE;
        AutoPtr<IActivityInfo> aInfo;
        ro->GetActivityInfo((IActivityInfo**)&aInfo);
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        AutoPtr<ICharSequence> startApp;
        IPackageItemInfo::Probe(appInfo)->LoadLabel(mHost->mPm, (ICharSequence**)&startApp);
        if (startApp == NULL) {
            usePkg = TRUE;
        }
        if (!usePkg) {
            // Use HashSet to track duplicates
            HashSet<AutoPtr<IInterface> > duplicates;
            duplicates.Insert(startApp);
            for (Int32 j = start + 1; j <= end ; j++) {
                AutoPtr<IInterface> item;
                rList->Get(j, (IInterface**)&item);
                AutoPtr<IResolveInfo> jRi = IResolveInfo::Probe(item);
                AutoPtr<IActivityInfo> aInfo;
                jRi->GetActivityInfo((IActivityInfo**)&aInfo);
                AutoPtr<IApplicationInfo> appInfo;
                IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                AutoPtr<ICharSequence> jApp;
                IPackageItemInfo::Probe(appInfo)->LoadLabel(mHost->mPm, (ICharSequence**)&jApp);
                HashSet<AutoPtr<IInterface> >::Iterator hit = duplicates.Find(jApp);
                if ((jApp == NULL) || hit != duplicates.End()) {
                    usePkg = TRUE;
                    break;
                }
                else {
                    duplicates.Insert(jApp);
                }
            }
            // Clear HashSet for later use
            duplicates.Clear();
        }
        for (Int32 k = start; k <= end; k++) {
            AutoPtr<IInterface> item;
            rList->Get(k, (IInterface**)&item);
            AutoPtr<IResolveInfo> add = IResolveInfo::Probe(item);
            if (mLastChosen != NULL) {
                AutoPtr<IActivityInfo> aInfo;
                mLastChosen->GetActivityInfo((IActivityInfo**)&aInfo);
                String lpkgName, lName;
                IPackageItemInfo::Probe(aInfo)->GetPackageName(&lpkgName);
                IPackageItemInfo::Probe(aInfo)->GetName(&lName);
                aInfo = NULL;
                add->GetActivityInfo((IActivityInfo**)&aInfo);
                String rpkgName, rName;
                IPackageItemInfo::Probe(aInfo)->GetPackageName(&rpkgName);
                IPackageItemInfo::Probe(aInfo)->GetName(&rName);
                if (lpkgName.Equals(rpkgName) && lName.Equals(rName))
                    mLastChosenPosition = mList.GetSize();
            }

            AutoPtr<IActivityInfo> aaInfo;
            add->GetActivityInfo((IActivityInfo**)&aaInfo);
            if (usePkg) {
                // Use application name for all entries from start to end-1
                String aname;
                IPackageItemInfo::Probe(aaInfo)->GetPackageName(&aname);
                AutoPtr<ICharSequence> cname;
                cname = CoreUtils::Convert(aname);
                AutoPtr<DisplayResolveInfo> info =
                        new DisplayResolveInfo(add, roLabel, cname, NULL);
                mList.PushBack(info);
            }
            else {
                // Use package name for all entries from start to end-1
                AutoPtr<IApplicationInfo> aappInfo;
                IComponentInfo::Probe(aaInfo)->GetApplicationInfo((IApplicationInfo**)&aappInfo);
                AutoPtr<ICharSequence> label;
                IPackageItemInfo::Probe(aappInfo)->LoadLabel(mHost->mPm, (ICharSequence**)&label);
                AutoPtr<DisplayResolveInfo> info =
                        new DisplayResolveInfo(add, roLabel, label, NULL);
                mList.PushBack(info);
            }
        }
    }
}

AutoPtr<IResolveInfo> ResolverActivity::ResolveListAdapter::ResolveInfoForPosition(
    /* [in] */ Int32 position,
    /* [in] */ Boolean filtered)
{
    AutoPtr<DisplayResolveInfo> info;
    if (filtered) {
        AutoPtr<IInterface> item;
        GetItem(position, (IInterface**)&item);
        info = (DisplayResolveInfo*)IObject::Probe(item);
    }
    else
        info = mList[position];
    return info->mRi;
}

AutoPtr<IIntent> ResolverActivity::ResolveListAdapter::IntentForPosition(
    /* [in] */ Int32 position,
    /* [in] */ Boolean filtered)
{
    AutoPtr<DisplayResolveInfo> dri;
    if (filtered) {
        AutoPtr<IInterface> item;
        GetItem(position, (IInterface**)&item);
        dri = (DisplayResolveInfo*)IObject::Probe(item);
    }
    else
        dri = mList[position];

    AutoPtr<IActivityInfo> ai;
    dri->mRi->GetActivityInfo((IActivityInfo**)&ai);
    AutoPtr<IIntent> inIntent;
    if (dri->mOrigIntent != NULL)
        inIntent = dri->mOrigIntent;
    else {
        String pkgName;
        IPackageItemInfo::Probe(ai)->GetPackageName(&pkgName);
        mHost->GetReplacementIntent(pkgName, mIntent, (IIntent**)&inIntent);
    }

    AutoPtr<IIntent> intent;
    CIntent::New(inIntent, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_FORWARD_RESULT
            |IIntent::FLAG_ACTIVITY_PREVIOUS_IS_TOP);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    String pkgName;
    IPackageItemInfo::Probe(appInfo)->GetPackageName(&pkgName);
    String name;
    IPackageItemInfo::Probe(ai)->GetName(&name);
    AutoPtr<IComponentName> component;
    CComponentName::New(pkgName, name, (IComponentName**)&component);
    intent->SetComponent(component);
    return intent;
}

ECode ResolverActivity::ResolveListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mList.GetSize();
    if (mFilterLastUsed && mLastChosenPosition >= 0) {
        (*count)--;
    }
    return NOERROR;
}

ECode ResolverActivity::ResolveListAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    if (mFilterLastUsed && mLastChosenPosition >= 0 && position >= mLastChosenPosition) {
        position++;
    }
    *item = (IObject*)mList[position].Get();
    REFCOUNT_ADD(*item);
    return NOERROR;
}

ECode ResolverActivity::ResolveListAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);

    *id = position;
    return NOERROR;
}

ECode ResolverActivity::ResolveListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** bkView)
{
    VALIDATE_NOT_NULL(bkView);
    AutoPtr<IView> view;
    if (convertView == NULL) {
        mInflater->Inflate(R::layout::resolve_list_item, parent, FALSE, (IView**)&view);

        AutoPtr<ViewHolder> holder = new ViewHolder(view);
        view->SetTag((IObject*)holder.Get());
    }
    else {
        view = convertView;
    }

    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    BindView(view, (DisplayResolveInfo*)IObject::Probe(item));
    *bkView = view;
    REFCOUNT_ADD(*bkView);
    return NOERROR;
}

void ResolverActivity::ResolveListAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ DisplayResolveInfo* info)
{
    AutoPtr<IInterface> tag;
    view->GetTag((IInterface**)&tag);
    AutoPtr<ViewHolder> holder = (ViewHolder*)IObject::Probe(tag);
    holder->mText->SetText(info->mDisplayLabel);
    if (mHost->mShowExtended) {
        IView::Probe(holder->mText2)->SetVisibility(IView::VISIBLE);
        holder->mText2->SetText(info->mExtendedInfo);
    }
    else {
        IView::Probe(holder->mText2)->SetVisibility(IView::GONE);
    }
    if (info->mDisplayIcon == NULL) {
        AutoPtr<LoadIconTask> task = new LoadIconTask(mHost);
        AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
        params->Set(0, (IObject*)info);
        task->Execute(params);
    }
    holder->mIcon->SetImageDrawable(info->mDisplayIcon);
}

ResolverActivity::ViewHolder::ViewHolder(
    /* [in] */ IView* view)
{
    AutoPtr<IView> tempView;
    view->FindViewById(R::id::text1, (IView**)&tempView);
    mText = ITextView::Probe(tempView);
    tempView = NULL;
    view->FindViewById(R::id::text2, (IView**)&tempView);
    mText2 = ITextView::Probe(tempView);
    tempView = NULL;
    view->FindViewById(R::id::icon, (IView**)&tempView);
    mIcon = IImageView::Probe(tempView);
}

CAR_INTERFACE_IMPL_2(ResolverActivity::ItemLongClickListener, Object,
    IAdapterViewOnItemLongClickListener, IAdapterViewOnItemClickListener)

ResolverActivity::ItemLongClickListener::OnItemLongClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* clicked)
{
    VALIDATE_NOT_NULL(clicked);
    Int32 hvCount = ((ListView*)mHost->mListView.Get())->GetHeaderViewsCount();
    position -= hvCount;
    if (position < 0) {
        // Header views don't count.
        *clicked = FALSE;
        return NOERROR;
    }

    AutoPtr<IResolveInfo> ri = mHost->mAdapter->ResolveInfoForPosition(position, TRUE);
    mHost->ShowAppDetails(ri);
    *clicked = TRUE;
    return NOERROR;
}

ResolverActivity::ItemLongClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemClick(parent, view, position, id);
}

ResolverActivity::LoadIconTask::LoadIconTask(
    /* [in] */ ResolverActivity* host)
    : mHost(host)
{}

ECode ResolverActivity::LoadIconTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    DisplayResolveInfo* info = (DisplayResolveInfo*)IObject::Probe((*params)[0]);
    if (info->mDisplayIcon == NULL) {
        mHost->LoadIconForResolveInfo(info->mRi, (IDrawable**)&info->mDisplayIcon);
    }
    *result = (*params)[0];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ResolverActivity::LoadIconTask::OnPostExecute(
    /* [in] */ IInterface* info)
{
    return mHost->mAdapter->NotifyDataSetChanged();
}

ResolverActivity::LoadIconIntoViewTask::LoadIconIntoViewTask(
    /* [in] */ IImageView* target,
    /* [in] */ ResolverActivity* host)
    : mTargetView(target)
    , mHost(host)
{
}

ECode ResolverActivity::LoadIconIntoViewTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    DisplayResolveInfo* info = (DisplayResolveInfo*)IObject::Probe((*params)[0]);
    if (info->mDisplayIcon == NULL) {
        mHost->LoadIconForResolveInfo(info->mRi, (IDrawable**)&info->mDisplayIcon);
    }
    *result = (*params)[0];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ResolverActivity::LoadIconIntoViewTask::OnPostExecute(
    /* [in] */ IInterface* _info)
{
    DisplayResolveInfo* info = (DisplayResolveInfo*)IObject::Probe(_info);
    return mTargetView->SetImageDrawable(info->mDisplayIcon);
}

CAR_INTERFACE_IMPL(ResolverActivity::ResolverComparator, Object, IComparator)

ResolverActivity::ResolverComparator::ResolverComparator(
    /* [in] */ ResolverActivity* host)
    : mHost(host)
{
    AutoPtr<ICollatorHelper> cHelper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&cHelper);
    AutoPtr<IResources> res;
    host->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> c;
    res->GetConfiguration((IConfiguration**)&c);
    AutoPtr<ILocale> locale;
    c->GetLocale((ILocale**)&locale);
    cHelper->GetInstance(locale, (ICollator**)&mCollator);
}

ECode ResolverActivity::ResolverComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    IResolveInfo* lhs = IResolveInfo::Probe(_lhs);
    IResolveInfo* rhs = IResolveInfo::Probe(_rhs);
    assert(lhs != NULL && rhs != NULL);

    // We want to put the one targeted to another user at the end of the dialog.
    if (((CResolveInfo*)lhs)->mTargetUserId != UserHandle::USER_CURRENT) {
        *result = 1;
        return NOERROR;
    }

    AutoPtr<IActivityInfo> lhsInfo;
    lhs->GetActivityInfo((IActivityInfo**)&lhsInfo);
    AutoPtr<IActivityInfo> rhsInfo;
    rhs->GetActivityInfo((IActivityInfo**)&rhsInfo);

    if (mHost->mStats != NULL) {
        String lhsPkgName;
        IPackageItemInfo::Probe(lhsInfo)->GetPackageName(&lhsPkgName);
        String rhsPkgName;
        IPackageItemInfo::Probe(rhsInfo)->GetPackageName(&rhsPkgName);
        Int64 timeDiff = GetPackageTimeSpent(rhsPkgName) - GetPackageTimeSpent(lhsPkgName);
        if (timeDiff != 0) {
            *result = timeDiff > 0 ? 1 : -1;
            return NOERROR;
        }
    }

    AutoPtr<ICharSequence> sa;
    IPackageItemInfo::Probe(lhs)->LoadLabel(mHost->mPm, (ICharSequence**)&sa);
    if (sa == NULL) {
        String name;
        IPackageItemInfo::Probe(lhsInfo)->GetName(&name);
        sa = CoreUtils::Convert(name);
    }
    AutoPtr<ICharSequence>  sb;
    IPackageItemInfo::Probe(rhs)->LoadLabel(mHost->mPm, (ICharSequence**)&sa);
    if (sb == NULL) {
        String name;
        IPackageItemInfo::Probe(lhsInfo)->GetName(&name);
        sb = CoreUtils::Convert(name);
    }

    String aStr, bStr;
    sa->ToString(&aStr);
    sb->ToString(&bStr);
    return mCollator->Compare(aStr, bStr, result);
}

Int64 ResolverActivity::ResolverComparator::GetPackageTimeSpent(
    /* [in] */ const String& packageName)
{
    if (mHost->mStats != NULL) {
        AutoPtr<IInterface> value;
        mHost->mStats->Get(CoreUtils::Convert(packageName), (IInterface**)&value);
        AutoPtr<IUsageStats> stats = IUsageStats::Probe(value);
        if (stats != NULL) {
            Int64 t;
            stats->GetTotalTimeInForeground(&t);
            return t;
        }

    }
    return 0;
}

CAR_INTERFACE_IMPL_2(ResolverActivity, Activity, IResolverActivity, IAdapterViewOnItemClickListener)

ResolverActivity::ResolverActivity()
    : mLaunchedFromUid(0)
    , mAdapter(NULL)
    , mSafeForwardingMode(FALSE)
    , mAlwaysUseOption(FALSE)
    , mShowExtended(FALSE)
    , mIconDpi(0)
    , mIconSize(0)
    , mMaxColumns(0)
    , mLastSelected(IAdapterView::INVALID_POSITION)
    , mResolvingHome(FALSE)
    , mRegistered(FALSE)
{
    mPackageMonitor = new _PackageMonitor(this);
}

AutoPtr<IIntent> ResolverActivity::MakeMyIntent()
{
    AutoPtr<IIntent> intent;
    AutoPtr<IIntent> inIntent;
    GetIntent((IIntent**)&inIntent);
    CIntent::New(inIntent, (IIntent**)&intent);
    intent->SetComponent(NULL);
    // The resolver activity is set to be hidden from recent tasks.
    // we don't want this attribute to be propagated to the next activity
    // being launched.  Note that if the original Intent also had this
    // flag set, we are now losing it.  That should be a very rare case
    // and we can live with this.
    Int32 flags;
    intent->GetFlags(&flags);
    intent->SetFlags(flags&~IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    return intent;
}

ECode ResolverActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    // Use a specialized prompt when we're handling the 'Home' app startActivity()
    AutoPtr<IIntent> intent = MakeMyIntent();
    AutoPtr<ArrayOf<String> > categories;
    intent->GetCategories((ArrayOf<String>**)&categories);
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_MAIN.Equals(action) && categories != NULL
        && categories->GetLength() == 1
        && ArrayUtils::Contains(categories.Get(), IIntent::CATEGORY_HOME)) {
        // Note: this field is not set to true in the compatibility version.
        mResolvingHome = TRUE;
    }

    SetSafeForwardingMode(TRUE);

    return OnCreate(savedInstanceState, intent, NULL, 0, NULL, NULL, TRUE);
}

ECode ResolverActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState,
    /* [in] */ IIntent* intent,
    /* [in] */ ICharSequence* title,
    /* [in] */ ArrayOf<IIntent*>* initialIntents,
    /* [in] */ IList* rList,
    /* [in] */ Boolean alwaysUseOption)
{
    return OnCreate(savedInstanceState, intent, title, 0, initialIntents, rList, alwaysUseOption);
}

ECode ResolverActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState,
    /* [in] */ IIntent* intent,
    /* [in] */ ICharSequence* title,
    /* [in] */ Int32 defaultTitleRes,
    /* [in] */ ArrayOf<IIntent*>* initialIntents,
    /* [in] */ IList* rList,
    /* [in] */ Boolean alwaysUseOption)
{
    SetTheme(R::style::Theme_DeviceDefault_Resolver);
    Activity::OnCreate(savedInstanceState);

    AutoPtr<IBinder> binder;
    GetActivityToken((IBinder**)&binder);
    if (FAILED(ActivityManagerNative::GetDefault()->GetLaunchedFromUid(binder, &mLaunchedFromUid)))
        mLaunchedFromUid = -1;

    GetPackageManager((IPackageManager**)&mPm);
    AutoPtr<IInterface> service;
    GetSystemService(IContext::USAGE_STATS_SERVICE, (IInterface**)&service);
    mUsm = IUsageStatsManager::Probe(service);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 t;
    system->GetCurrentTimeMillis(&t);
    const Int64 sinceTime = t - USAGE_STATS_PERIOD;
    mUsm->QueryAndAggregateUsageStats(sinceTime, (system->GetCurrentTimeMillis(&t), t), (IMap**)&mStats);
    Logger::D(TAG, "sinceTime=%lld", sinceTime);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetInteger(R::integer::config_maxResolverActivityColumns, &mMaxColumns);

    AutoPtr<ILooper> looper;
    GetMainLooper((ILooper**)&looper);
    mPackageMonitor->Register(this, looper, FALSE);
    mRegistered = TRUE;

    service = NULL;
    GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&service);
    AutoPtr<IActivityManager> am = IActivityManager::Probe(service);
    assert(am != NULL);
    am->GetLauncherLargeIconDensity(&mIconDpi);
    am->GetLauncherLargeIconSize(&mIconSize);

    mAdapter = new ResolveListAdapter(this, intent, initialIntents, rList, mLaunchedFromUid, alwaysUseOption);

    Int32 layoutId;
    Boolean useHeader;
    Boolean hasFilteredItem = mAdapter->HasFilteredItem();
    if (hasFilteredItem) {
        layoutId = R::layout::resolver_list_with_default;
        alwaysUseOption = FALSE;
        useHeader = TRUE;
    }
    else {
        useHeader = FALSE;
        layoutId = R::layout::resolver_list;
    }
    mAlwaysUseOption = alwaysUseOption;


    Int32 count = mAdapter->mList.GetSize();
    if (mLaunchedFromUid < 0 || UserHandle::IsIsolated(mLaunchedFromUid)) {
        // Gulp!
        Finish();
        return NOERROR;
    }
    else if (count > 1) {
        SetContentView(layoutId);
        AutoPtr<IView> listView;
        FindViewById(R::id::resolver_list, (IView**)&listView);
        AutoPtr<IAdapterView> adapterView = IAdapterView::Probe(listView);
        mListView = IListView::Probe(listView);
        adapterView->SetAdapter(mAdapter);
        AutoPtr<ItemLongClickListener> listener = new ItemLongClickListener(this);
        adapterView->SetOnItemClickListener(listener);
        adapterView->SetOnItemLongClickListener(listener);

        if (alwaysUseOption) {
            IAbsListView::Probe(listView)->SetChoiceMode(IAbsListView::CHOICE_MODE_SINGLE);
        }

        if (useHeader) {
            AutoPtr<ILayoutInflater> lInflater;
            LayoutInflater::From(this, (ILayoutInflater**)&lInflater);
            AutoPtr<IView> view;
            lInflater->Inflate(R::layout::resolver_different_item_header,
                IViewGroup::Probe(mListView), FALSE, (IView**)&view);
            mListView->AddHeaderView(view);
        }
    }
    else if (count == 1) {
        AutoPtr<IIntent> intent = mAdapter->IntentForPosition(0, FALSE);
        SafelyStartActivity(intent);
        mPackageMonitor->Unregister();
        mRegistered = FALSE;
        Finish();
        return NOERROR;
    }
    else {
        SetContentView(R::layout::resolver_list);
        AutoPtr<IView> empty;
        FindViewById(R::id::empty, (IView**)&empty);
        empty->SetVisibility(IView::VISIBLE);

        AutoPtr<IView> listView;
        FindViewById(R::id::resolver_list, (IView**)&listView);
        mListView = IListView::Probe(listView);
        listView->SetVisibility(IView::GONE);
    }

    AutoPtr<IView> view;
    FindViewById(R::id::contentPanel, (IView**)&view);
    AutoPtr<IResolverDrawerLayout> rdl = IResolverDrawerLayout::Probe(view);;
    if (rdl != NULL) {
        AutoPtr<ViewOnClickListener> listener = new ViewOnClickListener(this);
        rdl->SetOnClickOutsideListener(listener);
    }

    if (title == NULL) {
        String action;
        intent->GetAction(&action);
        title = GetTitleForAction(action, defaultTitleRes);
    }
    if (!TextUtils::IsEmpty(title)) {
        view = NULL;
        FindViewById(R::id::title, (IView**)&view);
        AutoPtr<ITextView> titleView = ITextView::Probe(view);
        if (titleView != NULL) {
            titleView->SetText(title);
        }
        SetTitle(title);
    }

    view = NULL;
    FindViewById(R::id::icon, (IView**)&view);
    AutoPtr<IImageView> iconView = IImageView::Probe(view);
    AutoPtr<DisplayResolveInfo> iconInfo = mAdapter->GetFilteredItem();
    if (iconView != NULL && iconInfo != NULL) {
        AutoPtr<LoadIconIntoViewTask> task = new LoadIconIntoViewTask(iconView, this);
        AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
        params->Set(0, (IObject*)iconInfo.Get());
        task->Execute(params);
    }

    if (alwaysUseOption || hasFilteredItem) {
        AutoPtr<IView> buttonLayout;
        FindViewById(R::id::button_bar, (IView**)&buttonLayout);
        if (buttonLayout != NULL) {
            buttonLayout->SetVisibility(IView::VISIBLE);
            view = NULL;
            buttonLayout->FindViewById(R::id::button_always, (IView**)&view);
            mAlwaysButton = IButton::Probe(view);
            view = NULL;
            buttonLayout->FindViewById(R::id::button_once, (IView**)&view);
            mOnceButton = IButton::Probe(view);
        }
        else {
            mAlwaysUseOption = FALSE;
        }
    }

    if (hasFilteredItem) {
        Int32 position = mAdapter->GetFilteredPosition();
        SetAlwaysButtonEnabled(TRUE, position, FALSE);
        IView::Probe(mOnceButton)->SetEnabled(TRUE);
    }

    return NOERROR;
}

ECode ResolverActivity::SetSafeForwardingMode(
    /* [in] */ Boolean safeForwarding)
{
    mSafeForwardingMode = safeForwarding;
    return NOERROR;
}

AutoPtr<ICharSequence> ResolverActivity::GetTitleForAction(
    /* [in] */ const String& action,
    /* [in] */ Int32 defaultTitleRes)
{
    AutoPtr<ActionTitle> title = mResolvingHome ? ActionTitle::HOME : ActionTitle::ForAction(action);
    Boolean named = mAdapter->HasFilteredItem();
    AutoPtr<ICharSequence> result;
    if (title == ActionTitle::DEFAULT && defaultTitleRes != 0) {
        GetText(defaultTitleRes, (ICharSequence**)&result);
    }
    else {
        if (named) {
            AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
            params->Set(0, mAdapter->GetFilteredItem()->mDisplayLabel);
            String str;
            GetString(title->mNamedTitleRes, params, &str);
            result = CoreUtils::Convert(str);
        }
        else
            GetText(title->mTitleRes, (ICharSequence**)&result);
    }

    return result;
}

ECode ResolverActivity::Dismiss()
{
    Boolean res;
    if (!(IsFinishing(&res), res)) {
        Finish();
    }
    return NOERROR;
}

ECode ResolverActivity::GetIcon(
    /* [in] */ IResources* res,
    /* [in] */ Int32 resId,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(res);
    VALIDATE_NOT_NULL(drawable);
    *drawable = NULL;
    res->GetDrawableForDensity(resId, mIconDpi, drawable);
    return NOERROR;
}

ECode ResolverActivity::LoadIconForResolveInfo(
        /* [in] */ IResolveInfo* ri,
        /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(ri);
    VALIDATE_NOT_NULL(drawable);

    ECode ec = NOERROR;
    do {
        String pkgName;
        ri->GetResolvePackageName(&pkgName);
        Int32 icon;
        ri->GetIcon(&icon);
        if (pkgName != NULL && icon != 0) {
            AutoPtr<IResources> resources;
            ec = mPm->GetResourcesForApplication(pkgName, (IResources**)&resources);
            if (FAILED(ec))
                break;
            GetIcon(resources, icon, drawable);
            if (*drawable != NULL) {
                return NOERROR;
            }
        }
        Int32 iconRes;
        ri->GetIconResource(&iconRes);
        if (iconRes != 0) {
            AutoPtr<IActivityInfo> activityInfo;
            ri->GetActivityInfo((IActivityInfo**)&activityInfo);
            String riPkgName;
            IPackageItemInfo::Probe(activityInfo)->GetPackageName(&riPkgName);
            AutoPtr<IResources> resources;
            ec = mPm->GetResourcesForApplication(riPkgName, (IResources**)&resources);
            if (FAILED(ec))
                break;
            GetIcon(resources, iconRes, drawable);
            if (*drawable != NULL) {
                return NOERROR;
            }
        }
    } while (0);

    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "Couldn't find resources for package");
    }
    return ri->LoadIcon(mPm, drawable);
}

ECode ResolverActivity::OnRestart()
{
    Activity::OnRestart();
    if (!mRegistered) {
        AutoPtr<ILooper> looper;
        GetMainLooper((ILooper**)&looper);
        mPackageMonitor->Register(this, looper, FALSE);
        mRegistered = TRUE;
    }
    mAdapter->HandlePackagesChanged();
    return NOERROR;
}

ECode ResolverActivity::OnStop()
{
    Activity::OnStop();
    if (mRegistered) {
        mPackageMonitor->Unregister();
        mRegistered = FALSE;
    }
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    Int32 flags;
    intent->GetFlags(&flags);
    if ((flags&IIntent::FLAG_ACTIVITY_NEW_TASK) != 0) {
        // This resolver is in the unusual situation where it has been
        // launched at the top of a new task.  We don't let it be added
        // to the recent tasks shown to the user, and we need to make sure
        // that each time we are launched we get the correct launching
        // uid (not re-using the same resolver from an old launching uid),
        // so we will now finish ourself since being no longer visible,
        // the user probably can't get back to us.
        Boolean changed;
        IsChangingConfigurations(&changed);
        if (!changed) {
            Finish();
        }
    }
    return NOERROR;
}

ECode ResolverActivity::OnRestoreInstanceState(
       /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnRestoreInstanceState(savedInstanceState);
    if (mAlwaysUseOption) {
        Int32 checkedPos;
        IAbsListView::Probe(mListView)->GetCheckedItemPosition(&checkedPos);
        Boolean hasValidSelection = checkedPos != IAdapterView::INVALID_POSITION;
        mLastSelected = checkedPos;
        SetAlwaysButtonEnabled(hasValidSelection, checkedPos, TRUE);
        IView::Probe(mOnceButton)->SetEnabled(hasValidSelection);
        if (hasValidSelection) {
            IAdapterView::Probe(mListView)->SetSelection(checkedPos);
        }
    }
    return NOERROR;
}

ECode ResolverActivity::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Int32 hvCount = ((ListView*)mListView.Get())->GetHeaderViewsCount();
    position -= hvCount;
    if (position < 0) {
        // Header views don't count.
        return NOERROR;
    }
    AutoPtr<IResolveInfo> resolveInfo = mAdapter->ResolveInfoForPosition(position, TRUE);
    if (mResolvingHome && HasManagedProfile() && !SupportsManagedProfiles(resolveInfo)) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        String format;
        resources->GetString(R::string::activity_resolver_work_profiles_support, &format);
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        AutoPtr<IPackageManager> pm;
        GetPackageManager((IPackageManager**)&pm);
        AutoPtr<ICharSequence> label;
        IPackageItemInfo::Probe(activityInfo)->LoadLabel(pm, (ICharSequence**)&label);
        AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
        params->Set(0, label);
        String strText = StringUtils::Format(format, params);

        AutoPtr<IToast> toast;
        Toast::MakeText(this, CoreUtils::Convert(strText), IToast::LENGTH_LONG, (IToast**)&toast);
        toast->Show();
        return NOERROR;
    }
    Int32 checkedPos;
    IAbsListView::Probe(mListView)->GetCheckedItemPosition(&checkedPos);
    Boolean hasValidSelection = checkedPos != IAdapterView::INVALID_POSITION;
    if (mAlwaysUseOption && (!hasValidSelection || mLastSelected != checkedPos)) {
        SetAlwaysButtonEnabled(hasValidSelection, checkedPos, TRUE);
        IView::Probe(mOnceButton)->SetEnabled(hasValidSelection);
        if (hasValidSelection) {
            IAbsListView::Probe(mListView)->SmoothScrollToPosition(checkedPos);
        }
        mLastSelected = checkedPos;
    }
    else {
        FAIL_RETURN(StartSelected(position, FALSE, TRUE));
    }

    return NOERROR;
}

Boolean ResolverActivity::HasManagedProfile()
{
    AutoPtr<IInterface> service;
    GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(service);
    if (userManager == NULL) {
        return FALSE;
    }

    // try {
        Int32 userId;
        GetUserId(&userId);
        AutoPtr<IList> profiles;
        if (FAILED(userManager->GetProfiles(userId, (IList**)&profiles)))
            return FALSE;

        Int32 size;
        profiles->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> item;
            profiles->Get(i, (IInterface**)&item);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(item);
            Boolean result = FALSE;
            if (userInfo != NULL && (userInfo->IsManagedProfile(&result), result)) {
                return TRUE;
            }
        }
    // } catch (SecurityException e) {
    //     return FALSE;
    // }
    return FALSE;
}

Boolean ResolverActivity::SupportsManagedProfiles(
    /* [in] */ IResolveInfo* resolveInfo)
{
    // try {
        AutoPtr<IPackageManager> pm;
        GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IActivityInfo> activityInfo;
        resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        String pkgName;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pkgName);
        AutoPtr<IApplicationInfo> appInfo;
        if (FAILED(pm->GetApplicationInfo(pkgName, 0 /* default flags */,
            (IApplicationInfo**)&appInfo)))
            return FALSE;

        Int32 version;
        appInfo->GetTargetSdkVersion(&version);
        return VersionNumberAtLeastL(version);
    // } catch (NameNotFoundException e) {
    //     return FALSE;
    // }
}

Boolean ResolverActivity::VersionNumberAtLeastL(
    /* [in] */ Int32 versionNumber)
{
    return versionNumber >= Build::VERSION_CODES::LOLLIPOP;
}

void ResolverActivity::SetAlwaysButtonEnabled(
    /* [in] */ Boolean hasValidSelection,
    /* [in] */ Int32 checkedPos,
    /* [in] */ Boolean filtered)
{
    Boolean enabled = FALSE;
    if (hasValidSelection) {
        AutoPtr<IResolveInfo> ri = mAdapter->ResolveInfoForPosition(checkedPos, filtered);
        if (((CResolveInfo*)ri.Get())->mTargetUserId == UserHandle::USER_CURRENT) {
            enabled = TRUE;
        }
    }
    IView::Probe(mAlwaysButton)->SetEnabled(enabled);
}

ECode ResolverActivity::OnButtonClick(
       /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);
    Int32 pos;
    IAbsListView::Probe(mListView)->GetCheckedItemPosition(&pos);
    FAIL_RETURN(StartSelected(mAlwaysUseOption ? pos : mAdapter->GetFilteredPosition(),
        id == R::id::button_always, mAlwaysUseOption));
    Dismiss();
    return NOERROR;
}

ECode ResolverActivity::StartSelected(
    /* [in] */ Int32 which,
    /* [in] */ Boolean always,
    /* [in] */ Boolean filtered)
{
    AutoPtr<IResolveInfo> ri = mAdapter->ResolveInfoForPosition(which, filtered);
    AutoPtr<IIntent> intent = mAdapter->IntentForPosition(which, filtered);
    FAIL_RETURN(OnIntentSelected(ri, intent, always));
    Finish();
    return NOERROR;
}

ECode ResolverActivity::GetReplacementIntent(
    /* [in] */ const String& packageName,
    /* [in] */ IIntent* defIntent,
    /* [out] */ IIntent** outIntent)
{
    VALIDATE_NOT_NULL(outIntent)
    *outIntent = defIntent;
    REFCOUNT_ADD(*outIntent)
    return NOERROR;
}

ECode ResolverActivity::OnIntentSelected(
    /* [in] */ IResolveInfo* ri,
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean alwaysCheck)
{
    if ((mAlwaysUseOption || mAdapter->HasFilteredItem()) && mAdapter->mOrigResolveList != NULL) {
        // Build a reasonable intent filter, based on what matched.
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);

        String action;
        intent->GetAction(&action);
        if (action != NULL) {
            filter->AddAction(action);
        }
        AutoPtr<ArrayOf<String> > categories;
        intent->GetCategories((ArrayOf<String>**)&categories);
        Int32 size = categories->GetLength();
        if (categories != NULL) {
            for (Int32 i = 0; i < size; ++i) {
                filter->AddCategory((*categories)[i]);
            }
        }
        filter->AddCategory(IIntent::CATEGORY_DEFAULT);

        Int32 match;
        ri->GetMatch(&match);
        Int32 cat = match&IIntentFilter::MATCH_CATEGORY_MASK;
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        if (cat == IIntentFilter::MATCH_CATEGORY_TYPE) {
            String mimeType;
            intent->ResolveType(this, &mimeType);
            if (mimeType != NULL) {
                if (FAILED(filter->AddDataType(mimeType))) {
                    Logger::W(TAG, "ResolverActivity");
                    filter = NULL;
                }
            }
        }
        String scheme;
        data->GetScheme(&scheme);
        if (data != NULL && scheme != NULL) {
            // We need the data specification if there was no type,
            // OR if the scheme is not one of our magical "file:"
            // or "content:" schemes (see IntentFilter for the reason).
            if (cat != IIntentFilter::MATCH_CATEGORY_TYPE
                    || (!(String("file") == scheme)
                    && !(String("content") == scheme))) {
                filter->AddDataScheme(scheme);

                // Look through the resolved filter to determine which part
                // of it matched the original Intent.
                AutoPtr<IIntentFilter> rfilter;
                ri->GetFilter((IIntentFilter**)&rfilter);
                AutoPtr<ArrayOf<IPatternMatcher*> > pIt;
                rfilter->GetSchemeSpecificParts((ArrayOf<IPatternMatcher*>**)&pIt);
                if (pIt != NULL) {
                    String ssp;
                    data->GetSchemeSpecificPart(&ssp);
                    Int32 size = pIt->GetLength();
                    for (Int32 i = 0; i < size; ++i) {
                        AutoPtr<IPatternMatcher> p = (*pIt)[i];
                        Boolean match;
                        p->Match(ssp, &match);
                        if (match) {
                            String path;
                            p->GetPath(&path);
                            Int32 type;
                            p->GetType(&type);
                            filter->AddDataSchemeSpecificPart(path, type);
                            break;
                        }
                    }
                }

                AutoPtr<ArrayOf<IIntentFilterAuthorityEntry*> > aIt;
                rfilter->GetAuthorities((ArrayOf<IIntentFilterAuthorityEntry*>**)&aIt);
                if (aIt != NULL) {
                    Int32 size = aIt->GetLength();
                    for (Int32 i = 0; i < size; ++i) {
                        AutoPtr<IIntentFilterAuthorityEntry> a = (*aIt)[i];
                        Int32 match;
                        a->Match(data, &match);
                        if (match >= 0) {
                            Int32 port;
                            a->GetPort(&port);
                            String host;
                            a->GetHost(&host);
                            filter->AddDataAuthority(host,
                                port >= 0 ? StringUtils::ToString(port) : String(NULL));
                            break;
                        }
                    }
                }

                pIt = NULL;
                rfilter->GetPaths((ArrayOf<IPatternMatcher*>**)&pIt);
                size = pIt->GetLength();
                if (pIt != NULL) {
                    String path;
                    data->GetPath(&path);
                    if (path != NULL) {
                        for (Int32 i = 0; i < size; ++i) {
                            AutoPtr<IPatternMatcher> p = (*pIt)[i];
                            Boolean match;
                            p->Match(path, &match);
                            if (match) {
                                String path;
                                p->GetPath(&path);
                                Int32 type;
                                p->GetType(&type);
                                filter->AddDataPath(path, type);
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (filter != NULL) {
            Int32 N;
            mAdapter->mOrigResolveList->GetSize(&N);
            AutoPtr<ArrayOf<IComponentName*> > set = ArrayOf<IComponentName*>::Alloc(N);
            Int32 bestMatch = 0;
            for (Int32 i=0; i<N; i++) {
                AutoPtr<IInterface> item;
                mAdapter->mOrigResolveList->Get(i, (IInterface**)&item);
                AutoPtr<IResolveInfo> r = IResolveInfo::Probe(item);
                AutoPtr<IActivityInfo> aInfo;
                r->GetActivityInfo((IActivityInfo**)&aInfo);
                String pkgName, name;
                IPackageItemInfo::Probe(aInfo)->GetPackageName(&pkgName);
                IPackageItemInfo::Probe(aInfo)->GetName(&name);
                CComponentName::New(pkgName, name, (IComponentName**)&(*set)[i]);
                Int32 rmatch;
                r->GetMatch(&rmatch);
                if (rmatch > bestMatch) bestMatch = rmatch;
            }

            AutoPtr<IComponentName> component;
            intent->GetComponent((IComponentName**)&component);
            if (alwaysCheck) {
                AutoPtr<IPackageManager> pkgMgr;
                GetPackageManager((IPackageManager**)&pkgMgr);
                FAIL_RETURN(pkgMgr->AddPreferredActivity(filter, bestMatch, set, component));
            }
            else {
                AutoPtr<IContentResolver> resolver;
                GetContentResolver((IContentResolver**)&resolver);
                String type;
                intent->ResolveTypeIfNeeded(resolver, &type);
                if (FAILED(AppGlobals::GetPackageManager()->SetLastChosenActivity(
                    intent, type, IPackageManager::MATCH_DEFAULT_ONLY,
                    filter, bestMatch, component))) {
                    Logger::D(TAG, "Error calling setLastChosenActivity\n");
                }
            }
        }
    }

    if (intent != NULL) {
        SafelyStartActivity(intent);
    }
    return NOERROR;
}

ECode ResolverActivity::SafelyStartActivity(
    /* [in] */ IIntent* intent)
{
    if (!mSafeForwardingMode) {
         return StartActivity(intent);
    }

    if (FAILED(StartActivityAsCaller(intent, NULL, UserHandle::USER_NULL))) {
        AutoPtr<IBinder> token;
        GetActivityToken((IBinder**)&token);
        String launchedFromPackage;
        if (FAILED(ActivityManagerNative::GetDefault()->GetLaunchedFromPackage(
            token, &launchedFromPackage))) {
            launchedFromPackage = "??";
        }
        Slogger::W(TAG, "Unable to launch as uid %d package %s, while running in %s",
            mLaunchedFromUid, launchedFromPackage.string(),
            CActivityThread::GetCurrentProcessName().string());
    }
    return NOERROR;
}

ECode ResolverActivity::ShowAppDetails(
    /* [in] */ IResolveInfo* ri)
{
    AutoPtr<IIntent> in;
    CIntent::New((IIntent**)&in);
    in->SetAction(ISettings::ACTION_APPLICATION_DETAILS_SETTINGS);
    AutoPtr<IActivityInfo> aInfo;
    ri->GetActivityInfo((IActivityInfo**)&aInfo);
    String pkgName;
    IPackageItemInfo::Probe(aInfo)->GetPackageName(&pkgName);
    AutoPtr<IUri> uri;
    Uri::FromParts(String("package"), pkgName, String(NULL), (IUri**)&uri);
    in->SetData(uri);
    in->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
    StartActivity(in);
    return NOERROR;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
