#define fehtid 1

#include "stdio.h"
#include <initializer_list>  //for inline test data sets

#include "safestr.h"
#include "logger.h"

#include "unistd.h" //getcwd

#include "cheaptricks.h"

#include "textpointer.h"
//rpi i2c interface to commonly used OLED driver chip
#include "SSD1306.h"

#include "din.h"
#include "application.h"

static const u8 bybye[]="Bye Bye!";

#if fehtid
#include "signal.h"
#endif

struct BonnetDemo: Application {

  class ButtonTracker {
    Din pin;
  public:
    bool isPressed=false;
    unsigned toggles=0;
    //how many calls to changed did not result in a change
    unsigned steady=0;
    const char id;//A,B,C,D,L,R,U;
    const unsigned pinnum=~0U;
public:

    ButtonTracker(char id,unsigned pinnum):id(id),pinnum(pinnum){
      //#do nothing here
    }

    void connect(){
      if(pinnum!=~0U){//if valid pin number
        pin.beGpio(pinnum,0,1);
      }
    }

    bool changed(){
      if(::changed(isPressed,pin.readpin())){
        ++toggles;
        steady=0;
        return true;
      } else {
        ++steady;
        return false;
      }
    }

    operator bool()const{
      return pin.readpin();
    }
  };

  ButtonTracker but[7]={{'A',5},{'B',6},{'C',4},{'D',22},{'L',27},{'R',23},{'U',17}};//
  SSD1306 hat;
  SSD1306::FrameBuffer fb;

  //avoiding reliance on stdin and stdout since we intend in using WiFi or ethernet to talk to devices.
  Fildes cin;
  Fildes cout;

  BonnetDemo(unsigned argc, char *argv[]):Application(argc,argv),
  hat({128,64,true,~0U}),
  fb(64),
  cin("console"),
  cout("console"){
    cin.preopened(STDIN_FILENO,false);//let us not close the console, let the OS tend to that.
    cin.setBlocking(false);//since available() is lying to us ...
    cout.preopened(STDOUT_FILENO,false);
    cout.setBlocking(false);
    fb.clear(1);
  }

  char watched='B';

#if fehtid
  pid_t fehpid=0;

  bool pidByName(){
    char buf[512];
    FILE * cmd_pipe=popen("pidof -s feh","r");
    if(!cmd_pipe){
      return false;
    }

    char *pidtext=fgets(buf,sizeof(buf),cmd_pipe);
    if(!pidtext){
      fehpid=0;//by clearing here we can poll the pid to see if the program is still running.
      return false;
    }
    fehpid=strtoul(pidtext,nullptr,10);
    pclose(cmd_pipe);
    return true;
  }
#endif

  /** like Arduino setup() */
  int main(){
    grabPins();
    if(hat.connect()){
      hat.begin();
      period=0.001;//want millisecond timing to match Arduino best practice.
      return Application::run();//all activity from this point on is via callbacks arranged in the previous few lines.
    } else {
      return 2;//hat.dev.errornumber;
    }
  }

  /** like Arduino loop() */
  bool keepAlive() override {
    bool dirty=false;
    int incoming=cin.available();
    if(incoming>0){
      u8 cmd[incoming+1];
      cin.read(cmd,incoming);
      cmd[incoming]=0;//guarantee null terminator so we can use naive string routines.
      switch(cmd[0]){
      case ' ':
#if fehtid
        dbg("Feh: %d",fehpid);
#endif
        for(unsigned pi=countof(but);pi-->0;){
          ButtonTracker &it(but[pi]);
          dbg("%c:%d for %d %s",it.id, it.isPressed, it.steady,(it.id==watched)?"<-":"");
        }
        break;
      case 'x'://gently quit this application
        cout.write(bybye,sizeof(bybye));
        return false;
      case 'z':
        dirty|=zebra();//single bar OR so that zebra is called even if we have already buffered something to show.
        break;
      case '.':
        dirty=true;
        break;
      }
    }
#if fehtid

    if(!fehpid){
      if(pidByName()){
        dbg("Feh is %d",fehpid);
      }
    } else {
      if(!pidByName()){
        dbg("Feh went offline");
      }
    }
#endif
    for(unsigned pi=countof(but);pi-->0;){
      ButtonTracker &it(but[pi]);
      if(it.changed()){        
        dbg("%c[%d] is now %d, toggled: %d",it.id,it.pinnum,it.isPressed,take(it.toggles));
      }
      if(it.id==watched){
        if(it.steady>=333){
          if(it.isPressed){
            it.steady=0;//auto repeat at debounce rate.
            dbg("Firing %c",it.id);
#if fehtid
            if(fehpid) {//checking, although kill process 0 is probably ignored, and even if not then it will ignore sigusr1
              kill(fehpid,SIGUSR1);
            }
#endif
          }
        }
      }
    }

    if(!hat.busy()){
      if(flagged(dirty)){
        dbg("now will refresh");
        hat.refresh(fb);
      }
    }
    return true;
  }

  bool zebra(){
    for(unsigned line=fb.segspan;line-->0;){
      for(unsigned page=fb.stride;page-->0;){
        fb(page,line)= 1<<(line&7);//something other than rectangles.
      }
    }
    return true;
  }

  void grabPins(){
    for(unsigned pi=countof(but);pi-->0;){
      ButtonTracker &it(but[pi]);
      it.connect();
    }
  }

};




int main(int argc, char *argv[]){
BonnetDemo demo(argc,argv);
  demo.main();

  dbg("tests completed \n");
  return 0;
} // main
