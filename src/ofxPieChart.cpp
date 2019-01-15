#include "ofxPieChart.h"


OfxPieChart::OfxPieChart(float radius, int center_x, int center_y, std::string title) {
	std::vector<float> percentages;
	std::vector<std::string> values;

	SetUpVariables(values, percentages, 0, radius, center_x, center_y, title);

	SetUpFonts();

	SetUpMousePosition();

	SetUpPanel(title);
}

OfxPieChart::OfxPieChart(std::map<std::string, float> value_to_percent, float radius, int center_x, int center_y, std::string title) {
	std::vector<float> percentages;
	std::vector<std::string> values;
	int longest_word_length = 0;
	for (const auto& percent : value_to_percent) {
		values.push_back(percent.first);
		if (percent.first.size() > longest_word_length) {
			longest_word_length = percent.first.size();
		}
		percentages.push_back(percent.second);
	}

	SetUpVariables(values, percentages, longest_word_length, radius, center_x, center_y, title);

	SetUpFonts();

	SetUpMousePosition();

	SetUpColors();

	SetUpPanel(title);
}

/*
 * Sets up the panel that allows the user to delete a pie chart
 */
void OfxPieChart::SetUpPanel(std::string title) {
	button_delete.addListener(this, &OfxPieChart::DeletePie);

	panel_size.set(100, 16);
	panel.setup();
	panel.setSize(panel_size.x, panel_size.y);
	panel.setDefaultHeight(panel_size.y);
	panel.setDefaultWidth(panel_size.x);
	panel.setName(title);

	panel.add(button_delete.setup("Delete"));
}

/*
 * Sets up the data needed to check if the user is trying to move either the pie or legend
 */
void OfxPieChart::SetUpMousePosition() {
	pie_mouse_ = false;
	pie_previous_x_ = ofGetMouseX();
	pie_previous_y_ = ofGetMouseY();

	legend_mouse_ = false;
	legend_previous_x_ = ofGetMouseX();
	legend_previous_y_ = ofGetMouseY();
}

/*
 * Sets up the fonts used in the pie chart labeling
 */
void OfxPieChart::SetUpFonts() {
	helvetica17_.load("Helvetica.ttf", 17);
	helvetica24_.load("Helvetica.ttf", 24);
	helvetica40_.load("Helvetica.ttf", 40);
}

void OfxPieChart::SetUpColors() {
	std::vector<float> red;
	red.push_back(1);
	red.push_back(0);
	red.push_back(0);

	std::vector<float> green;
	green.push_back(0);
	green.push_back(1);
	green.push_back(0);

	std::vector<float> blue;
	blue.push_back(0);
	blue.push_back(0);
	blue.push_back(1);

	list_of_colors.push_back(red);
	list_of_colors.push_back(green);
	list_of_colors.push_back(blue);
	int re = 20, gr = 90, bl = 140;
	for (int i = 3; i < percentages_.size(); i++) {
		std::vector<float> color;
		color.push_back(re / 255.0);
		color.push_back(gr / 255.0);
		color.push_back(bl / 255.0);
		list_of_colors.push_back(color);
		re += 62;
		re = re % 255;
		gr += 22;
		gr = gr % 255;
		bl += 46;
		bl = bl % 255;
	}
}

void OfxPieChart::PieDrag() {
	if (!IsMouseInPie() && !ofGetMousePressed()) {
		pie_mouse_ = false;
	}
	if (!pie_mouse_ && IsMouseInPie() && ofGetMousePressed()) {
		pie_mouse_ = true;
		pie_previous_x_ = ofGetMouseX();
		pie_previous_y_ = ofGetMouseY();
	}
	else if (pie_mouse_ && IsMouseInPie() && ofGetMousePressed()) {
		pie_center_x_ += ofGetMouseX() - pie_previous_x_;
		pie_center_y_ += ofGetMouseY() - pie_previous_y_;
		pie_previous_x_ = ofGetMouseX();
		pie_previous_y_ = ofGetMouseY();
	}
	else if (pie_mouse_ && IsMouseInPie() && !ofGetMousePressed()) {
		pie_mouse_ = false;
	}
}

