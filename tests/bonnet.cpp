
#include "stdio.h"
#include <initializer_list>  //for inline test data sets

#include "safestr.h"
#include "logger.h"

#include "unistd.h" //getcwd

#include "cheaptricks.h"

#include "textpointer.h"
//rpi i2c
#include "SSD1306.h"
#include "din.h"
#include "application.h"


static const u8 bybye[]="Bye Bye!";



struct BonnetDemo: Application {

  class ButtonTracker {
    Din pin;
  public:
    bool isPressed=false;
    unsigned toggles=0;
    const char id;//A,B,C,D,L,R,U;
    const unsigned pinnum=~0U;
public:

    ButtonTracker(char id,unsigned pinnum):id(id),pinnum(pinnum){
      //#do nothing here
    }

    void connect(){
      if(pinnum!=~0U){
        pin.beGpio(pinnum,0,1);
      }
    }

    bool changed(){
      if(::changed(isPressed,pin.readpin())){
        ++toggles;
        return true;
      } else {
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
      case 'x':
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

    for(unsigned pi=countof(but);pi-->0;){
      ButtonTracker &it(but[pi]);
      if(it.changed()){
        dbg("%c[%d] is now %d, toggled: %d",it.id,it.pinnum,it.isPressed,take(it.toggles));
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
