#include "skifree/recreation.hpp"
#include "spice/render.hpp"
#include <algorithm>
#include <cmath>
namespace ski::recreation {
void sprite(spice::Renderer& r,const std::string& name,float x,float footY,float /*width*/,float /*height*/,float scale=2) {
  const auto size=r.imageSize(name);
  r.image(name,{x-size.x*scale/2,footY-size.y*scale,size.x*scale,size.y*scale});
}
void landscape(spice::Renderer& r,const Game& game) {
  const auto& p=game.player();
  struct Draw {double y;int type;std::size_t index;};std::vector<Draw> order;
  const auto& objects=game.terrain();
  for(std::size_t i=0;i<objects.size();++i)order.push_back({objects[i].y,0,i});
  order.push_back({p.y+(p.pose==ski::recreation::Pose::Jumping?90:0),1,0});
  if(game.yeti().active)order.push_back({game.yeti().y,2,0});
  std::sort(order.begin(),order.end(),[](const auto& a,const auto& b){return a.y<b.y;});
  for(const auto& draw:order) {
    if(draw.type==0) {
      const auto& o=objects[draw.index];const auto x=static_cast<float>(480+o.x-p.x),y=static_cast<float>(190+o.y-p.y);
      if(x<-100||x>1060||y<-100||y>700)continue;
      switch(o.kind){
        case ski::recreation::Kind::SmallTree:sprite(r,"ski32_049",x,y,28,32);break;
        case ski::recreation::Kind::LargeTree:sprite(r,"ski32_051",x,y,32,64);break;
        case ski::recreation::Kind::Rock:sprite(r,"ski32_045",x,y,23,11);break;
        case ski::recreation::Kind::Ramp:sprite(r,"ski32_052",x,y,32,8);break;
        case ski::recreation::Kind::Snow:sprite(r,"ski32_027",x,y,43,10);break;
        case ski::recreation::Kind::Boarder:sprite(r,"ski32_038",x,y,23,31);break;
      }
    } else if(draw.type==1) {
      if(p.pose==ski::recreation::Pose::Eaten)continue;
      std::string name="ski32_001";float w=16,h=32,up=0;
      if(p.pose==ski::recreation::Pose::Crashed){name="ski32_013";w=32;h=24;}
      else if(p.pose==ski::recreation::Pose::Jumping){name=p.trick?(p.poseTime>0.5?"ski32_018":"ski32_019"):"ski32_014";w=32;h=32;up=static_cast<float>(std::sin(p.poseTime*3.14159265)*35);r.rect({470,188,22,3},{0.75f,0.8f,0.84f,1});}
      else if(p.heading<0){name=p.heading==-3?"ski32_004":p.heading==-1?"ski32_002":"ski32_003";w=24;h=28;}
      else if(p.heading>0){name=p.heading==3?"ski32_007":p.heading==1?"ski32_005":"ski32_006";w=24;h=28;}
      sprite(r,name,480,190-up,w,h);
    } else {
      const auto& y=game.yeti();std::string name=y.x<p.x?"ski32_072":"ski32_070";
      if(p.pose==ski::recreation::Pose::Eaten)name=p.poseTime>0.75?"ski32_076":"ski32_078";
      else if(static_cast<int>(y.time*7)%2)name=y.x<p.x?"ski32_073":"ski32_071";
      sprite(r,name,static_cast<float>(480+y.x-p.x),static_cast<float>(190+y.y-p.y),32,48);
    }
  }
}

}
