#include "MainWindow.h"
#include <iostream>

MainWindow::MainWindow()
{
	// Initialize screen dimensions
	screenHeight = 480;
	screenWidth = 640;

	// Initialize the main window
	this->set_title("Homepage");
	this->set_border_width(10);
	this->set_size_request(screenWidth, screenHeight);
	this->maximize();
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