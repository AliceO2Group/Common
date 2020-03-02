// most simple daemon example
// uses dummy default doLoop() function from the class
// helps to test e.g. command line parameters settings

#include <Common/SimpleLog.h>

int main() {

  SimpleLog theLog;
  theLog.setLogFile("/tmp/test.log",100,4,0);
  theLog.info("test message");

  return 0;
}
