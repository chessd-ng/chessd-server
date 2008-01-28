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
#include <exception>

#include "Core.hh"

using namespace std;

int main(void) {
	try {
		std::auto_ptr<XML::Tag> config(XML::loadXmlFile("config.xml"));
		Core::init(*config);
		Core& core = Core::singleton();
		try {
			core.connect();
		} catch(const char* error) {
			cout << error << endl;
			return 1;
		}
		string cmd;
		cin >> cmd;
		Core::close();
	} catch (const char* msg) {
		cout << "Error: " << msg << endl;
	} catch (const exception& error) {
		cout << "Error: " << error.what() << endl;
    }
	return 0;
}
