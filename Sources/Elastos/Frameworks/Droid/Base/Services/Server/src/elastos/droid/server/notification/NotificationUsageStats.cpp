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

#include "elastos/droid/server/notification/NotificationUsageStats.h"
#include "elastos/droid/server/notification/NotificationRecord.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationAction;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IProcess;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CoreUtils;
using Elastos::Core::IThread;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const Boolean NotificationUsageStats::ENABLE_AGGREGATED_IN_MEMORY_STATS = FALSE;
const Boolean NotificationUsageStats::ENABLE_SQLITE_LOG = FALSE;

AutoPtr< ArrayOf<AggregatedStats*> > NotificationUsageStats::InitEMPTY_AGGREGATED_STATS()
{
    AutoPtr< ArrayOf<AggregatedStats*> > args = ArrayOf<AggregatedStats*>::Alloc(0);
    return args;
}

const AutoPtr< ArrayOf<AggregatedStats*> > NotificationUsageStats::EMPTY_AGGREGATED_STATS = InitEMPTY_AGGREGATED_STATS();

const String NotificationUsageStats::SQLiteLog::TAG("NotificationSQLiteLog");

const Int32 NotificationUsageStats::SQLiteLog::MSG_POST;
const Int32 NotificationUsageStats::SQLiteLog::MSG_CLICK;
const Int32 NotificationUsageStats::SQLiteLog::MSG_REMOVE;
const Int32 NotificationUsageStats::SQLiteLog::MSG_DISMISS;

const String NotificationUsageStats::SQLiteLog::DB_NAME("notification_log.db");
const Int32 NotificationUsageStats::SQLiteLog::DB_VERSION = 4;

const Int64 NotificationUsageStats::SQLiteLog::HORIZON_MS = 7 * 24 * 60 * 60 * 1000LL;  // 1 week
const Int64 NotificationUsageStats::SQLiteLog::PRUNE_MIN_DELAY_MS = 6 * 60 * 60 * 1000LL;  // 6 hours
const Int64 NotificationUsageStats::SQLiteLog::PRUNE_MIN_WRITES = 1024;

const String NotificationUsageStats::SQLiteLog::TAB_LOG("log");
const String NotificationUsageStats::SQLiteLog::COL_EVENT_USER_ID("event_user_id");
const String NotificationUsageStats::SQLiteLog::COL_EVENT_TYPE("event_type");
const String NotificationUsageStats::SQLiteLog::COL_EVENT_TIME("event_time_ms");
const String NotificationUsageStats::SQLiteLog::COL_KEY("key");
const String NotificationUsageStats::SQLiteLog::COL_PKG("pkg");
const String NotificationUsageStats::SQLiteLog::COL_NOTIFICATION_ID("nid");
const String NotificationUsageStats::SQLiteLog::COL_TAG("tag");
const String NotificationUsageStats::SQLiteLog::COL_WHEN_MS("when_ms");
const String NotificationUsageStats::SQLiteLog::COL_DEFAULTS("defaults");
const String NotificationUsageStats::SQLiteLog::COL_FLAGS("flags");
const String NotificationUsageStats::SQLiteLog::COL_PRIORITY("priority");
const String NotificationUsageStats::SQLiteLog::COL_CATEGORY("category");
const String NotificationUsageStats::SQLiteLog::COL_ACTION_COUNT("action_count");
const String NotificationUsageStats::SQLiteLog::COL_POSTTIME_MS("posttime_ms");
const String NotificationUsageStats::SQLiteLog::COL_AIRTIME_MS("airtime_ms");
const String NotificationUsageStats::SQLiteLog::COL_FIRST_EXPANSIONTIME_MS("first_expansion_time_ms");
const String NotificationUsageStats::SQLiteLog::COL_AIRTIME_EXPANDED_MS("expansion_airtime_ms");
const String NotificationUsageStats::SQLiteLog::COL_EXPAND_COUNT("expansion_count");

const Int32 NotificationUsageStats::SQLiteLog::EVENT_TYPE_POST = 1;
const Int32 NotificationUsageStats::SQLiteLog::EVENT_TYPE_CLICK = 2;
const Int32 NotificationUsageStats::SQLiteLog::EVENT_TYPE_REMOVE = 3;
const Int32 NotificationUsageStats::SQLiteLog::EVENT_TYPE_DISMISS = 4;

Int64 NotificationUsageStats::SQLiteLog::sLastPruneMs;
Int64 NotificationUsageStats::SQLiteLog::sNumWrites;

const Int64 NotificationUsageStats::SQLiteLog::DAY_MS = 24 * 60 * 60 * 1000LL;

//===============================================================================
//                  SingleNotificationStats
//===============================================================================

