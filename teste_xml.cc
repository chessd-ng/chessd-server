#include <iostream>
#include "xml.hh"
#include "iksutil.hh"

using namespace XML;
using namespace std;

int main(void) {
	Description iq_desc;
	if(not iq_desc.loadFromFile("protocol/iq.desc")) {
		cout << "error" << endl;
		return 1;
	}
	Tag* xml = iksReadXMLString("<iq from=\"eu\" to=\"voce\" id=\"asdasd\"> <query code=\"123\"/> </iq>");
	if(xml==0) {
		cout << "error 1" << endl;
		return 1;
	}
	if(not iq_desc.validateXML(xml)) {
		cout << "error 2" << endl;
		return 1;
	}
	return 0;
}
