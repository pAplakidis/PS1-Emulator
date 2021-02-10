#include "helpers.h"

int min() throw(){
  return std::numeric_limits<int>::min();
}

int max() throw(){
  return std::numeric_limits<int>::max();
}

void assert_check(const char *msg, bool check){
  if(!check){
    std::cerr << msg << "\n";
    abort();
  }
}

int checked_add(int const a, int const b){
  if(a >= 0){
    assert_check("addition possitive overflow", b <= max() - a);
  }else{
    assert_check("addition negative overflow", b >= min() - a);
  }

  return a+b;
}

