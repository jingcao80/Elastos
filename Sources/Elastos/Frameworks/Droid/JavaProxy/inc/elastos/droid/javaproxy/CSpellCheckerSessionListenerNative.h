
#ifndef __ELASTOS_DROID_JAVAPROXY_CSPELLCHECKERSESSIONLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CSPELLCHECKERSESSIONLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CSpellCheckerSessionListenerNative.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::TextService::IISpellCheckerSessionListener;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::TextService::ISentenceSuggestionsInfo;
using Elastos::Droid::View::TextService::ISuggestionsInfo;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CSpellCheckerSessionListenerNative)
    , public Object
    , public IISpellCheckerSessionListener
    , public IBinder
{
public:
    ~CSpellCheckerSessionListenerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnGetSuggestions(
        /* [in] */ ArrayOf<ISuggestionsInfo*>* results);

    CARAPI OnGetSentenceSuggestions(
        /* [in] */ ArrayOf<ISentenceSuggestionsInfo*>* results);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CSPELLCHECKERSESSIONLISTENERNATIVE_H__
