#ifndef GLICKOSYSTEM_HH
#define GLICKOSYSTEM_HH
#include "Rating.hh"
/*
 * Glicko System number 1
*/
class GlickoSystem {
	public:
		//Step 1
		/*! \brief Calculate the new Rating Deviation*/
		static double InitRD(const Rating& r);

		//Step 2
		/*! \brief function g(RD)
		 * 1/sqrt(1+3q^2*RD^2/PI^2)
		 */
		static double gRD(const Rating &r);

		/*! \brief Function E*/
		static double E(const Rating &r1, const Rating &r2);

		/*! \brief Function E for BugHouse*/
//		double glickoBE(double RD) const;

		/*! \brief ln(10)/400 */
		static double q();

		/*! \brief function d^2*/
		static double dsquare(const Rating &r1, const Rating &r2);

		template<class T>
		static T square(T x) {
			return x*x;
		}
	private:
};
#endif
