#include <iostream>
#include <chrono>

#include <gtkmm/Application.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menuitem.h>
#include <glibmm/dispatcher.h>

#include "MainWindow.h"
#include "DrawingArea.h"
#include "ThingSpeak.h"

// Widget Definition Function Prototypes
void configButtons(Gtk::Button (&layoutBtn)[7]);

// Signal Handlers
void onBtn0(Glib::ustring btnPressed);
void onBtn1(Glib::ustring btnPressed);
void onBtn2(Glib::ustring btnPressed);
void onBtn3(Glib::ustring btnPressed);
void onBtn4(Glib::ustring btnPressed);
void onBtn5(Glib::ustring btnPressed);
void onBtn6(Glib::ustring btnPressed);
void onConfig1(MyArea *dArea); // 24 hrs
void onConfig2(MyArea *dArea); // 7 days
void onConfig3(MyArea *dArea); // 1 month

// General Functions
void asyncPolling(MyArea *dArea, int delayInMinutes);

// Utiltiy Functions
Glib::ustring setCss();

// Create global ThingSpeak polling object
char tsUrl[] = "https://api.thingspeak.com/channels/544573/feeds.json?api_key=BAY5Y9HPFP6V3C6G&results=24";
ThingSpeak tsPoller(tsUrl);

/*
	!!! Main !!!
*/
int main(int argc, char* argv[])
{
	auto app = Gtk::Application::create(argc, argv);

	auto mainContainer = Gtk::Box();
	MainWindow homepage("Dashboard");

	// Create drawing area and add to container
	MyArea mArea(12.0, 8.0);
	mainContainer.pack_start(mArea, Gtk::PACK_EXPAND_WIDGET);

	// Create buttons and group them in a container
	auto btnRowBox = Gtk::VBox();
	btnRowBox.set_name("btnRowBox");

	Gtk::Button btn[7];
	configButtons(btn);

	btnRowBox.pack_start(btn[0], Gtk::PACK_EXPAND_WIDGET, 0);
	btnRowBox.pack_start(btn[1], Gtk::PACK_EXPAND_WIDGET, 0);
	btnRowBox.pack_start(btn[2], Gtk::PACK_EXPAND_WIDGET, 0);
	btnRowBox.pack_start(btn[3], Gtk::PACK_EXPAND_WIDGET, 0);
	btnRowBox.pack_start(btn[4], Gtk::PACK_EXPAND_WIDGET, 0);
	btnRowBox.pack_start(btn[5], Gtk::PACK_EXPAND_WIDGET, 0);
	btnRowBox.pack_start(btn[6], Gtk::PACK_EXPAND_WIDGET, 0);

	mainContainer.add(btnRowBox);

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

	Gtk::MenuItem config3("Past month");
	subMenuItems.append(config3);
	config3.signal_activate().connect(sigc::bind(sigc::ptr_fun(onConfig3), &mArea));

	// Pack menu and main container into 1 container
	auto windowContainer = Gtk::VBox();
	windowContainer.pack_start(menuBar, Gtk::PACK_SHRINK, 0);
	windowContainer.pack_start(mainContainer, Gtk::PACK_EXPAND_WIDGET, 0);

	// Create scroller and add main container
	auto windowScroller = Gtk::ScrolledWindow();
	windowScroller.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	windowScroller.add(windowContainer);

	// Create thread to poll ThingSpeak asynchronously
	asyncPolling(&mArea, 15);

	// Load color settings from CSS file
	auto cssProvider = Gtk::CssProvider::create();
	auto styleContext = Gtk::StyleContext::create();
	auto screen = Gdk::Screen::get_default();
	cssProvider->load_from_data(setCss());
	styleContext->add_provider_for_screen(screen, cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	// Add main container to window
	homepage.add(windowScroller);
	homepage.show_all();

	return app->run(homepage);
}

/*
	Widget Configuration Function Definitions
*/
void configButtons(Gtk::Button(&layoutBtn)[7])
{
	for (int i = 0; i < 7; ++i)
		layoutBtn[i].set_name("optionBtns");

	layoutBtn[0].add_label("My Bedroom");
	layoutBtn[1].add_label("Upstairs Bedroom");
	layoutBtn[2].add_label("Downstairs Bedroom");
	layoutBtn[3].add_label("Master Bedroom");
	layoutBtn[4].add_label("Living Room");
	layoutBtn[5].add_label("Family Room");
	layoutBtn[6].add_label("Kitchen");

	layoutBtn[0].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn0), "0"));
	layoutBtn[1].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn1), "1"));
	layoutBtn[2].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn2), "2"));
	layoutBtn[3].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn3), "3"));
	layoutBtn[4].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn4), "4"));
	layoutBtn[5].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn5), "5"));
	layoutBtn[6].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn6), "6"));
}

/*
	Button Signal Handler Definitions
*/
void onBtn0(Glib::ustring btnPressed)
{
	return;
}

void onBtn1(Glib::ustring btnPressed)
{
	return;
}

void onBtn2(Glib::ustring btnPressed)
{
	return;
}

void onBtn3(Glib::ustring btnPressed)
{
	return;
}

void onBtn4(Glib::ustring btnPressed)
{
	return;
}

void onBtn5(Glib::ustring btnPressed)
{
	return;
}

void onBtn6(Glib::ustring btnPressed)
{
	return;
}

void onConfig1(MyArea* dArea)
{
	// configuration to display 24 hours of data
	dArea->setNumOfGridCols(8.0);

	// (24 hours with 1 hour resolution = 24 points)
	const char *dayUrl = "https://api.thingspeak.com/channels/544573/feeds.json?api_key=BAY5Y9HPFP6V3C6G&results=24";
	tsPoller.setUrl(dayUrl);
}

void onConfig2(MyArea* dArea)
{
	// configuration to display 7 days of data
	dArea->setNumOfGridCols(9.0);

	// (24 points per day for 7 days = 168 points)
	const char weeklyUrl[] = "https://api.thingspeak.com/channels/544573/feeds.json?api_key=BAY5Y9HPFP6V3C6G&results=168";
	tsPoller.setUrl(weeklyUrl);
}

void onConfig3(MyArea* dArea)
{
	// configuration to display 30 days of data
	dArea->setNumOfGridCols(12.0);

	// (168 points per day for 30 days = 5040 points)
	const char monthlyUrl[] = "https://api.thingspeak.com/channels/544573/feeds.json?api_key=BAY5Y9HPFP6V3C6G&results=5040";
	tsPoller.setUrl(monthlyUrl);
}

void asyncPolling(MyArea* dArea, int delayInMinutes)
{

	const unsigned int minsToMs = 60000;
	std::thread([dArea, delayInMinutes, minsToMs]()
		{
			while (true)
			{
				auto t = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayInMinutes*minsToMs);

				tsPoller.getChannelData();
				dArea->getFieldData(tsPoller);
				dArea->queue_draw();

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

	// option buttons
	retVal += "button label{color:white;font-family:sans-serif;font-size:20px;} ";
	retVal += "#optionBtns{border-radius:12px;background:#000069;margin:10px;padding:5px;box-shadow: 0 4px 8px 0 rgba(0,0,0,0.5),0 6px 20px 0 rgba(0,0,0,0.5);} ";
	retVal += "#optionBtns:hover{background:#3D3DAF;} ";

	// Miscellaneous widget items
	retVal += "#btnRowBox{background-color:#E5E5E5;} ";
	retVal += "#mainMenuBar{background:#ADD8E6;} ";

	return retVal;
}