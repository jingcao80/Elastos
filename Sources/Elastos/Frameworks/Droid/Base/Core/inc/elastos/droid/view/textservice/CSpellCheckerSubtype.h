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

#ifndef __ELASTOS_DROID_VIEW_TEXTSERVICE_CSPELLCHECKERSUBTYPE_H__
#define __ELASTOS_DROID_VIEW_TEXTSERVICE_CSPELLCHECKERSUBTYPE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_View_TextService_CSpellCheckerSubtype.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;

using Elastos::Core::Object;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

/**
 * This class is used to specify meta information of a subtype contained in a spell checker.
 * Subtype can describe locale (e.g. en_US, fr_FR...) used for settings.
 */
CarClass(CSpellCheckerSubtype)
    , public Object
    , public ISpellCheckerSubtype
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSpellCheckerSubtype();

    CARAPI constructor();

    /**
     * Constructor
     * @param nameId The name of the subtype
     * @param locale The locale supported by the subtype
     * @param extraValue The extra value of the subtype
     */
    CARAPI constructor(
        /* [in] */ Int32 nameId,
        /* [in] */ const String& locale,
        /* [in] */ const String& extraValue);

    /**
     * @return the name of the subtype
     */
    CARAPI GetNameResId(
        /* [out] */ Int32* id);

    /**
     * @return the locale of the subtype
     */
    CARAPI GetLocale(
        /* [out] */ String* locale);

    /**
     * @return the extra value of the subtype
     */
    CARAPI GetExtraValue(
        /* [out] */ String* value);

    CARAPI GetHashCode(
        /* [out] */ Int32* hc);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    static CARAPI_(AutoPtr<ILocale>) ConstructLocaleFromString(
        /* [in] */ const String& localeStr);

    /**
     * The string of ExtraValue in subtype should be defined as follows:
     * example: key0,key1=value1,key2,key3,key4=value4
     * @param key the key of extra value
     * @return the value of the specified key
     */
    CARAPI GetExtraValueOf(
        /* [in] */ const String& key,
        /* [out] */ String* str);

    /**
     * The string of ExtraValue in subtype should be defined as follows:
     * example: key0,key1=value1,key2,key3,key4=value4
     * @param key the key of extra value
     * @return the subtype contains specified the extra value
     */
    CARAPI ContainsExtraValueKey(
        /* [in] */ const String& key,
        /* [out] */ Boolean* bFlag);

    /**
     * @param context Context will be used for getting Locale and PackageManager.
     * @param packageName The package name of the spell checker
     * @param appInfo The application info of the spell checker
     * @return a display name for this subtype. The string resource of the label (mSubtypeNameResId)
     * can have only one %s in it. If there is, the %s part will be replaced with the locale's
     * display name by the formatter. If there is not, this method simply returns the string
     * specified by mSubtypeNameResId. If mSubtypeNameResId is not specified (== 0), it's up to the
     * framework to generate an appropriate display name.
     */
    CARAPI GetDisplayName(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [in] */ IApplicationInfo* appInfo,
        /* [out] */ ICharSequence** name);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Sort the list of subtypes
     * @param context Context will be used for getting localized strings
     * @param flags Flags for the sort order
     * @param sci SpellCheckerInfo of which subtypes are subject to be sorted
     * @param subtypeList List which will be sorted
     * @return Sorted list of subtypes
     * @hide
     */
    static CARAPI_(AutoPtr<IList>) Sort(
        /* [in] */ IContext* context,
        /* [in] */ Int32 flags,
        /* [in] */ ISpellCheckerInfo* sci,
        /* [in] */ IList* subtypeList);

private:
    CARAPI_(AutoPtr<IHashMap>) GetExtraValueHashMap();

    static CARAPI_(Int32) HashCodeInternal(
        /* [in] */ const String& locale,
        /* [in] */ const String& extraValue);

private:
    static const String TAG;
    static const String EXTRA_VALUE_PAIR_SEPARATOR;
    static const String EXTRA_VALUE_KEY_VALUE_SEPARATOR;

    Int32 mSubtypeHashCode;
    Int32 mSubtypeNameResId;
    String mSubtypeLocale;
    String mSubtypeExtraValue;
    AutoPtr<IHashMap> mExtraValueHashMapCache;
};

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TEXTSERVICE_CSPELLCHECKERSUBTYPE_H__
