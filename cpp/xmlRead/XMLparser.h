// XMLparser.h
//
// pietro.govoni@mib.infn.it
//
// last change : $Date: $
// by          : $Author: $
//

#ifndef XMLPARSER
#define XMLPARSER

#include <string>
#include <fstream>

class XMLparser
{
  public:
    
    XMLparser(string fileName);
    ~XMLparser();
    string GetFirstTag(string TagName = "");

  private:

    string SearchStartTag();
    string ReadTagName();
    
    fstream m_fileXML;
    string m_fileName;
    
};

#endif