SingleNotificationStats::SingleNotificationStats()
    : mPosttimeElapsedMs(-1)
    , mPosttimeToFirstClickMs(-1)
    , mPosttimeToDismissMs(-1)
    , mAirtimeCount(0)
    , mPosttimeToFirstAirtimeMs(-1)
    , mCurrentAirtimeStartElapsedMs(-1)
    , mAirtimeMs(0)
    , mPosttimeToFirstVisibleExpansionMs(-1)
    , mCurrentAirtimeExpandedStartElapsedMs(-1)
    , mAirtimeExpandedMs(0)
    , mUserExpansionCount(0)
    , mIsVisible(FALSE)
    , mIsExpanded(FALSE)
{}

SingleNotificationStats::~SingleNotificationStats()
{}

Int64 SingleNotificationStats::GetCurrentPosttimeMs()
{
    if (mPosttimeElapsedMs < 0) {
        return 0;
    }
    return SystemClock::GetElapsedRealtime() - mPosttimeElapsedMs;
}

Int64 SingleNotificationStats::GetCurrentAirtimeMs()
{
    Int64 result = mAirtimeMs;
    // Add incomplete airtime if currently shown.
    if (mCurrentAirtimeStartElapsedMs >= 0) {
        result += (SystemClock::GetElapsedRealtime() - mCurrentAirtimeStartElapsedMs);
    }
    return result;
}

Int64 SingleNotificationStats::GetCurrentAirtimeExpandedMs()
{
    Int64 result = mAirtimeExpandedMs;
    // Add incomplete expanded airtime if currently shown.
    if (mCurrentAirtimeExpandedStartElapsedMs >= 0) {
        result += (SystemClock::GetElapsedRealtime() - mCurrentAirtimeExpandedStartElapsedMs);
    }
    return result;
}

void SingleNotificationStats::OnClick()
{
    if (mPosttimeToFirstClickMs < 0) {
        mPosttimeToFirstClickMs = SystemClock::GetElapsedRealtime() - mPosttimeElapsedMs;
    }
}

void SingleNotificationStats::OnDismiss()
{
    if (mPosttimeToDismissMs < 0) {
        mPosttimeToDismissMs = SystemClock::GetElapsedRealtime() - mPosttimeElapsedMs;
    }
    Finish();
}

void SingleNotificationStats::OnCancel()
{
    Finish();
}

void SingleNotificationStats::OnRemoved()
{
    Finish();
}

ECode SingleNotificationStats::OnVisibilityChanged(
    /* [in] */ Boolean visible)
{
    Int64 elapsedNowMs = SystemClock::GetElapsedRealtime();
    Boolean wasVisible = mIsVisible;
    mIsVisible = visible;
    if (visible) {
        if (mCurrentAirtimeStartElapsedMs < 0) {
            mAirtimeCount++;
            mCurrentAirtimeStartElapsedMs = elapsedNowMs;
        }
        if (mPosttimeToFirstAirtimeMs < 0) {
            mPosttimeToFirstAirtimeMs = elapsedNowMs - mPosttimeElapsedMs;
        }
    }
    else {
        if (mCurrentAirtimeStartElapsedMs >= 0) {
            mAirtimeMs += (elapsedNowMs - mCurrentAirtimeStartElapsedMs);
            mCurrentAirtimeStartElapsedMs = -1;
        }
    }

    if (wasVisible != mIsVisible) {
        UpdateVisiblyExpandedStats();
    }
    return NOERROR;
}

void SingleNotificationStats::OnExpansionChanged(
    /* [in] */ Boolean userAction,
    /* [in] */ Boolean expanded)
{
    mIsExpanded = expanded;
    if (mIsExpanded && userAction) {
        mUserExpansionCount++;
    }
    UpdateVisiblyExpandedStats();
}

void SingleNotificationStats::UpdateVisiblyExpandedStats()
{
    Int64 elapsedNowMs = SystemClock::GetElapsedRealtime();
    if (mIsExpanded && mIsVisible) {
        // expanded and visible
        if (mCurrentAirtimeExpandedStartElapsedMs < 0) {
            mCurrentAirtimeExpandedStartElapsedMs = elapsedNowMs;
        }
        if (mPosttimeToFirstVisibleExpansionMs < 0) {
            mPosttimeToFirstVisibleExpansionMs = elapsedNowMs - mPosttimeElapsedMs;
        }
    }
    else {
        // not-expanded or not-visible
        if (mCurrentAirtimeExpandedStartElapsedMs >= 0) {
            mAirtimeExpandedMs += (elapsedNowMs - mCurrentAirtimeExpandedStartElapsedMs);
            mCurrentAirtimeExpandedStartElapsedMs = -1;
        }
    }
}

void SingleNotificationStats::Finish()
{
    OnVisibilityChanged(FALSE);
}

