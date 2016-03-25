
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/Intent.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CClipDataItem.h"
#include "elastos/droid/content/CClipData.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/ContentProvider.h"
#include "elastos/droid/content/CClipDataHelper.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Content::CClipDataItem;
using Elastos::Droid::Content::CClipData;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Internal::Utility::XmlUtils;

using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IInteger16;
using Elastos::Core::IFloat;
using Elastos::Core::IDouble;
using Elastos::Core::IByte;
using Elastos::Core::IChar32;
using Elastos::Core::ICloneable;
using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::Objects;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Content {

const String Intent::ATTR_ACTION("action");
const String Intent::TAG_CATEGORIES("categories");
const String Intent::ATTR_CATEGORY("category");
const String Intent::TAG_EXTRA("extra");
const String Intent::ATTR_TYPE("type");
const String Intent::ATTR_COMPONENT("component");
const String Intent::ATTR_DATA("data");
const String Intent::ATTR_FLAGS("flags");

const String Intent::TAG("Intent");

CAR_INTERFACE_IMPL_3(Intent, Object, IIntent, IParcelable, ICloneable)

Intent::Intent()
    : mFlags(0)
    , mContentUserHint(UserHandle::USER_CURRENT)
{}

Intent::~Intent()
{
    mExtras = NULL;
}

ECode Intent::constructor()
{
    return NOERROR;
}

ECode Intent::constructor(
    /* [in] */ IIntent* intent)
{
    if (intent == NULL) {
        return constructor();
    }

    intent->GetAction(&mAction);
    intent->GetData((IUri**)&mData);
    intent->GetType(&mType);
    intent->GetPackage(&mPackage);
    intent->GetFlags(&mFlags);
    AutoPtr<IComponentName> componentName;
    intent->GetComponent((IComponentName**)&componentName);
    if (componentName != NULL) {
        String packageName, className;
        componentName->GetPackageName(&packageName);
        componentName->GetClassName(&className);
        CComponentName::New(packageName, className, (IComponentName**)&mComponent);
    }
    AutoPtr< ArrayOf<String> > categories;
    intent->GetCategories((ArrayOf<String>**)&categories);
    if (categories != NULL) {
        assert(mCategories == NULL);
        mCategories = new HashSet<String>();
        for (Int32 i = 0; i < categories->GetLength(); ++i) {
            mCategories->Insert((*categories)[i]);
        }
    }
    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    if (extras != NULL) {
        CBundle::New(extras, (IBundle**)&mExtras);
    }

    AutoPtr<IRect> sourceBounds;
    intent->GetSourceBounds((IRect**)&sourceBounds);
    if (sourceBounds != NULL) {
        CRect::New(sourceBounds, (IRect**)&mSourceBounds);
    }
    AutoPtr<IIntent> selector;
    intent->GetSelector((IIntent**)&selector);
    if (selector != NULL) {
        CIntent::New(selector, (IIntent**)&mSelector);
    }

    AutoPtr<IClipData> clipData;
    intent->GetClipData((IClipData**)&clipData);
    if (clipData != NULL) {
        CClipData::New(clipData, (IClipData**)&mClipData);
    }

    return NOERROR;
}

ECode Intent::constructor(
    /* [in] */ IIntent* intent,
    /* [in] */ Boolean all)
{
    if (intent == NULL) {
        return constructor();
    }

    intent->GetAction(&mAction);
    intent->GetData((IUri**)&mData);
    intent->GetType(&mType);
    intent->GetPackage(&mPackage);
    AutoPtr<IComponentName> componentName;
    intent->GetComponent((IComponentName**)&componentName);
    if (componentName != NULL) {
        String packageName, className;
        componentName->GetPackageName(&packageName);
        componentName->GetClassName(&className);
        CComponentName::New(packageName, className, (IComponentName**)&mComponent);
    }
    AutoPtr< ArrayOf<String> > categories;
    intent->GetCategories((ArrayOf<String>**)&categories);
    if (categories != NULL) {
        assert(mCategories == NULL);
        mCategories = new HashSet<String>();
        for (Int32 i = 0; i < categories->GetLength(); ++i) {
            mCategories->Insert((*categories)[i]);
        }
    }

    return NOERROR;
}

ECode Intent::constructor(
    /* [in] */ const String& action)
{
    return SetAction(action);
}

ECode Intent::constructor(
    /* [in] */ const String& action,
    /* [in] */ IUri* uri)
{
    FAIL_RETURN(SetAction(action));
    mData = uri;
    return NOERROR;
}

ECode Intent::constructor(
    /* [in] */ IContext* packageContext,
    /* [in] */ const ClassID& clsId)
{
    AutoPtr<IClassInfo> info = TransformClassInfo(clsId);
    return CComponentName::New(packageContext, info, (IComponentName**)&mComponent);
}

ECode Intent::constructor(
    /* [in] */ const String& action,
    /* [in] */ IUri* uri,
    /* [in] */ IContext* packageContext,
    /* [in] */ IClassInfo* cls)
{
    FAIL_RETURN(SetAction(action));
    mData = uri;
    return CComponentName::New(packageContext, cls, (IComponentName**)&mComponent);
}

AutoPtr<IIntent> Intent::CreateChooser(
    /* [in] */ IIntent* target,
    /* [in] */ ICharSequence* title)
{
    AutoPtr<CIntent> intent;
    ASSERT_SUCCEEDED(CIntent::NewByFriend(IIntent::ACTION_CHOOSER, (CIntent**)&intent));
    AutoPtr<IParcelable> parcelable = (IParcelable*)target->Probe(EIID_IParcelable);
    intent->PutExtra(IIntent::EXTRA_INTENT, parcelable);
    if (title != NULL) {
        intent->PutExtra(IIntent::EXTRA_TITLE, title);
    }

    // Migrate any clip data and flags from target.
    Int32 flags;
    target->GetFlags(&flags);
    Int32 permFlags = flags &
            (IIntent::FLAG_GRANT_READ_URI_PERMISSION
                | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION
                | IIntent::FLAG_GRANT_PERSISTABLE_URI_PERMISSION
                | IIntent::FLAG_GRANT_PREFIX_URI_PERMISSION);
    if (permFlags != 0) {
        AutoPtr<IClipData> targetClipData;
        target->GetClipData((IClipData**)&targetClipData);
        AutoPtr<IUri> data;
        target->GetData((IUri**)&data);
        if (targetClipData == NULL && data != NULL) {
            // AutoPtr<CClipDataItem> item;
            // CClipDataItem::NewByFriend(data, (CClipDataItem**)&item);
            assert(0);
            AutoPtr< ArrayOf<String> > mimeTypes;
            String type;
            target->GetType(&type);
            if (!type.IsNull()) {
                mimeTypes = ArrayOf<String>::Alloc(1);
                (*mimeTypes)[0] = type;
            }
            else {
                mimeTypes = ArrayOf<String>::Alloc(0);
            }
            // CClipData::NewByFriend(NULL, mimeTypes, (IClipDataItem*)item,
            //         (CClipData**)(IClipData**)&targetClipData);
            assert(0);
        }
        if (targetClipData != NULL) {
            intent->SetClipData(targetClipData);
            intent->AddFlags(permFlags);
        }
    }

    return intent;
}

ECode Intent::Clone(
    /* [out] */ IInterface** obj)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);

    CloneImpl(intent);
    *obj = intent.Get();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Intent::CloneImpl(
    /* [in] */ IIntent* intentObj)
{
    VALIDATE_NOT_NULL(intentObj);
    Intent* intent = (Intent*)intentObj;

    intent->mAction = this->mAction;
    intent->mData = this->mData;
    intent->mType = this->mType;
    intent->mPackage = this->mPackage;
    intent->mComponent = this->mComponent;
    if (mCategories != NULL) {
        intent->mCategories = new HashSet<String>(mCategories->GetSize());
        HashSet<String>::Iterator it;
        for (it = mCategories->Begin(); it != mCategories->End(); ++it) {
            intent->mCategories->Insert(*it);
        }
    }
    return NOERROR;
}

ECode Intent::CloneFilter(
    /* [out] */ IIntent** intent)
{
    return CIntent::New(this, FALSE, intent);
}

AutoPtr<IIntent> Intent::MakeMainActivity(
    /* [in] */ IComponentName* mainActivity)
{
    AutoPtr<CIntent> intent;
    ASSERT_SUCCEEDED(CIntent::NewByFriend(IIntent::ACTION_MAIN, (CIntent**)&intent));
    intent->SetComponent(mainActivity);
    intent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    return (IIntent*)intent.Get();
}

AutoPtr<IIntent> Intent::MakeMainSelectorActivity(
    /* [in] */ const String& selectorAction,
    /* [in] */ const String& selectorCategory)
{
    AutoPtr<CIntent> intent;
    ASSERT_SUCCEEDED(CIntent::NewByFriend(IIntent::ACTION_MAIN, (CIntent**)&intent));
    intent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    AutoPtr<CIntent> selector;
    ASSERT_SUCCEEDED(CIntent::NewByFriend((CIntent**)&selector));
    selector->SetAction(selectorAction);
    selector->AddCategory(selectorCategory);
    intent->SetSelector(selector);
    return intent;
}

AutoPtr<IIntent> Intent::MakeRestartActivityTask(
    /* [in] */ IComponentName* mainActivity)
{
    AutoPtr<IIntent> intent = MakeMainActivity(mainActivity);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_CLEAR_TASK);
    return intent;
}

ECode Intent::GetIntent(
    /* [in] */ const String& uri,
    /* [out] */ IIntent** intent)
{
    return ParseUri(uri, 0, intent);
}

