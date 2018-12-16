/*
 * Xournal++
 *
 * Serialized input stream
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "InputStreamException.h"

#include <gtk/gtk.h>

class Serializeable;

class ObjectInputStream
{
public:
	ObjectInputStream();
	virtual ~ObjectInputStream();

public:
	bool read(const char* data, int len);

	void readObject(const char* name);
	string readObject();
	string getNextObjectName();
	void endObject();

	int readInt();
	double readDouble();
	string readString();

	void readData(void** data, int* len);
	cairo_surface_t* readImage();

	ObjectInputStream& operator>>(Serializeable* s);

private:
	void checkType(char type);

	string getType(char type);

private:
	XOJ_TYPE_ATTRIB;

	GString* str;
	gsize pos;
};
