/*
 *   Copyright (c) 2007-2008 Raphel H. Ribas,  Eduardo A. Ribas.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

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
    return;
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
