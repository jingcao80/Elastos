
#ifndef __CTEXTSERVICESSESSIONLISTENERNATIVE_H__
#define __CTEXTSERVICESSESSIONLISTENERNATIVE_H__

#include "_CTextServicesSessionListenerNative.h"
#include <jni.h>

using Elastos::Droid::View::TextService::IISpellCheckerSession;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CTextServicesSessionListenerNative)
{
public:
    ~CTextServicesSessionListenerNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnServiceConnected(
        /* [in] */ IISpellCheckerSession* spellCheckerSession);

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

#endif // __CTEXTSERVICESSESSIONLISTENERNATIVE_H__
