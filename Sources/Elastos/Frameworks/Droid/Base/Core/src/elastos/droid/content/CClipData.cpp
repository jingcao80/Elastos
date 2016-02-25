
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CClipData.h"
#include "elastos/droid/content/CClipDescription.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CClipDataItem.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/net/Uri.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Net::Uri;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringBuilder;
using Elastos::Core::IStringBuilder;

namespace Elastos {
namespace Droid {
namespace Content {

static AutoPtr< ArrayOf<String> > InitMimeTypes(
    /* [in] */ Int32 type)
{
    AutoPtr< ArrayOf<String> > ss = ArrayOf<String>::Alloc(1);
    switch(type) {
        case 0:
            (*ss)[0] = IClipDescription::MIMETYPE_TEXT_PLAIN;
            break;
        case 1:
            (*ss)[0] = IClipDescription::MIMETYPE_TEXT_HTML;
            break;
        case 2:
            (*ss)[0] = IClipDescription::MIMETYPE_TEXT_URILIST;
            break;
        case 3:
            (*ss)[0] = IClipDescription::MIMETYPE_TEXT_INTENT;
            break;
        default:
            break;
    }
    return ss;
}

const AutoPtr< ArrayOf<String> > CClipData::MIMETYPES_TEXT_PLAIN = InitMimeTypes(0);
const AutoPtr< ArrayOf<String> > CClipData::MIMETYPES_TEXT_HTML = InitMimeTypes(1);
const AutoPtr< ArrayOf<String> > CClipData::MIMETYPES_TEXT_URILIST = InitMimeTypes(2);
const AutoPtr< ArrayOf<String> > CClipData::MIMETYPES_TEXT_INTENT = InitMimeTypes(3);

const String CClipData::TAG("CClipData");

CAR_INTERFACE_IMPL_2(CClipData, Object, IClipData, IParcelable)

CAR_OBJECT_IMPL(CClipData)

CClipData::CClipData()
{
}

CClipData::~CClipData()
{
    mItems.Clear();
}

ECode CClipData::NewPlainText(
    /* [in] */ ICharSequence* label,
    /* [in] */ ICharSequence* text,
    /* [out] */ IClipData** data)
{
    VALIDATE_NOT_NULL(data);
    AutoPtr<IClipDataItem> item;
    FAIL_RETURN(CClipDataItem::New(text, (IClipDataItem**)&item));
    return CClipData::New(label, MIMETYPES_TEXT_PLAIN, item, data);
}

ECode CClipData::NewHtmlText(
    /* [in] */ ICharSequence* label,
    /* [in] */ ICharSequence* text,
    /* [in] */ const String& htmlText,
    /* [out] */ IClipData** data)
{
    VALIDATE_NOT_NULL(data);
    AutoPtr<IClipDataItem> item;
    FAIL_RETURN(CClipDataItem::New(text, htmlText, (IClipDataItem**)&item));
    return CClipData::New(label, MIMETYPES_TEXT_HTML, item, data);
}

ECode CClipData::NewIntent(
    /* [in] */ ICharSequence* label,
    /* [in] */ IIntent* intent,
    /* [out] */ IClipData** data)
{
    VALIDATE_NOT_NULL(data);
    AutoPtr<IClipDataItem> item;
    FAIL_RETURN(CClipDataItem::New(intent, (IClipDataItem**)&item));
    return CClipData::New(label, MIMETYPES_TEXT_INTENT, item, data);
}

ECode CClipData::NewUri(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ ICharSequence* label,
    /* [in] */ IUri* uri,
    /* [out] */ IClipData** data)
{
    VALIDATE_NOT_NULL(data);
    AutoPtr<IClipDataItem> item;
    FAIL_RETURN(CClipDataItem::New(uri, (IClipDataItem**)&item));
    AutoPtr< ArrayOf<String> > mimeTypes;
    String scheme;
    uri->GetScheme(&scheme);
    if (String("content").Equals(scheme)) {
        String realType;
        resolver->GetType(uri, &realType);
        resolver->GetStreamTypes(uri, String("*/*"), (ArrayOf<String>**)&mimeTypes);
        if (mimeTypes == NULL) {
            if (!realType.IsNull()) {
                mimeTypes = ArrayOf<String>::Alloc(2);
                (*mimeTypes)[0] = realType;
                (*mimeTypes)[0] = IClipDescription::MIMETYPE_TEXT_URILIST;
            }
        }
        else {
            AutoPtr< ArrayOf<String> > tmp = ArrayOf<String>::Alloc(
                    mimeTypes->GetLength() + (realType != NULL ? 2 : 1));
            Int32 i = 0;
            if (realType != NULL) {
                (*tmp)[0] = realType;
                i++;
            }
            tmp->Copy(i, mimeTypes, mimeTypes->GetLength());
            (*tmp)[i + mimeTypes->GetLength()] = IClipDescription::MIMETYPE_TEXT_URILIST;
            mimeTypes = tmp;
        }
    }
    if (mimeTypes == NULL) {
        mimeTypes = MIMETYPES_TEXT_URILIST;
    }

    return CClipData::New(label, mimeTypes, item, data);
}

ECode CClipData::NewRawUri(
    /* [in] */ ICharSequence* label,
    /* [in] */ IUri* uri,
    /* [out] */ IClipData** data)
{
    VALIDATE_NOT_NULL(data);
    AutoPtr<IClipDataItem> item;
    FAIL_RETURN(CClipDataItem::New(uri, (IClipDataItem**)&item));
    return CClipData::New(label, MIMETYPES_TEXT_URILIST, item, data);
}

ECode CClipData::GetDescription(
    /* [out] */ IClipDescription** description)
{
    VALIDATE_NOT_NULL(description);
    *description = mClipDescription;
    REFCOUNT_ADD(*description);
    return NOERROR;
}

ECode CClipData::AddItem(
    /* [in] */ IClipDataItem* item)
{
    if (item == NULL) {
        Slogger::E(TAG, "item is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mItems.PushBack(item);
    return NOERROR;
}

ECode CClipData::GetIcon(
    /* [out] */ IBitmap** icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = mIcon;
    REFCOUNT_ADD(*icon);
    return NOERROR;
}

ECode CClipData::GetItemCount(
    /* [out] */ Int32* itemCount)
{
    VALIDATE_NOT_NULL(itemCount);
    *itemCount = mItems.GetSize();
    return NOERROR;
}

ECode CClipData::GetItemAt(
    /* [in] */ Int32 index,
    /* [out] */ IClipDataItem** item)
{
    VALIDATE_NOT_NULL(item);
    *item = mItems[index];
    REFCOUNT_ADD(*item);
    return NOERROR;
}

ECode CClipData::PrepareToLeaveProcess()
{
    Int32 size = mItems.GetSize();
    AutoPtr<IClipDataItem> item;
    for (Int32 i = 0; i < size; i++) {
        item = mItems[i];
        AutoPtr<IIntent> intent;
        item->GetIntent((IIntent**)&intent);
        if (intent != NULL) {
            intent->PrepareToLeaveProcess();
        }

        AutoPtr<IUri> uri;
        item->GetUri((IUri**)&uri);
        assert(0 && "TODO");
        if (uri != NULL /* && CStrictMode::VmFileUriExposureEnabled() */) {
            uri->CheckFileUriExposed(String("ClipData.Item.getUri()"));
        }
    }
    return NOERROR;
}

ECode CClipData::FixUris(
    /* [in] */ Int32 contentUserHint)
{
    Int32 size = mItems.GetSize();
    AutoPtr<IClipDataItem> item;
    for (Int32 i = 0; i < size; i++) {
        item = mItems[i];
        AutoPtr<IIntent> intent;
        item->GetIntent((IIntent**)&intent);
        if (intent != NULL) {
            intent->FixUris(contentUserHint);
        }

        AutoPtr<IUri> uri;
        item->GetUri((IUri**)&uri);
        if (uri != NULL) {
            AutoPtr<IUri> newUri = ContentProvider::MaybeAddUserId(uri, contentUserHint);
            assert(0 && "TODO");
            // item->SetUri(newUri);
        }
    }
    return NOERROR;
}

ECode CClipData::FixUrisLight(
    /* [in] */ Int32 contentUserHint)
{
    Int32 size = mItems.GetSize();
    AutoPtr<IClipDataItem> item;
    for (Int32 i = 0; i < size; i++) {
        item = mItems[i];
        AutoPtr<IIntent> intent;
        item->GetIntent((IIntent**)&intent);
        if (intent != NULL) {
            AutoPtr<IUri> data;
            intent->GetData((IUri**)&data);
            if (data != NULL) {
                AutoPtr<IUri> newUri = ContentProvider::MaybeAddUserId(data, contentUserHint);
                intent->SetData(data);
            }
        }

        AutoPtr<IUri> uri;
        item->GetUri((IUri**)&uri);
        if (uri != NULL) {
            AutoPtr<IUri> newUri = ContentProvider::MaybeAddUserId(uri, contentUserHint);
            assert(0 && "TODO");
            // item->SetUri(newUri);
        }
    }
    return NOERROR;
}

ECode CClipData::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<StringBuilder> b = new StringBuilder(128);
    b->Append("ClipData { ");
    ToShortString((IStringBuilder*)b.Get());
    b->Append(" }");
    return b->ToString(str);
}

ECode CClipData::ToShortString(
    /* [in] */ IStringBuilder* sb)
{
    Boolean first = FALSE;
    if (mClipDescription != NULL) {
        Boolean result;
        mClipDescription->ToShortString(sb, &result);
        first = !result;
    }
    else {
        first = TRUE;
    }

    if (mIcon != NULL) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->Append(String("I:"));
        Int32 width = 0;
        mIcon->GetWidth(&width);
        sb->Append(width);
        sb->AppendChar('x');
        Int32 height = 0;
        mIcon->GetHeight(&height);
        sb->Append(height);
    }
    List< AutoPtr<IClipDataItem> >::Iterator it;
    for (it = mItems.Begin(); it != mItems.End(); ++it) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->AppendChar('{');
        (*it)->ToShortString(sb);
        sb->AppendChar('}');
    }

