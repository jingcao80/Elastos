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

package com.android.server.hdmi;

using Elastos::Droid::annotation.Nullable;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::ISlog;

using Elastos::Utility::IHashMap;

/**
 * A logger that prevents spammy log. For the same log message, it logs once every 20seconds.
 * This class is not thread-safe.
 * <p>
 * For convenience, use single character prefix for all messages.
 * Here are common acronyms
 * <ul>
 *   <li>[T]: Timout
 *   <li>[R]: Received message
 *   <li>[S]: Sent message
 *   <li>[P]: Device polling result
 * </ul>
 */
final class HdmiLogger {
    private static const String TAG = "HDMI";
    // Logging duration for same error message.
    private static const Int64 ERROR_LOG_DURATTION_MILLIS = 20 * 1000;  // 20s

    private static const Boolean DEBUG = FALSE;

    private static const ThreadLocal<HdmiLogger> sLogger = new ThreadLocal<>();

    // Key (String): log message.
    // Value (Pair(Long, Integer)): a pair of last log time millis and the number of logMessage.
    // Cache for warning.
    private final HashMap<String, Pair<Long, Integer>> mWarningTimingCache = new HashMap<>();
    // Cache for error.
    private final HashMap<String, Pair<Long, Integer>> mErrorTimingCache = new HashMap<>();

    private HdmiLogger() {
    }

    static const void Warning(String logMessage, Object... objs) {
        GetLogger()->WarningInternal(ToLogString(logMessage, objs));
    }

    private void WarningInternal(String logMessage) {
        String log = UpdateLog(mWarningTimingCache, logMessage);
        if (!log->IsEmpty()) {
            Slogger::W(TAG, log);
        }
    }

    static const void Error(String logMessage, Object... objs) {
        GetLogger()->ErrorInternal(ToLogString(logMessage, objs));
    }

    private void ErrorInternal(String logMessage) {
        String log = UpdateLog(mErrorTimingCache, logMessage);
        if (!log->IsEmpty()) {
            Slogger::E(TAG, log);
        }
    }

    static const void Debug(String logMessage, Object... objs) {
        GetLogger()->DebugInternal(ToLogString(logMessage, objs));
    }

    private void DebugInternal(String logMessage) {
        if (!DEBUG) {
            return;
        }
        Slogger::D(TAG, logMessage);
    }

    private static const String ToLogString(String logMessage, Object[] objs) {
        if (objs.length > 0) {
            return String->Format(logMessage, objs);
        } else {
            return logMessage;
        }
    }

    private static HdmiLogger GetLogger() {
        HdmiLogger logger = sLogger->Get();
        if (logger == NULL) {
            logger = new HdmiLogger();
            sLogger->Set(logger);
        }
        return logger;
    }

    private static String UpdateLog(HashMap<String, Pair<Long, Integer>> cache, String logMessage) {
        Int64 curTime = SystemClock->UptimeMillis();
        Pair<Long, Integer> timing = cache->Get(logMessage);
        if (ShouldLogNow(timing, curTime)) {
            String log = BuildMessage(logMessage, timing);
            cache->Put(logMessage, new Pair<>(curTime, 1));
            return log;
        } else {
            IncreaseLogCount(cache, logMessage);
        }
        return "";
    }

    private static String BuildMessage(String message, @Nullable Pair<Long, Integer> timing) {
        return new StringBuilder()
                .Append("[").Append(timing == NULL ? 1 : timing.second).Append("]:")
                .Append(message).ToString();
    }

    private static void IncreaseLogCount(HashMap<String, Pair<Long, Integer>> cache,
            String message) {
        Pair<Long, Integer> timing = cache->Get(message);
        if (timing != NULL) {
            cache->Put(message, new Pair<>(timing.first, timing.second + 1));
        }
    }

    private static Boolean ShouldLogNow(@Nullable Pair<Long, Integer> timing, Int64 curTime) {
        return timing == NULL || curTime - timing.first > ERROR_LOG_DURATTION_MILLIS;
    }
}