ECode Intent::ParseUri(
    /* [in] */ const String& uri,
    /* [in] */ Int32 flags,
    /* [out] */ IIntent** _intent)
{
    VALIDATE_NOT_NULL(_intent);

    Int32 i = 0;
    // try {
    // Validate intent scheme for if requested.
    if ((flags & IIntent::URI_INTENT_SCHEME) != 0) {
        if (!uri.StartWith("intent:")) {
            AutoPtr<CIntent> cintent;
            ASSERT_SUCCEEDED(CIntent::NewByFriend(IIntent::ACTION_VIEW, (CIntent**)&cintent));
            *_intent = cintent;
            REFCOUNT_ADD(*_intent);
            // try {
            AutoPtr<IUri> data;
            if (FAILED(Uri::Parse(uri, (IUri**)&data))) {
                return E_URI_SYNTAX_EXCEPTION;
            }
            (*_intent)->SetData(data);
            // } catch (IllegalArgumentException e) {
            //     throw new URISyntaxException(uri, e.getMessage());
            // }
            return NOERROR;
        }
    }

    // simple case
    i = uri.LastIndexOf("#");
    if (i == -1) {
        AutoPtr<IUri> data;
        FAIL_RETURN(Uri::Parse(uri, (IUri**)&data));
        AutoPtr<CIntent> cintent;
        FAIL_RETURN(CIntent::NewByFriend(IIntent::ACTION_VIEW, data, (CIntent**)&cintent));
        *_intent = cintent;
        REFCOUNT_ADD(*_intent);
        return NOERROR;
    }

    // old format Intent URI
    if (!uri.Substring(i).StartWith("#Intent;")) {
        return GetIntentOld(uri, _intent);
    }

    // new format
    AutoPtr<CIntent> intent;
    ASSERT_SUCCEEDED(CIntent::NewByFriend(IIntent::ACTION_VIEW, (CIntent**)&intent));
    AutoPtr<CIntent> baseIntent = intent;

    // fetch data part, if present
    String data = i >= 0 ? uri.Substring(0, i) : String(NULL);
    String scheme(NULL);
    i += String("#Intent;").GetLength();

    // loop over contents of Intent, all name=value;
    String sub = uri.Substring(i);
    while (!sub.StartWith("end")) {
        Int32 eq = uri.IndexOf('=', i);
        if (eq < 0) eq = i - 1;
        Int32 semi = uri.IndexOf(';', i);
        String value = String("");
        if (eq < semi) {
            FAIL_RETURN(Uri::Decode(uri.Substring(eq + 1, semi), &value));
        }

        // action
        if (sub.StartWith("action=")) {
            intent->SetAction(value);
        }
        // categories
        else if (sub.StartWith("category=")) {
            intent->AddCategory(value);
        }
        // type
        else if (sub.StartWith("type=")) {
            intent->mType = value;
        }
        // launch flags
        else if (sub.StartWith("launchFlags=")) {
            intent->mFlags = StringUtils::ParseInt32(value);
        }
        // package
        else if (sub.StartWith("package=")) {
            intent->mPackage = value;
        }
        // component
        else if (sub.StartWith("component=")) {
            intent->mComponent = NULL;
            CComponentName::UnflattenFromString(value, (IComponentName**)&intent->mComponent);
        }
        // scheme
        else if (sub.StartWith("scheme=")) {
            scheme = value;
        }
        // source bounds
        else if (sub.StartWith("sourceBounds=")) {
            // AutoPtr<IRectHelper> rectHelper;
            // CRectHelper::AcquireSingleton((IRectHelper**)&rectHelper);
            // AutoPtr<IRect> rect;
            // intent->mSourceBounds = NULL;
            // FAIL_RETURN(rectHelper->UnflattenFromString(value, (IRect**)&intent->mSourceBounds));
            assert(0);
        }
        // selector
        else if (semi == (i + 3) && sub.StartWith("SEL")) {
            intent = NULL;
            CIntent::NewByFriend((CIntent**)&intent);
        }
        // extra
        else {
            String key;
            FAIL_RETURN(Uri::Decode(uri.Substring(i + 2, eq), &key));
            // create Bundle if it doesn't already exist
            if (intent->mExtras == NULL) {
                CBundle::New((IBundle**)&intent->mExtras);
            }
            AutoPtr<IBundle> b = intent->mExtras;
            // add EXTRA
            if (sub.StartWith("S.")) {
                b->PutString(key, value);
            }
            else if (sub.StartWith("B.")) {
                b->PutBoolean(key, value.EqualsIgnoreCase("TRUE"));
            }
            else if (sub.StartWith("b.")) {
                b->PutByte(key, (Byte)StringUtils::ParseInt32(value));
            }
            else if (sub.StartWith("c.")) {
                b->PutChar(key, value.GetChar(0));
            }
            else if (sub.StartWith("d.")) {
                b->PutDouble(key, StringUtils::ParseDouble(value));
            }
            else if (sub.StartWith("f.")) {
                b->PutFloat(key, StringUtils::ParseFloat(value));
            }
            else if (sub.StartWith("i.")) {
                b->PutInt32(key, StringUtils::ParseInt32(value));
            }
            else if (sub.StartWith("l.")) {
                b->PutInt64(key, StringUtils::ParseInt64(value));
            }
            else if (sub.StartWith("s.")) {
                b->PutInt16(key, StringUtils::ParseInt16(value));
            }
            else {
                // throw new URISyntaxException(uri, "unknown EXTRA type", i);
                Slogger::E(TAG, "unknown EXTRA type %d", i);
                return E_URI_SYNTAX_EXCEPTION;
            }
        }

        // move to the next item
        i = semi + 1;
        sub = uri.Substring(i);
    }

    if (intent != baseIntent) {
        // The Intent had a selector; fix it up.
        baseIntent->SetSelector(intent);
        intent = baseIntent;
    }

    if (!data.IsNull()) {
        if (data.StartWith("intent:")) {
            data = data.Substring(7);
            if (!scheme.IsNull()) {
                data = scheme + ":" + data;
            }
        }

        if (!data.IsEmpty()) {
            // try {
            intent->mData = NULL;
            if (FAILED(Uri::Parse(data, (IUri**)&intent->mData))) {
                return E_URI_SYNTAX_EXCEPTION;
            }
            // } catch (IllegalArgumentException e) {
            //     throw new URISyntaxException(uri, e.getMessage());
            // }
        }
    }

    *_intent = (IIntent*)intent.Get();
    REFCOUNT_ADD(*_intent);

//    } catch (IndexOutOfBoundsException e) {
//        throw new URISyntaxException(uri, "illegal Intent URI format", i);
//    }
    return NOERROR;
}

ECode Intent::GetIntentOld(
    /* [in] */ const String& uri,
    /* [out] */ IIntent** _intent)
{
    VALIDATE_NOT_NULL(_intent);

    AutoPtr<CIntent> intent;

    Int32 i = uri.LastIndexOf('#');
    if (i >= 0) {
        String action = String(NULL);
        Int32 intentFragmentStart = i;
        Boolean isIntentFragment = FALSE;

        i++;

        if (uri.RegionMatches(i, String("action("), 0, 7)) {
            isIntentFragment = TRUE;
            i += 7;
            Int32 j = uri.IndexOf(')', i);
            action = uri.Substring(i, j);
            i = j + 1;
        }

        ASSERT_SUCCEEDED(CIntent::NewByFriend(action, (CIntent**)&intent));

        if (uri.RegionMatches(i, String("categories("), 0, 11)) {
            isIntentFragment = TRUE;
            i += 11;
            Int32 j = uri.IndexOf(')', i);
            while (i < j) {
                Int32 sep = uri.IndexOf('!', i);
                if (sep < 0 || sep > j) sep = j;
                if (i < sep) {
                    intent->AddCategory(uri.Substring(i, sep));
                }
                i = sep + 1;
            }
            i = j + 1;
        }

        if (uri.RegionMatches(i, String("type("), 0, 5)) {
            isIntentFragment = TRUE;
            i += 5;
            Int32 j = uri.IndexOf(')', i);
            intent->mType = uri.Substring(i, j);
            i = j + 1;
        }

        if (uri.RegionMatches(i, String("launchFlags("), 0, 12)) {
            isIntentFragment = TRUE;
            i += 12;
            Int32 j = uri.IndexOf(')', i);
            intent->mFlags = StringUtils::ParseInt32(uri.Substring(i, j));
            i = j + 1;
        }

        if (uri.RegionMatches(i, String("component("), 0, 10)) {
            isIntentFragment = TRUE;
            i += 10;
            Int32 j = uri.IndexOf(')', i);
            Int32 sep = uri.IndexOf('!', i);
            if (sep >= 0 && sep < j) {
                String pkg = uri.Substring(i, sep);
                String cls = uri.Substring(sep + 1, j);
                AutoPtr<CComponentName> component;
                CComponentName::NewByFriend(pkg, cls, (CComponentName**)&component);
                intent->mComponent = (IComponentName*)component.Get();
            }
            i = j + 1;
        }

        if (uri.RegionMatches(i, String("extras("), 0, 7)) {
            isIntentFragment = TRUE;
            i += 7;
            Int32 closeParen = uri.IndexOf(')', i);
            if (closeParen == -1) {
                // throw new URISyntaxException(uri,"EXTRA missing trailing ')'", i);
                Slogger::E(TAG, "EXTRA missing trailinge %d", i);
                return E_URI_SYNTAX_EXCEPTION;
            }

            while (i < closeParen) {
                // fetch the key value
                Int32 j = uri.IndexOf('=', i);
                if (j <= i + 1 || i >= closeParen) {
                    // throw new URISyntaxException(uri, "EXTRA missing '='", i);
                    Slogger::E(TAG, "EXTRA missing '=' %d", i);
                    return E_URI_SYNTAX_EXCEPTION;
                }
                Char32 type = uri.GetChar(i);
                i++;
                String key = uri.Substring(i, j);
                i = j + 1;

                // get type-value
                j = uri.IndexOf('!', i);
                if (j == -1 || j >= closeParen) j = closeParen;
                if (i >= j) {
                    // throw new URISyntaxException(uri, "EXTRA missing '!'", i);
                    Slogger::E(TAG, "EXTRA missing '!' %d", i);
                    return E_URI_SYNTAX_EXCEPTION;
                }
                String value = uri.Substring(i, j);
                i = j;

                // create Bundle if it doesn't already exist
                if (intent->mExtras == NULL) {
                    CBundle::New((IBundle**)&intent->mExtras);
                }

                String decodeS;
                // add item to bundle
                // try {
                switch (type) {
                    case 'S':
                        if (FAILED(Uri::Decode(value, &decodeS))) {
                            return E_URI_SYNTAX_EXCEPTION;
                        }
                        intent->mExtras->PutString(key, decodeS);
                        break;
                    case 'B':
                        intent->mExtras->PutBoolean(key, value.EqualsIgnoreCase("TRUE"));
                        break;
                    case 'b':
                        intent->mExtras->PutByte(key, (Byte)StringUtils::ParseInt32(value));
                        break;
                    case 'c':
                        if (FAILED(Uri::Decode(value, &decodeS))) {
                            return E_URI_SYNTAX_EXCEPTION;
                        }
                        intent->mExtras->PutChar(key, decodeS.GetChar(0));
                        break;
                    case 'd':
                        intent->mExtras->PutDouble(key, StringUtils::ParseDouble(value));
                        break;
                    case 'f':
                        intent->mExtras->PutFloat(key, StringUtils::ParseFloat(value));
                        break;
                    case 'i':
                        intent->mExtras->PutInt32(key, StringUtils::ParseInt32(value));
                        break;
                    case 'l':
                        intent->mExtras->PutInt64(key, StringUtils::ParseInt64(value));
                        break;
                    case 's':
                        intent->mExtras->PutInt16(key, StringUtils::ParseInt16(value));
                        break;
                    default:
                        //throw new URISyntaxException(uri, "EXTRA has unknown type", i);
                        Slogger::E(TAG, "EXTRA has unknown type %d", i);
                        return E_URI_SYNTAX_EXCEPTION;
                }
                // } catch (NumberFormatException e) {
                //     throw new URISyntaxException(uri, "EXTRA value can't be parsed", i);
                // }

                Char32 ch = uri.GetChar(i);
                if (ch == ')') break;
                if (ch != '!') {
                    //throw new URISyntaxException(uri, "EXTRA missing '!'", i);
                    Slogger::E(TAG, "EXTRA missing '!' %d", i);
                    return E_URI_SYNTAX_EXCEPTION;
                }
                i++;
            }
        }

        intent->mData = NULL;
        if (isIntentFragment) {
            Uri::Parse(uri.Substring(0, intentFragmentStart), (IUri**)&intent->mData);
        }
        else {
            Uri::Parse(uri, (IUri**)&intent->mData);
        }

        if (intent->mAction.IsNull()) {
            // By default, if no action is specified, then use VIEW.
            intent->mAction = IIntent::ACTION_VIEW;
        }
    }
    else {
        AutoPtr<IUri> data;
        Uri::Parse(IIntent::ACTION_VIEW, (IUri**)&data);
        CIntent::NewByFriend(IIntent::ACTION_VIEW, data, (CIntent**)&intent);
    }

    *_intent = (IIntent*)intent.Get();
    REFCOUNT_ADD(*_intent);
    return NOERROR;
}