    return NOERROR;
}

ECode CClipData::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    AutoPtr<IParcelable> parcelable = (IParcelable*)mClipDescription->Probe(EIID_IParcelable);
    FAIL_RETURN(parcelable->WriteToParcel(dest));

    if (mIcon != NULL) {
        dest->WriteInt32(1);
        parcelable = (IParcelable*)mIcon->Probe(EIID_IParcelable);
        FAIL_RETURN(parcelable->WriteToParcel(dest));
    }
    else {
        dest->WriteInt32(0);
    }

    Int32 N = mItems.GetSize();
    dest->WriteInt32(N);
    List< AutoPtr<IClipDataItem> >::Iterator it;
    for (it = mItems.Begin(); it != mItems.End(); ++it) {
        AutoPtr<CClipDataItem> item = (CClipDataItem*)(*it).Get();
        TextUtils::WriteToParcel(item->mText, dest);
        dest->WriteString(item->mHtmlText);
        if (item->mIntent != NULL) {
            dest->WriteInt32(1);
            parcelable = (IParcelable*)item->mIntent->Probe(EIID_IParcelable);
            FAIL_RETURN(parcelable->WriteToParcel(dest));
        }
        else {
            dest->WriteInt32(0);
        }
        if (item->mUri != NULL) {
            dest->WriteInt32(1);
            assert(0 && "TODO");
            // Uri::WriteToParcel(dest, item->mUri);
        }
        else {
            dest->WriteInt32(0);
        }
    }

