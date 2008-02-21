/*
 *   Copyright (c) 2007-2008 C3SL.
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

int main(int argc, char** argv) {
	try {
        std::string file_name = (argc>=2) ? argv[1] : "config.xml";
		std::auto_ptr<XML::Tag> config(XML::loadXmlFile(file_name));
		Core::init(*config);
		Core& core = Core::singleton();
        core.start();

        /* Run until any input is given */
		string cmd;
		cin >> cmd;
		Core::close();
	} catch (const char* msg) {
		cout << "Error: " << msg << endl;
        return 1;
	} catch (const exception& error) {
		cout << "Error: " << error.what() << endl;
        return 1;
    }
	return 0;
}