ECode Intent::GetAction(
    /* [out] */ String* action)
{
    VALIDATE_NOT_NULL(action)
    *action = mAction;
    return NOERROR;
}

ECode Intent::GetData(
    /* [out] */ IUri** data)
{
    VALIDATE_NOT_NULL(data)
    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode Intent::GetDataString(
    /* [out] */ String* dataString)
{
    VALIDATE_NOT_NULL(dataString)
    if (mData != NULL) {
        *dataString = Object::ToString(mData);
        return NOERROR;
    }
    *dataString = NULL;
    return NOERROR;
}

ECode Intent::GetScheme(
    /* [out] */ String* scheme)
{
    VALIDATE_NOT_NULL(scheme)
    if (mData != NULL) {
        return mData->GetScheme(scheme);
    }
    *scheme = NULL;
    return NOERROR;
}

ECode Intent::GetType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode Intent::ResolveType(
    /* [in] */ IContext* context,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    AutoPtr<IContentResolver> resolver;
    ASSERT_SUCCEEDED(context->GetContentResolver((IContentResolver**)&resolver));
    return ResolveType(resolver, type);
}

ECode Intent::ResolveType(
    /* [in] */ IContentResolver* resolver,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    if (!mType.IsNull()) {
        *type = mType;
        return NOERROR;
    }
    if (mData != NULL) {
        String scheme;
        mData->GetScheme(&scheme);
        if (scheme.Equals("content")) {
            return resolver->GetType(mData, type);
        }
    }

    *type = NULL;
    return NOERROR;
}

ECode Intent::ResolveTypeIfNeeded(
    /* [in] */ IContentResolver* resolver,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    if (mComponent != NULL) {
        *type = mType;
        return NOERROR;
    }

    return ResolveType(resolver, type);
}

ECode Intent::HasCategory(
    /* [in] */ const String& category,
    /* [out] */ Boolean* hasCategory)
{
    VALIDATE_NOT_NULL(hasCategory)
    *hasCategory = FALSE;

    if (mCategories != NULL) {
        HashSet<String>::Iterator it = mCategories->Find(category);
        if (it != mCategories->End()) *hasCategory = TRUE;
    }
    return NOERROR;
}

ECode Intent::GetCategories(
    /* [out, callee] */ ArrayOf<String>** _categories)
{
    VALIDATE_NOT_NULL(_categories)
    *_categories = NULL;

    if (mCategories != NULL) {
        AutoPtr< ArrayOf<String> > categories = ArrayOf<String>::Alloc(mCategories->GetSize());
        if (categories == NULL) {
            return E_OUT_OF_MEMORY;
        }
        HashSet<String>::Iterator it;
        Int32 idx = 0;
        for (it = mCategories->Begin(); it != mCategories->End(); ++it, ++idx) {
            (*categories)[idx] = *it;
        }

        *_categories = categories;
        REFCOUNT_ADD(*_categories);
        return NOERROR;
    }

    return NOERROR;
}

ECode Intent::GetSelector(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mSelector;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode Intent::GetClipData(
    /* [out] */ IClipData** clipData)
{
    VALIDATE_NOT_NULL(clipData)
    *clipData = mClipData;
    REFCOUNT_ADD(*clipData);
    return NOERROR;
}

ECode Intent::GetContentUserHint(
        /* [out] */ Int32* hint)
{
    VALIDATE_NOT_NULL(hint)
    *hint = mContentUserHint;
    return NOERROR;
}

ECode Intent::SetExtrasClassLoader(
    /* [in] */ IClassLoader* loader)
{
    if (mExtras != NULL) {
        mExtras->SetClassLoader(loader);
    }
    return NOERROR;
}

ECode Intent::HasExtra(
    /* [in] */ const String& name,
    /* [out] */ Boolean* hasExtra)
{
    VALIDATE_NOT_NULL(hasExtra)
    if (mExtras != NULL) {
        return mExtras->ContainsKey(name, hasExtra);
    }
    else {
        *hasExtra = FALSE;
    }

    return NOERROR;
}

ECode Intent::HasFileDescriptors(
    /* [out] */ Boolean* hasFileDescriptors)
{
    VALIDATE_NOT_NULL(hasFileDescriptors)
    if (mExtras != NULL) {
        mExtras->HasFileDescriptors(hasFileDescriptors);
    }
    else {
        *hasFileDescriptors = FALSE;
    }

    return NOERROR;
}

ECode Intent::SetAllowFds(
    /* [in] */ Boolean allowFds)
{
    if (mExtras != NULL){
        Boolean result;
        return mExtras->SetAllowFds(allowFds, &result);
    }
    return NOERROR;
}

ECode Intent::GetExtra(
    /* [in] */ const String& name,
    /* [out] */ IInterface** obj)
{
    return GetExtra(name, NULL, obj);
}

ECode Intent::GetBooleanExtra(
    /* [in] */ const String& name,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = defaultValue;
        return NOERROR;
    }
    else {
        return mExtras->GetBoolean(name, defaultValue, value);
    }
}

ECode Intent::GetByteExtra(
    /* [in] */ const String& name,
    /* [in] */ Byte defaultValue,
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = defaultValue;
        return NOERROR;
    }
    else {
        return mExtras->GetByte(name, defaultValue, value);
    }
}

ECode Intent::GetInt16Extra(
    /* [in] */ const String& name,
    /* [in] */ Int16 defaultValue,
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = defaultValue;
        return NOERROR;
    }
    else {
        return mExtras->GetInt16(name, defaultValue, value);
    }
}

ECode Intent::GetCharExtra(
    /* [in] */ const String& name,
    /* [in] */ Char32 defaultValue,
    /* [out] */ Char32* value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = defaultValue;
        return NOERROR;
    }
    else {
        return mExtras->GetChar(name, defaultValue, value);
    }
}

ECode Intent::GetInt32Extra(
    /* [in] */ const String& name,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = defaultValue;
        return NOERROR;
    }
    else {
        return mExtras->GetInt32(name, defaultValue, value);
    }
}

ECode Intent::GetInt64Extra(
    /* [in] */ const String& name,
    /* [in] */ Int64 defaultValue,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = defaultValue;
        return NOERROR;
    }
    else {
        return mExtras->GetInt64(name, defaultValue, value);
    }
}

ECode Intent::GetFloatExtra(
    /* [in] */ const String& name,
    /* [in] */ Float defaultValue,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = defaultValue;
        return NOERROR;
    }
    else {
        return mExtras->GetFloat(name, defaultValue, value);
    }
}

ECode Intent::GetDoubleExtra(
    /* [in] */ const String& name,
    /* [in] */ Double defaultValue,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = defaultValue;
        return NOERROR;
    }
    else {
        return mExtras->GetDouble(name, defaultValue, value);
    }
}

ECode Intent::GetStringExtra(
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetString(name, value);
    }
}

ECode Intent::GetCharSequenceExtra(
    /* [in] */ const String& name,
    /* [out] */ ICharSequence** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetCharSequence(name, value);
    }
}

ECode Intent::GetParcelableExtra(
    /* [in] */ const String& name,
    /* [out] */ IParcelable** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetParcelable(name, value);
    }
}

ECode Intent::GetParcelableArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<IParcelable*>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetParcelableArray(name, value);
    }
}

ECode Intent::GetParcelableArrayListExtra(
    /* [in] */ const String& name,
    /* [out] */ IArrayList** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetParcelableArrayList(name, value);
    }
}

ECode Intent::GetSerializableExtra(
    /* [in] */ const String& name,
    /* [out] */ ISerializable** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetSerializable(name, value);
    }
}

ECode Intent::GetInteger32ArrayListExtra(
    /* [in] */ const String& name,
    /* [out] */ IArrayList** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetIntegerArrayList(name, value);
    }
}

ECode Intent::GetStringArrayListExtra(
    /* [in] */ const String& name,
    /* [out] */ IArrayList** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetStringArrayList(name, value);
    }
}

ECode Intent::GetCharSequenceArrayListExtra(
    /* [in] */ const String& name,
    /* [out] */ IArrayList** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetCharSequenceArrayList(name, value);
    }
}

ECode Intent::GetBooleanArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Boolean>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetBooleanArray(name, value);
    }
}

ECode Intent::GetByteArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetByteArray(name, value);
    }
}

ECode Intent::GetInt16ArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Int16>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetInt16Array(name, value);
    }
}

ECode Intent::GetCharArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Char32>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetCharArray(name, value);
    }
}

ECode Intent::GetInt32ArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Int32>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetInt32Array(name, value);
    }
}

ECode Intent::GetInt64ArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Int64>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetInt64Array(name, value);
    }
}

ECode Intent::GetFloatArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Float>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetFloatArray(name, value);
    }
}

ECode Intent::GetDoubleArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Double>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetDoubleArray(name, value);
    }
}

ECode Intent::GetStringArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<String>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetStringArray(name, value);
    }
}

ECode Intent::GetCharSequenceArrayExtra(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<ICharSequence*>** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetCharSequenceArray(name, value);
    }
}

