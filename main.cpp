#include "main.h"
// #include "editor.cpp"





void myFrame::onButton(wxCommandEvent& event){
    char filename[1024];
    FILE *f = popen("zenity --file-selection", "r");
    std::string fileChosen = fgets(filename, 1024, f);
    fileChosen.pop_back();
    filePath->SetValue(fileChosen);
}



void myFrame::onEdit(wxCommandEvent& event) {
    std::string toEdit = "\""+(filePath->GetValue()).ToStdString(wxConvUTF8)+"\"";
    int subDivisions = checkChunkSlider->GetValue();
    int buffer = bufferSlider->GetValue();
    int dbMod = senSlider->GetValue();
    // showState = false;
    startIt->Show(false);
    progInt = 0;
    std::thread t(editor, toEdit, subDivisions, buffer, dbMod);
    t.detach();
}






// My identifiers
enum
{
    ID_File = 1,
    ID_Path = 2,
    ID_StartIt = 3,
};
 

// Button Actions
wxBEGIN_EVENT_TABLE(myFrame, wxFrame) 
    EVT_BUTTON(ID_File ,myFrame::onButton)
    EVT_BUTTON(ID_StartIt ,myFrame::onEdit)
wxEND_EVENT_TABLE();
wxIMPLEMENT_APP(MyApp);
 


bool MyApp::OnInit()
{
    wxInitAllImageHandlers();
    myFrame *frame = new myFrame();
    frame->Show(true);
    return true;
} 
 

myFrame::myFrame()
    : wxFrame(NULL, wxID_ANY, "PAVEA", wxDefaultPosition, wxSize(840,470), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER)
{
   

    wxAnimationCtrl *animCtrl = new wxAnimationCtrl(this, wxID_ANY, wxNullAnimation, wxPoint(314,95), wxDefaultSize, wxAC_DEFAULT_STYLE);
    if (animCtrl->LoadFile("/usr/include/pavea/loading.gif")) {
            animCtrl->Play(); // Start playing the animationz
        } 
    animCtrl->SetSize(wxSize(100,100));

    auto mainText = new wxStaticText();
    mainText->Create(this, wxID_ANY , "Puffles.Automatic.Video.Editor.Application", wxPoint(225, 20) , wxDefaultSize);

    filePath = new wxTextCtrl();
    filePath->Create(this, ID_Path, std::getenv("HOME"), wxPoint(177,60),wxSize(450,33));


    // Select your file
    selectFile = new wxButton();
    selectFile->Create(this, ID_File,"Select",wxPoint(100,60), wxDefaultSize);

    // Starts editing
    startIt = new wxButton();
    startIt->Create(this, ID_StartIt,"Edit",wxPoint(320,130), wxDefaultSize);

    // Decible cutoff sensitivity. Higher means cut more often
    auto sensText = new wxStaticText();
    sensText->Create(this, wxID_ANY , "dB Sensitivity Adjust", wxPoint(50, 180) , wxDefaultSize);
    senSlider = new wxSlider();
    senSlider->Create(this,wxID_ANY, 0,-10,10,wxPoint(10,200), wxSize(200,-1), wxSL_HORIZONTAL | wxSL_LABELS);

        // Chunks are the intervals that are checked. The larger the chunk the larger the clips included
    auto chunkText = new wxStaticText();
    chunkText->Create(this, wxID_ANY , "Check Chunk Size", wxPoint(305, 180) , wxDefaultSize);
    checkChunkSlider = new wxSlider();
    checkChunkSlider->Create(this,wxID_ANY, 5,0,120,wxPoint(270,200), wxSize(200,-1), wxSL_HORIZONTAL | wxSL_LABELS);


        // Buffer is extra video included before the cut in as to give context on what is going on
    auto bufferText = new wxStaticText();
    bufferText->Create(this, wxID_ANY , "Cut-in Buffer Size", wxPoint(565, 180) , wxDefaultSize);
    bufferSlider = new wxSlider();
    bufferSlider->Create(this,wxID_ANY, 5,0,60,wxPoint(525,200), wxSize(200,-1), wxSL_HORIZONTAL | wxSL_LABELS);    

    }

