#pragma once

#include <limits>
#include <iostream>
#include <cstdlib>

int min() throw();
int max() throw();
bool assert_check(const char *msg, bool check);
int *checked_add(int const a, int const b);