ECode SingleNotificationStats::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder;
    builder += "SingleNotificationStats{";
    builder += "posttimeElapsedMs=";
    builder += mPosttimeElapsedMs;
    builder += ", posttimeToFirstClickMs=";
    builder += mPosttimeToFirstClickMs;
    builder += ", posttimeToDismissMs=";
    builder += mPosttimeToDismissMs;
    builder += ", airtimeCount=";
    builder += mAirtimeCount;
    builder += ", airtimeMs=";
    builder += mAirtimeMs;
    builder += ", currentAirtimeStartElapsedMs=";
    builder += mCurrentAirtimeStartElapsedMs;
    builder += ", airtimeExpandedMs=";
    builder += mAirtimeExpandedMs;
    builder += ", posttimeToFirstVisibleExpansionMs=";
    builder += mPosttimeToFirstVisibleExpansionMs;
    builder += ", currentAirtimeExpandedSEMs=";
    builder += mCurrentAirtimeExpandedStartElapsedMs;
    builder += '}';

    *str = builder.ToString();
    return NOERROR;
}

//===============================================================================
//                  AggregateSamples
//===============================================================================

AggregateSamples::AggregateSamples()
    : mNumSamples(0)
    , mAvg(0.0)
    , mSum2(0.0)
    , mVar(0.0)
{}

AggregateSamples::~AggregateSamples()
{}

void AggregateSamples::AddSample(
    /* [in] */ Int64 sample)
{
    // Welford's "Method for Calculating Corrected Sums of Squares"
    // http://www.jstor.org/stable/1266577?seq=2
    mNumSamples++;
    const Double n = mNumSamples;
    const Double delta = sample - mAvg;
    mAvg += (1.0 / n) * delta;
    mSum2 += ((n - 1) / n) * delta * delta;
    const Double divisor = mNumSamples == 1 ? 1.0 : n - 1.0;
    mVar = mSum2 / divisor;
}

ECode AggregateSamples::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder;
    builder += "AggregateSamples{";
    builder += "numSamples=";
    builder += mNumSamples;
    builder += ", avg=";
    builder += mAvg;
    builder += ", var=";
    builder += mVar;
    builder += '}';

    *str = builder.ToString();
    return NOERROR;
}

//===============================================================================
//                  AggregatedStats
//===============================================================================

AggregatedStats::AggregatedStats(
    /* [in] */ const String& key)
    : mNumPostedByApp(0)
    , mNumUpdatedByApp(0)
    , mNumRemovedByApp(0)
    , mNumClickedByUser(0)
    , mNumDismissedByUser(0)
{
    mKey = key;

    mPosttimeMs = new AggregateSamples();
    mPosttimeToDismissMs = new AggregateSamples();
    mPosttimeToFirstClickMs = new AggregateSamples();
    mAirtimeCount = new AggregateSamples();
    mAirtimeMs = new AggregateSamples();
    mPosttimeToFirstAirtimeMs = new AggregateSamples();
    mUserExpansionCount = new AggregateSamples();
    mAirtimeExpandedMs = new AggregateSamples();
    mPosttimeToFirstVisibleExpansionMs = new AggregateSamples();
}

AggregatedStats::~AggregatedStats()
{}

void AggregatedStats::Collect(
    /* [in] */ SingleNotificationStats* singleNotificationStats)
{
    mPosttimeMs->AddSample(
            SystemClock::GetElapsedRealtime() - singleNotificationStats->mPosttimeElapsedMs);
    if (singleNotificationStats->mPosttimeToDismissMs >= 0) {
        mPosttimeToDismissMs->AddSample(singleNotificationStats->mPosttimeToDismissMs);
    }
    if (singleNotificationStats->mPosttimeToFirstClickMs >= 0) {
        mPosttimeToFirstClickMs->AddSample(singleNotificationStats->mPosttimeToFirstClickMs);
    }
    mAirtimeCount->AddSample(singleNotificationStats->mAirtimeCount);
    if (singleNotificationStats->mAirtimeMs >= 0) {
        mAirtimeMs->AddSample(singleNotificationStats->mAirtimeMs);
    }
    if (singleNotificationStats->mPosttimeToFirstAirtimeMs >= 0) {
        mPosttimeToFirstAirtimeMs->AddSample(
                singleNotificationStats->mPosttimeToFirstAirtimeMs);
    }
    if (singleNotificationStats->mPosttimeToFirstVisibleExpansionMs >= 0) {
        mPosttimeToFirstVisibleExpansionMs->AddSample(
                singleNotificationStats->mPosttimeToFirstVisibleExpansionMs);
    }
    mUserExpansionCount->AddSample(singleNotificationStats->mUserExpansionCount);
    if (singleNotificationStats->mAirtimeExpandedMs >= 0) {
        mAirtimeExpandedMs->AddSample(singleNotificationStats->mAirtimeExpandedMs);
    }
}

