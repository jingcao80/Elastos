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
 * limitations under the License
 */

package com.android.server.job;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::App::Job::IJobInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::IXml;

using Elastos::Droid::internal.annotations.VisibleForTesting;
using Elastos::Droid::internal.util.FastXmlSerializer;
using Elastos::Droid::Server::IIoThread;
using Elastos::Droid::Server::Ijob.controllers.JobStatus;

using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileNotFoundException;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IIOException;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;
using Org::Xmlpull::V1::IXmlSerializer;

/**
 * Maintains the master list of jobs that the job scheduler is tracking. These jobs are compared by
 * reference, so none of the functions in this class should make a copy.
 * Also handles read/write of persisted jobs.
 *
 * Note on locking:
 *      All callers to this class must <strong>lock on the class object they are calling</strong>.
 *      This is important b/c {@link com.android.server.job.JobStore.WriteJobsMapToDiskRunnable}
 *      and {@link com.android.server.job.JobStore.ReadJobMapFromDiskRunnable} lock on that
 *      object.
 */
public class JobStore {
    private static const String TAG = "JobStore";
    private static const Boolean DEBUG = JobSchedulerService.DEBUG;

    /** Threshold to adjust how often we want to write to the db. */
    private static const Int32 MAX_OPS_BEFORE_WRITE = 1;
    final ArraySet<JobStatus> mJobSet;
    final Context mContext;

    private Int32 mDirtyOperations;

    private static const Object sSingletonLock = new Object();
    private final AtomicFile mJobsFile;
    /** Handler backed by IoThread for writing to disk. */
    private final Handler mIoHandler = IoThread->GetHandler();
    private static JobStore sSingleton;

    /** Used by the {@link JobSchedulerService} to instantiate the JobStore. */
    static JobStore InitAndGet(JobSchedulerService jobManagerService) {
        {    AutoLock syncLock(sSingletonLock);
            if (sSingleton == NULL) {
                sSingleton = new JobStore(jobManagerService->GetContext(),
                        Environment->GetDataDirectory());
            }
            return sSingleton;
        }
    }

    /**
     * @return A freshly initialized job store object, with no loaded jobs.
     */
    @VisibleForTesting
    public static JobStore InitAndGetForTesting(Context context, File dataDir) {
        JobStore jobStoreUnderTest = new JobStore(context, dataDir);
        jobStoreUnderTest->Clear();
        return jobStoreUnderTest;
    }

    /**
     * Construct the instance of the job store. This results in a blocking read from disk.
     */
    private JobStore(Context context, File dataDir) {
        mContext = context;
        mDirtyOperations = 0;

        File systemDir = new File(dataDir, "system");
        File jobDir = new File(systemDir, "job");
        jobDir->Mkdirs();
        mJobsFile = new AtomicFile(new File(jobDir, "jobs.xml"));

        mJobSet = new ArraySet<JobStatus>();

        ReadJobMapFromDisk(mJobSet);
    }

    /**
     * Add a job to the master list, persisting it if necessary. If the JobStatus already exists,
     * it will be replaced.
     * @param jobStatus Job to add.
     * @return Whether or not an equivalent JobStatus was replaced by this operation.
     */
    public Boolean Add(JobStatus jobStatus) {
        Boolean replaced = mJobSet->Remove(jobStatus);
        mJobSet->Add(jobStatus);
        if (jobStatus->IsPersisted()) {
            MaybeWriteStatusToDiskAsync();
        }
        if (DEBUG) {
            Slogger::D(TAG, "Added job status to store: " + jobStatus);
        }
        return replaced;
    }

    /**
     * Whether this jobStatus object already exists in the JobStore.
     */
    public Boolean ContainsJobIdForUid(Int32 jobId, Int32 uId) {
        for (Int32 i=mJobSet->Size()-1; i>=0; i--) {
            JobStatus ts = mJobSet->ValueAt(i);
            if (ts->GetUid() == uId && ts->GetJobId() == jobId) {
                return TRUE;
            }
        }
        return FALSE;
    }

    Boolean ContainsJob(JobStatus jobStatus) {
        return mJobSet->Contains(jobStatus);
    }

    public Int32 Size() {
        return mJobSet->Size();
    }

