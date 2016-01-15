
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfoAccessibilityAction.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "elastos/droid/utility/CArraySet.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::Math;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Utility::ICollection;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

static CARAPI_(AutoPtr<IAccessibilityNodeInfoAccessibilityAction>) InitACTION(
    /*[in] */ Int32 value)
{
    AutoPtr<CAccessibilityNodeInfoAccessibilityAction> action;
    CAccessibilityNodeInfoAccessibilityAction::NewByFriend(value, NULL, (CAccessibilityNodeInfoAccessibilityAction**)&action);
    return (IAccessibilityNodeInfoAccessibilityAction*)action.Get();
}

const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_FOCUS = InitACTION(IAccessibilityNodeInfo::ACTION_FOCUS);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_CLEAR_FOCUS = InitACTION(IAccessibilityNodeInfo::ACTION_CLEAR_FOCUS);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_SELECT = InitACTION(IAccessibilityNodeInfo::ACTION_SELECT);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_CLEAR_SELECTION = InitACTION(IAccessibilityNodeInfo::ACTION_CLEAR_SELECTION);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_CLICK = InitACTION(IAccessibilityNodeInfo::ACTION_CLICK);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_LONG_CLICK = InitACTION(IAccessibilityNodeInfo::ACTION_LONG_CLICK);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_ACCESSIBILITY_FOCUS = InitACTION(IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS = InitACTION(IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_NEXT_AT_MOVEMENT_GRANULARITY = InitACTION(IAccessibilityNodeInfo::ACTION_NEXT_AT_MOVEMENT_GRANULARITY);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY = InitACTION(IAccessibilityNodeInfo::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_NEXT_HTML_ELEMENT = InitACTION(IAccessibilityNodeInfo::ACTION_NEXT_HTML_ELEMENT);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_PREVIOUS_HTML_ELEMENT = InitACTION(IAccessibilityNodeInfo::ACTION_PREVIOUS_HTML_ELEMENT);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_SCROLL_FORWARD = InitACTION(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_SCROLL_BACKWARD = InitACTION(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_COPY = InitACTION(IAccessibilityNodeInfo::ACTION_COPY);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_PASTE = InitACTION(IAccessibilityNodeInfo::ACTION_PASTE);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_CUT = InitACTION(IAccessibilityNodeInfo::ACTION_CUT);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_SET_SELECTION = InitACTION(IAccessibilityNodeInfo::ACTION_SET_SELECTION);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_EXPAND = InitACTION(IAccessibilityNodeInfo::ACTION_EXPAND);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_COLLAPSE = InitACTION(IAccessibilityNodeInfo::ACTION_COLLAPSE);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_DISMISS = InitACTION(IAccessibilityNodeInfo::ACTION_DISMISS);
const AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfoAccessibilityAction::ACTION_SET_TEXT = InitACTION(IAccessibilityNodeInfo::ACTION_SET_TEXT);

static AutoPtr<IArraySet> InitsStandardActions()
{
    AutoPtr<CArraySet> carraySet;
    CArraySet::NewByFriend((CArraySet**)&carraySet);
    AutoPtr<IArraySet> set = (IArraySet*)carraySet.Get();
    AutoPtr<ICollection> coll = ICollection::Probe(set);

    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_FOCUS));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_CLEAR_FOCUS));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_SELECT));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_CLEAR_SELECTION));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_CLICK));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_LONG_CLICK));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_ACCESSIBILITY_FOCUS));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_NEXT_AT_MOVEMENT_GRANULARITY));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_NEXT_HTML_ELEMENT));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_PREVIOUS_HTML_ELEMENT));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_SCROLL_FORWARD));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_SCROLL_BACKWARD));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_COPY));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_PASTE));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_CUT));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_SET_SELECTION));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_EXPAND));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_COLLAPSE));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_DISMISS));
    coll->Add((CAccessibilityNodeInfoAccessibilityAction::ACTION_SET_TEXT));

    return set;
}

const AutoPtr<IArraySet> CAccessibilityNodeInfoAccessibilityAction::sStandardActions = InitsStandardActions();

CAR_INTERFACE_IMPL(CAccessibilityNodeInfoAccessibilityAction, Object, IAccessibilityNodeInfoAccessibilityAction)

CAR_OBJECT_IMPL(CAccessibilityNodeInfoAccessibilityAction)

CAccessibilityNodeInfoAccessibilityAction::CAccessibilityNodeInfoAccessibilityAction()
    : mActionId(0)
{
}

CAccessibilityNodeInfoAccessibilityAction::~CAccessibilityNodeInfoAccessibilityAction()
{
}

ECode CAccessibilityNodeInfoAccessibilityAction::constructor(
    /* [in] */ Int32 actionId,
    /* [in] */ ICharSequence* label)
{
    if ((actionId & CAccessibilityNodeInfo::ACTION_TYPE_MASK) == 0
            && Elastos::Core::Math::BitCount(actionId) != 1) {
        // throw new IllegalArgumentException("Invalid standard action id");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mActionId = actionId;
    mLabel = label;

    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityAction::GetId(
    /* [out] */ Int32* actionId)
{
    VALIDATE_NOT_NULL(actionId);
    *actionId = mActionId;
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityAction::GetLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    *label = mLabel;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityAction::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mActionId;
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityAction::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (other == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    if (IObject::Probe(other) == THIS_PROBE(IObject)) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<CAccessibilityNodeInfoAccessibilityAction> obj = (CAccessibilityNodeInfoAccessibilityAction*)IAccessibilityNodeInfoAccessibilityAction::Probe(other);
    *result = mActionId == obj->mActionId;
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityAction::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    String str;
    mLabel->ToString(&str);
    StringBuilder builder;
    builder += "AccessibilityAction: ";
    builder += CAccessibilityNodeInfo::GetActionSymbolicName(mActionId);
    builder += " - ";
    builder += str;
    *info = builder.ToString();

    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos
