#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/separator.h>

class drawingArea : public Gtk::DrawingArea {
public:
	drawingArea();
	~drawingArea() override;
protected:
  	void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
  	void draw_text(const Cairo::RefPtr<Cairo::Context>& cr, Glib::ustring s, int x, int y);
};

class mainWindow : public Gtk::Window {
public:
	mainWindow();
	~mainWindow() override;

protected:
	void on_button_clicked(const Glib::ustring& data);

	Gtk::Box mainBox, rightBox;
	Gtk::Separator mainSep, rightSep1, rightSep2, rightSep3, rightSep4;
	drawingArea mainArea;
	Gtk::Box eps1Box, eps2Box, x0Box, x1Box, lBox, rBox;
	Gtk::Entry eps1Entry, eps2Entry, x0Entry, x1Entry, lEntry, rEntry;
	Gtk::Label eps1Label, eps2Label, x0Label, x1Label, lLabel, rLabel;
	Gtk::Button drawButton, nextButton;
};

#endif