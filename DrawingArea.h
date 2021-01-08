#ifndef MYAREA_H
#define MYAREA_H

#include <fmt/core.h>
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
	// Constructors and deconstructor
	MyArea();
	MyArea(double numOfRows, double numOfCols);
	virtual ~MyArea();

	void setNumOfGridCols(double numOfCols);
	void listAvailableFonts();
	// Used to get data from ThingSpeak class (fieldNum must be between 1 and 8)
	void getFieldData(ThingSpeak tsObj);
	bool toggleField(int fieldNum);

protected:
	const double M_PI = 3.14159265358979323846;
	const int dailyPoints = 48;
	const int weeklyPoints = 336;
	double numRows, numCols;
	float highestTemp, lowestTemp;
	int fieldEnable[2] = { 1, 0 }; // determines if fields should be plotted
	vector<float> field1, field2;

	// Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

	// Main functions
	void drawAreaBorder(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
	void drawAreaGrid(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
	void plotThingSpeakData(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
	void labelArea(const Cairo::RefPtr< Cairo::Context >& cr, double xPos, double yPos, Glib::ustring msgLbl);
	void getTempRange();
};

#endif // MYAREA_H