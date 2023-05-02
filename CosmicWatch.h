#include "Drawn.h"
#include <fstream>
#include <string>

using namespace std;

class CosmicEvent
{
 public:
  CosmicEvent(string line); // Constructor from string containing one line (event) from the input file
  ~CosmicEvent();
  UInt_t EventID;
  TDatime* Date;
  UInt_t t; // in ms
  Double_t ADC1;
  Double_t ADC2;
  Double_t SiPM; // in mV
  Double_t T;    // in C
  Double_t P;    // in Pa
  UInt_t dt;     // in us
  Bool_t Coincident;
  TString ID;

  void Print();
};

CosmicEvent::CosmicEvent(string line)
{
  string CellContent;
  stringstream sline(line);
  getline(sline, CellContent, '\t'); // Read the EventID
  EventID = atoi(CellContent.c_str());
  Int_t DateTimeInt[6] = {0, 0, 0, 0, 0, 0}; // Year, Month, Day, Hour, Minute, Second
  getline(sline, CellContent, ':');          // Read the hour
  DateTimeInt[3] = atoi(CellContent.c_str());
  getline(sline, CellContent, ':'); // Read the minute
  DateTimeInt[4] = atoi(CellContent.c_str());
  getline(sline, CellContent, '\t'); // Read the second
  DateTimeInt[5] = atoi(CellContent.c_str());
  getline(sline, CellContent, '/'); // Read the day
  DateTimeInt[2] = atoi(CellContent.c_str());
  getline(sline, CellContent, '/'); // Read the month
  DateTimeInt[1] = atoi(CellContent.c_str());
  getline(sline, CellContent, '\t'); // Read the year
  DateTimeInt[0] = atoi(CellContent.c_str());
  Date = new TDatime(DateTimeInt[0], DateTimeInt[1], DateTimeInt[2], DateTimeInt[3], DateTimeInt[4], DateTimeInt[5]);
  //   Date->Print();
  getline(sline, CellContent, '\t'); // Read the timestamp
  t = atoi(CellContent.c_str());
  getline(sline, CellContent, '\t'); // Read the ADC1
  ADC1 = atof(CellContent.c_str());
  getline(sline, CellContent, '\t'); // Read the ADC2
  ADC2 = atof(CellContent.c_str());
  getline(sline, CellContent, '\t'); // Read the SiPM
  SiPM = atof(CellContent.c_str());
  getline(sline, CellContent, '\t'); // Read the Temperature
  T = atof(CellContent.c_str());
  getline(sline, CellContent, '\t'); // Read the Pressure
  P = atof(CellContent.c_str());
  getline(sline, CellContent, '\t'); // Read the deadtime
  dt = atoi(CellContent.c_str());
  getline(sline, CellContent, '\t'); // Read the coincidence
  Coincident = ((Bool_t)atoi(CellContent.c_str()));
  getline(sline, CellContent, '\t'); // Read the ID
  ID = TString(CellContent.c_str());
}

CosmicEvent::~CosmicEvent()
{
}

void CosmicEvent::Print()
{
  cout << EventID << "\t" << t << "\t" << ADC1 << "\t" << ADC2 << "\t" << SiPM << "\t" << T << "\t" << P << "\t" << dt << "\t" << Coincident << "\t" << ID << endl;
}

class CosmicWatch
{
 public:
  CosmicWatch(TString path); // Constructor from string leading to input file
  ~CosmicWatch();            // Empty constructor

  UInt_t NEvents = 0;

  void PrintEvent(Int_t Event = 1);
  void PrintEvents(Int_t EventFrom = 1, Int_t EventTo = 2);
  UInt_t GetNCoincidences(Double_t t0, Double_t t1);
  UInt_t GetEventAfterTime(UInt_t t);
  TH1F GetCoincidenceRate(Double_t tStart, Double_t tEnd, Double_t tBinWidth);

 private:
  vector<CosmicEvent> CosmicVector;
};

CosmicWatch::CosmicWatch(TString path)
{
  fstream f;
  f.open(path, ios::in);
  for (Int_t iline = 0; !f.eof(); iline++) {
    string line;
    getline(f, line);
    if (iline < 7 || line.empty())
      continue;
    CosmicVector.push_back(CosmicEvent(line));
  }
  NEvents = CosmicVector.size();
}

CosmicWatch::~CosmicWatch()
{
}

void CosmicWatch::PrintEvent(Int_t Event = 1)
{
  if (Event >= NEvents)
    return;
  cout << "Event\tt[ms]\tADC1\tADC2\tSiP[mV]\tTemp[C]\tP[Pa]\tdt[us]\tCoinc\tID" << endl;
  CosmicVector.at(Event).Print();
}

void CosmicWatch::PrintEvents(Int_t EventFrom, Int_t EventTo)
{
  cout << "Event\tt[ms]\tADC1\tADC2\tSiP[mV]\tTemp[C]\tP[Pa]\tdt[us]\tCoinc\tID" << endl;
  for (Int_t iEvent = EventFrom; iEvent < EventTo; iEvent++) {
    if (iEvent >= NEvents)
      return;
    CosmicVector.at(iEvent).Print();
  }
}

UInt_t CosmicWatch::GetEventAfterTime(UInt_t t)
{
  for (Int_t iEvent = 0; iEvent < NEvents; iEvent++) {
    if (CosmicVector.at(iEvent).t > t)
      return iEvent;
  }

  return NEvents;
}

UInt_t CosmicWatch::GetNCoincidences(Double_t t0, Double_t t1)
{
  UInt_t Event0 = GetEventAfterTime((UInt_t)(t0 * 1000));
  UInt_t Event1 = GetEventAfterTime((UInt_t)(t1 * 1000)) - 1;

  UInt_t SumCoincidences = 0;
  for (Int_t iEvent = Event0; iEvent < Event1; iEvent++) {
    if (CosmicVector.at(iEvent).Coincident)
      SumCoincidences++;
  }

  return (SumCoincidences);
}

TH1F CosmicWatch::GetCoincidenceRate(Double_t tStart, Double_t tEnd, Double_t tBinWidth)
{
  UInt_t EventStart = GetEventAfterTime((UInt_t)(tStart * 1000));
  UInt_t EventEnd = GetEventAfterTime((UInt_t)(tEnd * 1000));
  UInt_t iEvent = EventStart;

  TH1F h(Form("h_%.1f_%.1f_%.1f", tStart, tEnd, tBinWidth), Form("Rate in %.1f second intervals", tBinWidth), (tEnd - tStart)/tBinWidth, tStart, tEnd);

  for (Int_t iBin = 0; iBin < h.GetXaxis()->GetNbins() + 1; iBin++) {
    UInt_t tUpperBinEdge = 1000*(h.GetBinCenter(iBin) + h.GetBinWidth(iBin) / 2.);
    UInt_t NCoincidencesInBin = 0;
    for (; CosmicVector.at(iEvent).t < tUpperBinEdge; iEvent++) {
      if (CosmicVector.at(iEvent).Coincident)
        NCoincidencesInBin++;
    }
    h.SetBinContent(iBin,((Double_t)NCoincidencesInBin)/tBinWidth);
    h.SetBinError(iBin,TMath::Sqrt((Double_t)NCoincidencesInBin)/tBinWidth);
  }

  return h;
}