
#include "elastos/droid/settings/inputmethod/CUserDictionaryLocalePicker.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Inputmethod {

CAR_OBJECT_IMPL(CUserDictionaryLocalePicker)

CUserDictionaryLocalePicker::CUserDictionaryLocalePicker()
{}

CUserDictionaryLocalePicker::~CUserDictionaryLocalePicker()
{}

ECode CUserDictionaryLocalePicker::constructor(
    /* [in] */ IUserDictionaryAddWordFragment* parent)
{
    LocalePicker::constructor();
    SetLocaleSelectionListener(ILocaleSelectionListener::Probe(parent));
    return NOERROR;
}

} // namespace Inputmethod
} // namespace Settings
} // namespace Droid
} // namespace Elastos