    /**
     * Remove the provided job. Will also delete the job if it was persisted.
     * @return Whether or not the job existed to be removed.
     */
    public Boolean Remove(JobStatus jobStatus) {
        Boolean removed = mJobSet->Remove(jobStatus);
        if (!removed) {
            if (DEBUG) {
                Slogger::D(TAG, "Couldn't remove job: didn't exist: " + jobStatus);
            }
            return FALSE;
        }
        if (jobStatus->IsPersisted()) {
            MaybeWriteStatusToDiskAsync();
        }
        return removed;
    }

    @VisibleForTesting
    CARAPI Clear() {
        mJobSet->Clear();
        MaybeWriteStatusToDiskAsync();
    }

    /**
     * @param userHandle User for whom we are querying the list of jobs.
     * @return A list of all the jobs scheduled by the provided user. Never NULL.
     */
    public List<JobStatus> GetJobsByUser(Int32 userHandle) {
        List<JobStatus> matchingJobs = new ArrayList<JobStatus>();
        Iterator<JobStatus> it = mJobSet->Iterator();
        while (it->HasNext()) {
            JobStatus ts = it->Next();
            if (UserHandle->GetUserId(ts->GetUid()) == userHandle) {
                matchingJobs->Add(ts);
            }
        }
        return matchingJobs;
    }

    /**
     * @param uid Uid of the requesting app.
     * @return All JobStatus objects for a given uid from the master list. Never NULL.
     */
    public List<JobStatus> GetJobsByUid(Int32 uid) {
        List<JobStatus> matchingJobs = new ArrayList<JobStatus>();
        Iterator<JobStatus> it = mJobSet->Iterator();
        while (it->HasNext()) {
            JobStatus ts = it->Next();
            if (ts->GetUid() == uid) {
                matchingJobs->Add(ts);
            }
        }
        return matchingJobs;
    }

    /**
     * @param uid Uid of the requesting app.
     * @param jobId Job id, specified at schedule-time.
     * @return the JobStatus that matches the provided uId and jobId, or NULL if none found.
     */
    public JobStatus GetJobByUidAndJobId(Int32 uid, Int32 jobId) {
        Iterator<JobStatus> it = mJobSet->Iterator();
        while (it->HasNext()) {
            JobStatus ts = it->Next();
            if (ts->GetUid() == uid && ts->GetJobId() == jobId) {
                return ts;
            }
        }
        return NULL;
    }

    /**
     * @return The live array of JobStatus objects.
     */
    public ArraySet<JobStatus> GetJobs() {
        return mJobSet;
    }

    /** Version of the db schema. */
    private static const Int32 JOBS_FILE_VERSION = 0;
    /** Tag corresponds to constraints this job needs. */
    private static const String XML_TAG_PARAMS_CONSTRAINTS = "constraints";
    /** Tag corresponds to execution parameters. */
    private static const String XML_TAG_PERIODIC = "periodic";
    private static const String XML_TAG_ONEOFF = "one-off";
    private static const String XML_TAG_EXTRAS = "extras";

    /**
     * Every time the state changes we write all the jobs in one swath, instead of trying to
     * track incremental changes.
     * @return Whether the operation was successful. This will only fail for e.g. if the system is
     * low on storage. If this happens, we continue as normal
     */
    private void MaybeWriteStatusToDiskAsync() {
        mDirtyOperations++;
        if (mDirtyOperations >= MAX_OPS_BEFORE_WRITE) {
            if (DEBUG) {
                Slogger::V(TAG, "Writing jobs to disk.");
            }
            mIoHandler->Post(new WriteJobsMapToDiskRunnable());
        }
    }

    @VisibleForTesting
    CARAPI ReadJobMapFromDisk(ArraySet<JobStatus> jobSet) {
        new ReadJobMapFromDiskRunnable(jobSet).Run();
    }

    /**
     * Runnable that writes {@link #mJobSet} out to xml.
     * NOTE: This Runnable locks on JobStore.this
     */
    private class WriteJobsMapToDiskRunnable implements Runnable {
        //@Override
        CARAPI Run() {
            final Int64 startElapsed = SystemClock->ElapsedRealtime();
            List<JobStatus> mStoreCopy = new ArrayList<JobStatus>();
            {    AutoLock syncLock(JobStore.this);
                // Copy over the jobs so we can release the lock before writing.
                for (Int32 i=0; i<mJobSet->Size(); i++) {
                    JobStatus jobStatus = mJobSet->ValueAt(i);
                    JobStatus copy = new JobStatus(jobStatus->GetJob(), jobStatus->GetUid(),
                            jobStatus->GetEarliestRunTime(), jobStatus->GetLatestRunTimeElapsed());
                    mStoreCopy->Add(copy);
                }
            }
            WriteJobsMapImpl(mStoreCopy);
            if (JobSchedulerService.DEBUG) {
                Slogger::V(TAG, "Finished writing, took " + (SystemClock->ElapsedRealtime()
                        - startElapsed) + "ms");
            }
        }