void AggregatedStats::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& indent)
{
    pw->Println(ToStringWithIndent(indent));
}

ECode AggregatedStats::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = ToStringWithIndent(String(""));
    return NOERROR;
}

String AggregatedStats::ToStringWithIndent(
    /* [in] */ const String& indent)
{
    StringBuilder builder;
    builder += indent;
    builder += "AggregatedStats{\n";
    builder += indent;
    builder += "  key='";
    builder += mKey;
    builder += "',\n";
    builder += indent;
    builder += "  numPostedByApp=";
    builder += mNumPostedByApp;
    builder += ",\n";
    builder += indent;
    builder += "  numUpdatedByApp=";
    builder += mNumUpdatedByApp;
    builder += ",\n";
    builder += indent;
    builder += "  numRemovedByApp=";
    builder += mNumRemovedByApp;
    builder += ",\n";
    builder += indent;
    builder += "  numClickedByUser=";
    builder += mNumClickedByUser;
    builder += ",\n";
    builder += indent;
    builder += "  numDismissedByUser=";
    builder += mNumDismissedByUser;
    builder += ",\n";
    builder += indent;
    builder += "  posttimeMs=";
    builder += TO_IINTERFACE(mPosttimeMs);
    builder += ",\n";
    builder += indent;
    builder += "  posttimeToDismissMs=";
    builder += TO_IINTERFACE(mPosttimeToDismissMs);
    builder += ",\n";
    builder += indent;
    builder += "  posttimeToFirstClickMs=";
    builder += TO_IINTERFACE(mPosttimeToFirstClickMs);
    builder += ",\n";
    builder += indent;
    builder += "  airtimeCount=";
    builder += TO_IINTERFACE(mAirtimeCount);
    builder += ",\n";
    builder += indent;
    builder += "  airtimeMs=";
    builder += TO_IINTERFACE(mAirtimeMs);
    builder += ",\n";
    builder += indent;
    builder += "  posttimeToFirstAirtimeMs=";
    builder += TO_IINTERFACE(mPosttimeToFirstAirtimeMs);
    builder += ",\n";
    builder += indent;
    builder += "  userExpansionCount=";
    builder += TO_IINTERFACE(mUserExpansionCount);
    builder += ",\n";
    builder += indent;
    builder += "  airtimeExpandedMs=";
    builder += TO_IINTERFACE(mAirtimeExpandedMs);
    builder += ",\n";
    builder += indent;
    builder += "  posttimeToFVEMs=";
    builder += TO_IINTERFACE(mPosttimeToFirstVisibleExpansionMs);
    builder += ",\n";
    builder += indent;
    builder += "}";

    return builder.ToString();
}

//===============================================================================
//                  NotificationUsageStats::SQLiteLog
//===============================================================================

NotificationUsageStats::SQLiteLog::SQLiteLog(
    /* [in] */ IContext* context)
{
    AutoPtr<IHandlerThread> backgroundThread;
    CHandlerThread::New(String("notification-sqlite-log"),
            IProcess::THREAD_PRIORITY_BACKGROUND, (IHandlerThread**)&backgroundThread);
    IThread::Probe(backgroundThread)->Start();
    mWriteHandler = (IHandler*)new SQLiteLogHandler(this);
    mHelper = (ISQLiteOpenHelper*)new SQLiteLogSQLiteOpenHelper(this);
}

NotificationUsageStats::SQLiteLog::~SQLiteLog()
{}

void NotificationUsageStats::SQLiteLog::LogPosted(
    /* [in] */ NotificationRecord* notification)
{
    AutoPtr<IMessage> msg;
    mWriteHandler->ObtainMessage(MSG_POST, (INotificationRecord*)notification, (IMessage**)&msg);
    Boolean res;
    mWriteHandler->SendMessage(msg, &res);
}

void NotificationUsageStats::SQLiteLog::LogClicked(
    /* [in] */ NotificationRecord* notification)
{
    AutoPtr<IMessage> msg;
    mWriteHandler->ObtainMessage(MSG_CLICK, (INotificationRecord*)notification, (IMessage**)&msg);
    Boolean res;
    mWriteHandler->SendMessage(msg, &res);
}

void NotificationUsageStats::SQLiteLog::LogRemoved(
    /* [in] */ NotificationRecord* notification)
{
    AutoPtr<IMessage> msg;
    mWriteHandler->ObtainMessage(MSG_REMOVE, (INotificationRecord*)notification, (IMessage**)&msg);
    Boolean res;
    mWriteHandler->SendMessage(msg, &res);
}

