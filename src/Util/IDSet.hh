#ifndef UTILIDSET_HH
#define UTILIDSET_HH

#include <stack>

namespace Util {
	class IDSet {
		public:
			/*! \brief Constructor */
			IDSet();

			/*! \brief Destructor */
			~IDSet();

			/*! \brief Acquire an unused ID.
			 *
			 * Get an unused ID and set it as in use.
			 * \return An unused ID.
			 */
			int acquireID();

			/*! \brief Release an in usedID.
			 *
			 * Release an in use ID so it can be
			 * acquired latter.
			 */
			void releaseID(int ID);
		private:
			std::stack<int> unused_ids;
			int used_id;
	};
}

#endif
