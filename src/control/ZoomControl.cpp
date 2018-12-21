#include "ZoomControl.h"

#include "gui/XournalView.h"

#include <cmath>

ZoomListener::~ZoomListener() { }

ZoomControl::ZoomControl()
 : view(NULL)
{
	XOJ_INIT_TYPE(ZoomControl);

	this->zoom = 1.0;
	this->lastZoomValue = 1.0;
	this->zoom100Value = 1.0;
	this->zoomFitValue = 1.0;
	this->curZoomMode = ZOOM_MODE_FIT;
	this->currentPage = 0;
	this->zoomCenterX = -1;
	this->zoomCenterY = -1;
	this->zoomStep = DEFAULT_ZOOM_STEP;
	this->zoomMax = DEFAULT_ZOOM_MAX;
	this->zoomMin = DEFAULT_ZOOM_MIN;
}

ZoomControl::~ZoomControl()
{
	XOJ_CHECK_TYPE(ZoomControl);

	XOJ_RELEASE_TYPE(ZoomControl);
}

void ZoomControl::addZoomListener(ZoomListener* listener)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->listener.push_back(listener);
}

void ZoomControl::initZoomHandler(GtkWidget* widget, XournalView* view)
{
	XOJ_CHECK_TYPE(ZoomControl);

	g_signal_connect(widget, "scroll_event", G_CALLBACK(onScrolledwindowMainScrollEvent), this);
	this->view = view;
}

void ZoomControl::setCurrentPage(size_t currentPage)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->currentPage = currentPage;
}

double ZoomControl::getZoomCenterX()
{
	XOJ_CHECK_TYPE(ZoomControl);

	return this->zoomCenterX;
}
void ZoomControl::setZoomCenterX(double zoomCenterX)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->zoomCenterX = zoomCenterX;
}
double ZoomControl::getZoomCenterY()
{
	XOJ_CHECK_TYPE(ZoomControl);

	return this->zoomCenterY;
}
void ZoomControl::setZoomCenterY(double zoomCenterY)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->zoomCenterY = zoomCenterY;
}
double ZoomControl::getZoomStep()
{
	return this->zoomStep;
}
void ZoomControl::setZoomStep(double zoomStep)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->zoomStep = zoomStep;
}
double ZoomControl::getZoomMax()
{
	XOJ_CHECK_TYPE(ZoomControl);

	return this->zoomMax;
}
void ZoomControl::setZoomMax(double zoomMax)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->zoomMax = zoomMax;
}
double ZoomControl::getZoomMin()
{
	XOJ_CHECK_TYPE(ZoomControl);

	return this->zoomMin;
}
void ZoomControl::setZoomMin(double zoomMin)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->zoomMin = zoomMin;
}

ZoomModeType ZoomControl::getCurZoomMode()
{
	XOJ_CHECK_TYPE(ZoomControl);

	return this->curZoomMode;
}

void ZoomControl::setCurZoomMode(ZoomModeType curZoomMode)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->curZoomMode = curZoomMode;
}

void ZoomControl::fireZoomChanged(double lastZoom)
{
	XOJ_CHECK_TYPE(ZoomControl);

	if (this->zoom < this->zoomMin)
	{
		this->zoom = this->zoomMin;
	}

	if (this->zoom > this->zoomMax)
	{
		this->zoom = this->zoomMax;
	}

	if (std::abs(this->zoom - lastZoom) < ZOOM_EPSILON)
	{
		this->zoom = lastZoom;
		return;
	}
// 	// Store current page
// 	size_t page = this->currentPage;

	for (ZoomListener* z : this->listener)
	{
		z->zoomChanged(lastZoom);
	}

// 	if (view != NULL)
// 	{
// 		// Restore page
// 		view->scrollTo(page, 0);
// 	}
}

void ZoomControl::fireZoomRangeValueChanged()
{
	XOJ_CHECK_TYPE(ZoomControl);

	for (ZoomListener* z : this->listener)
	{
		z->zoomRangeValuesChanged();
	}
}

double ZoomControl::getZoom()
{
	XOJ_CHECK_TYPE(ZoomControl);

	return this->zoom;
}

void ZoomControl::setZoom(double zoom)
{
	XOJ_CHECK_TYPE(ZoomControl);

	if (this->curZoomMode != ZOOM_MODE_NONE)
	{
		double lastZoom = this->zoom;
		this->zoom = zoom;
		fireZoomChanged(lastZoom);
	}
}

void ZoomControl::setZoom100(double zoom)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->zoom100Value = zoom;
	fireZoomRangeValueChanged();
}

void ZoomControl::setZoomFit(double zoom)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->zoomFitValue = zoom;
	fireZoomRangeValueChanged();

	if (this->curZoomMode == ZOOM_MODE_FIT)
	{
		this->setZoom(this->zoomFitValue);
	}
}