        private void WriteJobsMapImpl(List<JobStatus> jobList) {
            try {
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                XmlSerializer out = new FastXmlSerializer();
                out->SetOutput(baos, "utf-8");
                out->StartDocument(NULL, TRUE);
                out->SetFeature("http://xmlpull.org/v1/doc/features.html#indent-output", TRUE);

                out->StartTag(NULL, "job-info");
                out->Attribute(NULL, "version", Integer->ToString(JOBS_FILE_VERSION));
                for (Int32 i=0; i<jobList->Size(); i++) {
                    JobStatus jobStatus = jobList->Get(i);
                    if (DEBUG) {
                        Slogger::D(TAG, "Saving job " + jobStatus->GetJobId());
                    }
                    out->StartTag(NULL, "job");
                    AddIdentifierAttributesToJobTag(out, jobStatus);
                    WriteConstraintsToXml(out, jobStatus);
                    WriteExecutionCriteriaToXml(out, jobStatus);
                    WriteBundleToXml(jobStatus->GetExtras(), out);
                    out->EndTag(NULL, "job");
                }
                out->EndTag(NULL, "job-info");
                out->EndDocument();

                // Write out to disk in one fell sweep.
                FileOutputStream fos = mJobsFile->StartWrite();
                fos->Write(baos->ToByteArray());
                mJobsFile->FinishWrite(fos);
                mDirtyOperations = 0;
            } catch (IOException e) {
                if (DEBUG) {
                    Slogger::V(TAG, "Error writing out job data.", e);
                }
            } catch (XmlPullParserException e) {
                if (DEBUG) {
                    Slogger::D(TAG, "Error persisting bundle.", e);
                }
            }
        }

        /** Write out a tag with data comprising the required fields of this job and its client. */
        private void AddIdentifierAttributesToJobTag(XmlSerializer out, JobStatus jobStatus)
                throws IOException {
            out->Attribute(NULL, "jobid", Integer->ToString(jobStatus->GetJobId()));
            out->Attribute(NULL, "package", jobStatus->GetServiceComponent()->GetPackageName());
            out->Attribute(NULL, "class", jobStatus->GetServiceComponent()->GetClassName());
            out->Attribute(NULL, "uid", Integer->ToString(jobStatus->GetUid()));
        }

        private void WriteBundleToXml(PersistableBundle extras, XmlSerializer out)
                throws IOException, XmlPullParserException {
            out->StartTag(NULL, XML_TAG_EXTRAS);
            extras->SaveToXml(out);
            out->EndTag(NULL, XML_TAG_EXTRAS);
        }
        /**
         * Write out a tag with data identifying this job's constraints. If the constraint isn't here
         * it doesn't apply.
         */
        private void WriteConstraintsToXml(XmlSerializer out, JobStatus jobStatus) throws IOException {
            out->StartTag(NULL, XML_TAG_PARAMS_CONSTRAINTS);
            if (jobStatus->HasUnmeteredConstraint()) {
                out->Attribute(NULL, "unmetered", Boolean->ToString(TRUE));
            }
            if (jobStatus->HasConnectivityConstraint()) {
                out->Attribute(NULL, "connectivity", Boolean->ToString(TRUE));
            }
            if (jobStatus->HasIdleConstraint()) {
                out->Attribute(NULL, "idle", Boolean->ToString(TRUE));
            }
            if (jobStatus->HasChargingConstraint()) {
                out->Attribute(NULL, "charging", Boolean->ToString(TRUE));
            }
            out->EndTag(NULL, XML_TAG_PARAMS_CONSTRAINTS);
        }

