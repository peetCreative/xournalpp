#include "Font.h"

#include <serializing/ObjectInputStream.h>
#include <serializing/ObjectOutputStream.h>

XojFont::XojFont()
{
	XOJ_INIT_TYPE(XojFont);

	this->size = 0;
}

XojFont::~XojFont()
{
	XOJ_RELEASE_TYPE(XojFont);
}

string XojFont::getName()
{
	XOJ_CHECK_TYPE(XojFont);

	return this->name;
}

void XojFont::setName(string name)
{
	XOJ_CHECK_TYPE(XojFont);

	this->name = name;
}

double XojFont::getSize()
{
	XOJ_CHECK_TYPE(XojFont);

	return size;
}

void XojFont::setSize(double size)
{
	XOJ_CHECK_TYPE(XojFont);

	this->size = size;
}

void XojFont::operator=(const XojFont& font)
{
	XOJ_CHECK_TYPE(XojFont);

	this->name = font.name;
	this->size = font.size;
}

void XojFont::serialize(ObjectOutputStream& out)
{
	XOJ_CHECK_TYPE(XojFont);

	out.writeObject("XojFont");

	out.writeString(this->name);
	out.writeDouble(this->size);

	out.endObject();
}

void XojFont::readSerialized(ObjectInputStream& in)
{
	XOJ_CHECK_TYPE(XojFont);

	in.readObject("XojFont");

	this->name = in.readString();
	this->size = in.readDouble();

	in.endObject();
}
