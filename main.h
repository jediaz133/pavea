#include <wx/wxprec.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/dcbuffer.h>
#include <wx/button.h>
#include <wx/slider.h>
#include <iostream>
#include <wx/display.h>
#include <thread>
#include <chrono>
#include <wx/evtloop.h>
#include <wx/stattext.h>
#include <wx/image.h> // Required for wxImage
#include <cstdlib> 
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <sstream>
#include <fstream>
#include <mutex>
#include <wx/busyinfo.h>
#include <wx/progdlg.h>
#include <wx/popupwin.h>
#include <math.h>
#include <wx/animate.h>
#include <wx/filename.h>

// #include ""

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


class myFrame : public wxFrame
{
public:
    myFrame();
    void onButton(wxCommandEvent& event);
    void onEdit(wxCommandEvent& event);
    wxButton *selectFile;
    wxTextCtrl *filePath;
    wxSlider *senSlider;
    inline static wxButton* startIt;
    wxSlider *checkChunkSlider;
    wxSlider *bufferSlider;
    wxAnimationCtrl *animCtrl;
    wxDECLARE_EVENT_TABLE();
    // bool showState;

// 
private:
};
 
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};
 
using namespace std;
vector<vector<int>> toKeep(double avgDB, double vidLen, string filename,  int subDivisionsCheck, int dbMod);
double getInfo(string filename, bool avgOrTime = false); // avgOrTime is True to return total video time and False to return average video dB
static volatile int progInt;
extern bool showState;
double grabVol(string input);
int whitelister(vector<vector<int>> toKeep, string filename, int buffer);
void combiner(int size, string filename);
void editor(string file, int subDivisionsCheck, int buffer, int dbMod);
// static wxMessageDialog progress("loading");

