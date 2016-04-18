//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Sep 29 14:29:14 2015 by ROOT version 5.30/01
// from TTree pp12tree/RCF data Tree 2012 pp
// found on file: /home/hep/caines/ly247/Scratch/pp12tree_500GeV_150928/pp500Y12treeHighMult_150928.root
//////////////////////////////////////////////////////////

#ifndef pp12tree_h
#define pp12tree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class pp12tree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           mRunid;
   Int_t           mEventid;
   Float_t         mMagField;
   UShort_t        mTOFMult;
   Float_t         mVx;
   Float_t         mVy;
   Float_t         mVz;
   Float_t         mVPDVz;
   UShort_t        mbtofTrayMultiplicity;
   UShort_t        mbtofrawTrayMultiplicity;
   UShort_t        mbtofMatchPrimaryCount;
   UShort_t        mrefMult;
   UShort_t        mrefMultNeg;
   UShort_t        mrefMultPos;
   UInt_t          mNumberOfPrimaryVertices;
   Float_t         mNearestVz;
   UInt_t          mNumberOfPrimaryTracks;
   UInt_t          mNumberOfGlobalTracks;
   Int_t           mNtrk;
   Int_t           mNPos;
   Int_t           mNNeg;
   Float_t         mVertexRanking;
   UShort_t        mCTBMatch;
   UShort_t        mBTOFMatch;
   UShort_t        mBEMCMatch;
   UShort_t        mEEMCMatch;
   Float_t         mZDCAdc[16];
   Float_t         mZDCCointcidenceRate;
   UShort_t        mBBCEAdc[24];
   UShort_t        mBBCWAdc[24];
   UShort_t        mBBCETdc[24];
   UShort_t        mBBCWTdc[24];
   Float_t         mBBCCointcidenceRate;
   Short_t         mFlag[500];   //[mNtrk]
   Short_t         mid[500];   //[mNtrk]
   Float_t         mPt[500];   //[mNtrk]
   Float_t         mPx[500];   //[mNtrk]
   Float_t         mPy[500];   //[mNtrk]
   Float_t         mPz[500];   //[mNtrk]
   Float_t         mEta[500];   //[mNtrk]
   Float_t         mPhi[500];   //[mNtrk]
   Short_t         mCharge[500];   //[mNtrk]
   UShort_t        mNHitsPos[500];   //[mNtrk]
   UShort_t        mNHitsFit[500];   //[mNtrk]
   Float_t         mgDca[500];   //[mNtrk]
   Float_t         mgDcaD[500];   //[mNtrk]
   Float_t         mChi2prob[500];   //[mNtrk]
   Float_t         mChi2[500];   //[mNtrk]
   Double_t        mdEdx[500];   //[mNtrk]
   UShort_t        mNHitsdEdx[500];   //[mNtrk]
   Double_t        mNSigmaPion[500];   //[mNtrk]
   Double_t        mNSigmaKaon[500];   //[mNtrk]
   Double_t        mNSigmaProton[500];   //[mNtrk]
   UChar_t         mBEMCMatchFlag[500];   //[mNtrk]
   UChar_t         mBTOFMatchFlag[500];   //[mNtrk]
   Int_t           mNtrig;
   Int_t           mTrigId[20];   //[mNtrig]

   // List of branches
   TBranch        *b_mRunid;   //!
   TBranch        *b_mEventid;   //!
   TBranch        *b_mMagField;   //!
   TBranch        *b_mTOFMult;   //!
   TBranch        *b_mVx;   //!
   TBranch        *b_mVy;   //!
   TBranch        *b_mVz;   //!
   TBranch        *b_mVPDVz;   //!
   TBranch        *b_mbtofTrayMultiplicity;   //!
   TBranch        *b_mbtofrawTrayMultiplicity;   //!
   TBranch        *b_mbtofMatchPrimaryCount;   //!
   TBranch        *b_mrefMult;   //!
   TBranch        *b_mrefMultNeg;   //!
   TBranch        *b_mrefMultPos;   //!
   TBranch        *b_mNumberOfPrimaryVertices;   //!
   TBranch        *b_mNearestVz;   //!
   TBranch        *b_mNumberOfPrimaryTracks;   //!
   TBranch        *b_mNumberOfGlobalTracks;   //!
   TBranch        *b_mNtrk;   //!
   TBranch        *b_mNPos;   //!
   TBranch        *b_mNNeg;   //!
   TBranch        *b_mVertexRanking;   //!
   TBranch        *b_mCTBMatch;   //!
   TBranch        *b_mBTOFMatch;   //!
   TBranch        *b_mBEMCMatch;   //!
   TBranch        *b_mEEMCMatch;   //!
   TBranch        *b_mZDCAdc;   //!
   TBranch        *b_mZDCCointcidenceRate;   //!
   TBranch        *b_mBBCEAdc;   //!
   TBranch        *b_mBBCWAdc;   //!
   TBranch        *b_mBBCETdc;   //!
   TBranch        *b_mBBCWTdc;   //!
   TBranch        *b_mBBCCointcidenceRate;   //!
   TBranch        *b_mFlag;   //!
   TBranch        *b_mid;   //!
   TBranch        *b_mPt;   //!
   TBranch        *b_mPx;   //!
   TBranch        *b_mPy;   //!
   TBranch        *b_mPz;   //!
   TBranch        *b_mEta;   //!
   TBranch        *b_mPhi;   //!
   TBranch        *b_mCharge;   //!
   TBranch        *b_mNHitsPos;   //!
   TBranch        *b_mNHitsFit;   //!
   TBranch        *b_mgDca;   //!
   TBranch        *b_mgDcaD;   //!
   TBranch        *b_mChi2prob;   //!
   TBranch        *b_mChi2;   //!
   TBranch        *b_mdEdx;   //!
   TBranch        *b_mNHitsdEdx;   //!
   TBranch        *b_mNSigmaPion;   //!
   TBranch        *b_mNSigmaKaon;   //!
   TBranch        *b_mNSigmaProton;   //!
   TBranch        *b_mBEMCMatchFlag;   //!
   TBranch        *b_mBTOFMatchFlag;   //!
   TBranch        *b_mNtrig;   //!
   TBranch        *b_mTrigId;   //!

   pp12tree(TTree *tree=0);
   virtual ~pp12tree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef pp12tree_cxx
