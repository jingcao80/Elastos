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

#ifndef __ELASTOS_SQL_CDATATRUNCATION_H__
#define __ELASTOS_SQL_CDATATRUNCATION_H__

#include "_Elastos_Sql_CDataTruncation.h"
#include "Object.h"

using Elastos::Core::IThrowable;
using Elastos::Core::Object;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Sql {

CarClass(CDataTruncation)
    , public Object
    , public IDataTruncation
    , public ISQLWarning
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDataTruncation();

    CARAPI  constructor(
        /* [in] */ Int32 index,
        /* [in] */ Boolean parameter,
        /* [in] */ Boolean read,
        /* [in] */ Int32 dataSize,
        /* [in] */ Int32 transferSize);

    CARAPI  constructor(
        /* [in] */ Int32 index,
        /* [in] */ Boolean parameter,
        /* [in] */ Boolean read,
        /* [in] */ Int32 dataSize,
        /* [in] */ Int32 transferSize,
        /* [in] */ IThrowable * cause);

    CARAPI  GetDataSize(
        /* [out] */ Int32* size);

    CARAPI  GetIndex(
        /* [out] */ Int32* index);

    CARAPI  GetParameter(
        /* [out] */ Boolean* isPara);

    CARAPI  GetRead(
        /* [out] */ Boolean* isRead);

    CARAPI  GetTransferSize(
        /* [out] */ Int32* size);

    CARAPI GetNextWarning(
        /* [out] */ ISQLWarning ** sqlwar);

    CARAPI SetNextWarning(
        /* [in] */ ISQLWarning * w);

private:

    Int32 mIndex;

    Int32 mDataSize;

    Int32 mTransferSize;

    Boolean mParameter;

    Boolean mRead;

    const static String THE_REASON;

    const static String THE_SQLSTATE_READ;

    const static String THE_SQLSTATE_WRITE;

    const static Int32 THE_ERROR_CODE;// = 0;

    // const static Int64 serialVersionUID = 6464298989504059473L;
};

} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_CDATATRUNCATION_H__