void OfxPieChart::LegendDrag() {
	if (!IsMouseInLegend()) {
		legend_mouse_ = false;
	}
	if (!legend_mouse_ && IsMouseInLegend() && ofGetMousePressed()) {
		legend_mouse_ = true;
		legend_previous_x_ = ofGetMouseX();
		legend_previous_y_ = ofGetMouseY();
	}
	else if (legend_mouse_ && IsMouseInLegend() && ofGetMousePressed()) {
		legend_x_ += ofGetMouseX() - legend_previous_x_;
		legend_y_ += ofGetMouseY() - legend_previous_y_;
		legend_previous_x_ = ofGetMouseX();
		legend_previous_y_ = ofGetMouseY();
	}
	else if (legend_mouse_ && IsMouseInLegend() && !ofGetMousePressed()) {
		legend_mouse_ = false;
	}
}

/**
 * Draws the outline of the pie chart
 */
void OfxPieChart::DrawPieOutline() const {
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0, 0, 0);
	glVertex2d(pie_center_x_, pie_center_y_);
	for (float j = 0; j < kstep_ * 1001; j += kstep_) {
		glVertex2d(pie_center_x_ + (radius_ + 1) * sin(j), pie_center_y_ + (radius_ + 1) * cos(j));
	}
	glEnd();
}

void OfxPieChart::DrawPie() {
	float overall_slice = 0;
	std::vector<int> slice_number;

	for (int i = 0; i < percentages_.size(); i++) {
		glBegin(GL_TRIANGLE_FAN);
		glColor3f(list_of_colors.at(i).at(0), list_of_colors.at(i).at(1), list_of_colors.at(i).at(2));
		glVertex2d(pie_center_x_, pie_center_y_);
		for (float j = overall_slice; j < kstep_ * 1000 * percentages_.at(i) + overall_slice; j += kstep_) {
			glVertex2d(pie_center_x_ + radius_ * sin(j), pie_center_y_ + radius_ * cos(j));
		}
		overall_slice += kstep_ * 1000 * percentages_.at(i);
		glEnd();
	}

	ofSetColor(ofColor::black);
	helvetica40_.drawString(title, pie_center_x_ - title.size() * 12, pie_center_y_ - radius_ - 50);
}

/*
 * Draws the color legend for the pie chart
 */
void OfxPieChart::DrawLegend() {
	color_legend_outline.x = legend_x_;
	color_legend_outline.y = legend_y_;
	color_legend_outline.width = width_of_rec;
	color_legend_outline.height = height_of_rec;

	ofNoFill();
	ofSetColor(ofColor::black);
	ofDrawRectangle(color_legend_outline);
	ofFill();
	for (int i = 0; i < values_.size(); i++) {
		ofSetColor(ofColor::black);
		helvetica17_.drawString(values_.at(i), legend_x_ + 70, legend_y_ + 15 + 30 * (i + 1));
		ofSetColor(list_of_colors.at(i).at(0) * 255, list_of_colors.at(i).at(1) * 255, list_of_colors.at(i).at(2) * 255);

		ofDrawRectangle(legend_x_ + 10, legend_y_ + 30 * (i + 1), 20, 20);
	}
}

/*
 * Draws the pie chart's labels
 */
void OfxPieChart::DrawLabels() {
	ofSetColor(ofColor::black);
	float percentage_sum = 0;
	for (int i = 0; i < values_.size(); i++) {
		helvetica24_.drawString(values_.at(i), radius_ / 2 * sin(percentages_.at(i)*PI + percentage_sum) + pie_center_x_ - values_.at(i).length() * 7, radius_ / 2 * cos(percentages_.at(i)*PI + percentage_sum) + pie_center_y_);
		percentage_sum += percentages_.at(i) * 2 * PI;
	}
}

/*
 * Draws everything needed for the pie
 */
void OfxPieChart::Draw() {

	PieDrag();
	LegendDrag();

	ofFill();

	DrawPieOutline();
	DrawPie();
	DrawLegend();
	DrawLabels();

	panel.setPosition(pie_center_x_ + radius_, pie_center_y_ - radius_);
	panel.draw();
}