        private void WriteExecutionCriteriaToXml(XmlSerializer out, JobStatus jobStatus)
                throws IOException {
            final JobInfo job = jobStatus->GetJob();
            if (jobStatus->GetJob()->IsPeriodic()) {
                out->StartTag(NULL, XML_TAG_PERIODIC);
                out->Attribute(NULL, "period", Long->ToString(job->GetIntervalMillis()));
            } else {
                out->StartTag(NULL, XML_TAG_ONEOFF);
            }

            if (jobStatus->HasDeadlineConstraint()) {
                // Wall clock deadline.
                final Int64 deadlineWallclock =  System->CurrentTimeMillis() +
                        (jobStatus->GetLatestRunTimeElapsed() - SystemClock->ElapsedRealtime());
                out->Attribute(NULL, "deadline", Long->ToString(deadlineWallclock));
            }
            if (jobStatus->HasTimingDelayConstraint()) {
                final Int64 delayWallclock = System->CurrentTimeMillis() +
                        (jobStatus->GetEarliestRunTime() - SystemClock->ElapsedRealtime());
                out->Attribute(NULL, "delay", Long->ToString(delayWallclock));
            }

            // Only write out back-off policy if it differs from the default.
            // This also helps the case where the job is idle -> these aren't allowed to specify
            // back-off.
            if (jobStatus->GetJob()->GetInitialBackoffMillis() != JobInfo.DEFAULT_INITIAL_BACKOFF_MILLIS
                    || jobStatus->GetJob()->GetBackoffPolicy() != JobInfo.DEFAULT_BACKOFF_POLICY) {
                out->Attribute(NULL, "backoff-policy", Integer->ToString(job->GetBackoffPolicy()));
                out->Attribute(NULL, "initial-backoff", Long->ToString(job->GetInitialBackoffMillis()));
            }
            if (job->IsPeriodic()) {
                out->EndTag(NULL, XML_TAG_PERIODIC);
            } else {
                out->EndTag(NULL, XML_TAG_ONEOFF);
            }
        }
    }

    /**
     * Runnable that reads list of persisted job from xml. This is run once at start up, so doesn't
     * need to go through {@link JobStore#Add(com.android.server.job.controllers.JobStatus)}.
     */
    private class ReadJobMapFromDiskRunnable implements Runnable {
        private final ArraySet<JobStatus> jobSet;

        /**
         * @param jobSet Reference to the (empty) set of JobStatus objects that back the JobStore,
         *               so that after disk read we can populate it directly.
         */
        ReadJobMapFromDiskRunnable(ArraySet<JobStatus> jobSet) {
            this.jobSet = jobSet;
        }

        //@Override
        CARAPI Run() {
            try {
                List<JobStatus> jobs;
                FileInputStream fis = mJobsFile->OpenRead();
                {    AutoLock syncLock(JobStore.this);
                    jobs = ReadJobMapImpl(fis);
                    if (jobs != NULL) {
                        for (Int32 i=0; i<jobs->Size(); i++) {
                            this.jobSet->Add(jobs->Get(i));
                        }
                    }
                }
                fis->Close();
            } catch (FileNotFoundException e) {
                if (JobSchedulerService.DEBUG) {
                    Slogger::D(TAG, "Could not find jobs file, probably there was nothing to load.");
                }
            } catch (XmlPullParserException e) {
                if (JobSchedulerService.DEBUG) {
                    Slogger::D(TAG, "Error parsing xml.", e);
                }
            } catch (IOException e) {
                if (JobSchedulerService.DEBUG) {
                    Slogger::D(TAG, "Error parsing xml.", e);
                }
            }
        }

        private List<JobStatus> ReadJobMapImpl(FileInputStream fis)
                throws XmlPullParserException, IOException {
            XmlPullParser parser = Xml->NewPullParser();
            parser->SetInput(fis, NULL);

            Int32 eventType = parser->GetEventType();
            while (eventType != XmlPullParser.START_TAG &&
                    eventType != XmlPullParser.END_DOCUMENT) {
                eventType = parser->Next();
                Slogger::D(TAG, parser->GetName());
            }
            if (eventType == XmlPullParser.END_DOCUMENT) {
                if (DEBUG) {
                    Slogger::D(TAG, "No persisted jobs.");
                }
                return NULL;
            }

            String tagName = parser->GetName();
            if ("job-info".Equals(tagName)) {
                final List<JobStatus> jobs = new ArrayList<JobStatus>();
                // Read in version info.
                try {
                    Int32 version = Integer->ValueOf(parser->GetAttributeValue(NULL, "version"));
                    if (version != JOBS_FILE_VERSION) {
                        Slogger::D(TAG, "Invalid version number, aborting jobs file read.");
                        return NULL;
                    }
                } catch (NumberFormatException e) {
                    Slogger::E(TAG, "Invalid version number, aborting jobs file read.");
                    return NULL;
                }
                eventType = parser->Next();
                do {
                    // Read each <job/>
                    if (eventType == XmlPullParser.START_TAG) {
                        tagName = parser->GetName();
                        // Start reading job.
                        if ("job".Equals(tagName)) {
                            JobStatus persistedJob = RestoreJobFromXml(parser);
                            if (persistedJob != NULL) {
                                if (DEBUG) {
                                    Slogger::D(TAG, "Read out " + persistedJob);
                                }
                                jobs->Add(persistedJob);
                            } else {
                                Slogger::D(TAG, "Error reading job from file.");
                            }
                        }
                    }
                    eventType = parser->Next();
                } while (eventType != XmlPullParser.END_DOCUMENT);
                return jobs;
            }
            return NULL;
        }

