
#ifndef __ELASTOS_DROID_SERVER_REMOTE_ZIGBEEFACTORY_H__
#define __ELASTOS_DROID_SERVER_REMOTE_ZIGBEEFACTORY_H__

#include "remote/AbstractDeviceFactory.h"
#include "elastos/droid/os/AsyncTask.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IParcelStringList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class ZigbeeFactory : public AbstractDeviceFactory
{
private:
    class ParsingManager
    {
    public:
        /*
         *  Split a string with specific spliter
         *  Parameters: string need to be split, spliter
         *  Returns: string in a list - success, NULL- fail
        */
        static CARAPI_(AutoPtr<ArrayOf<String> >) Split(
            /* [in] */ const String& str);

        /*
         *  Combine a two-diamension array to a string: use "?" to split fields, use "$" to end a line
         *  Ex. zigbee1,zigbee2 ,group1(contains device "zigbee1,zigbee2"): combined string as following:
         *      2?1$zigbee1?1?258?65535?1$zigbee2?1?258?65535?1$zigbee2?1?258?65535?1$group1?258?65535?zigbee1?zigbee2$
         *      1'st segemnt: 2 means 2 devices,   1 means 1 group
         *      2'nd segement: zigbee1(device name), 1(device online status), 258(device type), 65535(device zone type),
         *                                          1(device state: on/off state - bulb, temperature - temperature sensor, door state -  door sensor)
         *      3'rd segement: group1(group name), 258(group type), 65535(group zone type), zigbee1?zigbee2 (device in the group)
         *  Parameters: string need to be combined, spliter
         *  Returns: combined string - success, NULL- fail
        */
        static CARAPI_(String) Combine(
            /* [in] */ ArrayOf<IParcelStringList*>* list);

    private:
        static const String mSpliter; //check split function if this char changes
        static const String mEnd;
    };

    /********************************************************************
     * socket connection correlation task classes
     ********************************************************************/
    class SocketCmd_Task : public AsyncTask
    {
    public:
        SocketCmd_Task(
            /* [in] */ const String& cmd,
            /* [in] */ ZigbeeFactory* host);

    protected:
        CARAPI_(AutoPtr<IInterface>) DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params);

        CARAPI_(Boolean) IsFuncReturnVoid();

    private:
        String mCmd;
        Boolean mIsVoid;
        ZigbeeFactory* mHost;
    };

public:
    /********************************
     *  ZigbeeFactory Constructor
     ********************************/
    //Using public constructor for java reflection
    ZigbeeFactory(
        /* [in] */ IContext* ctx);

    CARAPI RunForResult(
        /* [in] */ const String& cmd,
        /* [out] */ String* result);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 CMDRESERVEDLENGTH = 2;
    static const Int32 CMDPAIR = 2;

    String mRemoteCmd;
    Int32 mCmdCheckSN;
    // Class<?>[] paramClass;
    // Object[] paramObj;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_ZIGBEEFACTORY_H__
