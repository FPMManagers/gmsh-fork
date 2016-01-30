// Gmsh - Copyright (C) 1997-2016 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@onelab.info>.

#include <FL/Fl_Tabs.H>
#include <FL/Fl_Return_Button.H>
#include "FlGui.h"
#include "drawContext.h"
#include "contextWindow.h"
#include "paletteWindow.h"
#include "GModel.h"
#include "Geo.h"
#include "GeoStringInterface.h"
#include "OpenFile.h"
#include "Context.h"

static void elementary_define_parameter_cb(Fl_Widget *w, void *data)
{
  add_param(FlGui::instance()->elementaryContext->input[0]->value(),
            FlGui::instance()->elementaryContext->input[1]->value(),
            FlGui::instance()->elementaryContext->input[24]->value(),
            FlGui::instance()->elementaryContext->input[25]->value(),
            GModel::current()->getFileName());
  FlGui::instance()->resetVisibility();
}

static void elementary_define_point_cb(Fl_Widget *w, void *data)
{
  add_point(GModel::current()->getFileName(),
            FlGui::instance()->elementaryContext->input[2]->value(),
            FlGui::instance()->elementaryContext->input[3]->value(),
            FlGui::instance()->elementaryContext->input[4]->value(),
            FlGui::instance()->elementaryContext->input[5]->value());
  FlGui::instance()->resetVisibility();
  GModel::current()->setSelection(0);
  SetBoundingBox();
  drawContext::global()->draw();
}

static void elementary_snap_cb(Fl_Widget *w, void *data)
{
  CTX::instance()->geom.snap[0] = FlGui::instance()->elementaryContext->value[0]->value();
  CTX::instance()->geom.snap[1] = FlGui::instance()->elementaryContext->value[1]->value();
  CTX::instance()->geom.snap[2] = FlGui::instance()->elementaryContext->value[2]->value();
}