    return NOERROR;
}

ECode CClipData::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    ASSERT_SUCCEEDED(CClipDescription::New((IClipDescription**)&mClipDescription));
    AutoPtr<IParcelable> p = (IParcelable*)mClipDescription->Probe(EIID_IParcelable);
    p->ReadFromParcel(source);
    Int32 value;
    source->ReadInt32(&value);
    if (value != 0) {
        ASSERT_SUCCEEDED(CBitmap::New((IBitmap**)&mIcon));
        p = (IParcelable*)mIcon->Probe(EIID_IParcelable);
        p->ReadFromParcel(source);
    }
    else {
        mIcon = NULL;
    }
    Int32 N;
    source->ReadInt32(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ICharSequence> text;
        FAIL_RETURN(TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&text));
        String htmlText;
        source->ReadString(&htmlText);
        source->ReadInt32(&value);
        AutoPtr<IIntent> intent;
        if (value != 0) {
            ASSERT_SUCCEEDED(CIntent::New((IIntent**)&intent));
            p = (IParcelable*)intent->Probe(EIID_IParcelable);
            p->ReadFromParcel(source);
        }
        source->ReadInt32(&value);
        AutoPtr<IUri> uri;
        if (value != 0) {
            assert(0 && "TODO");
            // Uri::ReadFromParcel(source, (IUri**)&uri);
        }
        AutoPtr<IClipDataItem> item;
        ASSERT_SUCCEEDED(CClipDataItem::New(text, htmlText, intent, uri, (IClipDataItem**)&item));
        mItems.PushBack(item);
    }

    return NOERROR;
}

ECode CClipData::constructor(
    /* [in] */ ICharSequence* label,
    /* [in] */ ArrayOf<String>* mimeTypes,
    /* [in] */ IClipDataItem* item)
{
    FAIL_RETURN(CClipDescription::New(label, mimeTypes, (IClipDescription**)&mClipDescription));
    if (item == NULL) {
        Slogger::E(TAG, "item is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mItems.PushBack(item);
    return NOERROR;
}

ECode CClipData::constructor(
    /* [in] */ IClipDescription* description,
    /* [in] */ IClipDataItem* item)
{
    mClipDescription = description;
    if (item == NULL) {
        Slogger::E(TAG, "item is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mItems.PushBack(item);
    return NOERROR;
}

ECode CClipData::constructor(
    /* [in] */ IClipData* _other)
{
    AutoPtr<CClipData> other = (CClipData*)_other;
    mClipDescription = other->mClipDescription;
    mIcon = other->mIcon;
    mItems = other->mItems;
    return NOERROR;
}

ECode CClipData::constructor()
{
    return NOERROR;
}

} // Content
} // Droid
} // Elastos
