#include "graphBuilder.h"
#include <algorithm>
#include <utility>
#include <cmath>
#include <iostream>
#include <string>

double l = 1, r = -1, Oxpos, eps1 = -1, eps2 = -1;

std::vector<double> vec_x;

double integral(double f(double, double), double x, double a, double b, int n) {
	double ans = 0;

	double d = (b - a) / n;
	double cur = a;
	for (int i = 0; i < n; i++) {
		ans += d * (f(cur + d / 2, x));
		cur += d;
	}
	return ans;
}

double integral(double f(double, double), double x, double a, double b) {
	double prev_ans = integral(f, x, a, b, 1);
	double ans = integral(f, x, a, b, 2);
	int cnt = 2;
	while (fabs(ans - prev_ans) >= eps1) {
		cnt <<= 1;
		prev_ans = ans;
		ans = integral(f, x, a, b, cnt);
	}
	return ans;
}

double g(double y, double x) {
	return exp(-cos(y) * 0.5 * x * x) * sqrt(sin(y));
}

double f(double x) {
	return -1 + x * x * integral(g, x, 0, M_PI / 2.0);
}

drawingArea::drawingArea() {
	set_content_width(800);
	set_content_height(800);
  	set_draw_func(sigc::mem_fun(*this, &drawingArea::on_draw));
}

drawingArea::~drawingArea() {}

void drawingArea::draw_text(const Cairo::RefPtr<Cairo::Context>& cr, Glib::ustring s, int x, int y) {
	Pango::FontDescription font;

	font.set_family("Monospace");
	font.set_weight(Pango::Weight::BOLD);

	auto layout = create_pango_layout(s);

	layout->set_font_description(font);

	int text_width;
	int text_height;

	layout->get_pixel_size(text_width, text_height);

  	cr->move_to(x, y);

  	layout->show_in_cairo_context(cr);
}

std::string double_to_string(double x) {
	std::string s = std::to_string(x);
	int idx = -1;
	for (int i = 0; i < (int)s.size(); i++) {
		if (s[i] == '.') {
			idx = i;
			break;
		}
	}

	return s.substr(0, idx + 3);
}



void drawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
	int offset = 0.05 * std::min(width, height);

	{ // Bounding box
		cr->save();

		cr->set_line_width(5);
		cr->set_line_cap(Cairo::Context::LineCap::ROUND);

		cr->move_to(offset, offset);
		cr->line_to(offset, height - offset);
		cr->line_to(width - offset, height - offset);
		cr->line_to(width - offset, offset);
		cr->line_to(offset, offset);

		cr->stroke();

		cr->restore();
	}

	double minY, maxY;

	if (r > l) { // Drawing graph
		double d = (r - l) / (width - 2 * offset);

		double cur = l;
		std::vector<std::pair<int, double>> vec;

		for (int i = offset; i <= width - offset; i++) {
			vec.emplace_back(i, f(cur));
			cur += d;
		}

		minY = vec.front().second, maxY = vec.front().second;
		for (auto it : vec) {
			minY = std::min(minY, it.second);
			maxY = std::max(maxY, it.second);
		}

		draw_text(cr, double_to_string(maxY), 0.1 * offset, offset - 0.2 * offset);
		draw_text(cr, double_to_string(minY), 0.1 * offset, height - offset - 0.1 * offset);

		if (maxY >= 0 && minY <= 0) {
			Oxpos = height - (((0 - minY) / (maxY - minY)) * 
													(height - 2 * offset - 6) + offset + 3);
			draw_text(cr, "0", 0.5 * offset, Oxpos - 0.2 * offset);
			cr->save();

			cr->set_line_width(3);

			cr->move_to(offset, Oxpos);
			cr->line_to(width - offset, Oxpos);

			cr->stroke();

			cr->restore();
		}

		cr->save();

		cr->set_line_width(3);
		cr->set_source_rgb(1, 0.0, 0.0);
		cr->set_line_cap(Cairo::Context::LineCap::ROUND);

		cr->move_to(offset, height - (((vec.front().second - minY) / (maxY - minY)) * 
													(height - 2 * offset - 6) + offset + 3));

		for (int i = offset + 1; i <= width - offset; i++) {
			cr->line_to(i, height - (((vec[i - offset].second - minY) / (maxY - minY)) * 
													(height - 2 * offset - 6) + offset + 3));
		}

		cr->stroke();

		cr->restore();
	}

	if (r > l) { // Drawing grid
		double d = (r - l) / (width - 2 * offset);
		double cur = l;
		for (int i = offset; i <= width - offset - ((width - 2 * offset) / 10); i += (width - 2 * offset) / 10) {
			cr->save();

			cr->set_line_width(5);
			cr->set_line_cap(Cairo::Context::LineCap::ROUND);

			cr->move_to(i, height - offset + 0.2 * offset);
			cr->line_to(i, height - offset - 0.2 * offset);

			cr->stroke();

			cr->restore();


			cr->save();

			cr->set_line_width(0.5);

			cr->move_to(i, height - offset);
			cr->line_to(i, offset);

			cr->stroke();

			cr->restore();

			draw_text(cr, double_to_string(cur), i - ((width - 2 * offset) / 35), height - offset + 0.3 * offset);
			cur += d * ((width - 2 * offset) / 10);
		}
	}

	{
		cr->save();
		cr->set_line_width(1);
		cr->set_source_rgb(0.0, 1.0, 0.0);
		for (int i = 1; i < (int)vec_x.size(); i++) {
			cr->move_to(((vec_x[i] - l) / (r - l)) * (width - 2 * offset) + offset, 
				height - ((f(vec_x[i]) - minY) / (maxY - minY) * (height - 2 * offset) + offset));

			cr->line_to(((vec_x[0] - l) / (r - l)) * (width - 2 * offset) + offset, 
				height - ((f(vec_x[0]) - minY) / (maxY - minY) * (height - 2 * offset) + offset));
			cr->stroke();
		}

		for (int i = 2; i < (int)vec_x.size(); i++) {
			cr->move_to(((vec_x[i] - l) / (r - l)) * (width - 2 * offset) + offset, Oxpos);

			cr->line_to(((vec_x[i] - l) / (r - l)) * (width - 2 * offset) + offset, 
				height - ((f(vec_x[i]) - minY) / (maxY - minY) * (height - 2 * offset) + offset));
			cr->stroke();
		}

		cr->restore();
	}
}

