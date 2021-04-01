#include "processor.h"

Processor test;

using namespace std;
int main()
{
    test.process();
    // Encoding
    test.pcmEncodeSignal();
    test.a_lawEncodeSignal();
    test.mu_lawEncodeSignal();
    // Deocding
    test.pcmDecodeSignal();
    test.a_lawDecodeSignal();
    test.mu_lawDecodeSignal();
    // Test File
    test.testWrite();

    test.clearAll();   
}


