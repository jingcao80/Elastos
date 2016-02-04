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

using Elastos::Droid::Utility::ISparseIntArray;

/**
 * Unmodifiable version of {@link SparseIntArray}.
 */
final class UnmodifiableSparseIntArray {
    private static const String TAG = "ImmutableSparseIntArray";

    private final SparseIntArray mArray;

    public UnmodifiableSparseIntArray(SparseIntArray array) {
        mArray = array;
    }

    public Int32 Size() {
        return mArray->Size();
    }

    public Int32 Get(Int32 key) {
        return mArray->Get(key);
    }

    public Int32 Get(Int32 key, Int32 valueIfKeyNotFound) {
        return mArray->Get(key, valueIfKeyNotFound);
    }

    public Int32 KeyAt(Int32 index) {
        return mArray->KeyAt(index);
    }

    public Int32 ValueAt(Int32 index) {
        return mArray->ValueAt(index);
    }

    public Int32 IndexOfValue(Int32 value) {
        return mArray->IndexOfValue(value);
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return mArray->ToString();
    }
}
