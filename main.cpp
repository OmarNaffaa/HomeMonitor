#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // hide command line

#include <iostream>
#include <chrono>
#include <mutex>

#include <gtkmm/Application.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/messagedialog.h>
#include <glibmm/dispatcher.h>

#include "MainWindow.h"
#include "DrawingArea.h"
#include "ThingSpeak.h"

// Signal Handlers
void onConfig1(MyArea* dArea); // 24 hrs
void onConfig2(MyArea* dArea); // 7 days
void onBtn0(MyArea* dArea);
void onBtn1(MyArea* dArea);
void onBtn2(MainWindow* window);
void onBtn3(Gtk::Entry* entryBox);
void onBtn4(Gtk::Entry* entryBox);

// Threading Functions
std::mutex pollingMutex;
void asyncPolling(MyArea *dArea, int delayInMinutes);

// Utiltiy Functions
Glib::ustring setCss();

/* 
	Thingspeak Channel Information
*/
string tsChannel = "1277292";
string tsReadKey = "I4BV5Q70NNDWH0SP";
string tsNumOfRequests_Daily = "48";
string tsNumOfRequests_Weekly = "336";

/*
	Global Variables
*/
auto tsPoller = new ThingSpeak(tsChannel, tsReadKey, tsNumOfRequests_Daily);
MyArea* dArea_ref; // used in "onBtn3" and "onBtn4" functions

/*
	!!! Main !!!
*/
int main(int argc, char* argv[])
{
	auto app = Gtk::Application::create(argc, argv);

	auto mainContainer = Gtk::Box();
	MainWindow* homepage = new MainWindow();
	homepage->set_title("Home Monitor");

	// Create drawing area and add to container
	MyArea mArea(12.0, 8.0);
	dArea_ref = &mArea;
	mainContainer.pack_start(mArea, Gtk::PACK_EXPAND_WIDGET);

	// Create labels and entry boxes for main window
	Gtk::Label toggleLbl("Click to toggle display");
	toggleLbl.set_name("topColItem");
	Gtk::Label infoLbl("Additional Info");
	infoLbl.set_name("columnLbl");
	Gtk::Label modifyTsKeyAndChannel("Modify Channel or Key");
	modifyTsKeyAndChannel.set_name("columnLbl");

	Gtk::Entry* tsChannelEntry = new Gtk::Entry();
	tsChannelEntry->set_placeholder_text("Channel Number");
	tsChannelEntry->set_name("entryBox");
	Gtk::Entry* tsKeyEntry = new Gtk::Entry();
	tsKeyEntry->set_placeholder_text("Read Key");
	tsKeyEntry->set_name("entryBox");

	// Create buttons and group them in a container
	auto mainWindowColumn = Gtk::VBox();
	Gtk::Button btn[5];

	mainWindowColumn.set_name("btnRowBox");
	for (auto& btnItem : btn)
		btnItem.set_name("optionBtns");

	btn[0].add_label("My Bedroom");
	btn[0].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn0), &mArea));
	btn[1].add_label("Downstairs Bedroom");
	btn[1].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn1), &mArea));
	btn[2].add_label("Information");
	btn[2].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn2), homepage));
	btn[3].add_label("Set Channel");
	btn[3].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn3), tsChannelEntry));
	btn[4].add_label("Set Key");
	btn[4].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn4), tsKeyEntry));

	// Pack column widgets into main column container
	mainWindowColumn.pack_start(toggleLbl, Gtk::PACK_SHRINK, 0);
	mainWindowColumn.pack_start(btn[0], Gtk::PACK_SHRINK, 0);
	mainWindowColumn.pack_start(btn[1], Gtk::PACK_SHRINK, 0);
	mainWindowColumn.pack_start(modifyTsKeyAndChannel, Gtk::PACK_SHRINK, 0);
	mainWindowColumn.pack_start(*tsChannelEntry, Gtk::PACK_SHRINK, 0);
	mainWindowColumn.pack_start(btn[3], Gtk::PACK_SHRINK, 0);
	mainWindowColumn.pack_start(*tsKeyEntry, Gtk::PACK_SHRINK, 0);
	mainWindowColumn.pack_start(btn[4], Gtk::PACK_SHRINK, 0);
	mainWindowColumn.pack_end(btn[2], Gtk::PACK_SHRINK, 0);
	mainWindowColumn.pack_end(infoLbl, Gtk::PACK_SHRINK, 0);

	mainContainer.add(mainWindowColumn);

	// Create menubar
	Gtk::MenuBar menuBar;
	menuBar.set_name("mainMenuBar");
	Gtk::MenuItem graphSettings;
	graphSettings.set_label("Graph Timespan Settings (click to expand)");
	menuBar.append(graphSettings);
	Gtk::Menu subMenuItems;
	graphSettings.set_submenu(subMenuItems);

	Gtk::MenuItem config1("Past 24 hours");
	subMenuItems.append(config1);
	config1.signal_activate().connect(sigc::bind(sigc::ptr_fun(onConfig1), &mArea));

	Gtk::MenuItem config2("Past week");
	subMenuItems.append(config2);
	config2.signal_activate().connect(sigc::bind(sigc::ptr_fun(onConfig2), &mArea));

	// Pack menu and main container into 1 container
	auto windowContainer = Gtk::VBox();
	windowContainer.pack_start(menuBar, Gtk::PACK_SHRINK, 0);
	windowContainer.pack_start(mainContainer, Gtk::PACK_EXPAND_WIDGET, 0);

	// Create scroller and add main container
	auto windowScroller = Gtk::ScrolledWindow();
	windowScroller.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	windowScroller.add(windowContainer);

	// Create thread to poll ThingSpeak asynchronously
	asyncPolling(&mArea, 1);

	// Load color settings from CSS file
	auto cssProvider = Gtk::CssProvider::create();
	auto styleContext = Gtk::StyleContext::create();
	auto screen = Gdk::Screen::get_default();
	cssProvider->load_from_data(setCss());
	styleContext->add_provider_for_screen(screen, cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	// Add main container to window
	homepage->add(windowScroller);
	homepage->show_all();

	return app->run(*homepage);
}

