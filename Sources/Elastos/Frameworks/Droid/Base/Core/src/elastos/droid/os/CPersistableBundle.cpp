
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CPersistableBundle.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/utility/CArrayMap.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Internal::Utility::EIID_IXmlUtilsWriteMapCallback;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IDouble;
using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace Os {

static AutoPtr<IPersistableBundle> InitEMPTY()
{
    AutoPtr<CPersistableBundle> cpb;
    CPersistableBundle::NewByFriend((CPersistableBundle**)&cpb);
    cpb->mMap = CArrayMap::EMPTY;
    return (IPersistableBundle*)cpb.Get();
}

const AutoPtr<IPersistableBundle> CPersistableBundle::EMPTY = InitEMPTY();

const String CPersistableBundle::TAG_PERSISTABLEMAP("pbundle_as_map");

CAR_INTERFACE_IMPL_4(CPersistableBundle, BaseBundle, IPersistableBundle, ICloneable, IParcelable, IXmlUtilsWriteMapCallback)

CAR_OBJECT_IMPL(CPersistableBundle)

ECode CPersistableBundle::constructor()
{
    return BaseBundle::constructor();
}

ECode CPersistableBundle::constructor(
    /* [in] */ Int32 capacity)
{
    return BaseBundle::constructor(capacity);
}

ECode CPersistableBundle::constructor(
    /* [in] */ IPersistableBundle* b)
{
    return BaseBundle::constructor(IBaseBundle::Probe(b));
}

ECode CPersistableBundle::constructor(
    /* [in] */ IMap* map)
{
    FAIL_RETURN(BaseBundle::constructor())

    // First stuff everything in.
    PutAll(map);

    // Now verify each item throwing an exception if there is a violation.
    AutoPtr<ISet> keys;
    map->GetKeySet((ISet**)&keys);

    AutoPtr<IIterator> iterator;
    keys->GetIterator((IIterator**)&iterator);
    Boolean result;
    while (iterator->HasNext(&result), result) {
        AutoPtr<IInterface> key;
        iterator->GetNext((IInterface**)&key);
        AutoPtr<IInterface> value;
        map->Get(key, (IInterface**)&value);

        if (IMap::Probe(value) != NULL) {
            // Fix up any Maps by replacing them with PersistableBundles.
            AutoPtr<CPersistableBundle> cpb;
            CPersistableBundle::NewByFriend((CPersistableBundle**)&cpb);
            cpb->constructor(IMap::Probe(value));
            PutPersistableBundle(ICharSequence::Probe(key), (IPersistableBundle*)cpb.Get());
        }
        else if (value != NULL
            && IInteger32::Probe(value) == NULL
            && IInteger64::Probe(value) == NULL
            && IDouble::Probe(value) == NULL
            && ICharSequence::Probe(value) == NULL
            && IArrayOf::Probe(value) == NULL
            && IPersistableBundle::Probe(value) == NULL) {
            // throw new IllegalArgumentException("Bad value in PersistableBundle key=" + key +
            //         " value=" + value);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    return NOERROR;
}

AutoPtr<IPersistableBundle> CPersistableBundle::ForPair(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<IPersistableBundle> b;
    CPersistableBundle::New(1, (IPersistableBundle**)&b);
    IBaseBundle::Probe(b)->PutString(key, value);
    return b;
}

ECode CPersistableBundle::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<IPersistableBundle> b;
    CPersistableBundle::New(THIS_PROBE(IPersistableBundle), (IPersistableBundle**)&b);
    *obj = TO_IINTERFACE(b);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CPersistableBundle::PutPersistableBundle(
    /* [in] */ const String& key,
    /* [in] */ IPersistableBundle* value)
{
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    return PutPersistableBundle(keyObj, value);
}

ECode CPersistableBundle::PutPersistableBundle(
    /* [in] */ ICharSequence* key,
    /* [in] */ IPersistableBundle* value)
{
    Unparcel();
    IMap::Probe(mMap)->Put(key, value);
    return NOERROR;
}

ECode CPersistableBundle::GetPersistableBundle(
    /* [in] */ const String& key,
    /* [out] */ IPersistableBundle** value)
{
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    return GetPersistableBundle(keyObj, value);
}

ECode CPersistableBundle::GetPersistableBundle(
    /* [in] */ ICharSequence* key,
    /* [out] */ IPersistableBundle** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Unparcel();
    AutoPtr<IInterface> o;
    IMap::Probe(mMap)->Get((IInterface*)key, (IInterface**)&o);
    if (o == NULL) {
        return NOERROR;
    }

    // try {
    *value = IPersistableBundle::Probe(o);
    REFCOUNT_ADD(*value)
    return NOERROR;
    // } catch (ClassCastException e) {
    //     typeWarning(key, o, "Bundle", e);
    //     return null;
    // }

    return NOERROR;
}

ECode CPersistableBundle::WriteUnknownObject(
    /* [in] */ IInterface* v,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out)
{
    IPersistableBundle* pb =IPersistableBundle::Probe(v);
    if (pb == NULL) {
        //throw new XmlPullParserException("Unknown Object o=" + v);
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    String nullStr;
    out->WriteStartTag(nullStr, TAG_PERSISTABLEMAP);
    out->WriteAttribute(nullStr, String("name"), name);
    ((CPersistableBundle*) pb)->SaveToXml(out);
    out->WriteEndTag(nullStr, TAG_PERSISTABLEMAP);

    return NOERROR;
}

ECode CPersistableBundle::SaveToXml(
    /* [in] */ IXmlSerializer* out)
{
    Unparcel();
    XmlUtils::WriteMapXml(IMap::Probe(mMap), out, this);
    return NOERROR;
}

ECode CPersistableBundle::RestoreFromXml(
    /* [in] */ IXmlPullParser* in,
    /* [out] */ IPersistableBundle** pb)
{
    assert(0 && "TODO");
    // final int outerDepth = in.getDepth();
    // final String startTag = in.getName();
    // final String[] tagName = new String[1];
    // int event;
    // while (((event = in.next()) != XmlPullParser.END_DOCUMENT) &&
    //         (event != XmlPullParser.END_TAG || in.getDepth() < outerDepth)) {
    //     if (event == XmlPullParser.START_TAG) {
    //         return new PersistableBundle((Map<String, Object>)
    //                 XmlUtils.readThisMapXml(in, startTag, tagName, new MyReadMapCallback()));
    //     }
    // }
    // return EMPTY;
    return NOERROR;
}

ECode CPersistableBundle::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0 && "TODO");
    //return in.readPersistableBundle();
    return NOERROR;
}

ECode CPersistableBundle::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0 && "TODO");
    // final boolean oldAllowFds = parcel.pushAllowFds(false);
    // try {
    //     writeToParcelInner(parcel, flags);
    // } finally {
    //     parcel.restoreAllowFds(oldAllowFds);
    // }
    return NOERROR;
}

ECode CPersistableBundle::ToString(
    /* [out] */ String* str)
{
    assert(0 && "TODO");
    // if (mParcelledData != null) {
    //     if (mParcelledData == EMPTY_PARCEL) {
    //         return "PersistableBundle[EMPTY_PARCEL]";
    //     } else {
    //         return "PersistableBundle[mParcelledData.dataSize=" +
    //                 mParcelledData.dataSize() + "]";
    //     }
    // }
    // return "PersistableBundle[" + mMap.toString() + "]";
    return NOERROR;
}


} // namespace Os
} // namespace Droid
} // namespace Elastos
