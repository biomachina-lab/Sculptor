/***************************************************************************
                          svt_stride.h
                          ---------
    begin                : 10/26/2009
    author               : Zbigniew Starosolski
    email                : sculptor@biomachina.org

   integration program STRIDE by Dmitrij Frishman and Patrick Argos
   with Sculptor environment

 ***************************************************************************/

#ifndef _SVT_STRIDE_H
#define _SVT_STRIDE_H


#include <svt_system.h>
#include <sseindexer.h>
#include <nsc.h>
#include <svt_cmath.h>

#include <stdarg.h>               // needs: nsc , die, error, escape

#include <svt_stlVector.h>

#ifdef MACOSX
#include <CFURL.h>
#endif

/**
 * A class for calculation secondary structure information based on
 *  program STRIDE by Dmitrij Frishman and Patrick Argos
 *
 *@Zbigniew Starosolski
 */

class svt_stride
{

private:
// protot.h
BOOLEAN ChInStr(char *String, char Char);
BOOLEAN ExistsSecStr(CHAIN **Chain, int NChain);
BOOLEAN ExistSSBond(CHAIN **Chain,int NChain,int Cn1,int Cn2,char *Res1,char *Res2);
BOOLEAN IsHydrogen(char *AtomName);
BOOLEAN Specified(char **List, int ListLength, char Option);
FILE *efopen(char *file, char *mode);

char **AllocAsn(CHAIN **Chain, int NChain);
char **CharMatrix(int M, int N);
const char *OneToThree(char One);
char SpaceToDash(char Id);
char *Tim(void);
char ThreeToOne(char *Three);
char *tolostr(char *InputString);
const char *Translate(char Code);
double GetAtomRadius(char *AtomType);

float Ang(float *Coord1, float *Coord2, float *Coord3);
float **DefaultHelixMap(COMMAND *Cmd);
float **DefaultSheetMap(COMMAND *Cmd);
float Dist(float *Coord1, float *Coord2);
float ****Float4Dim(int M, int N, int K, int L);
float ***FloatCube(int M, int N, int K);
float **FloatMatrix(int M, int N);
float PercentCorrect(char *TestAsn, char *KnownAsn, int Length);
float SecStrContent(CHAIN *Chain, int *HelAlp, int *HelPI, int *Hel310, int *Sheet, int *Turn);
float Torsion(float *Coord1, float *Coord2, float *Coord3, float *Coord4);
float VectorProduct(float *Vector1, float *Vector2, float *Product);
float factrl(int n);

int AssessCorr(QUALITY *Qual);
int AssessPerc(QUALITY *Qual);
int Boundaries(char *Asn, int L, char SecondStr, int (*Bound)[2]);

int CheckAtom(char *At);
int CheckChain(CHAIN *Chain, COMMAND *Cmd);
int CheckRes(char *Res);
int CompareElements(char *Asn1, char *Asn2, char *Asn3, int Length,
		   char SecStrType, int *Better, int *Worse);
int CompPdbDssp(CHAIN *Chain, DSSP *Dssp);
int CollectOptions(char **List, int ListLength, int Stream, int *Options);
int DefineAcceptor(CHAIN *Chain, ACCEPTOR **Acc, int *ac, int Res, enum HYBRID Hybrid,
		   enum MGROUP Group, float HB_Radius, int N);
int DefineDnr(CHAIN *Chain, DONOR **Dnr, int *dc, int Res, enum HYBRID Hybrid,
	      enum MGROUP Group,  float HB_Radius, int N);
int Delete(char *String, char From);
int Difference(char *TestAsn, char *KnownAsn, int Length, char SecStrType, QUALITY *Qual);
int escape(int RetVal, const char *format, ... );
int FindAcc(CHAIN *Chain, ACCEPTOR **Acc, int *NAcc, COMMAND *Cmd);
int FindAtom(CHAIN *Chain, int ResNumb, const char *Atom, int *AtNumb);
int FindBnd(HBOND **HBond, RESIDUE *Res1, RESIDUE *Res2);
int FindChain(CHAIN **Chain, int NChain, char ChainId);
int FindDnr(CHAIN *Chain, DONOR **Dnr, int *NDnr, COMMAND *Cmd);
int FindHydrogenBonds(CHAIN **Chain, int NChain, HBOND **HBond, COMMAND *Cmd);
int FindPolInt(HBOND **HBond, RESIDUE *Res1, RESIDUE *Res2);
int FullElement(char *Asn1, char *Asn2, char *Asn3, int Length, char SecStrType,
		       int ElemLength, char EditChar, int *YYN, int *NYY, int *YNN, int *NNY);
int GetPdbChain(CHAIN **Chain, FILE *Db, long int Start);
int GetDsspAsn(CHAIN **Chain, int NChain, COMMAND *Cmd);
int ****Int4Dim(int M, int N, int K, int L);
int ***IntCube(int M, int N, int K);
int **IntMatrix(int M, int N);
int Link(HBOND **HBond, CHAIN **Chain, int Cn1, int Cn2, RESIDUE *Res1_1,
	 RESIDUE *Res1_2, RESIDUE *Res2_2, RESIDUE *Res2_1, RESIDUE *CRes1, RESIDUE *CRes2,
	 float **PhiPsiMap, MPATTERN **Pattern, int *NumPat, const char *Text, float Treshold,
	 COMMAND *Cmd, int Test);
int MakeEnds(int *Beg1, int ResBeg1, int NeiBeg1, char *Beg1Cn, char ResBeg1Cn,
		    int *End1, int ResEnd1, int NeiEnd1, char ResEnd1Cn, int *Beg2,
		    int ResBeg2, int NeiBeg2, char *Beg2Cn, char ResBeg2Cn, int *End2,
		    int ResEnd2, int NeiEnd2, char ResEnd2Cn, MPATTERN **Pat, int NPat);
int MolScript(CHAIN **Chain, int NChain, COMMAND *Cmd);
int Near(int Res1, int Res2, int Res3, int Res4, int Res5, int Res6, int Res7, int Res8,
	 char Cn1, char Cn2, char Cn3, char Cn4, int *DistBest, int *DistWorst);
int NearPar(int Res1, int Res2, int Res3, int Res4, int Res5, int Res6, int Res7, int Res8,
	 char Cn1, char Cn2, char Cn3, char Cn4, int *DistBest, int *DistWorst);
int NoDoubleHBond(HBOND **HBond, int NHBond);
int NotValid(CHAIN *Chain, const char *Message);
int OutSeq(CHAIN **Chain, int NChain, COMMAND *Cmd);
int Parse(char **List, int ListLength, char *Option);
int PdbN2SeqN(CHAIN *Chain, char *PdbN, int *SeqN);
int PlaceHydrogens(CHAIN *Chain);
int Presnell(char *Asn1, int L1, char *Asn2, int L2, char SecStr, float Threshold,
	       float *Q2, float *O);
int Process_ATOM(CHAIN **Chain, int *ChainNumber,
		 BOOLEAN *First_ATOM, COMMAND *Cmd,  char * rvs_one_line);
int Process_COMPND(BUFFER Buffer, enum METHOD *Method);
int Process_ENDMDL(CHAIN **Chain, int *ChainNumber);
int Process_EXPDTA(BUFFER Buffer, enum METHOD *Method);
int Process_HELIX(BUFFER Buffer, CHAIN **Chain, int *ChainNumber, COMMAND *Cmd);
int Process_JRNL(BUFFER Buffer, BOOLEAN *Published);
int Process_MODEL(enum METHOD *Method);
int Process_REMARK(BUFFER Buffer, enum METHOD *Method, float *Resolution, BOOLEAN *DsspAssigned);
int Process_SHEET(BUFFER Buffer, CHAIN **Chain, int *ChainNumber, COMMAND *Cmd);
int Process_SSBOND(BUFFER Buffer, CHAIN **Chain, int *ChainNumber, COMMAND *Cmd);
int Process_TER(BUFFER Buffer, CHAIN **Chain, int *ChainNumber);
int Process_TURN(BUFFER Buffer, CHAIN **Chain, int *ChainNumber, COMMAND *Cmd);
int ReadDSSP(CHAIN **Chain, DSSP **Dssp, COMMAND *Cmd);
int ReadPDBFile(CHAIN **Chain, int *NChain, COMMAND *Cmd, vector<char *> & rvs);
int ReadPhiPsiMap(char *MapFile, float ***PhiPsiMap, COMMAND *Cmd);
int Replace(char *String, char From, char To);
int ResInSecondStr(int ResNumb, int (*Bound)[2], int N, int *StrNumb);
int RightSide(int LnkA, int LnkD, int I1A, int I1D, int I2A, int I2D );
int RightSide2(int L_A1, int L_D1, int LnkD, int LnkA, int I1A, int I1D, int I2A, int I2D);
int RightSidePar(int LnkA, int LnkD, int I1A, int I1D, int I2A, int I2D );
int Sov(char *Asn1, int L1, char *Asn2, int L2, char SecStr, float Threshold, float *Q2);
int SplitString(char *Buffer, char **Fields, int MaxField);
int SSBond(CHAIN **Chain, int NChain);
int TorsBracket(float Torsion, float Min, float Max);
int TurnCondition(float Phi2,float Phi2S,float Psi2,float Psi2S,float Phi3,float Phi3S,float Psi3,float Psi3S,float Range1,float Range2);
int Uniq(char **List, int ListLength);
void Alias(int *D1,int *A1,int *D2,int *A2,char *D1Cn,char *A1Cn,char *D2Cn,char *A2Cn,MPATTERN *Pat);
void AllocChain(CHAIN **Chain);
void Area(CHAIN **Chain, int NChain);
void BackboneAngles(CHAIN **Chain, int NChain);
void BetaTurn(CHAIN **Chain, int Cn);
void Bridge(char *Asn1, char *Asn2, CHAIN **Chain, int Cn1, int Cn2, MPATTERN **Pat, int NPat);
void *ckalloc(size_t bytes);
void CorrectAsn(char *Asn, int Length, char SecStrType, char EditChar, int MaxLength);
void CorrectAsnDouble(char *Asn1, char *Asn2, char *KnownAsn, int Length,
		      char SecStrType, char EditChar);
void DeallocAcc(DONOR **Acc, int AccNumber);
void DeallocDnr(DONOR **Dnr, int DonNumber);
void DefaultCmd(COMMAND *Cmd);
void die(const char *format, ... ) ;
void DiscrPhiPsi(CHAIN **Chain, int NChain, COMMAND *Cmd);
void DistMatrix(CHAIN *Chain);
void DSSP_Energy(float *C, float *O, float *H, float *N, HBOND *HBond);
void ExcludeObvious(char *Asn1, char *Asn2, char *KnownAsn, int Length);
void ExtractAsn(CHAIN **Chain, int Cn, char *Asn);
void ExtractPdbAsn(CHAIN **Chain, int Cn, char *Asn);
void ExtractDsspAsn(CHAIN **Chain, int Cn, char *Asn);
void FillAsnAntiPar(char *Asn1, char *Asn2, CHAIN **Chain, int Cn1, int Cn2,
		    MPATTERN **Pat, int NPat, COMMAND *Cmd);
void FillAsnPar(char *Asn1, char *Asn2, CHAIN **Chain, int Cn1, int Cn2,
		MPATTERN **Pat, int NPat, COMMAND *Cmd);
void FilterAntiPar(MPATTERN **Pat, int NPat);
void FilterPar(MPATTERN **Pat, int NPat);
void FreeCharMatrix(char **Matrix, int M);
void FreeFloatMatrix(float **Matrix, int M);
void FreeIntMatrix(int **Matrix, int M);
void FreeIntCube(int ***Cube, int M, int N);
void FreeFloatCube(float ***Cube, int M, int N);
void GammaTurn(CHAIN **Chain, int Cn, HBOND **HBond);
void GetFileNameFromPath(char *Path, char *FileName);
void GetPdbAsn(CHAIN **Chain, int NChain);
void Glue(const char *String1, const char *String2, vector<char *> & vectorRaportString);
void GRID_Energy(float *CA2, float *C, float *O, float *H, float *N, COMMAND *Cmd, HBOND *HBond);
void Helix(CHAIN **Chain, int Cn, HBOND **HBond, COMMAND *Cmd, float **PhiPsiMap);
void HBondToBins(HBOND **HBond, int NHBond, COMMAND *Cmd);
void InitAsn(CHAIN **Chain, int NChain);
void InitChain(CHAIN **Chain);

void FreeChain(CHAIN *Chain);

void InsertFirst(DSSP *Dssp, CHAIN *Chain);
void InsertLast(DSSP *Dssp, CHAIN *Chain);
void JoinNeighb(MPATTERN **Nei, MPATTERN *Pat, int *MinDB2, int DB, int *MinDW2, int DW);
void JoinNeighbours(int *Lnk1A, int Res1, int *Lnk1D, int Res2, MPATTERN **Nei,
		    MPATTERN *Pat, int *MinDB1, int DB, int *MinDW1, int DW, int *Min, int j);
void Measure(CHAIN **Chain, int NChain, int El, FILE *Out);
void MergePatternsPar(MPATTERN **Pat, int NPat);
void MergePatternsAntiPar(MPATTERN **Pat, int NPat);

void OMEGA(CHAIN *Chain, int Res);
void Place123_X(float *Coord1, float *Coord2, float *Coord3, float Dist3X, float Ang23X,
		float *CoordX);
void PrepareBuffer(BUFFER Bf, CHAIN **Chain);
void PrintHydrBond(char *Text, HBOND *HBond);
void PrintPatterns(MPATTERN **Pat, int NPat, CHAIN **Chain, int Cn1, int Cn2);
void PrintStrideHelp(COMMAND *Cmd);
void ProcessStrideOptions(COMMAND *Cmd);
void Project4_123(float *Coord1, float *Coord2, float *Coord3, float *Coord4,
		  float *Coord_Proj4_123);
void PHI(CHAIN *Chain, int Res);
void PSI(CHAIN *Chain, int Res);
void Report(CHAIN **Chain, int NChain, HBOND **HBond, COMMAND *Cmd,vector<char *> & vectorRaportString);
void ReportDetailed(CHAIN **Chain, int NChain, COMMAND *Cmd,vector<char *> & vectorRaportString);
void ReportGeneral(CHAIN **Chain, vector<char *> & vectorRaportString);
void ReportHydrBonds(CHAIN **Chain, int NChain, HBOND **HBond,
		vector<char *> & vectorRaportString, COMMAND *Cmd);
void ReportShort(CHAIN **Chain, int NChain, vector<char *> & vectorRaportString);
void ReportSSBonds(CHAIN **Chain,vector<char *> & vectorRaportString);
void ReportSummary(CHAIN **Chain, int NChain, vector<char *> & vectorRaportString, COMMAND *Cmd);
void ReportTurnTypes(CHAIN **Chain, int NChain, vector<char *> & vectorRaportString);
void Sheet(CHAIN **Chain, int Cn1, int Cn2, HBOND **HBond, COMMAND *Cmd, float **PhiPsiMap);
void StringSort(char **Strings, int left, int right, int StrLen);
void StripPathFromLastExtention(char *Path, char *StrippedPath);

// from NSC program

int nsc_dclm(double *,double *, int, int, int ,double * , double ** , double * ,double ** ,int *);

int ico_dot_arc(int densit);
int ico_dot_dod(int densit);
int unsp_type(int densit);
int make_unsp(int densit, int mode, int * num_dot, int cubus);

double safe_asin(double f);

void * mycalloc(const char * filename, const int linenr, size_t nelem, size_t elsize);
void * myrealloc(const char * filename, const int linenr,void * ptr, size_t size);
void icosaeder_vertices(double *xus);
void divarc(double x1, double y1, double z1, double x2, double y2, double z2, int div1, int div2, double *xr, double *yr, double *zr);
void error(const char *fmt, ...);
void warning(const char *fmt, ...);

void nscmain (void);

/**
 * Constructor
 */
//svt_stride();
/**
* Destructor
*/
//~svt_stride();

public:

bool sseindexer(vector<char *> & rvs, vector<char *> & vectorRaportString);

};

#endif

