
#ifndef __ELASTOS_DROID_JAVAPROXY_CSPELLCHECKERSESSIONLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CSPELLCHECKERSESSIONLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CSpellCheckerSessionListenerNative.h"
#include <jni.h>

using Elastos::Droid::View::TextService::ISentenceSuggestionsInfo;
using Elastos::Droid::View::TextService::ISuggestionsInfo;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CSpellCheckerSessionListenerNative)
{
public:
    ~CSpellCheckerSessionListenerNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnGetSuggestions(
        /* [in] */ const ArrayOf<ISuggestionsInfo *>& results);

    CARAPI OnGetSentenceSuggestions(
        /* [in] */ const ArrayOf<ISentenceSuggestionsInfo *>& results);

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