        /**
         * @param parser Xml parser at the beginning of a "<job/>" tag. The next "parser->Next()" call
         *               will take the parser into the body of the job tag.
         * @return Newly instantiated job holding all the information we just read out of the xml tag.
         */
        private JobStatus RestoreJobFromXml(XmlPullParser parser) throws XmlPullParserException,
                IOException {
            JobInfo.Builder jobBuilder;
            Int32 uid;

            // Read out job identifier attributes.
            try {
                jobBuilder = BuildBuilderFromXml(parser);
                jobBuilder->SetPersisted(TRUE);
                uid = Integer->ValueOf(parser->GetAttributeValue(NULL, "uid"));
            } catch (NumberFormatException e) {
                Slogger::E(TAG, "Error parsing job's required fields, skipping");
                return NULL;
            }

            Int32 eventType;
            // Read out constraints tag.
            do {
                eventType = parser->Next();
            } while (eventType == XmlPullParser.TEXT);  // Push through to next START_TAG.

            if (!(eventType == XmlPullParser.START_TAG &&
                    XML_TAG_PARAMS_CONSTRAINTS->Equals(parser->GetName()))) {
                // Expecting a <constraints> start tag.
                return NULL;
            }
            try {
                BuildConstraintsFromXml(jobBuilder, parser);
            } catch (NumberFormatException e) {
                Slogger::D(TAG, "Error reading constraints, skipping.");
                return NULL;
            }
            parser->Next(); // Consume </constraints>

            // Read out execution parameters tag.
            do {
                eventType = parser->Next();
            } while (eventType == XmlPullParser.TEXT);
            if (eventType != XmlPullParser.START_TAG) {
                return NULL;
            }

            Pair<Long, Long> runtimes;
            try {
                runtimes = BuildExecutionTimesFromXml(parser);
            } catch (NumberFormatException e) {
                if (DEBUG) {
                    Slogger::D(TAG, "Error parsing execution time parameters, skipping.");
                }
                return NULL;
            }

            if (XML_TAG_PERIODIC->Equals(parser->GetName())) {
                try {
                    String val = parser->GetAttributeValue(NULL, "period");
                    jobBuilder->SetPeriodic(Long->ValueOf(val));
                } catch (NumberFormatException e) {
                    Slogger::D(TAG, "Error reading periodic execution criteria, skipping.");
                    return NULL;
                }
            } else if (XML_TAG_ONEOFF->Equals(parser->GetName())) {
                try {
                    if (runtimes.first != JobStatus.NO_EARLIEST_RUNTIME) {
                        jobBuilder->SetMinimumLatency(runtimes.first - SystemClock->ElapsedRealtime());
                    }
                    if (runtimes.second != JobStatus.NO_LATEST_RUNTIME) {
                        jobBuilder->SetOverrideDeadline(
                                runtimes.second - SystemClock->ElapsedRealtime());
                    }
                } catch (NumberFormatException e) {
                    Slogger::D(TAG, "Error reading job execution criteria, skipping.");
                    return NULL;
                }
            } else {
                if (DEBUG) {
                    Slogger::D(TAG, "Invalid parameter tag, skipping - " + parser->GetName());
                }
                // Expecting a parameters start tag.
                return NULL;
            }
            MaybeBuildBackoffPolicyFromXml(jobBuilder, parser);

            parser->NextTag(); // Consume parameters end tag.

            // Read out extras Bundle.
            do {
                eventType = parser->Next();
            } while (eventType == XmlPullParser.TEXT);
            if (!(eventType == XmlPullParser.START_TAG && XML_TAG_EXTRAS->Equals(parser->GetName()))) {
                if (DEBUG) {
                    Slogger::D(TAG, "Error reading extras, skipping.");
                }
                return NULL;
            }

            PersistableBundle extras = PersistableBundle->RestoreFromXml(parser);
            jobBuilder->SetExtras(extras);
            parser->NextTag(); // Consume </extras>

            return new JobStatus(jobBuilder->Build(), uid, runtimes.first, runtimes.second);
        }

