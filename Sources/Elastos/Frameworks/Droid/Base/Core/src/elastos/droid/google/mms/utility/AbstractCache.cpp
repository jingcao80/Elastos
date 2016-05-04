/*
 * Copyright (C) 2008 Esmertec AG.
 * Copyright (C) 2008 The Android Open Source Project
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

package com.google.android.mms.util;

using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IHashMap;

public abstract class AbstractCache<K, V> {
    private static const String TAG = "AbstractCache";
    private static const Boolean DEBUG = FALSE;
    private static const Boolean LOCAL_LOGV = FALSE;

    private static const Int32 MAX_CACHED_ITEMS  = 500;

    private final HashMap<K, CacheEntry<V>> mCacheMap;

    protected AbstractCache() {
        mCacheMap = new HashMap<K, CacheEntry<V>>();
    }

    public Boolean Put(K key, V value) {
        If (LOCAL_LOGV) {
            Logger::V(TAG, "Trying to put " + key + " into cache.");
        }

        If (mCacheMap->Size() >= MAX_CACHED_ITEMS) {
            // TODO Should remove the oldest or least hit cached entry
            // and then cache the new one.
            If (LOCAL_LOGV) {
                Logger::V(TAG, "Failed! size limitation reached.");
            }
            return FALSE;
        }

        If (key != NULL) {
            CacheEntry<V> cacheEntry = new CacheEntry<V>();
            cacheEntry.value = value;
            mCacheMap->Put(key, cacheEntry);

            If (LOCAL_LOGV) {
                Logger::V(TAG, key + " cached, " + mCacheMap->Size() + " items total.");
            }
            return TRUE;
        }
        return FALSE;
    }

    public V Get(K key) {
        If (LOCAL_LOGV) {
            Logger::V(TAG, "Trying to get " + key + " from cache.");
        }

        If (key != NULL) {
            CacheEntry<V> cacheEntry = mCacheMap->Get(key);
            If (cacheEntry != NULL) {
                cacheEntry.hit++;
                If (LOCAL_LOGV) {
                    Logger::V(TAG, key + " hit " + cacheEntry.hit + " times.");
                }
                return cacheEntry.value;
            }
        }
        return NULL;
    }

    public V Purge(K key) {
        If (LOCAL_LOGV) {
            Logger::V(TAG, "Trying to purge " + key);
        }

        CacheEntry<V> v = mCacheMap->Remove(key);

        If (LOCAL_LOGV) {
            Logger::V(TAG, mCacheMap->Size() + " items cached.");
        }

        return v != NULL ? v.value : NULL;
    }

    CARAPI PurgeAll() {
        If (LOCAL_LOGV) {
            Logger::V(TAG, "Purging cache, " + mCacheMap->Size()
                    + " items dropped.");
        }
        mCacheMap->Clear();
    }

    public Int32 Size() {
        return mCacheMap->Size();
    }

    private static class CacheEntry<V> {
        Int32 hit;
        V value;
    }
}