pp12tree::pp12tree(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/hep/caines/ly247/Scratch/pp12tree_500GeV_150928/pp500Y12treeHighMult_150928.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/home/hep/caines/ly247/Scratch/pp12tree_500GeV_150928/pp500Y12treeHighMult_150928.root");
      }
      f->GetObject("pp12tree",tree);

   }
   Init(tree);
}

pp12tree::~pp12tree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t pp12tree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t pp12tree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void pp12tree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("mRunid", &mRunid, &b_mRunid);
   fChain->SetBranchAddress("mEventid", &mEventid, &b_mEventid);
   fChain->SetBranchAddress("mMagField", &mMagField, &b_mMagField);
   fChain->SetBranchAddress("mTOFMult", &mTOFMult, &b_mTOFMult);
   fChain->SetBranchAddress("mVx", &mVx, &b_mVx);
   fChain->SetBranchAddress("mVy", &mVy, &b_mVy);
   fChain->SetBranchAddress("mVz", &mVz, &b_mVz);
   fChain->SetBranchAddress("mVPDVz", &mVPDVz, &b_mVPDVz);
   fChain->SetBranchAddress("mbtofTrayMultiplicity", &mbtofTrayMultiplicity, &b_mbtofTrayMultiplicity);
   fChain->SetBranchAddress("mbtofrawTrayMultiplicity", &mbtofrawTrayMultiplicity, &b_mbtofrawTrayMultiplicity);
   fChain->SetBranchAddress("mbtofMatchPrimaryCount", &mbtofMatchPrimaryCount, &b_mbtofMatchPrimaryCount);
   fChain->SetBranchAddress("mrefMult", &mrefMult, &b_mrefMult);
   fChain->SetBranchAddress("mrefMultNeg", &mrefMultNeg, &b_mrefMultNeg);
   fChain->SetBranchAddress("mrefMultPos", &mrefMultPos, &b_mrefMultPos);
   fChain->SetBranchAddress("mNumberOfPrimaryVertices", &mNumberOfPrimaryVertices, &b_mNumberOfPrimaryVertices);
   fChain->SetBranchAddress("mNearestVz", &mNearestVz, &b_mNearestVz);
   fChain->SetBranchAddress("mNumberOfPrimaryTracks", &mNumberOfPrimaryTracks, &b_mNumberOfPrimaryTracks);
   fChain->SetBranchAddress("mNumberOfGlobalTracks", &mNumberOfGlobalTracks, &b_mNumberOfGlobalTracks);
   fChain->SetBranchAddress("mNtrk", &mNtrk, &b_mNtrk);
   fChain->SetBranchAddress("mNPos", &mNPos, &b_mNPos);
   fChain->SetBranchAddress("mNNeg", &mNNeg, &b_mNNeg);
   fChain->SetBranchAddress("mVertexRanking", &mVertexRanking, &b_mVertexRanking);
   fChain->SetBranchAddress("mCTBMatch", &mCTBMatch, &b_mCTBMatch);
   fChain->SetBranchAddress("mBTOFMatch", &mBTOFMatch, &b_mBTOFMatch);
   fChain->SetBranchAddress("mBEMCMatch", &mBEMCMatch, &b_mBEMCMatch);
   fChain->SetBranchAddress("mEEMCMatch", &mEEMCMatch, &b_mEEMCMatch);
   fChain->SetBranchAddress("mZDCAdc", mZDCAdc, &b_mZDCAdc);
   fChain->SetBranchAddress("mZDCCointcidenceRate", &mZDCCointcidenceRate, &b_mZDCCointcidenceRate);
   fChain->SetBranchAddress("mBBCEAdc", mBBCEAdc, &b_mBBCEAdc);
   fChain->SetBranchAddress("mBBCWAdc", mBBCWAdc, &b_mBBCWAdc);
   fChain->SetBranchAddress("mBBCETdc", mBBCETdc, &b_mBBCETdc);
   fChain->SetBranchAddress("mBBCWTdc", mBBCWTdc, &b_mBBCWTdc);
   fChain->SetBranchAddress("mBBCCointcidenceRate", &mBBCCointcidenceRate, &b_mBBCCointcidenceRate);
   fChain->SetBranchAddress("mFlag", mFlag, &b_mFlag);
   fChain->SetBranchAddress("mid", mid, &b_mid);
   fChain->SetBranchAddress("mPt", mPt, &b_mPt);
   fChain->SetBranchAddress("mPx", mPx, &b_mPx);
   fChain->SetBranchAddress("mPy", mPy, &b_mPy);
   fChain->SetBranchAddress("mPz", mPz, &b_mPz);
   fChain->SetBranchAddress("mEta", mEta, &b_mEta);
   fChain->SetBranchAddress("mPhi", mPhi, &b_mPhi);
   fChain->SetBranchAddress("mCharge", mCharge, &b_mCharge);
   fChain->SetBranchAddress("mNHitsPos", mNHitsPos, &b_mNHitsPos);
   fChain->SetBranchAddress("mNHitsFit", mNHitsFit, &b_mNHitsFit);
   fChain->SetBranchAddress("mgDca", mgDca, &b_mgDca);
   fChain->SetBranchAddress("mgDcaD", mgDcaD, &b_mgDcaD);
   fChain->SetBranchAddress("mChi2prob", mChi2prob, &b_mChi2prob);
   fChain->SetBranchAddress("mChi2", mChi2, &b_mChi2);
   fChain->SetBranchAddress("mdEdx", mdEdx, &b_mdEdx);
   fChain->SetBranchAddress("mNHitsdEdx", mNHitsdEdx, &b_mNHitsdEdx);
   fChain->SetBranchAddress("mNSigmaPion", mNSigmaPion, &b_mNSigmaPion);
   fChain->SetBranchAddress("mNSigmaKaon", mNSigmaKaon, &b_mNSigmaKaon);
   fChain->SetBranchAddress("mNSigmaProton", mNSigmaProton, &b_mNSigmaProton);
   fChain->SetBranchAddress("mBEMCMatchFlag", mBEMCMatchFlag, &b_mBEMCMatchFlag);
   fChain->SetBranchAddress("mBTOFMatchFlag", mBTOFMatchFlag, &b_mBTOFMatchFlag);
   fChain->SetBranchAddress("mNtrig", &mNtrig, &b_mNtrig);
   fChain->SetBranchAddress("mTrigId", mTrigId, &b_mTrigId);
   Notify();
}

Bool_t pp12tree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void pp12tree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t pp12tree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef pp12tree_cxx
