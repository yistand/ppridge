//===============================================================================
//
//
//	2015.09.16	Li Yi
//	modifed to pp from He3Au 
//
//===============================================================================
//
//
//	2015.03.14	Li Yi
//	modifed to He3Au from dAu
//	delete trgsetname
//
//===============================================================================
//
//      2013.10.11      Li Yi   
//      This code do dihadron real and mix event for dAu03 data
//
//      2013.10.30      Li Yi
//      fix bugs: should use unattenuated zdc adc instead of attenuated
//
//
//      2014.03.19      note: these unattenuated zdc should not be used for centrality
//
//      2014.03.31      default dca < 3 cm
//                      nHits >= 25 points, nHits = nHitsFit-1
//                      require mix with the same BField
//                      record Mix and Real Vz distribution for Vz weighted filled
//                      mix 10 times
//                      mix diffVz < 1 cm (was 5 cm)
//                      Change input MultL and MultH meaning, now it is MultL <= mult <= MultH
//                      ( was MultL <= mult < MultH )
//
//
//      2014.04.01      modified from /home/yil/dAuRidge/macro/DetaDphidAu03_TPC-TPC_FTPCECent.C
//                      generalize to process TPC-TPC and TPC-FTPC
//                      TPC, FTPCE and ZDCE centrality
//                      Could handle difference Associate Particle Charge
//                      also do the real/mix as Vz weight. It will be 1 when no file found. it will also generate real/mix Vz dist. file
//
//
//
//      2014.04.03      1. Change to |Vz|<50 cm instead of |Vz|<30 cm
//                      ----->>> In Fact, I foget to change it 
//                      2. Add option for the Trigger particle charge 
//
//
//      2014.04.08      1. It WAS still 30 cm instead of 50 cm in 2014.04.03... Forget to update this part in code.
//                          change it to 50 cm now
//                      2. mix match requires: multiplicity match for TPC and FTPC centrality
//                                             ZDC difference less than 10 for ZDC centrality
//
//      2014.04.23      add PID for TPC particle. now only for Pion
//                      Using new production with dEdx and nSigmaPion
//
//
//      2014.05.06      bug: TrigPhi/Eta[ArrayLength] --> TrigPhi/Eta[MaxT]                      
//
//      2014.05.19	FTPC charge ability issue. Try to look at global tracks for FTPC
//      		Add two more pt bins: 0.15-0.3, 0.3-0.5 GeV/c
//
//	2014.06.29	TPC add option for global track
//
//	2014.09.20	Add track cut on pz, eta difference between global and primary
//
//	2014.09.25	For primary and global match (to cut large eta/pz change from global to primary), test Flag for which detector. 
//	2014.09.25	Change ZDC match for 10 ADC difference to 5 ADC
//
//
//	2014.09.29	Mix event: require sqrt(DeltaVx*DeltaVx+DeltaVy*DeltaVy) difference < 0.1
//	2014.09.30	Mix event: require sqrt(DeltaVx*DeltaVx+DeltaVy*DeltaVy) difference < 0.05
//	2014.10.01	The above mix event requirment for Vr doesn't work
//
//===============================================================================




#include "/home/hep/caines/ly247/pp500/include/pp13tree_150915.C"	
//#include "/home/hep/caines/ly247/pp500/include/pp12tree_150928.C"	
#include <TString.h>
#include <TRandom3.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TStopwatch.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

TStopwatch timer;


