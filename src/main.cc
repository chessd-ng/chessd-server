#include <iostream>
#include <string>

#include "Core.hh"

using namespace std;

const char* secret = "secret";

void log_traffic(const std::string& data, bool incoming) {
	cout << "-----------------------------" << endl;
	cout << (incoming ? "Incoming!" : "Outcoming!") << endl;
	cout << "-----------" << endl;
	cout << data << endl;
	cout << "-----------------------------" << endl;

}

int main(void) {
	try {
		XML::Tag* config = XML::loadXmlFile("config.xml");
		Core::init(*config);
		Core& core = Core::singleton();
		delete config;
		try {
			core.connect();
		} catch(const char* error) {
			cout << error << endl;
			return 1;
		}
		string cmd;
		cin >> cmd;
		Core::destroy();
	} catch (const char* msg) {
		cout << "Error: " << msg << endl;
	}
	return 0;
}
