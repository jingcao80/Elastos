#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/media/tv/TvInputInfoTvInputSettings.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/provider/Settings.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::Settings;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

String TvInputInfoTvInputSettings::TV_INPUT_SEPARATOR(":");
String TvInputInfoTvInputSettings::CUSTOM_NAME_SEPARATOR(",");

ECode TvInputInfoTvInputSettings::GetHiddenTvInputIds(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ ISet** result)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    String hiddenIdsString;
    Settings::Secure::GetStringForUser(cr, ISettingsSecure::TV_INPUT_HIDDEN_INPUTS, userId, &hiddenIdsString);

    AutoPtr<IHashSet> set;
    CHashSet::New((IHashSet**)&set);
    if (hiddenIdsString.IsEmpty()) {
        *result = ISet::Probe(set);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<ArrayOf<String> > ids;
    StringUtils::Split(hiddenIdsString, TV_INPUT_SEPARATOR, (ArrayOf<String>**)&ids);
    for (Int32 i = 0; i < ids->GetLength(); i++) {
        String id = (*ids)[i];
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        String s1;
        helper->Decode(id, &s1);
        AutoPtr<ICharSequence> cs1;
        CString::New(s1, (ICharSequence**)&cs1);
        set->Add(cs1);
    }
    *result = ISet::Probe(set);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TvInputInfoTvInputSettings::GetCustomLabels(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ IMap** result)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    String labelsString;
    Settings::Secure::GetStringForUser(cr, ISettingsSecure::TV_INPUT_CUSTOM_LABELS, userId, &labelsString);

    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    if (labelsString.IsEmpty()) {
        *result = IMap::Probe(map);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<ArrayOf<String> > pairs;
    StringUtils::Split(labelsString, TV_INPUT_SEPARATOR, (ArrayOf<String>**)&pairs);
    for (Int32 i = 0; i < pairs->GetLength(); i++) {
        String pairString = (*pairs)[i];
        AutoPtr<ArrayOf<String> > pair;
        StringUtils::Split(pairString, CUSTOM_NAME_SEPARATOR, (ArrayOf<String>**)&pair);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        String s1, s2;
        helper->Decode((*pair)[0], &s1);
        helper->Decode((*pair)[1], &s1);
        AutoPtr<ICharSequence> cs1;
        CString::New(s1, (ICharSequence**)&cs1);
        AutoPtr<ICharSequence> cs2;
        CString::New(s2, (ICharSequence**)&cs2);
        map->Put(cs1, cs2);
    }
    *result = IMap::Probe(map);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TvInputInfoTvInputSettings::PutHiddenTvInputs(
    /* [in] */ IContext* context,
    /* [in] */ ISet* hiddenInputIds,
    /* [in] */ Int32 userId)
{
    StringBuilder builder;
    Boolean firstItem = TRUE;
    AutoPtr<ArrayOf<IInterface*> > ids;
    hiddenInputIds->ToArray((ArrayOf<IInterface*>**)&ids);
    for (Int32 i = 0; i < ids->GetLength(); i++) {
        String inputId;
        ICharSequence::Probe((*ids)[i])->ToString(&inputId);
        EnsureValidField(inputId);
        if (firstItem) {
            firstItem = FALSE;
        } else {
            builder.Append(TV_INPUT_SEPARATOR);
        }
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        String str;
        helper->Encode(inputId, &str);
        builder.Append(str);
    }
    String s;
    builder.ToString(&s);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Boolean b;
    Settings::Secure::PutStringForUser(cr, ISettingsSecure::TV_INPUT_HIDDEN_INPUTS, s, userId, &b);
    return NOERROR;
}

ECode TvInputInfoTvInputSettings::PutCustomLabels(
    /* [in] */ IContext* context,
    /* [in] */ IMap* customLabels,
    /* [in] */ Int32 userId)
{
    StringBuilder builder;
    Boolean firstItem = TRUE;

    AutoPtr<ISet> entrySet;
    customLabels->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> i;
    entrySet->GetIterator((IIterator**)&i);
    Boolean b;
    while (i->HasNext(&b), b) {
        AutoPtr<IInterface> obj;
        i->GetNext((IInterface**)&obj);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
        obj = NULL;
        String sKey, sValue;
        entry->GetKey((IInterface**)&obj);
        ICharSequence::Probe(obj)->ToString(&sKey);
        obj = NULL;
        entry->GetValue((IInterface**)&obj);
        ICharSequence::Probe(obj)->ToString(&sValue);

        EnsureValidField(sKey);
        EnsureValidField(sValue);
        if (firstItem) {
            firstItem = FALSE;
        }
        else {
            builder.Append(TV_INPUT_SEPARATOR);
        }
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        String str;
        helper->Encode(sKey, &str);
        builder.Append(str);
        builder.Append(CUSTOM_NAME_SEPARATOR);
        helper->Encode(sValue, &str);
        builder.Append(str);
    }

    String s;
    builder.ToString(&s);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Settings::Secure::PutStringForUser(cr, ISettingsSecure::TV_INPUT_CUSTOM_LABELS, s, userId, &b);
    return NOERROR;
}

Boolean TvInputInfoTvInputSettings::IsHidden(
    /* [in] */ IContext* context,
    /* [in] */ const String& inputId,
    /* [in] */ Int32 userId)
{
    AutoPtr<ISet> set;
    GetHiddenTvInputIds(context, userId, (ISet**)&set);
    AutoPtr<ICharSequence> cs;
    CString::New(inputId, (ICharSequence**)&cs);
    Boolean b;
    set->Contains(cs, &b);
    return b;
}

String TvInputInfoTvInputSettings::GetCustomLabel(
    /* [in] */ IContext* context,
    /* [in] */ const String& inputId,
    /* [in] */ Int32 userId)
{
    AutoPtr<IMap> map;
    GetCustomLabels(context, userId, (IMap**)&map);
    AutoPtr<ICharSequence> csq;
    CString::New(inputId, (ICharSequence**)&csq);
    AutoPtr<IInterface> obj;
    map->Get(csq, (IInterface**)&obj);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
    String str;
    cs->ToString(&str);
    return str;
}

ECode TvInputInfoTvInputSettings::EnsureValidField(
    /* [in] */ const String& value)
{
    if (value.IsEmpty()) {
        // throw new IllegalArgumentException(value + " should not empty ");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
