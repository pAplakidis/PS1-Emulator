#include "helpers.h"

int min() throw(){
  return std::numeric_limits<int>::min();
}

int max() throw(){
  return std::numeric_limits<int>::max();
}

bool assert_check(const char *msg, bool check){
  if(!check){
    std::cerr << msg << "\n";
    //abort();
    return false;
  }
  return true;
}

int *checked_add(int const a, int const b){
  if(a >= 0){
    if(!assert_check("addition possitive overflow", b <= max() - a)){
      return NULL;
    }
  }else{
    if(!assert_check("addition negative overflow", b >= min() - a)){
      return NULL;
    }
  }

  int *sum = (int *)malloc(sizeof(int));
  *sum = a + b;
  return sum;
}

