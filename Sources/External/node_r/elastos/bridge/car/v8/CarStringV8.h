
#ifndef __CARSTRINGV8_H__
#define __CARSTRINGV8_H__

#include <elastos.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

#include <utils/Log.h>

namespace JSC {
namespace Bindings {

class CarStringImpl
{
public:
    void init() {}

    void init(const Elastos::String& string)
    {
        mImpl = StringImpl::create(string.string());
    }

    const char* utf8() const
    {
        if (!mUtf8String.data()) {
            mUtf8String = WTF::String(mImpl).utf8();
        }
        return mUtf8String.data();
    }

    int length() const { return mUtf8String.length(); }

    StringImpl* impl() const { return mImpl.get(); }

private:
    RefPtr<StringImpl> mImpl;
    mutable WTF::CString mUtf8String;
};

} // namespace Bindings
} // namespace JSC

#endif //__CARSTRINGV8_H__
