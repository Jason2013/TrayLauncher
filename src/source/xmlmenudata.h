#ifndef XMLMENUDATA_H
#define XMLMENUDATA_H

#include "pugixml/pugixml.hpp"
#include "stdheaders.h"


class XmlMenuData
{
public:
	XmlMenuData();
	~XmlMenuData();
	bool LoadFile(const TString &filename);
	bool SaveFile(const TString &filename);
	pugi::xml_document & XmlDoc() { return m_xdoc; }
protected:
private:
	pugi::xml_document m_xdoc;
};

class CMenuData;

bool XmlToMenuData(XmlMenuData &xd, CMenuData &md);
bool MenuDataToXml(const CMenuData &md, XmlMenuData &xd);

#endif // XMLMENUDATA_H
