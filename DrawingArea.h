#ifndef MYAREA_H
#define MYAREA_H

#include <vector>
#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>

class MyArea : public Gtk::DrawingArea
{
public:
	MyArea();
	MyArea(double numOfRows, double numOfCols);

	virtual ~MyArea();

	void setNumOfGridCols(double numOfCols);

protected:
	double numRows;
	double numCols;

	// Override default signal handler:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

	// functions
	void drawAreaBorder(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
	void drawAreaGrid(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
	void labelArea(const Cairo::RefPtr< Cairo::Context >& cr, double xPos, double yPos, Glib::ustring msgLbl);
};

#endif // MYAREA_H