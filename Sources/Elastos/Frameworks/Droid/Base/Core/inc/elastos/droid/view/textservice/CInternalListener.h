
#ifndef __ELASTOS_DROID_VIEW_TEXTSERVICE_CINTERNALLISTENER_H__
#define __ELASTOS_DROID_VIEW_TEXTSERVICE_CINTERNALLISTENER_H__

#include "_Elastos_Droid_View_TextService_CInternalListener.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::Internal::TextService::IITextServicesSessionListener;
using Elastos::Droid::View::Internal::TextService::IISpellCheckerSession;

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

CarClass(CInternalListener)
    , public Object
    , public IInternalListener
    , public IITextServicesSessionListener
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ISpellCheckerSessionListenerImpl* spellCheckerSessionListenerImpl);

    CARAPI OnServiceConnected(
        /* [in] */ IISpellCheckerSession* session);

private:
    AutoPtr<ISpellCheckerSessionListenerImpl> mParentSpellCheckerSessionListenerImpl;

    static const String TAG;
    static const Boolean DBG;
};

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TEXTSERVICE_CINTERNALLISTENER_H__
