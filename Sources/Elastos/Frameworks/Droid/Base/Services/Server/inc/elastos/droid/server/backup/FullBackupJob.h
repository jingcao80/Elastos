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

/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.backup;

using Elastos::Droid::App::Job::IJobInfo;
using Elastos::Droid::App::Job::IJobParameters;
using Elastos::Droid::App::Job::IJobScheduler;
using Elastos::Droid::App::Job::IJobService;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;

public class FullBackupJob extends JobService {
    private static const String TAG = "FullBackupJob";
    private static const Boolean DEBUG = TRUE;

    private static ComponentName sIdleService =
            new ComponentName("android", FullBackupJob.class->GetName());

    private static const Int32 JOB_ID = 0x5038;

    JobParameters mParams;

    public static void Schedule(Context ctx, Int64 minDelay) {
        JobScheduler js = (JobScheduler) ctx->GetSystemService(Context.JOB_SCHEDULER_SERVICE);
        JobInfo.Builder builder = new JobInfo->Builder(JOB_ID, sIdleService)
                .SetRequiresDeviceIdle(TRUE)
                .SetRequiredNetworkType(JobInfo.NETWORK_TYPE_UNMETERED)
                .SetRequiresCharging(TRUE);
        if (minDelay > 0) {
            builder->SetMinimumLatency(minDelay);
        }
        js->Schedule(builder->Build());
    }

    // callback from the Backup Manager Service: it's finished its work for this pass
    CARAPI FinishBackupPass() {
        if (mParams != NULL) {
            JobFinished(mParams, FALSE);
            mParams = NULL;
        }
    }

    // ----- scheduled job interface -----

    //@Override
    public Boolean OnStartJob(JobParameters params) {
        mParams = params;
        BackupManagerService service = BackupManagerService->GetInstance();
        return service->BeginFullBackup(this);
    }

    //@Override
    public Boolean OnStopJob(JobParameters params) {
        if (mParams != NULL) {
            mParams = NULL;
            BackupManagerService service = BackupManagerService->GetInstance();
            service->EndFullBackup();
        }
        return FALSE;
    }

}
