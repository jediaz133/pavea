#include "main.h"
using namespace std;


double grabVol(string input) // Reads the console and picks out the average decible
{
  int finalPos;
  double output;
  for (int i = 0; (input.at(input.find("mean_volume:")+13+i) != 'd'); i++){
      finalPos = i;
  }
  // Then returns the average volume of the whole video
  output = stod(input.substr(input.find("mean_volume:")+13, finalPos));
  return output;
  
}

double getInfo(string filename, bool avgOrTime) // avgOrTime is True to return total video time and False to return average video dB
{
  char cliBuffer[255];
  double output;
  string cliOut;
  FILE *stream;


  // Shoves a ffmpeg info command in to read the response and and then uses our grabVol function to cherry pick out the volume
  if (!avgOrTime) {
  string commandStr = "ffmpeg -i " + filename + " -af volumedetect -vn -f null /dev/null 2>&1";
  system(("echo command shoved in "+commandStr).c_str());
  char command[commandStr.length() + 1]; 
  strcpy(command, commandStr.c_str());


  // opens the string to read it
  stream = popen(command, "r");
  if (stream){
    while (!feof(stream)){
      // gets the opening of the command return and adds it to our 
      if (fgets(cliBuffer, 255, stream) != NULL){ cliOut.append(cliBuffer);}
    }
      pclose(stream); 
  }

  // grabVol to grab the volume from out cliOut string
  return grabVol(cliOut);  
  }
  else
  {


  //Shoves the command in same as before and reads it
  string commandStr = "ffprobe -show_entries format=duration " + filename;
  system(("echo command shoved in "+commandStr).c_str());
  char command[commandStr.length() + 1]; 
  strcpy(command, commandStr.c_str());
  stream = popen(command, "r");
  if (stream){
    while (!feof(stream)){
      if (fgets(cliBuffer, 255, stream) != NULL){ cliOut.append(cliBuffer);}
    }
      pclose(stream); 
  }
  
  // Looks through the string for the duration of the video
    int finalPosDur = 0;
    for (int i = 0; (cliOut.at(cliOut.find("duration=")+9+i) != '.'); i++){
        finalPosDur = i;
        }
    return (stod(cliOut.substr(cliOut.find("duration=")+9,finalPosDur+1)));
  }
}

vector<vector<int>> toKeep(double avgDB, double vidLen, string filename, int subDivisionsCheck = 5, int dbMod = 0)
{
  // wxMessageBox(");
  // wxMessageBox( wxT("Hello World!") );



  vector<vector<int>> toKeep;
  char cliBuffer[255];
  int finalPos;
  double output;
  string subInfo;
  FILE *stream;
  toKeep.clear();

  // Sorts through the whole length of video grabbing info  
  for (int i = 0; i <= vidLen; i+=subDivisionsCheck){
  cout << to_string(i) << " of " << to_string(vidLen) << "\n";
  string commandStr = "ffmpeg -ss "+ to_string(i) +" -to "+to_string(i+subDivisionsCheck)+" -i " + filename + " -af volumedetect -vn -f null /dev/null 2>&1";
  char command[commandStr.length() + 1] = "";
  strcpy(command, commandStr.c_str());
  subInfo = "";  
  // opens the string to read it
  stream = popen(command, "r");
  if (stream){
    // while not end of command return
    while (!feof(stream)){

      if (fgets(cliBuffer, 255, stream) != NULL){ subInfo.append(cliBuffer);}
    }
    pclose(stream);     
  }
  if (grabVol(subInfo) >= avgDB+dbMod){
    toKeep.push_back({i,i+subDivisionsCheck});
    system(("echo being "+to_string(i)+", end"+to_string(i+subDivisionsCheck)+", avg vol being: "+to_string(abs(grabVol(subInfo)))+", tot avg: "+to_string(abs(avgDB))).c_str());
  }
}
return toKeep;
// st->SetLabel(wxString((to_string(100*i/vidLen)).c_str()));

}


