//===============================================================================
//
//	2015.05.03	Li Yi
//
//	Event level:
//	Vz, Vx, Vy -- Event in the center of detector, good acceptance
//	Number of primary vertices -- Pile up
//	Nearest Vz -- Pile up
//	NumberofPrimaryTracks/NumberofGlobalTracks -- Pile up
//	Number of Tracks Matched with fast detector (BEMC, BTOF) / NumberofPrimaryTracks -- Pile up
//	ZDC, BBC coincidenceRate -- event monitor
//	Number of Primary Tracks with BTOF or BEMC match
//
//	Track level:
//	|P|, Pt, phi, eta, charge
//
//
//===============================================================================


#include "/home/hep/caines/ly247/pp500/include/pp13tree_150915.C"	
#include <TString.h>
#include <TRandom3.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TMath.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLine.h>
#include <TLatex.h>
#include <TGraph.h>
#include <TStopwatch.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;


TCanvas *plot1d(TCanvas *c, TH1D *h, int cutflag=0, double cutmin=0, double cutmax=0) {
	
	double ymin = h->GetBinContent(h->GetMinimumBin());
	double ymax = h->GetBinContent(h->GetMaximumBin());
	double yratio = 0.1;
	double hmin = ymin - (ymax-ymin)*yratio;
	double hmax = ymax + (ymax-ymin)*yratio;
	h->SetMaximum(hmax);
	h->SetMinimum(hmin);

	c = new TCanvas(Form("c1d%s",h->GetName()),h->GetTitle());
	h->Draw("h");

	if(cutflag) {
		TLine *l = new TLine();
		l->SetLineColor(2);
		l->DrawLine(cutmin,hmin,cutmin,hmax);
		l->DrawLine(cutmax,hmin,cutmax,hmax);
		double cutratio = h->Integral(h->FindBin(cutmin),h->FindBin(cutmax))/h->Integral();
		TLatex *lat = new TLatex();
		lat->SetNDC(0);
		lat->DrawLatex((cutmin+cutmax)/2.,(hmin+hmax)/2,Form("%.f%%",cutratio*100));
	}

	c->SaveAs(Form("%sQApp13_150915.png",h->GetName()));
	
	return c;
	
}



void ppQA() {

	// histograms
	TH1D *vx = new TH1D("vx","Vx",1000,-5,5);
	TH1D *vy = new TH1D("vy","Vy",1000,-5,5);
	TH1D *vz = new TH1D("vz","Vz",1000,-60,60);
	TH2D *vxy = new TH2D("vxy","Vx vs Vy",1000,-5,5,1000,-5,5);
	TH1D *NPvertex = new TH1D("NPvertex","Number Of Primary Vertices",50,0,50);
	TH1D *NearestVz = new TH1D("NearestVz","Distance to Nearest Vz",500,0,50);	// if only one vertex, nearestvz is set as 999
	TH1D *ratioP2G = new TH1D("ratioP2G","Number of Primary Tracks / Number Of Global Tracks",100,0,1);
	TH1D *ratioBEMCMatch = new TH1D("ratioBEMCMatch","BEMC Match / Number of Primary Tracks",100,0,1);
	TH1D *ratioBTOFMatch = new TH1D("ratioBTOFMatch","BTOF Match / Number of Primary Tracks",100,0,1);
//	TH1D *zdccoin = new TH1D("zdccoin","ZDC coincidence rate",1000,-60,60);
//	TH1D *bbccoin = new TH1D("bbccoin","BBC coincidence rate",1000,-60,60);
	TH1D *ntrkmatched = new TH1D("ntrkmatched","Number of primary tracks matched with BTOF or BEMC",150,0,150);
	


	TH1D *phi = new TH1D("phi","#phi distribution",1000,-TMath::Pi(), TMath::Pi());
	TH1D *eta = new TH1D("eta","#eta distribution",1000,-1.2, 1.2);
	TH1D *pt = new TH1D("pt","p_{T} distribution",1000,0, 10);
	TH1D *p = new TH1D("p","|p| distribution",1000,0, 10);

	//Instantiate both the tree reading progs:
	TChain *chain = new TChain("pp13tree");
	chain->Add("/home/hep/caines/ly247/Scratch/pp13tree_500GeV_150915/*root");
	pp13tree *Eve = new pp13tree(chain);

	// read in events
	Int_t nEvents = chain->GetEntries();
	cout<<"# of Events :"  <<nEvents<<endl;
	//Loop over Events
	for(int ievt=0;ievt<nEvents;ievt++) {
		if(ievt%1000==0) cout<<"event "<<ievt<<endl;
		chain->GetEvent(ievt);
		// Event variables
		vx->Fill(Eve->mVx);
		vy->Fill(Eve->mVy);
		vz->Fill(Eve->mVz);
		vxy->Fill(Eve->mVx,Eve->mVy);
		NPvertex->Fill(Eve->mNumberOfPrimaryVertices);
		NearestVz->Fill(Eve->mNearestVz);
		ratioP2G->Fill((Eve->mNumberOfGlobalTracks>0)?1.*Eve->mNumberOfPrimaryTracks/Eve->mNumberOfGlobalTracks:0);
		ratioBEMCMatch->Fill((Eve->mNumberOfPrimaryTracks>0)?1.*Eve->mBEMCMatch/Eve->mNumberOfPrimaryTracks:0);
		ratioBTOFMatch->Fill((Eve->mNumberOfPrimaryTracks>0)?1.*Eve->mBTOFMatch/Eve->mNumberOfPrimaryTracks:0);
	
		int counttrkmatched = 0;
		for(int itrk = 0; itrk<Eve->mNtrk; itrk++) {
			phi->Fill(Eve->mPhi[itrk]);
			eta->Fill(Eve->mEta[itrk]);
			pt->Fill(Eve->mPt[itrk]);
			p->Fill(sqrt(Eve->mPt[itrk]*Eve->mPt[itrk]+Eve->mPz[itrk]*Eve->mPz[itrk]));
			if(Eve->mBTOFMatchFlag[itrk]>0||Eve->mBEMCMatchFlag[itrk]>0) {
				counttrkmatched++;
			}
		}
		ntrkmatched->Fill(counttrkmatched);
	}

	// Draw on Canvas
	TCanvas *c;
	plot1d(c,vx);
	plot1d(c,vy);
	plot1d(c,vz);
	plot1d(c,NPvertex);
	plot1d(c,NearestVz);
	plot1d(c,ratioP2G);
	plot1d(c,ratioBEMCMatch);
	plot1d(c,ratioBTOFMatch);
	plot1d(c,ntrkmatched);
	plot1d(c,phi);
	plot1d(c,eta);
	plot1d(c,pt);
	plot1d(c,p);

	c = new TCanvas("cvxy","vxy");
	vxy->Draw("col");
	c->SaveAs("vxyQApp13_150915.png");


	// Write output
	TFile *fout = new TFile("QA_pp13_150915.root","recreate");
	vx->Write();
	vy->Write();
	vz->Write();
	vxy->Write();
	NPvertex->Write();
	NearestVz->Write();
	ratioP2G->Write();
	ratioBEMCMatch->Write();
	ratioBTOFMatch->Write();
	ntrkmatched->Write();
	phi->Write();
	eta->Write();
	pt->Write();
	p->Write();

	fout->Close();
	
}



