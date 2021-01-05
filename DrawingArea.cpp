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
	// determine graph constants
	const int tempRange = 100;
	const float xAxisOffset = 20.0;

	if ((numCols - 8.0) < 0.01)
		amntOfPoints = 24;
	else if ((numCols - 9.0) < 0.01)
		amntOfPoints = 168;
	else
		amntOfPoints = 5040;

	auto gridHorizLeft = width / numCols;
	auto gridHorizRight = width - (width / numCols);
	auto gridVertTop = height / numRows;
	auto gridVertBottom = height - (height / numRows);

	auto vertSpacing = (gridVertBottom - gridVertTop) / tempRange;
	auto horizSpacing = (gridHorizRight - gridHorizLeft) / amntOfPoints;

	cr->set_line_width(3);

	/*
		Plotting logic for graph
	*/
	// Thingspeak field 1
	cr->set_source_rgb(1.0, 0.0, 0.0);
	auto incVal = gridHorizLeft;
	double currPoint;
	double prevPoint = field1[0];
	for (int i = 1; i < field1.size(); ++i)
	{
		if (field1[i] != -1.0)
		{
			currPoint = gridVertBottom - ((field1[i] - xAxisOffset) * vertSpacing);

			cr->move_to(incVal, prevPoint);
			cr->line_to((incVal + horizSpacing), currPoint);
			cr->stroke();

			incVal += horizSpacing;
			prevPoint = currPoint;
		}
	}	
	/*
	// Thingspeak field 2
	incVal = gridHorizLeft;
	for (int i = 0; i < (field2.size() - 1); ++i)
	{
		if (field2[i] != -1.0 && field2[i + 1] != -1.0)
		{
			valOne = gridVertBottom - ((field2[i] - xAxisOffset) * vertSpacing);
			valTwo = gridVertBottom - ((field2[i + 1] - xAxisOffset) * vertSpacing);

			cr->move_to(incVal, valOne);
			cr->line_to((incVal + horizSpacing), valTwo);
			cr->stroke();

			incVal += horizSpacing;
		}
	}
	// Thingspeak field 3
	incVal = gridHorizLeft;
	for (int i = 0; i < (field3.size() - 1); ++i)
	{
		if (field3[i] != -1.0 && field3[i + 1] != -1.0)
		{
			valOne = gridVertBottom - ((field3[i] - xAxisOffset) * vertSpacing);
			valTwo = gridVertBottom - ((field3[i + 1] - xAxisOffset) * vertSpacing);

			cr->move_to(incVal, valOne);
			cr->line_to((incVal + horizSpacing), valTwo);
			cr->stroke();

			incVal += horizSpacing;
		}
	}
	// Thingspeak field 4
	incVal = gridHorizLeft;
	for (int i = 0; i < (field4.size() - 1); ++i)
	{
		if (field4[i] != -1.0 && field4[i + 1] != -1.0)
		{
			valOne = gridVertBottom - ((field4[i] - xAxisOffset) * vertSpacing);
			valTwo = gridVertBottom - ((field4[i + 1] - xAxisOffset) * vertSpacing);

			cr->move_to(incVal, valOne);
			cr->line_to((incVal + horizSpacing), valTwo);
			cr->stroke();

			incVal += horizSpacing;
		}
	}
	// Thingspeak field 5
	incVal = gridHorizLeft;
	for (int i = 0; i < (field5.size() - 1); ++i)
	{
		if (field5[i] != -1.0 && field5[i + 1] != -1.0)
		{
			valOne = gridVertBottom - ((field5[i] - xAxisOffset) * vertSpacing);
			valTwo = gridVertBottom - ((field5[i + 1] - xAxisOffset) * vertSpacing);

			cr->move_to(incVal, valOne);
			cr->line_to((incVal + horizSpacing), valTwo);
			cr->stroke();

			incVal += horizSpacing;
		}
	}
	// Thingspeak field 6
	incVal = gridHorizLeft;
	for (int i = 0; i < (field6.size() - 1); ++i)
	{
		if (field6[i] != -1.0 && field6[i + 1] != -1.0)
		{
			valOne = gridVertBottom - ((field6[i] - xAxisOffset) * vertSpacing);
			valTwo = gridVertBottom - ((field6[i + 1] - xAxisOffset) * vertSpacing);

			cr->move_to(incVal, valOne);
			cr->line_to((incVal + horizSpacing), valTwo);
			cr->stroke();

			incVal += horizSpacing;
		}
	}
	// Thingspeak field 7
	incVal = gridHorizLeft;
	for (int i = 0; i < (field7.size() - 1); ++i)
	{
		if (field7[i] != -1.0 && field7[i + 1] != -1.0)
		{
			valOne = gridVertBottom - ((field7[i] - xAxisOffset) * vertSpacing);
			valTwo = gridVertBottom - ((field7[i + 1] - xAxisOffset) * vertSpacing);

			cr->move_to(incVal, valOne);
			cr->line_to((incVal + horizSpacing), valTwo);
			cr->stroke();

			incVal += horizSpacing;
		}
	}
	// Thingspeak field 8
	incVal = gridHorizLeft;
	for (int i = 0; i < (field8.size() - 1); ++i)
	{
		if (field8[i] != -1.0 && field8[i + 1] != -1.0)
		{
			valOne = gridVertBottom - ((field8[i] - xAxisOffset) * vertSpacing);
			valTwo = gridVertBottom - ((field8[i + 1] - xAxisOffset) * vertSpacing);

			cr->move_to(incVal, valOne);
			cr->line_to((incVal + horizSpacing), valTwo);
			cr->stroke();

			incVal += horizSpacing;
		}
	}
	*/
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