void NotificationUsageStats::SQLiteLog::LogDismissed(
    /* [in] */ NotificationRecord* notification)
{
    AutoPtr<IMessage> msg;
    mWriteHandler->ObtainMessage(MSG_DISMISS, (INotificationRecord*)notification, (IMessage**)&msg);
    Boolean res;
    mWriteHandler->SendMessage(msg, &res);
}

void NotificationUsageStats::SQLiteLog::PrintPostFrequencies(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& indent,
    /* [in] */ DumpFilter* filter)
{
    AutoPtr<ISQLiteDatabase> db;
    mHelper->GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 nowMs;
    sys->GetCurrentTimeMillis(&nowMs);

    StringBuilder builder;
    builder += "SELECT ";
    builder += COL_EVENT_USER_ID;
    builder += ", ";
    builder += COL_PKG;
    builder += ", ";
    // Bucket by day by looking at 'floor((nowMs - eventTimeMs) / dayMs)'
    builder += "CAST(((";
    builder += nowMs;
    builder += " - ";
    builder += COL_EVENT_TIME;
    builder += ") / ";
    builder += DAY_MS;
    builder += ") AS int) ";
    builder += "AS day, ";
    builder += "COUNT(*) AS cnt ";
    builder += "FROM ";
    builder += TAB_LOG;
    builder += " ";
    builder += "WHERE ";
    builder += COL_EVENT_TYPE;
    builder += "=";
    builder += EVENT_TYPE_POST;
    builder += " ";
    builder += "GROUP BY ";
    builder += COL_EVENT_USER_ID;
    builder += ", day, ";
    builder += COL_PKG;

    String q = builder.ToString();

    AutoPtr<ICursor> cursor;
    db->RawQuery(q, NULL, (ICursor**)&cursor);
    // try {
    Boolean res1, res2, res3;
    for ((cursor->MoveToFirst(&res1), res1); (cursor->IsAfterLast(&res2), !res2); (cursor->MoveToNext(&res3), res3)) {
        Int32 userId;
        cursor->GetInt32(0, &userId);
        String pkg;
        cursor->GetString(1, &pkg);
        if (filter != NULL && !filter->Matches(pkg)) continue;
        Int32 day;
        cursor->GetInt32(2, &day);
        Int32 count;
        cursor->GetInt32(3, &count);

        builder.Reset();
        builder += indent;
        builder += "post_frequency{user_id=";
        builder += userId;
        builder += ",pkg=";
        builder += pkg;
        builder += ",day=";
        builder += day;
        builder += ",count=";
        builder += count;
        builder += "}";
        pw->Println(builder.ToString());
    }
    // } finally {
    ICloseable::Probe(cursor)->Close();
    // }
}

void NotificationUsageStats::SQLiteLog::WriteEvent(
    /* [in] */ Int64 eventTimeMs,
    /* [in] */ Int32 eventType,
    /* [in] */ NotificationRecord* r)
{
    AutoPtr<IContentValues> cv;
    CContentValues::New((IContentValues**)&cv);
    AutoPtr<IUserHandle> user;;
    r->mSbn->GetUser((IUserHandle**)&user);
    Int32 id;
    user->GetIdentifier(&id);
    cv->Put(COL_EVENT_USER_ID, id);
    cv->Put(COL_EVENT_TIME, eventTimeMs);
    cv->Put(COL_EVENT_TYPE, eventType);
    PutNotificationIdentifiers(r, cv);
    if (eventType == EVENT_TYPE_POST) {
        PutNotificationDetails(r, cv);
    }
    else {
        PutPosttimeVisibility(r, cv);
    }
    AutoPtr<ISQLiteDatabase> db;
    mHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    Int64 value;
    db->Insert(TAB_LOG, String(NULL), cv, &value);
    if (value < 0) {
        Logger::W(TAG, "Error while trying to insert values: %p", cv.Get());
        // Log.wtf(TAG, "Error while trying to insert values: " + cv);
    }
    sNumWrites++;
    PruneIfNecessary(db);
}

void NotificationUsageStats::SQLiteLog::PruneIfNecessary(
    /* [in] */ ISQLiteDatabase* db)
{
    // Prune if we haven't in a while.
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 nowMs;
    sys->GetCurrentTimeMillis(&nowMs);
    if (sNumWrites > PRUNE_MIN_WRITES ||
            nowMs - sLastPruneMs > PRUNE_MIN_DELAY_MS) {
        sNumWrites = 0;
        sLastPruneMs = nowMs;
        Int64 horizonStartMs = nowMs - HORIZON_MS;

        AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = StringUtils::ToString(horizonStartMs);
        Int32 deletedRows;
        db->Delete(TAB_LOG, COL_EVENT_TIME + String(" < ?"), args, &deletedRows);
        Logger::D(TAG, "Pruned event entries: %d", deletedRows);
    }
}