ECode Intent::GetBundleExtra(
    /* [in] */ const String& name,
    /* [out] */ IBundle** value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        *value = NULL;
        return NOERROR;
    }
    else {
        return mExtras->GetBundle(name, value);
    }
}

ECode Intent::GetExtra(
    /* [in] */ const String& name,
    /* [in] */ IInterface* defaultValue,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = defaultValue;
    if (mExtras != NULL) {
        AutoPtr<IInterface> newObj;
        mExtras->Get(name, (IInterface**)&newObj);
        if (newObj != NULL) {
            *obj = newObj;
            REFCOUNT_ADD(*obj);
            return NOERROR;
        }
    }

    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode Intent::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    if (mExtras != NULL) {
        return CBundle::New(mExtras, extras);
    }
    else {
        *extras = NULL;
    }
    return NOERROR;
}

ECode Intent::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags)
    *flags = mFlags;
    return NOERROR;
}

ECode Intent::IsExcludingStopped(
    /* [out] */ Boolean* isStopped)
{
    VALIDATE_NOT_NULL(isStopped)
    *isStopped = (mFlags &
            (IIntent::FLAG_EXCLUDE_STOPPED_PACKAGES|IIntent::FLAG_INCLUDE_STOPPED_PACKAGES))
                    == IIntent::FLAG_EXCLUDE_STOPPED_PACKAGES;
    return NOERROR;
}

ECode Intent::GetPackage(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    *packageName = mPackage;
    return NOERROR;
}

ECode Intent::GetComponent(
    /* [out] */ IComponentName** componentName)
{
    VALIDATE_NOT_NULL(componentName)
    *componentName = mComponent;
    REFCOUNT_ADD(*componentName);
    return NOERROR;
}

ECode Intent::GetSourceBounds(
    /* [out] */ IRect** sourceBounds)
{
    VALIDATE_NOT_NULL(sourceBounds)
    *sourceBounds = mSourceBounds;
    REFCOUNT_ADD(*sourceBounds);
    return NOERROR;
}

ECode Intent::ResolveActivity(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IComponentName** componentName)
{
    VALIDATE_NOT_NULL(componentName)
    if (mComponent != NULL) {
        *componentName = mComponent;
        REFCOUNT_ADD(*componentName);
        return NOERROR;
    }

    AutoPtr<IResolveInfo> info;
    pm->ResolveActivity(this, IPackageManager::MATCH_DEFAULT_ONLY,
            (IResolveInfo**)&info);

    if (info != NULL) {
        AutoPtr<IActivityInfo> activityInfo;
        FAIL_RETURN(info->GetActivityInfo((IActivityInfo**)&activityInfo));
        AutoPtr<IApplicationInfo> applicationInfo;
        FAIL_RETURN(IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo));
        String packageName;
        FAIL_RETURN(IPackageItemInfo::Probe(applicationInfo)->GetPackageName(&packageName));
        String name;
        FAIL_RETURN(IPackageItemInfo::Probe(activityInfo)->GetName(&name));
        return CComponentName::New(packageName, name, componentName);
    }

    *componentName = NULL;
    return NOERROR;
}

