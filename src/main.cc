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
#include <fstream>
#include <string>
#include <exception>

#include "Core.hh"
#include "I18n.hh"
#include "Util/Log.hh"

using namespace std;

int main(int argc, char** argv) {
    fstream log_file;
    
	try {
        /* Load config file */
        std::string file_name = (argc>=2) ? argv[1] : "config.xml";
		std::auto_ptr<XML::Tag> config(XML::parseXmlFile(file_name));

        /* Load translations */
        i18n.loadLangs("langs");

        /* Set log output */
        try {
            log_file.open(config->findChild("log").getAttribute("filename").c_str(), iostream::out);
            if(not log_file.fail()) {
                Util::log.setOutput(log_file);
            } else {
                cerr << "Failed to open log file" << endl;
            }
        } catch (XML::xml_error) {
            /* Log file is not set in the config */
        }

        /* Start the server */
		Core::init(*config);
		Core& core = Core::singleton();
        core.start();

        /* Run until the server shutdown on its own */
        core.join();

        /* Close the server */
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
