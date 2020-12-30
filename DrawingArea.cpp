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

	// plot current thingspeak data
	plotThingSpeakData(cr, dAreaWidth, dAreaHeight);

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
	cr->set_source_rgb(0.75, 0.75, 0.75);

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
	Glib::ustring graphRowIncrement[] = { "30", "40", "50", "60", "70", "80", "90", "100", "110", "120" };
	
	Glib::ustring graphColIncrement;
	if (numCols == 8.0) graphColIncrement = "Past 24 Hours";
	else if (numCols == 9.0) graphColIncrement = "Past Week";
	else graphColIncrement = "Past Month";

	const double vertColOffset = 15.0;
	const double vertRowOffset = 10.0;
	const double horizOffset = 60.0;

	labelArea(cr, (width/2 - horizOffset), (height - gridRowNum + vertColOffset), graphColIncrement); // x-axis label

	for (int i = (numRows - 3); i >= 0; --i) // rows
	{
		labelArea(cr, (gridColNum - horizOffset), (gridRowNum + vertRowOffset), (graphRowIncrement[i] + " F"));

		gridRowNum += (height / numRows);
	}
}

void MyArea::plotThingSpeakData(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height)
{
	// determine graph boundaries
	auto gridHorizLeft = width / numCols;
	auto gridHorizRight = width - (width / numCols);
	auto gridVertTop = height / numRows;
	auto gridVertBottom = height - (height / numRows);

	cr->set_line_width(2);
	cr->set_source_rgb(1.0, 0.0, 0.0);

	// TODO: Create plotting logic for graph
	cr->move_to(gridHorizLeft, gridVertTop);
	cr->line_to(gridHorizRight, gridVertBottom);
	cr->stroke();

	
}

void MyArea::labelArea(const Cairo::RefPtr< Cairo::Context >& cr, double xPos, double yPos, Glib::ustring msgLbl)
{
	int txtWidth;
	int txtHeight;

	Pango::FontDescription font;
	font.set_family("Roboto Slab");
	font.set_absolute_size(20.0 * PANGO_SCALE);
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

// Obtained online, used for debugging font selection
void MyArea::listAvailableFonts()
{
	int i;
	PangoFontFamily** families;
	int n_families;
	PangoFontMap* fontmap;

	fontmap = pango_cairo_font_map_get_default();
	pango_font_map_list_families(fontmap, &families, &n_families);
	printf("There are %d families\n", n_families);
	for (i = 0; i < n_families; i++) {
		PangoFontFamily* family = families[i];
		const char* family_name;

		family_name = pango_font_family_get_name(family);
		printf("Family %d: %s\n", i, family_name);
	}
	g_free(families);
}