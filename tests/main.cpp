#include "testlicense.h"
#include "teststitch.h"
#include "teststitchset.h"
#include "testcell.h"
#include "testcrochetdatamodel.h"

int main(int argc, char** argv) 
{

    QApplication app(argc, argv);
    int retval(0);

    QObject *test;

    test = new TestLicense();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;

    test = new TestStitch();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;

    test = new TestStitchSet();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;

    test = new TestCell();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;

    test = new TestCrochetDataModel();
    retval +=QTest::qExec(test, argc, argv);
    delete test;
    test = 0;


    return (retval ? 1 : 0);
}