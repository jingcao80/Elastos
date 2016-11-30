
#ifndef __ORG_ALLJOYN_BUS_NATIVETRANSLATOR_H__
#define __ORG_ALLJOYN_BUS_NATIVETRANSLATOR_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/Translator.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeTranslator : public ajn::Translator
{
public:
    NativeTranslator(
        /* [in] */ ITranslator* translator);

    ~NativeTranslator();

    virtual size_t NumTargetLanguages();

    virtual void GetTargetLanguage(
        /* [in] */ size_t index,
        /* [in] */ qcc::String& ret);

    virtual const char* Translate(
        /* [in] */ const char* sourceLanguage,
        /* [in] */ const char* targetLanguage,
        /* [in] */ const char* source,
        /* [in] */ qcc::String& buffer);

private:
    NativeTranslator(const NativeTranslator& other);
    NativeTranslator& operator =(const NativeTranslator& other);

private:
    AutoPtr<IWeakReference> mTranslator;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVETRANSLATOR_H__