void OfxPieChart::SetUpVariables(std::vector<std::string> values, std::vector<float> percentages, int longest_word_length, float radius, int center_x, int center_y, std::string title) {
	this->title = title;
	this->values_ = values;
	this->percentages_ = percentages;
	this->radius_ = radius;
	this->pie_center_x_ = center_x;
	this->pie_center_y_ = center_y;
	this->width_of_rec = longest_word_length * 10 + 100;
	this->height_of_rec = values.size() * 45;

	this->legend_x_ = center_x + radius + 50;
	this->legend_y_ = center_y + radius + 50;
}

/**
 * Imports the given data from a CSV file
 */
void OfxPieChart::ImportNewData(ofFile file) {
	if (file.exists()) {
		const string file_extension = ofToUpper(file.getExtension());

		if (file_extension == "CSV") {
			std::string value;
			std::map<std::string, float> number_of_occurrences;
			float total = 0;
			while (getline(file, value, ',')) {
				TrimWord(value);
				if (number_of_occurrences.find(value) != number_of_occurrences.end()) {
					int old = number_of_occurrences.at(value);
					old++;
					number_of_occurrences.insert_or_assign(value, old);
				}
				else {
					number_of_occurrences.insert(std::pair<std::string, int>(value, 1));
				}
				total++;
			}
			std::vector<float> pie_percentages;

			for (const auto& n : number_of_occurrences) {
				number_of_occurrences.insert_or_assign(n.first, n.second / total);
			}
			RecreatePie(number_of_occurrences, radius_, pie_center_x_, pie_center_y_, file.getFileName().substr(0, file.getFileName().size() - 4));
		}

	}
}

/**
 * Imports the given data from a FileDialogResult pointing to a CSV
 */
void OfxPieChart::ImportNewData(ofFileDialogResult openFileResult) {
	const ofFile file(openFileResult.getPath());
	ImportNewData(file);
}

/*
 *Resets all the values needed for a new pie
 */
void OfxPieChart::RecreatePie(std::map<std::string, float> value_to_percent, float radius, int center_x, int center_y,
	std::string title) {

	std::vector<float> percentages;
	std::vector<std::string> values;
	int longest_word_length = 0;
	for (const auto& percent : value_to_percent) {
		values.push_back(percent.first);
		if (percent.first.size() > longest_word_length) {
			longest_word_length = percent.first.size();
		}
		percentages.push_back(percent.second);
	}

	SetUpVariables(values, percentages, longest_word_length, radius, center_x, center_y, title);

	SetUpColors();
}

/*
 * Sets a boolean flag that let's an outside program know to delete this specific pie chart
 * This object
 */
void OfxPieChart::DeletePie() {
	to_be_deleted = true;
}

/**
 * Returns true if the mouse is in the pie chart
 */
bool OfxPieChart::IsMouseInPie() const {
	//TODO Currently I'm approximating the pie chart with a square. I'd like to change this to actually use a closer approximation
	return static_cast<int>(ofGetMouseX()) < static_cast<int>(pie_center_x_) + radius_ && static_cast<int>(ofGetMouseX()
		) > pie_center_x_ - radius_ && static_cast<int>(ofGetMouseY()) > pie_center_y_ - radius_ && static_cast<int>(
			ofGetMouseY()) < pie_center_y_ + radius_;
}

/**
 * Returns true if the mouse is in the color legend
 */
bool OfxPieChart::IsMouseInLegend() const {
	return static_cast<int>(ofGetMouseX()) > static_cast<int>(legend_x_) && static_cast<int>(ofGetMouseX()) < legend_x_
		+ width_of_rec && static_cast<int>(ofGetMouseY()) > legend_y_ && static_cast<int>(ofGetMouseY()) < legend_y_ +
		height_of_rec;
}

/**
 * Removes all spaces from the beginning and end of the given word
 */
void OfxPieChart::TrimWord(std::string& word) {
	if (word.empty()) return;

	// Trim spaces from left side
	while (word.find(" ") == 0)
	{
		word.erase(0, 1);
	}

	// Trim spaces from right side
	size_t len = word.size();
	while (word.rfind(' ') == --len)
	{
		word.erase(len, len + 1);
	}
}

