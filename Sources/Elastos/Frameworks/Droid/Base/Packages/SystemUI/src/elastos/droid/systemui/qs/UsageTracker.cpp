
#include "elastos/droid/systemui/qs/UsageTracker.h"
#include "elastos/droid/systemui/qs/CUsageTrackerReceiver.h"
#include "../R.h"

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IListenable;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

CAR_OBJECT_IMPL(CUsageTrackerReceiver);
ECode CUsageTrackerReceiver::constructor(
    /* [in] */ IListenable* host)
{
    mHost = (UsageTracker*)host;
    return BroadcastReceiver::constructor();
}

ECode CUsageTrackerReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (mHost->mResetAction.Equals(action)) {
        mHost->Reset();
    }
    return NOERROR;
}

const Int64 UsageTracker::MILLIS_PER_DAY = 1000 * 60 * 60 * 24;
CAR_INTERFACE_IMPL(UsageTracker, Object, IListenable);
UsageTracker::UsageTracker(
    /* [in] */ IContext* context,
    /* [in] */ QSTile/*Class<?>*/* tile)
{
    CUsageTrackerReceiver::New(this, (IBroadcastReceiver**)&mReceiver);
    mContext = context;
    mPrefKey = tile->GetSimpleName() + String("LastUsed");
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 v = 0;
    res->GetInteger(R::integer::days_to_show_timeout_tiles, &v);
    mTimeToShowTile = MILLIS_PER_DAY * v;
    mResetAction = String("com.android.systemui.qs.") + tile->GetSimpleName() + String(".usage_reset");
}

ECode UsageTracker::SetListening(
    /* [in] */ Boolean listen)
{
    if (listen && !mRegistered) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New(mResetAction, (IIntentFilter**)&filter);
        AutoPtr<IIntent> i;
        mContext->RegisterReceiver(mReceiver, filter, (IIntent**)&i);
        mRegistered = TRUE;
    }
    else if (!listen && mRegistered) {
        mContext->UnregisterReceiver(mReceiver);
        mRegistered = FALSE;
    }
    return NOERROR;
}

Boolean UsageTracker::IsRecentlyUsed()
{
    Int64 lastUsed = 0;
    GetSharedPrefs()->GetInt64(mPrefKey, 0, &lastUsed);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 v = 0;
    system->GetCurrentTimeMillis(&v);
    return (v - lastUsed) < mTimeToShowTile;
}

void UsageTracker::TrackUsage()
{
    AutoPtr<ISharedPreferencesEditor> editor;
    GetSharedPrefs()->Edit((ISharedPreferencesEditor**)&editor);

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 v = 0;
    system->GetCurrentTimeMillis(&v);
    editor->PutInt64(mPrefKey, v);
    Boolean tmp = FALSE;
    editor->Commit(&tmp);
}

void UsageTracker::Reset()
{
    AutoPtr<ISharedPreferencesEditor> editor;
    GetSharedPrefs()->Edit((ISharedPreferencesEditor**)&editor);
    editor->Remove(mPrefKey);
    Boolean tmp = FALSE;
    editor->Commit(&tmp);
}

AutoPtr<ISharedPreferences> UsageTracker::GetSharedPrefs()
{
    String name;
    mContext->GetPackageName(&name);
    AutoPtr<ISharedPreferences> sp;
    mContext->GetSharedPreferences(name, 0, (ISharedPreferences**)&sp);
    return sp;
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
