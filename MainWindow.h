#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm/window.h>
#include <gdkmm/screen.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/cssprovider.h>

class MainWindow : public Gtk::Window
{
public:
	// Constructor and Deconstructor
	MainWindow();
	virtual ~MainWindow();

	// Functions
	int getScreenWidth();
	int getScreenHeight();
	void determineScreenDimensions();


protected:
	int screenWidth;
	int screenHeight;
};

#endif // MAINWINDOW_H