int whitelister(vector<vector<int>> toKeep, string filename, int buffer = 0) //Uses the toKeep vector to create tmp videos be combined
{

  system("mkdir /tmp/pavea_tmp");
  for (int i=0; i <= toKeep.size()-1; i++)
  {
      // if this isnt the first clip
      // if the buffer being added overlaps behind the start time of the whole video 0:00 then just go from start to the clip same for the start clip
      if (toKeep[i][0] > buffer && i > 0) {

        // if clip is a continuing clip then dont include any buffer
        if (toKeep[i-1][1] != toKeep[i][0]){

        // if the buffer does'nt overlap with a the previous clip then use full buffer
        if (toKeep[i-1][1] < toKeep[i][0]-buffer)
        {    
          system(("ffmpeg -ss " + to_string(toKeep[i][0]-buffer) + " -to " + to_string(toKeep[i][0]) + " -i " + filename + " -start_at_zero -avoid_negative_ts 1 -c:a pcm_s16le -c:v libx264 /tmp/pavea_tmp/tmp"+to_string(i)+".mp4").c_str());  
          toKeep.insert(toKeep.begin()+i,{toKeep[i][0]-buffer, toKeep[i][0]});
          i++;
        }
        // if buffer does overlap previous clip then just include the inbetween the two clips
        else {
          cout << "previous clip last timestamp " <<to_string(toKeep[i-1][1]) << "   next clip last timestamp: " << to_string(toKeep[i][0]-buffer) << "\n";
          system(("ffmpeg -ss " + to_string(toKeep[i-1][1]) + " -to " + to_string(toKeep[i][0]) + " -i " + filename + " -start_at_zero -avoid_negative_ts 1 -c:a pcm_s16le -c:v libx264 /tmp/pavea_tmp/tmp"+to_string(i)+".mp4").c_str());
          toKeep.insert(toKeep.begin()+i,{toKeep[i][1]-buffer, toKeep[i][0]});
          i++;
        }
      }
    }
      // if it is the first clip and the buffer does overlap behind start then just include from start to first point
      else if (i == 0 && toKeep[i][0] < buffer)
      {
       system(("ffmpeg -ss 0 -to " + to_string(toKeep[i][0]) + " -i " + filename + " -start_at_zero -avoid_negative_ts 1 -c:a pcm_s16le -c:v libx264 /tmp/pavea_tmp/tmp"+to_string(i)+".mp4").c_str());
          toKeep.insert(toKeep.begin()+i,{0, toKeep[i][0]});
          i++;
      }
      // if its the first clip andd the buffer is smaller than the gap then just include the buffers worth
      else
      {
      
        system(("ffmpeg -ss " + to_string(toKeep[i][0]-buffer) + " -to " + to_string(toKeep[i][0]) + " -i " + filename + " -start_at_zero -avoid_negative_ts 1 -c:a pcm_s16le -c:v libx264 /tmp/pavea_tmp/tmp"+to_string(i)+".mp4").c_str());  
          toKeep.insert(toKeep.begin()+i,{toKeep[i][0]-buffer, toKeep[i][0]});
          i++;
      }
    // Creates the non buffer clips
    system(("ffmpeg -ss " + to_string(toKeep[i][0]) + " -to " + to_string(toKeep[i][1]) + " -i " + filename + " -start_at_zero -avoid_negative_ts 1 -c:a pcm_s16le -c:v libx264 /tmp/pavea_tmp/tmp"+to_string(i)+".mp4").c_str());

  }
  return toKeep.size();
}


 
void combiner(int size, string filename) // Combines all the temp files
{
  ofstream outputFile("concatList");
   if (outputFile.is_open()) {
        
      for (int i = 1; i <= size-1; i++){
        outputFile << "file '/tmp/pavea_tmp/tmp"+to_string(i)+".mp4'" << endl;
      }
        outputFile.close();
    }

    filename = filename.insert(filename.find("."),"_EDITED");
    system(("ffmpeg -y -f concat -safe 0 -i concatList -c:v libx264 "+filename).c_str());
    for (int i=0; i <= size-1; i++){
      remove(("/tmp/pavea_tmp/tmp"+to_string(i)+".mp4").c_str());
    }
    remove("concatList");
}




void editor(string file, int subDivisionsCheck, int buffer, int dbMod){
  vector<vector<int>> power = toKeep((getInfo(file,false)),getInfo(file,true),file, subDivisionsCheck, dbMod);
  int tmpCount = whitelister(power, file, buffer);
  combiner(tmpCount, file);
  myFrame::startIt->Show(true);
}