void NotificationUsageStats::SQLiteLog::PutNotificationIdentifiers(
    /* [in] */ NotificationRecord* r,
    /* [in] */ IContentValues* outCv)
{
    String key;
    r->mSbn->GetKey(&key);
    outCv->Put(COL_KEY, key);
    String pkg;
    r->mSbn->GetPackageName(&pkg);
    outCv->Put(COL_PKG, pkg);
}

void NotificationUsageStats::SQLiteLog::PutNotificationDetails(
    /* [in] */ NotificationRecord* r,
    /* [in] */ IContentValues* outCv)
{
    Int32 id;
    r->mSbn->GetId(&id);
    outCv->Put(COL_NOTIFICATION_ID, id);
    String tag;
    r->mSbn->GetTag(&tag);
    if (!tag.IsNull()) {
        outCv->Put(COL_TAG, tag);
    }

    Int64 postTime;
    r->mSbn->GetPostTime(&postTime);
    outCv->Put(COL_WHEN_MS, postTime);

    AutoPtr<INotification> notification = r->GetNotification();
    Int32 flags;
    notification->GetFlags(&flags);
    outCv->Put(COL_FLAGS, flags);
    Int32 priority;
    notification->GetPriority(&priority);
    outCv->Put(COL_PRIORITY, priority);
    String category;
    notification->GetCategory(&category);
    if (!category.IsNull()) {
        outCv->Put(COL_CATEGORY, category);
    }
    AutoPtr< ArrayOf<INotificationAction*> > actions;
    notification->GetActions((ArrayOf<INotificationAction*>**)&actions);
    outCv->Put(COL_ACTION_COUNT, actions != NULL ? actions->GetLength() : 0);
}

void NotificationUsageStats::SQLiteLog::PutPosttimeVisibility(
    /* [in] */ NotificationRecord* r,
    /* [in] */ IContentValues* outCv)
{
    AutoPtr<SingleNotificationStats> stats = r->mStats;
    outCv->Put(COL_POSTTIME_MS, stats->GetCurrentPosttimeMs());
    outCv->Put(COL_AIRTIME_MS, stats->GetCurrentAirtimeMs());
    outCv->Put(COL_EXPAND_COUNT, stats->mUserExpansionCount);
    outCv->Put(COL_AIRTIME_EXPANDED_MS, stats->GetCurrentAirtimeExpandedMs());
    outCv->Put(COL_FIRST_EXPANSIONTIME_MS, stats->mPosttimeToFirstVisibleExpansionMs);
}

void NotificationUsageStats::SQLiteLog::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& indent,
    /* [in] */ DumpFilter* filter)
{
    PrintPostFrequencies(pw, indent, filter);
}

//===============================================================================
//                  NotificationUsageStats::SQLiteLogHandler
//===============================================================================

NotificationUsageStats::SQLiteLogHandler::SQLiteLogHandler(
    /* [in] */ SQLiteLog* host)
    : mHost(host)
{}

NotificationUsageStats::SQLiteLogHandler::~SQLiteLogHandler()
{}

ECode NotificationUsageStats::SQLiteLogHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<NotificationRecord> r = (NotificationRecord*)INotificationRecord::Probe(obj);

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 nowMs;
    sys->GetCurrentTimeMillis(&nowMs);
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case SQLiteLog::MSG_POST: {
            Int64 time;
            r->mSbn->GetPostTime(&time);
            mHost->WriteEvent(time, SQLiteLog::EVENT_TYPE_POST, r);
            break;
        }
        case SQLiteLog::MSG_CLICK:
            mHost->WriteEvent(nowMs, SQLiteLog::EVENT_TYPE_CLICK, r);
            break;
        case SQLiteLog::MSG_REMOVE:
            mHost->WriteEvent(nowMs, SQLiteLog::EVENT_TYPE_REMOVE, r);
            break;
        case SQLiteLog::MSG_DISMISS:
            mHost->WriteEvent(nowMs, SQLiteLog::EVENT_TYPE_DISMISS, r);
            break;
        default:
            Logger::W(SQLiteLog::TAG, "Unknown message type: %d", what);
            // Log.wtf(TAG, "Unknown message type: " + msg.what);
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  NotificationUsageStats::SQLiteLogSQLiteOpenHelper
//===============================================================================

NotificationUsageStats::SQLiteLogSQLiteOpenHelper::SQLiteLogSQLiteOpenHelper(
    /* [in] */ SQLiteLog* host)
    : mHost(host)
{}

NotificationUsageStats::SQLiteLogSQLiteOpenHelper::~SQLiteLogSQLiteOpenHelper()
{}

