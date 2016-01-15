/*
 * Copyright (C) 2010 The Android Open Source Project
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

#ifndef SCOPED_LOCAL_REF_H_included
#define SCOPED_LOCAL_REF_H_included

#include <stddef.h>

// A smart pointer that deletes a JNI local reference when it goes out of scope.
template<typename T>
class ScopedLocalRef {
public:
    ScopedLocalRef(T* localRef) : mLocalRef(localRef) {
    }

    ~ScopedLocalRef() {
        reset();
    }

    void reset(T* ptr = NULL) {
        if (ptr != mLocalRef) {
            if (mLocalRef != NULL) {
                mLocalRef = NULL;
            }
            mLocalRef = ptr;
        }
    }

    AutoPtr<T> release() __attribute__((warn_unused_result)) {
        AutoPtr<T> localRef = mLocalRef;
        mLocalRef = NULL;
        return localRef;
    }

    AutoPtr<T> get() const {
        return mLocalRef;
    }

private:
    AutoPtr<T> mLocalRef;

    // Disallow copy and assignment.
    ScopedLocalRef(const ScopedLocalRef&);
    void operator=(const ScopedLocalRef&);
};

#endif  // SCOPED_LOCAL_REF_H_included
