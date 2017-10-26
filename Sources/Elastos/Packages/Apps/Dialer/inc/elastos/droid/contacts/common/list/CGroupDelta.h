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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CGROUPDELTA_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CGROUPDELTA_H__

#include "_Elastos_Droid_Contacts_Common_List_CGroupDelta.h"
#include "elastos/droid/contacts/common/model/ValuesDelta.h"

using Elastos::Droid::Contacts::Common::Model::ValuesDelta;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentProviderOperation;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

CarClass(CGroupDelta)
    , public ValuesDelta
    , public IGroupDelta
{
public:
    CGroupDelta();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Build {@link GroupDelta} from the {@link Settings} row for the given
     * {@link Settings#ACCOUNT_NAME}, {@link Settings#ACCOUNT_TYPE}, and
     * {@link Settings#DATA_SET}.
     */
    static CARAPI_(AutoPtr<IGroupDelta>) FromSettings(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& accountName,
        /* [in] */ const String& accountType,
        /* [in] */ const String& dataSet,
        /* [in] */ Boolean accountHasGroups);

    static CARAPI_(AutoPtr<IGroupDelta>) FromBefore(
        /* [in] */ IContentValues* before);

    static CARAPI_(AutoPtr<IGroupDelta>) FromAfter(
        /* [in] */ IContentValues* after);

    CARAPI GetUngrouped(
        /* [out] */ Boolean* ungrouped);

    // @Override
    CARAPI BeforeExists(
        /* [out] */ Boolean* isBefore);

    CARAPI GetShouldSync(
        /* [out] */ Boolean* result);

    CARAPI GetVisible(
        /* [out] */ Boolean* result);

    CARAPI PutShouldSync(
        /* [in] */ Boolean shouldSync);

    CARAPI PutVisible(
        /* [in] */ Boolean visible);

    CARAPI GetTitle(
        /* [in] */ IContext* context,
        /* [out] */ ICharSequence** title);

    using ValuesDelta::BuildDiff;

    /**
     * Build a possible {@link ContentProviderOperation} to persist any
     * changes to the {@link Groups} or {@link Settings} row described by
     * this {@link GroupDelta}.
     */
    CARAPI BuildDiff(
        /* [out] */ IContentProviderOperation** operation);

protected:
    CARAPI SetUngrouped(
        /* [in] */ Boolean accountHasGroups);

private:
    CARAPI_(String) GetAccountType();

private:
    Boolean mUngrouped;
    Boolean mAccountHasGroups;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CACCOUNTFILTERACTIVITY_H__
