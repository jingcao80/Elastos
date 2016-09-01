
#include "elastos/droid/javaproxy/CElSignatureParser.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::CSignature;
using Elastos::Droid::Content::Pm::EIID_IElSignatureParser;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CElSignatureParser::TAG = String("CElSignatureParser");

CAR_INTERFACE_IMPL_2(CElSignatureParser, Object, IElSignatureParser, IBinder)

CAR_OBJECT_IMPL(CElSignatureParser)

ECode CElSignatureParser::constructor(
    /* [in] */ Handle32 jVM)
{
    mJVM = (JavaVM*)jVM;
    return NOERROR;
}

ECode CElSignatureParser::GetSignature(
    /* [in] */ const String& pkg,
    /* [out] */ ISignature** signature)
{
    // LOGGERD(TAG, String("+ CElSignatureParser::GetSignature()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass eppKlass = env->FindClass("android/content/pm/ElSignature");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ElSignature : %d!\n", __LINE__);

    jmethodID m = env->GetStaticMethodID(eppKlass, "getSignature", "(Ljava/lang/String;)Landroid/content/pm/Signature;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getSignature %d", __LINE__);

    jstring jpkgPath = env->NewStringUTF(pkg.string());

    jobject jsignature = env->CallStaticObjectMethod(eppKlass, m, jpkgPath);
    Util::CheckErrorAndLog(env, TAG, "CallStaticObjectMethod() getSignature %d", __LINE__);

    if (jsignature != NULL) {
        jclass sKlass = env->FindClass("android/content/pm/Signature");
        Util::CheckErrorAndLog(env, TAG, "FindClass: Signature : %d!\n", __LINE__);

        jfieldID f = env->GetFieldID(sKlass, "mSignature", "[B");
        Util::CheckErrorAndLog(env, TAG, "GetFieldID: mSignature %d", __LINE__);

        jbyteArray jarray = (jbyteArray)env->GetObjectField(jsignature, f);
        Util::CheckErrorAndLog(env, TAG, "GetObjectField: mSignature %d", __LINE__);

        AutoPtr<ArrayOf<Byte> > byteArray;
        if (jarray != NULL) {
            Util::GetElByteArray(env, jarray, (ArrayOf<Byte>**)&byteArray);
            CSignature::New(byteArray, signature);
            env->DeleteLocalRef(jarray);
        }
        env->DeleteLocalRef(sKlass);
    }

    env->DeleteLocalRef(eppKlass);
    env->DeleteLocalRef(jsignature);

    // LOGGERD(TAG, String("- CElSignatureParser::GetSignature()"));
    return NOERROR;
}

ECode CElSignatureParser::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

}
}
}

