//===============================================================================
//
//
//	2015.09.30	Li Yi
//	modifed to pythia from data 
//	only K, proton, pion used. 
//
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
//	2014.09.25	For primary and global match (to cut large eta/pz change from global to primary), check Flag for which detector. 
//	2014.09.25	Change ZDC match for 10 ADC difference to 5 ADC
//
//
//	2014.09.29	Mix event: require sqrt(DeltaVx*DeltaVx+DeltaVy*DeltaVy) difference < 0.1
//	2014.09.30	Mix event: require sqrt(DeltaVx*DeltaVx+DeltaVy*DeltaVy) difference < 0.05
//	2014.10.01	The above mix event requirement for Vr doesn't work
//
//===============================================================================




#include "/home/hep/caines/ly247/pp500/include/pythia510tree_150929.C"	
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

float GetEta(float px, float py, float pz) {
	float p = sqrt(px*px+py*py+pz*pz);
	float eta = log((p+pz)/(p-pz))/2.;
	return eta;
}

float GetPhi(float px, float py) {
	float phi = atan2(px,py);
	return phi;
}

float GetPt(float px, float py) {
	float pt = sqrt(px*px+py*py);
	return pt;
}


//==============================  Main Function ============================== 
void deta_dphi_pythia(Int_t TPtNo=6, Int_t MultL = 2, Int_t MultH = 5000, string Trig="HardFSRoff", string pair="TPC-TPC", string centdefine = "TPC", int ACharge = 0, int TCharge = 0, string APID="All", string TPID="All" )          
//  INPUT:
//
// pair: TPC-TPC
//
// Trig: MB
// 	 Hard
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
{
	timer.Start();

	// 2014.04.01 ly
	// check input
	if((centdefine.compare("TPC"))) {                    // no match
		cout<<"ERR!! input centdefine should be TPC"<<endl;
		cout<<"Anyway... I will use TPC as default centrality"<<endl;
		centdefine="TPC";
	}
	cout<<"Centrality definition: "<<centdefine<<endl;

	if((pair.compare("TPC-TPC"))) {         // no match
		cout<<"ERR!! input pair should be TPC-TPC" <<endl;
		cout<<"BUT... I will use TPC-TPC anyway as default pair"<<endl;
		pair="TPC-TPC";
	}

	cout<<"Trigger particle charge: "<<TCharge<<endl;
	cout<<"Associate particle charge: "<<ACharge<<endl;

	cout<<"Particle PID? "<<TPID<<"-"<<APID<<endl;




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


	TChain *chain = new TChain("genevents");
	chain->Add(Form("/home/hep/caines/ly247/Scratch/pythia_pp510tree_150929/*%s*root",Trig.data()));


	//Instantiate both the tree reading progs:
	pythia510tree *Eve = new pythia510tree(chain);


	//Initial histograms
	TH2D *deta_dphi[NoAssocPt];                   // real events
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
		EtaDistribution[i]=new TH1D(Form("EtaDistribution_pT%d",i),Form("Eta Distribution for pT %.2f-%.2f ",PtLow[i],PtHigh[i]),etabinning,etamin,etamax);
		pTDistribution[i]=new TH1D(Form("pTDistribution_pT%d",i),Form("pT Distribution for pT %.2f-%.2f ",PtLow[i],PtHigh[i]),2000,0,20);
		deta_dphi[i]->Sumw2();
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

	//tirgger particle info 
	const int MaxT = 100;                 // max number of trigger particle in one events

	// histogram for event info.

	int maxntrack = 200;

	TH1D *hrefMult=new TH1D("refMult","TPC refMult vs TPC Ntrk",maxntrack,0,maxntrack);
	hrefMult->Sumw2();
	TH1D *hTpcNtracks=new TH1D("TpcNtracks","TPC Ntracks",maxntrack,0,maxntrack);
	hTpcNtracks->Sumw2();

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
	TString dir=Form("/home/hep/caines/ly247/Scratch/pp510pythiatreeCorr_150929/%sCent_%s%s%s_150929/",centdefine.data(),pair.data(),tchargename.Data(),achargename.Data());         // 2014.04.04 ly


	// read in events
	Int_t nEvents = chain->GetEntries();
	cout<<"# of Events :"  <<nEvents<<endl;
	Int_t nTrigProcessed = 0;
	//Loop over Events
	for(int ievt=0;ievt<nEvents;ievt++)
	{

		Double_t Phi=0;
		Double_t Pt=0;
		Double_t Eta=0;
		Int_t TotTrig=0;
		Double_t   TrigPt[MaxT]={0};
		Double_t   TrigPhi[MaxT]={0};
		Double_t   TrigEta[MaxT]={0};

		chain->GetEvent(ievt);

		int Ntrk=Eve->mParticles_;


		//============== Centrality requirement =============
		int tracks = 0;                     // note 2014.04.08 ly, will used for mix matching later
		int refmult = 0;
		if(!centdefine.compare("TPC")) {
			for(int ii = 0; ii<Ntrk ; ii++) {
				if(!(fabs(Eve->mParticles_mStatus[ii])==1)) continue;		// final status particle only 
				if(!(fabs(Eve->mParticles_mId[ii])==211||fabs(Eve->mParticles_mId[ii])==321||fabs(Eve->mParticles_mId[ii])==2212)) continue;		// pion, kaon, proton
				float eta = GetEta(Eve->mParticles_mPx[ii],Eve->mParticles_mPy[ii],Eve->mParticles_mPz[ii]);
				if(fabs(eta)<=1) tracks++; 
				if(fabs(eta)<=0.5) refmult++; 
			}
		}

		if(tracks<MultL||tracks>MultH) continue;         // 2014.03.31 ly. so [MultL, MultH] now

		//============== record event info.
		hrefMult->Fill(refmult);
		hTpcNtracks->Fill(tracks);			// here assuming centrality definition is TPC

		//=========================== real events
		Int_t trigNtrk;
		//if(!Ttracktype.compare("global")) {
		//	trigNtrk = Eve->mglobalNtrk;
		//}
		//else {
			trigNtrk = Eve->mParticles_;
		//}	
		for(int ma=0;ma<trigNtrk;ma++)
		{
			//cout<<"trig "<<trigNtrk<<endl;

			Double_t trigCharge, triggPz=999, trigpPz=999, triggEta=999, trigpEta=999;	
			Float_t trigpx, trigpy, trigpz;

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
				if(!(fabs(Eve->mParticles_mStatus[ma])==1)) continue;		// final status particle only 
				if(!(fabs(Eve->mParticles_mId[ma])==211||fabs(Eve->mParticles_mId[ma])==321||fabs(Eve->mParticles_mId[ma])==2212)) continue;		// pion, kaon, proton

				if(Eve->mParticles_mId[ma]>0) trigCharge = 1;		// already select pi, K, p
				else trigCharge = -1;
				trigpx = Eve->mParticles_mPx[ma];
				trigpy = Eve->mParticles_mPy[ma];
				trigpz = Eve->mParticles_mPz[ma];

				TrigPt[TotTrig] = GetPt(trigpx,trigpy);
				TrigPhi[TotTrig] = GetPhi(trigpx,trigpy);
				TrigEta[TotTrig] = GetEta(trigpx,trigpy,trigpz);
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
				assocNtrk = Eve->mParticles_;
			//}

			//2014.06.19 ly for(int n=0;n<Ntrk;n++)
			for(int n=0;n<assocNtrk;n++)	// 2014.06.19 ly
			{
				if( (n==ma) && (!pair.compare("TPC-TPC")) )  continue;			
				Double_t assocCharge, assocgPz = 999, assocpPz = 999, assocgEta = 999, assocpEta = 999;
				Float_t assocpx, assocpy, assocpz;

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
					if(!(fabs(Eve->mParticles_mStatus[n])==1)) continue;		// final status particle only 
					if(!(fabs(Eve->mParticles_mId[n])==211||fabs(Eve->mParticles_mId[n])==321||fabs(Eve->mParticles_mId[n])==2212)) continue;		// pion, kaon, proton

					if(Eve->mParticles_mId[n]>0) assocCharge = 1;		// already select pi, K, p
					else assocCharge = -1;
					assocpx = Eve->mParticles_mPx[n];
					assocpy = Eve->mParticles_mPy[n];
					assocpz = Eve->mParticles_mPz[n];

					Pt=GetPt(assocpx,assocpy);
					Phi=GetPhi(assocpx,assocpy);
					Eta=GetEta(assocpx,assocpy,assocpz);
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
					if(fabs(Eta)>=3.8 || fabs(Eta)<=2.8) continue;      // FTPC
				}
				else {
					if(fabs(Eta)>=1.0)continue;                        // TPC     // 2014.03.31 ly |eta|<1.0
				}

				if( ACharge>0 && assocCharge<=0) continue;       // 2014.04.01 ly
				if( ACharge<0 && assocCharge>=0) continue;       // 2014.04.01 ly


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
		nTrigProcessed+=TotTrig;

		//cout<<"Number of Trig = "<<TotTrig<<" mix event --- "<<ievt<<endl;		// test


		if(ievt%10000==0)  cout << ievt << endl;
		        //if(ievt>500)  break;      // test
	}//End of event loop


	cout << "Writing Histograms!" << endl;

	//Create output file
	TString outfile;
	
	outfile = Form("%spp500%s_PiKp_%s%s%s%s_T%g-%g_%s%d-%d.root",dir.Data(),Trig.data(),pair.data(),tchargename.Data(),achargename.Data(),tpidname.Data(),TPtLow,TPtHigh,centdefine.data(),MultL,MultH);           

	TFile *fout = new TFile(outfile,"recreate");


	cout<<fout->GetName()<<endl;

	fout->cd();

	//Write Histograms
	for(int i =0;i<NoAssocPt;i++)
	{
		deta_dphi[i]->Write();
		EtaDistribution[i]->Write();
		pTDistribution[i]->Write();
	}


	TrigTot->Write();
	TpTDistribution->Write();
	TEtaDistribution->Write();

	hrefMult->Write();
	hTpcNtracks->Write();

	fout->Close();

	cout<<"Total Trig Particle = "<<nTrigProcessed<<endl;

	//End and write out calculation time
	timer.Stop();
	cout<<"Good bye : " <<"\t"<<"RealTime : "<<timer.RealTime()/60.<<" mins\t"<<"CPU time : "<<"\t"<<timer.CpuTime()/60.<<" mins"<<endl;

}



