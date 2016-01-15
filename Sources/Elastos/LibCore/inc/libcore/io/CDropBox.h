#ifndef __LIBCORE_IO_CDROPBOX_H__
#define __LIBCORE_IO_CDROPBOX_H__

#include "core/Object.h"
#include "Singleton.h"
#include "_Libcore_IO_CDropBox.h"


namespace Libcore {
namespace IO {

CarClass(CDropBox)
    , public Singleton
    , public IDropBox
{
private:
        /**
     * Default Reporter which reports events to the log.
     */
    class DefaultReporter
        : public Object
        , public IReporter
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI AddData(
            /* [in] */ const String& tag,
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 flags);

        CARAPI AddText(
            /* [in] */ const String& tag,
            /* [in] */ const String& data);
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI SetReporter(
        /* [in] */  IReporter* reporter);

    /**
     * Returns non-null Reporter.
     */
    CARAPI GetReporter(
        /* [out] */  IReporter** reporter);

    CARAPI AddData(
        /* [in] */  const String& tag,
        /* [in] */  ArrayOf<Byte>* data,
        /* [in] */  Int32 flags);

    CARAPI AddText(
        /* [in] */  const String& tag,
        /* [in] */  const String& data);

private:
    static AutoPtr<IReporter> REPORTER;

};

} // namespace IO
} // namespace Libcore
#endif
