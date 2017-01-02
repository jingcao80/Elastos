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

#ifndef __ELASTOS_DROID_DATABASE_CSQLITEBULKCURSORDESCRIPTOR_H__
#define __ELASTOS_DROID_DATABASE_CSQLITEBULKCURSORDESCRIPTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Database_CBulkCursorDescriptor.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CBulkCursorDescriptor)
    , public Object
    , public IBulkCursorDescriptor
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBulkCursorDescriptor();

    ~CBulkCursorDescriptor();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* writeout);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* readin);

    CARAPI SetCursor(
        /* [in] */ IBulkCursor* cursor);

    CARAPI GetCursor(
        /* [out] */ IBulkCursor** cursor);

    CARAPI SetColumnNames(
        /* [in] */ ArrayOf<String>* columnNames);

    CARAPI GetColumnNames(
        /* [out, callee] */ ArrayOf<String>** columnNames);

    CARAPI SetWantsAllOnMoveCalls(
        /* [in] */ Boolean wantsAllOnMoveCalls);

    CARAPI GetWantsAllOnMoveCalls(
        /* [out] */ Boolean* wantsAllOnMoveCalls);

    CARAPI SetCount(
        /* [in] */ Int32 count);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI SetWindow(
        /* [in] */ ICursorWindow* window);

    CARAPI GetWindow(
        /* [out] */ ICursorWindow** window);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetWriteToParcelFlags(
        /* [in] */ Int32 flags);

private:
    AutoPtr<IBulkCursor> mCursor;
    AutoPtr< ArrayOf<String> > mColumnNames;
    Boolean mWantsAllOnMoveCalls;
    Int32 mCount;
    AutoPtr<ICursorWindow> mWindow;
    Int32 mWriteToParcelFlags;
};

} //Database
} //Droid
} //Elastos
#endif //__ELASTOS_DROID_DATABASE_CSQLITEBULKCURSORDESCRIPTOR_H__