void deta_dphi_pp(Int_t TPtNo=6, Int_t MultL = 2, Int_t MultH = 5000, int nopileupmatching=1, string Trig="HighMult", double dcacut = 3, int nhitcut = 25, int BField = -1, string pair="TPC-TPC", string centdefine = "TPC", int ACharge = 0, int TCharge = 0, string APID="All", string TPID="All" , string Atracktype="primary", string Ttracktype="primary")          
//  INPUT:
//
// BField: 1 for FF
//        -1 for RF
// pair: TPC-TPC
//
// Trig:
// 	VPDMB, HighMult
//
// centdefine:  TPC or ZDCE
//              TPC (all particle in |eta|<1)
//
// ACharge: 1 for Positive Charge Associate Particle
//         -1 for Negative Charge Associate
//          0 for the Inclusive Charge
//
// TCharge: 1 for Positive Charge Trigger Particle
//         -1 for Negative Charge Associate
//          0 for the Inclusive Charge
//
//
// APID:    All for non-identified particle, all particles 
//          (not in use currently 2014.04.23)
//
// TPID:    All for non-identified particle, all particles
//          Pion for Pion+ and Pion-.
//          Combined with TCharge to choose identified particle charges
// 
// Atracktype:
// 		associate particle
// 		primary for primary tracks
// 		global for global tracks
// 		
// Ttracktype:
// 		trigger particle
// 		primary for primary tracks
// 		global for global tracks
// 		
// nopileupmatching:
// 		Match TPC tracks to BEMC hits or not
//
//
{
	timer.Start();

	// 2014.04.01 ly
	// check input
	if((centdefine.compare("TPC"))&&(centdefine.compare("ZDCE"))) {                    // no match
		cout<<"ERR!! input centdefine should be TPC or ZDCE"<<endl;
		cout<<"BUT... I will use TPC as default centrality"<<endl;
		centdefine="TPC";
	}
	cout<<"Centrality definition: "<<centdefine<<endl;

	if((pair.compare("TPC-TPC"))) {         // no match
		cout<<"ERR!! input pair should be TPC-TPC" <<endl;
		cout<<"BUT... I will use TPC-TPC anyway as default pair"<<endl;
		pair="TPC-TPC";
	}
	cout<<"Pair used: "<<pair<<" ";
	cout<<Ttracktype<<"-"<<Atracktype<<endl;

	cout<<"Trigger particle charge: "<<TCharge<<endl;
	cout<<"Associate particle charge: "<<ACharge<<endl;

	cout<<"Particle PID? "<<TPID<<"-"<<APID<<endl;

	if(nopileupmatching) cout<<"Match BEMC/BTOF hits"<<endl;



	//Set Constants
	const float pi=TMath::Pi();
	const int NoAssocPt = 8;
	//const float PtLow[NoAssocPt]={0.15,0.5,1.0,1.5,2.0,2.5,3.0,4.0,6.0,0.15,0.3};
	//const float PtHigh[NoAssocPt]={0.5,1.0,1.5,2.0,2.5,3.0,4.0,6.0,10.0,0.3,0.5};
	const float PtLow[NoAssocPt]={0.15,1.0,2.0,3.0,4.0,6.0,1,0.15};
	const float PtHigh[NoAssocPt]={1.0,2.0,3.0,4.0,6.0,10.0,3,3};
	if(TPtNo>=NoAssocPt||TPtNo<0) { cout<<"TPtNo = "<<TPtNo<<" exceeds array"<<endl; exit(1);}
	const float TPtLow=PtLow[TPtNo];
	const float TPtHigh=PtHigh[TPtNo];


	TChain *chain = new TChain("pp13tree");
	chain->Add("/home/hep/caines/ly247/Scratch/pp13tree_500GeV_150915/*root");
	//TChain *chain = new TChain("pp12tree");
	//chain->Add(Form("/home/hep/caines/ly247/Scratch/pp12tree_500GeV_150928/*%s*root",Trig.data()));


	//Instantiate both the tree reading progs:
	pp13tree *Eve = new pp13tree(chain);
	//pp12tree *Eve = new pp12tree(chain);


	//Initial histograms
	TH2D *deta_dphi[NoAssocPt];                   // real events
	TH2D *mdeta_mdphi[NoAssocPt];                 // mix events
	TH1D *EtaDistribution[NoAssocPt];             // associate particle
	TH1D *pTDistribution[NoAssocPt];              // associate particle

	// histogram format
	int phibinning = 96;//48;
	int etabinning = 80;//48;
	double detamin = -2, detamax = 2;
	double etamin = -1, etamax = 1;
	if(!pair.compare("TPC-FTPC")) {
		detamin = -8;
		detamax = 8;
		etamin = -4;
		etamax = 4;
		etabinning = 320;
	}
	for(int i =0;i<NoAssocPt;i++) {
		deta_dphi[i]=new TH2D(Form("deta_dphi_pT%d",i),Form("Delta eta vs. delta phi: pT %.2f-%.2f ",PtLow[i],PtHigh[i]),etabinning,detamin,detamax,phibinning,-pi/2,3*pi/2);
		mdeta_mdphi[i]=new TH2D(Form("mdeta_mdphi_pT%d",i),Form("Mixed delta eta vs. delta phi: pT %.2f-%.2f ",PtLow[i],PtHigh[i]),etabinning,detamin,detamax,phibinning,-pi/2,3*pi/2);
		EtaDistribution[i]=new TH1D(Form("EtaDistribution_pT%d",i),Form("Eta Distribution for pT %.2f-%.2f ",PtLow[i],PtHigh[i]),etabinning,etamin,etamax);
		pTDistribution[i]=new TH1D(Form("pTDistribution_pT%d",i),Form("pT Distribution for pT %.2f-%.2f ",PtLow[i],PtHigh[i]),2000,0,20);
		deta_dphi[i]->Sumw2();
		mdeta_mdphi[i]->Sumw2();
		EtaDistribution[i]->Sumw2();
		pTDistribution[i]->Sumw2();
	}
		


	// trigger particle
	TH1D *TrigTot = new TH1D("TrigTot","Number of Trig.Particles",1,0,1);
	TrigTot->Sumw2();
	TH1D *TpTDistribution = new TH1D("TpTDistribution",Form("TpTDistribution pT %g-%g",TPtLow,TPtHigh),2000,0,20);
	TpTDistribution->Sumw2();
	TH1D *TEtaDistribution = new TH1D("TEtaDistribution",Form("TEtaDistribution pT %g-%g",TPtLow,TPtHigh),etabinning,-1,1);
	TEtaDistribution->Sumw2();



	// for Vz Mix weight use
	TH1D *EventTriggerVz=new TH1D("EventTriggerVz","All Trigger Particle Vz distribution",10000,-50, 50);            // 2014.03.31 ly, will weighted mix event by the real/mix ratio later
	TH1D *MatchedTriggerVz=new TH1D("MatchedTriggerVz","Matched Trigger Particle Vz distribution",10000,-50,50);              // 2014.03.31 ly, will weighted mix event by the real/mix ratio later
	TH1D *VzDifference = new TH1D("VzDifference","Vz difference",1000,-10,10);  // 2014.04.01 ly for monitoring purpose


	// histogram for event info.

	int maxzdc = 500;
	int maxntrack = 200;
	int maxbbc = 20000, bbcbins = 200;



	TH2D *BBCvsrefMult=new TH2D("BBCvsrefMult","Total BBC vs TPC refMult",maxntrack,0,maxntrack,bbcbins,0,maxbbc);
	TH2D *BBCEvsrefMult=new TH2D("BBCEvsrefMult","Total BBC East vs TPC refMult",maxntrack,0,maxntrack,bbcbins,0,maxbbc);
	TH2D *BBCWvsrefMult=new TH2D("BBCWvsrefMult","Total BBC West vs TPC refMult",maxntrack,0,maxntrack,bbcbins,0,maxbbc);
	TH2D *BBCWvsBBCE=new TH2D("BBCWvsBBCE","BBCW vs BBCE",bbcbins,0,maxbbc,bbcbins,0,maxbbc);
	BBCvsrefMult->Sumw2();
	BBCEvsrefMult->Sumw2();
	BBCWvsrefMult->Sumw2();
	BBCWvsBBCE->Sumw2();

	TH2D *BBCvsZDC=new TH2D("BBCvsZDC","Total BBC vs ZDC",maxzdc,0,maxzdc,bbcbins,0,maxbbc);
	TH2D *BBCEvsZDCE=new TH2D("BBCEvsZDCE","Total BBC East vs Attenuated ZDC East",maxzdc,0,maxzdc,bbcbins,0,maxbbc);
	TH2D *BBCWvsZDCW=new TH2D("BBCWvsZDCW","Total BBC West vs UnAttenuated ZDC West",maxzdc,0,maxzdc,bbcbins,0,maxbbc);
	BBCvsZDC->Sumw2();
	BBCEvsZDCE->Sumw2();
	BBCWvsZDCW->Sumw2();


	TH2D *refMultvsNPrimaryTrk=new TH2D("refMultvsNPrimaryTrk","TPC refMult vs NPrimaryTrk",maxntrack,0,maxntrack,maxntrack,0,maxntrack);
	TH2D *refMultvsNGlobalTrk=new TH2D("refMultvsNGlobalTrk","TPC refMult vs NGlobalTrk",maxntrack,0,maxntrack,maxntrack,0,maxntrack);
	TH2D *refMultvsNtrk=new TH2D("refMultvsNtrk","TPC refMult vs Ntrk",maxntrack,0,maxntrack,maxntrack,0,maxntrack);
	refMultvsNPrimaryTrk->Sumw2();
	refMultvsNGlobalTrk->Sumw2();
	refMultvsNtrk->Sumw2();


	TH2D *ZDCvsrefMult=new TH2D("ZDCvsrefMult","Total ZDC vs TPC refmult",maxntrack,0,maxntrack,maxzdc,0,maxzdc);
	TH2D *ZDCvsNtrk=new TH2D("ZDCvsNtrk","Total ZDC vs Ntrk",maxntrack,0,maxntrack,maxzdc,0,maxzdc);
	TH2D *ZDCEvsrefMult=new TH2D("ZDCEvsrefMult","Total Attenuated ZDC East vs TPC refmult",maxntrack,0,maxntrack,maxzdc,0,maxzdc);
	TH2D *ZDCEvsNtrk=new TH2D("ZDCEvsNtrk","Total Attenuated ZDC East vs Ntrk",maxntrack,0,maxntrack,maxzdc,0,maxzdc);
	TH2D *ZDCWvsrefMult=new TH2D("ZDCWvsrefMult","Total UnAttenuated ZDC West vs TPC refmult",maxntrack,0,maxntrack,maxzdc,0,maxzdc);
	TH2D *ZDCWvsNtrk=new TH2D("ZDCWvsNtrk","Total UnAttenuated ZDC West vs Ntrk",maxntrack,0,maxntrack,maxzdc,0,maxzdc);
	TH2D *ZDCWvsZDCE=new TH2D("ZDCWvsZDCE","UnAttenuated ZDCW vs Attenuated ZDCE",maxzdc,0,maxzdc,maxzdc,0,maxzdc);
	ZDCvsrefMult->Sumw2();
	ZDCvsNtrk->Sumw2();
	ZDCEvsrefMult->Sumw2();
	ZDCEvsNtrk->Sumw2();
	ZDCWvsrefMult->Sumw2();
	ZDCWvsNtrk->Sumw2();
	ZDCWvsZDCE->Sumw2();





	// Matching control
	//Max times matched for event
	Int_t MaxMatch = 10;          // 3;     // 2014.03.31 ly
	Double_t VzMatch = 1;         // 1;     // 2014.03.31 ly
	Double_t MultMatch = 1;		// 2015.09.16 ly
	//Double_t VrMatch = 0.05;			// 2014.09.29 ly
	int scratchy=0;                       // how many times overflow occurs and need to delte the old bin for current event recording

	//tirgger particle info in previous events for event mixing
	const int ArrayLength = 10000;         // max number events in history record
	const int MaxT = 100;                 // max number of trigger particle in one events
	Int_t HistTimes[ArrayLength] = {0};   // how many times been matched
	Int_t HistMult[ArrayLength] = {0};      // match Multiplicity for TPC, in ZDC difference less than 10
	Double_t HistVz[ArrayLength] = {0};      // will match Vz for mix events   2014.04.01 ly fix bugs. It was Int_t. 
	Double_t HistVx[ArrayLength] = {0};      // will match Vx for mix events   2014.09.30 ly for better mix event
	Double_t HistVy[ArrayLength] = {0};      // will match Vy for mix events   2014.09.30 ly for better mix event
	Int_t HistTotTrig[ArrayLength] = {0};
	Double_t HistTrigPhi[ArrayLength][MaxT] = {{0}};
	Double_t HistTrigEta[ArrayLength][MaxT] = {{0}};


	// 2014.04.01 ly weighted by Vz dist. from real/mix 
	TString BFieldname = "";                      // 2014.03.31
	if(BField>0) BFieldname="_FF";                // 2014.03.31
	if(BField<0) BFieldname="_RF";                // 2014.03.31
	TString achargename="";                         // 2014.04.01
	if(ACharge<0) achargename="ANegCharge";        // 2014.04.01
	if(ACharge>0) achargename="APosCharge";        // 2014.04.01
	TString tchargename="";                         // 2014.04.03
	if(TCharge<0) tchargename="TNegCharge";        // 2014.04.03
	if(TCharge>0) tchargename="TPosCharge";        // 2014.04.03
	TString tpidname="";                                           // 2014.04.23 ly
	if(!TPID.compare("Pion")) tpidname="_TPion";                    // 2014.04.23 ly
	if(!TPID.compare("Proton")) tpidname="_TProton";                    // 2014.07.10 ly
	if(!TPID.compare("Kaon")) tpidname="_TKaon";                    // 2014.07.10 ly
	TString atracktypename="";                                          
	TString ttracktypename="";                                        
	if( (!Atracktype.compare("global")) )		atracktypename="_Aglobal";                 
	if( (!Ttracktype.compare("global")) )		ttracktypename="_Tglobal";              
	TString nopileupmatchname="";
	if(nopileupmatching) nopileupmatchname="_BEMCBTOFmatch";
	TString dir=Form("/home/hep/caines/ly247/Scratch/pp500Y13treeCorr/%sCent_%s%s%s_150915/",centdefine.data(),pair.data(),tchargename.Data(),achargename.Data());         // 2014.04.04 ly
	//TString dir=Form("/home/hep/caines/ly247/Scratch/pp500Y12treeCorr_150928/%sCent_%s%s%s_150928/%s/",centdefine.data(),pair.data(),tchargename.Data(),achargename.Data(),Trig.data());         // 2014.04.04 ly
	TFile *fvzweight = new TFile(Form("%sInclude/VzACC/Vzratiorealmix%d_%s_dca%.0fnhit%dVz30_%s%s%s%s_T%g-%g_%s%d-%dMatch_VzMatch%gMultMatch%g%s.root", dir.Data(), MaxMatch,  BFieldname.Data(), dcacut, nhitcut, pair.data(), tchargename.Data(), tpidname.Data(), ttracktypename.Data(), TPtLow, TPtHigh, centdefine.data(), MultL, MultH, VzMatch,MultMatch,nopileupmatchname.Data()));            // for different TCharge (per trigger particle)
	TH1D *vzweight;
	if(fvzweight->IsOpen()) {                      // if real/mix histogram root file exists, read it out. if no use 1 as weight later
		vzweight = (TH1D*)fvzweight->Get("hratio");
		cout<<"do VzWeighted"<<endl;
	}
	// -- 2014.04.01 ly


	// read in events
	Int_t nEvents = chain->GetEntries();
	cout<<"# of Events :"  <<nEvents<<endl;
	Int_t nTrigProcessed = 0;
	//Loop over Events
	for(int ievt=0;ievt<nEvents;ievt++)
	{
		Double_t Vz=0;
		Double_t Vx=0;			// 2014.09.30 ly			
		Double_t Vy=0;			// 2014.09.30 ly

		Double_t Phi=0;
		Double_t Pt=0;
		Double_t Eta=0;
		Int_t TotTrig=0;
		Double_t   TrigPt[MaxT]={0};
		Double_t   TrigPhi[MaxT]={0};
		Double_t   TrigEta[MaxT]={0};

		chain->GetEvent(ievt);

		int Ntrk=Eve->mNtrk;


		//============== Event Cuts =========================
		// mix event with the same BField --> so process data with Field separately
		if(BField>0 && Eve->mMagField<0 ) continue;          // 2014.03.31 ly
		if(BField<0 && Eve->mMagField>0 ) continue;         // 2014.03.31 ly

		Vz = Eve->mVz;
		if(fabs(Vz)>50) continue;               // 2014.04.08 ly
		Vx = Eve->mVx;
		Vy = Eve->mVy;
		// Neutron Tag
		//      if(Eve->mZDCWSumAdc<16) continue;                       // 2013.10.30 Li YI, this is the  attenuated one
		//      if(Eve->mZDCAdc[0]<16) continue;                          // 2013.10.30 Li YI, need to use unattenuated zdc adc for n-tag

		//============== Centrality requirement =============
		int tracks = 0;                     // note 2014.04.08 ly, will used for mix matching later
		if(!centdefine.compare("TPC")) {
			for(int ii = 0; ii<Ntrk ; ii++) {
				if(fabs(Eve->mEta[ii])>1) continue;
				if(nopileupmatching>0 && Eve->mBEMCMatchFlag[ii]<=0 && Eve->mBTOFMatchFlag[ii]<=0) continue;
				tracks++;
			}
		}
		if(!centdefine.compare("ZDCE")) {
			tracks=Eve->mZDCAdc[13];                // attenuated zdc east adc
		}

		if(tracks<MultL||tracks>MultH) continue;         // 2014.03.31 ly. so [MultL, MultH] now



		//============== record event info.
		int refMult = Eve->mrefMult;
		int NPrimaryTrk = Eve->mNumberOfPrimaryTracks;
		int NGlobalTrk = Eve->mNumberOfGlobalTracks;
		float ZDCE = Eve->mZDCAdc[13];                             //  2013.11.01 Li YI, need to use attenuated one for centrality           // 2014.04.01 ly
		float ZDCW = Eve->mZDCAdc[0];                             //  2013.10.30 Li YI, need to   use unattenuated one for n-tag
		float ZDCSum = Eve->mZDCAdc[14];                          //  2013.10.30 Li YI not sure what this quantity means
		float BBCE = 0, BBCW = 0;
		for(int j=0;j<24;j++) {
			BBCE+=Eve->mBBCEAdc[j];
			BBCW+=Eve->mBBCWAdc[j];
		}
		float BBC=BBCE+BBCW;

		BBCvsrefMult->Fill(refMult,BBC);
		BBCEvsrefMult->Fill(refMult,BBCE);
		BBCWvsrefMult->Fill(refMult,BBCW);
		BBCWvsBBCE->Fill(BBCE,BBCW);

		BBCvsZDC->Fill(ZDCSum,BBC);
		BBCEvsZDCE->Fill(ZDCE,BBCE);
		BBCWvsZDCW->Fill(ZDCW,BBCW);


		refMultvsNtrk->Fill(Ntrk,refMult);
		refMultvsNPrimaryTrk->Fill(NPrimaryTrk,refMult);
		refMultvsNGlobalTrk->Fill(NGlobalTrk,refMult);


		ZDCvsrefMult->Fill(refMult,ZDCSum);
		ZDCvsNtrk->Fill(Ntrk,ZDCSum);
		ZDCEvsrefMult->Fill(refMult,ZDCE);
		ZDCEvsNtrk->Fill(Ntrk,ZDCE);
		ZDCWvsrefMult->Fill(refMult,ZDCW);
		ZDCWvsNtrk->Fill(Ntrk,ZDCW);
		ZDCWvsZDCE->Fill(ZDCE,ZDCW);


		//cout<<ievt<<" event"<<endl;

		//=========================== real events
		Int_t trigNtrk;
		//if(!Ttracktype.compare("global")) {
		//	trigNtrk = Eve->mglobalNtrk;
		//}
		//else {
			trigNtrk = Eve->mNtrk;
		//}	
		for(int ma=0;ma<trigNtrk;ma++)
		{
			//cout<<"trig "<<trigNtrk<<endl;

			Double_t trigNHitsFit, trigNHitsPos, triggDca, trigCharge, triggPz=999, trigpPz=999, triggEta=999, trigpEta=999;	

			//if(!Ttracktype.compare("global")) {
			//	trigNHitsFit = Eve->mglobalNHitsFit[ma]+1;
			//	trigNHitsPos = Eve->mglobalNHitsPos[ma];
			//	triggDca = Eve->mglobalDca[ma];
			//	trigCharge = Eve->mglobalCharge[ma];
			//	TrigPt[TotTrig] = Eve->mglobalPt[ma];
			//	TrigPhi[TotTrig] = Eve->mglobalPhi[ma];
			//	TrigEta[TotTrig] = Eve->mglobalEta[ma];
			//	triggPz = Eve->mglobalPz[ma];
			//	triggEta = Eve->mglobalEta[ma];
			//	for(int iptrk = 0; iptrk<Eve->mNtrk; iptrk++) {
			//		if((Eve->mglobalid[ma]==Eve->mid[iptrk])&&(fabs(Eve->mglobalFlag[ma]-Eve->mFlag[iptrk])<300)) {		// Flag: not in different detectors. FTPC flag 7xx, TPC flag 1xx or 3xx
			//			trigpPz = Eve->mPz[iptrk];
			//			trigpEta = Eve->mEta[iptrk];
			//			break;
			//		}
			//	}
			//}
			//else {
				trigNHitsFit = Eve->mNHitsFit[ma];
				trigNHitsPos = Eve->mNHitsPos[ma];
				triggDca = Eve->mgDca[ma];
				trigCharge = Eve->mCharge[ma];
				TrigPt[TotTrig] = Eve->mPt[ma];
				TrigPhi[TotTrig] = Eve->mPhi[ma];
				TrigEta[TotTrig] = Eve->mEta[ma];
			//	trigpPz = Eve->mPz[ma];
			//	trigpEta = Eve->mEta[ma];
			//	for(int igtrk = 0; igtrk<Eve->mglobalNtrk; igtrk++) {
			//		if((Eve->mid[ma]==Eve->mglobalid[igtrk])&&(fabs(Eve->mFlag[ma]-Eve->mglobalFlag[igtrk])<300)) {
			//			triggPz = Eve->mglobalPz[igtrk];
			//			triggEta = Eve->mglobalEta[igtrk];
			//			break;
			//		}
			//	}
			//}

			if(TCharge>0 && trigCharge<=0) continue; 
			if(TCharge<0 && trigCharge>=0) continue;        

			if(nopileupmatching && Eve->mBEMCMatchFlag[ma]<=0 && Eve->mBTOFMatchFlag[ma]<=0) continue;		// BEMC matching
			
			//if(!TPID.compare("Pion")) {                             // 2014.04.23 ly
			//	if(!Ttracktype.compare("global")) {
			//		// 2014.07.10 ly if(Eve->mglobalNSigmaPion[ma]>0 || Eve->mglobalNSigmaPion[ma]<-2) continue;      // Pion 
			//		if(fabs(Eve->mglobalNSigmaPion[ma])>2 || fabs(Eve->mglobalNSigmaProton[ma])<2 || fabs(Eve->mglobalNSigmaKaon[ma])<2 ) continue;      // Pion 
			//	}
			//	else {
			//		// 2014.07.10 ly if(Eve->mNSigmaPion[ma]>0 || Eve->mNSigmaPion[ma]<-2) continue;      // Pion 
			//		if(fabs(Eve->mNSigmaPion[ma])>2 || fabs(Eve->mNSigmaProton[ma])<2 || fabs(Eve->mNSigmaKaon[ma])<2 ) continue;      // Pion 
			//	}
			//}       // 2014.04.23 ly

			//if(!TPID.compare("Proton")) {                             // 2014.04.23 ly
			//	if(!Ttracktype.compare("global")) {
			//		// 2014.07.10 ly if(Eve->mglobalNSigmaProton[ma]>0 || Eve->mglobalNSigmaProton[ma]<-2) continue;      // Proton 
			//		if(fabs(Eve->mglobalNSigmaProton[ma])>2 || fabs(Eve->mglobalNSigmaPion[ma])<2 || fabs(Eve->mglobalNSigmaKaon[ma])<2 ) continue;      // Proton 
			//	}
			//	else {
			//		// 2014.07.10 ly if(Eve->mNSigmaProton[ma]>0 || Eve->mNSigmaProton[ma]<-2) continue;      // Proton 
			//		if(fabs(Eve->mNSigmaProton[ma])>2 || fabs(Eve->mNSigmaPion[ma])<2 || fabs(Eve->mNSigmaKaon[ma])<2 ) continue;      // Proton 
			//	}
			//}       // 2014.04.23 ly


			//if(!TPID.compare("Kaon")) {                             // 2014.04.23 ly
			//	if(!Ttracktype.compare("global")) {
			//		// 2014.07.10 ly if(Eve->mglobalNSigmaKaon[ma]>0 || Eve->mglobalNSigmaKaon[ma]<-2) continue;      // Kaon 
			//		if(fabs(Eve->mglobalNSigmaKaon[ma])>2 || fabs(Eve->mglobalNSigmaPion[ma])<2 || fabs(Eve->mglobalNSigmaProton[ma])<2 ) continue;      // Kaon 
			//	}
			//	else {
			//		// 2014.07.10 ly if(Eve->mNSigmaKaon[ma]>0 || Eve->mNSigmaKaon[ma]<-2) continue;      // Kaon 
			//		if(fabs(Eve->mNSigmaKaon[ma])>2 || fabs(Eve->mNSigmaPion[ma])<2 || fabs(Eve->mNSigmaProton[ma])<2 ) continue;      // Kaon 
			//	}
			//}       // 2014.04.23 ly

			if( (trigNHitsFit-1) < nhitcut ) continue;         // 2014.03.31 nHits = nHitsFit - 1
			if(1.0*(trigNHitsFit-1)/trigNHitsPos < 0.51) continue;      // 2014.03.31 nHits = nHitsFit - 1
			if(fabs(triggDca) >= dcacut) continue;            // 2014.03.31 ly: dca > 3 cm for example

			if(fabs(TrigEta[TotTrig])>=1.0) continue;                // TPC         // 2014.03. 31 |eta| < 1.0
			if(TrigPt[TotTrig]<TPtLow||TrigPt[TotTrig]>=TPtHigh)continue;

			//fill trigger particle histogram
			TEtaDistribution->Fill(TrigEta[TotTrig]);
			TpTDistribution->Fill(TrigPt[TotTrig]);

			////Begin Associate Loop for real events
			Int_t assocNtrk;
			//if(!Atracktype.compare("global") ) {			
			//	assocNtrk = Eve->mglobalNtrk;		
			//}
			//else {
				assocNtrk = Eve->mNtrk;
			//}

			//2014.06.19 ly for(int n=0;n<Ntrk;n++)
			for(int n=0;n<assocNtrk;n++)	// 2014.06.19 ly
			{
				if( (n==ma) && (!pair.compare("TPC-TPC")) && (!Ttracktype.compare(Atracktype)))  continue;			// Note: there is one situation missing. TPC-TPC with different track type, the track id need to be checked.
				Double_t assocNHitsFit, assocNHitsPos, assocgDca, assocCharge, assocgPz = 999, assocpPz = 999, assocgEta = 999, assocpEta = 999;
				Int_t assocnhitscut;

				//// read in track information. global or primary
				//if( (!Atracktype.compare("global")) )  {          
				//	assocNHitsFit = Eve->mglobalNHitsFit[n]+1;		// will ' -1 ' later in the cuts, global track doesn't have primary vertex in fitting
				//	assocNHitsPos = Eve->mglobalNHitsPos[n];
				//	assocgDca = Eve->mglobalDca[n];
				//	assocCharge = Eve->mglobalCharge[n];
				//	Pt=Eve->mglobalPt[n];
				//	Phi=Eve->mglobalPhi[n];
				//	Eta=Eve->mglobalEta[n];
				//	assocgPz=Eve->mglobalPz[n];
				//	assocgEta=Eve->mglobalEta[n];
				//	for(int iptrk = 0 ; iptrk<Eve->mNtrk ; iptrk++) {
				//		if((Eve->mglobalid[n]==Eve->mid[iptrk])&&(fabs(Eve->mglobalFlag[n]-Eve->mFlag[iptrk])<300)) {		// Flag: FTPC 7xx. TPC 1xx or 3xx. TPC pileup: 1xxx
				//			assocpPz=Eve->mPz[iptrk];
				//			assocpEta=Eve->mEta[iptrk];
				//			break;
				//		}
				//	}
				//}
				//else {
					assocNHitsFit = Eve->mNHitsFit[n];
					assocNHitsPos = Eve->mNHitsPos[n];
					assocgDca = Eve->mgDca[n];
					assocCharge = Eve->mCharge[n];
					Pt=Eve->mPt[n];
					Phi=Eve->mPhi[n];
					Eta=Eve->mEta[n];
				//	assocpPz=Eve->mPz[n];
				//	assocpEta=Eve->mEta[n];
				//	for(int igtrk = 0 ; igtrk<Eve->mglobalNtrk; igtrk++) {
				//		if((Eve->mid[n]==Eve->mglobalid[igtrk])&&(fabs(Eve->mFlag[n]-Eve->mglobalFlag[igtrk])<300)) {
				//			assocgPz=Eve->mglobalPz[igtrk];
				//			assocgEta=Eve->mglobalEta[igtrk];
				//			break;
				//		}
				//	}
				//}

				if( !pair.compare("TPC-FTPC") )  {          // 2014.04.01 ly
					assocnhitscut = 5;
					if(fabs(Eta)>=3.8 || fabs(Eta)<=2.8) continue;      // FTPC
				}
				else {
					assocnhitscut = nhitcut;
					if(fabs(Eta)>=1.0)continue;                        // TPC     // 2014.03.31 ly |eta|<1.0
				}

				if(1.0*(assocNHitsFit-1)/assocNHitsPos < 0.51) continue;      // 2014.03.31 ly
				if(fabs(assocgDca) >= dcacut) continue;       // 2014.03.31 ly dca<3cm

				if( ACharge>0 && assocCharge<=0) continue;       // 2014.04.01 ly
				if( ACharge<0 && assocCharge>=0) continue;       // 2014.04.01 ly

				if( nopileupmatching && Eve->mBEMCMatchFlag[n]<=0 && Eve->mBTOFMatchFlag[n]<=0 ) continue;

				if( (assocNHitsFit-1) < assocnhitscut) continue;             // 2014.06.19 ly

				if(Pt<0.15)continue;

				Float_t dPhi = Phi - TrigPhi[TotTrig];
				Float_t dEta = Eta - TrigEta[TotTrig];

				//Folding 
				if(dPhi<-pi/2) dPhi+=2*pi;            // ly 05.09
				if(dPhi>3*pi/2) dPhi-=2*pi;

				Int_t region=999;
				for(int iptr = 0;iptr<NoAssocPt;iptr++) {
					if(Pt<PtHigh[iptr]&&Pt>=PtLow[iptr]) { 
						region = iptr;              // found pt bin

						if(TotTrig==0)        // fill associate particle info only once per event not per trigger particle
						{
							pTDistribution[region]->Fill(Pt);
							EtaDistribution[region]->Fill(Eta);

						}

						//fill real event \Delta\eta - \Delta\phi distribution
						deta_dphi[region]->Fill(dEta,dPhi);
					}
				}

			}//End of Associate Loop

			TotTrig++;

		}//End of trig loop

		// fill no. of trigger particle in event
		TrigTot->Fill(0.5,TotTrig);
		EventTriggerVz->Fill(Vz,TotTrig);               // 2014.03.31 ly  Trigger particle Vz dist. for all trigger particle read in
		nTrigProcessed+=TotTrig;

		//cout<<"Number of Trig = "<<TotTrig<<" mix event --- "<<ievt<<endl;		// test

		//========================  mix events
		// Loop events in history
		for(int ihevt = 0; ihevt < ArrayLength; ihevt ++) {

			if(HistTimes[ihevt] > MaxMatch) continue;

			if(HistTotTrig[ihevt]<=0) continue;

			// 2014.04.08 ly    need match multiplicity
			if((!centdefine.compare("TPC")) || (!centdefine.compare("FTPCE"))) {
				//2015.09.16 ly if(HistMult[ihevt]!=tracks) continue;           
				if(fabs(HistMult[ihevt]-tracks)>MultMatch) continue;           	//2015.09.16 ly
			}
			if(!centdefine.compare("ZDCE")) {
				if(fabs(HistMult[ihevt]-tracks)>=10) continue;
				//2014.09.29 change back to 10 ADC if(fabs(HistMult[ihevt]-tracks)>=5) continue;	// 2014.09.25
			}
			// --- end of 2014.04.08 ly match multiplicity

			double diffVz = HistVz[ihevt] - Vz;
			if(fabs(diffVz) > VzMatch) continue;

			//cout<<"diffVz = "<<diffVz<<endl; // test
			VzDifference->Fill(diffVz);

			// 2014.10.01 ly double DeltaVx = HistVx[ihevt]-Vx;
			// 2014.10.01 ly double DeltaVy = HistVy[ihevt]-Vy;
			// 2014.10.01 ly double diffVr = TMath::Sqrt(DeltaVx*DeltaVx + DeltaVy*DeltaVy);				// Mix Vr ly 2014.09.29
			// 2014.10.01 ly if(fabs(diffVr) > VrMatch) continue;				// Mix Vr ly 2014.09.29

			// 2014.10.01 ly VrDifference->Fill(diffVr);					// Mix Vr ly 2014.09.29
			// 2014.10.01 ly VxDifference->Fill(DeltaVx);					// Mix Vr ly 2014.09.29
			// 2014.10.01 ly VyDifference->Fill(DeltaVy);					// Mix Vr ly 2014.09.29

			int flag = 0;           // 2014.03.31 ly whether any mixed pair

			// Begin History Trigger loop for mix events
			for(int iht = 0; iht<HistTotTrig[ihevt] ; iht++)         
			{

				double weight4vz = 1;
				if(fvzweight->IsOpen()) {
					weight4vz= vzweight->GetBinContent(vzweight->FindBin(HistVz[ihevt]));   // 2014.04.01 ly
					                    //cout<<"Vz = "<<HistVz[ihevt]<<"\t"<<vzweight->GetBinContent(vzweight->FindBin(HistVz[ihevt]))<<"\tweight4vz ="<<weight4vz<<endl;         // test 
				}

				//// Begin Associate loop for mix events
				Int_t assocNtrk;
				//if( (!Atracktype.compare("global")) ) {
				//	assocNtrk = Eve->mglobalNtrk;
				//}
				//else {
					assocNtrk = Eve->mNtrk;
				//}

				// 2014.06.19 ly for(int ia = 0; ia<Ntrk ; ia++)             
				for(int ia = 0; ia<assocNtrk ; ia++)             // 2014.06.19 ly
				{
					Double_t assocNHitsFit, assocNHitsPos, assocgDca, assocCharge,  assocgPz = 999, assocpPz = 999, assocgEta = 999, assocpEta = 999;
					Int_t assocnhitscut;

					//if( (!Ttracktype.compare("global")) ) {         
					//	assocNHitsFit = Eve->mglobalNHitsFit[ia]+1;              // will ' -1 ' later in the cuts, global track doesn't have primary vertex in fitting
					//	assocNHitsPos = Eve->mglobalNHitsPos[ia];
					//	assocgDca = Eve->mglobalDca[ia];
					//	assocCharge = Eve->mglobalCharge[ia];
					//	Pt=Eve->mglobalPt[ia];
					//	Phi=Eve->mglobalPhi[ia];
					//	Eta=Eve->mglobalEta[ia];
					//	assocgPz=Eve->mglobalPz[ia];
					//	assocgEta=Eve->mglobalEta[ia];
					//	for(int iptrk = 0 ; iptrk<Eve->mNtrk ; iptrk++) {
					//		if((Eve->mglobalid[ia]==Eve->mid[iptrk])&&(fabs(Eve->mglobalFlag[ia]-Eve->mFlag[iptrk])<300)) {		// Flag: FTPC 7xx, TPC 1xx or 3xx. TPC pile up 1xxx
					//			assocpPz=Eve->mPz[iptrk];
					//			assocpEta=Eve->mEta[iptrk];
					//			break;
					//		}
					//	}
					//}
					//else {
						assocNHitsFit = Eve->mNHitsFit[ia];
						assocNHitsPos = Eve->mNHitsPos[ia];
						assocgDca = Eve->mgDca[ia];
						assocCharge = Eve->mCharge[ia];
						Pt=Eve->mPt[ia];
						Phi=Eve->mPhi[ia];
						Eta=Eve->mEta[ia];
					//	assocpPz=Eve->mPz[ia];
					//	assocpEta=Eve->mEta[ia];
					//	for(int igtrk = 0 ; igtrk<Eve->mglobalNtrk; igtrk++) {
					//		if((Eve->mid[ia]==Eve->mglobalid[igtrk])&&(fabs(Eve->mFlag[ia]-Eve->mglobalFlag[igtrk])<300)) {
					//			assocgPz=Eve->mglobalPz[igtrk];
					//			assocgEta=Eve->mglobalEta[igtrk];
					//			break;
					//		}
					//	}
					//}

					if(!pair.compare("TPC-FTPC")) {
						assocnhitscut = 5;
						if(fabs(Eta)>=3.8 || fabs(Eta)<=2.8) continue;      // FTPC
					}
					else {
						assocnhitscut = nhitcut;

						if(fabs(Eta)>=1.0)continue;                        // TPC     // 2014.03.31 ly |eta|<1.0
					}


					if(1.0*(assocNHitsFit-1)/assocNHitsPos < 0.51) continue;        // 2014.03.31 ly nHits = nHitsFit - 1
					if(fabs(assocgDca) >= dcacut) continue;      // 2014.03.31 ly dca<3cm

					if( ACharge>0 && assocCharge<=0) continue;       // 2014.04.01 ly
					if( ACharge<0 && assocCharge>=0) continue;       // 2014.04.01 ly
					if((assocNHitsFit-1) < assocnhitscut ) continue;       // 2014.03.31 ly nHits = nHitsFit - 1


					if(Pt<0.15) continue;
					
					if( nopileupmatching && Eve->mBEMCMatchFlag[ia]<=0 && Eve->mBTOFMatchFlag[ia]<=0 ) continue;

					Float_t mdPhi = Phi - HistTrigPhi[ihevt][iht];
					Float_t mdEta = Eta - HistTrigEta[ihevt][iht];

					//Folding 
					if(mdPhi<-pi/2) mdPhi+=2*pi;            
					if(mdPhi>3*pi/2) mdPhi-=2*pi;

					if( fabs(assocgEta-assocpEta)>1) continue;


					Int_t region = 999;
					for(int iptr = 0 ;iptr<NoAssocPt; iptr++) {
						if(Pt<PtHigh[iptr]&&Pt>=PtLow[iptr]) { 
							region = iptr;          // found pt bin

							// fill mix events


							mdeta_mdphi[region]->Fill(mdEta,mdPhi, weight4vz);             // 2014.04.01 ly fill by real/mix ratio
							flag++;
						}
					}


				}// End of Associate loop for mixing event
				if(flag>0) {
					MatchedTriggerVz->Fill(HistVz[ihevt],weight4vz );          // 2014.03.31 ly  Mixed Trigger Particle Vz dist. for all trigger particle in history found to be mix // 2014.04.01 ly 
				}
			}// End of trig loop for mix event
			if(flag>0) {
				HistTimes[ihevt]++;
			}
		}// End of Mix event array



		//record trigger particle for later mixing events
		if(TotTrig<=0) continue;
		//find a spot for current event
		int seat = -1;
		        //cout<<"find seat?"<<endl;	// test
		for(int isp = 0 ;isp<ArrayLength; isp++) {
			//            cout<<isp<<"\t"<<HistTimes[isp]<<"\t"<<HistTotTrig[isp]<<endl;
			//            if( HistTimes[isp]>MaxMatch ) cout<<"history["<<isp<<"] matched enough"<<endl;
			if( (HistTimes[isp]==0&&HistTotTrig[isp]==0) || (HistTimes[isp]>MaxMatch)  ) {
				seat = isp;
				break;
			}
		}
		//        cout<<"seat="<<seat<<endl;
		//overflow protection
		if(seat==-1) {            // overflow, no seat for current event trigger particles to record
			int DelFlag = 0;                        // test if successful delete one bin
			for(int imatch = MaxMatch; imatch >= 0  ; imatch --) { 
				//                cout<<"match = "<<imatch<<endl;
				for(int dary = 0; dary < ArrayLength; dary++) {
					if(HistTimes[dary]==(imatch)) {          // delete the old bin which been matched 2 (MaxMatch-1) times, if not search for 1 times, 0 times
						//                        cout<<dary<<"\t"<<HistTimes[dary]<<endl;
						HistTimes[dary] = MaxMatch+1;            // set as MaxMatch times 
						seat = dary;                                // put in the active event here
						DelFlag=1;                                  // found
						break;
					}
				}
				if(DelFlag==1) break;
			}
			scratchy++;             // how many times delete the old bins to make room for the later one
			//            cout<<"delete "<<seat<<endl;
		}
		        //cout<<"seat = "<<seat<<endl;	//test

		//store info.
		HistTimes[seat] = 1;
		HistMult[seat] = tracks;                // 2014.04.08 ly    match multiplicity now
		HistVz[seat]=Eve->mVz;
		HistVx[seat]=Eve->mVx;
		HistVy[seat]=Eve->mVy;
		HistTotTrig[seat] = TotTrig;
		for(int i = 0; i<TotTrig; i++) {
			HistTrigPhi[seat][i] = TrigPhi[i];
			HistTrigEta[seat][i] = TrigEta[i];
		}


		if(ievt%10000==0)  cout << ievt << endl;
		        //if(ievt>500)  break;      // test
	}//End of event loop


	cout << "Writing Histograms!" << endl;

	//Create output file
	TString outfile;
	TString vzweightname = "";                                  // 2014.04.01 ly
	if(fvzweight->IsOpen()) vzweightname="Vzweighted_";          // 2014.04.01 ly
	
	outfile = Form("%s%spp500_dca%.0fnhit%dVz30_%s%s%s%s%s%s_T%g-%g_%s%d-%dMatch_VzMatch%gMultMatch%g%s_Mix%dx%s.root",dir.Data(),vzweightname.Data(),dcacut,nhitcut,pair.data(),tchargename.Data(),achargename.Data(),tpidname.Data(),ttracktypename.Data(),atracktypename.Data(),TPtLow,TPtHigh,centdefine.data(),MultL,MultH,VzMatch,MultMatch,nopileupmatchname.Data(),MaxMatch,BFieldname.Data());           // 2014.06   ly mix event with the same bfield, ACharge, TCharge, pair, centrality definition, Trigger PID, Trigger track type, Associate track type

	TFile *fout = new TFile(outfile,"recreate");


	cout<<fout->GetName()<<endl;

	fout->cd();

	//Write Histograms
	for(int i =0;i<NoAssocPt;i++)
	{
		deta_dphi[i]->Write();
		mdeta_mdphi[i]->Write();
		EtaDistribution[i]->Write();
		pTDistribution[i]->Write();
	}


	TrigTot->Write();
	TpTDistribution->Write();
	TEtaDistribution->Write();


	EventTriggerVz->Write();
	MatchedTriggerVz->Write();

	// 2014.10.01 ly VzDifference->Write();
	// 2014.10.01 ly VrDifference->Write();			// 2014.09.29	ly mix event Vr matching
	// 2014.10.01 ly VxDifference->Write();			// 2014.09.29	ly mix event Vr matching
	// 2014.10.01 ly VyDifference->Write();			// 2014.09.29	ly mix event Vr matching

	BBCvsrefMult->Write();
	BBCEvsrefMult->Write();
	BBCWvsrefMult->Write();
	BBCWvsBBCE->Write();

	BBCvsZDC->Write();
	BBCEvsZDCE->Write();
	BBCWvsZDCW->Write();

	refMultvsNtrk->Write();
	refMultvsNPrimaryTrk->Write();
	refMultvsNGlobalTrk->Write();


	ZDCvsrefMult->Write();
	ZDCvsNtrk->Write();
	ZDCEvsrefMult->Write();
	ZDCEvsNtrk->Write();
	ZDCWvsrefMult->Write();
	ZDCWvsNtrk->Write();
	ZDCWvsZDCE->Write();


	fout->Close();

	cout<<"Times delete events without mix "<<MaxMatch<<" x: "<<scratchy<<endl;


	vector<int> timesummary (MaxMatch+3,0);
	for(int i = 0; i<ArrayLength; i++) {
		//        cout<<i<<"\t"<<HistTimes[i]<<"\t"<<HistTotTrig[i]<<endl;
		if(HistTotTrig[i]!=0) {
			if(HistTimes[i]<=MaxMatch+1 && HistTimes[i]>=0) {
				timesummary[HistTimes[i]]++;
			}
			else {
				timesummary[MaxMatch+2]++;              // overflow or underflow
			}
		}
	}
	cout<<endl;
	cout<<"Total Trig Particle = "<<nTrigProcessed<<endl;
	for(int ib = 0; ib<MaxMatch+2; ib++) {
		cout<<timesummary[ib]<<" events mixed "<<ib<<" times in buffer"<<endl;
	}
	if(timesummary[MaxMatch+2]) cout<<" and addtional "<<timesummary[MaxMatch+2]<<" are overflow or underflow mixed"<<endl;
	cout<<endl;


	//End and write out calculation time
	timer.Stop();
	cout<<"Good bye : " <<"\t"<<"RealTime : "<<timer.RealTime()/60.<<" mins\t"<<"CPU time : "<<"\t"<<timer.CpuTime()/60.<<" mins"<<endl;

}