/*
	Signal Handler Definitions
*/
void onConfig1(MyArea* dArea)
{
	// configuration to display 24 hours of data
	dArea->setNumOfGridCols(8.0);

	// replace object with new poller
	delete tsPoller;
	tsPoller = new ThingSpeak(tsChannel, tsReadKey, tsNumOfRequests_Daily);

	// update data
	pollingMutex.lock();

	tsPoller->getChannelData();
	dArea->getFieldData(*tsPoller);
	dArea->queue_draw();

	pollingMutex.unlock();
}

void onConfig2(MyArea* dArea)
{
	// configuration to display 7 days of data
	dArea->setNumOfGridCols(9.0);

	// replace object with new poller
	delete tsPoller;
	tsPoller = new ThingSpeak(tsChannel, tsReadKey, tsNumOfRequests_Weekly);

	// update data
	pollingMutex.lock();

	tsPoller->getChannelData();
	dArea->getFieldData(*tsPoller);
	dArea->queue_draw();

	pollingMutex.unlock();
}

void onBtn0(MyArea* dArea)
{
	// toggle thingspeak field 1
	dArea->toggleField(0);

	// update chart
	pollingMutex.lock();

	tsPoller->getChannelData();
	dArea->getFieldData(*tsPoller);
	dArea->queue_draw();

	pollingMutex.unlock();
}

void onBtn1(MyArea* dArea)
{
	// toggle thingspeak field 2
	dArea->toggleField(1);

	// update chart
	pollingMutex.lock();

	tsPoller->getChannelData();
	dArea->getFieldData(*tsPoller);
	dArea->queue_draw();

	pollingMutex.unlock();
}

