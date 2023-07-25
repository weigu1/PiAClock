// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// weigu.lu
// Small example of an analog clock on a 64x64 matrix.
// For more examples, look at demo-main.cc
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"
#include "graphics.h"

#include <signal.h>
#include <time.h>
#include <math.h>

using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

int main(int argc, char *argv[]) {
  rgb_matrix::Font font;
  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "regular";  // or e.g. "adafruit-hat"
  defaults.rows = 64; //64x64 matrix
  defaults.cols = 64;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  Canvas *canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &defaults);
  if (canvas == NULL)
    return 1;
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  const int x_center = 32;
  const int y_center = 32;

  // set the colors here
  Color sec_color(255, 255, 255);
  Color text_color(255, 50, 50);
  Color min_color(50, 255, 50);
  Color hour_color(50, 50, 255);
  Color bg_color(0, 0, 0);

  const int sec_hand_length = 25;
  const int min_hand_length = 23;
  const int hour_hand_length = 17;
  float dot_x_sec = 0, dot_x_min = 0, dot_x_hour = 0;
  float dot_y_sec = 0, dot_y_min = 0, dot_y_hour = 0;
  float angle = 0;

  const char *bdf_font_file = "../fonts/4x6.bdf";
  font.LoadFont(bdf_font_file);
  int text_xy_arr[12][2] = {
                            {45,10}, // 1 
                            {55,21}, // 2
                            {59,35}, // 3 
                            {55,49}, // 4
                            {45,60}, // 5
                            {31,64}, // 6
                            {16,60}, // 7
                            {6,49},  // 8
                            {3,35},  // 9
                            {2,21},  // 10
                            {12,10}, // 11
                            {29,6}   // 12
                           }; 
  struct tm tm;
  time_t now = time(0); // get time
  localtime_r(&now, &tm);
  int old_sec = tm.tm_sec;
  int old_min = tm.tm_min;
  // draw  everything for the first time
  for (int hour=1; hour<13; hour++) { // draw the 12 hour numbers
    rgb_matrix::DrawText(canvas,font,
                         text_xy_arr[hour-1][0], text_xy_arr[hour-1][1], // x,y
                         text_color, NULL, std::to_string(hour).c_str(),0);
  }
  angle = tm.tm_sec*6 - 90;
  dot_x_sec = cos(angle/360 * 2 * M_PI) * sec_hand_length + 32;
  dot_y_sec = sin(angle/360 * 2 * M_PI) * sec_hand_length + 32;
  rgb_matrix::DrawCircle(canvas,dot_x_sec,dot_y_sec,1,sec_color);
  angle = tm.tm_min*6 - 90;
  dot_x_min = cos(angle/360 * 2 * M_PI) * min_hand_length + 32;
  dot_y_min = sin(angle/360 * 2 * M_PI) * min_hand_length + 32;
  rgb_matrix::DrawLine(canvas,x_center,y_center,dot_x_min,dot_y_min,min_color);
  angle = (tm.tm_hour+(tm.tm_min/60.0))*5*6 - 90;  
  dot_x_hour = cos(angle/360 * 2 * M_PI) * hour_hand_length + 32;
  dot_y_hour = sin(angle/360 * 2 * M_PI) * hour_hand_length + 32;
  rgb_matrix::DrawLine(canvas,x_center,y_center,dot_x_hour,dot_y_hour,hour_color);

  while (!interrupt_received) {
    now = time(0);  // get time
    localtime_r(&now, &tm);
    if (tm.tm_sec != old_sec) {
      old_sec = tm.tm_sec;
      // delete old second hand
      rgb_matrix::DrawCircle(canvas,dot_x_sec,dot_y_sec,1,bg_color);
      // draw new second hand
      angle = tm.tm_sec*6 - 90;
      dot_x_sec = cos(angle/360 * 2 * M_PI) * sec_hand_length + 32;
      dot_y_sec = sin(angle/360 * 2 * M_PI) * sec_hand_length + 32;
      rgb_matrix::DrawCircle(canvas,dot_x_sec,dot_y_sec,1,sec_color);
    }
    if (tm.tm_min != old_min) {
      old_min = tm.tm_min; 
      // delete old min and hour hands
      rgb_matrix::DrawLine(canvas,x_center,y_center,dot_x_min,dot_y_min,bg_color);
      rgb_matrix::DrawLine(canvas,x_center,y_center,dot_x_hour,dot_y_hour,bg_color);
      // draw new min and hour hands
      angle = tm.tm_min*6 - 90;
      dot_x_min = cos(angle/360 * 2 * M_PI) * min_hand_length + 32;
      dot_y_min = sin(angle/360 * 2 * M_PI) * min_hand_length + 32;
      rgb_matrix::DrawLine(canvas,x_center,y_center,dot_x_min,dot_y_min,min_color);
      angle = (tm.tm_hour+(tm.tm_min/60.0))*5*6 - 90;  
      dot_x_hour = cos(angle/360 * 2 * M_PI) * hour_hand_length + 32;
      dot_y_hour = sin(angle/360 * 2 * M_PI) * hour_hand_length + 32;
      rgb_matrix::DrawLine(canvas,x_center,y_center,dot_x_hour,dot_y_hour,hour_color);
    }
  }
  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;
  return 0;
}

