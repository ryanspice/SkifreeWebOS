// Included only in the test build. Exercises the reconstructed state directly
// so long-course/yeti boundary fixtures need no shipping teleport/debug API.
#include <iostream>
#include <array>
#include "skifree/presentation.hpp"
#include <limits>
int classicFixtureSuite() {
  using namespace ski::classic;
  int checks=0;
  auto check=[&](bool value,const char* name){if(!value)throw std::runtime_error(name);++checks;std::cout<<"PASS "<<name<<'\n';};
  reset(1);
  check(state().frame==3&&state().y==0&&!state().fast,"original stationary start, no chase selection");
  advance(0.04);const auto& initial=draws();
  for(int id:{53,54,55,56,61,62,63})check(std::any_of(initial.begin(),initial.end(),[&](auto d){return d.sprite==id;}),"original title / instruction / course sign present");
  check(permObjectCount<NUM_PERM_OBJECTS&&PermObjectList_0040c720.nextObject-PermObjectList_0040c720.startingObject>=4,"original permanent objects fit pool");
  reset(1);key(Key::Down);for(int i=0;i<10;++i)advance(0.04);
  check(state().y==45&&playerActor->verticalVelocityMaybe==10,"original acceleration: ten 40ms ticks move 45 pixels");
  const int before=state().y;key(Key::Fast);advance(0.04);
  check(state().y-before==20&&state().fast,"F doubles movement without boost timer");
  for(int i=0;i<75;++i){advance(0.04);draws();}
  check(state().fast,"fast mode persists beyond two seconds");key(Key::Fast);check(!state().fast,"F toggles off");
  pause(true);const auto frozen=state();advance(0.25);check(state().y==frozen.y&&state().clockMs==frozen.clockMs,"pause freezes simulation");pause(false);
  reset(1);key(Key::Down);for(int i=0;i<10;++i)advance(0.04);
  const auto beforeStall=state();const auto velocityBeforeStall=playerActor->verticalVelocityMaybe;
  advance(0.25);
  check(state().y==beforeStall.y+velocityBeforeStall,"overdue timer coalesces to one movement callback, not six catch-up moves");
  check(state().clockMs==650&&timerFrameDurationInMillis==250,"timer callback keeps real active milliseconds after a stall");
  check(state().speed==playerActor->verticalVelocityMaybe*1000/(250*16),"speed display uses the measured callback interval");
  const auto stalledY=state().y;advance(0.75);
  check(state().clockMs==1400&&state().y-stalledY==11,"long callback delay is neither clamped nor converted to extra movement");
  reset(1);key(Key::Down);advance(0.02);pause(true);advance(5);pause(false);advance(0.02);
  check(state().y==0&&state().clockMs==20,"resume starts a fresh timer interval and excludes paused time");
  advance(0.02);check(state().clockMs==60&&timerFrameDurationInMillis==40,"first resumed callback has its own 40 ms interval");
  reset(1);advance(std::numeric_limits<double>::quiet_NaN());advance(std::numeric_limits<double>::infinity());advance(-1);advance(0.04);
  check(state().clockMs==40,"invalid elapsed input cannot poison the active clock");
  reset(1);character('x');check(state().x==2,"original lowercase x nudges right by two pixels");
  character('X');check(state().x==0,"original uppercase X nudges left by two pixels");
  character('y');check(state().y==2,"original lowercase y nudges downhill by two pixels");
  character('Y');check(state().y==0,"original uppercase Y nudges uphill by two pixels");
  character('f');check(state().fast,"original character f toggles fast mode");character('f');
  const auto beforeRedraw=state();character('r');check(state().x==beforeRedraw.x&&state().y==beforeRedraw.y&&state().clockMs==beforeRedraw.clockMs,"original r redraw does not advance the game");
  key(Key::Enter);check(playerActor->inAirCounter==0&&state().frame==3,"original Enter leaves a living skier unchanged");
  key(Key::Down);playerActor->verticalVelocityMaybe=10;pause(true);const auto beforeStep=state();character('t');
  check(state().y==beforeStep.y+10&&state().paused,"original t advances exactly one game update while paused");
  check(state().clockMs==beforeStep.clockMs&&timerFrameDurationInMillis==0,"manual step does not invent elapsed milliseconds");pause(false);
  reset(1);key(Key::Jump);check(playerActor->inAirCounter==2&&state().frame==13,"Insert uses original small jump");
  reset(1);click();check(playerActor->inAirCounter==4&&state().frame==13,"mouse click / remote OK use original larger jump");
  reset(1);pointer(320,120);pointer(320,240);check(state().frame==0,"mouse below skier points downhill");key(Key::Left);check(state().frame==1,"left key turns from downhill");
  for(auto [x,course]:std::array<std::pair<short,Course>,3>{{{-448,Course::Slalom},{0,Course::Freestyle},{416,Course::TreeSlalom}}}) {
    reset(1);updateActorPositionMaybe(playerActor,x,639,0);key(Key::Down);playerActor->verticalVelocityMaybe=4;advance(0.04);
    check(state().course==course,"crossing original course start selects course");
  }
  // Cross real course boundaries through the ordinary timer/player update. The
  // test-only positioning avoids simulating a full mountain or adding a debug
  // API. Expected milliseconds come from known 4-pixel crossings: the boundary
  // is one pixel into a 40 ms callback, so its interpolated time is 10 ms.
  for(auto [x,course]:std::array<std::pair<short,Course>,2>{{{-448,Course::Slalom},{416,Course::TreeSlalom}}}) {
    reset(1);updateActorPositionMaybe(playerActor,x,639,0);key(Key::Down);playerActor->verticalVelocityMaybe=4;advance(0.04);
    check(state().course==course&&state().y==643&&state().clockMs==40,"timed course starts on an actual downhill start-line crossing");
    // The original EXE initializes start-current, briefly negative, then uses
    // current-start on subsequent callbacks. Preserve that original quirk.
    check(state().elapsedMs==-30,"original first crossing retains its signed sub-tick time");
    advance(0.04);
    check(state().elapsedMs==70,"race clock advances from the interpolated start time");
    for(int i=0;i<4;++i)advance(0.04);
    const auto racing=state();pause(true);advance(5);
    check(state().elapsedMs==racing.elapsedMs&&state().clockMs==racing.clockMs&&state().y==racing.y,"pausing an active timed course freezes race time and position");
    pause(false);advance(0.04);
    check(state().course==course&&state().elapsedMs==racing.elapsedMs+40,"resuming an active timed course excludes the paused interval");
    const auto beforeRaceStall=state();advance(0.25);
    check(state().elapsedMs==beforeRaceStall.elapsedMs+250,"race clock includes actual callback delay after a stall");
    const short finish=course==Course::Slalom?8640:16640;
    updateActorPositionMaybe(playerActor,x,finish-1,0);key(Key::Down);playerActor->verticalVelocityMaybe=4;
    const auto beforeFinish=state();advance(0.04);const auto finished=state();
    check(finished.course==Course::Free&&finished.result.course==(course==Course::Slalom?"SS":"GS")&&finished.result.timed&&finished.result.serial>0,"actual timed finish emits the matching course result");
    check(finished.elapsedMs==beforeFinish.elapsedMs+10&&finished.result.value==finished.elapsedMs,"timed finish interpolates the line crossing and saves the same elapsed time");
    advance(0.04);
    check(state().elapsedMs==finished.elapsedMs&&state().result.serial==finished.result.serial,"completed course time remains fixed after returning to free skiing");

    reset(1);updateActorPositionMaybe(playerActor,x,639,0);key(Key::Down);playerActor->verticalVelocityMaybe=4;advance(0.04);advance(0.04);
    const auto beforePenalty=state();const auto* gate=currentSlalomFlag;
    const short wrongSide=gate->maybeX+(gate->spriteIdx==23?32:-32);
    updateActorPositionMaybe(playerActor,wrongSide,gate->maybeY-1,0);key(Key::Down);playerActor->verticalVelocityMaybe=4;advance(0.04);advance(0.04);
    check(state().course==course&&state().elapsedMs==beforePenalty.elapsedMs+5080,"wrong-side gate adds the original five-second race penalty");
  }
  reset(1);updateActorPositionMaybe(playerActor,2000,700,0);key(Key::Down);
  for(int i=0;i<20;++i)advance(0.04);
  check(state().course==Course::Free&&state().clockMs==800&&state().elapsedMs==0,"free skiing advances simulation without inventing a race stopwatch");
  reset(1);updateActorPositionMaybe(playerActor,0,639,0);key(Key::Down);playerActor->verticalVelocityMaybe=4;advance(0.04);
  for(int i=0;i<10;++i)advance(0.04);
  check(state().course==Course::Freestyle&&state().clockMs==440&&state().elapsedMs==0,"Freestyle is scored on style and does not start the race clock");
  updateActorPositionMaybe(playerActor,0,16639,0);key(Key::Down);playerActor->verticalVelocityMaybe=4;advance(0.04);
  check(state().course==Course::Free&&state().result.course=="FS"&&!state().result.timed&&state().elapsedMs==0,"Freestyle finish emits a style result while leaving race time unchanged");
  reset(1);auto* bottom=PermObjectList_0040c720.nextObject-1;
  check(bottom->actorTypeMaybe==ACTOR_TYPE_6_YETI_BOTTOM&&bottom->maybeY==32060,"bottom yeti is part of every normal game");
  updateActorPositionMaybe(playerActor,500,31999,0);updateYeti(bottom);
  check(bottom->xVelocity==0&&bottom->yVelocity==0,"yeti waits before 2000m boundary");
  updateActorPositionMaybe(playerActor,500,32001,0);updateYeti(bottom);
  check(bottom->xVelocity!=0||bottom->yVelocity!=0,"normal downhill yeti pursues beyond 2000m");
  reset(1);auto* boarder=addActorOfType(ACTOR_TYPE_3_SNOWBOARDER,31);updateActorPositionMaybe(boarder,100,100,0);updateActorType3_snowboarder(boarder);
  check(boarder->typeMaybe==3&&boarder->frameNo>=31&&boarder->frameNo<=38,"snowboarder uses original animation and movement table");
  reset(1);updateEntPackIniKeyValue("SS",1234,1);const auto firstResult=state().result;
  updateEntPackIniKeyValue("SS",1234,1);
  check(firstResult.serial>0&&state().result.serial>firstResult.serial,"repeated identical course finishes emit distinct results");
  reset(1);check(state().result.serial==0&&state().result.course.empty(),"new run clears result identity");
  advance(0.04);updateEntPackIniKeyValue("SS",1234,1);key(Key::Restart);
  check(state().clockMs==40&&state().result.course.empty(),"core restart samples the active clock and clears the prior result");
  advance(0.04);check(timerFrameDurationInMillis==40,"first callback after core restart uses only its new elapsed interval");
  reset(1);
  for(int tick=0;tick<500;++tick){advance(0.04);draws();}
  const auto scene=draws();
  const auto title=std::find_if(scene.begin(),scene.end(),[](const auto& d){return d.sprite==53;});
  bool reproduced=false,clear=true;
  auto overlaps=[](const Draw& a,const Draw& b){return a.x<b.x+b.width&&b.x<a.x+a.width&&a.y<b.y+b.height&&b.y<a.y+a.height;};
  if(title!=scene.end())for(const auto& raw:scene)if(ski::presentation::liftSprite(raw.sprite)){
    reproduced|=overlaps(raw,*title);clear&=!overlaps(ski::presentation::originalSprite(raw,true),*title);
    const auto gameplay=ski::presentation::originalSprite(raw,false);
    clear&=gameplay.x==raw.x&&gameplay.y==raw.y;
  }
  check(title!=scene.end()&&reproduced&&clear,"moving opening lift clears Chris Pirih credit without changing gameplay coordinates");
  // Long mixed-input runs exercise collisions, pool reuse, restarts and 16-bit
  // coordinate wrapping under ASan/UBSan. This is not a human gameplay verdict.
  for(unsigned seed=1;seed<=12;++seed) {
    reset(seed);key(Key::Down);
    for(int frame=0;frame<6000;++frame) {
      if(frame%233==0)key(Key::Fast);
      if(frame%91==0)click();
      if(frame%71==0)key(Key::Left);
      if(frame%77==0)key(Key::Right);
      if(frame%149==0)key(Key::Down);
      if(!state().alive)reset(seed+static_cast<unsigned>(frame));
      advance(0.04);draws();
      if(state().actors>NUM_ACTORS)throw std::runtime_error("actor pool overflow");
    }
  }
  check(true,"72000 ticks of mixed input and rendering lifecycle");
  std::cout<<checks<<" classic checks passed\n";return 0;
}
