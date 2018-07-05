#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <vector>
#include <algorithm>

unsigned int Factorial(unsigned int number) {
    return number > 1 ? Factorial(number - 1)*number : 1;
}

void can_use_cxx14 (std::vector<int>& v) {
    
    std::transform(v.begin(), v.end(), v.begin(), [](auto x) { return x * x; } );
	
}

TEST_CASE("Factorials are computed", "[factorial]") {
    REQUIRE(Factorial(0) == 1);
    REQUIRE(Factorial(1) == 1);
    REQUIRE(Factorial(2) == 2);
    REQUIRE(Factorial(3) == 6);
    REQUIRE(Factorial(10) == 3628800);
}

TEST_CASE("Can we use cxx14", "[can_use_cxx14]") {
	
	std::vector <int> vect  = {0, 1, 2, 3, 4, 5};
	can_use_cxx14(vect);
    
	REQUIRE(vect[0] == 0);
	REQUIRE(vect[1] == 1);
	REQUIRE(vect[2] == 4);
	REQUIRE(vect[3] == 9);
	REQUIRE(vect[4] == 16);
	REQUIRE(vect[5] == 25);
}