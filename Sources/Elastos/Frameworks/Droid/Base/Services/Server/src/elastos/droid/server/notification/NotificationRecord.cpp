
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Widget.h>
#include "elastos/droid/server/notification/NotificationRecord.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/Objects.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::INotificationAction;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Core::ICharSequence;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::IString;
using Elastos::Utility::Arrays;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CAR_INTERFACE_IMPL(NotificationRecord, Object, INotificationRecord);

NotificationRecord::NotificationRecord(
    /* [in] */ IStatusBarNotification* sbn,
    /* [in] */ Int32 score)
    : mIsUpdate(FALSE)
    , mOriginalFlags(0)
    , mIsCanceled(FALSE)
    , mScore(0)
    , mContactAffinity(0.0f)
    , mRecentlyIntrusive(FALSE)
    , mIntercept(FALSE)
    , mRankingTimeMs(0)
    , mPackagePriority(0)
    , mAuthoritativeRank(0)
    , mPackageVisibility(0)
{
    mSbn = sbn;
    mScore = score;
    AutoPtr<INotification> notification;
    sbn->GetNotification((INotification**)&notification);
    notification->GetFlags(&mOriginalFlags);
    mRankingTimeMs = CalculateRankingTimeMs(0);
}

NotificationRecord::~NotificationRecord()
{}

void NotificationRecord::CopyRankingInformation(
    /* [in] */ NotificationRecord* previous)
{
    mContactAffinity = previous->mContactAffinity;
    mRecentlyIntrusive = previous->mRecentlyIntrusive;
    mPackagePriority = previous->mPackagePriority;
    mPackageVisibility = previous->mPackageVisibility;
    mIntercept = previous->mIntercept;
    mRankingTimeMs = CalculateRankingTimeMs(previous->GetRankingTimeMs());
    // Don't copy mGlobalSortKey, recompute it.
}

AutoPtr<INotification> NotificationRecord::GetNotification()
{
    AutoPtr<INotification> notification;
    mSbn->GetNotification((INotification**)&notification);
    return notification;
}

Int32 NotificationRecord::GetFlags()
{
    AutoPtr<INotification> notification;
    mSbn->GetNotification((INotification**)&notification);
    Int32 flags;
    notification->GetFlags(&flags);
    return flags;
}

AutoPtr<IUserHandle> NotificationRecord::GetUser()
{
    AutoPtr<IUserHandle> user;
    mSbn->GetUser((IUserHandle**)&user);
    return user;
}

String NotificationRecord::GetKey()
{
    String key;
    mSbn->GetKey(&key);
    return key;
}

Int32 NotificationRecord::GetUserId()
{
    Int32 id;
    mSbn->GetUserId(&id);
    return id;
}

void NotificationRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ IContext* baseContext)
{
    AutoPtr<INotification> notification;
    mSbn->GetNotification((INotification**)&notification);

    StringBuilder buider;
    buider += prefix;
    buider += TO_IINTERFACE(this);
    pw->Println(buider.ToString());

    Int32 uid;
    mSbn->GetUid(&uid);
    Int32 userId;
    mSbn->GetUserId(&userId);
    buider.Reset();
    buider += prefix;
    buider += "  uid=";
    buider += uid;
    buider += " userId=";
    buider += userId;
    pw->Println(buider.ToString());

    Int32 icon;
    notification->GetIcon(&icon);
    String packageName;
    mSbn->GetPackageName(&packageName);
    String str;
    IdDebugString(baseContext, packageName, icon, &str);
    buider.Reset();
    buider += prefix;
    buider += "  icon=0x";
    buider += StringUtils::ToString(icon);
    buider += " / ";
    buider += str;
    pw->Println(buider.ToString());

    Int32 priority;
    notification->GetPriority(&priority);
    Int32 score;
    mSbn->GetScore(&score);
    buider.Reset();
    buider += prefix;
    buider += "  pri=";
    buider += priority;
    buider += " score=";
    buider += score;
    pw->Println(buider.ToString());

    String key;
    mSbn->GetKey(&key);
    buider.Reset();
    buider += prefix;
    buider += "  key=";
    buider += key;
    pw->Println(buider.ToString());

    buider.Reset();
    buider += prefix;
    buider += "  groupKey=";
    buider += GetGroupKey();
    pw->Println(buider.ToString());

    AutoPtr<IPendingIntent> intent;
    notification->GetContentIntent((IPendingIntent**)&intent);
    buider.Reset();
    buider += prefix;
    buider += "  contentIntent=";
    buider += intent;
    pw->Println(buider.ToString());

    intent = NULL;
    notification->GetDeleteIntent((IPendingIntent**)&intent);
    buider.Reset();
    buider += prefix;
    buider += "  deleteIntent=";
    buider += intent;
    pw->Println(buider.ToString());

    AutoPtr<ICharSequence> tickerText;
    notification->GetTickerText((ICharSequence**)&tickerText);
    buider.Reset();
    buider += prefix;
    buider += "  tickerText=";
    buider += tickerText;
    pw->Println(buider.ToString());

    AutoPtr<IRemoteViews> contentView;
    notification->GetContentView((IRemoteViews**)&contentView);
    buider.Reset();
    buider += prefix;
    buider += "  contentView=";
    buider += contentView;
    pw->Println(buider.ToString());

    Int32 defaults;
    notification->GetDefaults(&defaults);
    Int32 flags;
    notification->GetFlags(&flags);
    str = "";
    str.AppendFormat("  defaults=0x%08x flags=0x%08x", defaults, flags);
    pw->Println(prefix + str);

    AutoPtr<IUri> sound;
    notification->GetSound((IUri**)&sound);
    buider.Reset();
    buider += prefix;
    buider += "  sound=";
    buider += sound;
    pw->Println(buider.ToString());

    Int32 audioStreamType;
    notification->GetAudioStreamType(&audioStreamType);
    buider.Reset();
    buider += prefix;
    buider += "  audioStreamType=";
    buider += audioStreamType;
    pw->Println(buider.ToString());

    AutoPtr<IAudioAttributes> audioAttributes;
    notification->GetAudioAttributes((IAudioAttributes**)&audioAttributes);
    buider.Reset();
    buider += prefix;
    buider += "  audioAttributes=";
    buider += audioAttributes;
    pw->Println(buider.ToString());

    Int32 color;
    notification->GetColor(&color);
    str = "";
    str.AppendFormat("  color=0x%08x", color);
    pw->Println(prefix + str);

    AutoPtr< ArrayOf<Int64> > vibrate;
    notification->GetVibrate((ArrayOf<Int64>**)&vibrate);
    buider.Reset();
    buider += prefix;
    buider += "  vibrate=";
    buider += Arrays::ToString(vibrate);
    pw->Println(buider.ToString());


    Int32 ledARGB, ledOnMS, ledOffMS;
    notification->GetLedARGB(&ledARGB);
    notification->GetLedOnMS(&ledOnMS);
    notification->GetLedOffMS(&ledOffMS);
    str = "";
    str.AppendFormat("  led=0x%08x onMs=%d offMs=%d", ledARGB, ledOnMS, ledOffMS);
    pw->Println(prefix + str);

    AutoPtr< ArrayOf<INotificationAction*> > actions;
    notification->GetActions((ArrayOf<INotificationAction*>**)&actions);

    if (actions != NULL && actions->GetLength() > 0) {
        pw->Println(prefix + String("  actions={"));
        const Int32 N = actions->GetLength();
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<INotificationAction> action = (*actions)[i];
            AutoPtr<ICharSequence> title;
            action->GetTitle((ICharSequence**)&title);
            String titleStr;
            title->ToString(&titleStr);
            AutoPtr<IPendingIntent> actionIntent;
            action->GetActionIntent((IPendingIntent**)&actionIntent);
            String intentStr;
            IObject::Probe(actionIntent)->ToString(&intentStr);

            str = "";
            str.AppendFormat("%s    [%d] \"%s\" -> %s",
                    prefix.string(), i, titleStr.string(), intentStr.string());
            pw->Println(str);
        }
        pw->Println(prefix + String("  }"));
    }

    AutoPtr<IBundle> extras;
    notification->GetExtras((IBundle**)&extras);
    Int32 size;
    if (extras != NULL && (extras->GetSize(&size), size) > 0) {
        pw->Println(prefix + String("  extras={"));
        AutoPtr<ISet> set;
        extras->GetKeySet((ISet**)&set);
        AutoPtr<IIterator> iterator;
        set->GetIterator((IIterator**)&iterator);

        Boolean hasNext;
        while (iterator->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            iterator->GetNext((IInterface**)&obj);
            String key;
            ICharSequence::Probe(obj)->ToString(&key);

            buider.Reset();
            buider += prefix;
            buider += "    ";
            buider += key;
            buider += "=";
            pw->Print(buider.ToString());
            AutoPtr<IInterface> val;
            extras->Get(key, (IInterface**)&val);
            if (val == NULL) {
                pw->Println(String("null"));
            }
            else {
                AutoPtr<IClassInfo> clsInfo;
                CObject::ReflectClassInfo(val, (IClassInfo**)&clsInfo);
                String name;
                clsInfo->GetName(&name);
                pw->Print(name);
                if (ICharSequence::Probe(val) != NULL || IString::Probe(val) != NULL) {
                    // redact contents from bugreports
                }
                else if (IBitmap::Probe(val) != NULL) {
                    Int32 width, height;
                    IBitmap::Probe(val)->GetWidth(&width);
                    IBitmap::Probe(val)->GetHeight(&height);
                    str = "";
                    str.AppendFormat(" (%dx%d)", width, height);
                    pw->Print(str);
                }
                else if (IArrayOf::Probe(val) != NULL) {
                    Int32 N;
                    IArrayOf::Probe(val)->GetLength(&N);
                    buider.Reset();
                    buider += " (";
                    buider += N;
                    buider += ")";
                    pw->Println(buider.ToString());
                }
                else {
                    buider.Reset();
                    buider += " (";
                    buider += val;//String.valueOf(val)
                    buider += ")";
                    pw->Print(buider.ToString());
                }
                pw->Println();
            }
        }
        pw->Println(prefix + String("  }"));
    }

    buider.Reset();
    buider += prefix;
    buider += "  stats=";
    str = "";
    mStats->ToString(&str);
    buider += str;
    pw->Println(buider.ToString());

    buider.Reset();
    buider += prefix;
    buider += "  mContactAffinity=";
    buider += mContactAffinity;
    pw->Println(buider.ToString());

    buider.Reset();
    buider += prefix;
    buider += "  mRecentlyIntrusive=";
    buider += mRecentlyIntrusive;
    pw->Println(buider.ToString());

    buider.Reset();
    buider += prefix;
    buider += "  mPackagePriority=";
    buider += mPackagePriority;
    pw->Println(buider.ToString());

    buider.Reset();
    buider += prefix;
    buider += "  mPackageVisibility=";
    buider += mPackageVisibility;
    pw->Println(buider.ToString());

    buider.Reset();
    buider += prefix;
    buider += "  mIntercept=";
    buider += mIntercept;
    pw->Println(buider.ToString());

    buider.Reset();
    buider += prefix;
    buider += "  mGlobalSortKey=";
    buider += mGlobalSortKey;
    pw->Println(buider.ToString());

    buider.Reset();
    buider += prefix;
    buider += "  mRankingTimeMs=";
    buider += mRankingTimeMs;
    pw->Println(buider.ToString());
}

