#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace ski::classic {
// One original game instance; platform input/rendering remain outside the core.
enum class Key { Left=37,Right=39,Up=38,Down=40,Jump=45,Enter=13,Restart=113,Fast=1000,NorthWest=103,NorthEast=105,SouthWest=97,SouthEast=99 };
enum class Course { Free, Slalom, TreeSlalom, Freestyle };
struct Draw {int sprite,x,y,width,height;};
struct Result {std::string course;int value=0;bool timed=false;std::uint64_t serial=0;};
struct State {
    int x=0,y=0,frame=0,style=0,elapsedMs=0,clockMs=0,distance=0,speed=0,inAir=0,actors=0;
    bool alive=false,fast=false,paused=false;
    Course course=Course::Free;Result result;
};
void reset(std::uint32_t seed);
// Active elapsed time. Due 40 ms timer events coalesce into one callback per
// call, carrying actual elapsed milliseconds into scoring and animation.
void advance(double seconds);
void pause(bool value);
void key(Key key);
void click();
// Original character commands, including t single-step and x/X/y/Y nudges.
void character(char value);
void pointer(int x,int y);
const std::vector<Draw>& draws();
State state();
}