double ZoomControl::getZoomFit()
{
	XOJ_CHECK_TYPE(ZoomControl);

	return this->zoomFitValue;
}

double ZoomControl::getZoom100()
{
	XOJ_CHECK_TYPE(ZoomControl);

	return this->zoom100Value;
}

void ZoomControl::zoom100()
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->curZoomMode = ZOOM_MODE_100;
	this->setZoom(this->zoom100Value);
	this->curZoomMode = ZOOM_MODE_NONE;
}

void ZoomControl::zoomFit()
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->curZoomMode = ZOOM_MODE_FIT;
	this->setZoom(this->zoomFitValue);
}

void ZoomControl::zoomOneStep(ZoomModeType mode)
{
	XOJ_CHECK_TYPE(ZoomControl);

	this->setCurZoomMode(mode);
	switch (mode) {
		case ZOOM_MODE_IN_STEP:
			this->setZoom(this->zoom + this->zoomStep);
			this->setCurZoomMode(ZOOM_MODE_NONE);
			break;
		case ZOOM_MODE_OUT_STEP:
			this->setZoom(this->zoom - this->zoomStep);
			this->setCurZoomMode(ZOOM_MODE_NONE);
			break;
		case ZOOM_MODE_IN_CTRL_SCROLL:
			this->setZoom(this->zoom + this->zoomStep);
			break;
		case ZOOM_MODE_OUT_CTRL_SCROLL:
			this->setZoom(this->zoom - this->zoomStep);
			break;
		default:
			this->setCurZoomMode(ZOOM_MODE_NONE);
	}
}

bool ZoomControl::onScrolledwindowMainScrollEvent(GtkWidget* widget, GdkEventScroll* event, ZoomControl* zoom)
{
	XOJ_CHECK_TYPE_OBJ(zoom, ZoomControl);

	guint state = event->state & gtk_accelerator_get_default_mod_mask();

	// do not handle e.g. ALT + Scroll (e.g. Compiz use this shortcut for setting transparency...)
	if (state != 0 && (state & ~(GDK_CONTROL_MASK | GDK_SHIFT_MASK)))
	{
		return true;
	}

	if (state & GDK_CONTROL_MASK)
	{
		//set zoom center (for shift centered scroll)
		zoom->setZoomCenterX(event->x);
		zoom->setZoomCenterY(event->y);

		if (event->direction == GDK_SCROLL_UP || event->direction == GDK_SCROLL_LEFT)
		{
			zoom->zoomOneStep(ZOOM_MODE_IN_CTRL_SCROLL);
		}
		else if (event->direction == GDK_SCROLL_UP || event->direction == GDK_SCROLL_LEFT)
		{
			zoom->zoomOneStep(ZOOM_MODE_OUT_CTRL_SCROLL);
		}
		else if (event->direction == GDK_SCROLL_SMOOTH)
		{
			gdouble delta_x = 0;
			gdouble delta_y = 0;
			if (gdk_event_get_scroll_deltas((GdkEvent*) event, &delta_x, &delta_y) &&
				(std::abs(delta_y) > ZOOM_EPSILON))
			{
				ZoomModeType mode;
				if (delta_y > 0)
					mode = ZOOM_MODE_IN_CTRL_SCROLL;
				else
					mode = ZOOM_MODE_OUT_CTRL_SCROLL;
				double zoom_step = zoom->getZoomStep();
				if (std::abs(delta_y) > zoom_step)
				{
					zoom->zoomOneStep(mode);
				}
				else
				{
					zoom->setCurZoomMode(mode);
					// Minus delta because scrolling down means srcolling out
					zoom->setZoom(zoom->getZoom() - delta_y);
				}
			}
			// I think at the end is_stop gets one maybe we can use this for when to redraw
			if (event->is_stop == 1)
			{
				zoom->setCurZoomMode(ZOOM_MODE_NONE);
			}
		}

		return true;
	}

	// Shift + Wheel scrolls the in the perpendicular direction
	if (state & GDK_SHIFT_MASK)
	{
		if (event->direction == GDK_SCROLL_UP)
		{
			event->direction = GDK_SCROLL_LEFT;
		}
		else if (event->direction == GDK_SCROLL_LEFT)
		{
			event->direction = GDK_SCROLL_UP;
		}
		else if (event->direction == GDK_SCROLL_DOWN)
		{
			event->direction = GDK_SCROLL_RIGHT;
		}
		else if (event->direction == GDK_SCROLL_RIGHT)
		{
			event->direction = GDK_SCROLL_DOWN;
		}

		event->state &= ~GDK_SHIFT_MASK;
		state &= ~GDK_SHIFT_MASK;
	}

	return false;
}

void ZoomListener::zoomRangeValuesChanged() { }
