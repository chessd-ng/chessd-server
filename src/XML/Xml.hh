#ifndef XML_HH
#define XML_HH

#include <vector>
#include <map>
#include <string>
#include "Tag.hh"
#include "CData.hh"
#include "TagGenerator.hh"
#include "Description.hh"

namespace XML {

	/* Loads a xml file */
	Tag* loadXmlFile(const std::string& filename);

}

#endif
