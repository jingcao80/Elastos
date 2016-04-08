#ifndef __PATTTERN_TEST_H
#define __PATTTERN_TEST_H

#include <stdio.h>
#include <elastos.h>
#include <elautoptr.h>
#include "Elastos.CoreLibrary.h"
#include "elastos/StringBuilder.h"

using namespace Elastos;
using namespace Elastos::Core;
using namespace Elastos::Text;
using namespace Libcore::ICU;
using namespace Elastos::Utility;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest();

public:
    int main(int argc, char* argv[]);
    int test_11155(int argc, char* argv[]);
};


#endif //__PATTTERN_TEST_H
