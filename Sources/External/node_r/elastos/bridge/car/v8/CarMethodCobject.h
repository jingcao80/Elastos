
#ifndef __CARMETHODCOBJECT_H__
#define __CARMETHODCOBJECT_H__

#include <elastos.h>

#if ENABLE(CAR_BRIDGE)

#include "CarMethod.h"
#include "CarString.h"

#include <wtf/text/CString.h>

namespace JSC {
namespace Bindings {

class CarMethodCobject : public CarMethod
{
public:
    CarMethodCobject(IMethodInfo*);
    virtual ~CarMethodCobject();

    // CarMethod implementation
    virtual WTF::String name() const { return mName.impl(); }
    virtual RuntimeType returnTypeClassName() const { return mReturnTypeClassName.utf8(); }
    virtual WTF::String parameterAt(int i) const { return mParameters[i]; }
    virtual const char* signature() const {return mSignature; }
    virtual CarDataType returnType() const { return mReturnType; }
    virtual bool isStatic() const { return mIsStatic; }

    // Method implementation
    virtual int numParameters() const { return mParameters.size(); }

    virtual bool isRunOnUiThread() const { return mIsRunOnUiThread; }
    virtual void setRunOnUiThread(bool isRunOnUiThread) { mIsRunOnUiThread = isRunOnUiThread; }
private:
    WTF::Vector<WTF::String> mParameters;
    CarString mName;
    mutable char* mSignature;
    CarString mReturnTypeClassName;
    CarDataType mReturnType;
    bool mIsStatic;

    bool mIsRunOnUiThread;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARMETHODCOBJECT_H__
