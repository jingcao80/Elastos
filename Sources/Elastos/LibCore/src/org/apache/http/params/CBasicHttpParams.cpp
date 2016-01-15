
#include "org/apache/http/params/CBasicHttpParams.h"
#include "elastos/core/CString.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

const Int64 CBasicHttpParams::serialVersionUID;

CAR_INTERFACE_IMPL_2(CBasicHttpParams, AbstractHttpParams, IBasicHttpParams, ICloneable)

CAR_OBJECT_IMPL(CBasicHttpParams)

ECode CBasicHttpParams::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IInterface** parameter)
{
    VALIDATE_NOT_NULL(parameter)
    // See if the parameter has been explicitly defined
    AutoPtr<IInterface> param;
    if (mParameters != NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(name, (ICharSequence**)&cs);
        mParameters->Get(cs, (IInterface**)&param);
    }
    *parameter = param;
    REFCOUNT_ADD(*parameter)
    return NOERROR;
}

ECode CBasicHttpParams::SetParameter(
    /* [in] */ const String& name,
    /* [in] */ IInterface* value,
    /* [out] */ IHttpParams** httpParams)
{
    VALIDATE_NOT_NULL(httpParams)
    if (mParameters == NULL) {
        CHashMap::New((IHashMap**)&mParameters);
    }
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    mParameters->Put(cs, value);
    *httpParams = (IHttpParams*)this;
    REFCOUNT_ADD(*httpParams)
    return NOERROR;
}

ECode CBasicHttpParams::RemoveParameter(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mParameters == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    //this is to avoid the case in which the key has a null value
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    Boolean contains;
    if (mParameters->ContainsKey(cs, &contains), contains) {
        mParameters->Remove(cs);
        *result = TRUE;
        return NOERROR;
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
}

ECode CBasicHttpParams::SetParameters(
    /* [in] */ ArrayOf<String>* names,
    /* [in] */ IInterface* value)
{
    for (Int32 i = 0; i < names->GetLength(); i++) {
        AutoPtr<IHttpParams> p;
        SetParameter((*names)[i], value, (IHttpParams**)&p);
    }
    return NOERROR;
}

ECode CBasicHttpParams::IsParameterSet(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> value;
    GetParameter(name, (IInterface**)&value);
    *result = value != NULL;
    return NOERROR;
}

ECode CBasicHttpParams::IsParameterSetLocally(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mParameters == NULL) {
        *result = FALSE;
    }
    else {
        AutoPtr<ICharSequence> cs;
        CString::New(name, (ICharSequence**)&cs);
        AutoPtr<IInterface> value;
        mParameters->Get(cs, (IInterface**)&value);
        *result = value != NULL;
    }
    return NOERROR;
}

ECode CBasicHttpParams::Clear()
{
    mParameters = NULL;
    return NOERROR;
}

ECode CBasicHttpParams::Copy(
    /* [out] */ IHttpParams** httpParams)
{
    VALIDATE_NOT_NULL(httpParams)
    AutoPtr<IBasicHttpParams> clone;
    CBasicHttpParams::New((IBasicHttpParams**)&clone);
    AutoPtr<IHttpParams> hp = IHttpParams::Probe(clone);
    CopyParams(hp);
    *httpParams = hp;
    REFCOUNT_ADD(*httpParams)
    return NOERROR;
}

ECode CBasicHttpParams::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<IBasicHttpParams> clone;
    CBasicHttpParams::New((IBasicHttpParams**)&clone);
    AutoPtr<IHttpParams> hp = IHttpParams::Probe(clone);
    CopyParams(hp);
    *obj = clone->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CBasicHttpParams::CopyParams(
    /* [in] */ IHttpParams* target)
{
    if (mParameters == NULL)
        return NOERROR;

    AutoPtr<ISet> set;
    mParameters->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> iter;
    set->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        iter->GetNext((IInterface**)&value);
        AutoPtr<IMapEntry> me = IMapEntry::Probe(value);
        AutoPtr<IInterface> key;
        me->GetKey((IInterface**)&key);
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(key);
        if (key != NULL) {
            String str;
            cs->ToString(&str);
            AutoPtr<IInterface> v;
            me->GetValue((IInterface**)&v);
            AutoPtr<IHttpParams> p;
            target->SetParameter(str, v, (IHttpParams**)&p);
        }
    }
    return NOERROR;
}

ECode CBasicHttpParams::constructor()
{
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