void onBtn2(MainWindow* window)
{
	// Get most recent temperature data;
	string dialogText;
	string timestamp = tsPoller->getMostRecentTimestamp();
	string temp1 = tsPoller->getMostRecentTemp(1);
	string temp2 = tsPoller->getMostRecentTemp(2);

	if (temp1 != "Invalid Field" && temp1 != "")
	{
		dialogText += " \nMost Recent Bedroom Temperature: ";
		dialogText += temp1 + " degrees fahrenheit\n";
	}

	if (temp2 != "Invalid Field" && temp2 != "")
	{
		dialogText += " \nMost Recent Downstairs Temperature: ";
		dialogText += temp2 + " degrees fahrenheit\n";
	}

	if (timestamp != "")
	{
		dialogText += " \nTime of Last Update (Relative to server): ";
		dialogText += timestamp.substr(timestamp.find_first_of('T')+1, 8);
		dialogText += " \nDate of Last Update (Relative to server): ";
		dialogText += timestamp.substr(0, timestamp.find_first_of('T'));
	}

	// Display dialog with additional information
	std::unique_ptr<Gtk::MessageDialog> dialog;
	dialog.reset(new Gtk::MessageDialog(*window, "Additional Temperature Information"));

	dialog->set_name("dialogWindow");
	dialog->set_default_size(600, 300);
	dialog->set_secondary_text(dialogText);
	dialog->run();
}

void onBtn3(Gtk::Entry* entryBox)
{
	// Update ThingSpeak channel number
	auto entryTxt = entryBox->get_text();

	if (entryTxt != "")
	{
		tsChannel = entryTxt;

		// replace object with new poller
		delete tsPoller;
		tsPoller = new ThingSpeak(tsChannel, tsReadKey, tsNumOfRequests_Daily);

		// update data
		pollingMutex.lock();

		tsPoller->getChannelData();
		dArea_ref->getFieldData(*tsPoller);
		dArea_ref->queue_draw();

		pollingMutex.unlock();

		entryBox->set_text("");
	}
}

void onBtn4(Gtk::Entry* entryBox)
{
	// Update ThingSpeak read key - TODO
	string entryTxt = entryBox->get_text();

	if (entryTxt != "")
	{
		tsReadKey = entryTxt;

		// replace object with new poller
		delete tsPoller;
		tsPoller = new ThingSpeak(tsChannel, tsReadKey, tsNumOfRequests_Daily);

		// update data
		pollingMutex.lock();

		tsPoller->getChannelData();
		dArea_ref->getFieldData(*tsPoller);
		dArea_ref->queue_draw();

		pollingMutex.unlock();

		// clear dialog
		entryBox->set_text("");
	}
}

void asyncPolling(MyArea* dArea, int delayInMinutes)
{
	const unsigned int minsToMs = 60000;
	std::thread([dArea, delayInMinutes, minsToMs]()
		{
			while (true)
			{
				auto t = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayInMinutes*minsToMs);

				pollingMutex.lock();

				tsPoller->getChannelData();
				dArea->getFieldData(*tsPoller);
				dArea->queue_draw();

				pollingMutex.unlock();

				std::this_thread::sleep_until(t);
			}
		}
	).detach();
}

Glib::ustring setCss()
{
	// Creates string to represent window CSS styling
	Glib::ustring retVal = "";
	
	// title bar buttons
	retVal += "button.titlebutton{padding-top:5px;padding-bottom:5px;padding-left:10px;padding-right:10px;} ";
	retVal += "button.titlebutton.maximize:hover{background:#BFBFBF;} ";
	retVal += "button.titlebutton.minimize:hover{background:#BFBFBF;} ";
	retVal += "button.titlebutton.close:hover{background:#C90000;} ";

	// labels and entries
	retVal += "#topColItem{color:black;font-family:sans-serif;font-size:20px;margin-bottom:10px;padding:5px;}";
	retVal += "#columnLbl{color:black;font-family:sans-serif;font-size:20px;margin-top:50px;margin-bottom:10px;padding:5px;}";
	retVal += "#entryBox{color:black;font-family:sans-serif;font-size:20px;margin:5px;padding:5px;}";

	// option buttons
	retVal += "button label{color:white;font-family:sans-serif;font-size:20px;} ";
	retVal += "#optionBtns{border-radius:12px;background:#000069;margin:5px;padding-left:5px;padding-right:5px;padding-top:20px;padding-bottom:20px;} ";
	retVal += "#optionBtns:hover{background:#3D3DAF;} ";

	// dialogs
	retVal += "#dialogWindow label{color:black;font-family:sans-serif;font-size:20px;}";

	// Miscellaneous widget items
	retVal += "#btnRowBox{background-color:#E5E5E5;} ";
	retVal += "#mainMenuBar{background:#ADD8E6;} ";

	return retVal;
}