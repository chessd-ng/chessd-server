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
	Tag* xml = iksReadXMLString("<iq from=\"eu\" to=\"voce\" id=\"asdasd\"> <query code=\"123\"/> <query code=\"456\"/> </iq>");
	if(xml==0) {
		cout << "error 1" << endl;
		return 1;
	}
	if(not iq_desc.validateXML(xml)) {
		cout << "error 2" << endl;
		return 1;
	}
	cout << xml->xml() << endl;
	delete xml;
	return 0;
}
