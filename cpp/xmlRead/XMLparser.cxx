// XMLparser.cxx
//
// pietro.govoni@mib.infn.it
//
// last change : $Date: $
// by          : $Author: $
//

#include "XMLparser.h"

XMLparser::XMLparser(string fileName):
  m_fileName(fileName)
{
  m_fileXML.open(m_fileName.c_str(),ios::in);
}

XMLparser::~XMLparser()
{
  m_fileXML.close();
}


string XMLparser::GetFirstTag(string TagName = "")
{
  char charBuffer;
  /*
  1. trovo apertura tag
  2. leggo il tag
  	1. non ok => continuo
	2. ok => leggo
  3. leggo il contenuto fino al primo <
  4. controllo sia la chiusura giusta del tag
  */

  SearchStartTag();
  if (TagName == readTagName()) {/*voila!*/}
    
  for (;;)
    {
      m_fileXML >> charBuffer;
      if (charBuffer == "<")
        {
          readTagName();
	}
    }
}

string XMLparser::SearchStartTag()
{
  char charBuffer;
  for (;;)
    { 
      m_fileXML >> charBuffer;
      if (charBuffer == "<") 
	{
          m_fileXML >> charBuffer;
          if (charBuffer == "/") 
	  
	}
      break;
    }  
}

string XMLparser::ReadTagName()
{
  string TagName = "";
  char charBuffer;
  for (;;)
    { 
      m_fileXML >> charBuffer;
      if (charBuffer == ">") break;
      TagName += charBuffer;
    }  
  return TagName;
}
