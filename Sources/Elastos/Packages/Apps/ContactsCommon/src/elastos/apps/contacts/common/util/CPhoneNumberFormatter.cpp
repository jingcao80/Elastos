
#include "CPhoneNumberFormatter.h"
#include "PhoneNumberFormatter.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CAR_INTERFACE_IMPL(CPhoneNumberFormatter, Singleton, IPhoneNumberFormatter)

CAR_SINGLETON_IMPL(CPhoneNumberFormatter)

ECode CPhoneNumberFormatter::SetPhoneNumberFormattingTextWatcher(
    /* [in] */ IContext* context,
    /* [in] */ ITextView* textView)
{
    PhoneNumberFormatter::SetPhoneNumberFormattingTextWatcher(context, textView);
    return NOERROR;
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
