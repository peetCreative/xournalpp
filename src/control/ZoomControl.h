/*
 * Xournal++
 *
 * Controls the zoom level
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <XournalType.h>

#include <gtk/gtk.h>
#include <vector>

#define DEFAULT_ZOOM_MAX 5
#define DEFAULT_ZOOM_MIN 0.3
#define DEFAULT_ZOOM_STEP 0.1
#define MIN_ZOOM_STEP 0.05
#define ZOOM_EPSILON 0.001

class XournalView;

enum ZoomModeType
{
	ZOOM_MODE_NONE,
	ZOOM_MODE_GESTURE,
	ZOOM_MODE_IN_STEP,
	ZOOM_MODE_OUT_STEP,
	ZOOM_MODE_IN_CTRL_SCROLL,
	ZOOM_MODE_OUT_CTRL_SCROLL,
	ZOOM_MODE_SLIDER,
	ZOOM_MODE_FIT,
	ZOOM_MODE_100,
	ZOOM_MODE_INIT
};


class ZoomListener
{
public:
	virtual void zoomChanged(double lastZoom) = 0;
	virtual void zoomRangeValuesChanged();

	virtual ~ZoomListener();
};

class ZoomControl
{
public:
	ZoomControl();
	virtual ~ZoomControl();

	void zoomOneStep(ZoomModeType);

	void zoomFit();
	void zoom100();

	double getZoom();
	//curZoomMode needs to be set before and after
	void setZoom(double zoom);

	void setZoom100(double zoom);
	void setZoomFit(double zoom);

	double getZoomFit();
	double getZoom100();

	void addZoomListener(ZoomListener* listener);

	void initZoomHandler(GtkWidget* widget, XournalView* view);

	void setCurrentPage(size_t currentPage);

	double getZoomCenterX();
	void setZoomCenterX(double zoomCenterX);

	double getZoomCenterY();
	void setZoomCenterY(double zoomCenterY);

	double getZoomStep();
	void setZoomStep(double zoomStep);

	double getZoomMax();
	void setZoomMax(double zoomMax);

	double getZoomMin();
	void setZoomMin(double zoomMin);

	ZoomModeType getCurZoomMode();
	void setCurZoomMode(ZoomModeType);

protected:
	void fireZoomChanged(double lastZoom);
	void fireZoomRangeValueChanged();

	static bool onScrolledwindowMainScrollEvent(GtkWidget* widget, GdkEventScroll* event, ZoomControl* zoom);

private:
	XOJ_TYPE_ATTRIB;

	XournalView* view;

	std::vector<ZoomListener*> listener;

	double zoom;

	double lastZoomValue;

	double zoom100Value;
	double zoomFitValue;

	size_t currentPage;

	double zoomCenterX;
	double zoomCenterY;

	double zoomStep;
	double zoomMax;
	double zoomMin;

	ZoomModeType curZoomMode;
};
