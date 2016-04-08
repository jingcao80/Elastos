
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CBACKDROPVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CBACKDROPVIEW_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * Convenience class for capturing gestures for later analysis.
 */
class GestureRecorder : public Object
{
private:
    class GHandler: public Handler
    {
    public:
        GHandler(
            /* [in] */ GestureRecorder* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GestureRecorder* mHost;
    };

public:
    class Gesture : public Object
    {
    public:
        class Record : public Object
        {
        public:
            virtual CARAPI_(String) ToJson() = 0;

        public:
            Int64 mTime;
        };

        class MotionEventRecord : public Record
        {
        public:
            MotionEventRecord(
                /* [in] */ Int64 when,
                /* [in] */ IMotionEvent* event);

            CARAPI_(String) ActionName(
                /* [in] */ Int32 action);

            CARAPI_(String) ToJson();

        public:
            AutoPtr<IMotionEvent> mEvent;
        };

        class TagRecord : public Record
        {
        public:
            TagRecord(
                /* [in] */ Int64 when,
                /* [in] */ const String& tag,
                /* [in] */ const String& info);

            CARAPI_(String) ToJson();

        public:
            String mTag;
            String mInfo;
        };

    public:
        Gesture();

        CARAPI_(void) Add(
            /* [in] */ IMotionEvent* ev);

        CARAPI_(void) Tag(
            /* [in] */ Int64 when,
            /* [in] */ const String& tag,
            /* [in] */ const String& info);

        CARAPI_(Boolean) IsComplete();

        CARAPI_(String) ToJson();

    private:
        AutoPtr<ILinkedList> mRecords;  /*<Record*/
        HashSet<String> mTags;
        Int64 mDownTime;
        Boolean mComplete;
    };

public:
    GestureRecorder(
        /* [in] */ const String& filename);

    CARAPI_(void) Add(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) Tag(
        /* [in] */ Int64 when,
        /* [in] */ const String& tag,
        /* [in] */ const String& info);

    CARAPI_(void) Tag(
        /* [in] */ Int64 when,
        /* [in] */ const String& tag);

    CARAPI_(void) Tag(
        /* [in] */ const String& tag);

    CARAPI_(void) Tag(
        /* [in] */ const String& tag,
        /* [in] */ const String& info);

    /**
     * Generates a JSON string capturing all completed gestures.
     * Not threadsafe; call with a lock.
     */
    CARAPI_(String) ToJsonLocked();

    CARAPI_(String) ToJson();

    CARAPI_(void) SaveLater();

    CARAPI_(void) Save();

    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

public:
    static const Boolean DEBUG; // for now
    static const String TAG;

private:
    // -=-=-=-=-=-=-=-=-=-=-=-

    static const Int64 SAVE_DELAY; // ms
    static const Int32 SAVE_MESSAGE;

    AutoPtr<ILinkedList> mGestures;  /*<Gesture*/
    AutoPtr<Gesture> mCurrentGesture;
    Int32 mLastSaveLen;
    String mLogfile;

    AutoPtr<IHandler> mHandler;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CBACKDROPVIEW_H__
