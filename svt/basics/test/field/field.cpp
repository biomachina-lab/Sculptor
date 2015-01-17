// small testprogram for the svt_field class
#include <svt_field.h>

int main()
{
    cout << "test_field: init fields A,B and C (A and B immediate mode, C flush mode)" << endl;

    svt_field<int> fA;
    svt_field<int> fB;
    svt_field<int> fC(SVT_FIELD_FLUSH);

    cout << "test_field: setting the data of A to 5, B to 17 and C to 15" << endl;
    fA.set(5);
    fB.set(17);
    fC.set(15);
    fA.print();
    fB.print();
    fC.print();

    cout << "test_field: connecting A with B and C with A" << endl;
    fA.connectTo(&fB);
    fC.connectTo(&fA);
    fA.print();
    fB.print();
    fC.print();

    cout << "test_field: setting A to 3" << endl;
    fA.set(3);
    fA.print();
    fB.print();
    fC.print();

    cout << "test_field: disconnecting A from B" << endl;
    fA.disconnectFrom(&fB);
    fA.print();
    fB.print();
    fC.print();

    cout << "test_field: setting C to 5" << endl;
    fC.set(5);
    fA.print();
    fB.print();
    fC.print();

    cout << "test_field: flushing C" << endl;
    fC.flush();
    fA.print();
    fB.print();
    fC.print();
};
