#include <exception>
#include <iostream>
int classicFixtureSuite();
int main() {
  try{return classicFixtureSuite();}
  catch(const std::exception& e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}
}
