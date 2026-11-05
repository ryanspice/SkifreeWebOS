#include "skifree/classic.hpp"
#include "skifree/recreation.hpp"
#include "skifree/assets.hpp"
#include "skifree/presentation.hpp"
#include "skifree/scores.hpp"
#include "skifree/score_store.hpp"
#include "spice/render.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <thread>
#ifdef SKIFREE_WEBOS
#include <unistd.h>
#endif

namespace {
using spice::Color;
using spice::Rect;
constexpr Color Ink{0.08f,0.14f,0.20f,1}, Blue{0.03f,0.30f,0.49f,1}, White{1,1,1,1};
constexpr Color NoteInk{0.12f,0.24f,0.68f,1};
// Small built-in bitmap alphabet. No system or redistributed font is required.
const std::map<char,std::array<unsigned char,7>> Glyphs{
 {'A',{14,17,17,31,17,17,17}},{'B',{30,17,17,30,17,17,30}},{'C',{14,17,16,16,16,17,14}},
 {'D',{30,17,17,17,17,17,30}},{'E',{31,16,16,30,16,16,31}},{'F',{31,16,16,30,16,16,16}},
 {'G',{14,17,16,23,17,17,15}},{'H',{17,17,17,31,17,17,17}},{'I',{31,4,4,4,4,4,31}},
 {'J',{7,2,2,2,18,18,12}},{'K',{17,18,20,24,20,18,17}},{'L',{16,16,16,16,16,16,31}},
 {'M',{17,27,21,21,17,17,17}},{'N',{17,25,21,19,17,17,17}},{'O',{14,17,17,17,17,17,14}},
 {'P',{30,17,17,30,16,16,16}},{'Q',{14,17,17,17,21,18,13}},{'R',{30,17,17,30,20,18,17}},
 {'S',{15,16,16,14,1,1,30}},{'T',{31,4,4,4,4,4,4}},{'U',{17,17,17,17,17,17,14}},
 {'V',{17,17,17,17,17,10,4}},{'W',{17,17,17,21,21,21,10}},{'X',{17,17,10,4,10,17,17}},
 {'Y',{17,17,10,4,4,4,4}},{'Z',{31,1,2,4,8,16,31}},
 {'0',{14,17,19,21,25,17,14}},{'1',{4,12,4,4,4,4,14}},{'2',{14,17,1,2,4,8,31}},
 {'3',{30,1,1,14,1,1,30}},{'4',{2,6,10,18,31,2,2}},{'5',{31,16,16,30,1,1,30}},
 {'6',{14,16,16,30,17,17,14}},{'7',{31,1,2,4,8,8,8}},{'8',{14,17,17,14,17,17,14}},
 {'9',{14,17,17,15,1,1,14}},{'.',{0,0,0,0,0,12,12}},{':',{0,12,12,0,12,12,0}},
 {'-',{0,0,0,31,0,0,0}},{'/',{1,1,2,4,8,16,16}},{'+',{0,4,4,31,4,4,0}},
 {'!',{4,4,4,4,4,0,4}},{'?',{14,17,1,2,4,0,4}},{'&',{12,18,20,8,21,18,13}},
 {'<',{2,4,8,16,8,4,2}},{'>',{8,4,2,1,2,4,8}},{'(',{2,4,8,8,8,4,2}},{')',{8,4,2,2,2,4,8}},
 {'e',{0,0,14,17,31,16,14}},{'b',{16,16,22,25,17,17,30}}
};
void prepareFont(spice::Renderer& renderer) {
  const std::array<std::pair<std::string,Color>,4> palettes{{{"font-ink",Ink},{"font-white",White},{"font-red",{0.8f,0.05f,0.12f,1}},{"font-note",NoteInk}}};
  for(const auto& [name,color]:palettes) {
    std::vector<unsigned char> pixels(768*8*4,0);
    for(const auto& [ch,rows]:Glyphs)for(int y=0;y<7;++y)for(int x=0;x<5;++x)if(rows[static_cast<std::size_t>(y)]&(1<<(4-x))) {
      const auto offset=static_cast<std::size_t>((y*768+static_cast<unsigned char>(ch)*6+x)*4);
      pixels[offset]=static_cast<unsigned char>(color.r*255);pixels[offset+1]=static_cast<unsigned char>(color.g*255);pixels[offset+2]=static_cast<unsigned char>(color.b*255);pixels[offset+3]=255;
    }
    renderer.loadPixels(name,768,8,pixels.data());
  }
}
float textWidth(const std::string& value,float scale) {return value.empty()?0:(6*static_cast<float>(value.size())-1)*scale;}
void text(spice::Renderer& r,const std::string& value,float x,float y,float scale=2,Color color=Ink,bool exactCase=false) {
  const std::string atlas=color.r>0.7f&&color.g>0.7f?"font-white":color.r>0.6f&&color.g<0.2f?"font-red":color.b>0.6f?"font-note":"font-ink";
  for (const char raw : value) {
    const char c=!exactCase&&raw>='a'&&raw<='z'?static_cast<char>(raw-'a'+'A'):raw;
    const auto it=Glyphs.find(c);
    if(it!=Glyphs.end())r.sprite(atlas,{static_cast<float>(static_cast<unsigned char>(c)*6),0,5,7},{x,y,5*scale,7*scale});
    x+=6*scale;
  }
}
void centeredText(spice::Renderer& r,const std::string& value,float center,float y,float scale=2,Color color=Ink) {
  text(r,value,center-textWidth(value,scale)/2,y,scale,color);
}
void versionNote(spice::Renderer& r,const ski::classic::Draw& version) {
  // Overlay only: preserve the original Version 1.04 resource and its credit.
  const float x=(version.x+36)*1.5f,y=(version.y+3)*1.5f;
  const std::array<spice::Point,6> scribble{{{x-2,y+3},{x+29,y-1},{x+2,y+6},{x+28,y+3},{x-1,y+1},{x+30,y+5}}};
  for(std::size_t i=1;i<scribble.size();++i)r.line(scribble[i-1].x,scribble[i-1].y,scribble[i].x,scribble[i].y,NoteInk,1.4f);
  text(r,"WebOS 1.0.0",version.x*1.5f+24,(version.y+version.height)*1.5f+6,1.5f,NoteInk,true);
}
std::string jsonString(const std::string& value) {
  std::string result="\"";
  for(char c:value) { if(c=='"'||c=='\\') result+='\\'; if(static_cast<unsigned char>(c)>=32) result+=c; }
  return result+'"';
}
struct Options {
  int width=1280,height=720,frames=0;
  bool fullscreen=false,fixed=false,logInput=false;
  std::filesystem::path assets,save,script,capture,report,remoteMap;
};
Options options(int argc,char** argv) {
  Options o;
#ifdef SKIFREE_WEBOS
  o.fullscreen=true; o.width=1920; o.height=1080;
#endif
  auto number=[](const std::string& v,int low,int high){std::size_t used=0;int n=std::stoi(v,&used);if(used!=v.size()||n<low||n>high)throw std::runtime_error("Invalid numeric option");return n;};
  for(int i=1;i<argc;++i) {
    const std::string a=argv[i];
    auto next=[&](){if(i+1>=argc)throw std::runtime_error("Missing option value: "+a);return std::string(argv[++i]);};
    if(a=="--assets")o.assets=next(); else if(a=="--save")o.save=next();
    else if(a=="--width")o.width=number(next(),640,7680); else if(a=="--height")o.height=number(next(),360,4320);
    else if(a=="--frames")o.frames=number(next(),1,100000); else if(a=="--script")o.script=next();
    else if(a=="--capture")o.capture=next(); else if(a=="--report")o.report=next();
    else if(a=="--remote-map")o.remoteMap=next(); else if(a=="--fullscreen")o.fullscreen=true;
    else if(a=="--fixed-frame")o.fixed=true; else if(a=="--log-input")o.logInput=true;
    else if(a=="--help") {std::cout<<"SkiFree for webOS 1.0.0\n--assets DIR --save FILE --fullscreen --width N --height N\n--remote-map FILE --log-input\nDesktop QA: --frames N --fixed-frame --script FILE --capture PNG --report JSON\nArrows steer. Click/OK jumps. Insert is a small jump. F toggles fast mode. F2 restarts. F3/Back pauses.\nNative adaptation with Original and Recreation. Original game and artwork: Chris Pirih.\n";std::exit(0);}
#ifdef SKIFREE_WEBOS
    else if(i==1&&a.size()>=2&&a.front()=='{'&&a.back()=='}') {} // webOS launch parameters.
#endif
    else throw std::runtime_error("Unknown option: "+a);
  }
#ifdef SKIFREE_WEBOS
  if(o.fixed||!o.script.empty())throw std::runtime_error("Scripted QA is desktop-only");
#endif
  if(o.assets.empty()) {
    const auto exe=std::filesystem::absolute(argv[0]).parent_path();
    for(const auto& dir:{exe/"assets",exe.parent_path().parent_path()/"assets",std::filesystem::current_path()/"assets"})
      if(std::filesystem::is_regular_file(dir/"manifest.json")){o.assets=dir;break;}
    if(o.assets.empty())o.assets=exe/"assets";
  }
  return o;
}
enum class Screen { Setup, Run, Pause, Help, Initials, Scores, Versions, Quit };
enum class Edition { Original, Recreation, Chase };
struct App {
  Screen screen=Screen::Setup,helpReturn=Screen::Pause,quitReturn=Screen::Setup;
  int selection=0;
  bool running=true,recordWritable=true,assetsReady=false,retryAssets=false,pointerActive=false;
  std::string assetError;
  std::optional<spice::Point> lastPointer;
  std::filesystem::path save;
  ski::scores::Store scoreStore;
  ski::scores::StoreStatus scoreStatus=ski::scores::StoreStatus::Disabled;
  ski::scores::Tables scores;
  std::string initials="AAA";
  bool initialsPending=false;
  std::string resultSeen;
  ski::scores::Placement scorePlacement;
  std::size_t scoreCourse=0;
  bool browsingScores=false;
  std::uint64_t resultSerial=0;
  bool deterministic=false;
  std::uint32_t seed=1;
  Edition edition=Edition::Original;
  std::array<ski::recreation::Game,2> recreation;
  std::array<bool,2> recreationStarted{};
  bool originalIntro=true;
  bool originalHudStarted=false;
  ski::recreation::Game& extra(){return recreation[edition==Edition::Chase?1:0];}
  const ski::recreation::Game& extra()const{return recreation[edition==Edition::Chase?1:0];}
  std::string editionName()const{return edition==Edition::Original?"ORIGINAL 1.04":edition==Edition::Recreation?"RECREATION":"YETI CHASE";}
  void freeze(bool paused) {
    if(edition==Edition::Original)ski::classic::pause(paused);
    else if(paused)extra().pause();else extra().resume();
  }
  void choose(Edition value) {
    if(!assetsReady)return;
    freeze(true);edition=value;
    if(value!=Edition::Original) {
      const auto i=value==Edition::Chase?1u:0u;
      if(!recreationStarted[i]){extra().start(++seed,value==Edition::Chase);recreationStarted[i]=true;}
    }
    freeze(false);screen=Screen::Run;selection=0;lastPointer.reset();pointerActive=false;
  }
  bool colorsVisible()const {
    const auto st=ski::classic::state();
    return screen!=Screen::Initials&&(screen!=Screen::Run||(edition==Edition::Original&&originalIntro&&st.x==0&&st.y==0));
  }
  explicit App(const std::filesystem::path& path,const std::filesystem::path& legacyDirectory={}):save(path.empty()?path:path.parent_path()/"scores-initials-v1.txt"),
      scoreStore(save,legacyDirectory.empty()?(path.empty()?path:path.parent_path()/"scores-v104.txt"):legacyDirectory/"scores-initials-v1.txt",
                 legacyDirectory.empty()?std::filesystem::path{}:legacyDirectory/"scores-v104.txt") {
    scoreStatus=scoreStore.load(scores);
    recordWritable=scoreStatus==ski::scores::StoreStatus::Ok||scoreStatus==ski::scores::StoreStatus::Missing;
    // Prove creation/replacement before advertising that scores can be saved.
    // This writes only a successfully loaded table; invalid/recovery files stay untouched.
    persist();
  }
  void persist() {
    if(initialsPending&&scorePlacement.qualified())scores[resultCourse()][scorePlacement.rank-1].initials=initials;
    if(!recordWritable)return;
    scoreStatus=scoreStore.save(scores);
    recordWritable=scoreStatus==ski::scores::StoreStatus::Ok;
  }
  std::size_t resultCourse()const {return resultSeen=="SS"?0u:resultSeen=="GS"?1u:2u;}
  void acceptInitials() {
    persist();initialsPending=false;screen=Screen::Scores;selection=0;
  }
  void cycleInitial(int direction) {
    if(selection<3)initials[static_cast<std::size_t>(selection)]=static_cast<char>('A'+(initials[static_cast<std::size_t>(selection)]-'A'+direction+26)%26);
  }
  Rect initialRect(int slot,int part)const {
    return {350.0f+static_cast<float>(slot)*90,part==0?222.0f:part==1?258.0f:322.0f,80,part==1?56.0f:30.0f};
  }
  void start() {
    if(edition==Edition::Original){
      const auto runSeed=deterministic?++seed:static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
      ski::classic::reset(runSeed);resultSeen.clear();resultSerial=0;originalIntro=true;originalHudStarted=false;
    }
    else {extra().start(++seed,edition==Edition::Chase);recreationStarted[edition==Edition::Chase?1:0]=true;}
    screen=Screen::Run;selection=0;pointerActive=false;lastPointer.reset();
  }
  std::vector<std::string> buttons() const {
    switch(screen) {
      case Screen::Setup:return {"CHECK AGAIN","EXIT"};
      case Screen::Pause:return {"RESUME","NEW RUN","CONTROLS",edition==Edition::Original?(ski::classic::state().fast?"FAST MODE: ON":"FAST MODE: OFF"):"RESUME AND BOOST","CHOOSE A RUN","HIGH SCORES","EXIT"};
      case Screen::Help:return {"BACK"};
      case Screen::Scores:return browsingScores?std::vector<std::string>{"< COURSE","COURSE >","BACK"}:std::vector<std::string>{"CONTINUE"};
      case Screen::Initials:return {"SAVE INITIALS"};
      case Screen::Versions:return {"RED: ORIGINAL 1.04","GREEN: RECREATION","YELLOW: YETI CHASE","BACK"};
      case Screen::Quit:return {"CANCEL","EXIT"};
      default:return {};
    }
  }
  Rect buttonRect(int i)const {
    if(screen==Screen::Scores&&browsingScores)return {250.0f+static_cast<float>(i)*155,420,150,32};
    if(screen==Screen::Pause)return {310,223.0f+static_cast<float>(i)*34,340,32};
    return {310,screen==Screen::Help||screen==Screen::Scores||screen==Screen::Initials?420.0f:(screen==Screen::Setup?365.0f:235.0f)+static_cast<float>(i)*38,340,32};
  }
  void pause() {if(screen==Screen::Run){freeze(true);screen=Screen::Pause;selection=0;}}
  void action(std::string key,bool repeat=false) {
    pointerActive=false;
    if(key=="enter") {
      if(screen==Screen::Run&&edition==Edition::Original){if(ski::classic::state().alive)ski::classic::key(ski::classic::Key::Enter);else start();return;}
      key="ok";
    }
    if(screen==Screen::Initials) {
      if(key=="left")selection=(selection+3)%4;
      else if(key=="right")selection=(selection+1)%4;
      else if(key=="up")cycleInitial(1);
      else if(key=="down")cycleInitial(-1);
      else if(key=="erase"&&!repeat)selection=std::max(0,selection-1);
      else if(key=="ok"&&!repeat){if(selection<3)++selection;else acceptInitials();}
      else if((key=="back"||key=="pause")&&!repeat)acceptInitials();
      else if(!repeat&&(key=="original"||key=="recreation"||key=="chase"||key=="help"||key=="restart"))acceptInitials();
      else return;
      if(key!="original"&&key!="recreation"&&key!="chase"&&key!="help"&&key!="restart")return;
    }
    if(!repeat&&assetsReady) {
      if(key=="original"){choose(Edition::Original);return;}
      if(key=="recreation"){choose(Edition::Recreation);return;}
      if(key=="chase"){choose(Edition::Chase);return;}
      if(key=="help") {
        if(screen==Screen::Help){screen=helpReturn;if(screen==Screen::Run)freeze(false);}
        else{helpReturn=screen;freeze(true);screen=Screen::Help;selection=0;}
        return;
      }
    }
    if(screen==Screen::Scores&&browsingScores) {
      if(key=="left"||key=="right") {if(!repeat)scoreCourse=(scoreCourse+(key=="left"?2u:1u))%3;return;}
      if(key=="back"||key=="pause") {if(!repeat){screen=Screen::Pause;selection=5;}return;}
    }
    if(key=="back"||key=="pause") {
      if(repeat)return;
      if(screen==Screen::Run)pause();
      else if(screen==Screen::Pause){freeze(false);screen=Screen::Run;}
      else if(screen==Screen::Help){screen=helpReturn;selection=0;if(screen==Screen::Run)freeze(false);}
      else if(screen==Screen::Versions){screen=Screen::Pause;selection=0;}
      else if(screen==Screen::Scores){screen=Screen::Run;ski::classic::pause(false);}
      else if(screen==Screen::Quit){screen=quitReturn;selection=0;}
      else{quitReturn=screen;screen=Screen::Quit;selection=0;}
      return;
    }
    if(assetsReady&&key=="restart"&&!repeat){start();return;}
    if(screen==Screen::Run) {
      if(edition!=Edition::Original) {
        if(key=="ok"&&extra().mode()==ski::recreation::Mode::GameOver&&!repeat){start();return;}
        if(key=="left")extra().turn(-1);else if(key=="right")extra().turn(1);
        else if(key=="up")extra().brake();else if(key=="down")extra().downhill();
        else if((key=="ok"||key=="jump"||key=="fast")&&!repeat)extra().action();
        return;
      }
      using K=ski::classic::Key;
      if(key=="left")ski::classic::key(K::Left);else if(key=="right")ski::classic::key(K::Right);
      else if(key=="up")ski::classic::key(K::Up);else if(key=="down")ski::classic::key(K::Down);
      else if(key=="northwest")ski::classic::key(K::NorthWest);else if(key=="northeast")ski::classic::key(K::NorthEast);
      else if(key=="southwest")ski::classic::key(K::SouthWest);else if(key=="southeast")ski::classic::key(K::SouthEast);
      else if(key=="fast"&&!repeat)ski::classic::key(K::Fast);
      else if(key=="jump"&&!repeat)ski::classic::key(K::Jump);
      else if(key=="ok"&&!repeat){if(!ski::classic::state().alive)start();else ski::classic::click();}
      return;
    }
    const auto count=static_cast<int>(buttons().size());if(!count)return;
    if(key=="up"||key=="left")selection=(selection+count-1)%count;
    else if(key=="down"||key=="right")selection=(selection+1)%count;
    else if(key=="ok"&&!repeat) {
      if(screen==Screen::Setup){if(selection==0)retryAssets=true;else running=false;}
      else if(screen==Screen::Pause) {
        if(selection==0){freeze(false);screen=Screen::Run;}
        else if(selection==1)start();
        else if(selection==2){helpReturn=screen;screen=Screen::Help;selection=0;}
        else if(selection==3){if(edition==Edition::Original)ski::classic::key(ski::classic::Key::Fast);else{freeze(false);screen=Screen::Run;extra().action();}}
        else if(selection==4){screen=Screen::Versions;selection=0;}
        else if(selection==5){screen=Screen::Scores;browsingScores=true;selection=0;}
        else{quitReturn=screen;screen=Screen::Quit;selection=0;}
      }else if(screen==Screen::Help){screen=helpReturn;selection=0;if(screen==Screen::Run)freeze(false);}
      else if(screen==Screen::Versions){if(selection<3)choose(static_cast<Edition>(selection));else{screen=Screen::Pause;selection=0;}}
      else if(screen==Screen::Scores){
        if(browsingScores){if(selection<2)scoreCourse=(scoreCourse+(selection==0?2u:1u))%3;else{screen=Screen::Pause;selection=5;}}
        else{screen=Screen::Run;ski::classic::pause(false);}
      }
      else if(screen==Screen::Quit){if(selection==1){persist();running=false;}else{screen=quitReturn;selection=0;}}
    }
  }
  void pointer(spice::Point p,bool click) {
    const bool moved=!lastPointer||std::abs(p.x-lastPointer->x)>1||std::abs(p.y-lastPointer->y)>1;
    if(!click&&!moved)return;
    lastPointer=p;pointerActive=true;
    if(screen==Screen::Initials) {
      for(int slot=0;slot<3;++slot)for(int part=0;part<3;++part)if(initialRect(slot,part).contains(p)) {
        selection=slot;if(click&&part!=1)cycleInitial(part==0?1:-1);return;
      }
      if(buttonRect(0).contains(p)){selection=3;if(click)acceptInitials();}
      return;
    }
    if(click&&assetsReady&&colorsVisible())for(int i=0;i<4;++i) {
      if(Rect{30.0f+i*230.0f,500,210,28}.contains(p)) {
        const std::array<std::string,4> actions{"original","recreation","chase","help"};action(actions[static_cast<std::size_t>(i)]);return;
      }
    }
    if(screen==Screen::Run) {
      if(edition!=Edition::Original) {
        extra().point(p.x-480,p.y-190);
        if(click){if(extra().mode()==ski::recreation::Mode::GameOver)start();else extra().action();}
        return;
      }
      // Original mouse direction logic consumes motion, not a latched target.
      if(moved)ski::classic::pointer(static_cast<int>(p.x/1.5f),static_cast<int>(p.y/1.5f));
      if(click){if(!ski::classic::state().alive)start();else ski::classic::click();}
      return;
    }
    const auto bs=buttons();for(std::size_t i=0;i<bs.size();++i)if(buttonRect(static_cast<int>(i)).contains(p)){selection=static_cast<int>(i);if(click)action("ok");break;}
  }
  void character(char value) {
    if(screen==Screen::Initials) {
      if(value>='a'&&value<='z')value=static_cast<char>(value-'a'+'A');
      if(value>='A'&&value<='Z') {
        selection=std::min(selection,2);initials[static_cast<std::size_t>(selection)]=value;++selection;
      }
      return;
    }
    if(edition==Edition::Original&&(screen==Screen::Run||screen==Screen::Pause)) {
      pointerActive=false;ski::classic::character(value);
      const auto result=ski::classic::state().result;
      if(!result.course.empty()&&result.serial!=resultSerial)finish(result);
    }
  }
  void finish(const ski::classic::Result& result) {
    resultSeen=result.course;resultSerial=result.serial;const auto c=result.course=="SS"?0u:result.course=="GS"?1u:2u;
    scoreCourse=c;browsingScores=false;
    scorePlacement=ski::scores::submit(scores[c],result.value,result.timed);
    initials="AAA";initialsPending=scorePlacement.qualified();
    persist();ski::classic::pause(true);screen=initialsPending?Screen::Initials:Screen::Scores;selection=0;
  }
  void update(double dt) {
    if(screen!=Screen::Run)return;
    if(edition!=Edition::Original){extra().advance(dt);return;}
    ski::classic::advance(dt);const auto st=ski::classic::state();
    if(!st.result.course.empty()&&st.result.serial!=resultSerial) {
      finish(st.result);
    }
  }
};
std::string screenName(Screen screen) {
  switch(screen){case Screen::Setup:return "setup";case Screen::Run:return "playing";case Screen::Pause:return "paused";case Screen::Help:return "help";case Screen::Initials:return "initials";case Screen::Scores:return "scores";case Screen::Versions:return "versions";case Screen::Quit:return "exit-confirm";}
  return "unknown";
}
std::string timeString(int ms) {
  char b[40];std::snprintf(b,sizeof b,"%02d:%02d:%02d.%02d",ms/3600000,(ms/60000)%60,(ms/1000)%60,(ms/10)%100);return b;
}
std::string bestScoreLabel(const App& app,const ski::classic::State& st) {
  const auto course=static_cast<int>(st.course)-1;
  if(course<0||course>2||app.scores[static_cast<std::size_t>(course)].empty())return "BEST: --";
  const auto& best=app.scores[static_cast<std::size_t>(course)].front();
  return "BEST: "+best.initials+" "+(course==2?std::to_string(best.value):timeString(best.value));
}
void draw(spice::Renderer& r,App& app,const spice::Viewport& view) {
  r.begin(view,White);
  const auto st=ski::classic::state();
  if(app.assetsReady&&app.edition==Edition::Original) {
    if(st.y>0)app.originalHudStarted=true;
    const auto& scene=ski::classic::draws();
    if(app.originalIntro)app.originalIntro=std::any_of(scene.begin(),scene.end(),[](const auto& d){return d.sprite==53;});
    for(const auto& raw:scene) {
      const auto d=ski::presentation::originalSprite(raw,app.originalIntro);
      char name[16];std::snprintf(name,sizeof name,"ski32_%03d",d.sprite);
      r.image(name,{d.x*1.5f,d.y*1.5f,d.width*1.5f,d.height*1.5f});
    }
    if(app.originalIntro)for(const auto& d:scene)if(d.sprite==54)versionNote(r,d);
    if(app.originalHudStarted) {
      const auto courseLabel=st.course==ski::classic::Course::Slalom?"SLALOM":st.course==ski::classic::Course::TreeSlalom?"TREE SLALOM":st.course==ski::classic::Course::Freestyle?"FREESTYLE":"FREE SKIING";
      const bool timed=st.course==ski::classic::Course::Slalom||st.course==ski::classic::Course::TreeSlalom;
      r.rect({738,0,222,113},White);r.rect({738,0,222,113},Ink,0,1);
      text(r,std::string(courseLabel)+" / LIVE",747,6,1.3f);
      const std::array<std::string,5> stats{"TIME:  "+(timed?timeString(st.elapsedMs):"--"),"DIST:  "+std::to_string(st.distance)+"M","SPEED: "+std::to_string(st.speed)+"M/S","STYLE: "+std::to_string(st.style),bestScoreLabel(app,st)};
      float y=24;for(const auto& value:stats){text(r,value,747,y,1.5f);y+=18;}
    }
    if(app.screen==Screen::Run&&st.x==0&&st.y==0) {
      r.rect({130,471,700,22},White);centeredText(r,"MOUSE AIM / ARROWS    CLICK / OK JUMP    BACK / F3 MENU",480,477,1.5f);
    }
    if(app.screen==Screen::Run&&!st.alive)centeredText(r,"CLICK / OK OR F2 TO START AGAIN",480,477,1.5f);
  }else if(app.assetsReady) {
    ski::recreation::landscape(r,app.extra());
    r.rect({28,20,258,56},White);text(r,app.editionName(),40,30,2);
    text(r,app.edition==Edition::Chase?"RECREATION / PURSUIT":"FREE SKIING",40,53,1.3f);
    r.rect({738,0,222,77},White);r.rect({738,0,222,77},Ink,0,1);
    text(r,"DIST: "+std::to_string(static_cast<int>(app.extra().distance()))+"M",747,6,1.5f);
    text(r,"STYLE: "+std::to_string(app.extra().score()),747,30,1.5f);
    const auto cooldown=static_cast<int>(std::ceil(app.extra().player().cooldown));
    text(r,cooldown>0?"BOOST: "+std::to_string(cooldown)+"S":"BOOST: READY",747,54,1.5f);
    if(app.extra().mode()==ski::recreation::Mode::GameOver)centeredText(r,"RUN COMPLETE - PRESS OK TO TRY AGAIN",480,470,2);
  }
  if(app.screen!=Screen::Run) {
    r.rect({0,0,960,540},Ink.opacity(0.22f));
    r.rect({234,102,500,372},{0,0,0,0.12f},10);r.rect({230,96,500,374},{0.97f,0.98f,0.99f,1},10);
    r.rect({230,96,500,50},Ink,10);
    const auto title=app.screen==Screen::Setup?"GAME FILES UNAVAILABLE":app.screen==Screen::Help?"CONTROLS":app.screen==Screen::Initials?"ENTER YOUR INITIALS":app.screen==Screen::Scores?"HIGH SCORES":app.screen==Screen::Versions?"CHOOSE A RUN":app.screen==Screen::Quit?"EXIT GAME?":"PAUSED";
    text(r,"SKIFREE FOR WEBOS",250,106,1.2f,White);text(r,title,250,125,2,White);
    if(app.screen==Screen::Setup) {
      text(r,"SOME GAME FILES COULD NOT LOAD.",250,185,2);text(r,"REINSTALL SKIFREE FOR WEBOS.",250,220,2);
    }else if(app.screen==Screen::Help) {
      const bool original=app.edition==Edition::Original;
      const std::array<std::string,9> lines{app.editionName(),"MOUSE / MAGIC POINTER: STEER",original?"CLICK / REMOTE OK: JUMP OR TRICK":"OK / F: BOOST OR AIR TRICK",original?"INSERT / NUMPAD 0: SMALL JUMP":"UP: BRAKE    DOWN: SKI DOWNHILL",original?"F: TOGGLE FAST MODE":"BOOST TO STAY AHEAD OF THE YETI","F2: NEW RUN    F3 / BACK: PAUSE","RED ORIGINAL / GREEN RECREATION","YELLOW YETI CHASE / BLUE HELP",original?"TIME: SLALOM / TREE SLALOM ONLY":"SWITCH RUNS WITHOUT RESTARTING"};
      float y=159;for(const auto& line:lines){text(r,line,250,y,1.7f);y+=26;}
      text(r,"ORIGINAL GAME AND ART: CHRIS PIRIH",250,397,1.35f);
    }else if(app.screen==Screen::Initials) {
      centeredText(r,"NEW TOP TEN SCORE",480,163,2);
      centeredText(r,"RANK "+std::to_string(app.scorePlacement.rank)+"  /  "+(app.scorePlacement.timed?timeString(app.scorePlacement.value):std::to_string(app.scorePlacement.value)+" STYLE"),480,192,1.7f);
      for(int slot=0;slot<3;++slot) {
        for(int part=0;part<3;++part) {
          const auto box=app.initialRect(slot,part);const bool selected=slot==app.selection;
          r.rect(box,part==1&&selected?Blue:White,5);r.rect(box,selected?Blue:Ink,5,1);
          centeredText(r,part==0?"+":part==2?"-":std::string(1,app.initials[static_cast<std::size_t>(slot)]),box.x+40,box.y+(part==1?14:7),part==1?4.0f:2.0f,part==1&&selected?White:Ink);
        }
      }
      centeredText(r,"TYPE A-Z OR USE LEFT / RIGHT, UP / DOWN",480,369,1.4f);
      centeredText(r,"OK: NEXT / SAVE    BACK: KEEP INITIALS",480,395,1.4f);
    }else if(app.screen==Screen::Pause) {
      text(r,app.editionName(),280,160,2);text(r,"F3 / BACK TO CONTINUE",300,197,1.7f);
    }else if(app.screen==Screen::Versions) {
      text(r,"ORIGINAL: CLASSIC 1.04 RULES.",270,164,1.8f);text(r,"RECREATION: A FRESH TAKE.",270,197,1.8f);
    }else if(app.screen==Screen::Scores) {
      const auto c=app.scoreCourse;
      const std::string courseLabel=c==0?"SLALOM":c==1?"TREE SLALOM":"FREESTYLE";
      text(r,(app.browsingScores?"ORIGINAL / ":"")+courseLabel,270,153,2);
      float y=178;std::size_t row=0;for(const auto& entry:app.scores[c]){
        const bool current=!app.browsingScores&&app.scorePlacement.isCurrentRow(row);
        text(r,std::to_string(++row)+". "+entry.initials+"  "+(c==2?std::to_string(entry.value):timeString(entry.value))+(current?"  <- YOU":""),260,y,1.6f);y+=21;
      }
      if(app.browsingScores) {
        if(app.scores[c].empty())centeredText(r,"NO SAVED SCORES YET",480,235,2);
        centeredText(r,"LEFT / RIGHT: COURSE    BACK: MENU",480,397,1.4f);
      }else if(!app.scorePlacement.qualified())text(r,(c==2?std::to_string(app.scorePlacement.value):timeString(app.scorePlacement.value))+"  - TRY AGAIN",260,397,1.6f);
    }else text(r,"LEAVE THE MOUNTAIN?",290,177,2);
    const auto bs=app.buttons();for(std::size_t i=0;i<bs.size();++i){const auto box=app.buttonRect(static_cast<int>(i));const bool focus=app.screen==Screen::Initials?app.selection==3:static_cast<int>(i)==app.selection;r.rect(box,focus?Blue:White,5);if(focus)r.rect(box,Blue,5,2);text(r,(focus?"> ":"  ")+bs[i],box.x+12,box.y+10,1.8f,focus?White:Ink);}
  }
  if(!app.recordWritable&&app.screen!=Screen::Run)
    centeredText(r,"HIGH SCORES ARE NOT BEING SAVED",480,477,1.5f,{0.8f,0.05f,0.12f,1});
  if(app.assetsReady&&app.colorsVisible()) {
    const std::array<Color,4> colors{{{0.65f,0.02f,0.04f,1},{0,0.36f,0.1f,1},{0.95f,0.75f,0.05f,1},{0.03f,0.23f,0.65f,1}}};
    const std::array<std::string,4> labels{"RED ORIGINAL","GREEN RECREATION","YELLOW YETI CHASE","BLUE HELP"};
    for(int i=0;i<4;++i){const float x=30.0f+i*230.0f;const bool selected=i==3?app.screen==Screen::Help:static_cast<int>(app.edition)==i;r.rect({x,500,210,28},{0.95f,0.97f,0.99f,1},5);if(selected)r.rect({x,500,210,28},Blue,5,1.5f);const float labelX=x+(210-(textWidth(labels[static_cast<std::size_t>(i)],1.5f)+18))/2;r.rect({labelX,509,10,10},colors[static_cast<std::size_t>(i)],3);text(r,labels[static_cast<std::size_t>(i)],labelX+18,510,1.5f);}
  }
  r.end();
}
struct Event {int frame;std::string action;};
int run(int argc,char** argv) {
  auto opt=options(argc,argv);
  std::map<int,std::string> keys{{80,"left"},{79,"right"},{82,"up"},{81,"down"},{40,"ok"},{44,"jump"},{9,"fast"},{59,"restart"},{60,"pause"},{41,"back"},{270,"back"},{19,"pause"},{73,"jump"},{98,"jump"},{92,"left"},{94,"right"},{96,"up"},{90,"down"},{95,"northwest"},{97,"northeast"},{89,"southwest"},{91,"southeast"},{74,"northwest"},{75,"northeast"},{77,"southwest"},{78,"southeast"}};
  keys[88]="enter";
  // Actual SDL-webOS extension values, not browser keycodes 403-406.
  const std::array<std::string,4> colorActions{"original","recreation","chase","help"};
  for(int i=0;i<4;++i){keys[486+i]=colorActions[static_cast<std::size_t>(i)];keys[62+i]=colorActions[static_cast<std::size_t>(i)];} // F5-F8 desktop equivalents.
  if(!opt.remoteMap.empty()) {
    std::ifstream in(opt.remoteMap);if(!in)throw std::runtime_error("Remote map unavailable");std::string line;
    while(std::getline(in,line)){if(line.empty()||line[0]=='#')continue;std::istringstream row(line);int code=-1;std::string action,extra;row>>code>>action;const std::array<std::string,11> valid{"left","right","up","down","ok","back","pause","original","recreation","chase","help"};if(!row||code<0||code>65535||(row>>extra)||std::find(valid.begin(),valid.end(),action)==valid.end())throw std::runtime_error("Invalid remote mapping");keys[code]=action;}
  }
  std::vector<Event> events;
  if(!opt.script.empty()) {std::ifstream in(opt.script);if(!in)throw std::runtime_error("QA script missing");Event e{};while(in>>e.frame>>e.action){if(e.frame<0||events.size()>1000)throw std::runtime_error("Invalid QA script");events.push_back(e);}if(!in.eof())throw std::runtime_error("Invalid QA script");}
  spice::Sdl sdl;sdl.SetMainReady();
  if(sdl.Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER)!=0)throw std::runtime_error(sdl.GetError());
  struct Quit{spice::Sdl& s;~Quit(){s.Quit();}} quit{sdl};
  std::filesystem::path legacyDirectory;
#ifdef SKIFREE_WEBOS
  if(opt.save.empty()) {
    legacyDirectory=std::filesystem::absolute(argv[0]).parent_path();
    // SDL-webOS GetPrefPath returns the protected install directory. The package
    // provides a sticky writable state parent; each runtime UID owns its child.
    opt.save=legacyDirectory/"state"/("user-"+std::to_string(getuid()))/"record.txt";
  }
#else
  char* pref=sdl.GetPrefPath("ryanspice","skifree-native");
  if(pref) {
    if(opt.save.empty())opt.save=std::filesystem::path(pref)/"record.txt";
    sdl.Free(pref);
  }
#endif
  std::cout<<"Game asset directory: "<<opt.assets.string()<<std::endl;
  App app(opt.save,legacyDirectory);
  app.deterministic=opt.fixed;
  if(!opt.fixed)app.seed=static_cast<std::uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count());
  sdl.GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);sdl.GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);sdl.GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
  sdl.GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);sdl.GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);
  auto* window=sdl.CreateWindow("SkiFree for webOS",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,opt.width,opt.height,SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI|(opt.fullscreen?SDL_WINDOW_FULLSCREEN_DESKTOP:0));
  if(!window)throw std::runtime_error(sdl.GetError());
  auto context=sdl.GL_CreateContext(window);
  if(!context){sdl.DestroyWindow(window);throw std::runtime_error(sdl.GetError());}
  struct Window{spice::Sdl& s;SDL_Window* w;SDL_GLContext c;~Window(){s.GL_DeleteContext(c);s.DestroyWindow(w);}} windowScope{sdl,window,context};
  const int vsync=sdl.GL_SetSwapInterval(opt.fixed?0:1)==0?sdl.GL_GetSwapInterval():0;
  spice::Renderer renderer(sdl,{});prepareFont(renderer);
  auto loadAssets=[&]() {
    for(const auto* name:ski::RequiredSprites)renderer.removeImage(name);
    app.assetsReady=false;app.assetError=ski::checkAssetFiles(opt.assets);
    if(app.assetError.empty()) {
      try {for(const auto* name:ski::RequiredSprites)renderer.loadImage(name,opt.assets/(std::string(name)+".png"));app.assetsReady=true;}
      catch(const std::exception& error) {app.assetError=error.what();for(const auto* name:ski::RequiredSprites)renderer.removeImage(name);}
    }
    if(app.assetsReady)app.start();else app.screen=Screen::Setup;app.selection=0;app.retryAssets=false;
    std::cout<<"Game assets: "<<(app.assetsReady?"ready":app.assetError)<<std::endl;
  };
  loadAssets();
  std::vector<SDL_GameController*> controllers;
  auto openController=[&](int i){if(controllers.size()<4&&sdl.IsGameController(i)){auto* c=sdl.GameControllerOpen(i);if(c)controllers.push_back(c);}};
  for(int i=0;i<sdl.NumJoysticks();++i)openController(i);
  struct Controllers{spice::Sdl& s;std::vector<SDL_GameController*>& v;~Controllers(){for(auto* c:v)s.GameControllerClose(c);}} controllerScope{sdl,controllers};
  auto viewport=[&](){int ww=0,wh=0,dw=0,dh=0;sdl.GetWindowSize(window,&ww,&wh);sdl.GL_GetDrawableSize(window,&dw,&dh);return spice::Viewport::make(ww,wh,dw,dh);};
  auto view=viewport();bool focused=true,shown=true,minimized=false;int frame=0;int axisDirection=0;
  std::array<bool,4> colorHeld{};
  auto colorAction=[&](int index,const char* source) {
    app.action(colorActions[static_cast<std::size_t>(index)]);
#ifdef SKIFREE_WEBOS
    try {
      std::ofstream receipt(app.save.parent_path()/"runtime-color.json");
      receipt<<"{\"version\":\"1.0.0\",\"color\":"<<index<<",\"edition\":"<<static_cast<int>(app.edition)<<",\"source\":"<<jsonString(source)<<"}\n";
    }catch(const std::exception&){}
#else
    (void)source;
#endif
  };
  auto last=std::chrono::steady_clock::now();
  while(app.running&&(opt.frames==0||frame<opt.frames)) {
    const auto began=std::chrono::steady_clock::now();
    std::array<bool,4> colorEventSeen{};
    for(const auto& command:events)if(command.frame==frame) {
      SDL_Event e{};
      if(command.action.rfind("score:",0)==0) {
        // Desktop QA only: webOS rejects --script and --fixed-frame.
        std::string value=command.action.substr(6);std::replace(value.begin(),value.end(),':',' ');
        std::istringstream input(value);std::string course,extra;int score=0;
        if(!(input>>course>>score)||(input>>extra)||(course!="SS"&&course!="GS"&&course!="FS"))throw std::runtime_error("Invalid QA score");
        app.finish({course,score,course!="FS"});continue;
      }else if(command.action=="ok") {e.type=SDL_CONTROLLERBUTTONDOWN;e.cbutton.button=0;}
      else if(command.action.rfind("char:",0)==0&&command.action.size()==6) {
        char c=command.action[5];e.type=SDL_KEYDOWN;if(c>='A'&&c<='Z'){e.key.keysym.mod=KMOD_SHIFT;c=static_cast<char>(c-'A'+'a');}
        e.key.keysym.scancode=static_cast<SDL_Scancode>(SDL_SCANCODE_A+c-'a');e.key.keysym.sym=c;
      }else if(command.action=="blur"||command.action=="focus"||command.action=="hide"||command.action=="show") {e.type=SDL_WINDOWEVENT;e.window.event=command.action=="blur"?SDL_WINDOWEVENT_FOCUS_LOST:command.action=="focus"?SDL_WINDOWEVENT_FOCUS_GAINED:command.action=="hide"?SDL_WINDOWEVENT_HIDDEN:SDL_WINDOWEVENT_SHOWN;}
      else if(command.action.rfind("move:",0)==0||command.action.rfind("click:",0)==0) {
        const bool click=command.action.rfind("click:",0)==0;
        std::string point=command.action.substr(click?6:5);std::replace(point.begin(),point.end(),':',' ');
        std::istringstream coordinates(point);int x=0,y=0;std::string extra;
        if(!(coordinates>>x>>y)||(coordinates>>extra))throw std::runtime_error("Invalid scripted pointer coordinates");
        e.type=click?SDL_MOUSEBUTTONDOWN:SDL_MOUSEMOTION;
        if(click){e.button.x=x;e.button.y=y;e.button.button=SDL_BUTTON_LEFT;}else{e.motion.x=x;e.motion.y=y;}
      } else if(command.action.rfind("scan:",0)==0){e.type=SDL_KEYDOWN;e.key.keysym.scancode=static_cast<SDL_Scancode>(std::stoi(command.action.substr(5)));}
      else if(command.action=="red-keycode"){e.type=SDL_KEYDOWN;e.key.keysym.sym=2097169;}
      else {e.type=SDL_KEYDOWN;const bool repeated=command.action.rfind("repeat-",0)==0;const auto action=repeated?command.action.substr(7):command.action;auto found=std::find_if(keys.begin(),keys.end(),[&](const auto& pair){return pair.second==action;});if(found==keys.end())throw std::runtime_error("Unknown scripted action");e.key.keysym.scancode=static_cast<SDL_Scancode>(found->first);e.key.repeat=repeated?1:0;}
      if(sdl.PushEvent(&e)<0)throw std::runtime_error("QA event injection failed");
    }
    if(app.retryAssets)loadAssets();
    SDL_Event event{};
    while(sdl.PollEvent(&event)) {
      if(event.type==SDL_QUIT)app.running=false;
      else if(event.type==SDL_WINDOWEVENT) {
        const auto e=event.window.event;
        if(e==SDL_WINDOWEVENT_CLOSE)app.running=false;
        if(e==SDL_WINDOWEVENT_FOCUS_LOST)focused=false;
        if(e==SDL_WINDOWEVENT_FOCUS_GAINED)focused=true;
        if(e==SDL_WINDOWEVENT_HIDDEN)shown=false;
        if(e==SDL_WINDOWEVENT_SHOWN)shown=true;
        if(e==SDL_WINDOWEVENT_MINIMIZED)minimized=true;
        if(e==SDL_WINDOWEVENT_RESTORED)minimized=false;
        if(!focused||!shown||minimized){app.pause();axisDirection=0;}
        if(e==SDL_WINDOWEVENT_RESIZED||e==SDL_WINDOWEVENT_SIZE_CHANGED)view=viewport();
        last=began;
      } else if(event.type==SDL_KEYDOWN&&focused&&shown&&!minimized) {
        const int scan=static_cast<int>(event.key.keysym.scancode);
        if(opt.logInput)std::cout<<"SDL scancode "<<scan<<" "<<sdl.GetScancodeName(event.key.keysym.scancode)<<" keycode "<<event.key.keysym.sym<<'\n';
        if(scan>=486&&scan<=489) {
          const int color=scan-486;colorEventSeen[static_cast<std::size_t>(color)]=true;
          if(!event.key.repeat&&!colorHeld[static_cast<std::size_t>(color)])colorAction(color,"scancode");
        }else if(scan==0&&event.key.keysym.sym==2097169) {
          // Some webOS builds expose Red only as this native keycode.
          colorEventSeen[0]=true;if(!event.key.repeat)colorAction(0,"native-red-keycode");
        }else if(app.screen==Screen::Initials&&scan>=SDL_SCANCODE_A&&scan<=SDL_SCANCODE_Z) {
          if(!event.key.repeat)app.character(static_cast<char>('A'+scan-SDL_SCANCODE_A));
        }else if(app.screen==Screen::Initials&&scan==SDL_SCANCODE_BACKSPACE){app.action("erase",event.key.repeat!=0);
        }else if(app.edition==Edition::Original&&(app.screen==Screen::Run||app.screen==Screen::Pause)&&(scan==9||scan==21||scan==23||scan==27||scan==28)) {
          char c=static_cast<char>('a'+scan-SDL_SCANCODE_A);
          const bool upper=((event.key.keysym.mod&KMOD_SHIFT)!=0)!=((event.key.keysym.mod&KMOD_CAPS)!=0);
          if(upper)c=static_cast<char>(c-'a'+'A');
          app.character(c);
#ifndef SKIFREE_WEBOS
        }else if(scan==40){app.action("enter",event.key.repeat!=0);
#endif
        }else {auto it=keys.find(scan);if(it!=keys.end())app.action(it->second,event.key.repeat!=0);}
      } else if((event.type==SDL_MOUSEMOTION||event.type==SDL_MOUSEBUTTONDOWN)&&focused&&shown&&!minimized) {
        const bool click=event.type==SDL_MOUSEBUTTONDOWN;
        auto p=view.pointer(static_cast<float>(click?event.button.x:event.motion.x),static_cast<float>(click?event.button.y:event.motion.y));
        if(p&&(!click||event.button.button==SDL_BUTTON_LEFT))app.pointer(*p,click);
      } else if(event.type==SDL_CONTROLLERDEVICEADDED)openController(event.cdevice.which);
      else if(event.type==SDL_CONTROLLERDEVICEREMOVED) {
        controllers.erase(std::remove_if(controllers.begin(),controllers.end(),[&](auto* c){if(sdl.JoystickInstanceID(sdl.GameControllerGetJoystick(c))==event.cdevice.which){sdl.GameControllerClose(c);return true;}return false;}),controllers.end());app.pause();axisDirection=0;
      } else if(event.type==SDL_CONTROLLERBUTTONDOWN&&focused&&shown&&!minimized) {
        const std::map<int,std::string> buttons{{0,"ok"},{1,"back"},{6,"pause"},{11,"up"},{12,"down"},{13,"left"},{14,"right"}};
        const auto it=buttons.find(event.cbutton.button);if(it!=buttons.end())app.action(it->second);
      } else if(event.type==SDL_CONTROLLERAXISMOTION&&event.caxis.axis==0&&focused&&shown&&!minimized) {
        const int direction=event.caxis.value>16000?1:event.caxis.value<-16000?-1:0;
        if(direction!=axisDirection){if(direction)app.action(direction<0?"left":"right");axisDirection=direction;}
      }
    }
    // Some compositor paths update the extended SDL keyboard state without a
    // normal KeyDown event. Edge detection avoids repeat/duplicate switching.
    int keyCount=0;const Uint8* held=sdl.GetKeyboardState(&keyCount);
    for(int i=0;i<4;++i) {
      const bool pressed=held&&keyCount>486+i&&held[486+i]!=0;
      if(focused&&shown&&!minimized&&pressed&&!colorHeld[static_cast<std::size_t>(i)]&&!colorEventSeen[static_cast<std::size_t>(i)])colorAction(i,"keyboard-state");
      colorHeld[static_cast<std::size_t>(i)]=pressed;
    }
    if(focused&&shown&&!minimized) {
      const double dt=opt.fixed?1.0/60:std::chrono::duration<double>(began-last).count();app.update(dt);draw(renderer,app,view);
#ifdef SKIFREE_WEBOS
      if(frame==60) {
        // App-local startup evidence only; never intercepts other apps or input.
        try {
          const auto folder=app.save.parent_path();
          renderer.screenshot(folder/"runtime-startup.png");
          std::ofstream receipt(folder/"runtime-status.json");
          receipt<<"{\"version\":\"1.0.0\",\"renderer\":"<<jsonString(renderer.renderer)
                 <<",\"gl_version\":"<<jsonString(renderer.version)<<",\"assets_ready\":"<<(app.assetsReady?"true":"false")
                 <<",\"assets_path\":"<<jsonString(opt.assets.string())<<",\"save_path\":"<<jsonString(app.save.string())
                 <<",\"record_writable\":"<<(app.recordWritable?"true":"false")
                 <<",\"score_counts\":["<<app.scores[0].size()<<','<<app.scores[1].size()<<','<<app.scores[2].size()<<']'
                 <<",\"width\":"<<view.drawable_w<<",\"height\":"<<view.drawable_h<<"}\n";
        } catch(const std::exception& error) {std::cerr<<"Startup receipt: "<<error.what()<<'\n';}
      }
#endif
      if(opt.frames&&frame==opt.frames-1&&!opt.capture.empty())renderer.screenshot(opt.capture);
      sdl.GL_SwapWindow(window);
    }
    last=began;++frame;
    if(!opt.fixed) {const auto budget=focused&&shown&&!minimized?std::chrono::milliseconds(16):std::chrono::milliseconds(50);if(vsync<=0||!focused||!shown||minimized)std::this_thread::sleep_until(began+budget);}
  }
  app.persist();
  if(!opt.report.empty()) {
    std::ofstream out(opt.report);if(!out)throw std::runtime_error("Report path unavailable");out<<std::setprecision(17);
    out<<"{\n  \"edition\": "<<static_cast<int>(app.edition)<<",\n  \"edition_name\": "<<jsonString(app.editionName())<<",\n  \"recreation_distance\": "<<app.recreation[0].distance()<<",\n  \"chase_distance\": "<<app.recreation[1].distance()<<",\n";
    out<<"  \"original_intro\": "<<(app.originalIntro?"true":"false")<<",\n  \"lift_offset\": "<<ski::presentation::liftOffset(app.originalIntro)<<",\n  \"score_rank\": "<<app.scorePlacement.rank<<",\n  \"score_value\": "<<app.scorePlacement.value<<",\n  \"player_x\": "<<ski::classic::state().x<<",\n";
    out<<"  \"race_elapsed_ms\": "<<ski::classic::state().elapsedMs<<",\n  \"course\": "<<static_cast<int>(ski::classic::state().course)<<",\n";
    out<<"  \"browsing_scores\": "<<(app.browsingScores?"true":"false")<<",\n  \"score_course\": "<<app.scoreCourse<<",\n  \"score_counts\": ["<<app.scores[0].size()<<','<<app.scores[1].size()<<','<<app.scores[2].size()<<"],\n";
    out<<"  \"original_hud_visible\": "<<(app.originalHudStarted?"true":"false")<<",\n  \"original_hud_best\": "<<jsonString(bestScoreLabel(app,ski::classic::state()))<<",\n";
    out<<"  \"screen\": "<<jsonString(screenName(app.screen))<<",\n  \"distance\": "<<(ski::classic::state().y/16.0)<<",\n  \"elapsed\": "<<(ski::classic::state().clockMs/1000.0)<<",\n  \"fast\": "<<(ski::classic::state().fast?"true":"false")<<",\n  \"entities\": "<<ski::classic::state().actors<<",\n  \"style\": "<<ski::classic::state().style<<",\n  \"in_air\": "<<ski::classic::state().inAir<<",\n  \"gles\": "<<(renderer.is_es?"true":"false")<<",\n  \"renderer\": "<<jsonString(renderer.renderer)<<",\n  \"gl_version\": "<<jsonString(renderer.version)<<",\n  \"drawable_width\": "<<view.drawable_w<<",\n  \"drawable_height\": "<<view.drawable_h<<",\n  \"swap_interval\": "<<vsync<<",\n  \"record_writable\": "<<(app.recordWritable?"true":"false")<<",\n  \"assets_ready\": "<<(app.assetsReady?"true":"false")<<",\n  \"assets_path\": "<<jsonString(opt.assets.string())<<",\n  \"asset_error\": "<<jsonString(app.assetError)<<",\n  \"heading\": "<<ski::classic::state().frame<<",\n  \"pointer_active\": "<<(app.pointerActive?"true":"false")<<"\n}\n";
  }
  std::cout<<"SkiFree native: "<<screenName(app.screen)<<", GLES "<<renderer.version<<", "<<view.drawable_w<<'x'<<view.drawable_h<<", distance "<<(ski::classic::state().y/16.0)<<" m\n";
  return 0;
}
}
int main(int argc,char** argv) {
  try{return run(argc,argv);}catch(const std::exception& e){std::cerr<<"SkiFree: "<<e.what()<<'\n';return 1;}
}