        private JobInfo.Builder BuildBuilderFromXml(XmlPullParser parser) throws NumberFormatException {
            // Pull out required fields from <job> attributes.
            Int32 jobId = Integer->ValueOf(parser->GetAttributeValue(NULL, "jobid"));
            String packageName = parser->GetAttributeValue(NULL, "package");
            String className = parser->GetAttributeValue(NULL, "class");
            ComponentName cname = new ComponentName(packageName, className);

            return new JobInfo->Builder(jobId, cname);
        }

        private void BuildConstraintsFromXml(JobInfo.Builder jobBuilder, XmlPullParser parser) {
            String val = parser->GetAttributeValue(NULL, "unmetered");
            if (val != NULL) {
                jobBuilder->SetRequiredNetworkType(JobInfo.NETWORK_TYPE_UNMETERED);
            }
            val = parser->GetAttributeValue(NULL, "connectivity");
            if (val != NULL) {
                jobBuilder->SetRequiredNetworkType(JobInfo.NETWORK_TYPE_ANY);
            }
            val = parser->GetAttributeValue(NULL, "idle");
            if (val != NULL) {
                jobBuilder->SetRequiresDeviceIdle(TRUE);
            }
            val = parser->GetAttributeValue(NULL, "charging");
            if (val != NULL) {
                jobBuilder->SetRequiresCharging(TRUE);
            }
        }

        /**
         * Builds the back-off policy out of the params tag. These attributes may not exist, depending
         * on whether the back-off was set when the job was first scheduled.
         */
        private void MaybeBuildBackoffPolicyFromXml(JobInfo.Builder jobBuilder, XmlPullParser parser) {
            String val = parser->GetAttributeValue(NULL, "initial-backoff");
            if (val != NULL) {
                Int64 initialBackoff = Long->ValueOf(val);
                val = parser->GetAttributeValue(NULL, "backoff-policy");
                Int32 backoffPolicy = Integer->ValueOf(val);  // Will throw NFE which we catch higher up.
                jobBuilder->SetBackoffCriteria(initialBackoff, backoffPolicy);
            }
        }

        /**
         * Convenience function to read out and convert deadline and delay from xml into elapsed real
         * time.
         * @return A {@link android.util.Pair}, where the first value is the earliest elapsed runtime
         * and the second is the latest elapsed runtime.
         */
        private Pair<Long, Long> BuildExecutionTimesFromXml(XmlPullParser parser)
                throws NumberFormatException {
            // Pull out execution time data.
            final Int64 nowWallclock = System->CurrentTimeMillis();
            final Int64 nowElapsed = SystemClock->ElapsedRealtime();

            Int64 earliestRunTimeElapsed = JobStatus.NO_EARLIEST_RUNTIME;
            Int64 latestRunTimeElapsed = JobStatus.NO_LATEST_RUNTIME;
            String val = parser->GetAttributeValue(NULL, "deadline");
            if (val != NULL) {
                Int64 latestRuntimeWallclock = Long->ValueOf(val);
                Int64 maxDelayElapsed =
                        Math->Max(latestRuntimeWallclock - nowWallclock, 0);
                latestRunTimeElapsed = nowElapsed + maxDelayElapsed;
            }
            val = parser->GetAttributeValue(NULL, "delay");
            if (val != NULL) {
                Int64 earliestRuntimeWallclock = Long->ValueOf(val);
                Int64 minDelayElapsed =
                        Math->Max(earliestRuntimeWallclock - nowWallclock, 0);
                earliestRunTimeElapsed = nowElapsed + minDelayElapsed;

            }
            return Pair->Create(earliestRunTimeElapsed, latestRunTimeElapsed);
        }
    }
}
