#include "ScrollHandler.h"
#include "../model/Page.h"
#include "Control.h"

ScrollHandler::ScrollHandler(Control * control) {
	this->control = control;
}

ScrollHandler::~ScrollHandler() {
}

void ScrollHandler::goToPreviousPage() {
	if (control->getWindow()) {
		scrollToPage(control->getWindow()->getXournal()->getCurrentPage() - 1);
	}
}

void ScrollHandler::goToNextPage() {
	if (control->getWindow()) {
		scrollToPage(control->getWindow()->getXournal()->getCurrentPage() + 1);
	}
}

void ScrollHandler::scrollToPage(XojPage * page, double top) {
	Document * doc = this->control->getDocument();

	doc->lock();
	int p = doc->indexOf(page);
	doc->unlock();

	if (p != -1) {
		scrollToPage(p, top);
	}
}

void ScrollHandler::scrollToPage(int page, double top) {
	if (control->getWindow()) {
		control->getWindow()->getXournal()->scrollTo(page, top);
	}
}

void ScrollHandler::adjustmentScroll(GtkAdjustment * adj, double scroll, int size) {
	double v = gtk_adjustment_get_value(adj);
	double max = gtk_adjustment_get_upper(adj) - size;

	double newPos = v + scroll;
	if (newPos < 0) {
		newPos = 0;
	}

	if (newPos > max) {
		newPos = max;
	}

	gtk_adjustment_set_value(adj, newPos);
}

void ScrollHandler::scrollRelative(double x, double y) {
	if (!control->getWindow()) {
		return;
	}
	GtkWidget * scroll = control->getWindow()->get("scrolledwindowMain");

	GtkAdjustment * hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll));
	GtkAdjustment * vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scroll));

	GtkAllocation alloc = { 0 };
	gtk_widget_get_allocation(scroll, &alloc);

	adjustmentScroll(hadj, x, alloc.width);
	adjustmentScroll(vadj, y, alloc.height);
}

void ScrollHandler::scrollToSpinPange() {
	if (!control->getWindow()) {
		return;
	}
	GtkWidget * spinPageNo = control->getWindow()->getSpinPageNo();
	int page = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinPageNo));
	if (page == 0) {
		return;
	}
	scrollToPage(page - 1);
}

void ScrollHandler::scrollToAnnotatedPage(bool next) {
	if (!control->getWindow()) {
		return;
	}

	int step;
	if (next) {
		step = 1;
	} else {
		step = -1;
	}

	Document * doc = control->getDocument();
	doc->lock();

	for (int i = control->getCurrentPageNo() + step; i >= 0 && i < doc->getPageCount(); i += step) {
		if (doc->getPage(i)->isAnnotated()) {
			scrollToPage(i);
			break;
		}
	}

	doc->unlock();
}

bool ScrollHandler::isPageVisible(int page) {
	if (!control->getWindow()) {
		return false;
	}

	return control->getWindow()->getXournal()->isPageVisible(page);
}
