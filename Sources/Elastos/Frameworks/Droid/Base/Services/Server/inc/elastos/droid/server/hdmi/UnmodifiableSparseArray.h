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

using Elastos::Droid::Utility::ISparseArray;

/**
 * Unmodifiable version of {@link SparseArray}.
 */
final class UnmodifiableSparseArray<E> {
    private static const String TAG = "ImmutableSparseArray";

    private final SparseArray<E> mArray;

    public UnmodifiableSparseArray(SparseArray<E> array) {
       mArray = array;
    }

    public Int32 Size() {
        return mArray->Size();
    }

    public E Get(Int32 key) {
        return mArray->Get(key);
    }

    public E Get(Int32 key, E valueIfKeyNotFound) {
        return mArray->Get(key, valueIfKeyNotFound);
    }

    public Int32 KeyAt(Int32 index) {
        return mArray->KeyAt(index);
    }

    public E ValueAt(Int32 index) {
        return mArray->ValueAt(index);
    }

    public Int32 IndexOfValue(E value) {
        return mArray->IndexOfValue(value);
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return mArray->ToString();
    }
}
