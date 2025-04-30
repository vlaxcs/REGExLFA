#include "Tester.h"

using namespace std;
int main() {

    Tester &app = Tester::getInstance();
    app.init();
    app.run();
}