elementaryContextWindow::elementaryContextWindow(int deltaFontSize)
{
  FL_NORMAL_SIZE -= deltaFontSize;

  int width = 31 * FL_NORMAL_SIZE;
  int height = 4 * WB + 8 * BH;

  win = new paletteWindow(width, height, CTX::instance()->nonModalWindows ? true : false,
                          "Elementary Entity Context");
  win->box(GMSH_WINDOW_BOX);
  {
    Fl_Tabs *o = new Fl_Tabs(WB, WB, width - 2 * WB, height - 2 * WB);
    // 0: Parameter
    {
      group[0] = new Fl_Group
        (WB, WB + BH, width - 2 * WB, height - 2 * WB - BH, "Parameter");
      input[0] = new Fl_Input(2 * WB, 2 * WB + 1 * BH, IW, BH, "Name");
      input[0]->value("lc");
      input[1] = new Fl_Input(2 * WB, 2 * WB + 2 * BH, IW, BH, "Value");
      input[1]->value("0.1");
      input[24] = new Fl_Input(2 * WB, 2 * WB + 3 * BH, IW, BH, "Label");
      input[24]->value("");
      input[25] = new Fl_Input(2 * WB, 2 * WB + 4 * BH, IW, BH, "Path");
      input[25]->value("Parameters");
      for(int i = 0; i < 2; i++)   input[i]->align(FL_ALIGN_RIGHT);
      for(int i = 24; i < 26; i++) input[i]->align(FL_ALIGN_RIGHT);
      {
        Fl_Return_Button *o = new Fl_Return_Button
          (width - BB - 2 * WB, 2 * WB + 7 * BH, BB, BH, "Add");
        o->callback(elementary_define_parameter_cb);
      }
      group[0]->end();
    }
    // 1: Point
    {
      group[1] = new Fl_Group
        (WB, WB + BH, width - 2 * WB, height - 2 * WB - BH, "Point");
      input[2] = new Fl_Input(2 * WB, 2 * WB + 1 * BH, IW, BH, "X coordinate");
      input[2]->value("0");
      input[3] = new Fl_Input(2 * WB, 2 * WB + 2 * BH, IW, BH, "Y coordinate");
      input[3]->value("0");
      input[4] = new Fl_Input(2 * WB, 2 * WB + 3 * BH, IW, BH, "Z coordinate");
      input[4]->value("0");
      input[5] = new Fl_Input(2 * WB, 2 * WB + 4 * BH, IW, BH,
                              "Prescribed mesh element size at point");
      input[5]->value("1.0");
      for(int i = 2; i < 6; i++) {
        input[i]->align(FL_ALIGN_RIGHT);
      }
      value[0] = new Fl_Value_Input(2 * WB, 2 * WB + 5 * BH, IW/3, BH);
      value[1] = new Fl_Value_Input(2 * WB + IW/3, 2 * WB + 5 * BH, IW/3, BH);
      value[2] = new Fl_Value_Input(2 * WB + 2*IW/3, 2 * WB + 5 * BH, IW/3, BH,
                                    "Snapping grid spacing");
      for(int i = 0; i < 3; i++) {
        value[i]->align(FL_ALIGN_RIGHT);
        value[i]->callback(elementary_snap_cb);
      }
      {
        Fl_Return_Button *o = new Fl_Return_Button
          (width - BB - 2 * WB, 2 * WB + 7 * BH, BB, BH, "Add");
        o->callback(elementary_define_point_cb);
      }
      group[1]->end();
    }
    // 2: Translation
    {
      group[2] = new Fl_Group
        (WB, WB + BH, width - 2 * WB, height - 2 * WB - BH, "Translation");
      input[6] = new Fl_Input(2 * WB, 2 * WB + 1 * BH, IW, BH, "X component");
      input[6]->value("0");
      input[7] = new Fl_Input(2 * WB, 2 * WB + 2 * BH, IW, BH, "Y component");
      input[7]->value("0");
      input[8] = new Fl_Input(2 * WB, 2 * WB + 3 * BH, IW, BH, "Z component");
      input[8]->value("1");
      for(int i = 6; i < 9; i++) {
        input[i]->align(FL_ALIGN_RIGHT);
      }
      group[2]->end();
    }
    // 3: Rotation
    {
      group[3] = new Fl_Group
        (WB, WB + BH, width - 2 * WB, height - 2 * WB - BH, "Rotation");
      input[9] = new Fl_Input
        (2 * WB, 2 * WB + 1 * BH, IW, BH, "X coordinate of an axis point");
      input[9]->value("0");
      input[10] = new Fl_Input
        (2 * WB, 2 * WB + 2 * BH, IW, BH, "Y coordinate of an axis point");
      input[10]->value("0");
      input[11] = new Fl_Input
        (2 * WB, 2 * WB + 3 * BH, IW, BH, "Z coordinate of an axis point");
      input[11]->value("0");
      input[12] = new Fl_Input
        (2 * WB, 2 * WB + 4 * BH, IW, BH, "X component of axis direction");
      input[12]->value("0");
      input[13] = new Fl_Input
        (2 * WB, 2 * WB + 5 * BH, IW, BH, "Y component of axis direction");
      input[13]->value("1");
      input[14] = new Fl_Input
        (2 * WB, 2 * WB + 6 * BH, IW, BH, "Z component of axis direction");
      input[14]->value("0");
      input[15] = new Fl_Input
        (2 * WB, 2 * WB + 7 * BH, IW, BH, "Angle in radians");
      input[15]->value("Pi/4");
      for(int i = 9; i < 16; i++) {
        input[i]->align(FL_ALIGN_RIGHT);
      }
      group[3]->end();
    }
    // 4: Scale
    {
      group[4] = new Fl_Group
        (WB, WB + BH, width - 2 * WB, height - 2 * WB - BH, "Scale");
      input[16] = new Fl_Input
        (2 * WB, 2 * WB + 1 * BH, IW, BH, "X component of direction");
      input[16]->value("0");
      input[17] = new Fl_Input
        (2 * WB, 2 * WB + 2 * BH, IW, BH, "Y component of direction");
      input[17]->value("0");
      input[18] = new Fl_Input
        (2 * WB, 2 * WB + 3 * BH, IW, BH, "Z component of direction");
      input[18]->value("0");
      input[19] = new Fl_Input
        (2 * WB, 2 * WB + 4 * BH, IW, BH, "Factor");
      input[19]->value("0.5");
      for(int i = 16; i < 20; i++) {
        input[i]->align(FL_ALIGN_RIGHT);
      }
      group[4]->end();
    }
    // 5: Symmetry
    {
      group[5] = new Fl_Group
        (WB, WB + BH, width - 2 * WB, height - 2 * WB - BH, "Symmetry");
      input[20] = new Fl_Input(2 * WB, 2 * WB + 1 * BH, IW, BH, "A");
      input[20]->value("1");
      input[21] = new Fl_Input(2 * WB, 2 * WB + 2 * BH, IW, BH, "B");
      input[21]->value("0");
      input[22] = new Fl_Input(2 * WB, 2 * WB + 3 * BH, IW, BH, "C");
      input[22]->value("0");
      input[23] = new Fl_Input(2 * WB, 2 * WB + 4 * BH, IW, BH, "D");
      input[23]->value("1");
      for(int i = 20; i < 24; i++) {
        input[i]->align(FL_ALIGN_RIGHT);
      }
      group[5]->end();
    }
    o->end();
  }

  win->position(CTX::instance()->ctxPosition[0], CTX::instance()->ctxPosition[1]);
  win->end();

  FL_NORMAL_SIZE += deltaFontSize;
}

