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

package com.android.settings.net;

using static android::Net::NetworkStats::ISET_DEFAULT;
using static android::Net::NetworkStats::ISET_FOREGROUND;
using static android::Net::NetworkStats::ITAG_NONE;
using static android::Net::NetworkStatsHistory::IFIELD_RX_BYTES;
using static android::Net::NetworkStatsHistory::IFIELD_TX_BYTES;
using static android::Text::Format::DateUtils::IHOUR_IN_MILLIS;

using Elastos::Droid::Content::IAsyncTaskLoader;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IINetworkStatsSession;
using Elastos::Droid::Net::INetworkStatsHistory;
using Elastos::Droid::Net::INetworkTemplate;
using Elastos::Droid::Os::IBundle;

using Elastos::Droid::Settings::DataUsageSummary::IAppItem;

/**
 * Loader for historical chart data for both network and UID details.
 */
public class ChartDataLoader extends AsyncTaskLoader<ChartData> {
    private static const String KEY_TEMPLATE = "template";
    private static const String KEY_APP = "app";
    private static const String KEY_FIELDS = "fields";

    private final INetworkStatsSession mSession;
    private final Bundle mArgs;

    public static Bundle BuildArgs(NetworkTemplate template, AppItem app) {
        return BuildArgs(template, app, FIELD_RX_BYTES | FIELD_TX_BYTES);
    }

    public static Bundle BuildArgs(NetworkTemplate template, AppItem app, Int32 fields) {
        final Bundle args = new Bundle();
        args->PutParcelable(KEY_TEMPLATE, template);
        args->PutParcelable(KEY_APP, app);
        args->PutInt(KEY_FIELDS, fields);
        return args;
    }

    public ChartDataLoader(Context context, INetworkStatsSession session, Bundle args) {
        Super(context);
        mSession = session;
        mArgs = args;
    }

    //@Override
    protected void OnStartLoading() {
        super->OnStartLoading();
        ForceLoad();
    }

    //@Override
    public ChartData LoadInBackground() {
        final NetworkTemplate template = mArgs->GetParcelable(KEY_TEMPLATE);
        final AppItem app = mArgs->GetParcelable(KEY_APP);
        final Int32 fields = mArgs->GetInt(KEY_FIELDS);

        try {
            return LoadInBackground(template, app, fields);
        } catch (RemoteException e) {
            // since we can't do much without history, and we don't want to
            // leave with half-baked UI, we bail hard.
            throw new RuntimeException("problem reading network stats", e);
        }
    }

    private ChartData LoadInBackground(NetworkTemplate template, AppItem app, Int32 fields)
            throws RemoteException {
        final ChartData data = new ChartData();
        data.network = mSession->GetHistoryForNetwork(template, fields);

        if (app != NULL) {
            // load stats for current uid and template
            final Int32 size = app.uids->Size();
            for (Int32 i = 0; i < size; i++) {
                final Int32 uid = app.uids->KeyAt(i);
                data.detailDefault = CollectHistoryForUid(
                        template, uid, SET_DEFAULT, data.detailDefault);
                data.detailForeground = CollectHistoryForUid(
                        template, uid, SET_FOREGROUND, data.detailForeground);
            }

            if (size > 0) {
                data.detail = new NetworkStatsHistory(data.detailForeground->GetBucketDuration());
                data.detail->RecordEntireHistory(data.detailDefault);
                data.detail->RecordEntireHistory(data.detailForeground);
            } else {
                data.detailDefault = new NetworkStatsHistory(HOUR_IN_MILLIS);
                data.detailForeground = new NetworkStatsHistory(HOUR_IN_MILLIS);
                data.detail = new NetworkStatsHistory(HOUR_IN_MILLIS);
            }
        }

        return data;
    }

    //@Override
    protected void OnStopLoading() {
        super->OnStopLoading();
        CancelLoad();
    }

    //@Override
    protected void OnReset() {
        super->OnReset();
        CancelLoad();
    }

    /**
     * Collect {@link NetworkStatsHistory} for the requested UID, combining with
     * an existing {@link NetworkStatsHistory} if provided.
     */
    private NetworkStatsHistory CollectHistoryForUid(
            NetworkTemplate template, Int32 uid, Int32 set, NetworkStatsHistory existing)
            throws RemoteException {
        final NetworkStatsHistory history = mSession->GetHistoryForUid(
                template, uid, set, TAG_NONE, FIELD_RX_BYTES | FIELD_TX_BYTES);

        if (existing != NULL) {
            existing->RecordEntireHistory(history);
            return existing;
        } else {
            return history;
        }
    }
}
