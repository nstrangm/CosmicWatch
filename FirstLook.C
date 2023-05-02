#include "CosmicWatch.h"

void FirstLook()
{
  cout << "---> Starting first look at cosmic data" << endl;

  CosmicWatch LectureTest("FileC002.txt");
  
  LectureTest.PrintEvents(0,5);


  // Define end of experiment at 150,000 seconds to have the same bin edged for all binnings
  Double_t tStart = 0.;
  Double_t tEnd = 2400.;
  // Double_t tEnd = 150000.;

  UInt_t NConcidences = LectureTest.GetNCoincidences(tStart, tEnd);
  cout << "Coincidence rate over the whole experiment = (" << NConcidences / tEnd << " +- " << TMath::Sqrt(NConcidences) / tEnd << ") 1/s" << endl;

  TH1F hs = LectureTest.GetCoincidenceRate(tStart,tEnd,1);
  TF1* fs = new TF1("fs","pol0",tStart,tEnd);
  hs.Fit(fs,"0");
  TH1F hm = LectureTest.GetCoincidenceRate(tStart,tEnd,60);
  TF1* fm = new TF1("fm","pol0",tStart,tEnd);
  hm.Fit(fm,"0");
  TH1F h10m = LectureTest.GetCoincidenceRate(tStart,tEnd,600);
  TF1* f10m = new TF1("f10m","pol0",tStart,tEnd);
  h10m.Fit(f10m,"0");

  Int_t Colors[3] = {kBlue+1, kRed+1, kGreen+1};
  Plotting1D PRates;
  PRates.NewHist(&hs, "1s intervals",20,1,Colors[0]);
  PRates.NewFunc(fs,"",1,3,Colors[0]);
  PRates.NewHist(&hm, "1m intervals",21,1,Colors[1]);
  PRates.NewFunc(fm,"",1,3,Colors[1]);
  PRates.NewHist(&h10m, "10m intervals",33,2,Colors[2]);
  PRates.NewFunc(f10m,"",1,3,Colors[2]);
  PRates.SetAxisLabel("t (s)", "Coincidence rate (1/s)",0.95,1.2);
  PRates.Plot("DifferentBinningRates.pdf");
  
  cout << "---> Finished the first look at cosmic data" << endl;
}