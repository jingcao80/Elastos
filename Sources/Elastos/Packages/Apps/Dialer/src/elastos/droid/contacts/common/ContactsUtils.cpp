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

#include "elastos/droid/contacts/common/ContactsUtils.h"

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

const String ContactsUtils::SCHEME_IMTO("imto");
const String ContactsUtils::SCHEME_MAILTO("mailto");
const String ContactsUtils::SCHEME_SMSTO("smsto");
const String ContactsUtils::TAG("ContactsUtils");
Int32 ContactsUtils::sThumbnailSize = -1;

String ContactsUtils::LookupProviderNameFromId(
    /* [in] */ Int32 protocol)
{
    assert(0);
    return String(NULL);
}

Boolean ContactsUtils::IsGraphic(
    /* [in] */ ICharSequence* str)
{
    assert(0);
    return FALSE;
}

Boolean ContactsUtils::AreObjectsEqual(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b)
{
    assert(0);
    return FALSE;
}

Boolean ContactsUtils::AreIntentActionEqual(
    /* [in] */ IIntent* a,
    /* [in] */ IIntent* b)
{
    assert(0);
    return FALSE;
}

Boolean ContactsUtils::AreContactWritableAccountsAvailable(
    /* [in] */ IContext* context)
{
    assert(0);
    return FALSE;
}

Boolean ContactsUtils::AreGroupWritableAccountsAvailable(
    /* [in] */ IContext* context)
{
    assert(0);
    return FALSE;
}

Int32 ContactsUtils::GetThumbnailSize(
    /* [in] */ IContext* context)
{
    assert(0);
    return 0;
}

AutoPtr<Pair<AutoPtr<IIntent>, AutoPtr<IIntent> > > ContactsUtils::BuildImIntent(
    /* [in] */ IContext* context,
    /* [in] */ ImDataItem* im)
{
    assert(0);
    return NULL;
}

AutoPtr<IIntent> ContactsUtils::GetCustomImIntent(
    /* [in] */ ImDataItem* im,
    /* [in] */ Int32 protocol)
{
    assert(0);
    return NULL;
}

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