ECode NotificationUsageStats::SQLiteLogSQLiteOpenHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    StringBuilder builder;
    builder += "CREATE TABLE ";
    builder += SQLiteLog::TAB_LOG;
    builder += " (";
    builder += "_id INTEGER PRIMARY KEY AUTOINCREMENT,";
    builder += SQLiteLog::COL_EVENT_USER_ID;
    builder += " INT,";
    builder += SQLiteLog::COL_EVENT_TYPE;
    builder += " INT,";
    builder += SQLiteLog::COL_EVENT_TIME;
    builder += " INT,";
    builder += SQLiteLog::COL_KEY;
    builder += " TEXT,";
    builder += SQLiteLog::COL_PKG;
    builder += " TEXT,";
    builder += SQLiteLog::COL_NOTIFICATION_ID;
    builder += " INT,";
    builder += SQLiteLog::COL_TAG;
    builder += " TEXT,";
    builder += SQLiteLog::COL_WHEN_MS;
    builder += " INT,";
    builder += SQLiteLog::COL_DEFAULTS;
    builder += " INT,";
    builder += SQLiteLog::COL_FLAGS;
    builder += " INT,";
    builder += SQLiteLog::COL_PRIORITY;
    builder += " INT,";
    builder += SQLiteLog::COL_CATEGORY;
    builder += " TEXT,";
    builder += SQLiteLog::COL_ACTION_COUNT;
    builder += " INT,";
    builder += SQLiteLog::COL_POSTTIME_MS;
    builder += " INT,";
    builder += SQLiteLog::COL_AIRTIME_MS;
    builder += " INT,";
    builder += SQLiteLog::COL_FIRST_EXPANSIONTIME_MS;
    builder += " INT,";
    builder += SQLiteLog::COL_AIRTIME_EXPANDED_MS;
    builder += " INT,";
    builder += SQLiteLog::COL_EXPAND_COUNT;
    builder += " INT";
    builder += ")";

    db->ExecSQL(builder.ToString());
    return NOERROR;
}

ECode NotificationUsageStats::SQLiteLogSQLiteOpenHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    if (oldVersion <= 3) {
        // Version 3 creation left 'log' in a weird state. Just reset for now.
        db->ExecSQL(String("DROP TABLE IF EXISTS ") + SQLiteLog::TAB_LOG);
        OnCreate(db);
    }
    return NOERROR;
}

//===============================================================================
//                  NotificationUsageStats
//===============================================================================

NotificationUsageStats::NotificationUsageStats(
    /* [in] */ IContext* context)
{
    CHashMap::New((IMap**)&mStats);

    mSQLiteLog = NULL;
    if (ENABLE_SQLITE_LOG) {
        mSQLiteLog = new SQLiteLog(context);
    }
}

NotificationUsageStats::~NotificationUsageStats()
{}

void NotificationUsageStats::RegisterPostedByApp(
    /* [in] */ NotificationRecord* notification)
{
    {    AutoLock syncLock(this);
        notification->mStats = new SingleNotificationStats();
        notification->mStats->mPosttimeElapsedMs = SystemClock::GetElapsedRealtime();
        AutoPtr< ArrayOf<AggregatedStats*> > args = GetAggregatedStatsLocked(notification);
        for (Int32 i = 0; i < args->GetLength(); i++) {
            AutoPtr<AggregatedStats> stats = (*args)[i];
            stats->mNumPostedByApp++;
        }
        if (ENABLE_SQLITE_LOG) {
            mSQLiteLog->LogPosted(notification);
        }
    }
}

void NotificationUsageStats::RegisterUpdatedByApp(
    /* [in] */ NotificationRecord* notification,
    /* [in] */ NotificationRecord* old)
{
    notification->mStats = old->mStats;
    AutoPtr< ArrayOf<AggregatedStats*> > args = GetAggregatedStatsLocked(notification);
    for (Int32 i = 0; i < args->GetLength(); i++) {
        AutoPtr<AggregatedStats> stats = (*args)[i];
        stats->mNumUpdatedByApp++;
    }
}

void NotificationUsageStats::RegisterRemovedByApp(
    /* [in] */ NotificationRecord* notification)
{
    {    AutoLock syncLock(this);
        notification->mStats->OnRemoved();
        AutoPtr< ArrayOf<AggregatedStats*> > args = GetAggregatedStatsLocked(notification);
        for (Int32 i = 0; i < args->GetLength(); i++) {
            AutoPtr<AggregatedStats> stats = (*args)[i];
            stats->mNumRemovedByApp++;
            stats->Collect(notification->mStats);
        }
        if (ENABLE_SQLITE_LOG) {
            mSQLiteLog->LogRemoved(notification);
        }
    }
}