void elementaryContextWindow::show(int pane)
{
  for(int i = 0; i < 6; i++)
    group[i]->hide();
  group[pane]->show();
  win->show();
}

static void physical_name_cb(Fl_Widget *w, void *data)
{
  std::string name = FlGui::instance()->physicalContext->input[0]->value();
  for(GModel::piter it = GModel::current()->firstPhysicalName();
      it != GModel::current()->lastPhysicalName(); it++){
    if(it->second == name){
      FlGui::instance()->physicalContext->input[0]->textcolor(FL_RED);
      FlGui::instance()->physicalContext->input[0]->redraw();
      return;
    }
  }
  FlGui::instance()->physicalContext->input[0]->textcolor(FL_FOREGROUND_COLOR);
  FlGui::instance()->physicalContext->input[0]->redraw();
}

static void physical_number_cb(Fl_Widget *w, void *data)
{
  if(FlGui::instance()->physicalContext->butt[0]->value()){
    FlGui::instance()->physicalContext->value[0]->value(NEWPHYSICAL());
    FlGui::instance()->physicalContext->value[0]->deactivate();
  }
  else{
    FlGui::instance()->physicalContext->value[0]->activate();
    int val = FlGui::instance()->physicalContext->value[0]->value();
    for(GModel::piter it = GModel::current()->firstPhysicalName();
        it != GModel::current()->lastPhysicalName(); it++){
      if(it->first.second == val){
        FlGui::instance()->physicalContext->value[0]->textcolor(FL_RED);
        FlGui::instance()->physicalContext->value[0]->redraw();
        return;
      }
    }
  }
  FlGui::instance()->physicalContext->value[0]->textcolor(FL_FOREGROUND_COLOR);
  FlGui::instance()->physicalContext->value[0]->redraw();
}

