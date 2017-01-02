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

#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/CUserDictionaryWords.h"
#include "elastos/droid/provider/UserDictionary.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CUserDictionaryWords)

CAR_INTERFACE_IMPL(CUserDictionaryWords, Singleton, IUserDictionaryWords)

ECode CUserDictionaryWords::AddWord(
    /* [in] */ IContext* context,
    /* [in] */ const String& word,
    /* [in] */ Int32 frequency,
    /* [in] */ Int32 localeType)
{
    return UserDictionary::Words::AddWord(context, word, frequency, localeType);
}

ECode CUserDictionaryWords::AddWord(
    /* [in] */ IContext* context,
    /* [in] */ const String& word,
    /* [in] */ Int32 frequency,
    /* [in] */ const String& shortcut,
    /* [in] */ ILocale* locale)
{
    return UserDictionary::Words::AddWord(context, word, frequency, shortcut, locale);
}

ECode CUserDictionaryWords::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = UserDictionary::Words::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
