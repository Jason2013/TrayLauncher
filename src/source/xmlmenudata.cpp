#include "xmlmenudata.h"
#include "MenuData.h"
#include <sstream>

namespace {

const std::string ToString(const std::wstring & wstr, const int cp)
{
	int len = WideCharToMultiByte(cp, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	std::vector<char> buf(len);
	WideCharToMultiByte(cp, 0, wstr.c_str(), -1, &buf[0], len, NULL, NULL);
	return &buf[0];
}

const std::wstring ToWstring(const std::string & str, const int cp)
{
	DWORD flags = MB_PRECOMPOSED;
	if (CP_UTF7 == cp || CP_UTF8 == cp) {
		flags = 0;
	}
	const int len = MultiByteToWideChar(cp, flags, str.c_str(), -1, 0, 0);
	std::vector<wchar_t> buf(len);
	MultiByteToWideChar(cp, flags, str.c_str(), -1, &buf[0], buf.size());
	return &buf[0];
}

}
using namespace pugi;

// xml node and attribute names:

typedef const char * const XmlName;
XmlName xn_root = "root";

// names for xml menu
XmlName xn_menu_root = "menu";
XmlName xn_item = "item";
XmlName xn_sub = "is_menu";
XmlName xn_name = "name";
XmlName xn_path = "path";
XmlName xn_icon = "icon";

typedef const TCHAR * const XmlValue;
XmlValue xv_true = _T("true");


XmlMenuData::XmlMenuData()
{
	//ctor
}

XmlMenuData::~XmlMenuData()
{
	//dtor
}

bool XmlMenuData::LoadFile(const TString &filename)
{
	return m_xdoc.load_file(filename.c_str());
}

bool XmlMenuData::SaveFile(const TString &filename)
{
	return m_xdoc.save_file(filename.c_str());
}

namespace {

bool set_text(pugi::xml_node node, const pugi::char_t *value)
{
	bool result = false;
	if(node.type() == node_element) {
		xml_node f = node.first_child();
		if (!f)
		{
			f = node.append_child(node_pcdata);
			f.set_value(value);
			result = true;
		}
		else if (f.type() == node_pcdata)
		{
			f.set_value(value);
			result = true;
		}
	}
	return result;
}

// get xml attribute, add one if not found.
pugi::xml_attribute got_attribute(pugi::xml_node node, const pugi::char_t *name)
{
	pugi::xml_attribute attr = node.attribute(name);
	return attr ? attr : node.append_attribute(name);
}

// get xml node, add one if not found.
pugi::xml_node got_child(pugi::xml_node node, const pugi::char_t *name)
{
	pugi::xml_node child = node.child(name);
	return child ? child : node.append_child(name);
}

bool set_child_text(pugi::xml_node parent, const pugi::char_t *name, const pugi::char_t *value)
{
	return set_text(got_child(parent, name), value);
}


inline const TString UTF8ToTString(const std::string &utf8)
{
	return ToWstring(utf8, CP_UTF8);
}

inline const std::string TStringToUTF8(const TString &str)
{
	return ToString(str, CP_UTF8);
}

inline void set_xml_attr(xml_node node, const pugi::char_t *name, const TString & value)
{
	if (!value.empty()) {
		got_attribute(node, name) = TStringToUTF8(value).c_str();
	}
}

inline const TString get_item_attr(xml_node node, const pugi::char_t *name)
{
	return UTF8ToTString(node.attribute(name).value());
}


inline bool IsSubMenu(xml_node item)
{
	return get_item_attr(item, xn_sub) == xv_true && item.child(xn_item);
}

inline bool IsMenuItem(xml_node item)
{
	return get_item_attr(item, xn_sub) != xv_true;
}

bool XmlToMD(xml_node node, CMenuData & menu)
{
	if (!node) {
		return false;
	}

	for (xml_node item = node.child(xn_item); item; item = item.next_sibling(xn_item)) {
		if (IsSubMenu(item)) {
			///
			menu.AddMenu(menu.Count(), _T(""), _T(""));
			CMenuData *pSub = menu.Menu(menu.Count()-1);
			if (pSub) {
				pSub->Name(get_item_attr(item, xn_name));
				pSub->Icon(get_item_attr(item, xn_icon));
				XmlToMD(item, *pSub);
			}
		} else if (IsMenuItem(item)) {
			menu.AddItem(menu.Count(), _T(""), _T(""));
			CItem *pItem = menu.Item(menu.Count()-1);
			if (pItem) {
				pItem->Name(get_item_attr(item, xn_name));
				pItem->Path(get_item_attr(item, xn_path));
				pItem->Ex(get_item_attr(item, xn_icon));
			}
		} else {
			// other thing?
		}
	}
	return true;
}


bool MDToXml(const CMenuData &menu, xml_node node)
{
	if (!menu.Count() || !node) {
		return false;
	}

	for (unsigned i = 0; i < menu.Count(); ++i) {
		if (menu.IsMenu(i)) {
			xml_node sub = node.append_child(xn_item);
			if (sub) {
				const CMenuData *p = menu.Menu(i);
				// set node attribs for sub menu
				set_xml_attr(sub, xn_sub, xv_true);

				set_xml_attr(sub, xn_name, p->Name());
				set_xml_attr(sub, xn_icon, p->Icon());
				MDToXml(*menu.Menu(i), sub);
			}
		} else {
			xml_node item = node.append_child(xn_item);
			if (item) {
				const CItem *p = menu.Item(i);
				set_xml_attr(item, xn_name, p->Name());

				set_xml_attr(item, xn_path, p->Path());
				set_xml_attr(item, xn_icon, p->Ex());
			}
		}
	}
	return true;
}
}

bool XmlToMenuData(XmlMenuData &xd, CMenuData &md)
{
	xml_document & doc = xd.XmlDoc();
	xml_node menu_root = doc.child(xn_root).child(xn_menu_root);
	return XmlToMD(menu_root, md);
}

bool MenuDataToXml(const CMenuData &md, XmlMenuData &xd)
{
	xml_document & doc = xd.XmlDoc();
	xml_node menu_root = got_child(got_child(doc, xn_root), xn_menu_root);
	return MDToXml(md, menu_root);
}
