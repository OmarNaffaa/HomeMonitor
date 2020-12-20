#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm/window.h>
#include <gdkmm/screen.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/cssprovider.h>

using titleString = Glib::ustring; // alias

class MainWindow : public Gtk::Window
{
public:
	// Constructor and Deconstructor
	MainWindow();
	MainWindow(titleString windowTitle);
	MainWindow(int dispWidth, int dispHeight);
	MainWindow(int dispWidth, int dispHeight, titleString windowTitle);
	virtual ~MainWindow();

	// Functions
	int getScreenWidth();
	int getScreenHeight();
	void determineScreenDimensions();


protected:
	int screenWidth;
	int screenHeight;
	titleString screenTitle;
};

#endif // MAINWINDOW_H