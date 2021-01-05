#ifndef MYAREA_H
#define MYAREA_H

#include <vector>
#include <map>
#include <string>
#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>
#include <glib.h>
#include <pango/pangocairo.h>

#include "ThingSpeak.h"

class MyArea : public Gtk::DrawingArea
{
public:
	MyArea();
	MyArea(double numOfRows, double numOfCols);

	virtual ~MyArea();

	void setNumOfGridCols(double numOfCols);
	void listAvailableFonts();
	// Used to get data from ThingSpeak class (fieldNum must be between 1 and 8)
	void getFieldData(ThingSpeak tsObj);

protected:
	double numRows;
	double numCols;
	int amntOfPoints;
	vector<float> field1, field2, field3, field4, field5, field6, field7, field8;

	// Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

	// Main functions
	void drawAreaBorder(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
	void drawAreaGrid(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
	void plotThingSpeakData(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
	void labelArea(const Cairo::RefPtr< Cairo::Context >& cr, double xPos, double yPos, Glib::ustring msgLbl);

};

#endif // MYAREA_H