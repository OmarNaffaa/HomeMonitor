#include "DrawingArea.h"
#include <iostream>
using std::cout; using std::endl;

MyArea::MyArea() : numRows(12.0), numCols(8.0), highestTemp(0), lowestTemp(200) {}

MyArea::MyArea(double numOfRows, double numOfCols) :
	numRows(numOfRows), numCols(numOfCols), highestTemp(0), lowestTemp(200) {}

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
	cr->line_to(0, 0);
	cr->stroke();
}

void MyArea::drawAreaGrid(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height)
{
	auto gridColNum = width / numCols;
	auto gridRowNum = height / numRows;

	auto gridHorizLeft = width / numCols;
	auto gridHorizRight = width - (width / numCols);
	auto gridVertTop = height / numRows;
	auto gridVertBottom = height - (height / numRows);

	cr->set_line_width(1.0);
	cr->set_source_rgb(0.7, 0.7, 0.7);

	// draw grid columns
	for (int i = 0; i < (numCols - 1); ++i)
	{
		cr->move_to(gridColNum, gridVertTop);
		cr->line_to(gridColNum, gridVertBottom);

		gridColNum += (width / numCols);
	}

	// draw grid rows
	for (int i = 0; i < (numRows - 1); ++i)
	{
		cr->move_to(gridHorizLeft, gridRowNum);
		cr->line_to(gridHorizRight, gridRowNum);

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
	Glib::ustring graphColIncrement;
	if (numCols == 8.0) graphColIncrement = "Past 24 Hours";
	if (numCols == 9.0) graphColIncrement = "Past Week";

	const double vertColOffset = 15.0;
	const double vertRowOffset = -15.0;
	const double horizOffset = 65.0;

	// label columns (x-axis)
	labelArea(cr, (width/2 - horizOffset), (height - gridRowNum + vertColOffset), graphColIncrement);

	// label rows (y-axis)
	getTempRange();
	float currTemp = highestTemp;
	float tempIncrement = (highestTemp - lowestTemp) / (numRows - 2);

	if (highestTemp != -200 || lowestTemp != 200 || highestTemp == lowestTemp)
	{
		for (int i = (numRows - 2); i >= 0; --i)
		{
			labelArea(cr, (gridColNum - horizOffset), (gridRowNum + vertRowOffset), (fmt::format("{:.1f}", currTemp) + " F"));

			gridRowNum += (height / numRows);
			currTemp -= tempIncrement;
		}
	}
}

void MyArea::plotThingSpeakData(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height)
{
	// determine graph constants
	int amntOfPointsField1 = field1.size();
	int amntOfPointsField2 = field2.size();
	float pointRadius = ((numCols - 8.0) < 0.01) ? 4.0 : 0.0;

	getTempRange();
	float tempRange = highestTemp - lowestTemp;
	float xAxisOffset = lowestTemp;

	auto gridHorizLeft = width / numCols;
	auto gridHorizRight = width - (width / numCols);
	auto gridVertTop = height / numRows;
	auto gridVertBottom = height - (height / numRows);

	auto vertSpacing = (gridVertBottom - gridVertTop) / tempRange;
	auto horizSpacingField1 = (gridHorizRight - gridHorizLeft) / (amntOfPointsField1 - 1);
	auto horizSpacingField2 = (gridHorizRight - gridHorizLeft) / (amntOfPointsField2 - 1);

	double incVal;

	cr->set_line_width(3);

	/*
		Plotting logic for graph
	*/
	// Thingspeak field 1
	if (fieldEnable[0] == 1 && field1.size() > 1)
	{
		incVal = gridHorizLeft;
		double currPoint;
		double prevPoint = gridVertBottom - ((field1[0] - xAxisOffset) * vertSpacing);

		cr->begin_new_path();
		cr->set_source_rgb(0.0, 0.7, 0.0);

		for (int i = 1; i < field1.size(); ++i)
		{
			if (field1[i] > 19) // values must be at least 20
			{
				currPoint = gridVertBottom - ((field1[i] - xAxisOffset) * vertSpacing);

				cr->move_to(incVal, prevPoint);
				cr->arc(incVal, prevPoint, pointRadius, 0.0, 2*M_PI);
				cr->close_path();
				cr->fill_preserve();
				cr->line_to((incVal + horizSpacingField1), currPoint);
				cr->stroke();

				incVal += horizSpacingField1;
				prevPoint = currPoint;
			}
		}

		cr->arc(incVal, prevPoint, (pointRadius + 1.0), 0.0, 2 * M_PI);
		cr->close_path();
		cr->fill_preserve();
	}
	// Thingspeak field 2
	if (fieldEnable[1] == 1 && field2.size() > 1)
	{
		incVal = gridHorizLeft;
		double currPoint;
		double prevPoint = gridVertBottom - ((field2[0] - xAxisOffset) * vertSpacing);

		cr->begin_new_path();
		cr->set_source_rgb(0.7, 0.6, 0.0);

		for (int i = 1; i < field2.size(); ++i)
		{
			if (field2[i] > 19) // values must be at least 20
			{
				currPoint = gridVertBottom - ((field2[i] - xAxisOffset) * vertSpacing);
				
				cr->move_to(incVal, prevPoint);
				cr->arc(incVal, prevPoint, pointRadius, 0.0, 2 * M_PI);
				cr->close_path();
				cr->fill_preserve();
				cr->line_to((incVal + horizSpacingField2), currPoint);
				cr->stroke();

				incVal += horizSpacingField2;
				prevPoint = currPoint;
			}
		}

		cr->set_source_rgb(0.7, 0.6, 0.0);
		cr->arc(incVal, prevPoint, (pointRadius + 1.0), 0.0, 2 * M_PI);
		cr->close_path();
		cr->fill_preserve();
	}
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

bool MyArea::toggleField(int fieldNum)
{
	if (fieldNum >= 0 && fieldNum < 8)
	{
		if (fieldEnable[fieldNum] == 0)
			fieldEnable[fieldNum] = 1;
		else
			fieldEnable[fieldNum] = 0;

		return true;
	}
	else
	{
		return false;
	}
}

void MyArea::setNumOfGridCols(double numOfCols)
{
	numCols = numOfCols;
	this->queue_draw();
}

void MyArea::getFieldData(ThingSpeak tsObj)
{
	field1.clear();
	field2.clear();

	// Iterate through each item in ThingSpeak data bundle and store in local vectors
	for (auto& mapItem : tsObj.getFieldResults())
	{
		for (auto it = mapItem.cbegin(); it != mapItem.cend(); ++it)
		{
			// ThingSpeak field 1
			try
			{
				if (fieldEnable[0] == 1 && it->first == "field1")
					field1.push_back(std::stof(it->second));
			}
			catch (const std::invalid_argument& ia)
			{
				std::cout << "Field 1 - Invalid Argument: " << ia.what() << '\n';
			}
			// ThingSpeak field 2
			try
			{
				if (fieldEnable[1] == 1 && it->first == "field2")
					field2.push_back(std::stof(it->second));
			}
			catch (const std::invalid_argument& ia)
			{
				std::cout << "Field 2 - Invalid Argument: " << ia.what() << '\n';
			}
		}
	}
}

void MyArea::getTempRange()
{
	highestTemp = -200.0;
	lowestTemp = 200.0;

	// search first vector for temperature extremes
	if (field1.size() != 0)
		for (auto& tempPoint : field1)
		{
			if (tempPoint > highestTemp) highestTemp = tempPoint;
			if (tempPoint < lowestTemp) lowestTemp = tempPoint;
		}

	// search second vector for temperature extremes
	if (field2.size() != 0)
		for (auto& tempPoint : field2)
		{
			if (tempPoint > highestTemp) highestTemp = tempPoint;
			if (tempPoint < lowestTemp) lowestTemp = tempPoint;
		}
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