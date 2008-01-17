#ifndef GLICKOSYSTEM_HH
#define GLICKOSYSTEM_HH
#include "Rating.hh"
/*
 * Glicko System number 1
*/
class GlickoSystem {
	public:
		//Step 1
		/*! \brief returns the c constant for calcuting the new RD based on time*/
		static double c();
		/*! \brief Calculate the new Rating Deviation*/
		static double InitRD(const Rating& r, long long int t);

		//Step 2
		/*! \brief function g(RD)
		 * 1/sqrt(1+3q^2*RD^2/PI^2)
		 */
		static double gRD(const Rating &r);

		static double gBRD(const Rating &r1, const Rating &r2, const Rating &r3);

		/*! \brief Function E*/
		static double E(const Rating &r1, const Rating &r2);

		/*! \brief Function E for Bughouse*/
		static double BE(const Rating &r, const Rating &r1, const Rating &r2, const Rating &r3);

		/*! \brief ln(10)/400 */
		static double q();

		/*! \brief ln(10)/800 */
		static double Bq();

		/*! \brief function d^2*/
		static double dsquare(const Rating &r1, const Rating &r2);

		static int round(double a);

		template<class T>
		static T square(T x) {
			return x*x;
		}
	private:
};
#endif