void NotificationUsageStats::RegisterDismissedByUser(
    /* [in] */ NotificationRecord* notification)
{
    {    AutoLock syncLock(this);
        notification->mStats->OnDismiss();
        AutoPtr< ArrayOf<AggregatedStats*> > args = GetAggregatedStatsLocked(notification);
        for (Int32 i = 0; i < args->GetLength(); i++) {
            AutoPtr<AggregatedStats> stats = (*args)[i];
            stats->mNumDismissedByUser++;
            stats->Collect(notification->mStats);
        }
        if (ENABLE_SQLITE_LOG) {
            mSQLiteLog->LogDismissed(notification);
        }
    }
}

void NotificationUsageStats::RegisterClickedByUser(
    /* [in] */ NotificationRecord* notification)
{
    {    AutoLock syncLock(this);
        notification->mStats->OnClick();
        AutoPtr< ArrayOf<AggregatedStats*> > args = GetAggregatedStatsLocked(notification);
        for (Int32 i = 0; i < args->GetLength(); i++) {
            AutoPtr<AggregatedStats> stats = (*args)[i];
            stats->mNumClickedByUser++;
        }
        if (ENABLE_SQLITE_LOG) {
            mSQLiteLog->LogClicked(notification);
        }
    }
}

void NotificationUsageStats::RegisterCancelDueToClick(
    /* [in] */ NotificationRecord* notification)
{
    {    AutoLock syncLock(this);
        notification->mStats->OnCancel();
        AutoPtr< ArrayOf<AggregatedStats*> > args = GetAggregatedStatsLocked(notification);
        for (Int32 i = 0; i < args->GetLength(); i++) {
            AutoPtr<AggregatedStats> stats = (*args)[i];
            stats->Collect(notification->mStats);
        }
    }
}

void NotificationUsageStats::RegisterCancelUnknown(
    /* [in] */ NotificationRecord* notification)
{
    {    AutoLock syncLock(this);
        notification->mStats->OnCancel();
        AutoPtr< ArrayOf<AggregatedStats*> > args = GetAggregatedStatsLocked(notification);
        for (Int32 i = 0; i < args->GetLength(); i++) {
            AutoPtr<AggregatedStats> stats = (*args)[i];
            stats->Collect(notification->mStats);
        }
    }
}

AutoPtr< ArrayOf<AggregatedStats*> > NotificationUsageStats::GetAggregatedStatsLocked(
    /* [in] */ NotificationRecord* record)
{
    if (!ENABLE_AGGREGATED_IN_MEMORY_STATS) {
        return EMPTY_AGGREGATED_STATS;
    }

    AutoPtr<IStatusBarNotification> n = record->mSbn;

    Int32 userId;
    n->GetUserId(&userId);
    String user = StringUtils::ToString(userId);

    String pkg;
    n->GetPackageName(&pkg);
    StringBuilder builder;
    builder += user;
    builder += ":";
    builder += pkg;
    String userPackage = builder.ToString();

    String key;
    n->GetKey(&key);

    // TODO: Use pool of arrays.
    AutoPtr< ArrayOf<AggregatedStats*> > args = ArrayOf<AggregatedStats*>::Alloc(3);
    (*args)[0] = GetOrCreateAggregatedStatsLocked(user);
    (*args)[1] = GetOrCreateAggregatedStatsLocked(userPackage);
    (*args)[2] = GetOrCreateAggregatedStatsLocked(key);

    return args;
}

AutoPtr<AggregatedStats> NotificationUsageStats::GetOrCreateAggregatedStatsLocked(
    /* [in] */ const String& key)
{
    AutoPtr<IInterface> obj;
    mStats->Get(CoreUtils::Convert(key), (IInterface**)&obj);
    AutoPtr<AggregatedStats> result = (AggregatedStats*)IObject::Probe(obj);
    if (result == NULL) {
        result = new AggregatedStats(key);
        mStats->Put(CoreUtils::Convert(key), TO_IINTERFACE(result));
    }
    return result;
}

void NotificationUsageStats::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& indent,
    /* [in] */ DumpFilter* filter)
{
    {    AutoLock syncLock(this);
        if (ENABLE_AGGREGATED_IN_MEMORY_STATS) {
            AutoPtr<ICollection> value;
            mStats->GetValues((ICollection**)&value);
            AutoPtr<IIterator> it;
            value->GetIterator((IIterator**)&it);
            Boolean res;
            while (it->HasNext(&res), res) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<AggregatedStats> as = (AggregatedStats*)IObject::Probe(obj);
                if (filter != NULL && !filter->Matches(as->mKey))
                    continue;
                as->Dump(pw, indent);
            }
        }
        if (ENABLE_SQLITE_LOG) {
            mSQLiteLog->Dump(pw, indent, filter);
        }
    }
}

} // Notification
} // Server
} // Droid
} // Elastos
