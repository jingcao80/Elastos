
#include "list/DialerPhoneNumberListAdapter.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

DialerPhoneNumberListAdapter::DialerPhoneNumberListAdapter()
{
    mShortcutEnabled = ArrayOf<Boolean>::Alloc(IDialerPhoneNumberListAdapter::SHORTCUT_COUNT);
}

ECode DialerPhoneNumberListAdapter::constructor(
    /* [in] */ IContext* context)
{
    PhoneNumberListAdapter::constructor(context);

    mCountryIso = GeoUtil::GetCurrentCountryIso(context);

    // Enable all shortcuts by default
    for (Int32 i = 0; i < mShortcutEnabled->GetLength(); i++) {
        (*mShortcutEnabled)[i] = TRUE;
    }
}

ECode DialerPhoneNumberListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALUE_NOT_NULL(count);

    Int32 superCount, shortcutCount;
    PhoneNumberListAdapter::GetCount(&superCount);
    GetShortcutCount(&shortcutCount);
    *count = superCount + shortcutCount;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::GetShortcutCount(
    /* [out] */ Int32* shortcutCount)
{
    VALUE_NOT_NULL(shortcutCount);

    Int32 count = 0;
    for (Int32 i = 0; i < mShortcutEnabled->GetLength; i++) {
        if ((*mShortcutEnabled)[i]) count++;
    }
    *shortcutCount = count;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALUE_NOT_NULL(type);

    Int32 shortcut;
    GetShortcutTypeFromPosition(position, &shortcut);
    if (shortcut >= 0) {
        // shortcutPos should always range from 1 to SHORTCUT_COUNT
        Int32 count;
        PhoneNumberListAdapter::GetViewTypeCount(*count);
        *type = count + shortcut;
        return NOERROR;
    }
    else {
        return PhoneNumberListAdapter::GetItemViewType(position, type);
    }
}

ECode DialerPhoneNumberListAdapter::GetViewTypeCount(
    /* [in] */ Int32* count)
{
    VALUE_NOT_NULL(count);

    // Number of item view types in the super implementation + 2 for the 2 new shortcuts
    Int32 viewTypeCount;
    PhoneNumberListAdapter::GetViewTypeCount(&viewTypeCount);
    *count = viewTypeCount + SHORTCUT_COUNT;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALUE_NOT_NULL(view);

    Int32 shortcutType;
    GetShortcutTypeFromPosition(position, &shortcutType);
    if (shortcutType >= 0) {
        if (convertView != NULL) {
            AssignShortcutToView(IContactListItemView::Probe(convertView), shortcutType);
            *view = convertView;
            REFCOUNT_ADD(*view);
            return NOERROR;
        }
        else {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IContactListItemView> v;
            CContactListItemView::New(context, NULL, (IContactListItemView**)&v);
            AssignShortcutToView(v, shortcutType);
            *view = v;
            REFCOUNT_ADD(*view);
            return NOERROR;
        }
    }
    else {
        return PhoneNumberListAdapter::GetView(position, convertView, parent, view);
    }
}

ECode DialerPhoneNumberListAdapter::GetShortcutTypeFromPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALUE_NOT_NULL(type);

    Int32 count;
    PhoneNumberListAdapter::GetCount(&count);
    Int32 shortcutCount = position - count;
    if (shortcutCount >= 0) {
        // Iterate through the array of shortcuts, looking only for shortcuts where
        // mShortcutEnabled[i] is true
        for (Int32 i = 0; shortcutCount >= 0 && i < mShortcutEnabled->GetLength(); i++) {
            if ((*mShortcutEnabled)[i]) {
                shortcutCount--;
                if (shortcutCount < 0) {
                    *type = i;
                    return NOERROR;
                }
            }
        }
        // throw new IllegalArgumentException("Invalid position - greater than cursor count "
        //         + " but not a shortcut.");
        Logger::E(String("DialerPhoneNumberListAdapter"), "Invalid position - greater than cursor count "
            " but not a shortcut.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *type = IDialerPhoneNumberListAdapter::SHORTCUT_INVALID;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    Int32 count;
    GetShortcutCount(&count);
    Boolean isEmpty;
    *result = count == 0 && PhoneNumberListAdapter::IsEmpty(&isEmpty), isEmpty;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    Int32 shortcutType;
    GetShortcutTypeFromPosition(position, &shortcutType);
    if (shortcutType >= 0) {
        *result = TRUE;
        return NOERROR;
    }
    else {
        return PhoneNumberListAdapter::IsEnabled(position, result);
    }
}

ECode DialerPhoneNumberListAdapter::AssignShortcutToView(
    /* [in] */ IContactListItemView* v,
    /* [in] */ Int32 shortcutType)
{
    AutoPtr<ICharSequence> text;
    Int32 drawableId;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    String number;
    GetFormattedQueryString(&number);
    switch (shortcutType) {
        case SHORTCUT_DIRECT_CALL:
            resources->GetString(R::string::search_shortcut_call_number,
                    number, (ICharSequence**)&text);
            drawableId = R::drawable::ic_search_phone;
            break;
        case SHORTCUT_ADD_NUMBER_TO_CONTACTS:
            resources->GetString(R::string::search_shortcut_add_to_contacts, (ICharSequence**)&text);
            drawableId = R::drawable::ic_search_add_contact;
            break;
        case SHORTCUT_MAKE_VIDEO_CALL:
            resources->GetString(R::string::search_shortcut_make_video_call, (ICharSequence**)&text);
            drawableId = R::drawable::ic_videocam;
            break;
        default:
            Logger::E(String("DialerPhoneNumberListAdapter"), "Invalid shortcut type");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("Invalid shortcut type");
    }
    v->SetDrawableResource(R::drawable::search_shortcut_background, drawableId);
    v->SetDisplayName(text);
    Int32 position;
    PhoneNumberListAdapter::GetPhotoPosition(&position);
    v->SetPhotoPosition(position);
    v->SetAdjustSelectionBoundsEnabled(FALSE);

    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::SetShortcutEnabled(
    /* [in] */ Int32 shortcutType,
    /* [in] */ Boolean visible,
    /* [out] */ Boolean* enabled)
{
    VALUE_NOT_NULL(enabled);

    Boolean changed = (*mShortcutEnabled)[shortcutType] != visible;
    (*mShortcutEnabled)[shortcutType] = visible;
    *enabled = changed;

    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::GetFormattedQueryString(
    /* [out] */ String* str)
{
    VALUE_NOT_NULL(str);
    *str = mFormattedQueryString;
    return NOERROR;
}

ECode DialerPhoneNumberListAdapter::SetQueryString(
    /* [in] */ const String& queryString)
{
    mFormattedQueryString = PhoneNumberUtils::FormatNumber(
            PhoneNumberUtils::NormalizeNumber(queryString), mCountryIso);
    return PhoneNumberListAdapter::SetQueryString(queryString);
}

} // List
} // Dialer
} // Apps
} // Elastos