ECode Intent::ResolveActivityInfo(
    /* [in] */ IPackageManager* pm,
    /* [in] */ Int32 flags,
    /* [out] */ IActivityInfo** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IActivityInfo> ai;
    if (mComponent != NULL) {
        // try {
        pm->GetActivityInfo(mComponent, flags, (IActivityInfo**)&ai);
        // } catch (PackageManager.NameNotFoundException e) {
        //     // ignore
        // }
    }
    else {
        AutoPtr<IResolveInfo> info;
        pm->ResolveActivity(this, IPackageManager::MATCH_DEFAULT_ONLY | flags,
                (IResolveInfo**)&info);
        if (NULL != info) {
            FAIL_RETURN(info->GetActivityInfo((IActivityInfo**)&ai));
        }
    }

    *result = ai;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Intent::ResolveSystemService(
    /* [in] */ IPackageManager* pm,
    /* [in] */ Int32 flags,
    /* [out] */ IComponentName** componentName)
{
    VALIDATE_NOT_NULL(componentName)
    *componentName = NULL;

    if (mComponent != NULL) {
        *componentName = mComponent;
        REFCOUNT_ADD(*componentName)
        return NOERROR;
    }

    AutoPtr<IList> results;
    pm->QueryIntentServices(this, flags, (IList**)&results);
    if (results == NULL) {
        return NOERROR;
    }

    AutoPtr<IComponentName> comp;
    Int32 size, f;
    results->GetSize(&size);
    IResolveInfo* ri;
    String packageName, serviceName;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        results->Get(i, (IInterface**)&obj);
        ri = IResolveInfo::Probe(obj);

        AutoPtr<IServiceInfo> si;
        ri->GetServiceInfo((IServiceInfo**)&si);
        AutoPtr<IApplicationInfo> ai;
        IComponentInfo::Probe(si)->GetApplicationInfo((IApplicationInfo**)&ai);
        ai->GetFlags(&f);
        if ((f & IApplicationInfo::FLAG_SYSTEM) == 0) {
            continue;
        }

        IPackageItemInfo::Probe(ai)->GetPackageName(&packageName);
        IPackageItemInfo::Probe(si)->GetName(&serviceName);
        AutoPtr<IComponentName> foundComp;
        CComponentName::New(packageName, serviceName, (IComponentName**)&foundComp);
        if (comp != NULL) {
            // throw new IllegalStateException("Multiple system services handle " + this
            //         + ": " + comp + ", " + foundComp);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        comp = foundComp;
    }

    *componentName = comp;
    REFCOUNT_ADD(*componentName)
    return NOERROR;
}

ECode Intent::SetAction(
    /* [in] */ const String& action)
{
    mAction = action;
    return NOERROR;
}

ECode Intent::SetData(
    /* [in] */ IUri* data)
{
    mData = data;
    mType = NULL;
    return NOERROR;
}

ECode Intent::SetDataAndNormalize(
    /* [in] */ IUri* data)
{
    AutoPtr<IUri> tmpUri;
    data->NormalizeScheme((IUri**)&tmpUri);
    return SetData(tmpUri);
}

ECode Intent::SetType(
    /* [in] */ const String& type)
{
    mData = NULL;
    mType = type;
    return NOERROR;
}

ECode Intent::SetTypeAndNormalize(
    /* [in] */ const String& type)
{
    String mimeType = NormalizeMimeType(type);
    return SetType(mimeType);
}

ECode Intent::SetDataAndType(
    /* [in] */ IUri* data,
    /* [in] */ const String& type)
{
    mData = data;
    mType = type;
    return NOERROR;
}

ECode Intent::SetDataAndTypeAndNormalize(
    /* [in] */ IUri* data,
    /* [in] */ const String& type)
{
    AutoPtr<IUri> tmpUri;
    data->NormalizeScheme((IUri**)&tmpUri);
    String mimeType = NormalizeMimeType(type);
    return SetDataAndType(tmpUri, mimeType);
}

ECode Intent::AddCategory(
    /* [in] */ const String& category)
{
    if (mCategories == NULL) {
        mCategories = new HashSet<String>();
    }
    mCategories->Insert(category);
    return NOERROR;
}

ECode Intent::RemoveCategory(
    /* [in] */ const String& category)
{
    if (mCategories != NULL) {
        mCategories->Erase(category);
        if (mCategories->IsEmpty()) {
            mCategories = NULL;
        }
    }
    return NOERROR;
}

ECode Intent::SetSelector(
    /* [in] */ IIntent* selector)
{
    if (selector == this) {
        //throw new IllegalArgumentException(
        //        "Intent being set as a selector of itself");
        Slogger::E(TAG, "Intent being set as a selector of itself");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (selector != NULL && !mPackage.IsNull()) {
        //throw new IllegalArgumentException(
        //        "Can't set selector when package name is already set");
        Slogger::E(TAG, "Can't set selector when package name is already set");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mSelector = selector;
    return NOERROR;
}

ECode Intent::SetClipData(
    /* [in] */ IClipData* clip)
{
    mClipData = clip;
    return NOERROR;
}

ECode Intent::SetContentUserHint(
    /* [in] */ Int32 hint)
{
    mContentUserHint = hint;
    return NOERROR;
}

ECode Intent::PutBooleanExtra(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutBoolean(name, value);
}

ECode Intent::PutByteExtra(
    /* [in] */ const String& name,
    /* [in] */ Byte value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutByte(name, value);
}

ECode Intent::PutCharExtra(
    /* [in] */ const String& name,
    /* [in] */ Char32 value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutChar(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ Int16 value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutInt16(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ Int32 value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutInt32(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ Int64 value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutInt64(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ Float value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    FAIL_RETURN(mExtras->PutFloat(name, value));
    return NOERROR;
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ Double value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutDouble(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    FAIL_RETURN(mExtras->PutString(name, value));
    return NOERROR;
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ ICharSequence* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutCharSequence(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ IParcelable* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutParcelable(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<IParcelable*>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutParcelableArray(name, value);
}

ECode Intent::PutParcelableArrayListExtra(
    /* [in] */ const String& name,
    /* [in] */ IArrayList* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutParcelableArrayList(name, value);
}

ECode Intent::PutInteger32ArrayListExtra(
    /* [in] */ const String& name,
    /* [in] */ IArrayList* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutIntegerArrayList(name, value);
}

ECode Intent::PutStringArrayListExtra(
    /* [in] */ const String& name,
    /* [in] */ IArrayList* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutStringArrayList(name, value);
}

ECode Intent::PutCharSequenceArrayListExtra(
    /* [in] */ const String& name,
    /* [in] */ IArrayList* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutCharSequenceArrayList(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ ISerializable* value)
{
    VALIDATE_NOT_NULL(value)
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutSerializable(name, value);
}

ECode Intent::PutBooleanArrayExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Boolean>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutBooleanArray(name, value);
}

ECode Intent::PutByteArrayExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Byte>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutByteArray(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Int16>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutInt16Array(name, value);
}

ECode Intent::PutCharArrayExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Char32>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutCharArray(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Int32>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutInt32Array(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Int64>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutInt64Array(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Float>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutFloatArray(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Double>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutDoubleArray(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<String>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutStringArray(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<ICharSequence*>* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutCharSequenceArray(name, value);
}

ECode Intent::PutExtra(
    /* [in] */ const String& name,
    /* [in] */ IBundle* value)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutBundle(name, value);
}

ECode Intent::PutExtras(
    /* [in] */ IIntent* src)
{
    AutoPtr<IBundle> extras;
    src->GetExtras((IBundle**)&extras);

    if (extras != NULL) {
        if (mExtras == NULL) {
            return CBundle::New(extras, (IBundle**)&mExtras);
        }
        else {
            return mExtras->PutAll(extras);
        }
    }

    return NOERROR;
}

ECode Intent::PutExtras(
    /* [in] */ IBundle* extras)
{
    if (mExtras == NULL) {
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&mExtras));
    }
    return mExtras->PutAll(extras);
}

ECode Intent::ReplaceExtras(
    /* [in] */ IIntent* src)
{
    mExtras = NULL;
    AutoPtr<IBundle> extras;
    src->GetExtras((IBundle**)&extras);
    if (extras != NULL) {
        return CBundle::New(extras, (IBundle**)&mExtras);
    }
    return NOERROR;
}

ECode Intent::ReplaceExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = NULL;
    if (extras != NULL) {
        return CBundle::New(extras, (IBundle**)&mExtras);
    }

    return NOERROR;
}

ECode Intent::RemoveExtra(
    /* [in] */ const String& name)
{
    if (mExtras != NULL) {
        mExtras->Remove(name);
        Int32 size;
        mExtras->GetSize(&size);
        if (size == 0) {
            mExtras = NULL;
        }
    }

    return NOERROR;
}

ECode Intent::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode Intent::AddFlags(
    /* [in] */ Int32 flags)
{
    mFlags |= flags;
    return NOERROR;
}

ECode Intent::SetPackage(
    /* [in] */ const String& packageName)
{
    if (!packageName.IsNull() && mSelector != NULL) {
        //throw new IllegalArgumentException(
        //        "Can't set package name when selector is already set");
        Slogger::E(TAG, "Can't set package name when selector is already set");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mPackage = packageName;
    return NOERROR;
}

ECode Intent::SetComponent(
    /* [in] */ IComponentName* component)
{
    mComponent = component;
    return NOERROR;
}

ECode Intent::SetClassName(
    /* [in] */ IContext* packageContext,
    /* [in] */ const String& className)
{
    mComponent = NULL;
    return CComponentName::New(packageContext, className, (IComponentName**)&mComponent);
}

ECode Intent::SetClassName(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    mComponent = NULL;
    return CComponentName::New(packageName, className, (IComponentName**)&mComponent);
}

ECode Intent::SetClass(
    /* [in] */ IContext* packageContext,
    /* [in] */ const ClassID& clsId)
{
    mComponent = NULL;
    AutoPtr<IClassInfo> info = TransformClassInfo(clsId);
    return CComponentName::New(packageContext, info, (IComponentName**)&mComponent);
}

ECode Intent::SetSourceBounds(
    /* [in] */ IRect* r)
{
    mSourceBounds = NULL;
    if (r != NULL) {
        return CRect::New(r, (IRect**)&mSourceBounds);
    }
    return NOERROR;
}

ECode Intent::FillIn(
    /* [in] */ IIntent* other,
    /* [in] */ Int32 flags,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    ECode ec = NOERROR;
    assert(other != NULL);

    AutoPtr<CIntent> otherIntent = (CIntent*)other;
    assert(otherIntent != NULL);
    Int32 changes = 0;
    Boolean mayHaveCopiedUris = FALSE;

    if (!otherIntent->mAction.IsNull() &&
            (mAction.IsNull() || (flags & IIntent::FILL_IN_ACTION) != 0)) {
        mAction = otherIntent->mAction;
        changes |= IIntent::FILL_IN_ACTION;
    }

    if ((otherIntent->mData != NULL || !otherIntent->mType.IsNull())
        && ((mData == NULL && mType.IsNull())
            || (flags & IIntent::FILL_IN_DATA) != 0)) {
        mData = otherIntent->mData;
        mType = otherIntent->mType;
        changes |= IIntent::FILL_IN_DATA;
        mayHaveCopiedUris = TRUE;
    }

    if (otherIntent->mCategories != NULL
                && (mCategories == NULL || (flags & IIntent::FILL_IN_CATEGORIES) != 0)) {
        if (otherIntent->mCategories != NULL) {
            mCategories = new HashSet<String>(
                    otherIntent->mCategories->Begin(), otherIntent->mCategories->End());
        }
        changes |= IIntent::FILL_IN_CATEGORIES;
    }

    if (!otherIntent->mPackage.IsNull() &&
            (mPackage.IsNull() || (flags & IIntent::FILL_IN_PACKAGE) != 0)) {
        // Only do this if mSelector is not set.
        if (mSelector == NULL) {
            mPackage = otherIntent->mPackage;
            changes |= IIntent::FILL_IN_PACKAGE;
        }
    }

    // Selector is special: it can only be set if explicitly allowed,
    // for the same reason as the component name.
    if (otherIntent->mSelector != NULL && (flags & IIntent::FILL_IN_SELECTOR) != 0) {
        if (mPackage.IsNull()) {
            mSelector = NULL;
            CIntent::New(otherIntent->mSelector, (IIntent**)&mSelector);
            mPackage = NULL;
            changes |= IIntent::FILL_IN_SELECTOR;
        }
    }

    if (otherIntent->mClipData != NULL &&
            (mClipData == NULL || (flags & IIntent::FILL_IN_CLIP_DATA) != 0)) {
        mClipData = otherIntent->mClipData;
        changes |= IIntent::FILL_IN_CLIP_DATA;
        mayHaveCopiedUris = TRUE;
    }

    // Component is special: it can -only- be set if explicitly allowed,
    // since otherwise the sender could force the intent somewhere the
    // originator didn't intend.
    if (otherIntent->mComponent != NULL && (flags & IIntent::FILL_IN_COMPONENT) != 0) {
        mComponent = otherIntent->mComponent;
        changes |= IIntent::FILL_IN_COMPONENT;
    }

    mFlags |= otherIntent->mFlags;
    if (otherIntent->mSourceBounds != NULL
            && (mSourceBounds == NULL || (flags & IIntent::FILL_IN_SOURCE_BOUNDS) != 0)) {
        mSourceBounds = NULL;
        CRect::New(otherIntent->mSourceBounds, (IRect**)&mSourceBounds);
        changes |= IIntent::FILL_IN_SOURCE_BOUNDS;
    }

    if (mExtras == NULL) {
        if (otherIntent->mExtras != NULL) {
            CBundle::New(otherIntent->mExtras, (IBundle**)&mExtras);
            mayHaveCopiedUris = TRUE;
        }
    }
    else if (otherIntent->mExtras != NULL) {
        AutoPtr<IBundle> newb;
        ec = CBundle::New(otherIntent->mExtras, (IBundle**)&newb);
        FAIL_GOTO(ec, RUNTIME_EXCEPTION);
        ec = newb->PutAll(mExtras);
        FAIL_GOTO(ec, RUNTIME_EXCEPTION);
        mExtras = newb;
        mayHaveCopiedUris = TRUE;
RUNTIME_EXCEPTION:
        if (FAILED(ec)) {
            Slogger::W(TAG, "Failure filling in extras %d", ec);
        }
    }

    if (mayHaveCopiedUris && mContentUserHint == IUserHandle::USER_CURRENT
            && otherIntent->mContentUserHint != IUserHandle::USER_CURRENT) {
        mContentUserHint = otherIntent->mContentUserHint;
    }

    *result = changes;
    return NOERROR;
}

ECode Intent::FilterEquals(
    /* [in] */ IIntent* otherObj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    *isEqual = FALSE;

    if (otherObj == NULL) {
        return NOERROR;
    }

    Intent* other = (Intent*)otherObj;
    if (!mAction.Equals(other->mAction)) return NOERROR;
    if (!Objects::Equals(mData, other->mData)) return NOERROR;
    if (!mType.Equals(other->mType)) return NOERROR;
    if (!mPackage.Equals(other->mPackage)) return NOERROR;
    if (!Objects::Equals(mComponent, other->mComponent)) return NOERROR;

    AutoPtr< ArrayOf<String> > categories;
    other->GetCategories((ArrayOf<String>**)&categories);
    if (mCategories != NULL) {
        if (categories == NULL) {
            return NOERROR;
        }
        Int32 size = mCategories->GetSize();
        if (size != categories->GetLength()) {
            return NOERROR;
        }
        for (Int32 i = 0; i < categories->GetLength(); ++i) {
            String s = (*categories)[i];
            if (mCategories->Find(s) == mCategories->End()) {
                return NOERROR;
            }
        }
    }
    else {
        if (categories != NULL) {
            return NOERROR;
        }
    }

    *isEqual = TRUE;
    return NOERROR;
}

Int32 GetSetHashCode(
    /* [in] */ HashSet<String>& hs)
{
    Int32 result = 0;
    HashSet<String>::Iterator it = hs.Begin();
    for(; it != hs.End(); ++it) {
        String str = *it;
        result += (str == NULL ? 0 : str.GetHashCode());
    }
    return result;
}

ECode Intent::FilterHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 code = 0;
    if (!mAction.IsNull()) {
        code += mAction.GetHashCode();
    }
    if (mData != NULL) {
        code += Object::GetHashCode(mData);
    }
    if (!mType.IsNull()) {
        code += mType.GetHashCode();
    }
    if (!mPackage.IsNull()) {
        code += mPackage.GetHashCode();
    }
    if (mComponent != NULL) {
        code += Object::GetHashCode(mComponent);
    }
    if (mCategories != NULL) {
        code += GetSetHashCode(*mCategories);
    }
    *result = code;
    return NOERROR;
}

ECode Intent::ToString(
    /* [out] */ String* str)
{
    StringBuilder b(128);
    b.Append("Intent { ");
    ToShortString((IStringBuilder*)&b, TRUE, TRUE, TRUE, FALSE);
    b.Append(" }");
    return b.ToString(str);
}

ECode Intent::ToInsecureString(
    /* [out] */ String* str)
{
    StringBuilder b(128);
    b.Append("Intent { ");
    ToShortString((IStringBuilder*)&b, FALSE, TRUE, TRUE, FALSE);
    b.Append(" }");
    return b.ToString(str);
}

ECode Intent::ToInsecureStringWithClip(
    /* [out] */ String* str)
{
    StringBuilder b(128);
    b.Append("Intent { ");
    ToShortString((IStringBuilder*)&b, FALSE, TRUE, TRUE, TRUE);
    b.Append(" }");
    return b.ToString(str);
}

ECode Intent::ToShortString(
    /* [in] */ Boolean secure,
    /* [in] */ Boolean comp,
    /* [in] */ Boolean extras,
    /* [in] */ Boolean clip,
    /* [out] */ String* str)
{
    StringBuilder b(128);
    ToShortString((IStringBuilder*)&b, secure, comp, extras, clip);
    return b.ToString(str);
}

ECode Intent::ToShortString(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Boolean secure,
    /* [in] */ Boolean comp,
    /* [in] */ Boolean extras,
    /* [in] */ Boolean clip)
{
    Boolean first = TRUE;
    if (!mAction.IsNull()) {
        sb->Append(String("act="));
        sb->Append(mAction);
        first = FALSE;
    }
    if (mCategories != NULL) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->Append(String("cat=["));
        Boolean didone = FALSE;
        HashSet<String>::Iterator it;
        for (it = mCategories->Begin(); it != mCategories->End(); ++it) {
            if (didone) sb->Append(String(","));
            didone = TRUE;
            sb->Append(*it);
        }
        sb->Append(String("]"));
    }
    if (mData != NULL) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->Append(String("dat="));

        if (secure) {
            String dataStr;
            mData->ToSafeString(&dataStr);
            sb->Append(dataStr);
        }
        else {
            String dataStr = Object::ToString(mData);
            sb->Append(dataStr);
        }
    }
    if (!mType.IsNull()) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->Append(String("typ="));
        sb->Append(mType);
    }
    if (mFlags != 0) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->Append(String("flg=0x"));
//        Integer.toHexString(mFlags)
        sb->Append(StringUtils::ToHexString(mFlags));
    }
    if (!mPackage.IsNull()) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->Append(String("pkg="));
        sb->Append(mPackage);
    }
    if (comp && mComponent != NULL) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->Append(String("cmp="));
        String s;
        mComponent->FlattenToShortString(&s);
        sb->Append(s);
    }
    if (mSourceBounds != NULL) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->Append(String("bnds="));
        String s;
        mSourceBounds->ToShortString(&s);
        sb->Append(s);
    }
    if (mClipData != NULL) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        if (clip) {
            sb->Append(String("clip={"));
            mClipData->ToShortString(sb);
            sb->AppendChar('}');
        }
        else {
            sb->Append(String("(has clip)"));
        }
    }
    if (extras && mExtras != NULL) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->Append(String("(has extras)"));
    }
    if (mContentUserHint != IUserHandle::USER_CURRENT) {
        if (!first) {
            sb->AppendChar(' ');
        }
        first = FALSE;
        sb->Append(String("u="));
        sb->Append(mContentUserHint);
    }
    if (mSelector != NULL) {
        sb->Append(String(" sel={"));
        mSelector->ToShortString(sb, secure, comp, extras, clip);
        sb->Append(String("}"));
    }

    return NOERROR;
}

ECode Intent::ToURI(
    /* [out] */ String* str)
{
    return ToUri(0, str);
}

ECode Intent::ToUri(
    /* [in] */ Int32 flags,
    /* [out] */ String* str)
{
    StringBuilder uri(128);
    String scheme(NULL);
    if (mData != NULL) {
        String data = Object::ToString(mData);
        if ((flags & IIntent::URI_INTENT_SCHEME) != 0) {
            AutoPtr<ArrayOf<Char32> > charArray = data.GetChars();
            Char32 c;
            Int32 N = charArray->GetLength();
            for (Int32 i = 0; i < N; i++) {
                c = (*charArray)[i];
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                        || c == '.' || c == '-') {
                    continue;
                }
                if (c == ':' && i > 0) {
                    // Valid scheme.
                    scheme = data.Substring(0, i);
                    uri.Append("intent:");
                    data = data.Substring(i+1);
                    break;
                }

                // No scheme.
                break;
            }
        }
        uri.Append(data);
    }
    else if ((flags & IIntent::URI_INTENT_SCHEME) != 0) {
        uri.Append("intent:");
    }

    uri.Append("#Intent;");

    ToUriInner(uri, scheme, flags);

    if (mSelector != NULL) {
        uri.Append("SEL;");
        // Note that for now we are not going to try to handle the
        // data part; not clear how to represent this as a URI, and
        // not much utility in it.
        ((CIntent*)mSelector.Get())->ToUriInner(uri, String(NULL), flags);
    }

    uri.Append("end");
    return uri.ToString(str);
}

void Intent::ToUriInner(
    /* [in] */ StringBuilder& uri,
    /* [in] */ const String& scheme,
    /* [in] */ Int32 flags)
{
    assert(0 && "TODO");
    // if (!scheme.IsNull()) {
    //     uri.Append("scheme=");
    //     uri.Append(scheme);
    //     uri.AppendChar(';');
    // }
    // if (!mAction.IsNull()) {
    //     uri.Append("action=");
    //     String s;
    //     Uri::Encode(mAction, &s);
    //     uri.Append(s);
    //     uri.AppendChar(';');
    // }
    // if (mCategories != NULL) {
    //     HashSet<String>::Iterator it;
    //     for (it = mCategories->Begin(); it != mCategories->End(); ++it) {
    //         uri.Append("category=");
    //         String s;
    //         Uri::Encode(*it, &s);
    //         uri.Append(s);
    //         uri.AppendChar(';');
    //     }
    // }
    // if (!mType.IsNull()) {
    //     uri.Append("type=");
    //     String s;
    //     Uri::Encode(mType, String("/"), &s);
    //     uri.Append(s);
    //     uri.AppendChar(';');
    // }
    // if (mFlags != 0) {
    //     uri.Append("launchFlags=0x");
    //     uri.Append(StringUtils::ToString(mFlags));
    //     uri.AppendChar(';');
    // }
    // if (!mPackage.IsNull()) {
    //     uri.Append("package=");
    //     String s;
    //     Uri::Encode(mPackage, &s);
    //     uri.Append(s);
    //     uri.AppendChar(';');
    // }

    // if (mComponent != NULL) {
    //     uri.Append("component=");
    //     String compS;
    //     mComponent->FlattenToShortString(&compS);
    //     String s;
    //     Uri::Encode(compS, String("/"), &s);
    //     uri.Append(s);
    //     uri.AppendChar(';');
    // }
    // if (mSourceBounds != NULL) {
    //     uri.Append("sourceBounds=");
    //     String tmp;
    //     mSourceBounds->FlattenToString(&tmp);
    //     String s;
    //     Uri::Encode(tmp, &s);
    //     uri.Append(s);
    //     uri.AppendChar(';');
    // }
    // if (mExtras != NULL) {
    //     AutoPtr<IArrayList> objContainer;
    //     mExtras->KeySet((IArrayList**)&objContainer);
    //     AutoPtr<IObjectEnumerator> objEnumerator;
    //     objContainer->GetObjectEnumerator((IObjectEnumerator**)&objEnumerator);
    //     Boolean hasNext = FALSE;
    //     while ((objEnumerator->MoveNext(&hasNext), hasNext)) {
    //         AutoPtr<ICharSequence> obj;
    //         objEnumerator->Current((IInterface**)(ICharSequence**)&obj);
    //         String key;
    //         obj->ToString(&key);
    //         AutoPtr<IInterface> value;
    //         mExtras->Get(key, (IInterface**)&value);
    //         Char32 entryType =
    //                 ICharSequence::Probe(value) != NULL    ? 'S' :
    //                 IBoolean::Probe(value) != NULL         ? 'B' :
    //                 IByte::Probe(value) != NULL            ? 'b' :
    //                 IChar32::Probe(value) != NULL          ? 'c' :
    //                 IDouble::Probe(value) != NULL          ? 'd' :
    //                 IFloat::Probe(value) != NULL           ? 'f' :
    //                 IInteger32::Probe(value) != NULL       ? 'i' :
    //                 IInteger64::Probe(value) != NULL       ? 'l' :
    //                 IInteger16::Probe(value) != NULL       ? 's' :
    //                 '\0';
    //         if (entryType != '\0') {
    //             uri.AppendChar(entryType);
    //             uri.AppendChar('.');
    //             String s;
    //             Uri::Encode(key, &s);
    //             uri.Append(s);
    //             uri.AppendChar('=');
    //             String tmp;
    //             if (entryType == 'S') {
    //                 ((ICharSequence*)value.Get())->ToString(&tmp);
    //             }
    //             else if (entryType == 'B') {
    //                 Boolean val;
    //                 ((IBoolean*)value.Get())->GetValue(&val);
    //                 tmp = StringUtils::BooleanToString(val);
    //             }
    //             else if (entryType == 'b') {
    //                 Byte val;
    //                 ((IByte*)value.Get())->GetValue(&val);
    //                 tmp = StringUtils::ToString((Int32)val);
    //             }
    //             else if (entryType == 'd') {
    //                 Double val;
    //                 ((IDouble*)value.Get())->GetValue(&val);
    //                 tmp = StringUtils::DoubleToString(val);
    //             }
    //             else if (entryType == 'f') {
    //                 Float val;
    //                 ((IFloat*)value.Get())->GetValue(&val);
    //                 tmp = StringUtils::DoubleToString((Double)val);
    //             }
    //             else if (entryType == 'i') {
    //                 Int32 val;
    //                 ((IInteger32*)value.Get())->GetValue(&val);
    //                 tmp = StringUtils::ToString(val);
    //             }
    //             else if (entryType == 'l') {
    //                 Int64 val;
    //                 ((IInteger64*)value.Get())->GetValue(&val);
    //                 tmp = StringUtils::ToString(val);
    //             }
    //             else if (entryType == 's') {
    //                 Int16 val;
    //                 ((IInteger16*)value.Get())->GetValue(&val);
    //                 tmp = StringUtils::ToString((Int32)val);
    //             }
    //             Uri::Encode(tmp, &s);
    //             uri.Append(s);
    //             uri.AppendChar(';');
    //         }
    //     }
    // }
}

ECode Intent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mAction);
    Uri::WriteToParcel(dest, mData);
    dest->WriteString(mType);
    dest->WriteInt32(mFlags);
    dest->WriteString(mPackage);
    CComponentName::WriteToParcel(mComponent, dest);

    if (mSourceBounds != NULL) {
        dest->WriteInt32(1);
        dest->WriteInterfacePtr(mSourceBounds);
    }
    else {
        dest->WriteInt32(0);
    }

    if (mCategories != NULL) {
        dest->WriteInt32(mCategories->GetSize());
        HashSet<String>::Iterator it;
        for (it = mCategories->Begin(); it != mCategories->End(); ++it) {
            dest->WriteString(*it);
        }
    }
    else {
        dest->WriteInt32(0);
    }

    if (mSelector != NULL) {
        dest->WriteInt32(1);
        dest->WriteInterfacePtr(mSelector);
    }
    else {
        dest->WriteInt32(0);
    }

    if (mClipData != NULL) {
        dest->WriteInt32(1);
        IParcelable::Probe(mClipData)->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }

    dest->WriteInt32(mContentUserHint);

    if (mExtras != NULL) {
        dest->WriteInt32(1);
        IParcelable::Probe(mExtras)->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }

    return NOERROR;
}

