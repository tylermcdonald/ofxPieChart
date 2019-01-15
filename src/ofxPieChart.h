#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class OfxPieChart
{
public:
	ofRectangle color_legend_outline;
	int width_of_rec;
	int height_of_rec;

	std::string title;

	std::vector<std::vector<float>> list_of_colors;

	OfxPieChart(float radius, int center_x, int center_y, std::string title);
	void DeletePie();
	OfxPieChart(std::map<std::string, float> value_to_percent, float radius, int center_x, int center_y, std::string title);
	void Draw();

	void ImportNewData(ofFile file);
	void ImportNewData(ofFileDialogResult open_file_result);

	void SetCenterX(const int center_x) {
		this->pie_center_x_ = center_x;
	}

	void SetCenterY(const int center_y) {
		this->pie_center_y_ = center_y;
	}

	void SetRadius(const float radius) {
		this->radius_ = radius;
	}

	void SetPercentages(const std::vector<float>& percentages) {
		this->percentages_ = percentages;
	}

	int CenterX() const {
		return pie_center_x_;
	}

	int CenterY() const {
		return pie_center_y_;
	}

	float Radius() const {
		return radius_;
	}

	std::vector<float> Percentages() const {
		return percentages_;
	}

	bool to_be_deleted = false;

	ofxPanel panel;
	ofVec2f panel_size;
	ofxButton button_delete;

private:
	int pie_center_x_;
	int pie_center_y_;
	int legend_x_;
	int legend_y_;

	float radius_;
	std::vector<float> percentages_;
	std::vector<std::string> values_;

	float pie_previous_x_;
	float pie_previous_y_;
	float legend_previous_x_;
	float legend_previous_y_;

	bool pie_mouse_;
	bool legend_mouse_;

	static void TrimWord(std::string& word);
	void RecreatePie(std::map<std::string, float> value_to_percent, float radius, int center_x, int center_y, std::string title);

	void SetUpColors();

	void PieDrag();
	void LegendDrag();

	void DrawPieOutline() const;
	void DrawPie();
	void DrawLegend();
	void DrawLabels();

	bool IsMouseInPie() const;
	bool IsMouseInLegend() const;

	const float kstep_ = TWO_PI / 1000;

	void SetUpVariables(std::vector<std::string> values, std::vector<float> percentages, int longest_word_length,
		float radius, int center_x, int center_y, std::string title);
	void SetUpMousePosition();
	void SetUpPanel(std::string title);
	void SetUpFonts();

	ofTrueTypeFont helvetica17_;
	ofTrueTypeFont helvetica24_;
	ofTrueTypeFont helvetica40_;
};