void MyArea::getFieldData(ThingSpeak tsObj)
{
	field1.clear();

	// Iterate through each item in ThingSpeak data bundle and store in local vectors
	for (auto& mapItem : tsObj.getFieldResults())
	{
		for (auto it = mapItem.cbegin(); it != mapItem.cend(); ++it)
		{
			// ThingSpeak field 1
			try
			{
				if (it->first == "field1") field1.push_back(std::stof(it->second));
			}
			catch (const std::invalid_argument& ia)
			{
				field1.push_back(-1.0);
				std::cout << "Field 1 - Invalid Argument: " << ia.what() << '\n';
			}
			// ThingSpeak field 2
			try
			{
				if (it->first == "field2") field2.push_back(std::stof(it->second));
			}
			catch (const std::invalid_argument& ia)
			{
				field2.push_back(-1.0);
				std::cout << "Field 2 - Invalid Argument: " << ia.what() << '\n';
			}
			// ThingSpeak field 3
			try
			{
				if (it->first == "field3") field3.push_back(std::stof(it->second));
			}
			catch (const std::invalid_argument& ia)
			{
				field3.push_back(-1.0);
				std::cout << "Field 3 - Invalid Argument: " << ia.what() << '\n';
			}
			// ThingSpeak field 4
			try
			{
				if (it->first == "field4") field4.push_back(std::stof(it->second));
			}
			catch (const std::invalid_argument& ia)
			{
				field4.push_back(-1.0);
				std::cout << "Field 4 - Invalid Argument: " << ia.what() << '\n';
			}
			// ThingSpeak field 5
			try 
			{
				if (it->first == "field5") field5.push_back(std::stof(it->second));
			}
			catch (const std::invalid_argument& ia)
			{
				field5.push_back(-1.0);
				std::cout << "Field 5 - Invalid Argument: " << ia.what() << '\n';
			}
			// ThingSpeak field 6
			try 
			{
				if (it->first == "field6") field6.push_back(std::stof(it->second));
			}
			catch (const std::invalid_argument& ia)
			{
				field6.push_back(-1.0);
				std::cout << "Field 6 - Invalid Argument: " << ia.what() << '\n';
			}
			// ThingSpeak field 7
			try 
			{
				if (it->first == "field7") field7.push_back(std::stof(it->second));
			}
			catch (const std::invalid_argument& ia)
			{
				field7.push_back(-1.0);
				std::cout << "Field 7 - Invalid Argument: " << ia.what() << '\n';
			}
			// ThingSpeak field 8
			try 
			{
				if (it->first == "field8") field8.push_back(std::stof(it->second));
			}
			catch (const std::invalid_argument& ia)
			{
				field8.push_back(-1.0);
				std::cout << "Field 8 - Invalid Argument: " << ia.what() << '\n';
			}
		}
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