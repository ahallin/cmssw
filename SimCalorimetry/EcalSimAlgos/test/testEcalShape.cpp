#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EcalSimParameterMap.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EcalShape.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include<iostream>
#include<iomanip>

#include "TROOT.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"

int main() {

  edm::MessageDrop::instance()->debugEnabled = false;

  EcalSimParameterMap parameterMap;

  EBDetId barrel(1,1);
  double thisPhase = parameterMap.simParameters(barrel).timePhase();

  EcalShape theShape(thisPhase, true );

  std::cout << "Parameters for the ECAL MGPA shape \n" << std::endl;

  std::cout << "Rising time for ECAL shape (timePhase) = " << parameterMap.simParameters(barrel).timePhase() << std::endl;
  std::cout << "Bin of maximum = " << parameterMap.simParameters(barrel).binOfMaximum() << std::endl;

  // standard display of the implemented shape function

  const int csize = EcalShapeBase::k1NSecBinsTotal;
  TCanvas * showShape = new TCanvas("showShape","showShape",2*csize,csize);

  const int nsamp = EcalShapeBase::k1NSecBinsTotal;
  const int tconv = EcalShapeBase::kNBinsPerNSec;

  const unsigned int histsiz = nsamp*tconv;

//  const std::vector<double>& nt = theShape.getTimeTable();
//  const std::vector<double>& ntd = theShape.getDerivTable();

  TH1F* shape1 = new TH1F("shape1","Tabulated Ecal MGPA shape",histsiz,0.,(float)(histsiz));
  TH1F* deriv1 = new TH1F("deriv1","Tabulated Ecal MGPA derivative",histsiz,0.,(float)(histsiz));


  const double ToM = theShape.timeOfMax();
  const double T0 = theShape.timeOfThr();
  const double risingTime = theShape.timeToRise();

  
  std::cout << "interpolated ECAL pulse shape and its derivative \n" << std::endl;
  for ( unsigned int i = 0; i < histsiz; ++i ) 
  {
     const double time ( (i-0.5)*0.1 - T0 ) ;
     const double myShape ( theShape( time ) ) ;
     const double myDeriv ( theShape.derivative( time ) ) ;
     shape1->Fill((float)(i+0.5),(float)myShape );
     deriv1->Fill((float)(i+0.5),(float)myDeriv );
     std::cout << " time (ns) = " << std::fixed << std::setw(6) << std::setprecision(2) << time + T0 + 0.1
	       << " shape = " << std::setw(11) << std::setprecision(8) << myShape 
	       << " derivative = " << std::setw(11) << std::setprecision(8) << myDeriv << std::endl;
  }

  showShape->Divide(2,1);
  showShape->cd(1);
  shape1->Draw();
  showShape->cd(2);
  deriv1->Draw();
  showShape->SaveAs("EcalShape.jpg");
  showShape->Clear("");

  delete shape1;
  delete deriv1;

  std::cout << "\n Maximum time from tabulated values = " << std::setprecision(2) << ToM << std::endl;
  std::cout << "\n Tzero from tabulated values        = " << std::setprecision(2) << T0 << std::endl;
  std::cout << "\n Rising time from tabulated values  = " << std::setprecision(2) << risingTime << std::endl;

  // signal used with the nominal parameters and no jitter

  std::cout << "\n computed ECAL pulse shape and its derivative (LHC timePhaseShift = 1) \n" << std::endl;
  double tzero = risingTime-(parameterMap.simParameters(barrel).binOfMaximum()-1.)*25.;
  double x = tzero ;

  TH1F* shape2 = new TH1F("shape2","Computed Ecal MGPA shape",nsamp,0.,(float)(nsamp));
  TH1F* deriv2 = new TH1F("deriv2","Computed Ecal MGPA derivative",nsamp,0.,(float)(nsamp));
  double y = 0.;
  double dy = 0.;
  for ( unsigned int i = 0; i < histsiz; ++i ) {
    y  = (theShape)(x);
    dy = theShape.derivative(x);
    shape2->Fill((float)(x-tzero),(float)y);
    deriv2->Fill((float)(x-tzero),(float)dy);
    std::cout << " time (ns) = "  << std::fixed    << std::setw(6)         << std::setprecision(2) << x-tzero 
              << " shape = "      << std::setw(11) << std::setprecision(5) << y
              << " derivative = " << std::setw(11) << std::setprecision(5) << dy << std::endl;
    x = x+1./(double)tconv;
  }

  for (int iSample = 0; iSample < 10; iSample++) 
     std::cout << (theShape)(tzero + iSample*25.0) << std::endl; 

  showShape->Divide(2,1);
  showShape->cd(1);
  gPad->SetGrid();
  shape2->GetXaxis()->SetNdivisions(10,kFALSE);
  shape2->Draw();

  showShape->cd(2);
  gPad->SetGrid();
  deriv2->GetXaxis()->SetNdivisions(10,kFALSE);
  deriv2->Draw();

  showShape->SaveAs("EcalShapeUsed.jpg");

  delete shape2;
  delete deriv2;
  delete showShape;

  return 0;

} 
