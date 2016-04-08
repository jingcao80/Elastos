#include "elastos/droid/media/CMediaCodecInfo.h"
#include "elastos/droid/utility/CRange.h"
#include "elastos/droid/utility/CRational.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Utility::CRange;
using Elastos::Droid::Utility::CRational;
using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::ICharSequence;
using Elastos::Core::Math;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CMediaCodecInfo::Feature, IInterface)

namespace Elastos {
namespace Droid {
namespace Media {

const Int32 CMediaCodecInfo::ERROR_UNRECOGNIZED = (1 << 0);
const Int32 CMediaCodecInfo::ERROR_UNSUPPORTED = (1 << 1);
const Int32 CMediaCodecInfo::ERROR_NONE_SUPPORTED = (1 << 2);
AutoPtr<IRange> CMediaCodecInfo::POSITIVE_INTEGERS;
AutoPtr<IRange> CMediaCodecInfo::POSITIVE_LONGS;
AutoPtr<IRange> CMediaCodecInfo::POSITIVE_RATIONALS;
AutoPtr<IRange> CMediaCodecInfo::SIZE_RANGE;
AutoPtr<IRange> CMediaCodecInfo::FRAME_RATE_RANGE;

CAR_INTERFACE_IMPL(CMediaCodecInfo, Object, IMediaCodecInfo)

CAR_OBJECT_IMPL(CMediaCodecInfo)

CMediaCodecInfo::CMediaCodecInfo()
    : mIsEncoder(FALSE)
{
}

CMediaCodecInfo::~CMediaCodecInfo()
{
}

ECode CMediaCodecInfo::constructor(
    /* [in] */ const String& name,
    /* [in] */ Boolean isEncoder,
    /* [in] */ ArrayOf<IMediaCodecInfoCodecCapabilities*>* caps)
{
    mName = name;
    mIsEncoder = isEncoder;
    AutoPtr<IHashMap> hMap;
    CHashMap::New((IHashMap**)&hMap);
    mCaps = IMap::Probe(hMap);

    for (Int32 i = 0; i < caps->GetLength(); i++) {
        AutoPtr<IMediaCodecInfoCodecCapabilities> c = (*caps)[i];
        String str;
        c->GetMimeType(&str);
        AutoPtr<ICharSequence> csq;
        CString::New(str, (ICharSequence**)&csq);
        mCaps->Put(csq, c);
    }
    return NOERROR;
}

ECode CMediaCodecInfo::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mName;
    return NOERROR;
}

ECode CMediaCodecInfo::IsEncoder(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsEncoder;
    return NOERROR;
}

ECode CMediaCodecInfo::GetSupportedTypes(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISet> typeSet;
    mCaps->GetKeySet((ISet**)&typeSet);
    AutoPtr<ArrayOf<IInterface*> > types;
    typeSet->ToArray((ArrayOf<IInterface*>**)&types);
    Arrays::Sort(types);
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(types->GetLength());
    for (Int32 i = 0; i < types->GetLength(); i++) {
        String s;
        ICharSequence::Probe((*types)[i])->ToString(&s);
        array->Set(i, s);
    }
    *result = array;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaCodecInfo::GetCapabilitiesForType(
    /* [in] */ const String& type,
    /* [out] */ IMediaCodecInfoCodecCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<ICharSequence> csq;
    CString::New(type, (ICharSequence**)&csq);

    AutoPtr<IInterface> obj;
    mCaps->Get(csq, (IInterface**)&obj);
    AutoPtr<IMediaCodecInfoCodecCapabilities> caps =
        IMediaCodecInfoCodecCapabilities::Probe(obj);

    if (caps == NULL) {
        // throw new IllegalArgumentException("codec does not support type");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // clone writable object
    return caps->Dup(result);
}

ECode CMediaCodecInfo::MakeRegular(
    /* [out] */ IMediaCodecInfo** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> caps;
    CArrayList::New((IArrayList**)&caps);

    AutoPtr<ICollection> values;
    mCaps->GetValues((ICollection**)&values);
    AutoPtr<ArrayOf<IInterface*> > array;
    values->ToArray((ArrayOf<IInterface*>**)&array);

    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IMediaCodecInfoCodecCapabilities> c =
                IMediaCodecInfoCodecCapabilities::Probe((*array)[i]);
        Boolean b;
        if (c->IsRegular(&b), b) {
            caps->Add(c);
        }
    }

    Int32 size;
    caps->GetSize(&size);
    Int32 capSize;
    mCaps->GetSize(&capSize);

    if (size == 0) {
        *result = NULL;
        return NOERROR;
    }
    else if (size == capSize) {
        *result = this;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<ArrayOf<IInterface*> > a;
    caps->ToArray((ArrayOf<IInterface*>**)&a);
    AutoPtr<ArrayOf<IMediaCodecInfoCodecCapabilities*> > capArray
            = ArrayOf<IMediaCodecInfoCodecCapabilities*>::Alloc(a->GetLength());
    for (Int32 i = 0; i < a->GetLength(); i++) {
        capArray->Set(i, IMediaCodecInfoCodecCapabilities::Probe((*a)[i]));
    }
    return CMediaCodecInfo::New(mName, mIsEncoder, capArray, result);
}

ECode CMediaCodecInfo::CheckPowerOfTwo(
    /* [in] */ Int32 value,
    /* [in] */ const String& message,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if ((value & (value - 1)) != 0) {
        // throw new IllegalArgumentException(message);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = value;
    return NOERROR;
}

void CMediaCodecInfo::Init()
{
    AutoPtr<IInteger32> begin;
    CInteger32::New(1, (IInteger32**)&begin);
    AutoPtr<IInteger32> end;
    CInteger32::New(Elastos::Core::Math::INT32_MAX_VALUE, (IInteger32**)&end);
    CRange::Create(begin, end, (IRange**)&POSITIVE_INTEGERS);

    AutoPtr<IInteger64> b64;
    CInteger64::New(1l, (IInteger64**)&b64);
    AutoPtr<IInteger64> e64;
    CInteger64::New(Elastos::Core::Math::INT64_MAX_VALUE, (IInteger64**)&e64);
    CRange::Create(b64, e64, (IRange**)&POSITIVE_LONGS);

    AutoPtr<IRational> s;
    CRational::New(1, Elastos::Core::Math::INT32_MAX_VALUE, (IRational**)&s);
    AutoPtr<IRational> e;
    CRational::New(Elastos::Core::Math::INT32_MAX_VALUE, 1, (IRational**)&e);
    CRange::Create(s, e, (IRange**)&POSITIVE_RATIONALS);

    end = NULL;
    CInteger32::New(32768, (IInteger32**)&end);
    CRange::Create(begin, end, (IRange**)&SIZE_RANGE);

    begin = NULL;
    end = NULL;
    CInteger32::New(0, (IInteger32**)&begin);
    CInteger32::New(960, (IInteger32**)&end);
    CRange::Create(begin, end, (IRange**)&FRAME_RATE_RANGE);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
