#include "graphBuilder.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[]) {
	auto app = Gtk::Application::create("org.graphBuilder");

	return app->make_window_and_run<mainWindow>(argc, argv);
}