ECode NotificationRecord::IdDebugString(
    /* [in] */ IContext* baseContext,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 id,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<IContext> c;
    ECode ec;

    if (!packageName.IsNull()) {
        // try {
        ec = baseContext->CreatePackageContext(packageName, 0, (IContext**)&c);
        if (FAILED(ec)) {
            c = baseContext;
        }
        // } catch (NameNotFoundException e) {
        //     c = baseContext;
        // }
    }
    else {
        c = baseContext;
    }

    AutoPtr<IResources> r;
    c->GetResources((IResources**)&r);
    // try {
    String name;
    ec = r->GetResourceName(id, &name);
    if (FAILED(ec)) {
        *str = "<name unknown>";
        return NOERROR;
    }
    // } catch (Resources.NotFoundException e) {
    //     return "<name unknown>";
    // }
    *str = name;
    return NOERROR;
}

ECode NotificationRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int32 HashCode;
    sys->IdentityHashCode((INotificationRecord*)this, &HashCode);

    String packageName;
    mSbn->GetPackageName(&packageName);
    AutoPtr<IUserHandle> user;
    mSbn->GetUser((IUserHandle**)&user);
    String result1;
    user->ToString(&result1);
    Int32 id;
    mSbn->GetId(&id);
    String tag;
    mSbn->GetTag(&tag);
    Int32 score;
    mSbn->GetScore(&score);
    String key;
    mSbn->GetKey(&key);

    AutoPtr<INotification> notification;
    mSbn->GetNotification((INotification**)&notification);
    String result2;
    IObject::Probe(notification)->ToString(&result2);

    str->AppendFormat("NotificationRecord(0x%08x: pkg=%s user=%s id=%d tag=%s score=%d key=%s: %s)",
            HashCode, packageName.string(), result1.string(), id, tag.string(),
            score, key.string(), result2.string());
    return NOERROR;
}

