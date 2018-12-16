/*
 * Xournal++
 *
 * An element on the Document
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <serializing/Serializeable.h>
#include <Rectangle.h>
#include <XournalType.h>

#include <gtk/gtk.h>

#include <vector>

enum ElementType
{
	ELEMENT_STROKE = 1, ELEMENT_IMAGE, ELEMENT_TEXIMAGE, ELEMENT_TEXT
};

class ShapeContainer
{
public:
	virtual bool contains(double x, double y) = 0;

	virtual ~ShapeContainer() { }
};

class Element : public Serializeable
{
protected:
	Element(ElementType type);

public:
	virtual ~Element();

public:
	ElementType getType() const;

	void setX(double x);
	void setY(double y);
	double getX();
	double getY();

	virtual void move(double dx, double dy);
	virtual void scale(double x0, double y0, double fx, double fy) = 0;
	virtual void rotate(double x0, double y0, double xo, double yo, double th) = 0;

	void setColor(int color);
	int getColor() const;

	double getElementWidth();
	double getElementHeight();

	Rectangle boundingRect();

	virtual bool intersectsArea(const GdkRectangle* src);
	virtual bool intersectsArea(double x, double y, double width, double height);

	virtual bool isInSelection(ShapeContainer* container);

	virtual bool rescaleOnlyAspectRatio();

	/**
	 * Take 1:1 copy of this element
	 */
	virtual Element* clone() = 0;

private:
	XOJ_TYPE_ATTRIB;

protected:
	// The position on the screen
	double x;
	double y;

	virtual void calcSize() = 0;

	double width;
	double height;

	void serializeElement(ObjectOutputStream& out);
	void readSerializedElement(ObjectInputStream& in);

protected:
	// If the size has been calculated
	bool sizeCalculated;

private:
	ElementType type;

	// The color in RGB format
	int color;
};

typedef std::vector<Element*> ElementVector;
