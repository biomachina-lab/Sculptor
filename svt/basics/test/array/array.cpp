// small testprogram for the svt_array class
#include <svt_array.h>
#include <svt_iostream.h>

int main()
{
    cout << "test_array: init array with one default element" << endl;

    svt_array<int> ar(1,5);
    ar.print();

    cout << "test_array: add elements" << endl;
    ar.addElement(2);
    ar.addElement(3);
    ar+=4;
    ar+=5;
    ar+=6;
    ar+=7;
    ar.print();

    cout << "test_array: find element with value 5" << endl;
    cout << "found element at index: " << ar.find(5) << endl;

    cout << "test_array: set first element to 15" << endl;
    ar[0] = 15;
    ar.print();

    cout << "test_array: ar[1]?" << endl;
    cout << ar[1] << endl;

    cout << "test_array: delete the last element" << endl;
    ar.delElement();
    ar.print();

    cout << "test_array: delete the 2nd element" << endl;
    ar.delElement(2);
    ar.print();

    cout << "test_array: delete 3 elements from pos 1" << endl;
    ar.delElements(3,1);
    ar.print();

    cout << "test_array: add 5 elements (set to 7)" << endl;
    ar.addElements(5,7);
    ar.print();
	return 0;
};
