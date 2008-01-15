#include "GlickoSystem.hh"
#include <cmath>
#include <algorithm>

double GlickoSystem::InitRD(const Rating &r) {
	return std::min(sqrt(r.volatility()*r.volatility()),350.0);
}
double GlickoSystem::gRD(const Rating &r) {
	return 1.0/(sqrt(1.0 + 3*q()*q()*r.volatility()*r.volatility()/M_PI/M_PI));
}
double GlickoSystem::E(const Rating &r1, const Rating &r2) {
	return 1.0/(1.0+pow(10.0,gRD(r2)*double(r2.rating()-r1.rating())/400.0 ));
}
double GlickoSystem::q() {
	return 0.00575646273248511;
}
double GlickoSystem::dsquare(const Rating &r1, const Rating &r2) {
	return 0.0;
}