ECode Intent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    String action;
    source->ReadString(&action);
    SetAction(action);
    Uri::ReadFromParcel(source, (IUri**)&mData);
    source->ReadString(&mType);
    source->ReadInt32(&mFlags);
    source->ReadString(&mPackage);
    CComponentName::ReadFromParcel(source, (IComponentName**)&mComponent);

    Int32 value;
    source->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mSourceBounds = IRect::Probe(obj);
    }

    source->ReadInt32(&value);
    if (value > 0) {
        assert(mCategories == NULL);
        mCategories = new HashSet<String>();
        String str;
        for (Int32 i = 0; i < value; i++) {
            source->ReadString(&str);
            mCategories->Insert(str);
        }
    }

    source->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mSelector = IIntent::Probe(obj);
    }

    source->ReadInt32(&value);
    if (value != 0) {
        mClipData = NULL;
        CClipData::New((IClipData**)&mClipData);
        IParcelable * p = IParcelable::Probe(mClipData);
        p->ReadFromParcel(source);
    }


    source->ReadInt32(&mContentUserHint);

    source->ReadInt32(&value);
    if (value != 0) {
        mExtras = NULL;
        CBundle::New((IBundle**)&mExtras);
        IParcelable * p = IParcelable::Probe(mExtras);
        p->ReadFromParcel(source);
    }

    return NOERROR;
}

