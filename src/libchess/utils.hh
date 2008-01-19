#ifndef UTILS_HH
#define UTILS_HH
#include <string>

std::string toString(int n) {
	std::string ans;
	while(n>0) {
		ans+=(char)(n%10 + '0');
		n/=10;
	}
	return ans;
}

#endif
