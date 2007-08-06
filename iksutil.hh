#ifndef IKSUTIL_HH
#define IKSUTIL_HH

#include <iksemel.h>
#include <string>
#include "xml.hh"

/* Transform a Tag into a iksemel tree */
iks* tag2iks(const XML::Tag* tag);

/* Transform a iksemel tree into a Tag */
XML::Tag* iks2tag(iks* tree); 

/* Read an xml from a file */
XML::Tag* iksReadXMLFile(const std::string& filename); 
XML::Tag* iksReadXMLString(const std::string& xml); 

#endif
