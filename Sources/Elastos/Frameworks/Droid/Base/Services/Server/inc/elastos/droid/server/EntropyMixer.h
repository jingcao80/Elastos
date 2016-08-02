
#ifndef __ELASTOS_DROID_SERVER_ENTROPY_MIXER_H__
#define __ELASTOS_DROID_SERVER_ENTROPY_MIXER_H__

#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/os/Handler.h>
#include "_Elastos.Droid.Server.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;

using Elastos::IO::IFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * A service designed to load and periodically save &quot;randomness&quot;
 * for the Linux kernel RNG and to mix in data from Hardware RNG (if present)
 * into the Linux RNG.
 *
 * <p>When a Linux system starts up, the entropy pool associated with
 * {@code /dev/random} may be in a fairly predictable state.  Applications which
 * depend strongly on randomness may find {@code /dev/random} or
 * {@code /dev/urandom} returning predictable data.  In order to counteract
 * this effect, it's helpful to carry the entropy pool information across
 * shutdowns and startups.
 *
 * <p>On systems with Hardware RNG (/dev/hw_random), a block of output from HW
 * RNG is mixed into the Linux RNG on EntropyMixer's startup and whenever
 * EntropyMixer periodically runs to save a block of output from Linux RNG on
 * disk. This mixing is done in a way that does not increase the Linux RNG's
 * entropy estimate is not increased. This is to avoid having to trust/verify
 * the quality and authenticity of the &quot;randomness&quot; of the HW RNG.
 *
 * <p>This class was modeled after the script in
 * <a href="http://www.kernel.org/doc/man-pages/online/pages/man4/random.4.html">man
 * 4 random</a>.
 */
class EntropyMixer
    : public Object
    , public IEntropyMixer
    , public IBinder
{
private:
    /**
     * Handler that periodically updates the entropy on disk.
     */
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("EntropyMixer::MyHandler")

        MyHandler(
            /* [in] */ EntropyMixer* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        EntropyMixer* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("EntropyMixer::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ EntropyMixer* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        EntropyMixer* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    /** Test only interface, not for public use */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& entropyFile,
        /* [in] */ const String& randomDevice,
        /* [in] */ const String& hwRandomDevice);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ const String& entropyFile,
        /* [in] */ const String& randomDevice,
        /* [in] */ const String& hwRandomDevice);

    void ScheduleEntropyWriter();

    void LoadInitialEntropy();

    void WriteEntropy();

    /**
     * Add additional information to the kernel entropy pool.  The
     * information isn't necessarily "random", but that's ok.  Even
     * sending non-random information to {@code /dev/urandom} is useful
     * because, while it doesn't increase the "quality" of the entropy pool,
     * it mixes more bits into the pool, which gives us a higher degree
     * of uncertainty in the generated randomness.  Like nature, writes to
     * the random device can only cause the quality of the entropy in the
     * kernel to stay the same or increase.
     *
     * <p>For maximum effect, we try to target information which varies
     * on a per-device basis, and is not easily observable to an
     * attacker.
     */
    void AddDeviceSpecificEntropy();

    /**
     * Mixes in the output from HW RNG (if present) into the Linux RNG.
     */
    void AddHwRandomEntropy();

    static String GetSystemDir();

private:
    static const String TAG;
    static const Int32 ENTROPY_WHAT;
    static const Int32 ENTROPY_WRITE_PERIOD;  // 3 hrs
    static const Int64 START_TIME;
    static const Int64 START_NANOTIME;

    String mRandomDevice;
    String mHwRandomDevice;
    String mEntropyFile;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_ENTROPY_MIXER_H__