ECode Intent::ParseIntent(
    /* [in] */ IResources* resources,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IIntent** _intent)
{
    VALIDATE_NOT_NULL(_intent);

    Int32 size = ArraySize(R::styleable::Intent);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::Intent, size);

    AutoPtr<CIntent> intent;
    CIntent::NewByFriend((CIntent**)&intent);
    AutoPtr<ITypedArray> sa;
    ASSERT_SUCCEEDED(resources->ObtainAttributes(attrs, layout, (ITypedArray**)&sa));
    String action;
    sa->GetString(R::styleable::Intent_action, &action);
    intent->SetAction(action);

    String data, mimeType;
    sa->GetString(R::styleable::Intent_data, &data);
    sa->GetString(R::styleable::Intent_mimeType, &mimeType);
    AutoPtr<IUri> uri;
    if (!data.IsNull()) {
        Uri::Parse(data, (IUri**)&uri);
    }
    intent->SetDataAndType(uri, mimeType);

    String packageName, className;
    sa->GetString(R::styleable::Intent_targetPackage, &packageName);
    sa->GetString(R::styleable::Intent_targetClass, &className);
    if (!packageName.IsNull() && !className.IsNull()) {
        AutoPtr<CComponentName> component;
        CComponentName::NewByFriend(packageName, className, (CComponentName**)&component);
        intent->SetComponent(component);
    }

    sa->Recycle();
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    Int32 type;
    Int32 tmpDepth;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&tmpDepth), tmpDepth) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        String nodeName;
        parser->GetName(&nodeName);
        if (nodeName.Equals(TAG_CATEGORIES)) {
            size = ArraySize(R::styleable::IntentCategory);
            layout = ArrayOf<Int32>::Alloc(size);
            layout->Copy(R::styleable::IntentCategory, size);

            ASSERT_SUCCEEDED(resources->ObtainAttributes(attrs, layout, (ITypedArray**)&sa));
            String cat;
            sa->GetString(R::styleable::IntentCategory_name, &cat);
            sa->Recycle();

            if (!cat.IsNull()) {
                intent->AddCategory(cat);
            }
            XmlUtils::SkipCurrentTag(parser);

        }
        else if (nodeName.Equals(TAG_EXTRA)) {
            if (intent->mExtras == NULL) {
                CBundle::New((IBundle**)&intent->mExtras);
            }
            resources->ParseBundleExtra(TAG_EXTRA, attrs, intent->mExtras);
            XmlUtils::SkipCurrentTag(parser);
        }
        else {
            XmlUtils::SkipCurrentTag(parser);
        }
    }

    *_intent = (IIntent*)intent;
    REFCOUNT_ADD(*_intent);

    return NOERROR;
}

ECode Intent::SaveToXml(
    /* [in] */ IXmlSerializer* out)
{
    assert(0 && "TODO");
    // if (mAction != NULL) {
    //     out.attribute(NULL, ATTR_ACTION, mAction);
    // }
    // if (mData != NULL) {
    //     out.attribute(NULL, ATTR_DATA, mData.toString());
    // }
    // if (mType != NULL) {
    //     out.attribute(NULL, ATTR_TYPE, mType);
    // }
    // if (mComponent != NULL) {
    //     out.attribute(NULL, ATTR_COMPONENT, mComponent.flattenToShortString());
    // }
    // out.attribute(NULL, ATTR_FLAGS, Integer.toHexString(getFlags()));

    // if (mCategories != NULL) {
    //     out.startTag(NULL, TAG_CATEGORIES);
    //     for (int categoryNdx = mCategories.size() - 1; categoryNdx >= 0; --categoryNdx) {
    //         out.attribute(NULL, ATTR_CATEGORY, mCategories.valueAt(categoryNdx));
    //     }
    //     out.endTag(NULL, TAG_CATEGORIES);
    // }
    return NOERROR;
}

AutoPtr<IIntent> Intent::RestoreFromXml(
    /* [in] */ IXmlPullParser* in)
{
    assert(0 && "TODO");
    // Intent intent = new Intent();
    // final int outerDepth = in.getDepth();

    // int attrCount = in.getAttributeCount();
    // for (int attrNdx = attrCount - 1; attrNdx >= 0; --attrNdx) {
    //     final String attrName = in.getAttributeName(attrNdx);
    //     final String attrValue = in.getAttributeValue(attrNdx);
    //     if (ATTR_ACTION.Equals(attrName)) {
    //         intent.setAction(attrValue);
    //     } else if (ATTR_DATA.Equals(attrName)) {
    //         intent.setData(Uri.parse(attrValue));
    //     } else if (ATTR_TYPE.Equals(attrName)) {
    //         intent.setType(attrValue);
    //     } else if (ATTR_COMPONENT.Equals(attrName)) {
    //         intent.setComponent(ComponentName.unflattenFromString(attrValue));
    //     } else if (ATTR_FLAGS.Equals(attrName)) {
    //         intent.setFlags(Integer.valueOf(attrValue, 16));
    //     } else {
    //         Log.e("Intent", "restoreFromXml: unknown attribute=" + attrName);
    //     }
    // }

    // int event;
    // String name;
    // while (((event = in.next()) != XmlPullParser.END_DOCUMENT) &&
    //         (event != XmlPullParser.END_TAG || in.getDepth() < outerDepth)) {
    //     if (event == XmlPullParser.START_TAG) {
    //         name = in.getName();
    //         if (TAG_CATEGORIES.Equals(name)) {
    //             attrCount = in.getAttributeCount();
    //             for (int attrNdx = attrCount - 1; attrNdx >= 0; --attrNdx) {
    //                 intent.addCategory(in.getAttributeValue(attrNdx));
    //             }
    //         } else {
    //             Log.w("Intent", "restoreFromXml: unknown name=" + name);
    //             XmlUtils.skipCurrentTag(in);
    //         }
    //     }
    // }

    // return intent;
    return NULL;
}

String Intent::NormalizeMimeType(
    /* [in] */ const String& type)
{
    if (type.IsNull()) {
        return String(NULL);
    }

    String result = type.Trim().ToLowerCase();
    Int32 semicolonIndex = result.IndexOf(';');
    if (semicolonIndex != -1) {
        result = result.Substring(0, semicolonIndex);
    }

    return result;
}

ECode Intent::PrepareToLeaveProcess()
{
    SetAllowFds(FALSE);

    if (mSelector != NULL) {
        mSelector->PrepareToLeaveProcess();
    }
    if (mClipData != NULL) {
        mClipData->PrepareToLeaveProcess();
    }

    if (mData != NULL /*&& StrictMode::VmFileUriExposureEnabled()*/) {
        // There are several ACTION_MEDIA_* broadcasts that send file://
        // Uris, so only check common actions.
        if (IIntent::ACTION_VIEW.Equals(mAction) ||
            IIntent::ACTION_EDIT.Equals(mAction) ||
            IIntent::ACTION_ATTACH_DATA.Equals(mAction)) {
            mData->CheckFileUriExposed(String("Intent.getData()"));
        }
    }
    return NOERROR;
}

ECode Intent::PrepareToEnterProcess()
{
    if (mContentUserHint != IUserHandle::USER_CURRENT) {
        FixUris(mContentUserHint);
        mContentUserHint = IUserHandle::USER_CURRENT;
    }
    return NOERROR;
}

ECode Intent::FixUris(
    /* [in] */ Int32 contentUserHint)
{
    AutoPtr<IUri> data;
    GetData((IUri**)&data);
    if (data != NULL) {
        mData = ContentProvider::MaybeAddUserId(data, contentUserHint);
    }
    if (mClipData != NULL) {
        mClipData->FixUris(contentUserHint);
    }
    String action;
    GetAction(&action);
    if (IIntent::ACTION_SEND.Equals(action)) {
        AutoPtr<IParcelable> stream;
        GetParcelableExtra(IIntent::EXTRA_STREAM, (IParcelable**)&stream);
        if (stream != NULL) {
            AutoPtr<IUri> uri = ContentProvider::MaybeAddUserId(IUri::Probe(stream), mContentUserHint);
            PutExtra(IIntent::EXTRA_STREAM, IParcelable::Probe(uri));
        }
    }
    else if (IIntent::ACTION_SEND_MULTIPLE.Equals(action)) {
        AutoPtr<IArrayList> streams;
        GetParcelableArrayListExtra(IIntent::EXTRA_STREAM, (IArrayList**)&streams);
        if (streams != NULL) {
            AutoPtr<IArrayList> newStreams;
            CArrayList::New((IArrayList**)&newStreams);
            Int32 size = 0;
            streams->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                streams->Get(i, (IInterface**)&obj);
                newStreams->Add(ContentProvider::MaybeAddUserId(IUri::Probe(obj), mContentUserHint));
            }
            PutParcelableArrayListExtra(IIntent::EXTRA_STREAM, newStreams);
        }
    }
    else if (IMediaStore::ACTION_IMAGE_CAPTURE.Equals(action)
            || IMediaStore::ACTION_IMAGE_CAPTURE_SECURE.Equals(action)
            || IMediaStore::ACTION_VIDEO_CAPTURE.Equals(action)) {
        AutoPtr<IParcelable> output;
        GetParcelableExtra(IMediaStore::EXTRA_OUTPUT, (IParcelable**)&output);
        if (output != NULL) {
            AutoPtr<IUri> uri = ContentProvider::MaybeAddUserId(IUri::Probe(output), mContentUserHint);
            PutExtra(IMediaStore::EXTRA_OUTPUT, IParcelable::Probe(uri));
        }
    }
    return NOERROR;
}

