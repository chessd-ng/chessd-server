#include "IDSet.hh"

namespace Util {

	IDSet::IDSet() : used_id(0) {
	}

	IDSet::~IDSet() {
	}

	int IDSet::acquireID() {
		if(this->unused_ids.empty()) {
			return this->used_id++;
		} else {
			int id = this->unused_ids.top();
			this->unused_ids.pop();
			return id;
		}
	}

	void IDSet::releaseID(int id) {
		this->unused_ids.push(id);
	}

}