void NotificationRecord::SetContactAffinity(
    /* [in] */ Float contactAffinity)
{
    mContactAffinity = contactAffinity;
}

Float NotificationRecord::GetContactAffinity()
{
    return mContactAffinity;
}

void NotificationRecord::SetRecentlyIntusive(
    /* [in] */ Boolean recentlyIntrusive)
{
    mRecentlyIntrusive = recentlyIntrusive;
}

Boolean NotificationRecord::IsRecentlyIntrusive()
{
    return mRecentlyIntrusive;
}

void NotificationRecord::SetPackagePriority(
    /* [in] */ Int32 packagePriority)
{
    mPackagePriority = packagePriority;
}

Int32 NotificationRecord::GetPackagePriority()
{
    return mPackagePriority;
}

void NotificationRecord::SetPackageVisibilityOverride(
    /* [in] */ Int32 packageVisibility)
{
    mPackageVisibility = packageVisibility;
}

Int32 NotificationRecord::GetPackageVisibilityOverride()
{
    return mPackageVisibility;
}

Boolean NotificationRecord::SetIntercepted(
    /* [in] */ Boolean intercept)
{
    mIntercept = intercept;
    return mIntercept;
}

Boolean NotificationRecord::IsIntercepted()
{
    return mIntercept;
}

Boolean NotificationRecord::IsCategory(
    /* [in] */ const String& category)
{
    String cat;
    GetNotification()->GetCategory(&cat);
    return Objects::Equals(CoreUtils::Convert(cat), CoreUtils::Convert(category));
}

Boolean NotificationRecord::IsAudioStream(
    /* [in] */ Int32 stream)
{
    Int32 type;
    GetNotification()->GetAudioStreamType(&type);
    return type == stream;
}

Boolean NotificationRecord::IsAudioAttributesUsage(
    /* [in] */ Int32 usage)
{
    AutoPtr<IAudioAttributes> attributes;
    GetNotification()->GetAudioAttributes((IAudioAttributes**)&attributes);
    Int32 result;
    return attributes != NULL && (attributes->GetUsage(&result), result) == usage;
}

Int64 NotificationRecord::GetRankingTimeMs()
{
    return mRankingTimeMs;
}

Int64 NotificationRecord::CalculateRankingTimeMs(
    /* [in] */ Int64 previousRankingTimeMs)
{
    AutoPtr<INotification> n = GetNotification();
    // Take developer provided 'when', unless it's in the future.
    Int64 postTime;
    mSbn->GetPostTime(&postTime);
    Int64 when;
    n->GetWhen(&when);
    if (when != 0 && when <= postTime) {
        return when;
    }
    // If we've ranked a previous instance with a timestamp, inherit it. This case is
    // important in order to have ranking stability for updating notifications.
    if (previousRankingTimeMs > 0) {
        return previousRankingTimeMs;
    }
    return postTime;
}

void NotificationRecord::SetGlobalSortKey(
    /* [in] */ const String& globalSortKey)
{
    mGlobalSortKey = globalSortKey;
}

String NotificationRecord::GetGlobalSortKey()
{
    return mGlobalSortKey;
}

void NotificationRecord::SetAuthoritativeRank(
    /* [in] */ Int32 authoritativeRank)
{
    mAuthoritativeRank = authoritativeRank;
}

Int32 NotificationRecord::GetAuthoritativeRank()
{
    return mAuthoritativeRank;
}

String NotificationRecord::GetGroupKey()
{
    String groupKey;
    mSbn->GetGroupKey(&groupKey);
    return groupKey;
}

} // Notification
} // Server
} // Droid
} // Elastos
