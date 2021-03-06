#include "MainWindow.h"

MainWindow::MainWindow() : screenHeight(900), screenWidth(1200)
{
	// Initialize the main window
	this->set_title("");
	this->set_border_width(10);
	this->set_size_request(screenWidth, screenHeight);
}

MainWindow::MainWindow(titleString windowTitle) :
	screenHeight(900), screenWidth(900), screenTitle(windowTitle)
{
	// Initialize the main window
	this->set_title(screenTitle);
	this->set_border_width(10);
	this->set_size_request(screenWidth, screenHeight);
}

MainWindow::MainWindow(int dispWidth, int dispHeight) :
	screenHeight(dispHeight), screenWidth(dispWidth)
{
	// Initialize the main window
	this->set_title("Homepage");
	this->set_border_width(10);
	this->set_size_request(screenWidth, screenHeight);
}

MainWindow::MainWindow(int dispWidth, int dispHeight, titleString windowTitle) :
	screenHeight(dispHeight), screenWidth(dispWidth), screenTitle(windowTitle)
{
	// Initialize the main window
	this->set_title(screenTitle);
	this->set_border_width(10);
	this->set_size_request(screenWidth, screenHeight);
}

MainWindow::~MainWindow() {}

void MainWindow::determineScreenDimensions()
{
	auto screen = Gdk::Screen::get_default();

	this->screenWidth = screen->get_width();
	this->screenHeight = screen->get_height();
}

int MainWindow::getScreenWidth()
{
	return screenWidth;
}

int MainWindow::getScreenHeight()
{
	return screenHeight;
} 