physicalContextWindow::physicalContextWindow(int deltaFontSize)
{
  FL_NORMAL_SIZE -= deltaFontSize;

  int width = 28 * FL_NORMAL_SIZE;
  int height = 2 * WB + 3 * BH;

  win = new paletteWindow(width, height, CTX::instance()->nonModalWindows ? true : false,
                          "Physical Group Context");
  win->box(GMSH_WINDOW_BOX);
  {
    input[0] = new Fl_Input(WB, WB, (2 * width) / 3, BH, "Name");
    input[0]->value("");
    input[0]->align(FL_ALIGN_RIGHT);
    input[0]->callback(physical_name_cb);
    input[0]->when(FL_WHEN_CHANGED);

    butt[0] = new Fl_Check_Button(WB, WB + BH, width - 2 * WB, BH, "Automatic numbering");
    butt[0]->value(1);
    butt[0]->callback(physical_number_cb);

    value[0] = new Fl_Value_Input(WB, WB + 2 * BH, (2 * width) / 3, BH, "Numeric tag");
    value[0]->value(0);
    value[0]->deactivate();
    value[0]->align(FL_ALIGN_RIGHT);
    value[0]->callback(physical_number_cb);
    value[0]->when(FL_WHEN_CHANGED);
  }

  win->position(CTX::instance()->ctxPosition[0], CTX::instance()->ctxPosition[1]);
  win->end();

  FL_NORMAL_SIZE += deltaFontSize;
}

void physicalContextWindow::show()
{
  physical_name_cb(0, 0);
  physical_number_cb(0, 0);
  if(!win->shown()) win->show();
}

meshContextWindow::meshContextWindow(int deltaFontSize)
{
  FL_NORMAL_SIZE -= deltaFontSize;

  static Fl_Menu menu_transfinite_dir[] = {
    {"Left", 0, 0, 0},
    {"Right", 0, 0, 0},
    {"Alternated", 0, 0, 0},
    {0}
  };

  int width = 29 * FL_NORMAL_SIZE;
  int height = 4 * WB + 4 * BH;

  win = new paletteWindow
    (width, height, CTX::instance()->nonModalWindows, "Mesh Context");
  win->box(GMSH_WINDOW_BOX);
  {
    Fl_Tabs *o = new Fl_Tabs(WB, WB, width - 2 * WB, height - 2 * WB);
    // 0: Element size at points
    {
      group[0] = new Fl_Group
        (WB, WB + BH, width - 2 * WB, height - 2 * WB - BH, "Element size");
      input[0] = new Fl_Input(2 * WB, 2 * WB + 1 * BH, IW, BH, "Value");
      input[0]->value("0.1");
      input[0]->align(FL_ALIGN_RIGHT);
      group[0]->end();
    }
    // 1: Transfinite line
    {
      group[1] = new Fl_Group
        (WB, WB + BH, width - 2 * WB, height - 2 * WB - BH, "Transfinite Line");
      input[1] = new Fl_Input(2 * WB, 2 * WB + 1 * BH, IW, BH, "Number of points");
      input[1]->value("10");
      input[2] = new Fl_Input(2 * WB, 2 * WB + 3 * BH, IW, BH, "Parameter");
      input[2]->value("1");
      for(int i = 1; i < 3; i++) {
        input[i]->align(FL_ALIGN_RIGHT);
      }
      static Fl_Menu_Item menu_trsf_mesh[] = {
        {"Progression", 0, 0, 0},
        {"Bump", 0, 0, 0},
        {0}
      };
      choice[0] = new Fl_Choice(2 * WB, 2 * WB + 2 * BH, IW, BH, "Type");
      choice[0]->menu(menu_trsf_mesh);
      choice[0]->align(FL_ALIGN_RIGHT);
      group[1]->end();
    }

    // 2: Transfinite surface
    {
      group[2] = new Fl_Group
        (WB, WB + BH, width - 2 * WB, height - 2 * WB - BH, "Transfinite Surface");

      choice[1] = new Fl_Choice
        (2 * WB, 2 * WB + 1 * BH, IW, BH, "Transfinite Arrangement");
      choice[1]->menu(menu_transfinite_dir);
      choice[1]->align(FL_ALIGN_RIGHT);

      group[2]->end();
    }
    o->end();
  }

  win->position(CTX::instance()->ctxPosition[0], CTX::instance()->ctxPosition[1]);
  win->end();

  FL_NORMAL_SIZE += deltaFontSize;
}

void meshContextWindow::show(int pane)
{
  for(int i = 0; i < 3; i++)
    group[i]->hide();
  group[pane]->show();
  win->show();
}
