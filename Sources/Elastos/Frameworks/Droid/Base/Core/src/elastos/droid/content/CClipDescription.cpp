
#include "elastos/droid/content/CClipDescription.h"
#include "elastos/droid/content/CClipDescriptionHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(CClipDescription, Object, IClipDescription, IParcelable)

CAR_OBJECT_IMPL(CClipDescription)

CClipDescription::CClipDescription()
    : mLabel(NULL)
    , mMimeTypes(NULL)
{}

CClipDescription::~CClipDescription()
{}

ECode CClipDescription::GetLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label)
    *label = mLabel;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode CClipDescription::HasMimeType(
    /* [in] */ const String& mimeType,
    /* [out] */ Boolean* hasMimeType)
{
    VALIDATE_NOT_NULL(hasMimeType)
    const Int32 N = mMimeTypes->GetLength();
    AutoPtr<IClipDescriptionHelper> helper;
    FAIL_RETURN(CClipDescriptionHelper::AcquireSingleton((IClipDescriptionHelper**)&helper));
    Boolean isMatch = FALSE;

    for (Int32 i = 0; i < N; i++) {
        if ((helper->CompareMimeTypes((*mMimeTypes)[i], mimeType, &isMatch), isMatch)) {
            *hasMimeType = TRUE;
            return NOERROR;
        }
    }

    *hasMimeType = FALSE;
    return NOERROR;
}

ECode CClipDescription::FilterMimeTypes(
    /* [in] */ const String& mimeType,
    /* [out, callee] */ ArrayOf<String>** mimeTypes)
{
    VALIDATE_NOT_NULL(mimeTypes)
    *mimeTypes = NULL;

    List<String> array;
    const Int32 N = mMimeTypes->GetLength();
    AutoPtr<IClipDescriptionHelper> helper;
    FAIL_RETURN(CClipDescriptionHelper::AcquireSingleton((IClipDescriptionHelper**)&helper));
    Boolean isMatch = FALSE;

    for (Int32 i = 0; i < N; i++) {
        if ((helper->CompareMimeTypes((*mMimeTypes)[i], mimeType, &isMatch), isMatch)) {
            array.PushBack((*mMimeTypes)[i]);
        }
    }

    if (array.IsEmpty()) {
        return NOERROR;
    }

    Int32 size = array.GetSize();
    AutoPtr<ArrayOf<String> > rawArray = ArrayOf<String>::Alloc(size);
    List<String>::Iterator it = array.Begin();
    Int32 index = 0;

    for (; it != array.End(); ++it) {
        (*rawArray)[index] = *it;
        index++;
    }
    array.Clear();
    *mimeTypes = rawArray;
    REFCOUNT_ADD(*mimeTypes);
    return NOERROR;
}

ECode CClipDescription::GetMimeTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = mMimeTypes->GetLength();
    return NOERROR;
}

ECode CClipDescription::GetMimeType(
    /* [in] */ Int32 index,
    /* [out] */ String* mimeType)
{
    VALIDATE_NOT_NULL(mimeType)
    *mimeType = String(NULL);

    if (index >= mMimeTypes->GetLength()) return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;

    if (index < 0) {
        return NOERROR;
    }

    *mimeType = (*mMimeTypes)[index];
    return NOERROR;
}

ECode CClipDescription::Validate()
{
    if (NULL == mMimeTypes) {
        //throw new NullPointerException("null mime types");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (mMimeTypes->GetLength() <= 0) {
        //throw new IllegalArgumentException("must have at least 1 mime type");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    for (Int32 i = 0; i < mMimeTypes->GetLength(); i++) {
        if ((*mMimeTypes)[i].IsNull()) {
            //throw new NullPointerException("mime type at " + i + " is null");
            return E_NULL_POINTER_EXCEPTION;
        }
    }

    return NOERROR;
}

ECode CClipDescription::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder b(128);
    b.Append(String("ClipDescription { "));
    Boolean ret = FALSE;
    FAIL_RETURN(ToShortString((IStringBuilder*)&b, &ret));
    b.Append(String(" }"));
    return b.ToString(str);
}

ECode CClipDescription::ToShortString(
    /* [in] */ IStringBuilder* builder,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    Boolean first = TRUE;

    for (Int32 i = 0; i < mMimeTypes->GetLength(); i++) {
        if (!first) {
            builder->AppendChar(' ');
        }
        first = FALSE;
        builder->Append((*mMimeTypes)[i]);
    }
    if (NULL != mLabel) {
        if (!first) {
            builder->AppendChar(' ');
        }
        first = FALSE;
        builder->AppendChar('"');
        builder->Append(mLabel);
        builder->AppendChar('"');
    }

    *value = !first;
    return NOERROR;
}

ECode CClipDescription::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    mLabel = NULL;
    FAIL_RETURN(TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mLabel));
    FAIL_RETURN(CreateStringArray(source, (ArrayOf<String>**)&mMimeTypes));
    return NOERROR;
}

ECode CClipDescription::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    TextUtils::WriteToParcel(mLabel, dest);
    FAIL_RETURN(WriteStringArray(dest, mMimeTypes));
    return NOERROR;
}

ECode CClipDescription::constructor(
    /* [in] */ ICharSequence* label,
    /* [in] */ ArrayOf<String>* mimeTypes)
{
    if (NULL == mimeTypes) {
        //throw new NullPointerException("mimeTypes is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mLabel = label;
    mMimeTypes = mimeTypes;
    return NOERROR;
}

ECode CClipDescription::constructor(
    /* [in] */ IClipDescription* o)
{
    VALIDATE_NOT_NULL(o)
    FAIL_RETURN(o->GetLabel((ICharSequence**)&mLabel));
    Int32 count = 0;
    FAIL_RETURN(o->GetMimeTypeCount(&count));
    mMimeTypes = ArrayOf<String>::Alloc(count);
    for (Int32 i = 0; i< count; i++) {
        String tmp;
        FAIL_RETURN(o->GetMimeType(i, &tmp));
        (*mMimeTypes)[i] = tmp;
    }
    return NOERROR;
}

ECode CClipDescription::constructor()
{
    return NOERROR;
}

ECode CClipDescription::WriteStringArray(
    /* [in] */ IParcel* dest,
    /* [in] */ ArrayOf<String>* val)
{
    if (NULL != val) {
        Int32 N = val->GetLength();
        FAIL_RETURN(dest->WriteInt32(N));
        for (Int32 i = 0; i < N; i++) {
            FAIL_RETURN(dest->WriteString((*val)[i]));
        }
    }
    else {
        FAIL_RETURN(dest->WriteInt32(-1));
    }
    return NOERROR;
}

ECode CClipDescription::CreateStringArray(
    /* [in] */ IParcel* source,
    /* [out, callee] */ ArrayOf<String>** val)
{
    VALIDATE_NOT_NULL(val)
    *val = NULL;

    Int32 N = 0;
    FAIL_RETURN(source->ReadInt32(&N));
    if (N >= 0) {
        AutoPtr<ArrayOf<String> > tmpArray = ArrayOf<String>::Alloc(N);
        for (Int32 i = 0; i < N; i++) {
            String tmp;
            FAIL_RETURN(source->ReadString(&tmp));
            (*tmpArray)[i] = tmp;
        }
        *val = tmpArray;
        REFCOUNT_ADD(*val);
    }

    return NOERROR;
}

}
}
}

