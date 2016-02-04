
#include "elastos/droid/view/textservice/CInternalListener.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::TextService::EIID_IITextServicesSessionListener;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

//========================================================================================
//              CInternalListener::
//========================================================================================
const String CInternalListener::TAG("SpellCheckerSession");
const Boolean CInternalListener::DBG = FALSE;

CAR_INTERFACE_IMPL_2(CInternalListener, Object, IInternalListener, IITextServicesSessionListener)

CAR_OBJECT_IMPL(CInternalListener)

ECode CInternalListener::constructor(
    /* [in] */ ISpellCheckerSessionListenerImpl* spellCheckerSessionListenerImpl)
{
    mParentSpellCheckerSessionListenerImpl = spellCheckerSessionListenerImpl;
    return NOERROR;
}

ECode CInternalListener::OnServiceConnected(
    /* [in] */ IISpellCheckerSession* session)
{
    if (DBG) {
        Logger::W(TAG, "SpellCheckerSession connected.");
    }
    mParentSpellCheckerSessionListenerImpl->OnServiceConnected(session);
    return NOERROR;
}

} // namespace TextService
} // namespace View
} // namespace Droid
} // namespace Elastos
