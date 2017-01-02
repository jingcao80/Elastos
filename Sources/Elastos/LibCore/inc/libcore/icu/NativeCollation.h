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

#ifndef __LIBCORE_ICU_NATIVECOLLATION_H__
#define __LIBCORE_ICU_NATIVECOLLATION_H__

#include <unicode/unistr.h>
#include "coredef.h"
#include <elastos/utility/etl/HashMap.h>
#include "Mutex.h"
#include "Elastos.CoreLibrary.h"

using Elastos::Core::Mutex;
using Elastos::Utility::ILocale;
using Elastos::Utility::Etl::HashMap;

namespace Libcore {
namespace ICU {

class NativeCollation
{
private:
    NativeCollation();
    NativeCollation(const NativeCollation&);

public:
    // Collator.
    static CARAPI_(void) CloseCollator(
        /* [in] */ Int64 collatoraddress);

    static CARAPI_(Int32) Compare(
        /* [in] */ Int64 collatoraddress,
        /* [in] */ const String& source,
        /* [in] */ const String& target);

    static CARAPI GetAttribute(
        /* [in] */ Int64 collatoraddress,
        /* [in] */ Int32 type,
        /* [out] */ Int32* result);

    static CARAPI GetCollationElementIterator(
        /* [in] */ Int64 collatoraddress,
        /* [in] */ const String& source,
        /* [out] */ Int64* coleitr);

    static CARAPI_(String) GetRules(
        /* [in] */ Int64 collatoraddress);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) GetSortKey(
        /* [in] */ Int64 collatoraddress,
        /* [in] */ const String& source);

    static CARAPI OpenCollator(
        /* [in] */ ILocale* locale,
        /* [out] */ Int64 * value);

private:
    static CARAPI OpenCollator(
        /* [in] */ const String& languageTag,
        /* [out] */ Int64* address);

public:
    static CARAPI OpenCollatorFromRules(
        /* [in] */ const String& rules,
        /* [in] */ Int32 normalizationmode,
        /* [in] */ Int32 collationstrength,
        /* [out] */ Int64* address);

    static CARAPI SafeClone(
        /* [in] */ Int64 collatoraddress,
        /* [out] */ Int64* address);

    static CARAPI SetAttribute(
        /* [in] */ Int64 collatoraddress,
        /* [in] */ Int32 type,
        /* [in] */ Int32 value);

    // CollationElementIterator.
    static CARAPI_(void) CloseElements(
        /* [in] */ Int64 address);

    static CARAPI_(Int32) GetMaxExpansion(
        /* [in] */ Int64 address,
        /* [in] */ Int32 order);

    static CARAPI_(Int32) GetOffset(
        /* [in] */ Int64 address);

    static CARAPI GetNext(
        /* [in] */ Int64 address,
        /* [out] */ Int32* result);

    static CARAPI GetPrevious(
        /* [in] */ Int64 address,
        /* [out] */ Int32* result);

    static CARAPI_(void) Reset(
        /* [in] */ Int64 address);

    static CARAPI SetOffset(
        /* [in] */ Int64 address,
        /* [in] */ Int32 offset);

    static CARAPI SetText(
        /* [in] */ Int64 address,
        /* [in] */ const String& source);

private:
    static HashMap<Int32, UnicodeString*> sUstrMap;
    static Mutex sUstrMapLock;
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_NATIVECOLLATION_H__
