#include "ObjectInputStream.h"

#include "Serializeable.h"

#include <string.h>

ObjectInputStream::ObjectInputStream()
{
	XOJ_INIT_TYPE(ObjectInputStream);

	this->str = NULL;
	this->pos = 0;
}

ObjectInputStream::~ObjectInputStream()
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	if (this->str)
	{
		g_string_free(this->str, true);
	}

	XOJ_RELEASE_TYPE(ObjectInputStream);
}

bool ObjectInputStream::read(const char* data, int len)
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	if (this->str)
	{
		g_string_free(this->str, true);
	}
	this->str = g_string_new_len(data, len);
	this->pos = 0;


	//	//clipboad debug
	//	FILE * fp = fopen("/home/andreas/tmp/xoj/clipboard.bin", "w");
	//	fwrite(str->str, len, 1, fp);
	//	fclose(fp);

	try
	{
		string version = readString();
		if (version != XML_VERSION_STR)
		{
			g_warning("ObjectInputStream version mismatch... two different Xournal versions running? (%s / %s)",
					  version.c_str(), XML_VERSION_STR);
			return false;
		}
	}
	catch (InputStreamException& e)
	{
		g_warning("InputStreamException: %s", e.what());
		return false;
	}
	return true;
}

void ObjectInputStream::readObject(const char* name)
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	string type = readObject();
	if (type != name)
	{
		throw INPUT_STREAM_EXCEPTION("Try to read object type {1} but read object type {2}", name, type);
	}
}

string ObjectInputStream::readObject()
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	checkType('{');
	return readString();
}

string ObjectInputStream::getNextObjectName()
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	int pos = this->pos;
	checkType('{');
	string name = readString();

	this->pos = pos;

	return name;
}

void ObjectInputStream::endObject()
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	checkType('}');
}

int ObjectInputStream::readInt()
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	checkType('i');

	if (this->pos + sizeof(int) >= this->str->len)
	{
		throw InputStreamException("End reached, but try to read an integer", __FILE__, __LINE__);
	}

	int i = *((int*) (this->str->str + this->pos));
	this->pos += sizeof(int);
	return i;
}

double ObjectInputStream::readDouble()
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	checkType('d');

	if (this->pos + sizeof(double) >= this->str->len)
	{
		throw InputStreamException("End reached, but try to read an double", __FILE__, __LINE__);
	}

	double d = *((double*) (this->str->str + this->pos));
	this->pos += sizeof(double);
	return d;
}

string ObjectInputStream::readString()
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	checkType('s');

	if (this->pos + sizeof(int) >= this->str->len)
	{
		throw InputStreamException("End reached, but try to read an string", __FILE__, __LINE__);
	}

	int len = *((int*) (this->str->str + this->pos));
	this->pos += sizeof(int);

	if (this->pos + len >= this->str->len)
	{
		throw InputStreamException("End reached, but try to read an string", __FILE__, __LINE__);
	}

	string s((const char*) (this->str->str + this->pos), len);
	this->pos += len;
	return s;
}

void ObjectInputStream::readData(void** data, int* length)
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	checkType('b');

	if (this->pos + 2 * sizeof(int) >= this->str->len)
	{
		throw InputStreamException("End reached, but try to read data", __FILE__, __LINE__);
	}

	int len = *((int*) (this->str->str + this->pos));
	this->pos += sizeof(int);

	int width = *((int*) (this->str->str + this->pos));
	this->pos += sizeof(int);

	if (this->pos + (len * width) >= this->str->len)
	{
		throw InputStreamException("End reached, but try to read data", __FILE__, __LINE__);
	}

	if (len == 0)
	{
		*length = 0;
		*data = NULL;
	}
	else
	{
		*data = g_malloc(len * width);
		*length = len;

		memcpy(*data, this->str->str + this->pos, len * width);

		this->pos += len * width;
	}

}

class PngDatasource
{
public:

	PngDatasource(char* start, int len)
	{
		this->data = start;
		this->len = len;
		this->pos = 0;
	}

	char* data;
	int len;
	int pos;
};

cairo_status_t cairoReadFunction(PngDatasource* obj, unsigned char* data, unsigned int length)
{
	for (unsigned int i = 0; i < length; i++, obj->pos++)
	{
		if (obj->pos >= obj->len)
		{
			return CAIRO_STATUS_READ_ERROR;
		}
		data[i] = obj->data[obj->pos];
	}

	return CAIRO_STATUS_SUCCESS;
}

cairo_surface_t* ObjectInputStream::readImage()
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	checkType('m');

	if (this->pos + sizeof(int) >= this->str->len)
	{
		throw InputStreamException("End reached, but try to read an image", __FILE__, __LINE__);
	}

	int len = *((int*) (this->str->str + this->pos));
	//this->pos += sizeof(int);
	//totally not equivalent!
	this->pos += sizeof(gsize);

	if (this->pos + len >= this->str->len)
	{
		throw InputStreamException("End reached, but try to read an image", __FILE__, __LINE__);
	}

	PngDatasource source(this->str->str + this->pos, len);
	//cairo_surface_t * img = cairo_image_surface_create_from_png_stream((cairo_read_func_t) cairoReadFunction, &source);
	cairo_surface_t* img = cairo_image_surface_create_from_png_stream((cairo_read_func_t) &cairoReadFunction, &source);

	this->pos += len;

	return img;
}

ObjectInputStream& ObjectInputStream::operator>>(Serializeable* s)
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	s->readSerialized(*this);
	return *this;
}

void ObjectInputStream::checkType(char type)
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	if (this->pos + 2 > this->str->len)
	{
		throw INPUT_STREAM_EXCEPTION("End reached, but try to read {1}, index {2} of {3}",
									 getType(type), this->pos, this->str->len);
	}
	if (this->str->str[this->pos] != '_')
	{
		throw INPUT_STREAM_EXCEPTION("Expected type signature of {1}, index {2} of {3}, but read '{4}'",
									 getType(type), this->pos, this->str->len, this->str->str[this->pos]);
	}
	this->pos++;

	if (this->str->str[this->pos] != type)
	{
		throw INPUT_STREAM_EXCEPTION("Expected {1} but read {2}", getType(type), getType(this->str->str[this->pos]));
	}

	this->pos++;
}

string ObjectInputStream::getType(char type)
{
	XOJ_CHECK_TYPE(ObjectInputStream);

	string ret;
	if (type == '{')
	{
		ret = "Object begin";
	}
	else if (type == '}')
	{
		ret = "Object end";
	}
	else if (type == 'i')
	{
		ret = "Number";
	}
	else if (type == 'd')
	{
		ret = "Floating point";
	}
	else if (type == 's')
	{
		ret = "String";
	}
	else if (type == 'b')
	{
		ret = "Binary";
	}
	else if (type == 'm')
	{
		ret = "Image";
	}
	else
	{
		char* str = g_strdup_printf("Unknown type: %02x (%c)", type, type);
		ret = str;
		g_free(str);
	}

	return ret;
}
