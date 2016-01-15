#include "test.h"
#include <stdio.h>
#include <elautoptr.h>
#include "Elastos.CoreLibrary.h"

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Text;
using namespace Libcore::ICU;

// private static byte[] serialize(Object object) throws IOException {
//     ByteArrayOutputStream out = new ByteArrayOutputStream();
//     new ObjectOutputStream(out).writeObject(object);
//     return out.toByteArray();
// }

// private static Object deserialize(byte[] bytes) throws IOException, ClassNotFoundException {
//     ObjectInputStream in = new ObjectInputStream(new ByteArrayInputStream(bytes));
//     Object result = in.readObject();
//     assertEquals(-1, in.read());
//     return result;
// }

// void assertSameReserialized(Object o)
// {
//     assertSame(o, SerializationTester.reserialize(o));
// }

int CTest::testSerialization(int argc, char* argv[])
{
    // assertSameReserialized(AttributedCharacterIterator.Attribute.LANGUAGE);
    // assertSameReserialized(DateFormat.Field.ERA);
    // assertSameReserialized(DateFormat.Field.TIME_ZONE);
    // assertSameReserialized(NumberFormat.Field.INTEGER);
    return 0;
}

int CTest::testSerializingSubclass(int argc, char* argv[])
{

    return 0;
}
