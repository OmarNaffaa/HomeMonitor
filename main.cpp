#include <iostream>

#include <gtkmm/Application.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menuitem.h>

#include "MainWindow.h"
#include "DrawingArea.h"

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

void onConfig1(MyArea* dArea);
void onConfig2(MyArea* dArea);
void onConfig3(MyArea* dArea);

/*
	!!! Main !!!
*/
int main(int argc, char* argv[])
{
	auto app = Gtk::Application::create(argc, argv);
	
	auto mainContainer = Gtk::Box();
	MainWindow homepage;

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

	// Load color settings from CSS file
	auto cssProvider = Gtk::CssProvider::create();
	cssProvider->load_from_path("CSS\\mainwindow.css");
	auto styleContext = Gtk::StyleContext::create();
	auto screen = Gdk::Screen::get_default();
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
}

void onConfig2(MyArea* dArea)
{
	// configuration to display 7 days of data
	dArea->setNumOfGridCols(9.0);
}

void onConfig3(MyArea* dArea)
{
	// configuration to display 30 days of data
	dArea->setNumOfGridCols(12.0);
}