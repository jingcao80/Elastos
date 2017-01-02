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

#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTFLOCK_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTFLOCK_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CStructFlock.h"


namespace Elastos {
namespace Droid {
namespace System {

CarClass(CStructFlock)
    , public Object
    , public IStructFlock
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /** The operation type, one of F_RDLCK, F_WRLCK, or F_UNLCK. */
    CARAPI GetType(
        /* [out] */ Int16* type);

    CARAPI SetType(
        /* [in] */ Int16 type);

    /** How to interpret l_start, one of SEEK_CUR, SEEK_END, SEEK_SET. */
    CARAPI GetWhence(
        /* [out] */ Int16* whence);

    CARAPI SetWhence(
        /* [in] */ Int16 whence);

    /** Start offset. */
    CARAPI GetStart(
        /* [out] */ Int64* start);

    CARAPI SetStart(
        /* [in] */ Int64 start);

    /** Byte count to operate on. */
    CARAPI GetLen(
        /* [out] */ Int64* len);

    CARAPI SetLen(
        /* [in] */ Int64 len);

    /** Process blocking our lock (filled in by F_GETLK, otherwise unused). */
    CARAPI GetPid(
        /* [out] */ Int32* pid);

    CARAPI SetPid(
        /* [in] */ Int32 pid);

private:
    Int16 mType;
    Int16 mWhence;
    Int64 mStart;
    Int64 mLen;
    Int32 mPid;

};

}// namespace System
}// namespace Droid
}// namespace Elastos


#endif
