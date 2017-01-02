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

#include "elastos/droid/service/notification/CConditionHelper.h"
#include "elastos/droid/service/notification/Condition.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CAR_SINGLETON_IMPL(CConditionHelper)

CAR_INTERFACE_IMPL(CConditionHelper, Singleton, IConditionHelper)

ECode CConditionHelper::StateToString(
    /* [in] */ Int32 state,
    /* [out] */ String* str)
{
    return Condition::StateToString(state, str);
}

ECode CConditionHelper::RelevanceToString(
    /* [in] */ Int32 flags,
    /* [out] */ String* str)
{
    return Condition::RelevanceToString(flags, str);
}

ECode CConditionHelper::NewId(
    /* [in] */ IContext* context,
    /* [out] */ IUriBuilder** id)
{
    return Condition::NewId(context, id);
}

ECode CConditionHelper::IsValidId(
    /* [in] */ IUri* id,
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* isValidId)
{
    return Condition::IsValidId(id, pkg, isValidId);
}

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos