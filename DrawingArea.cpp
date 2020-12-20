#include "DrawingArea.h"
#include <iostream>
using std::cout; using std::endl;

MyArea::MyArea() : numRows(10.0), numCols(10.0) {}

MyArea::MyArea(double numOfRows, double numOfCols) :
	numRows(numOfRows), numCols(numOfCols) {}

MyArea::~MyArea() {}

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	Gtk::Allocation allocation = get_allocation();
	const int dAreaWidth = allocation.get_width();
	const int dAreaHeight = allocation.get_height();

	// initialize drawing area
	drawAreaGrid(cr, dAreaWidth, dAreaHeight);
	drawAreaBorder(cr, dAreaWidth, dAreaHeight);

	return true;
}

void MyArea::drawAreaBorder(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height)
{
	// create a box outline around drawing area border
	cr->set_line_width(7.0);
	cr->set_source_rgb(0.5, 0.5, 0.8);

	cr->move_to(0, 0);
	cr->line_to(width, 0);
	cr->line_to(width, height);
	cr->line_to(0, height);
	cr->line_to(0, height);
	cr->line_to(0, 0);
	cr->stroke();
}

void MyArea::drawAreaGrid(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height)
{
	auto gridColNum = width / numCols;
	auto gridRowNum = height / numRows;

	cr->set_line_width(1.0);
	cr->set_source_rgb(0.7, 0.7, 0.7);

	// draw grid columns
	for (int i = 0; i < (numCols - 1); ++i)
	{
		cr->move_to(gridColNum, 0);
		cr->line_to(gridColNum, height);

		gridColNum += (width / numCols);
	}

	// draw grid rows
	for (int i = 0; i < (numRows - 1); ++i)
	{
		cr->move_to(0, gridRowNum);
		cr->line_to(width, gridRowNum);

		gridRowNum += (height / numRows);
	}

	cr->stroke();

	// create graph axis
	gridColNum = width / numCols;
	gridRowNum = height / numRows;

	cr->set_line_width(2.0);
	cr->set_source_rgb(0.0, 0.0, 0.0);

	cr->move_to(gridColNum, gridRowNum);
	cr->line_to(gridColNum, (height - gridRowNum));
	cr->line_to((width - gridColNum), (height - gridRowNum));

	cr->stroke();

	// label grid rows and columns
	Glib::ustring graphColIncrement[11];
	determineColLbls(graphColIncrement);

	Glib::ustring graphRowIncrement[] = { "30", "40", "50", "60", "70", "80", "90", "100", "110", "120" };

	const double vertOffset = 10.0;
	const double horizOffset = 60.0;

	for (int i = 0; i < (numCols - 1); ++i) // columns
	{
		labelArea(cr, (gridColNum + vertOffset), (height - gridRowNum + vertOffset), graphColIncrement[i]);

		gridColNum += (width / numCols);
	}

	gridColNum = width / numCols;
	for (int i = (numRows - 3); i >= 0; --i) // rows
	{
		labelArea(cr, (gridColNum - horizOffset), (gridRowNum + vertOffset), (graphRowIncrement[i] + " F"));

		gridRowNum += (height / numRows);
	}
}

void MyArea::labelArea(const Cairo::RefPtr< Cairo::Context >& cr, double xPos, double yPos, Glib::ustring msgLbl)
{
	int txtWidth;
	int txtHeight;

	Pango::FontDescription font;
	font.set_family("Monospace");
	font.set_size(16);
	font.set_weight(Pango::WEIGHT_BOLD);

	auto layout = create_pango_layout(msgLbl);
	layout->set_font_description(font);
	layout->get_pixel_size(txtWidth, txtHeight);

	cr->move_to(xPos, yPos);
	layout->show_in_cairo_context(cr);
}

void MyArea::setNumOfGridCols(double numOfCols)
{
	numCols = numOfCols;
	this->queue_draw();
}

void MyArea::determineColLbls(Glib::ustring(&lblArr)[11])
{
	if (numCols == 8.0) // 24 hour rendering
	{
		lblArr[0] = "00:00";
		lblArr[1] = "04:00";
		lblArr[2] = "08:00";
		lblArr[3] = "12:00";
		lblArr[4] = "16:00";
		lblArr[5] = "20:00";
	}
	else if (numCols == 9.0) // 7 day rendering
	{
		lblArr[0] = "Sun";
		lblArr[1] = "Mon";
		lblArr[2] = "Tue";
		lblArr[3] = "Wed";
		lblArr[4] = "Thu";
		lblArr[5] = "Fri";
		lblArr[6] = "Sat";
	}
	else // past 30 days rendering
	{
		lblArr[0] = "0";
		lblArr[1] = "3";
		lblArr[2] = "6";
		lblArr[3] = "9";
		lblArr[4] = "12";
		lblArr[5] = "15";
		lblArr[6] = "18";
		lblArr[7] = "21";
		lblArr[8] = "24";
		lblArr[9] = "27";
	}
}