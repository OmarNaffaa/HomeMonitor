#include <iostream>
#include <chrono>
#include <mutex>

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
void configButtons(Gtk::Button (&layoutBtn)[7], MyArea* dArea);

// Signal Handlers
void onConfig1(MyArea* dArea); // 24 hrs
void onConfig2(MyArea* dArea); // 7 days
void onBtn0(MyArea* dArea);
void onBtn1(MyArea* dArea);
void onBtn2(MyArea* dArea);
void onBtn3(MyArea* dArea);
void onBtn4(MyArea* dArea);
void onBtn5(MyArea* dArea);
void onBtn6(MyArea* dArea);

// Threading Functions
std::mutex pollingMutex;
void asyncPolling(MyArea *dArea, int delayInMinutes);

// Utiltiy Functions
Glib::ustring setCss();

/* 
	Thingspeak polling object
*/
char tsChannel[] = "544573";
char tsReadKey[] = "BAY5Y9HPFP6V3C6G";
char tsNumOfRequests_Daily[] = "24";
char tsNumOfRequests_Weekly[] = "168";

ThingSpeak *tsPoller = new ThingSpeak(tsChannel, tsReadKey, tsNumOfRequests_Daily);

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
	configButtons(btn, &mArea);

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

	// Pack menu and main container into 1 container
	auto windowContainer = Gtk::VBox();
	windowContainer.pack_start(menuBar, Gtk::PACK_SHRINK, 0);
	windowContainer.pack_start(mainContainer, Gtk::PACK_EXPAND_WIDGET, 0);

	// Create scroller and add main container
	auto windowScroller = Gtk::ScrolledWindow();
	windowScroller.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	windowScroller.add(windowContainer);

	// Create thread to poll ThingSpeak asynchronously
	asyncPolling(&mArea, 10);

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
void configButtons(Gtk::Button(&layoutBtn)[7], MyArea* dArea)
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

	layoutBtn[0].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn0), dArea));
	layoutBtn[1].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn1), dArea));
	layoutBtn[2].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn2), dArea));
	layoutBtn[3].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn3), dArea));
	layoutBtn[4].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn4), dArea));
	layoutBtn[5].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn5), dArea));
	layoutBtn[6].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&onBtn6), dArea));
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

void onBtn2(MyArea* dArea)
{
	// toggle thingspeak field 3
	dArea->toggleField(2);

	// update chart
	pollingMutex.lock();

	tsPoller->getChannelData();
	dArea->getFieldData(*tsPoller);
	dArea->queue_draw();

	pollingMutex.unlock();
}

void onBtn3(MyArea* dArea)
{
	// toggle thingspeak field 4
	dArea->toggleField(3);

	// update chart
	pollingMutex.lock();

	tsPoller->getChannelData();
	dArea->getFieldData(*tsPoller);
	dArea->queue_draw();

	pollingMutex.unlock();
}

void onBtn4(MyArea* dArea)
{
	// toggle thingspeak field 5
	dArea->toggleField(4);

	// update chart
	pollingMutex.lock();

	tsPoller->getChannelData();
	dArea->getFieldData(*tsPoller);
	dArea->queue_draw();

	pollingMutex.unlock();
}

void onBtn5(MyArea* dArea)
{
	// toggle thingspeak field 6
	dArea->toggleField(5);

	// update chart
	pollingMutex.lock();

	tsPoller->getChannelData();
	dArea->getFieldData(*tsPoller);
	dArea->queue_draw();

	pollingMutex.unlock();
}

void onBtn6(MyArea* dArea)
{
	// toggle thingspeak field 7
	dArea->toggleField(6);

	// update chart
	pollingMutex.lock();

	tsPoller->getChannelData();
	dArea->getFieldData(*tsPoller);
	dArea->queue_draw();

	pollingMutex.unlock();
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

	// option buttons
	retVal += "button label{color:white;font-family:sans-serif;font-size:20px;} ";
	retVal += "#optionBtns{border-radius:12px;background:#000069;margin:10px;padding:5px;box-shadow: 0 4px 8px 0 rgba(0,0,0,0.5),0 6px 20px 0 rgba(0,0,0,0.5);} ";
	retVal += "#optionBtns:hover{background:#3D3DAF;} ";

	// Miscellaneous widget items
	retVal += "#btnRowBox{background-color:#E5E5E5;} ";
	retVal += "#mainMenuBar{background:#ADD8E6;} ";

	return retVal;
}