#include "hassettings.h" //numIDs
#include "art.h"
#include "artcore.h"
#include "permalloc.h"
bool VersionInfo::setParams(ArgSet &args){
  set(format,args);
  set(buildId,args);
  return isModified();
}

void VersionInfo::getParams(ArgSet &args) const{
  if(format){//then we are a proxy and vi is the remote's version which we append to ours
    args.next()=format;
    args.next()=buildId;
  }
}

double ArtController::heartRate(double divider){
  return divider>0? ratio(ArtHeartRate,divider):0;
}

void ArtController::onHeartbeat(HasSettings&hs,SettingsGrouper &sg){
  if(cv.beAutoReporting){
    for(ReportSet scanner(reporter);scanner.hasNext();){
      scanner.next().doLogic(hs);
    }
    if(cv.bootFlag){
      post('M');//spam these until master shuts us down.
    }
    if(cv.fullSync){
      sg.sendAll();
    }
  }
}
/////////////////////
void CoreValues::init(){
  beAutoReporting=1;
  bootFlag=1;
  fullSync.freeze();
}

void CoreValues::run(){
  beAutoReporting=1;
  bootFlag=0;
}

bool CoreValues::setParams(ArgSet &args){
  set(beAutoReporting,args);
  set(bootFlag,args);
  return isModified();
}


void CoreValues::getParams(ArgSet &args)const{
  args.next()=beAutoReporting;
  args.next()=bootFlag;
}
///////////////////

void Reporter::doLogic(HasSettings&unit){
  if(pwm()){
    unit.post(id);//this is what requires all periodic reports be 'Mx' commands, or 'Qx'
  }
}
unsigned numIDSfuckitalltohell(const char *ids){//linker cna't find this although IDE is quite happy
  return 1;//just want a compile
}

ReportSet::ReportSet(const char *ids):
  Indexer< Reporter >(StaticBuffer(Reporter,numIDS(ids))) {
  //Indexer< Reporter >(new Reporter[strlen(ids)](), strlen(ids) * sizeof(Reporter)) {
  isMaster=true;
  for(ReportSet scanner(*this);scanner.hasNext();){//same index as pmap.
    Reporter &rep(scanner.next());
    rep.id=*ids++;
    rep.freeze();//zero init twiddle would "spam hard"
  }
}

//we only clone for scanning:
ReportSet::ReportSet(const ReportSet &other):Indexer<Reporter>(other,false/*all allocated*/){
  isMaster=false;
}

ReportSet::~ReportSet(){
  if(isMaster){
    //delete[] buffer;
    StaticFree(buffer);
  }
}

bool ReportSet::setFrom(ConstArgSet &args){
  double etc=0;//last one present feeds all the others.
  for(ReportSet scanner(*this);scanner.hasNext();){
    etc=args.next(etc);
    scanner.next().setDivider(ArtController::heartRate(etc));
  }
  return isModified();
}

unsigned ReportSet::numParams()const {
  return allocated();
}

bool ReportSet::setParams(ArgSet &args){
  ConstArgSet rates(args);
  return setFrom(rates);
}

void ReportSet::getParams(ArgSet &args)const{
  for(ReportSet scanner(*this);scanner.hasNext();){
    double divider=scanner.next().getDivider();
    args.next()= ArtController::heartRate(divider);
  }
}

void ArtController::setFrom(double keepAlive, ConstArgSet &table){
  reporter.setFrom(table);
  cv.fullSync.setDivider(heartRate(keepAlive));
}

ArtController::ArtController(const char*ids)://
  HasSettings("VHMS"),
  vi(),
  reporter(ids){
  cv.init();
}

bool ArtController::setParam(ID fieldID, ArgSet&args){
  switch(fieldID){
  default:
    return false;
  case 'V'://maydo: version compatibility check, flash an LED if not tolerable.
    vi[0].setParams(args);
    vi[1].setParams(args);
    break;
  case 'H':
    reporter.setParams(args);
    break;
  case 'M':
    cv.setParams(args);
    break;
  case 'S':
    cv.fullSync.setDivider(heartRate(args.next()));
  }
  return true;
}

bool ArtController::getParam(ID fieldID, ArgSet&args) const {
  switch(fieldID){
  default:
    return false;
  case 'V':
    reportVersions(args);
    vi[0].getParams(args);
    vi[1].getParams(args);
    break;
  case 'H':
    reporter.getParams(args);
    break;
  case 'M':
    cv.getParams(args);
    break;
  case 'S':
    args.next()= ArtController::heartRate(cv.fullSync.getDivider());
    break;
  }
  return true;
}
/** only makes sense to call this from a proxy/master where twin is the desired config*/
bool ArtController::bootCheck(ArtController &twin){
  if(flagged(cv.bootFlag)){//if remote booted
    twin.cv.bootFlag=0; //clear flag
    post('M'); //send to it.
    return true;
  } else {
    return false;
  }
}

void ArtController::reportVersions(ArgSet &args){
  args.next() = 1.0; //interface revision.
  args.next() =  //build id
               #include "svnrevision.txt"
                 ;
}
