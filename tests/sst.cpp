//"(C) Andrew L. Heilveil, 2017"
#include "logger.h"
#include "application.h"
#include "amptekanalyzer.h"

static AmptekAnalyzer *root=nullptr; //easy handle for debugger access to what is otherwise hidden.
/** the 'true' main is ConsoleApplication::main*/
int main(int argc, char *argv[]){
  AmptekAnalyzer app(unsigned(argc),argv);
  root=&app;
  if(app.failure(app.main())) {
    switch(app.errornumber) {//see AmptekAnalyzer::doHostCommand for matching these code to their actions.
    case EL3RST: //reboot
      system("sudo /bin/systemctl reboot");
      return -1;
    case EL3HLT: //halt system
      system("sudo /bin/systemctl halt");
      return -1;
    }
    wtf("Program exit code:%d, may mean: %s",app.errornumber,app.errorText());
  } else {
    wtf("Normal program exit");
  }
  return app.errornumber;
} // main