mainWindow::mainWindow() : eps1Label("eps1"), eps2Label("eps2"), x0Label("x0"), x1Label("x1"), 
		lLabel("l"), rLabel("r"), rightBox(Gtk::Orientation::VERTICAL), nextButton("Next step"), 
		drawButton("Draw") {

	set_title("Graph Builder");

	lBox.append(lLabel);
	lLabel.set_margin(5);
	lBox.append(lEntry);
	lEntry.set_hexpand();

	rBox.append(rLabel);
	rLabel.set_margin(5);
	rBox.append(rEntry);
	rEntry.set_hexpand();

	eps1Box.append(eps1Label);
	eps1Label.set_margin(5);
	eps1Box.append(eps1Entry);
	eps1Entry.set_hexpand();

	eps2Box.append(eps2Label);
	eps2Label.set_margin(5);
	eps2Box.append(eps2Entry);	
	eps2Entry.set_hexpand();

	x0Box.append(x0Label);
	x0Label.set_margin(5);
	x0Box.append(x0Entry);
	x0Entry.set_hexpand();

	x1Box.append(x1Label);
	x1Label.set_margin(5);
	x1Box.append(x1Entry);
	x1Entry.set_hexpand();

	rightBox.append(lBox);
	rightBox.append(rBox);
	rightBox.append(rightSep3);
	rightSep3.set_margin(3);
	rightBox.append(drawButton);
	drawButton.signal_clicked().connect(sigc::bind(
		sigc::mem_fun(*this, &mainWindow::on_button_clicked), "draw"));
	drawButton.set_margin(3);
	rightBox.append(rightSep4);
	rightSep4.set_margin(3);
	rightBox.append(eps1Box);
	rightBox.append(eps2Box);
	rightBox.append(x0Box);
	rightBox.append(x1Box);

	lBox.set_margin(3);
	rBox.set_margin(3);
	eps1Box.set_margin(3);
	eps2Box.set_margin(3);
	x0Box.set_margin(3);
	x1Box.set_margin(3);

	rightBox.append(rightSep1);
	rightSep1.set_margin(3);

	rightBox.append(nextButton);
	nextButton.signal_clicked().connect(sigc::bind(
		sigc::mem_fun(*this, &mainWindow::on_button_clicked), "next"));

	nextButton.set_margin(3);

	rightBox.append(rightSep2);
	rightSep2.set_margin(3);

	rightBox.set_margin(10);

	mainBox.append(mainArea);
	mainArea.set_expand();
	mainBox.append(mainSep);
	mainBox.append(rightBox);


	set_child(mainBox);
}

mainWindow::~mainWindow() {}

void mainWindow::on_button_clicked(const Glib::ustring& data) {
	std::string s1 = mainWindow::eps1Entry.get_text();
	if (s1.size() == 0) {
		mainWindow::eps1Entry.set_text("0.1");
	}
	eps1 = std::stod(mainWindow::eps1Entry.get_text());
	std::string s2 = mainWindow::eps2Entry.get_text();
	if (s2.size() == 0) {
		mainWindow::eps2Entry.set_text("0.1");
	}
	eps2 = std::stod(mainWindow::eps2Entry.get_text());
	if (data == "draw") {
		std::string sl = mainWindow::lEntry.get_text();
		if (sl.size() == 0) {
			mainWindow::lEntry.set_text("0.0");
		}
		l = std::stod(mainWindow::lEntry.get_text());

		std::string sr = mainWindow::rEntry.get_text();
		if (sr.size() == 0) {
			mainWindow::rEntry.set_text("0.0");
		}
		r = std::stod(mainWindow::rEntry.get_text());
		mainWindow::mainArea.queue_draw();
	}
	if (data == "next") {
		if (vec_x.empty()) {
			std::string s0 = mainWindow::x0Entry.get_text();
			if (s0.size() == 0) {
				mainWindow::x0Entry.set_text("0");
			}
			double x0 = std::stod(mainWindow::x0Entry.get_text());

			std::string s1 = mainWindow::x1Entry.get_text();
			if (s1.size() == 0) {
				mainWindow::x1Entry.set_text("0");
			}
			double x1 = std::stod(mainWindow::x1Entry.get_text());
			vec_x.push_back(x0);
			vec_x.push_back(x1);
		} else {
			double x_i = vec_x.back(), x0 = vec_x[0];
			vec_x.push_back(x_i - (f(x_i) * (x_i - x0)) / (f(x_i) - f(x0)));
			if (fabs(vec_x.back() - x_i) < eps2) {
				std::cout << eps2 << "\n";
				std::string text;
				text += "Root found";
				Gtk::Label label(text);
				rightBox.append(label);
			} else {
				std::string text;
				text += "x = ";
				text += std::to_string(vec_x.back());
				text += ", f(x) = ";
				text += std::to_string(f(vec_x.back()));
				Gtk::Label label(text);
				rightBox.append(label);
			}
		}
		mainWindow::mainArea.queue_draw();
	}
}