ECode Intent::MigrateExtraStreamToClipData(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Refuse to touch if extras already parcelled
    Boolean ret;
    if (mExtras != NULL && (mExtras->IsParcelled(&ret), ret)) {
        *result = FALSE;
        return NOERROR;
    }

    // Bail when someone already gave us ClipData
    AutoPtr<IClipData> clipData;
    GetClipData((IClipData**)&clipData);
    if (clipData != NULL) {
        *result = FALSE;
        return NOERROR;
    }

    String action;
    GetAction(&action);
    ECode ec = NOERROR;
    if (IIntent::ACTION_CHOOSER.Equals(action)) {
        // Inspect contained intents to see if we need to migrate extras. We
        // don't promote ClipData to the parent, since ChooserActivity will
        // already start the picked item as the caller, and we can't combine
        // the flags in a safe way.

        Boolean migrated = FALSE;
        // try {
        AutoPtr<IParcelable> extras;
        GetParcelableExtra(IIntent::EXTRA_INTENT, (IParcelable**)&extras);
        AutoPtr<IIntent> intent = IIntent::Probe(extras);
        if (intent != NULL) {
            Boolean bval;
            intent->MigrateExtraStreamToClipData(&bval);
            migrated |= bval;
        }
        // } catch (ClassCastException e) {
        // }
        // try {
        AutoPtr<ArrayOf<IParcelable*> > intents;
        GetParcelableArrayExtra(IIntent::EXTRA_INITIAL_INTENTS, (ArrayOf<IParcelable*>**)&intents);
        if (intents != NULL) {
            Boolean bval;
            IIntent* intent;
            for (Int32 i = 0; i < intents->GetLength(); i++) {
                intent = IIntent::Probe((*intents)[i]);
                if (intent != NULL) {
                    intent->MigrateExtraStreamToClipData(&bval);
                    migrated |= bval;
                }
            }
        }
        // } catch (ClassCastException e) {
        // }
        *result = migrated;
        return NOERROR;
    }
    else if (IIntent::ACTION_SEND.Equals(action)) {
        // try {
        AutoPtr<IParcelable> parcelable;
        ec = GetParcelableExtra(IIntent::EXTRA_STREAM, (IParcelable**)&parcelable);
        if (FAILED(ec)) {
            *result = FALSE;
            return NOERROR;
        }
        AutoPtr<IUri> stream;
        if (parcelable != NULL) {
            stream = (IUri*)parcelable->Probe(Elastos::Droid::Net::EIID_IUri);
        }

        AutoPtr<ICharSequence> text;
        GetCharSequenceExtra(IIntent::EXTRA_TEXT, (ICharSequence**)&text);
        String htmlText;
        GetStringExtra(IIntent::EXTRA_HTML_TEXT, &htmlText);
        if (stream != NULL || text != NULL || !htmlText.IsNull()) {
            AutoPtr< ArrayOf<String> > types = ArrayOf<String>::Alloc(1);
            String t;
            GetType(&t);
            (*types)[0] = t;
            AutoPtr<IClipDataItem> item;
            ec = CClipDataItem::New(text, htmlText, NULL, stream, (IClipDataItem**)&item);
            if (FAILED(ec)) {
                *result = FALSE;
                return NOERROR;
            }
            AutoPtr<IClipData> newClipData;
            ec = CClipData::New(NULL, types, item, (IClipData**)&newClipData);
            if (FAILED(ec)) {
                *result = FALSE;
                return NOERROR;
            }
            SetClipData(newClipData);
            AddFlags(IIntent::FLAG_GRANT_READ_URI_PERMISSION);
            *result = TRUE;
            return NOERROR;
        }
        // } catch (ClassCastException e) {
        // }
    }
    else if (IIntent::ACTION_SEND_MULTIPLE.Equals(action)) {
        // try {
        AutoPtr<IArrayList> streams, texts, htmlTexts;
        GetParcelableArrayListExtra(IIntent::EXTRA_STREAM, (IArrayList**)&streams);
        GetCharSequenceArrayListExtra(IIntent::EXTRA_TEXT, (IArrayList**)&texts);
        GetStringArrayListExtra(IIntent::EXTRA_HTML_TEXT, (IArrayList**)&htmlTexts);
        Int32 num = -1;
        AutoPtr< ArrayOf<IUri*> > streamsArray;
        AutoPtr< ArrayOf<ICharSequence*> > textsArray;
        AutoPtr< ArrayOf<String> > htmlTextsArray;
        if (streams != NULL) {
            streams->GetSize(&num);
            streamsArray = ArrayOf<IUri*>::Alloc(num);
            AutoPtr<IIterator> it;
            streams->GetIterator((IIterator**)&it);
            Boolean hasNext;
            Int32 i = 0;
            IUri* uri;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                uri = IUri::Probe(obj);
                streamsArray->Set(i, uri);
                i++;
            }
        }
        if (texts != NULL) {
            Int32 textsNum;
            texts->GetSize(&textsNum);
            if (num >= 0 && num != textsNum) {
                // Wha...!  F- you.
                return FALSE;
            }
            num = textsNum;
            textsArray = ArrayOf<ICharSequence*>::Alloc(num);
            AutoPtr<IIterator> it;
            texts->GetIterator((IIterator**)&it);
            Boolean hasNext;
            Int32 i = 0;
            ICharSequence* seq;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                seq = ICharSequence::Probe(obj);
                textsArray->Set(i, seq);
                i++;
            }
        }
        if (htmlTexts != NULL) {
            Int32 htmlTextsNum;
            htmlTexts->GetSize(&htmlTextsNum);
            if (num >= 0 && num != htmlTextsNum) {
                // Wha...!  F- you.
                return FALSE;
            }
            num = htmlTextsNum;
            htmlTextsArray = ArrayOf<String>::Alloc(num);
            AutoPtr<IIterator> it;
            htmlTexts->GetIterator((IIterator**)&it);
            Boolean hasNext;
            Int32 i = 0;
            ICharSequence* seq;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                seq = ICharSequence::Probe(obj);
                (*htmlTextsArray)[i] = Object::ToString(seq);
                i++;
            }
        }

        if (num > 0) {
            String t;
            GetType(&t);
            AutoPtr< ArrayOf<String> > types = ArrayOf<String>::Alloc(1);
            (*types)[0] = t;

            AutoPtr<IClipDataItem> cpi = MakeClipItem(streamsArray, textsArray, htmlTextsArray, 0);

            AutoPtr<CClipData> clipData;
            ec = CClipData::NewByFriend(NULL, types, cpi, (CClipData**)&clipData);
            if (FAILED(ec)) {
                *result = FALSE;
                return NOERROR;
            }

            for (Int32 i = 1; i < num; i++) {
                cpi = MakeClipItem(streamsArray, textsArray, htmlTextsArray, i);
                clipData->AddItem(cpi);
            }

            SetClipData((IClipData*)clipData.Get());
            AddFlags(IIntent::FLAG_GRANT_READ_URI_PERMISSION);
            return TRUE;
        }
        // } catch (ClassCastException e) {
        // }
    }
    else if (IMediaStore::ACTION_IMAGE_CAPTURE.Equals(action)
        || IMediaStore::ACTION_IMAGE_CAPTURE_SECURE.Equals(action)
        || IMediaStore::ACTION_VIDEO_CAPTURE.Equals(action)) {

        // try {
        AutoPtr<IParcelable> p;
        GetParcelableExtra(IMediaStore::EXTRA_OUTPUT, (IParcelable**)&p);
        AutoPtr<IUri> output = IUri::Probe(p);
        if (output == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        // } catch (ClassCastException e) {
        //     return FALSE;
        // }

        AutoPtr<IClipDataHelper> helper;
        CClipDataHelper::AcquireSingleton((IClipDataHelper**)&helper);
        AutoPtr<ICharSequence> seq = CoreUtils::Convert(String(""));
        AutoPtr<IClipData> cd;
        helper->NewRawUri(seq, output, (IClipData**)&cd);
        SetClipData(cd);
        AddFlags(IIntent::FLAG_GRANT_WRITE_URI_PERMISSION | IIntent::FLAG_GRANT_READ_URI_PERMISSION);

        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

AutoPtr<IClipDataItem> Intent::MakeClipItem(
    /* [in] */ ArrayOf<IUri*>* streams,
    /* [in] */ ArrayOf<ICharSequence*>* texts,
    /* [in] */ ArrayOf<String>* htmlTexts,
    /* [in] */ Int32 which)
{
    AutoPtr<IUri> uri;
    if (streams != NULL && which >= 0 && which < streams->GetLength()) {
        uri = (*streams)[which];
    }
    AutoPtr<ICharSequence> text;
    if (texts != NULL && which >= 0 && which < texts->GetLength()) {
        text = (*texts)[which];
    }
    String htmlText;
    if (htmlTexts != NULL && which >= 0 && which < htmlTexts->GetLength()) {
        htmlText = (*htmlTexts)[which];
    }
    AutoPtr<IClipDataItem> item;
    CClipDataItem::New(text, htmlText, NULL, uri, (IClipDataItem**)&item);
    return item;
}

AutoPtr<IClassInfo> Intent::TransformClassInfo(
    /* [in] */ const ClassID& objId)
{
    AutoPtr<IModuleInfo> moduleInfo;
    String path(objId.mUunm);
    ASSERT_SUCCEEDED(_CReflector_AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo));
    Int32 clsCount;
    moduleInfo->GetClassCount(&clsCount);
    AutoPtr<ArrayOf<IClassInfo*> > buf = ArrayOf<IClassInfo*>::Alloc(clsCount);
    moduleInfo->GetAllClassInfos(buf.Get());
    AutoPtr<IClassInfo> info;
    ClassID id;
    id.mUunm = (char*)malloc(80);
    for (Int32 i = 0; i < clsCount; i++) {
        (*buf)[i]->GetId(&id);
        if (id == objId) {
           info = (*buf)[i];
           break;
        }
    }
    free(id.mUunm);
    return info;
}

ECode Intent::IsDocument(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mFlags & IIntent::FLAG_ACTIVITY_NEW_DOCUMENT) == IIntent::FLAG_ACTIVITY_NEW_DOCUMENT;
    return NOERROR;
}

Boolean Intent::IsAccessUriMode(
    /* [in] */ Int32 modeFlags)
{
    return (modeFlags & (IIntent::FLAG_GRANT_READ_URI_PERMISSION
            | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION)) != 0;
}

} // namespace Content
} // namespace Droid
} // namespace Elastos
