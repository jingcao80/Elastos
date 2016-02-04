
#ifndef __ELASTOS_DROID_SERVICE_TEXTSERVICE_CINTERNALISPELLCHECKERSESSION_H__
#define __ELASTOS_DROID_SERVICE_TEXTSERVICE_CINTERNALISPELLCHECKERSESSION_H__

#include "_Elastos_Droid_Service_TextService_CInternalISpellCheckerSession.h"
#include "elastos/droid/service/textservice/SpellCheckerService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace TextService {

CarClass(CInternalISpellCheckerSession)
    , public SpellCheckerService::InternalISpellCheckerSession
{
public:
    CAR_OBJECT_DECL()
};

} // namespace TextService
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_TEXTSERVICE_CINTERNALISPELLCHECKERSESSION_H__
