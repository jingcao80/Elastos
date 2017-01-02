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

#include "elastos/droid/internal/telephony/CCallerInfoAsyncQuery.h"
#include "elastos/droid/internal/telephony/CCallerInfoAsyncQueryHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CCallerInfoAsyncQueryHelper, Singleton, ICallerInfoAsyncQueryHelper)

CAR_SINGLETON_IMPL(CCallerInfoAsyncQueryHelper)

ECode CCallerInfoAsyncQueryHelper::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* ctx,
    /* [in] */ IUri* contactRef,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [out] */ ICallerInfoAsyncQuery** cia)
{
    return CCallerInfoAsyncQuery::StartQuery(token, ctx, contactRef, listener, cookie, cia);
}

ECode CCallerInfoAsyncQueryHelper::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* ctx,
    /* [in] */ const String& number,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [out] */ ICallerInfoAsyncQuery** cia)
{
    return CCallerInfoAsyncQuery::StartQuery(token, ctx, number, listener, cookie, cia);
}

ECode CCallerInfoAsyncQueryHelper::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* ctx,
    /* [in] */ const String& number,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int64 subId,
    /* [out] */ ICallerInfoAsyncQuery** cia)
{
    return CCallerInfoAsyncQuery::StartQuery(token, ctx, number, listener, cookie, subId, cia);
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony