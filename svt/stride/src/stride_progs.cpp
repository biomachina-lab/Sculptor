#include <svt_stride.h>

DONOR **Dnr = NULL;
ACCEPTOR **Acc = NULL;
int NDnr=0, NAcc=0;

void svt_stride::Area(CHAIN **Chain, int NChain)
{

    double *Coord, *Radii, OverallArea, *AreaPerAtom, *p1, *p2;
    int At, TotalAt=0, Cn, Res, DotsPerSphere=600;
    AreaPerAtom = NULL;

    for( Cn=0; Cn<NChain; Cn++ )
    {
        if( !Chain[Cn]->Valid )
            continue;

        for( Res=0; Res<Chain[Cn]->NRes; Res++ )
            for( At=0; At<Chain[Cn]->Rsd[Res]->NAtom; At++ )
                if( !IsHydrogen(Chain[Cn]->Rsd[Res]->AtomType[At]) )
                    TotalAt ++;
    }

    Coord = (double *)ckalloc(3*TotalAt*sizeof(double));
    Radii = (double *)ckalloc(TotalAt*sizeof(double));

    p1 = Coord;
    p2 = Radii;

    for( Cn=0; Cn<NChain; Cn++ )
    {
        if( !Chain[Cn]->Valid )
            continue;

        for( Res=0; Res<Chain[Cn]->NRes; Res++ )
            for( At=0; At<Chain[Cn]->Rsd[Res]->NAtom; At++ )
                if( !IsHydrogen(Chain[Cn]->Rsd[Res]->AtomType[At]) ) {
                    (*p1++) = (double)Chain[Cn]->Rsd[Res]->Coord[At][0];
                    (*p1++) = (double)Chain[Cn]->Rsd[Res]->Coord[At][1];
                    (*p1++) = (double)Chain[Cn]->Rsd[Res]->Coord[At][2];
                    (*p2++) = GetAtomRadius(Chain[Cn]->Rsd[Res]->AtomType[At])+1.4;
                }

    }

    p1 = Coord;
    p2 = Radii;

    NSC(Coord,Radii,TotalAt,DotsPerSphere,FLAG_ATOM_AREA,&OverallArea,&AreaPerAtom,NULL,NULL,NULL);
    double* OrigAreaPerAtom = AreaPerAtom;

    for( Cn=0; Cn<NChain; Cn++ )
    {
        if( !Chain[Cn]->Valid )
            continue;

        for( Res=0; Res<Chain[Cn]->NRes; Res++ )
        {
            Chain[Cn]->Rsd[Res]->Prop->Solv = 0.0;
            for( At=0; At<Chain[Cn]->Rsd[Res]->NAtom; At++ )
                if( !IsHydrogen(Chain[Cn]->Rsd[Res]->AtomType[At]) )
                {
                    Chain[Cn]->Rsd[Res]->Prop->Solv += (*AreaPerAtom++);
                }
        }
    }

    if (OrigAreaPerAtom)
        free(OrigAreaPerAtom);
    free(Coord);
    free(Radii);
}

/*************************************************************************
**                                                                      **
** Remove short stretches of secondary structure from the assignment    **
**                                                                      **
** INPUT:   *Asn       String with one letter secondary structure       **
**                     assignment                                       **
**          Length     Length of the string                             **
**          SecStrType Type of the secondary structure to which this    **
**                     operation should be applied                      **
**          EditChar   Character to be used instead of removed symbols  **
**          MaxLength  Maximal length of secondary struture segments to **
**                     be removed                                       **
**                                                                      **
** OUTPUT:  *Asn       Edited secondary structure assignment            **
**                                                                      **
*************************************************************************/
void svt_stride::CorrectAsn(char *Asn, int Length, char SecStrType, char EditChar, int MaxLength)
{

  int NStr = 0, Res, Flag = 0, Bound[MAX_ASSIGN][2], i;

  for( Res=0; Res<Length; Res++ ) {
    if( Asn[Res] == SecStrType && Flag == 0 ) {
      Flag = 1;
      Bound[NStr][0] = Res;
    }
    else
    if( Asn[Res] != SecStrType && Flag == 1 ) {
      Flag = 0;
      Bound[NStr++][1] = Res-1;
    }
  }

  for( i=0; i<NStr; i++ )
    if( Bound[i][1]-Bound[i][0]+1 <= MaxLength )
      for( Res=Bound[i][0]; Res<=Bound[i][1]; Res++ )
	Asn[Res] = EditChar;
}

void svt_stride::CorrectAsnDouble(char *Asn1, char *Asn2, char *KnownAsn, int Length,
		      char SecStrType, char EditChar)
{

  register int Res;

  for( Res=0; Res<Length; Res++ )
    if( (Asn1[Res] == SecStrType || Asn2[Res] == SecStrType) && KnownAsn[Res] != SecStrType &&
        ( (Res == 0 && Asn1[Res+1] != SecStrType && Asn2[Res+1] != SecStrType) ||
	  (Res == Length-1 && Asn1[Res-1] != SecStrType && Asn2[Res-1] != SecStrType) ||
          (Res > 0 && Res < Length-1 &&
	   Asn1[Res-1] != SecStrType && Asn2[Res-1] != SecStrType &&
	   Asn1[Res+1] != SecStrType && Asn2[Res+1] != SecStrType) ) )
      Asn1[Res] = Asn2[Res] = EditChar;

}

/*************************************************************************
**                                                                      **
** Calculate the number of true positives, true negatives, false        **
** negatives and false positives resulting from comparison of test and  **
** known secondary structure assignments for a particular secondary     **
** structure type                                                       **
**                                                                      **
** INPUT:   *TestAsn   String with one letter test secondary structure  **
**                     assignment                                       **
**          *KnownAsn  String with one letter known secondary structure **
**                     assignment                                       **
**          Length     Length of the assignment                         **
**          SecStrType Type of the secondary structure to which this    **
**                     operation should be applied                      **
**                                                                      **
** OUTPUT:  *Quality   Pointer to the structure with quality assessment **
**                                                                      **
*************************************************************************/
int svt_stride::Difference(char *TestAsn, char *KnownAsn, int Length, char SecStrType, QUALITY *Qual)
{
  register int Res;

  Qual->TP = Qual->TN = Qual->FP = Qual->FN = 0;

  for( Res=0; Res<Length; Res++ ) {
    if( KnownAsn[Res] != 'X' ) {

      if( KnownAsn[Res] == SecStrType && TestAsn[Res]  == SecStrType ) Qual->TP++;
      else
      if( KnownAsn[Res] != SecStrType && TestAsn[Res]  != SecStrType ) Qual->TN++;
      else
      if( KnownAsn[Res] != SecStrType && TestAsn[Res]  == SecStrType ) Qual->FP++;
      else
      if( KnownAsn[Res] == SecStrType && TestAsn[Res]  != SecStrType ) Qual->FN++;
    }
  }

  if( Qual->TP == 0 && Qual->TN == 0 && Qual->FP == 0 && Qual->FN == 0 )  {
    Qual->Perc = 0.0;
    return(FAILURE);
  }

  Qual->Perc =
    ((float)Qual->TP+(float)Qual->TN)/
      ((float)Qual->TP+(float)Qual->TN+(float)Qual->FP+(float)Qual->FN);

  return(SUCCESS);
}

/*************************************************************************
**                                                                      **
** Calculate percent of the correctly assigned residues                 **
**                                                                      **
** INPUT:   *TestAsn   String with one letter test secondary structure  **
**                     assignment                                       **
**          *KnownAsn  String with one letter known secondary structure **
**                     assignment                                       **
**          Length     Length of the assignment                         **
**                                                                      **
** RETURNS:            Percent correct                                  **
**                                                                      **
*************************************************************************/
float svt_stride::PercentCorrect(char *TestAsn, char *KnownAsn, int Length)
{
  int Res, Count=0;;

  for( Res=0; Res<Length; Res++ )
    if( KnownAsn[Res] == TestAsn[Res] )
      Count++;

  return( ((float)Count/(float)Length) );
}

/*************************************************************************
**                                                                      **
** Calculate measures of secondary structure assignment quality based   **
** on the number of true positives, true negatives, false negatives and **
** false positives resulting from comparison of test and known          **
** assignments                                                          **
**                                                                      **
** INPUT:   *Quality   Pointer to the structure with quality assessment **
**                     assignment                                       **
** OUTPUT:  Quality->Corr  Correlation coefficient between the two      **
**                         assignments as suggested by B.Matthews       **
**                         (1975) Biochim. Biophys. Acta, 405, 442-451  **
**          Quality->Perc  Percent correct                              **
**                                                                      **
*************************************************************************/
int svt_stride::AssessCorr(QUALITY *Qual)
{

  float TP, TN, FP, FN;

  if( (Qual->TP == 0 && Qual->FN == 0) || (Qual->TP == 0 && Qual->FP == 0) ) return(FAILURE);
  else {
    TP = (float)Qual->TP;
    TN = (float)Qual->TN;
    FP = (float)Qual->FP;
    FN =(float)Qual->FN;

    Qual->Corr = (TP*TN - FN*FP)/sqrt((TN+FN)*(TN+FP)*(TP+FN)*(TP+FP));

    return(SUCCESS);
  }
}

int svt_stride::AssessPerc(QUALITY *Qual)
{

  float TP, TN, FP, FN;

  TP = (float)Qual->TP;
  TN = (float)Qual->TN;
  FP = (float)Qual->FP;
  FN =(float)Qual->FN;

  Qual->Perc = (TP+TN)/(TP+TN+FP+FN);

  return(SUCCESS);
}

void svt_stride::ExcludeObvious(char *Asn1, char *Asn2, char *KnownAsn, int Length)
{
  register int i;

  for( i=0; i<Length; i++ )
    if( Asn1[i] == Asn2[i] ) {
      KnownAsn[i] = 'X';
      Asn1[i] = 'X';
      Asn2[i] = 'X';
    }
}



int svt_stride::CheckAtom(char *At)
{
  int AtomTypeCnt, AtomTypeNumber = 95;
  static const char *Atom[MAX_AtomType] = {
    "AD1", "AD2", "AE1", "AE2", "C", "CA", "CB", "CD", "CD1", "CD2", "CE", "CE1", "CE2",
    "CE3", "CG", "CG1", "CG2", "CH2", "CH3", "CZ", "CZ2", "CZ3", "HG", "HG1", "HH", "HH2",
    "HZ", "HZ2", "HZ3", "N", "ND1", "ND2", "NE", "NE1", "NE2", "NH1", "NH2", "NZ", "O",
    "OD1", "OD2", "OE", "OE1", "OE2", "OG", "OG1", "OH", "OXT", "SD", "SG", "H", "HA", "HB",
    "HD1", "HD2", "HE", "HE1", "HE2", "HE3", "1H", "1HA", "1HB", "1HD", "1HD1", "1HD2",
    "1HE", "1HE2", "1HG", "1HG1", "1HG2", "1HH1", "1HH2", "1HZ", "2H", "2HA", "2HB", "2HD",
    "2HD1", "2HD2", "2HE", "2HE2", "2HG", "2HG1", "2HG2", "2HH1", "2HH2", "2HZ", "3H", "3HB",
    "3HD1", "3HD2", "3HE", "3HG1", "3HG2", "3HZ"
    };

  for( AtomTypeCnt=0; AtomTypeCnt<AtomTypeNumber; AtomTypeCnt++ )
    if( !strcmp(At,Atom[AtomTypeCnt]) )
      return(SUCCESS);

  return(FAILURE);
}

int svt_stride::CheckRes(char *Res)
{
  register int ResTypeCnt;
  int ResTypeNumber = 45;
  static const char *Rsd[MAX_ResType] = {
    "ACE", "ALA", "ARG", "ASN", "ASP", "ASX", "CYS", "GLN", "GLU", "GLX", "GLY", "HIS", "ILE",
    "LEU", "LYS", "MET", "PRO", "PHE", "SER", "THR", "TRP", "TYR", "VAL", "FOR", "UNK", "HOH",
    /* Residues found empirically in the protein files */
/*  1gp1   1gp1   1hne   1tmn   2mcg   5hvp   6cha   1bbo   1ctg   act1   act1   aom1   rom7 */
    "SEC", "ALM", "MSU", "CLT", "PCA", "STA", "EXC", "ABU", "HYP", "HMS", "ASS", "OCT", "CYH",
/*  sod0  7adh   2psg   tim1   tim2   2pia */
    "MN", "INI", "PO3", "SUL", "WAT", "FMN"
    };

  for( ResTypeCnt=0; ResTypeCnt<ResTypeNumber; ResTypeCnt++ )
      if( !strcmp(Res,Rsd[ResTypeCnt]) )
	 return(SUCCESS);

  return(FAILURE);
}

int svt_stride::CheckChain(CHAIN *Chain, COMMAND *Cmd)
{

  int Res, HelAlp, HelPI, Hel310, Sheet, Turn, Bound[300][2];
  int i, j, AsnNumb=0, At, SuspCnt, Beg, End;
  float Content;

  if( Cmd->NProcessed && !ChInStr(Cmd->Processed,SpaceToDash(Chain->Id)) ) {
    Chain->Valid = NO;
    return(FAILURE);
  }

  if( Chain->NRes < 5 )
    return(NotValid(Chain,"less than 5 residues"));

  if( !Cmd->Stringent )
    return(SUCCESS);

  for( Res=0; Res<Chain->NRes; Res++ ){
    if( !CheckRes(Chain->Rsd[Res]->ResType) )
      Chain->NonStandRes++;
    for( At=0; At<Chain->Rsd[Res]->NAtom; At++) {
      if( !CheckAtom(Chain->Rsd[Res]->AtomType[At]) )
	Chain->NonStandAtom++;
      if(Chain->Rsd[Res]->Coord[At][0] < MIN_X || Chain->Rsd[Res]->Coord[At][0] > MAX_X ||
	 Chain->Rsd[Res]->Coord[At][1] < MIN_Y || Chain->Rsd[Res]->Coord[At][1] > MAX_Y ||
	 Chain->Rsd[Res]->Coord[At][2] < MIN_Z || Chain->Rsd[Res]->Coord[At][2] > MAX_Z ||
	 Chain->Rsd[Res]->Occupancy[At] < MIN_Occupancy ||
	 Chain->Rsd[Res]->Occupancy[At] > MAX_Occupancy ||
	 Chain->Rsd[Res]->TempFactor[At] < MIN_TempFactor ||
	 Chain->Rsd[Res]->TempFactor[At] > MAX_TempFactor )
	break;
    }
    if( At < Chain->Rsd[Res]->NAtom )
      break;
  }

  if( Res < Chain->NRes )
    return(NotValid(Chain,"suspicious coordinates, occupancy or temperature factor"));

  if( 100.0*(float)Chain->NonStandRes/(float)Chain->NRes > MAXNONSTAND )
    return(NotValid(Chain,"too many non-standard residues"));

  if( Chain->NRes < Cmd->MinLength )
    return(NotValid(Chain,"Short chain"));

  if( Chain->NRes > Cmd->MaxLength )
    return(NotValid(Chain,"Long chain"));

  if( Chain->Method == XRay &&
     (Chain->Resolution < Cmd->MinResolution || Chain->Resolution > Cmd->MaxResolution ) )
    return(NotValid(Chain,"Resolution out of range"));

  if( (int)strlen(Cmd->Cond) != 0 )  {

    if( ChInStr(Cmd->Cond,'c') ) {
      for( Res=0; Res<Chain->NRes; Res++ )
	if( FindAtom(Chain,Res,"N",&At) ||
	    FindAtom(Chain,Res,"O",&At) ||
	    FindAtom(Chain,Res,"C",&At) )
	  break;

      if( Res == Chain->NRes )
	return(NotValid(Chain,"only CA"));
    }

    if( Chain->Method == NMR && !ChInStr(Cmd->Cond,'n') )
      return(NotValid(Chain,"NMR chain"));

    if( Chain->Method == XRay && !ChInStr(Cmd->Cond,'x') )
      return(NotValid(Chain,"XRay chain"));

    if( Chain->Method == Model && !ChInStr(Cmd->Cond,'m') )
      return(NotValid(Chain,"Model chain"));

    if( Chain->Published == NO && ChInStr(Cmd->Cond,'p') )
      return(NotValid(Chain,"Not published"));

    if( Chain->DsspAssigned == YES && ChInStr(Cmd->Cond,'d') )
      return(NotValid(Chain,"Assigned according to DSSP"));

    if( ChInStr(Cmd->Cond,'a') ) {

      if( Chain->Valid && Chain->NHelix == 0 && Chain->NSheet == -1 && Chain->NTurn == 0 )
	return(NotValid(Chain,"No assignment"));

      if( (Content = SecStrContent(Chain,&HelAlp,&HelPI,&Hel310,&Sheet,&Turn)) < 0.4 ||
	   Content > 0.9 )
	return(NotValid(Chain,"Suspicious content"));

      SuspCnt = 0;
      for( Res=1; Res<Chain->NRes-1; Res++ ) {
	if( ( Chain->Rsd[Res]->Prop->PdbAsn != 'H' && Chain->Rsd[Res]->Prop->PdbAsn != 'T' &&
	     Chain->Rsd[Res]->Prop->Phi > -150.0 && Chain->Rsd[Res]->Prop->Phi < 0.0 &&
	     Chain->Rsd[Res]->Prop->Psi > -100.0 && Chain->Rsd[Res]->Prop->Psi < 10.0) )
	  SuspCnt++;

      }

      if( (float)SuspCnt/(float)Chain->NRes > 0.4 )
	return(NotValid(Chain,"Suspicious assignment"));

      for( i=0; i<Chain->NHelix; i++ ) {
	if( !PdbN2SeqN(Chain,Chain->Helix[i]->PDB_ResNumb1,&Beg) ||
	   !PdbN2SeqN(Chain,Chain->Helix[i]->PDB_ResNumb2,&End) ||
	   /*	      !CheckRes(Chain->Helix[i]->PDB_ResNumb1) ||
		      !CheckRes(Chain->Helix[i]->PDB_ResNumb2) || */
	   Chain->Helix[i]->Class > 10 ||
	   Chain->Helix[i]->Class < 1  ||
	   End-Beg > 100 || End-Beg < 0 )
	  break;
	else
	  if( Chain->Helix[i]->Class == 1 ) {
	    Bound[AsnNumb][0] = Beg;
	    Bound[AsnNumb][1] = End;
	    AsnNumb++;
	  }
      }
      if( i < Chain->NHelix )
	return(NotValid(Chain,"Erraneous helix assignment"));

      for( i=0; i<Chain->NSheet; i++ )
	for( j=0; j<Chain->Sheet[i]->NStrand; j++ ) {
	  if( !PdbN2SeqN(Chain,Chain->Sheet[i]->PDB_ResNumb1[j],&Beg) ||
	     !PdbN2SeqN(Chain,Chain->Sheet[i]->PDB_ResNumb2[j],&End) ||
	     /*	        !CheckRes(Chain->Sheet[i]->PDB_ResNumb1[j]) ||
			!CheckRes(Chain->Sheet[i]->PDB_ResNumb2[j]) || */
	     End-Beg > 100 || End-Beg < 0 )
	    break;
	  else
	    if( Chain->Sheet[i]->Sence[j] != 0 ) {
	      Bound[AsnNumb][0] = Beg;
	      Bound[AsnNumb][1] = End;
	      AsnNumb++;
	    }
	  if( j < Chain->Sheet[i]->NStrand )
	    break;
	}

      if( i < Chain->NSheet )
	return(NotValid(Chain,"Erraneous sheet assignment"));

      for( i=0; i<Chain->NTurn; i++ )
	if( !PdbN2SeqN(Chain,Chain->Turn[i]->PDB_ResNumb1,&Beg) ||
	   !PdbN2SeqN(Chain,Chain->Turn[i]->PDB_ResNumb2,&End) ||
	   End-Beg > 100 || End-Beg < 0 )
	  break;

      if( i < Chain->NTurn )
	NotValid(Chain,"Erraneous turn assignment");

      for( i=0; i<AsnNumb-1; i++ ) {
	for( j=i+1; j<AsnNumb; j++ ) {
	  if( Bound[i][0] == Bound[j][0] && Bound[i][1] == Bound[j][1] ) continue;
	  if( (Bound[j][0] > Bound[i][0] && Bound[j][0] < Bound[i][1]) ||
	     (Bound[j][1] > Bound[i][0] && Bound[j][1] < Bound[i][1]) ||
	     (Bound[i][0] > Bound[j][0] && Bound[i][0] < Bound[j][1]) ||
	     (Bound[i][1] > Bound[j][0] && Bound[i][1] < Bound[j][1]) )
	    break;
	}
	if( j < AsnNumb )
	  break;
      }

      if( i < AsnNumb-1 )
	return(NotValid(Chain,"Assignment overlap"));
    }
  }

  fprintf(stderr,"ACCEPTED %s %c %4d %7.3f\n",
	  Chain->File,Chain->Id,Chain->NRes,Chain->Resolution);
  return(SUCCESS);
}


int svt_stride::NotValid(CHAIN *Chain, const char *Message)
{
   svtout << "IGNORED " << SpaceToDash(Chain->Id) << " ( " <<  Message << " ) "<< endl;
   //fprintf(stderr,"IGNORED %s %c ",Chain->File,SpaceToDash(Chain->Id));
   Chain->Valid = NO;
   return 0;

}

FILE *svt_stride::efopen(char *file, char *mode)    /* fopen file, die if can't */
{
  FILE *fp;

  if( (fp=fopen(file,mode)) )
    return fp;
  else
	  svtout << "can't open file " << endl;
  return(FAILURE);
}


int svt_stride::Uniq(char **List, int ListLength)
{
    int i, j;

    for( i=1; i<ListLength-1; i++ ) {
      if( *List[i] != '-' ) continue;
      for( j=i+1; j<ListLength; j++ ) {
	if( *List[j] != '-' ) continue;
	if( !strcmp(List[i],List[j] ) ) return(0);
      }
    }

    return(1);
}

BOOLEAN svt_stride::Specified(char **List, int ListLength, char Option)
{
    int i;

    for( i=1; i<ListLength; i++ )
      if( *List[i] == '-' && *(List[i]+1) == Option )
	return(YES);

    return(NO);
}

int svt_stride::Parse(char **List, int ListLength, char *Option)
{
    int i;

    for( i=1; i<ListLength; i++ ) {
      if( *List[i] != '-' ) continue;
      if( !strcmp(List[i],Option) ) return(i);
    }

    return(0);
}

int svt_stride::CollectOptions(char **List, int ListLength, int Stream, int *Options)
{
    int OptCnt, i;

    OptCnt = 0;

    for( i=1; i<ListLength; i++ )
	if( *List[i] == '-' && !isdigit( *(List[i]+1) ) && atoi( List[i]+2 ) == Stream )
	    Options[OptCnt++] = i;

    return(OptCnt);

}

void svt_stride::die(const char *format, ... ) {

	char buffer[256];
	va_list args;
	va_start (args, format);
	vsprintf (buffer,format, args);
	svtout << buffer <<endl;
	va_end (args);

}

int svt_stride::ReadDSSP(CHAIN **Chain, DSSP **Dssp, COMMAND *Cmd)
{

  FILE *fi;
  int ChainNumber = -1, Start = 0, i;
  BOOLEAN DuplicateChain = NO;
  BUFFER Buffer, Tmp1, Tmp2;
  char *Fields[MAX_FIELD];

  if( strlen(Cmd->DsspFile) == 0 ) {
    strcpy(Cmd->DsspFile,DSSPPATH);
    GetFileNameFromPath(Chain[0]->File,Tmp1);
    StripPathFromLastExtention(Tmp1,Tmp2);
    strcat(Cmd->DsspFile,Tmp2);
    strcat(Cmd->DsspFile,".dssp");
  }

  if( (fi = fopen(Cmd->DsspFile,"r")) ) {

    while( fgets(Buffer,BUFSZ,fi) != NULL && !DuplicateChain ) {
      if( Buffer[2] == '#' ) Start = 1;
      else
      if( Start == 1 ) {
	if( Buffer[13] == '!' ) continue;
	if( ChainNumber > 0 ) {
	  for( i=0; i<ChainNumber; i++ )
	    if( Dssp[i]->Id == Buffer[11] ) {
	      DuplicateChain = YES;
	      fprintf(stderr,"# Duplicate chain(s) in DSSP file %s\n",Cmd->DsspFile);
	    break;
	  }
	}
	if( ( ChainNumber == -1 || Buffer[11] != Dssp[ChainNumber]->Id ) ) {
	  ChainNumber++;
	  Dssp[ChainNumber]       = (DSSP *)ckalloc(sizeof(DSSP));
	  Dssp[ChainNumber]->ResType = CharMatrix(MAX_RES,RES_FIELD);
	  Dssp[ChainNumber]->PDB_ResNumb = CharMatrix(MAX_RES,RES_FIELD);
	  Dssp[ChainNumber]->SecondStr = (char *)ckalloc(MAX_RES*sizeof(char));
	  Dssp[ChainNumber]->Accessibility = (float *)ckalloc(MAX_RES*sizeof(float));
	  strcpy(Dssp[ChainNumber]->File,Cmd->DsspFile);
	  Dssp[ChainNumber]->Id   = Buffer[11];
	  Dssp[ChainNumber]->NRes = 0;
	}

	if( islower(Buffer[13]) ) Buffer[13] = 'C';
	strcpy(Dssp[ChainNumber]->ResType[ Dssp[ChainNumber]->NRes ],OneToThree(Buffer[13]));
	if( Buffer[16] == ' ' )
	  Dssp[ChainNumber]->SecondStr[ Dssp[ChainNumber]->NRes ] = 'C';
	else
	  Dssp[ChainNumber]->SecondStr[ Dssp[ChainNumber]->NRes ] = Buffer[16];
	SplitString(Buffer+6,Fields,1);
	strcpy(Dssp[ChainNumber]->PDB_ResNumb[Dssp[ChainNumber]->NRes],Fields[0]);
	SplitString(Buffer+34,Fields,1);
	Dssp[ChainNumber]->Accessibility[ Dssp[ChainNumber]->NRes ] = atof(Fields[0]);
	Dssp[ChainNumber]->NRes++;
      }
    }
    fclose(fi);
  }

  ChainNumber++;

  for( i=0; i<ChainNumber; i++ )
    Dssp[i]->Id = SpaceToDash(Dssp[i]->Id);

  return(ChainNumber);
}

/*************************************************************************
**                                                                      **
** Check whether PDB and DSSP files correspond to each other            **
**                                                                      **
** INPUT:   *Chain     Pointer to a PDB chain                           **
**          *Dssp      Pointer to a DSSP chain                          **
**                                                                      **
*************************************************************************/

int svt_stride::CompPdbDssp(CHAIN *Chain, DSSP *Dssp)
{

  int Res, CA;

  /* If the first or the last PDB residue does not have Ca, insert one residue in
     coil conformation in the beginning or in the end of the DSSP chain */
  if( !FindAtom(Chain,0,"CA",&CA) )
    InsertFirst(Dssp,Chain);

  if( Chain->NRes-1 > 0 && !FindAtom(Chain,Chain->NRes-1,"CA",&CA) )
    InsertLast(Dssp,Chain);

  /* Check correspondence of residue types */
  for( Res=0; Res<Chain->NRes; Res++ )
    if( strcmp(Chain->Rsd[Res]->ResType,Dssp->ResType[Res]) )
      return(FAILURE);

  /* If DSSP chain is still longer than PDB chain, shrink it */
  if( Chain->NRes != Dssp->NRes )
    Dssp->NRes = Chain->NRes;

  return(SUCCESS);
}

/*************************************************************************
**                                                                      **
** Insert one residue in the beginning of a DSSP chain and assign coil  **
** conformation to it. This is neccessary to get rid of descrepancy     **
** between the DSSP and PDB protein chains resulting from the fact      **
** that DSSP does not consider residues without Ca atoms. Such residues,**
** e.g. ACE, often occur as first residues in PDB files                 **
**                                                                      **
** INPUT:   *Chain     Pointer to protein chain                         **
**          *Dssp      Pointer to DSSP chain                            **
**                                                                      **
** OUTPUT:  *DSSP      Pointer to modified DSSP chain                   **
**                                                                      **
*************************************************************************/
void svt_stride::InsertFirst(DSSP *Dssp, CHAIN *Chain)
{
  int Res;

  for( Res=Dssp->NRes; Res>=1; Res-- ) {
    strcpy(Dssp->ResType[Res],Dssp->ResType[Res-1]);
    strcpy(Dssp->PDB_ResNumb[Res],Dssp->PDB_ResNumb[Res-1]);
    Dssp->SecondStr[Res] = Dssp->SecondStr[Res-1];
  }

  strcpy(Dssp->ResType[0],Chain->Rsd[0]->ResType);
  strcpy(Dssp->PDB_ResNumb[0],Chain->Rsd[0]->PDB_ResNumb);
  Dssp->SecondStr[0] = 'C';

  Dssp->NRes++;
}

/*************************************************************************
**                                                                      **
** Insert one residue in the end a DSSP chain and assign coil           **
** conformation to it. This is neccessary to get rid of descrepancy     **
** between the DSSP and PDB protein chains resulting from the fact      **
** that DSSP does not consider residues without Ca atoms. Such residues,**
** often occur as last residues in PDB files                            **
**                                                                      **
** INPUT:   *Chain     Pointer to protein chain                         **
**          *Dssp      Pointer to DSSP chain                            **
**                                                                      **
** OUTPUT:  *DSSP      Pointer to modified DSSP chain                   **
**                                                                      **
*************************************************************************/
void svt_stride::InsertLast(DSSP *Dssp, CHAIN *Chain)
{

  strcpy(Dssp->ResType[Dssp->NRes],Chain->Rsd[Dssp->NRes]->ResType);
  strcpy(Dssp->PDB_ResNumb[Dssp->NRes],Chain->Rsd[Dssp->NRes]->PDB_ResNumb);
  Dssp->SecondStr[Dssp->NRes] = 'C';

  Dssp->NRes++;
}

/*************************************************************************
**                                                                      **
** Get DSSP secondary structure assignment for every residue            **
**                                                                      **
*************************************************************************/
int svt_stride::GetDsspAsn(CHAIN **Chain, int NChain, COMMAND *Cmd)
{
  DSSP **Dssp;

  int NDsspChain=0, DsspCn, Cn, i;

  Dssp  = (DSSP **)ckalloc(MAX_CHAIN*sizeof(DSSP *));

  if( (NDsspChain = ReadDSSP(Chain,Dssp,Cmd)) == 0 ){
    die("NODSSP Dssp file for %s not found\n",Chain[0]->File);
	return 0;
  }
  for( Cn=0; Cn<NChain; Cn++ ) {

    if( !Chain[Cn]->Valid )
      continue;

    for( DsspCn=0; DsspCn<NDsspChain; DsspCn++ )
      if( SpaceToDash(Chain[Cn]->Id) == Dssp[DsspCn]->Id )
	break;

    if( DsspCn == NDsspChain ) {
      svtout << "No DSSP chain corresponding to " << Chain[Cn]->File << "  " << SpaceToDash(Chain[Cn]->Id) << endl;
	  continue;
    }

    if( !CompPdbDssp(Chain[Cn],Dssp[DsspCn]) ){
      die("PDBDSSPDIF Chain %c differs from %s%c\n",
	  Dssp[DsspCn]->Id,Chain[Cn]->File,SpaceToDash(Chain[Cn]->Id));
    return 0;
		}
    for( i=0; i<Chain[Cn]->NRes; i++ ) {
      Chain[Cn]->Rsd[i]->Prop->DsspAsn  = Dssp[DsspCn]->SecondStr[i];
      Chain[Cn]->Rsd[i]->Prop->DsspSolv = Dssp[DsspCn]->Accessibility[i];
    }
  }

  if( Cmd->DsspAsn ) {
    for( i=0; i<NDsspChain; i++ ) free(Dssp[i]);
    free(Dssp);
  }
 return (SUCCESS);
}

/*************************************************************************
**                                                                      **
** Calculate the number of individual secondary structure elements of   **
** type SecStrType and length not less than ElemLength that are:        **
**  - Present in Asn1 and Asn2 and absent  in Asn3 (YYN)                **
**  - Present in Asn2 and Asn3 and absent  in Asn1 (NYY)                **
**  - Absent  in Asn2 and Asn3 and present in Asn1 (YYN)                **
**  - Absent  in Asn1 and Asn2 and present in Asn3 (YYN)                **
**                                                                      **
*************************************************************************/

int svt_stride::FullElement(char *Asn1, char *Asn2, char *Asn3, int Length, char SecStrType, int ElemLength,
		 char EditChar, int *YYN, int *NYY, int *YNN, int *NNY)
{

  register int i, j, Count1, Count2, Count3;
  int Beg, ElLength;

  *YYN = 0;
  *NYY = 0;
  *YNN = 0;
  *NNY = 0;

  if( ElemLength >= Length )
    return(0);

  ElLength = ElemLength-1;
  Count1 = 0;
  Count2 = 0;
  Count3 = 0;

  Beg = -1;

  for( i=1; i<Length; i++ )
	{
    if( ( i == 0 &&
	    ( ( Asn1[i] == SecStrType && Asn2[i] == SecStrType && Asn3[i] == SecStrType) ||
          ( Asn1[i] != SecStrType && Asn2[i] != SecStrType && Asn3[i] != SecStrType) ) )
       ||
        ( i  > 0 &&
	      ( Asn1[i] != Asn1[i-1] || Asn2[i] != Asn2[i-1] || Asn3[i] != Asn3[i-1] ) )
       ||
          i == Length-1 ) {

      if( Count1 >= ElLength && Count2 >= ElLength && Count3 <  ElLength )
	(*YYN)++;
      else
      if( Count1 <  ElLength && Count2 >= ElLength && Count3 >= ElLength )
	(*NYY)++;
      else
      if( Count1 >= ElLength && Count2 <  ElLength && Count3 <  ElLength )
	(*YNN)++;
      else
      if( Count1 <  ElLength && Count2 <  ElLength && Count3 >= ElLength )
	(*NNY)++;

/*       if( Count1 >= ElLength || Count2 >= ElLength || Count3 >= ElLength ) {
 * 	for( j=Beg-1; j<i; j++ ) {
 * 	  Asn1[j] = 'X';
 * 	  Asn2[j] = 'X';
 * 	  Asn3[j] = 'X';
 * 	}
 *       }
 *
 */
      if( Count1 >= ElLength && ( Count2 < ElLength || Count3 < ElLength ) )
	for( j=Beg-1; j<i; j++ )
	  Asn1[j] = EditChar;

      if( Count2 >= ElLength && ( Count1 < ElLength || Count3 < ElLength ) )
	for( j=Beg-1; j<i; j++ )
	  Asn2[j] = EditChar;

      if( Count3 >= ElLength && ( Count1 < ElLength || Count2 < ElLength ) )
	for( j=Beg-1; j<i; j++ )
	  Asn3[j] = EditChar;

      Count1 = 0;
      Count2 = 0;
      Count3 = 0;
      Beg = -1;

    }
    else {
      if( Asn1[i] == SecStrType ) Count1++;
      if( Asn2[i] == SecStrType ) Count2++;
      if( Asn3[i] == SecStrType ) Count3++;
      if( Beg == -1 && (Count1 == 1 || Count2 == 1 || Count3 == 1) ) Beg = i;
    }
  }

  CorrectAsn(Asn1,Length,SecStrType,EditChar,ElLength);
  CorrectAsn(Asn2,Length,SecStrType,EditChar,ElLength);
  CorrectAsn(Asn3,Length,SecStrType,EditChar,ElLength);

  return( (*YYN) * (*NYY) * (*YNN) * (*NNY) );
}


/*************************************************************************
**                                                                      **
** Calculate the number of individual secondary structure elements of   **
** type SecStrType in the known assignment Asn2 that are:               **
**  - Reproduced in Asn1 better than in Asn3 (Better)                   **
**  - Reproduced in Asn1 worse  than in Asn3 (Worse)                    **
**                                                                      **
*************************************************************************/

int svt_stride::CompareElements(char *Asn1, char *Asn2, char *Asn3, int Length,
		   char SecStrType, int *Better, int *Worse)
{

  register int i, j, Count1, Count2;
  int TotalNumber = 0, Beg;

  *Better = 0;
  *Worse = 0;

  Beg = -1;

  for( i=0; i<Length; i++ ) {
    if( (Asn1[i] == SecStrType || Asn2[i] == SecStrType || Asn3[i] == SecStrType) &&
	(i == 0 ||
	 ( Asn1[i-1] != SecStrType && Asn2[i-1] != SecStrType && Asn3[i-1] != SecStrType) ) ) {
      TotalNumber++;
      Beg = i;
    }
    else
    if( Beg != -1 && ( i == Length-1 ||
	 ( Asn1[i] != SecStrType && Asn2[i] != SecStrType && Asn3[i] != SecStrType ) ) ) {
      Count1 = Count2 = 0;
      for( j=Beg; j<=i; j++ ) {
	if( (Asn1[j] == SecStrType || Asn2[j] == SecStrType) && Asn1[j] != Asn2[j] )
	  Count1++;
	if( (Asn3[j] == SecStrType || Asn2[j] == SecStrType) && Asn3[j] != Asn2[j] )
	  Count2++;
      }
      if( Count1 > Count2 )
	(*Worse)++;
      else
      if( Count2 > Count1 )
	(*Better)++;
      Beg = -1;
    }
  }
  return(TotalNumber);
}

int svt_stride::escape(int RetVal, const char *format, ... ) {

char buffer[256];
va_list args;
va_start (args, format);
vsprintf (buffer,format, args);
svtout << buffer <<endl;
va_end (args);
return(RetVal);
}

void svt_stride::GetFileNameFromPath(char *Path, char *FileName)
{

  int i;
  static char DirDelim[5] = { ':','/','\\',']','\0'};

  for( i = (int)strlen(Path)-1; i>=0; i-- )
    if( strchr(DirDelim,Path[i]) ) break;

  strcpy(FileName,Path+i+1);
}


void svt_stride::StripPathFromLastExtention(char *Path, char *StrippedPath)
{
  int i;

  strcpy(StrippedPath,Path);

  for( i = (int)strlen(StrippedPath); i>=0; i-- )
    if( StrippedPath[i] == '.' ) {
      StrippedPath[i] = '\0';
      break;
    }
}


void svt_stride::FillAsnAntiPar(char *Asn1, char *Asn2, CHAIN **Chain, int Cn1, int Cn2,
		    MPATTERN **Pat, int NPat, COMMAND *Cmd)
{
  register int i, j;
  int Beg1, Beg2, End1, End2;
  int B1D, B1A, B2D, B2A, E1D, E1A, E2D, E2A;
  char B1DCn, B1ACn, B2DCn, B2ACn, E1DCn, E1ACn, E2DCn, E2ACn, Beg1Cn, Beg2Cn;
  MPATTERN *CurrPat, *PrevPat;;

  for( i=0; i<NPat; i++ ) {

    if( Pat[i]->Nei1 != NULL && Pat[i]->Nei2 == NULL )
      CurrPat = Pat[i]->Nei1;
    else
    if( Pat[i]->Nei2 != NULL && Pat[i]->Nei1 == NULL )
      CurrPat = Pat[i]->Nei2;
    else
      continue;

    if( Cmd->Info ) {
      fprintf(stdout,"From: %c %c ",
	      Pat[i]->Hb1->Dnr->Chain->Id,Pat[i]->Hb2->Dnr->Chain->Id);
      if( Pat[i]->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id )
	fprintf(stdout,"%s %s %s %s \n",
	    Chain[Cn1]->Rsd[Pat[i]->Hb1->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[Pat[i]->Hb1->Acc->A_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[Pat[i]->Hb2->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[Pat[i]->Hb2->Acc->A_Res]->PDB_ResNumb);
      else
	fprintf(stdout,"%s %s %s %s \n",
	    Chain[Cn2]->Rsd[Pat[i]->Hb1->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[Pat[i]->Hb1->Acc->A_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[Pat[i]->Hb2->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[Pat[i]->Hb2->Acc->A_Res]->PDB_ResNumb);
    }

    PrevPat = Pat[i];
    while( CurrPat->Nei1 != NULL && CurrPat->Nei2 != NULL ) {

      if( (CurrPat->Nei1->Nei1 == CurrPat || CurrPat->Nei1->Nei2 == CurrPat) &&
	 CurrPat->Nei1 != PrevPat ) {
	PrevPat = CurrPat;
	CurrPat = CurrPat->Nei1;
      }
      else
      if( (CurrPat->Nei2->Nei1 == CurrPat || CurrPat->Nei2->Nei2 == CurrPat) &&
	 CurrPat->Nei2 != PrevPat ) {
	PrevPat = CurrPat;
	CurrPat = CurrPat->Nei2;
      }
      else {
	fprintf(stdout,"Cycle Anti%s%c i = %d \n",Chain[Cn1]->File,Chain[Cn1]->Id,i);
	break;
      }
    }

    if( Cmd->Info ) {
      fprintf(stdout,"To: %c %c ",
	      CurrPat->Hb1->Dnr->Chain->Id,CurrPat->Hb2->Dnr->Chain->Id);
      if( CurrPat->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id )
	fprintf(stdout,"%s %s %s %s \n",
	    Chain[Cn1]->Rsd[CurrPat->Hb1->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[CurrPat->Hb1->Acc->A_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[CurrPat->Hb2->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[CurrPat->Hb2->Acc->A_Res]->PDB_ResNumb);
      else
	fprintf(stdout,"%s %s %s %s \n",
	    Chain[Cn2]->Rsd[CurrPat->Hb1->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[CurrPat->Hb1->Acc->A_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[CurrPat->Hb2->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[CurrPat->Hb2->Acc->A_Res]->PDB_ResNumb);
    }

    Alias(&B1D,&B1A,&B2D,&B2A,&B1DCn,&B1ACn,&B2DCn,&B2ACn,Pat[i]);
    Alias(&E1D,&E1A,&E2D,&E2A,&E1DCn,&E1ACn,&E2DCn,&E2ACn,CurrPat);

    if( (Cn1 != Cn2 || E1D - B2A <  E2D - B2A ) &&
        ( MakeEnds(&Beg1,B1D,B2A,&Beg1Cn,B1DCn,&End1,E2A,E1D,E2ACn,&Beg2,E2D,E1A,&Beg2Cn,E2DCn,
		   &End2,B1A,B2D,B1ACn,Pat,NPat) ||
          MakeEnds(&Beg1,B1D,B2A,&Beg1Cn,B1DCn,&End1,E1D,E2A,E1DCn,&Beg2,E1A,E2D,&Beg2Cn,E1ACn,
		   &End2,B1A,B2D,B1ACn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || E2D - B2A <  E1D - B2A ) &&
        ( MakeEnds(&Beg1,B1D,B2A,&Beg1Cn,B1DCn,&End1,E1A,E2D,E1ACn,&Beg2,E1D,E2A,&Beg2Cn,E1DCn,
		   &End2,B1A,B2D,B1ACn,Pat,NPat) ||
          MakeEnds(&Beg1,B1D,B2A,&Beg1Cn,B1DCn,&End1,E2D,E1A,E2DCn,&Beg2,E2A,E1D,&Beg2Cn,E2ACn,
		   &End2,B1A,B2D,B1ACn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || B2A - E1D < B2A - E2D ) &&
        ( MakeEnds(&Beg1,B1A,B2D,&Beg1Cn,B1ACn,&End1,E2D,E1A,E2DCn,&Beg2,E2A,E1D,&Beg2Cn,E2ACn,
		   &End2,B1D,B2A,B1DCn,Pat,NPat) ||
          MakeEnds(&Beg1,B1A,B2D,&Beg1Cn,B1ACn,&End1,E1A,E2D,E1ACn,&Beg2,E1D,E2A,&Beg2Cn,E1DCn,
		   &End2,B1D,B2A,B1DCn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || B2A - E2D < B2A - E1D ) &&
        ( MakeEnds(&Beg1,B1A,B2D,&Beg1Cn,B1ACn,&End1,E1D,E2A,E1DCn,&Beg2,E1A,E2D,&Beg2Cn,E1ACn,
		   &End2,B1D,B2A,B1DCn,Pat,NPat) ||
          MakeEnds(&Beg1,B1A,B2D,&Beg1Cn,B1ACn,&End1,E2A,E1D,E2ACn,&Beg2,E2D,E1A,&Beg2Cn,E2DCn,
		   &End2,B1D,B2A,B1DCn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || B1D - E2A <  B2D - E2A ) &&
        ( MakeEnds(&Beg1,E1D,E2A,&Beg1Cn,E1DCn,&End1,B2A,B1D,B2ACn,&Beg2,B2D,B1A,&Beg2Cn,B2DCn,
		   &End2,E1A,E2D,E1ACn,Pat,NPat) ||
          MakeEnds(&Beg1,E1D,E2A,&Beg1Cn,E1DCn,&End1,B1D,B2A,B1DCn,&Beg2,B1A,B2D,&Beg2Cn,B1ACn,
		   &End2,E1A,E2D,E1ACn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || B2D - E2A <  B1D - E2A ) &&
        ( MakeEnds(&Beg1,E1D,E2A,&Beg1Cn,E1DCn,&End1,B1A,B2D,B1ACn,&Beg2,B1D,B2A,&Beg2Cn,B1DCn,
		   &End2,E1A,E2D,E1ACn,Pat,NPat) ||
          MakeEnds(&Beg1,E1D,E2A,&Beg1Cn,E1DCn,&End1,B2D,B1A,B2DCn,&Beg2,B2A,B1D,&Beg2Cn,B2ACn,
		   &End2,E1A,E2D,E1ACn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || E2A - B1D < E2A - B2D ) &&
        ( MakeEnds(&Beg1,E1A,E2D,&Beg1Cn,E1ACn,&End1,B2D,B1A,B2DCn,&Beg2,B2A,B1D,&Beg2Cn,B2ACn,
		   &End2,E1D,E2A,E1DCn,Pat,NPat) ||
          MakeEnds(&Beg1,E1A,E2D,&Beg1Cn,E1ACn,&End1,B1A,B2D,B1ACn,&Beg2,B1D,B2A,&Beg2Cn,B1DCn,
		   &End2,E1D,E2A,E1DCn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || E2A - B2D < E2A - B1D ) &&
        ( MakeEnds(&Beg1,E1A,E2D,&Beg1Cn,E1ACn,&End1,B1D,B2A,B1DCn,&Beg2,B1A,B2D,&Beg2Cn,B1ACn,
		   &End2,E1D,E2A,E1DCn,Pat,NPat) ||
          MakeEnds(&Beg1,E1A,E2D,&Beg1Cn,E1ACn,&End1,B2A,B1D,B2ACn,&Beg2,B2D,B1A,&Beg2Cn,B2DCn,
		   &End2,E1D,E2A,E1DCn,Pat,NPat) ) )
      ;
    else {
      fprintf(stdout,"Ne tot variant.. Anti.. %s%c\n",Chain[Cn1]->File,Chain[Cn1]->Id);
      continue;
    }


    if( Beg1Cn == Chain[Cn1]->Id ) {
      for( j=Beg1; j<=End1; j++ )
	Asn1[j] = 'N';
      for( j=Beg2; j<=End2; j++ )
	Asn2[j] = 'N';
    }
    else {
      for( j=Beg1; j<=End1; j++ )
	Asn2[j] = 'N';
      for( j=Beg2; j<=End2; j++ )
	Asn1[j] = 'N';
    }

    Pat[i]->Nei1 = NULL;
    Pat[i]->Nei2 = NULL;
    CurrPat->Nei1 = NULL;
    CurrPat->Nei2 = NULL;

  }
}


void svt_stride::FillAsnPar(char *Asn1, char *Asn2, CHAIN **Chain, int Cn1, int Cn2,
		MPATTERN **Pat, int NPat, COMMAND *Cmd)
{
  register int i, j;
  int Beg1, Beg2, End1, End2;
  int B1D, B1A, B2D, B2A, E1D, E1A, E2D, E2A;
  char B1DCn, B1ACn, B2DCn, B2ACn, E1DCn, E1ACn, E2DCn, E2ACn, Beg1Cn, Beg2Cn;
  MPATTERN *CurrPat, *PrevPat;;

  for( i=0; i<NPat; i++ ) {

    if( Pat[i]->Nei1 != NULL && Pat[i]->Nei2 == NULL )
      CurrPat = Pat[i]->Nei1;
    else
    if( Pat[i]->Nei2 != NULL && Pat[i]->Nei1 == NULL )
      CurrPat = Pat[i]->Nei2;
    else
      continue;

    if( Cmd->Info ) {
      fprintf(stdout,"From: %c %c ",
	      Pat[i]->Hb1->Dnr->Chain->Id,Pat[i]->Hb2->Dnr->Chain->Id);
      if( Pat[i]->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id )
	fprintf(stdout,"%s %s %s %s \n",
	    Chain[Cn1]->Rsd[Pat[i]->Hb1->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[Pat[i]->Hb1->Acc->A_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[Pat[i]->Hb2->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[Pat[i]->Hb2->Acc->A_Res]->PDB_ResNumb);
      else
	fprintf(stdout,"%s %s %s %s \n",
	    Chain[Cn2]->Rsd[Pat[i]->Hb1->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[Pat[i]->Hb1->Acc->A_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[Pat[i]->Hb2->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[Pat[i]->Hb2->Acc->A_Res]->PDB_ResNumb);
    }

    PrevPat = Pat[i];
    while( CurrPat->Nei1 != NULL && CurrPat->Nei2 != NULL ) {

      if( (CurrPat->Nei1->Nei1 == CurrPat || CurrPat->Nei1->Nei2 == CurrPat) &&
	 CurrPat->Nei1 != PrevPat ) {
	PrevPat = CurrPat;
	CurrPat = CurrPat->Nei1;
      }
      else {
	PrevPat = CurrPat;
	CurrPat = CurrPat->Nei2;
      }
    }

    if( Cmd->Info ) {
      fprintf(stdout,"To: %c %c ",
	      CurrPat->Hb1->Dnr->Chain->Id,CurrPat->Hb2->Dnr->Chain->Id);
      if( CurrPat->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id )
	fprintf(stdout,"%s %s %s %s \n",
	    Chain[Cn1]->Rsd[CurrPat->Hb1->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[CurrPat->Hb1->Acc->A_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[CurrPat->Hb2->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[CurrPat->Hb2->Acc->A_Res]->PDB_ResNumb);
      else
	fprintf(stdout,"%s %s %s %s \n",
	    Chain[Cn2]->Rsd[CurrPat->Hb1->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[CurrPat->Hb1->Acc->A_Res]->PDB_ResNumb,
	    Chain[Cn1]->Rsd[CurrPat->Hb2->Dnr->D_Res]->PDB_ResNumb,
	    Chain[Cn2]->Rsd[CurrPat->Hb2->Acc->A_Res]->PDB_ResNumb);
    }

    Alias(&B1D,&B1A,&B2D,&B2A,&B1DCn,&B1ACn,&B2DCn,&B2ACn,Pat[i]);
    Alias(&E1D,&E1A,&E2D,&E2A,&E1DCn,&E1ACn,&E2DCn,&E2ACn,CurrPat);

    if( ( Cn1 != Cn2 || abs(E1D-B2A) < abs(E2D-B2A) ) &&
        ( MakeEnds(&Beg1,B1D,B2A,&Beg1Cn,B1DCn,&End1,E2A,E1D,E2ACn,&Beg2,B1A,B2D,&Beg2Cn,B1ACn,
		   &End2,E2D,E1A,E2DCn,Pat,NPat) ||
          MakeEnds(&Beg1,B1D,B2A,&Beg1Cn,B1DCn,&End1,E1D,E2A,E1DCn,&Beg2,B1A,B2D,&Beg2Cn,B1ACn,
		   &End2,E1A,E2D,E1ACn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || abs(E2D-B2A) < abs(E1D-B2A) ) &&
        ( MakeEnds(&Beg1,B1D,B2A,&Beg1Cn,B1DCn,&End1,E1A,E2D,E1ACn,&Beg2,B1A,B2D,&Beg2Cn,B1ACn,
		   &End2,E1D,E2A,E1DCn,Pat,NPat) ||
          MakeEnds(&Beg1,B1D,B2A,&Beg1Cn,B1DCn,&End1,E2D,E1A,E2DCn,&Beg2,B1A,B2D,&Beg2Cn,B1ACn,
		   &End2,E2A,E1D,E2ACn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || abs(B2A-E1D) < abs(B2A-E2D) ) &&
        ( MakeEnds(&Beg1,B1A,B2D,&Beg1Cn,B1ACn,&End1,E2D,E1A,E2DCn,&Beg2,B1D,B2A,&Beg2Cn,B1DCn,
		   &End2,E2A,E1D,E2ACn,Pat,NPat) ||
          MakeEnds(&Beg1,B1A,B2D,&Beg1Cn,B1ACn,&End1,E1A,E2D,E1ACn,&Beg2,B1D,B2A,&Beg2Cn,B1DCn,
		   &End2,E1D,E2A,E1DCn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || abs(B2A-E2D) < abs(B2A-E1D) ) &&
        ( MakeEnds(&Beg1,B1A,B2D,&Beg1Cn,B1ACn,&End1,E1D,E2A,E1DCn,&Beg2,B1D,B2A,&Beg2Cn,B1DCn,
		   &End2,E1A,E2D,E1ACn,Pat,NPat) ||
          MakeEnds(&Beg1,B1A,B2D,&Beg1Cn,B1ACn,&End1,E2A,E1D,E2ACn,&Beg2,B1D,B2A,&Beg2Cn,B1DCn,
		   &End2,E2D,E1A,E2DCn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || abs(B1D-E2A) < abs(B2D-E2A) ) &&
        ( MakeEnds(&Beg1,E1D,E2A,&Beg1Cn,E1DCn,&End1,B2A,B1D,B2ACn,&Beg2,E1A,E2D,&Beg2Cn,E1ACn,
		   &End2,B2D,B1A,B2DCn,Pat,NPat) ||
          MakeEnds(&Beg1,E1D,E2A,&Beg1Cn,E1DCn,&End1,B1D,B2A,B1DCn,&Beg2,E1A,E2D,&Beg2Cn,E1ACn,
		   &End2,B1A,B2D,B1ACn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || abs(B2D-E2A) < abs(B1D-E2A) ) &&
        ( MakeEnds(&Beg1,E1D,E2A,&Beg1Cn,E1DCn,&End1,B1A,B2D,B1ACn,&Beg2,E1A,E2D,&Beg2Cn,E1ACn,
		   &End2,B1D,B2A,B1DCn,Pat,NPat) ||
          MakeEnds(&Beg1,E1D,E2A,&Beg1Cn,E1DCn,&End1,B2D,B1A,B2DCn,&Beg2,E1A,E2D,&Beg2Cn,E1ACn,
		   &End2,B2A,B1D,B2ACn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || abs(E2A-B1D) < abs(E2A-B2D) ) &&
        ( MakeEnds(&Beg1,E1A,E2D,&Beg1Cn,E1ACn,&End1,B2D,B1A,B2DCn,&Beg2,E1D,E2A,&Beg2Cn,E1DCn,
		   &End2,B2A,B1D,B2ACn,Pat,NPat) ||
          MakeEnds(&Beg1,E1A,E2D,&Beg1Cn,E1ACn,&End1,B1A,B2D,B1ACn,&Beg2,E1D,E2A,&Beg2Cn,E1DCn,
		   &End2,B1D,B2A,B1DCn,Pat,NPat) ) )
      ;
    else
    if( ( Cn1 != Cn2 || abs(E2A-B2D) < abs(E2A-B1D) ) &&
        ( MakeEnds(&Beg1,E1A,E2D,&Beg1Cn,E1ACn,&End1,B1D,B2A,B1DCn,&Beg2,E1D,E2A,&Beg2Cn,E1DCn,
		   &End2,B1A,B2D,B1ACn,Pat,NPat) ||
          MakeEnds(&Beg1,E1A,E2D,&Beg1Cn,E1ACn,&End1,B2A,B1D,B2ACn,&Beg2,E1D,E2A,&Beg2Cn,E1DCn,
		   &End2,B2D,B1A,B2DCn,Pat,NPat) ) )
      ;
    else {
      fprintf(stdout,"Ne tot variant.. Par %s%c\n",Chain[Cn1]->File,Chain[Cn1]->Id);
      continue;
    }

    if( Beg1Cn == Chain[Cn1]->Id ) {
      for( j=Beg1; j<=End1; j++ ) Asn1[j] = 'P';
      for( j=Beg2; j<=End2; j++ ) Asn2[j] = 'P';
    }
    else {
      for( j=Beg1; j<=End1; j++ ) Asn2[j] = 'P';
      for( j=Beg2; j<=End2; j++ ) Asn1[j] = 'P';
    }

    Pat[i]->Nei1 = NULL;
    Pat[i]->Nei2 = NULL;
    CurrPat->Nei1 = NULL;
    CurrPat->Nei2 = NULL;

  }
}


int svt_stride::MakeEnds(int *Beg1, int ResBeg1, int NeiBeg1, char *Beg1Cn, char ResBeg1Cn, int *End1,
	     int ResEnd1, int NeiEnd1, char ResEnd1Cn, int *Beg2, int ResBeg2, int NeiBeg2,
	     char *Beg2Cn, char ResBeg2Cn, int *End2, int ResEnd2, int NeiEnd2,
	     char ResEnd2Cn, MPATTERN **Pat, int NPat)
{

  register int i;
  int Flag1 = 0, Flag2 = 0;


  if( ResBeg1 <= NeiBeg1 && NeiBeg1 <= NeiEnd1 && NeiEnd1 <= ResEnd1 &&
      ResBeg2 <= NeiBeg2 && NeiBeg2 <= NeiEnd2 && NeiEnd2 <= ResEnd2 &&
      ResBeg1Cn == ResEnd1Cn && ResBeg2Cn == ResEnd2Cn ) {

    *Beg1 = ResBeg1;
    *End1 = ResEnd1;
    *Beg2 = ResBeg2;
    *End2 = ResEnd2;
    *Beg1Cn = ResBeg1Cn;
    *Beg2Cn = ResBeg2Cn;

    for( i=0; i<NPat && (Flag1 == 0 || Flag2 == 0); i++ ) {
      if( ( (Pat[i]->Hb1->Dnr->D_Res == (*Beg1)
	     && Pat[i]->Hb1->Acc->A_Res == (*End2)
	     && Pat[i]->Hb1->Dnr->Chain->Id == (*Beg1Cn)
	     && Pat[i]->Hb1->Acc->Chain->Id == (*Beg2Cn) )
	   ||
	   (Pat[i]->Hb1->Acc->A_Res == (*Beg1)
	    && Pat[i]->Hb1->Dnr->D_Res == (*End2)
	    && Pat[i]->Hb1->Acc->Chain->Id == (*Beg1Cn)
	    && Pat[i]->Hb1->Dnr->Chain->Id == (*Beg2Cn) ) )
	 && Pat[i]->Hb1->Dnr->D_Res == Pat[i]->Hb2->Acc->A_Res
	 && Pat[i]->Hb2->Dnr->D_Res == Pat[i]->Hb1->Acc->A_Res )
	Flag1 = 1;
      if( ( (Pat[i]->Hb1->Dnr->D_Res == (*Beg2)
	     && Pat[i]->Hb1->Acc->A_Res == (*End1)
	     && Pat[i]->Hb1->Dnr->Chain->Id == (*Beg2Cn)
	     && Pat[i]->Hb1->Acc->Chain->Id == (*Beg1Cn) )
	   ||
	   (Pat[i]->Hb1->Acc->A_Res == (*Beg2)
	    && Pat[i]->Hb1->Dnr->D_Res == (*End1)
	    && Pat[i]->Hb1->Acc->Chain->Id == (*Beg2Cn)
	    && Pat[i]->Hb1->Dnr->Chain->Id == (*Beg1Cn) ) )
	 && Pat[i]->Hb1->Dnr->D_Res == Pat[i]->Hb2->Acc->A_Res
	 && Pat[i]->Hb2->Dnr->D_Res == Pat[i]->Hb1->Acc->A_Res )
	Flag2 = 1;
    }

    if( !Flag1 ) {
      if( *Beg1 != NeiBeg1 ) (*Beg1)++;
      if( *End2 != NeiEnd2 ) (*End2)--;
    }

    if( !Flag2 ) {
      if( *End1 != NeiEnd1 ) (*End1)--;
      if( *Beg2 != NeiBeg2 ) (*Beg2)++;
    }
    return(SUCCESS);
  }

  return(FAILURE);
}


void svt_stride::FilterAntiPar(MPATTERN **Pat, int NPat)
{

  register int i, j;
  int I1A, I1D, I2A, I2D, J1A, J1D, J2A, J2D;
  char I1ACn, I1DCn, I2ACn, I2DCn, J1ACn, J1DCn, J2ACn, J2DCn;

  for( i=0; i<NPat; i++ ) {

    if( !Pat[i]->ExistPattern ) continue;

    Alias(&I1D,&I1A,&I2D,&I2A,&I1DCn,&I1ACn,&I2DCn,&I2ACn,Pat[i]);

    for( j=0; j<NPat; j++ ) {

      if( j == i || !Pat[j]->ExistPattern ) continue;

      Alias(&J1D,&J1A,&J2D,&J2A,&J1DCn,&J1ACn,&J2DCn,&J2ACn,Pat[j]);

      if( J1D == J2A && J2D == J1A && I1D != I2A && I2D != I1A &&
	 ( (J1D == I1D && J1A == I1A) ||  (J1D == I1A && J1A == I1D) ||
	   (J1D == I2A && J1A == I2D) ||  (J1D == I2D && J1A == I2A) ) ) continue;

      if( ( ( I1D < I2A || I2D < I1A ) &&
	   ( (J1A <= I2A && J1A >= I1D && J2D <= I2A && J2D >= I1D && J2DCn == I1DCn &&
	      J2A <= I1A && J2A >= I2D && J1D <= I1A && J1D >= I2D && J1DCn == I2DCn) ||
	     (J2A <= I2A && J2A >= I1D && J1D <= I2A && J1D >= I1D && J1DCn == I1DCn &&
	      J1A <= I1A && J1A >= I2D && J2D <= I1A && J2D >= I2D && J2DCn == I2DCn) ) ) ||
	  ( ( I1D > I2A || I2D > I1A ) &&
	   ( (J1A >= I2A && J1A <= I1D && J2D >= I2A && J2D <= I1D && J2DCn == I1DCn &&
	      J2A >= I1A && J2A <= I2D && J1D >= I1A && J1D <= I2D && J1DCn == I2DCn) ||
	     (J2A >= I2A && J2A <= I1D && J1D >= I2A && J1D <= I1D && J1DCn == I1DCn &&
	      J1A >= I1A && J1A <= I2D && J2D >= I1A && J2D <= I2D && J2DCn == I2DCn) ) ) ) {
	Pat[j]->ExistPattern = NO;
      }
    }
  }
}

void svt_stride::FilterPar(MPATTERN **Pat, int NPat)
{

  register int i, j;
  int I1A, I1D, I2A, I2D, J1A, J1D, J2A, J2D;
  char I1ACn, I1DCn, I2ACn, I2DCn, J1ACn, J1DCn, J2ACn, J2DCn;

  for( i=0; i<NPat; i++ ) {

    if( !Pat[i]->ExistPattern ) continue;

    Alias(&I1D,&I1A,&I2D,&I2A,&I1DCn,&I1ACn,&I2DCn,&I2ACn,Pat[i]);

    for( j=0; j<NPat; j++ ) {

      if( j == i || !Pat[j]->ExistPattern ) continue;

      Alias(&J1D,&J1A,&J2D,&J2A,&J1DCn,&J1ACn,&J2DCn,&J2ACn,Pat[j]);

      if( ( ( I1A >= I2D && I1D >= I2A ) &&
	   ( (J1A >= I2A && J1A <= I1D && J2D >= I2A && J2D <= I1D && J2DCn == I1DCn &&
	      J2A <= I1A && J2A >= I2D && J1D <= I1A && J1D >= I2D && J1DCn == I2DCn) ||
	     (J2A >= I2A && J2A <= I1D && J1D >= I2A && J1D <= I1D && J1DCn == I1DCn &&
	      J1A <= I1A && J1A >= I2D && J2D <= I1A && J2D >= I2D && J2DCn == I2DCn) ) ) ||

	  ( I2A >= I1D && I2D >= I1A  &&
	   ( (J1A <= I2A && J1A >= I1D && J2D <= I2A && J2D >= I1D && J2DCn == I1DCn &&
	      J2A >= I1A && J2A <= I2D && J1D >= I1A && J1D <= I2D && J1DCn == I2DCn) ||

	     (J2A <= I2A && J2A >= I1D && J1D <= I2A && J1D >= I1D && J1DCn == I1DCn &&
	      J1A >= I1A && J1A <= I2D && J2D >= I1A && J2D <= I2D && J2DCn == I2DCn) ) ) ) {
	Pat[j]->ExistPattern = NO;
      }
    }
  }
}

void svt_stride::Alias(int *D1,int *A1,int *D2,int *A2,char *D1Cn,char *A1Cn,char *D2Cn,char *A2Cn,
	  MPATTERN *Pat)
{
    *D1 = Pat->Hb1->Dnr->D_Res;
    *A1 = Pat->Hb1->Acc->A_Res;
    *D2 = Pat->Hb2->Dnr->D_Res;
    *A2 = Pat->Hb2->Acc->A_Res;
    *D1Cn = Pat->Hb1->Dnr->Chain->Id;
    *A1Cn = Pat->Hb1->Acc->Chain->Id;
    *D2Cn = Pat->Hb2->Dnr->Chain->Id;
    *A2Cn = Pat->Hb2->Acc->Chain->Id;
}

/*************************************************************************
**                                                                      **
**  Calculate torsion angle                                             **
**                                                                      **
** INPUT:  *Coord1, *Coord2, *Coord3, *Coord4 Coordinates of four atoms **
**                                                                      **
** RETURNS: Calculate torsion angle                                     **
**                                                                      **
** Adapted  from  the  program  of D.S.Moss.                            **
** Reference:   Moss,  D. S. (1992)  Molecular  geometry.  In:          **
** Computer modelling of biomolecular processess,  Goodfellow, J.M,     **
** Moss,D.S., eds, pp. 5-18.                                            **
**                                                                      **
*************************************************************************/



float svt_stride::Torsion(float *Coord1, float *Coord2, float *Coord3, float *Coord4)
{
  double Comp[3][3], ScalarProd, TripleScalarProd, AbsTorsAng;
  double Perp_123[3], Perp_234[3], Len_Perp_123, Len_Perp_234;
  int i, j, k;

  /* Find the components of the three bond vectors */
  for( i=0; i<3; i++ ) {
    Comp[0][i] = (double)(Coord2[i]-Coord1[i]);
    Comp[1][i] = (double)(Coord3[i]-Coord2[i]);
    Comp[2][i] = (double)(Coord4[i]-Coord3[i]);
  }

  /* Calculate vectors perpendicular to the planes 123 and 234 */
  Len_Perp_123 = 0.0; Len_Perp_234 = 0.0;
  for( i=0; i<3; i++ ) {
    j = (i+1)%3;
    k = (j+1)%3;
    Perp_123[i] = Comp[0][j]*Comp[1][k] - Comp[0][k]*Comp[1][j];
    Perp_234[i] = Comp[1][j]*Comp[2][k] - Comp[1][k]*Comp[2][j];
    Len_Perp_123 += Perp_123[i]*Perp_123[i];
    Len_Perp_234 += Perp_234[i]*Perp_234[i];
  }

  Len_Perp_123 = sqrt(Len_Perp_123);
  Len_Perp_234 = sqrt(Len_Perp_234);

  /* Normalize the vectors perpendicular to 123 and 234 */
  for( i=0; i<3; i++ ) {
    Perp_123[i] /= Len_Perp_123;
    Perp_234[i] /= Len_Perp_234;
  }

  /* Find the scalar product of the unit normals */
  ScalarProd = 0.0;
  for( i=0; i<3; i++ )
    ScalarProd += Perp_123[i]*Perp_234[i];

  /* Find the absolute value of the torsion angle */
  if( ScalarProd > 0.0 && fabs(ScalarProd - 1.0) < Eps )
    ScalarProd -= Eps;
  else
  if( ScalarProd < 0.0 && fabs(ScalarProd + 1.0) < Eps )
    ScalarProd += Eps;
  AbsTorsAng = RADDEG*acos(ScalarProd);

  /* Find the triple scalar product of the three bond vectors */
  TripleScalarProd = 0.0;
  for( i=0; i<3; i++ )
    TripleScalarProd += Comp[0][i]*Perp_234[i];

  /* Torsion angle has the sign of the triple scalar product */
  return( (TripleScalarProd > 0.0) ? (float)AbsTorsAng : (float)(-AbsTorsAng) );

}
/*************************************************************************
**                                                                      **
** INPUT:  *Coord1   Coordinates of the first point                     **
**         *Coord2   Coordinates of the second point                    **
**                                                                      **
** RETURNS:          Distance between two points                        **
**                                                                      **
*************************************************************************/
float svt_stride::Dist(float *Coord1, float *Coord2)
{
  register int i;
  float Dist=0;

  for( i=0; i<3; i++ )
    Dist += (Coord1[i]-Coord2[i])*(Coord1[i]-Coord2[i]);

  return( sqrt(Dist) );
}

/*************************************************************************
**                                                                      **
** INPUT:  *Coord1   Coordinates of the first point                     **
**         *Coord2   Coordinates of the second point                    **
**         *Coord3   Coordinates of the third point                     **
**                                                                      **
** RETURNS:          Angle 1-2-3                                        **
**                                                                      **
*************************************************************************/
float svt_stride::Ang(float *Coord1, float *Coord2, float *Coord3)
{
  float Vector1[3], Vector2[3];
  double A, B, C, D;

  Vector1[0] = Coord1[0] - Coord2[0];
  Vector1[1] = Coord1[1] - Coord2[1];
  Vector1[2] = Coord1[2] - Coord2[2];

  Vector2[0] = Coord3[0] - Coord2[0];
  Vector2[1] = Coord3[1] - Coord2[1];
  Vector2[2] = Coord3[2] - Coord2[2];

  A = Vector1[0]*Vector2[0]+Vector1[1]*Vector2[1]+Vector1[2]*Vector2[2];
  B = sqrt( Vector1[0]*Vector1[0]+Vector1[1]*Vector1[1]+Vector1[2]*Vector1[2]);
  C = sqrt( Vector2[0]*Vector2[0]+Vector2[1]*Vector2[1]+Vector2[2]*Vector2[2]);

  D = A/(B*C);
  if( D > 0.0 && fabs(D - 1.0) < Eps )
    D -= Eps;
  else
  if( D < 0.0 && fabs(D + 1.0) < Eps )
    D += Eps;

  return((float)(RADDEG*acos(D)));
}

/*************************************************************************
**                                                                      **
** INPUT:  *Chain    Protein chain                                      **
**         *Res      Residue number                                     **
**                                                                      **
** OUTPUT: Chain->Rsd[Res]->Prop->Phi Phi torsional angle               **
**                                                                      **
*************************************************************************/
void svt_stride::PHI(CHAIN *Chain, int Res)
{

  int C_Prev, N_Curr, CA_Curr, C_Curr;
  RESIDUE *r, *rr;

  r = Chain->Rsd[Res];
  r->Prop->Phi = 360.0;

  if( Res == 0 )
    return;

  rr = Chain->Rsd[Res-1];

  if( FindAtom(Chain,Res-1,"C",&C_Prev) && FindAtom(Chain,Res,"N",&N_Curr)   &&
      FindAtom(Chain,Res,"CA",&CA_Curr) && FindAtom(Chain,Res,"C",&C_Curr)   &&
      Dist(rr->Coord[C_Prev],r->Coord[N_Curr]) < BREAKDIST ) {
    r->Prop->Phi = Torsion(rr->Coord[C_Prev],r->Coord[N_Curr],
			   r->Coord[CA_Curr],r->Coord[C_Curr]);
  }
}

/*************************************************************************
**                                                                      **
** INPUT:  *Chain    Protein chain                                      **
**         *Res      Residue number                                     **
**                                                                      **
** OUTPUT: Chain->Rsd[Res]->Prop->Psi  Psi torsional angle              **
**                                                                      **
*************************************************************************/
void svt_stride::PSI(CHAIN *Chain, int Res)
{

  int N_Curr, CA_Curr, C_Curr, N_Next;
  RESIDUE *r, *rr;

  r = Chain->Rsd[Res];
  r->Prop->Psi = 360.0;

  if( Res == Chain->NRes-1 )
    return;

  rr = Chain->Rsd[Res+1];

  if( FindAtom(Chain,Res,"N",&N_Curr) && FindAtom(Chain,Res,"CA",&CA_Curr) &&
      FindAtom(Chain,Res,"C",&C_Curr) && FindAtom(Chain,Res+1,"N",&N_Next) &&
      Dist(r->Coord[C_Curr],rr->Coord[N_Next]) < BREAKDIST ){

    r->Prop->Psi = Torsion(r->Coord[N_Curr],r->Coord[CA_Curr],
			   r->Coord[C_Curr],rr->Coord[N_Next]);
  }
}

/*************************************************************************
**                                                                      **
** INPUT:  *Chain    Protein chain                                      **
**         *Res      Residue number                                     **
**                                                                      **
** OUTPUT: *Omega    Omega torsional angle                              **
**                                                                      **
*************************************************************************/
void svt_stride::OMEGA(CHAIN *Chain, int Res)
{

  int CA_Prev, C_Prev, N_Curr, CA_Curr;
  RESIDUE *r, *rr;


  r = Chain->Rsd[Res];
  r->Prop->Omega = 360.0;

  if( Res == 0 )
    return;

  rr = Chain->Rsd[Res-1];

  if( FindAtom(Chain,Res-1,"CA",&CA_Prev) && FindAtom(Chain,Res-1,"C",&C_Prev)   &&
      FindAtom(Chain,Res,"N",&N_Curr)     && FindAtom(Chain,Res,"CA",&CA_Curr) ) {

    r->Prop->Omega = Torsion(rr->Coord[CA_Prev],rr->Coord[C_Prev],
			     r->Coord[N_Curr],r->Coord[CA_Curr]);
  }
}

/*************************************************************************
**                                                                      **
** Place atom X in the plane of atoms 1,2 and 3 given the               **
** distance |X-3| and angle 2-3-X                                       **
**                                                                      **
** INPUT: *Coord1, *Coord2, *Coord3  Coordinates of three atoms  in the **
**                                   plane                              **
**        Dist3X                     Distance between atom 3 and the    **
**                                   atom to be placed                  **
**        Ang23X                     Angle between atoms 2,3 and the    **
**                                   atom to be placed                  **
**                                                                      **
** OUTPUT: *Coordx                   Coordinates of the placed atom     **
**                                                                      **
*************************************************************************/


void svt_stride::Place123_X(float *Coord1, float *Coord2, float *Coord3, float Dist3X, float Ang23X,
	       float *CoordX)
{

/*

   Atom1
     \                     AtomX
      \ ^UnVect2            /
       \|                  /
       Atom2----------Atom3->UnVect1

*/

  float Length_23, Length_12;
  float Proj3X_1, Proj3X_2, Proj12_1, Proj12_2, Rad1, Rad2;
  float UnVect1[3], UnVect2[3];
  int i;

  Length_23 = Dist(Coord3,Coord2);
  Length_12 = Dist(Coord2,Coord1);
  Rad1 = RAD(180.0-Ang23X);
  Rad2 = RAD(Ang(Coord1,Coord2,Coord3)-90.0);
  Proj3X_1 = Dist3X*cos(Rad1);
  Proj3X_2 = Dist3X*sin(Rad1);
  Proj12_2 = cos(Rad2)*Length_12;
  Proj12_1 = sin(Rad2)*Length_12;

  for( i=0; i<3; i++ ) {
    UnVect1[i] = (Coord3[i]-Coord2[i])/Length_23;
    UnVect2[i] = ((Coord1[i]-Coord2[i]) - ( -UnVect1[i]*Proj12_1))/Proj12_2;
  }

  for( i=0; i<3; i++ )
    CoordX[i] = Proj3X_1*UnVect1[i]+Proj3X_2*UnVect2[i]+Coord3[i];
}


/*************************************************************************
**                                                                      **
** INPUT: *Vector1, Vector2                                             **
**                                                                      **
** OUTPUT: *Product          Vector pruduct of Vector1 and Vector2      **
**                                                                      **
*************************************************************************/
float svt_stride::VectorProduct(float *Vector1, float *Vector2, float *Product)
{

  int i, j, k;
  float ProductLength;

  ProductLength = 0.0;

  for( i=0; i<3; i++ ) {
    j = (i+1)%3;
    k = (j+1)%3;
    Product[i] = Vector1[j]*Vector2[k] - Vector1[k]*Vector2[j];
    ProductLength += Product[i]*Product[i];
  }

  return(sqrt(ProductLength));
}

/*************************************************************************
**                                                                      **
** Find projection of an atom to a plane                                **
**                                                                      **
** INPUT: *Coord1, *Coord2, *Coord3  Coordinates of three atoms in a    **
**                                   plance                             **
**        *Coord4                    Coordinates of the fourth atom     **
**                                                                      **
** OUTPUT: *Coord_Proj4_123          Coordinates of the fourth atom's   **
**                                   projection to the place            **
**                                                                      **
*************************************************************************/
void svt_stride::Project4_123(float *Coord1, float *Coord2, float *Coord3, float *Coord4,
		 float *Coord_Proj4_123)
{

/*
                          Atom4
   Atom3                  .
   \                     .
    \                   .
     \                 .  .Proj4_123
      \               ..
      Atom2-------Atom1

*/


  float Vector21[3], Vector23[3], Vector14[3], VectorNormal_123[3];
  float Length_21 = 0.0, Length_23 = 0.0, Length_14 = 0.0, NormalLength;
  float COS_Norm_14, Proj_14_Norm;
  int i;

  for( i=0; i<3; i++ ) {
    Vector21[i] = Coord1[i] - Coord2[i];
    Vector23[i] = Coord3[i] - Coord2[i];
    Vector14[i] = Coord4[i] - Coord1[i];
    Length_21 += Vector21[i]*Vector21[i];
    Length_23 += Vector23[i]*Vector23[i];
    Length_14 += Vector14[i]*Vector14[i];
  }

  Length_21 = sqrt(Length_21);
  Length_23 = sqrt(Length_23);
  Length_14 = sqrt(Length_14);

  NormalLength = VectorProduct(Vector21,Vector23,VectorNormal_123);

  for( i=0; i<3; i++ )
    VectorNormal_123[i] /= NormalLength;

  COS_Norm_14 = 0.0;

  for( i=0; i<3; i++ )
    COS_Norm_14 += VectorNormal_123[i]*Vector14[i];

  COS_Norm_14 /= (Length_14*NormalLength);

  if( COS_Norm_14 < 0.0 ) {
    COS_Norm_14 = fabs(COS_Norm_14);
    for( i=0; i<3; i++ )
      VectorNormal_123[i] = -VectorNormal_123[i];
  }

  Proj_14_Norm = Length_14*COS_Norm_14;

  for( i=0; i<3; i++ ) {
    VectorNormal_123[i] *= Proj_14_Norm;
    Coord_Proj4_123[i] = (Vector14[i] - VectorNormal_123[i]) + Coord1[i];
  }
}

double svt_stride::GetAtomRadius(char *AtomType)
{

  if( !strcmp(AtomType,"O") )
    return(1.40);
  else
  if( !strcmp(AtomType,"N") )
    return(1.65);
  else
  if( !strcmp(AtomType,"CA") )
    return(1.87);
  else
  if( !strcmp(AtomType,"C") )
    return(1.76);
  else
    return(1.80);
}

/*************************************************
 Calculate the hydrogen bond energy as defined by
 Boobbyer et al., 1989
**************************************************/

void svt_stride::GRID_Energy(float *CA2, float *C, float *O, float *H, float *N, COMMAND *Cmd, HBOND *HBond)
{

  float ProjH[3];

 /***** Distance dependence ( 8-6 potential ) ****/

  if( Cmd->Truncate && HBond->AccDonDist < RmGRID )
    HBond->AccDonDist = RmGRID;
  HBond->Er = CGRID/pow(HBond->AccDonDist,float(8.0)) - DGRID/pow(HBond->AccDonDist,float(6.0));

 /************** Angular dependance ****************/

 /* Find projection of the hydrogen on the O-C-CA plane */
  Project4_123(O,C,CA2,H,ProjH);


 /* Three angles determining the direction of the hydrogen bond */
  HBond->ti = fabs(180.0 - Ang(ProjH,O,C));
  HBond->to = Ang(H,O,ProjH);
  HBond->p  = Ang(N,H,O);

 /* Calculate both angle-dependent HB energy components Et and Ep */
  if( HBond->ti >= 0.0 && HBond->ti < 90.0 )
    HBond->Et = cos(RAD(HBond->to))*(0.9+0.1*sin(RAD(2*HBond->ti)));
  else
  if( HBond->ti >= 90.0 && HBond->ti < 110.0 )
    HBond->Et = K1GRID*cos(RAD(HBond->to))*
      (pow((K2GRID-pow(cos(RAD(HBond->ti)),2.0)),3.0));
  else
    HBond->Et = 0.0;

  if( HBond->p > 90.0 && HBond->p < 270.0 )
    HBond->Ep = pow(cos(RAD(HBond->p)),2.0);
  else
    HBond->Ep = 0.0;

    /******** Full hydrogen bond energy *********************/
  HBond->Energy = 1000.0*HBond->Er*HBond->Et*HBond->Ep;
}

#define Q -27888.0

/********************************************************
 Calculate the energy of polar interaction as defined by
 Kabsch and Sander (1983)
*********************************************************/

void svt_stride::DSSP_Energy(float *C, float *O, float *H, float *N,
		 HBOND *HBond)

/* Dummy not used, for compatibility with GRID_Energy */
{ HBond->Energy = Q/Dist(O,H) + Q/Dist(C,N) - Q/HBond->AccDonDist - Q/Dist(C,H); }

void svt_stride::Helix(CHAIN **Chain, int Cn, HBOND **HBond, COMMAND *Cmd, float **PhiPsiMap)
{

  int BondNumb, i;
  float *Prob, MCONST;
  RESIDUE **r;

  MCONST = 1+Cmd->C1_H;

  Prob = (float *)ckalloc(MAX_RES*sizeof(float));

  for( i=0; i<Chain[Cn]->NRes; i++ )
    Prob[i] = 0.0;


  for( i=0; i<Chain[Cn]->NRes-5; i++ ) {

    r = &Chain[Cn]->Rsd[i];

    if( r[0]->Prop->PhiZn != ERR && r[0]->Prop->PsiZn != ERR &&
        r[1]->Prop->PhiZn != ERR && r[1]->Prop->PsiZn != ERR &&
        r[2]->Prop->PhiZn != ERR && r[2]->Prop->PsiZn != ERR &&
        r[3]->Prop->PhiZn != ERR && r[3]->Prop->PsiZn != ERR &&
        r[4]->Prop->PhiZn != ERR && r[4]->Prop->PsiZn != ERR ) {

      if( (BondNumb = FindPolInt(HBond,r[4],r[0])) != ERR ) {
	Prob[i] = HBond[BondNumb]->Energy*(MCONST+Cmd->C2_H*
	    0.5*(PhiPsiMap[r[0]->Prop->PhiZn][r[0]->Prop->PsiZn]+
		 PhiPsiMap[r[4]->Prop->PhiZn][r[4]->Prop->PsiZn]));

      }
    }
  }

  for( i=0; i<Chain[Cn]->NRes-5; i++ ) {

    if( Prob[i] < Cmd->Treshold_H1 && Prob[i+1] < Cmd->Treshold_H1 ) {

      r = &Chain[Cn]->Rsd[i];

      r[1]->Prop->Asn = 'H';
      r[2]->Prop->Asn = 'H';
      r[3]->Prop->Asn = 'H';
      r[4]->Prop->Asn = 'H';
      if( r[0]->Prop->PhiZn!= ERR && r[0]->Prop->PsiZn != ERR &&
	  PhiPsiMap[r[0]->Prop->PhiZn][r[0]->Prop->PsiZn] > Cmd->Treshold_H3 )
	r[0]->Prop->Asn = 'H';
      if( r[5]->Prop->PhiZn != ERR && r[5]->Prop->PsiZn != ERR &&
	  PhiPsiMap[r[5]->Prop->PhiZn][r[5]->Prop->PsiZn] > Cmd->Treshold_H4 )
	r[5]->Prop->Asn = 'H';
    }
  }

  for( i=0; i<Chain[Cn]->NRes-4; i++ ) {

    r = &Chain[Cn]->Rsd[i];

    if(  FindBnd(HBond,r[3],r[0]) != ERR && FindBnd(HBond,r[4],r[1]) != ERR &&
       /*************************** This should be improved **************************/
         ( (r[1]->Prop->Asn != 'H' && r[2]->Prop->Asn != 'H') ||
	   (r[2]->Prop->Asn != 'H' && r[3]->Prop->Asn != 'H') ) )
       /******************************************************************************/
      {
	r[1]->Prop->Asn = 'G';
	r[2]->Prop->Asn = 'G';
	r[3]->Prop->Asn = 'G';
      }
  }

  for( i=0; i<Chain[Cn]->NRes-6; i++ ) {

    r = &Chain[Cn]->Rsd[i];

    if( FindBnd(HBond,r[5],r[0]) != ERR && FindBnd(HBond,r[6],r[1]) != ERR &&
        r[1]->Prop->Asn == 'C' && r[2]->Prop->Asn == 'C' &&
        r[3]->Prop->Asn == 'C' && r[4]->Prop->Asn == 'C' &&
        r[5]->Prop->Asn == 'C' ) {
      r[1]->Prop->Asn = 'I';
      r[2]->Prop->Asn = 'I';
      r[3]->Prop->Asn = 'I';
      r[4]->Prop->Asn = 'I';
      r[5]->Prop->Asn = 'I';
    }
  }

  if( Cmd->Info ) {
    fprintf(stdout,"%s%c\n",Chain[Cn]->File,Chain[Cn]->Id);

    for( i=0; i<Chain[Cn]->NRes-4; i++ ) {

      r = &Chain[Cn]->Rsd[i];

      if( r[0]->Prop->PhiZn != ERR && r[0]->Prop->PsiZn != ERR &&
	  r[4]->Prop->PhiZn != ERR && r[4]->Prop->PsiZn != ERR ) {

	fprintf(stdout,"%s (%d) %c %10.7f %8.5f %8.5f | %4d %4d\n",
		r[0]->PDB_ResNumb,i,r[0]->Prop->Asn,Prob[i],
		PhiPsiMap[r[0]->Prop->PhiZn][r[0]->Prop->PsiZn],
		PhiPsiMap[r[4]->Prop->PhiZn][r[4]->Prop->PsiZn],
		r[4]->Prop->PhiZn,r[4]->Prop->PsiZn);
      }
    }
  }
  free(Prob);
}

int svt_stride::FindDnr(CHAIN *Chain, DONOR **Dnr, int *NDnr, COMMAND *Cmd)
{

  int Res, dc;
  char Rsd[RES_FIELD];

  dc = *NDnr;

  for( Res=0; Res<Chain->NRes; Res++ ) {

    strcpy(Rsd,Chain->Rsd[Res]->ResType);

    DefineDnr(Chain,Dnr,&dc,Res,Nsp2,Peptide,1.90,0);

    if( !Cmd->SideChainHBond ) continue;

    if( !strcmp(Rsd,"TRP") )
      DefineDnr(Chain,Dnr,&dc,Res,Nsp2,Trp,1.90,0);
    else if( !strcmp(Rsd,"ASN") ) DefineDnr(Chain,Dnr,&dc,Res,Nsp2,Asn,1.90,0);
    else if( !strcmp(Rsd,"GLN") ) DefineDnr(Chain,Dnr,&dc,Res,Nsp2,Gln,1.90,0);
    else if( !strcmp(Rsd,"ARG") ) {
      DefineDnr(Chain,Dnr,&dc,Res,Nsp2,Arg,1.90,1);
      DefineDnr(Chain,Dnr,&dc,Res,Nsp2,Arg,1.90,2);
      DefineDnr(Chain,Dnr,&dc,Res,Nsp2,Arg,1.90,3);
    }
    else if( !strcmp(Rsd,"HIS") ) {
      DefineDnr(Chain,Dnr,&dc,Res,Nsp2,His,1.90,1);
      DefineDnr(Chain,Dnr,&dc,Res,Nsp2,His,1.90,2);
    }
    else if( !strcmp(Rsd,"LYS") ) DefineDnr(Chain,Dnr,&dc,Res,Nsp3,Lys,2.10,0);
    else if( !strcmp(Rsd,"SER") ) DefineDnr(Chain,Dnr,&dc,Res,Osp3,Ser,1.70,0);
    else if( !strcmp(Rsd,"THR") ) DefineDnr(Chain,Dnr,&dc,Res,Osp3,Thr,1.70,0);
    else if( !strcmp(Rsd,"TYR") ) DefineDnr(Chain,Dnr,&dc,Res,Osp2,Tyr,1.70,0);
  }

  *NDnr = dc;
  return(dc);
}

int svt_stride::DefineDnr(CHAIN *Chain, DONOR **Dnr, int *dc, int Res, enum HYBRID Hybrid, enum MGROUP Group,
	      float HB_Radius, int N)
{

  Dnr[*dc] = (DONOR *)ckalloc(sizeof(DONOR));

  Dnr[*dc]->Chain = Chain;
  Dnr[*dc]->D_Res = Res;
  if( Group != Peptide )
    Dnr[*dc]->DD_Res = Res;
  else
    Dnr[*dc]->DD_Res = Res-1;
  Dnr[*dc]->DDI_Res = Res;
  Dnr[*dc]->Hybrid = Hybrid;
  Dnr[*dc]->Group = Group;
  Dnr[*dc]->HB_Radius = HB_Radius;

  if( Group == Peptide ) {
    if( Res != 0 ) {
      FindAtom(Chain,Res,"N",&Dnr[*dc]->D_At);
      FindAtom(Chain,Res-1,"C",&Dnr[*dc]->DD_At);
    }
    else {
      Dnr[*dc]->D_At  = ERR;
      Dnr[*dc]->DD_At = ERR;
    }
    FindAtom(Chain,Res,"CA",&Dnr[*dc]->DDI_At);
    FindAtom(Chain,Res,"H",&Dnr[*dc]->H);
  }
  else if( Group == Trp ) {
    FindAtom(Chain,Res,"NE1",&Dnr[*dc]->D_At);
    FindAtom(Chain,Res,"CE2",&Dnr[*dc]->DD_At);
    FindAtom(Chain,Res,"CD1",&Dnr[*dc]->DDI_At);
  }
  else if( Group == Asn ) {
    FindAtom(Chain,Res,"ND1",&Dnr[*dc]->D_At);
    FindAtom(Chain,Res,"CG",&Dnr[*dc]->DD_At);
    FindAtom(Chain,Res,"CB",&Dnr[*dc]->DDI_At);
  }
  else if( Group == Gln ) {
    FindAtom(Chain,Res,"NE2",&Dnr[*dc]->D_At);
    FindAtom(Chain,Res,"CD",&Dnr[*dc]->DD_At);
    FindAtom(Chain,Res,"CG",&Dnr[*dc]->DDI_At);
  }
  else if( Group == Arg ) {
    if( N == 1 ) {
      FindAtom(Chain,Res,"NE",&Dnr[*dc]->D_At);
      FindAtom(Chain,Res,"CZ",&Dnr[*dc]->DD_At);
      FindAtom(Chain,Res,"CD",&Dnr[*dc]->DDI_At);
    }
    else
    if( N == 2 ) {
      FindAtom(Chain,Res,"NH1",&Dnr[*dc]->D_At);
      FindAtom(Chain,Res,"CZ",&Dnr[*dc]->DD_At);
      FindAtom(Chain,Res,"NE",&Dnr[*dc]->DDI_At);
    }
    else
    if( N == 3 ) {
      FindAtom(Chain,Res,"NH2",&Dnr[*dc]->D_At);
      FindAtom(Chain,Res,"CZ",&Dnr[*dc]->DD_At);
      FindAtom(Chain,Res,"NE",&Dnr[*dc]->DDI_At);
    }
  }
  else if( Group == His ) {
      if( N == 1 ) {
	FindAtom(Chain,Res,"ND1",&Dnr[*dc]->D_At);
	FindAtom(Chain,Res,"CG",&Dnr[*dc]->DD_At);
	FindAtom(Chain,Res,"CE1",&Dnr[*dc]->DDI_At);
      }
      else if( N == 2 ) {
	FindAtom(Chain,Res,"NE2",&Dnr[*dc]->D_At);
	FindAtom(Chain,Res,"CE1",&Dnr[*dc]->DD_At);
	FindAtom(Chain,Res,"CD2",&Dnr[*dc]->DDI_At);
      }
    }
  else if( Group == Tyr ) {
    FindAtom(Chain,Res,"OH",&Dnr[*dc]->D_At);
    FindAtom(Chain,Res,"CZ",&Dnr[*dc]->DD_At);
    FindAtom(Chain,Res,"CE1",&Dnr[*dc]->DDI_At);
  }
  else if( Group == Lys ) {
    FindAtom(Chain,Res,"NZ",&Dnr[*dc]->D_At);
    FindAtom(Chain,Res,"CE",&Dnr[*dc]->DD_At);
  }
  else if( Group == Ser ) {
    FindAtom(Chain,Res,"OG",&Dnr[*dc]->D_At);
    FindAtom(Chain,Res,"CB",&Dnr[*dc]->DD_At);
  }
  else if( Group == Thr ) {
    FindAtom(Chain,Res,"OG1",&Dnr[*dc]->D_At);
    FindAtom(Chain,Res,"CB",&Dnr[*dc]->DD_At);
  }

  if( Dnr[*dc]->H == ERR || Dnr[*dc]->D_At   == ERR || Dnr[*dc]->DD_At  == ERR ||
      (Dnr[*dc]->DDI_At == ERR && (Hybrid == Nsp2 || Hybrid == Osp2 )) ) {
    free(Dnr[*dc]); return(FAILURE);
  }
  else (*dc)++;
  return(SUCCESS);
}


int svt_stride::FindAcc(CHAIN *Chain, ACCEPTOR **Acc, int *NAcc, COMMAND *Cmd)
{

  int Res, ac;
  char Rsd[RES_FIELD];

  ac = *NAcc;

  for( Res=0; Res<Chain->NRes; Res++ ) {
    strcpy(Rsd,Chain->Rsd[Res]->ResType);

    DefineAcceptor(Chain,Acc,&ac,Res,Osp2,Peptide,1.60,0);

    if( !Cmd->SideChainHBond ) continue;

    if( !strcmp(Rsd,"HIS") ) {
      DefineAcceptor(Chain,Acc,&ac,Res,Nsp2,His,1.60,0);
      DefineAcceptor(Chain,Acc,&ac,Res,Nsp2,His,1.60,0);
    }
    else if( !strcmp(Rsd,"SER") ) DefineAcceptor(Chain,Acc,&ac,Res,Osp3,Ser,1.70,0);
    else if( !strcmp(Rsd,"THR") ) DefineAcceptor(Chain,Acc,&ac,Res,Osp3,Thr,1.70,0);
    else if( !strcmp(Rsd,"ASN") ) DefineAcceptor(Chain,Acc,&ac,Res,Osp2,Asn,1.60,0);
    else if( !strcmp(Rsd,"GLN") ) DefineAcceptor(Chain,Acc,&ac,Res,Osp2,Gln,1.60,0);
    else if( !strcmp(Rsd,"ASP") ) {
      DefineAcceptor(Chain,Acc,&ac,Res,Osp2,Asp,1.60,1);
      DefineAcceptor(Chain,Acc,&ac,Res,Osp2,Asp,1.60,2);
    }
    else if( !strcmp(Rsd,"GLU") ) {
      DefineAcceptor(Chain,Acc,&ac,Res,Osp2,Glu,1.60,1);
      DefineAcceptor(Chain,Acc,&ac,Res,Osp2,Glu,1.60,2);
    }
    else if( !strcmp(Rsd,"TYR") ) DefineAcceptor(Chain,Acc,&ac,Res,Osp2,Tyr,1.70,0);
    else if( !strcmp(Rsd,"MET") ) DefineAcceptor(Chain,Acc,&ac,Res,Ssp3,Met,1.95,0);
    else if( !strcmp(Rsd,"CYS") ) DefineAcceptor(Chain,Acc,&ac,Res,Ssp3,Cys,1.70,0);
    }

  *NAcc = ac;
  return(ac);
}


int svt_stride::DefineAcceptor(CHAIN *Chain, ACCEPTOR **Acc, int *ac, int Res, enum HYBRID Hybrid,
		   enum MGROUP Group, float HB_Radius, int N)
{

  Acc[*ac] = (ACCEPTOR *)ckalloc(sizeof(ACCEPTOR));

  Acc[*ac]->Chain = Chain;
  Acc[*ac]->A_Res    = Res;
  Acc[*ac]->AA_Res   = Res;
  Acc[*ac]->AA2_Res   = Res;
  Acc[*ac]->Hybrid    = Hybrid;
  Acc[*ac]->Group     = Group;
  Acc[*ac]->HB_Radius = HB_Radius;

  if( Group == Peptide ) {
    if( Res != Chain->NRes-1 ) {
      FindAtom(Chain,Res,"O",&Acc[*ac]->A_At);
      FindAtom(Chain,Res,"C",&Acc[*ac]->AA_At);
    }
    else {
      Acc[*ac]->A_At = ERR;
      Acc[*ac]->AA_At = ERR;
    }
    FindAtom(Chain,Res,"CA",&Acc[*ac]->AA2_At);
  }
  else if( Group == His ) {
    if( N == 1 ) {
      FindAtom(Chain,Res,"ND1",&Acc[*ac]->A_At);
      FindAtom(Chain,Res,"CG",&Acc[*ac]->AA_At);
      FindAtom(Chain,Res,"CE1",&Acc[*ac]->AA2_At);
    }
    else if( N == 2 ) {
      FindAtom(Chain,Res,"NE2",&Acc[*ac]->A_At);
      FindAtom(Chain,Res,"CE1",&Acc[*ac]->AA_At);
      FindAtom(Chain,Res,"CD2",&Acc[*ac]->AA2_At);
    }
  }
  else if( Group == Asn ) {
    FindAtom(Chain,Res,"OD1",&Acc[*ac]->A_At);
    FindAtom(Chain,Res,"CG",&Acc[*ac]->AA_At);
    FindAtom(Chain,Res,"CB",&Acc[*ac]->AA2_At);
  }
  else if( Group == Gln ) {
    FindAtom(Chain,Res,"OE1",&Acc[*ac]->A_At);
    FindAtom(Chain,Res,"CD",&Acc[*ac]->AA_At);
    FindAtom(Chain,Res,"CG",&Acc[*ac]->AA2_At);
  }
  else if( Group == Asp ) {
    if( N == 1 ) {
      FindAtom(Chain,Res,"OD1",&Acc[*ac]->A_At);
      FindAtom(Chain,Res,"CG",&Acc[*ac]->AA_At);
      FindAtom(Chain,Res,"CB",&Acc[*ac]->AA2_At);
    }
    else if( N == 2 ) {
      FindAtom(Chain,Res,"ND2",&Acc[*ac]->A_At);
      FindAtom(Chain,Res,"CG",&Acc[*ac]->AA_At);
      FindAtom(Chain,Res,"CB",&Acc[*ac]->AA2_At);
    }
  }
  else if( Group == Glu ) {
    if( N == 1 ) {
      FindAtom(Chain,Res,"OE1",&Acc[*ac]->A_At);
      FindAtom(Chain,Res,"CD",&Acc[*ac]->AA_At);
      FindAtom(Chain,Res,"CG",&Acc[*ac]->AA2_At);
    }
    else if( N == 2 ) {
      FindAtom(Chain,Res,"NE2",&Acc[*ac]->A_At);
      FindAtom(Chain,Res,"CD",&Acc[*ac]->AA_At);
      FindAtom(Chain,Res,"CG",&Acc[*ac]->AA2_At);
    }
  }
  else if( Group == Tyr ) {
    FindAtom(Chain,Res,"OH",&Acc[*ac]->A_At);
    FindAtom(Chain,Res,"CZ",&Acc[*ac]->AA_At);
    FindAtom(Chain,Res,"CE1",&Acc[*ac]->AA2_At);
  }
  else if( Group == Ser ) {
    FindAtom(Chain,Res,"OG",&Acc[*ac]->A_At);
    FindAtom(Chain,Res,"CB",&Acc[*ac]->AA_At);
  }
  else if( Group == Thr ) {
    FindAtom(Chain,Res,"OG1",&Acc[*ac]->A_At);
    FindAtom(Chain,Res,"CB",&Acc[*ac]->AA_At);
  }
  else if( Group == Met ) {
    FindAtom(Chain,Res,"SD",&Acc[*ac]->A_At);
    FindAtom(Chain,Res,"CG",&Acc[*ac]->AA_At);
  }
  else if( Group == Cys ) {
    FindAtom(Chain,Res,"SG",&Acc[*ac]->A_At);
    FindAtom(Chain,Res,"CB",&Acc[*ac]->AA_At);
  }

  if( Acc[*ac]->A_At   == ERR || Acc[*ac]->AA_At  == ERR ||
      (Acc[*ac]->AA2_At == ERR && (Hybrid == Nsp2 || Hybrid == Osp2 )) ) {
    free(Acc[*ac]); return(FAILURE);
  }
  else (*ac)++;
  return(SUCCESS);
}


int svt_stride::FindHydrogenBonds(CHAIN **Chain, int NChain, HBOND **HBond, COMMAND *Cmd)
{
  //DONOR **Dnr;
  //ACCEPTOR **Acc;
  //int NDnr=0, NAcc=0;
  BOOLEAN *BondedDonor, *BondedAcceptor;
  int dc, ac, ccd, cca, cc, hc=0, i;
  //void (*HBOND_Energy)(float *,float *,float *,float *,float *,COMMAND *,HBOND *);
  BUFFER Text;

  NDnr = 0;
  NAcc = 0;

  Dnr = (DONOR **)ckalloc(MAXDONOR*sizeof(DONOR *));
  for(i=0;i<MAXDONOR;i++) Dnr[i] = NULL;
  Acc = (ACCEPTOR **)ckalloc(MAXACCEPTOR*sizeof(ACCEPTOR *));
  for(i=0;i<MAXACCEPTOR;i++) Acc[i] = NULL;

  for( cc=0; cc<NChain; cc++ ) {
    FindDnr(Chain[cc],Dnr,&NDnr,Cmd);
    FindAcc(Chain[cc],Acc,&NAcc,Cmd);
  }

  BondedDonor    = (BOOLEAN *)ckalloc(NDnr*sizeof(BOOLEAN));
  BondedAcceptor = (BOOLEAN *)ckalloc(NAcc*sizeof(BOOLEAN));
  for( i=0; i<NDnr; i++ )
    BondedDonor[i] = NO;
  for( i=0; i<NAcc; i++ )
    BondedAcceptor[i] = NO;

  //if( Cmd->EnergyType == 'D' )
    //HBOND_Energy = DSSP_Energy;

  //else
    //HBOND_Energy = GRID_Energy;

  for( dc=0; dc<NDnr; dc++ ) {

      if( Dnr[dc]->Group != Peptide &&
         !Cmd->SideChainHBond ) continue;

    for( ac=0; ac<NAcc; ac++ ) {

      if( abs(Acc[ac]->A_Res - Dnr[dc]->D_Res) < 2 && Acc[ac]->Chain->Id == Dnr[dc]->Chain->Id )
	continue;

       if( Acc[ac]->Group != Peptide && !Cmd->SideChainHBond ) continue;

      if( hc == MAXHYDRBOND ){
    	  die("Number of hydrogen bonds exceeds current limit of %d \n",
	    MAXHYDRBOND);
      return 0;
      }

      HBond[hc] = (HBOND *)ckalloc(sizeof(HBOND));

      HBond[hc]->ExistHydrBondRose = NO;
      HBond[hc]->ExistHydrBondBaker = NO;
      HBond[hc]->ExistPolarInter = NO;
      HBond[hc]->Dnr = 0;
      HBond[hc]->Acc = 0;

      if( (HBond[hc]->AccDonDist =
	   Dist(Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->D_At],
		Acc[ac]->Chain->Rsd[Acc[ac]->A_Res]->Coord[Acc[ac]->A_At]) ) <=
	 Cmd->DistCutOff ) {


	if( Cmd->MainChainPolarInt && Dnr[dc]->Group == Peptide &&
	   Acc[ac]->Group == Peptide && Dnr[dc]->H != ERR) {
		//HBOND_Energy
		GRID_Energy(Acc[ac]->Chain->Rsd[Acc[ac]->AA2_Res]->Coord[Acc[ac]->AA2_At],
                      Acc[ac]->Chain->Rsd[Acc[ac]->AA_Res]->Coord[Acc[ac]->AA_At],
                      Acc[ac]->Chain->Rsd[Acc[ac]->A_Res]->Coord[Acc[ac]->A_At],
                      Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->H],
                      Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->D_At],
                      Cmd,HBond[hc]);

	  if( HBond[hc]->Energy < -10.0 &&
	     ( (Cmd->EnergyType == 'G' && fabs(HBond[hc]->Et) > Eps &&
		fabs(HBond[hc]->Ep) > Eps ) || Cmd->EnergyType != 'G' ) )
	    HBond[hc]->ExistPolarInter = YES;
	}

	if( Cmd->MainChainHBond &&
	    (HBond[hc]->OHDist =
	     Dist(Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->H],
		  Acc[ac]->Chain->Rsd[Acc[ac]->A_Res]->Coord[Acc[ac]->A_At])) <= 2.5 &&
	    (HBond[hc]->AngNHO =
	     Ang(Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->D_At],
		 Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->H],
		 Acc[ac]->Chain->Rsd[Acc[ac]->A_Res]->Coord[Acc[ac]->A_At])) >= 90.0 &&
	     HBond[hc]->AngNHO <= 180.0 &&
	    (HBond[hc]->AngCOH =
	     Ang(Acc[ac]->Chain->Rsd[Acc[ac]->AA_Res]->Coord[Acc[ac]->AA_At],
		 Acc[ac]->Chain->Rsd[Acc[ac]->A_Res]->Coord[Acc[ac]->A_At],
		 Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->H])) >= 90.0 &&

	     HBond[hc]->AngCOH <= 180.0 )
	  HBond[hc]->ExistHydrBondBaker = YES;

	if( Cmd->MainChainHBond &&
	   HBond[hc]->AccDonDist <= Dnr[dc]->HB_Radius+Acc[ac]->HB_Radius ) {

	  HBond[hc]->AccAng =
	    Ang(Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->D_At],
		Acc[ac]->Chain->Rsd[Acc[ac]->A_Res]->Coord[Acc[ac]->A_At],
		Acc[ac]->Chain->Rsd[Acc[ac]->AA_Res]->Coord[Acc[ac]->AA_At]);

	  if( ( ( Acc[ac]->Hybrid == Nsp2 || Acc[ac]->Hybrid == Osp2 ) &&
	       ( HBond[hc]->AccAng >= MINACCANG_SP2 &&
		HBond[hc]->AccAng <= MAXACCANG_SP2 ) ) ||
	     ( ( Acc[ac]->Hybrid == Ssp3 ||  Acc[ac]->Hybrid == Osp3 ) &&
	      ( HBond[hc]->AccAng >= MINACCANG_SP3 &&
	       HBond[hc]->AccAng <= MAXACCANG_SP3 ) ) ) {

	    HBond[hc]->DonAng =
	      Ang(Acc[ac]->Chain->Rsd[Acc[ac]->A_Res]->Coord[Acc[ac]->A_At],
		  Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->D_At],
		  Dnr[dc]->Chain->Rsd[Dnr[dc]->DD_Res]->Coord[Dnr[dc]->DD_At]);

	    if( ( ( Dnr[dc]->Hybrid == Nsp2 || Dnr[dc]->Hybrid == Osp2 ) &&
		 ( HBond[hc]->DonAng >= MINDONANG_SP2 &&
		  HBond[hc]->DonAng <= MAXDONANG_SP2 ) ) ||
	       ( ( Dnr[dc]->Hybrid == Nsp3 || Dnr[dc]->Hybrid == Osp3 ) &&
		( HBond[hc]->DonAng >= MINDONANG_SP3 &&
		 HBond[hc]->DonAng <= MAXDONANG_SP3 ) ) ) {

	      if( Dnr[dc]->Hybrid == Nsp2 || Dnr[dc]->Hybrid == Osp2 ) {
		HBond[hc]->AccDonAng =
		  fabs(Torsion(Dnr[dc]->Chain->Rsd[Dnr[dc]->DDI_Res]->Coord[Dnr[dc]->DDI_At],
			       Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->D_At],
			       Dnr[dc]->Chain->Rsd[Dnr[dc]->DD_Res]->Coord[Dnr[dc]->DD_At],
			       Acc[ac]->Chain->Rsd[Acc[ac]->A_Res]->Coord[Acc[ac]->A_At]));

		if( HBond[hc]->AccDonAng > 90.0 && HBond[hc]->AccDonAng < 270.0 )
		  HBond[hc]->AccDonAng = fabs(180.0 - HBond[hc]->AccDonAng);

	      }

	      if( Acc[ac]->Hybrid == Nsp2 || Acc[ac]->Hybrid == Osp2 ) {
		HBond[hc]->DonAccAng =
		  fabs(Torsion(Dnr[dc]->Chain->Rsd[Dnr[dc]->D_Res]->Coord[Dnr[dc]->D_At],
			       Acc[ac]->Chain->Rsd[Acc[ac]->A_Res]->Coord[Acc[ac]->A_At],
			       Acc[ac]->Chain->Rsd[Acc[ac]->AA_Res]->Coord[Acc[ac]->AA_At],
			       Acc[ac]->Chain->Rsd[Acc[ac]->AA2_Res]->Coord[Acc[ac]->AA2_At]));

		if(HBond[hc]->DonAccAng > 90.0 && HBond[hc]->DonAccAng < 270.0)
		  HBond[hc]->DonAccAng = fabs(180.0 - HBond[hc]->DonAccAng);

	      }

	      if( ( Dnr[dc]->Hybrid != Nsp2 && Dnr[dc]->Hybrid != Osp2 &&
		   Acc[ac]->Hybrid != Nsp2 && Acc[ac]->Hybrid != Osp2 ) ||
		 ( Acc[ac]->Hybrid != Nsp2 && Acc[ac]->Hybrid != Osp2 &&
		  ( Dnr[dc]->Hybrid == Nsp2 || Dnr[dc]->Hybrid == Osp2 ) &&
		  HBond[hc]->AccDonAng <= ACCDONANG ) ||
		 ( Dnr[dc]->Hybrid != Nsp2 && Dnr[dc]->Hybrid != Osp2 &&
		  ( Acc[ac]->Hybrid == Nsp2 || Acc[ac]->Hybrid == Osp2 ) &&
		  HBond[hc]->DonAccAng <= DONACCANG ) ||
		 ( ( Dnr[dc]->Hybrid == Nsp2 || Dnr[dc]->Hybrid == Osp2 ) &&
		  ( Acc[ac]->Hybrid == Nsp2 || Acc[ac]->Hybrid == Osp2 ) &&
		  HBond[hc]->AccDonAng <= ACCDONANG &&
		  HBond[hc]->DonAccAng <= DONACCANG ) )
		HBond[hc]->ExistHydrBondRose = YES;
	    }
	  }
	}

      }

      if( (HBond[hc]->ExistPolarInter && HBond[hc]->Energy < 0.0)
	  || HBond[hc]->ExistHydrBondRose || HBond[hc]->ExistHydrBondBaker ) {
	HBond[hc]->Dnr = Dnr[dc];
	HBond[hc]->Acc = Acc[ac];
	BondedDonor[dc] = YES;
	BondedAcceptor[ac] = YES;
	if( (ccd = FindChain(Chain,NChain,Dnr[dc]->Chain->Id)) != ERR ) {
	  if( Chain[ccd]->Rsd[Dnr[dc]->D_Res]->Inv->NBondDnr < MAXRESDNR )
	    Chain[ccd]->Rsd[Dnr[dc]->D_Res]->Inv->
	      HBondDnr[Chain[ccd]->Rsd[Dnr[dc]->D_Res]->Inv->NBondDnr++] = hc;
	  else
	    printf("Residue %s %s of chain %s%c is is involved in more than %d hydrogen bonds (%d)\n",
		Chain[ccd]->Rsd[Dnr[dc]->D_Res]->ResType,
		Chain[ccd]->Rsd[Dnr[dc]->D_Res]->PDB_ResNumb,
		Chain[ccd]->File,SpaceToDash(Chain[ccd]->Id),
		MAXRESDNR,Chain[ccd]->Rsd[Dnr[dc]->D_Res]->Inv->NBondDnr);
	}
	if( (cca  = FindChain(Chain,NChain,Acc[ac]->Chain->Id)) != ERR ) {
	  if( Chain[cca]->Rsd[Acc[ac]->A_Res]->Inv->NBondAcc < MAXRESACC )
	    Chain[cca]->Rsd[Acc[ac]->A_Res]->Inv->
	      HBondAcc[Chain[cca]->Rsd[Acc[ac]->A_Res]->Inv->NBondAcc++] = hc;
	  else
	    printf("Residue %s %s of chain %s%c is is involved in more than %d hydrogen bonds (%d)\n",
		Chain[cca]->Rsd[Acc[ac]->A_Res]->ResType,
		Chain[cca]->Rsd[Acc[ac]->A_Res]->PDB_ResNumb,
		Chain[cca]->File,SpaceToDash(Chain[cca]->Id),
		MAXRESDNR,Chain[cca]->Rsd[Acc[ac]->A_Res]->Inv->NBondAcc);
	}
	if( ccd != cca && ccd != ERR ) {
	  Chain[ccd]->Rsd[Dnr[dc]->D_Res]->Inv->InterchainHBonds = YES;
	  Chain[cca]->Rsd[Acc[ac]->A_Res]->Inv->InterchainHBonds = YES;
	  if( HBond[hc]->ExistHydrBondRose ) {
	    Chain[0]->NHydrBondInterchain++;
	    Chain[0]->NHydrBondTotal++;
	  }
	}
	else
	if( ccd == cca && ccd != ERR && HBond[hc]->ExistHydrBondRose ) {
	  Chain[ccd]->NHydrBond++;
	  Chain[0]->NHydrBondTotal++;
	}
	hc++;
      }
      else
	free(HBond[hc]);
    }
  }
  if( Cmd->Info )
    for( i=0; i<hc; i++ ) {
      if( HBond[i]->Energy < 0.0 ) {
	sprintf(Text,"%3d ",i);
	PrintHydrBond(Text,HBond[i]);
      }
    }

  /*for( i=0; i<NDnr; i++ )
      if( !BondedDonor[i] )
      {
          free(Dnr[i]);
          Dnr[i] = 0;
      }
  free(Dnr);
  for( i=0; i<NAcc; i++ )
      if( !BondedAcceptor[i] )
      {
          free(Acc[i]);
          Acc[i] = 0;
      }
  free(Acc);*/

  if( NDnr )
    free(BondedDonor);
  if( NAcc )
    free(BondedAcceptor);

  return(hc);
}

void svt_stride::PrintHydrBond(char *Text, HBOND *HBond)
{

  fprintf(stdout,"HB %s %20s %3s %4s %4d %c <> %3s %4s %4d %c ",Text,
	  HBond->Dnr->Chain->File,
	  HBond->Dnr->Chain->Rsd[HBond->Dnr->D_Res]->ResType,
	  HBond->Dnr->Chain->Rsd[HBond->Dnr->D_Res]->PDB_ResNumb,
	  HBond->Dnr->D_Res,HBond->Dnr->Chain->Id,
	  HBond->Acc->Chain->Rsd[HBond->Acc->A_Res]->ResType,
	  HBond->Acc->Chain->Rsd[HBond->Acc->A_Res]->PDB_ResNumb,
	  HBond->Acc->A_Res,HBond->Acc->Chain->Id);

  fprintf(stdout," %7.1f ",HBond->AccDonDist);
  if( HBond->ExistPolarInter )
    fprintf(stdout,"%7.1f ",HBond->Energy);
  else
    fprintf(stdout,"XXXXXXX ");

  if( HBond->ExistHydrBondRose )
    fprintf(stdout,"YES ");
  else
    fprintf(stdout,"NO ");

  if( HBond->ExistHydrBondBaker )
    fprintf(stdout,"YES\n");
  else
    fprintf(stdout,"NO\n");



}

int svt_stride::FindPolInt(HBOND **HBond, RESIDUE *Res1, RESIDUE *Res2)
{

  register int i, j, hb;
  INVOLVED *p1, *p2;

  p1 = Res1->Inv;
  p2 = Res2->Inv;

  if( p1->NBondDnr && p2->NBondAcc ) {
    for( i=0; i<p1->NBondDnr; i++ ) {
      hb = p1->HBondDnr[i];
      for( j=0; j<p2->NBondAcc; j++ )
	if( hb == p2->HBondAcc[j] && HBond[hb]->ExistPolarInter )
	  return(hb);
    }
  }

  return(ERR);
}

int svt_stride::FindBnd(HBOND **HBond, RESIDUE *Res1, RESIDUE *Res2)
{

  register int i, j, hb;
  INVOLVED *p1, *p2;

  p1 = Res1->Inv;
  p2 = Res2->Inv;

  if( p1->NBondDnr && p2->NBondAcc ) {
    for( i=0; i<p1->NBondDnr; i++ ) {
      hb = p1->HBondDnr[i];
      for( j=0; j<p2->NBondAcc; j++ )
	if( hb == p2->HBondAcc[j] && HBond[hb]->ExistHydrBondRose )
	  return(hb);
    }
  }

  return(ERR);
}

int svt_stride::NoDoubleHBond(HBOND **HBond, int NHBond)
{

  int i, j, NExcl=0;

  for( i=0; i<NHBond-1; i++ )
    for( j=i+1; j<NHBond; j++ )
      if( HBond[i]->Dnr->D_Res == HBond[j]->Dnr->D_Res &&
	  HBond[i]->Dnr->Chain->Id == HBond[j]->Dnr->Chain->Id &&
	  HBond[i]->ExistPolarInter && HBond[j]->ExistPolarInter ) {
	if( HBond[i]->Energy < 5.0*HBond[j]->Energy ) {
	  HBond[j]->ExistPolarInter = NO;
	  NExcl++;
	}
	else
	if( HBond[j]->Energy < 5.0*HBond[i]->Energy ) {
	  HBond[i]->ExistPolarInter = NO;
	  NExcl++;
	}
      }

  return(NExcl);
}


void svt_stride::InitChain(CHAIN **Chain)
{

  *Chain = (CHAIN *)ckalloc(sizeof(CHAIN));

  (*Chain)->NRes                = 0;
  (*Chain)->NHetRes             = 0;
  (*Chain)->NonStandRes         = 0;
  (*Chain)->NHet                = 0;
  (*Chain)->NonStandAtom        = 0;
  (*Chain)->NHelix              = 0;
  (*Chain)->NSheet              = -1;
  (*Chain)->NTurn               = 0;
  (*Chain)->NAssignedTurn       = 0;
  (*Chain)->NBond               = 0;
  (*Chain)->NHydrBond           = 0;
  (*Chain)->NHydrBondTotal      = 0;
  (*Chain)->NHydrBondInterchain = 0;
  (*Chain)->Method              = XRay;
  (*Chain)->Ter                 = 0;
  (*Chain)->Resolution          = 0.0;

  (*Chain)->File                = (char           *)ckalloc(BUFSZ*sizeof(char));
  (*Chain)->Rsd                 = (RESIDUE       **)ckalloc(MAX_RES*sizeof(RESIDUE *));
  for(int i=0;i<MAX_RES;i++)
      (*Chain)->Rsd[i] = NULL;

  (*Chain)->HetRsd              = (HETERORESIDUE **)ckalloc(MAX_HETRES*sizeof(HETERORESIDUE *));
  for(int i=0;i<MAX_HETRES;i++)
      (*Chain)->HetRsd[i] = NULL;

  (*Chain)->Het                 = (HET           **)ckalloc(MAX_HET*sizeof(HET *));
  for(int i=0;i<MAX_HET;i++)
      (*Chain)->Het[i] = NULL;

  (*Chain)->Helix               = (HELIX         **)ckalloc(MAX_HELIX*sizeof(HELIX *));
  for(int i=0;i<MAX_HELIX;i++)
      (*Chain)->Helix[i] = NULL;

  (*Chain)->Sheet               = (SHEET         **)ckalloc(MAX_SHEET*sizeof(SHEET *));
  for(int i=0;i<MAX_SHEET;i++)
      (*Chain)->Sheet[i] = NULL;

  (*Chain)->Turn                = (TURN          **)ckalloc(MAX_TURN*sizeof(TURN *));
  for(int i=0;i<MAX_TURN;i++)
      (*Chain)->Turn[i] = NULL;

  (*Chain)->AssignedTurn        = (TURN          **)ckalloc(MAX_TURN*sizeof(TURN *));
  for(int i=0;i<MAX_TURN;i++)
      (*Chain)->AssignedTurn[i] = NULL;

  (*Chain)->SSbond              = (SSBOND        **)ckalloc(MAX_BOND*sizeof(SSBOND *));
  for(int i=0;i<MAX_BOND;i++)
      (*Chain)->SSbond[i] = NULL;

  (*Chain)->Info                = (char          **)ckalloc(MAX_INFO*sizeof(char *));
  for(int i=0;i<MAX_INFO;i++)
      (*Chain)->Info[i] = NULL;

  (*Chain)->Valid               = YES;
}

void svt_stride::FreeChain(CHAIN *Chain)
{
    int i;

    // (char           *)ckalloc(BUFSZ*sizeof(char));
    free(Chain->File);
    Chain->File = NULL;
    // (RESIDUE       **)ckalloc(MAX_RES*sizeof(RESIDUE *));
    for(i=0;i<MAX_RES;i++)
    {
        if (Chain->Rsd[i])
        {
            if (Chain->Rsd[i]->Inv)
                free(Chain->Rsd[i]->Inv);
            Chain->Rsd[i]->Inv = NULL;
            if (Chain->Rsd[i]->Prop)
                free(Chain->Rsd[i]->Prop);
            Chain->Rsd[i]->Prop = NULL;
            free(Chain->Rsd[i]);
            Chain->Rsd[i] = NULL;
        }
    }
    free(Chain->Rsd);
    Chain->Rsd = NULL;
    // (HETERORESIDUE **)ckalloc(MAX_HETRES*sizeof(HETERORESIDUE *));
    for(i=0;i<MAX_HETRES;i++)
        if (Chain->HetRsd[i])
        {
            free(Chain->HetRsd[i]);
            Chain->HetRsd[i] = NULL;
        }
    free(Chain->HetRsd);
    Chain->HetRsd = NULL;
    // (HET           **)ckalloc(MAX_HET*sizeof(HET *));
    for(i=0;i<MAX_HET;i++)
        if (Chain->Het[i])
        {
            free(Chain->Het[i]);
            Chain->Het[i] = NULL;
        }
    free(Chain->Het);
    Chain->Het = NULL;
    // (HELIX         **)ckalloc(MAX_HELIX*sizeof(HELIX *));
    for(i=0;i<MAX_HELIX;i++)
        if (Chain->Helix[i])
        {
            free(Chain->Helix[i]);
            Chain->Helix[i] = NULL;
        }
    free(Chain->Helix);
    Chain->Helix = NULL;
    // (SHEET         **)ckalloc(MAX_SHEET*sizeof(SHEET *));
    for(i=0;i<MAX_SHEET;i++)
        if (Chain->Sheet[i])
        {
            free(Chain->Sheet[i]);
            Chain->Sheet[i] = NULL;
        }
    free(Chain->Sheet);
    Chain->Sheet = NULL;
    // (TURN          **)ckalloc(MAX_TURN*sizeof(TURN *));
    for(i=0;i<MAX_TURN;i++)
        if (Chain->Turn[i])
        {
            free(Chain->Turn[i]);
            Chain->Turn[i] = NULL;
        }
    free(Chain->Turn);
    Chain->Turn = NULL;
    // (TURN          **)ckalloc(MAX_TURN*sizeof(TURN *));
    for(i=0;i<MAX_TURN;i++)
        if (Chain->AssignedTurn[i])
        {
            free(Chain->AssignedTurn[i]);
            Chain->AssignedTurn[i] = NULL;
        }
    free(Chain->AssignedTurn);
    Chain->AssignedTurn = NULL;
    // (SSBOND        **)ckalloc(MAX_BOND*sizeof(SSBOND *));
    for(i=0;i<MAX_BOND;i++)
        if (Chain->SSbond[i])
        {
            free(Chain->SSbond[i]);
            Chain->SSbond[i] = NULL;
        }
    free(Chain->SSbond);
    Chain->SSbond = NULL;
    // (char          **)ckalloc(MAX_INFO*sizeof(char *));
    for(i=0;i<MAX_INFO;i++)
        if (Chain->Info[i])
        {
            free(Chain->Info[i]);
            Chain->Info[i] = NULL;
        }
    free(Chain->Info);
    Chain->Info = NULL;
}

void svt_stride::Measure(CHAIN **Chain, int NChain, int El, FILE *Out)
{

  QUALITY QualDssp_H, QualDssp_E, Qual_H, Qual_E;
  int i, Cn, Flag1, Flag2;
  float Q2_, Q2_Dssp, O_, O_Dssp;
  float Content, AlphaCont, BetaCont, PerCor, PerCorDssp;
  int HelAlp, HelPI, Hel310, Sheet, Turn;
  int YYN_H, NYY_H, YNN_H, NNY_H, YYN_E, NYY_E, YNN_E, NNY_E;
  int Total_H, Better_H, Worse_H, Total_E, Better_E, Worse_E;
  char Tmp[3][MAX_RES], *Asn, *PdbAsn, *DsspAsn;
  CHAIN *c;


  for( Cn=0; Cn<NChain; Cn++ ) {

    c = Chain[Cn];

    if( !c->Valid )
      continue;

    Asn     = (char *)ckalloc(c->NRes*sizeof(char));
    PdbAsn  = (char *)ckalloc(c->NRes*sizeof(char));
    DsspAsn = (char *)ckalloc(c->NRes*sizeof(char));

    ExtractAsn(Chain,Cn,Asn);
    ExtractPdbAsn(Chain,Cn,PdbAsn);
    ExtractDsspAsn(Chain,Cn,DsspAsn);

    for( i=0; i<c->NRes; i++ ) {
      if( Asn[i] != 'H' && Asn[i] != 'E' && Asn[i] != 'B' )
	Asn[i] = 'C';
      if( PdbAsn[i] != 'H' && PdbAsn[i] != 'E' )
	PdbAsn[i] = 'C';
      if( DsspAsn[i] != 'H' && DsspAsn[i] != 'E' && DsspAsn[i] != 'B' )
	DsspAsn[i] = 'C';
      if( Asn[i] == 'B' )
	Asn[i] = 'E';
      if( DsspAsn[i] == 'B' )
	DsspAsn[i] = 'E';
    }

    CorrectAsnDouble(Asn,DsspAsn,PdbAsn,c->NRes,'E','C');


    if( El ) {
      FullElement(Asn,PdbAsn,DsspAsn,c->NRes,'H',4,'z',&YYN_H,&NYY_H,&YNN_H,&NNY_H);
      FullElement(Asn,PdbAsn,DsspAsn,c->NRes,'E',1,'y',&YYN_E,&NYY_E,&YNN_E,&NNY_E);
      fprintf(Out,"DIFF_EL ");
    }
    else
      fprintf(Out,"DIFF ");

    Content = SecStrContent(c,&HelAlp,&HelPI,&Hel310,&Sheet,&Turn);
    AlphaCont = (float)HelAlp/(float)c->NRes;
    BetaCont  = (float)Sheet/(float)c->NRes;

    Presnell(PdbAsn,c->NRes,DsspAsn,c->NRes,'E',0.5,&Q2_Dssp,&O_Dssp);
    Presnell(PdbAsn,c->NRes,Asn,c->NRes,'E',0.5,&Q2_,&O_);

    Difference(DsspAsn,PdbAsn,c->NRes,'H',&QualDssp_H);
    Difference(DsspAsn,PdbAsn,c->NRes,'E',&QualDssp_E);
    Difference(Asn,PdbAsn,c->NRes,'H',&Qual_H);
    Difference(Asn,PdbAsn,c->NRes,'E',&Qual_E);

    PerCor     = PercentCorrect(Asn,PdbAsn,c->NRes);
    PerCorDssp = PercentCorrect(DsspAsn,PdbAsn,c->NRes);

    Total_H = CompareElements(Asn,PdbAsn,DsspAsn,c->NRes,'H',&Better_H,&Worse_H);
    Total_E = CompareElements(Asn,PdbAsn,DsspAsn,c->NRes,'E',&Better_E,&Worse_E);

    if( (Flag1=AssessCorr(&Qual_H)) )
      fprintf(Out,"%6.4f ",Qual_H.Corr);
    else fprintf(Out,"  None ");
    fprintf(Out,"%6.4f ",Qual_H.Perc);

    if( (Flag2=AssessCorr(&QualDssp_H)) )
      fprintf(Out,"%6.4f ",QualDssp_H.Corr);
    else fprintf(Out,"  None ");
    fprintf(Out,"%6.4f ",QualDssp_H.Perc);

    if( Flag1 && Flag2 )
      fprintf(Out,"%7.4f ",Qual_H.Corr-QualDssp_H.Corr);
    else fprintf(Out,"   None ");
    fprintf(Out,"%7.4f | ",Qual_H.Perc-QualDssp_H.Perc);


    if( (Flag1=AssessCorr(&Qual_E)) )
      fprintf(Out,"%6.4f ",Qual_E.Corr);
    else fprintf(Out,"  None ");
    fprintf(Out,"%6.4f ",Qual_E.Perc);

    if( (Flag2=AssessCorr(&QualDssp_E)) )
      fprintf(Out,"%6.4f ",QualDssp_E.Corr);
    else fprintf(Out,"  None ");
    fprintf(Out,"%6.4f ",QualDssp_E.Perc);

    if( Flag1 && Flag2 )
      fprintf(Out,"%7.4f ",Qual_E.Corr-QualDssp_E.Corr);
    else fprintf(Out,"   None ");
    fprintf(Out,"%7.4f | ",Qual_E.Perc-QualDssp_E.Perc);

    fprintf(Out,"%6.4f %6.4f | %6.4f %6.4f | ",Q2_,Q2_Dssp,O_,O_Dssp);

    fprintf(Out,"%4d %4d %4d %4d | %4d %4d %4d %4d |%4d %4d %4d %4d |%4d %4d %4d %4d | %s%c %4d %4.2f | %5.3f %5.3f %5.3f",
	    Qual_H.TP,Qual_H.TN,Qual_H.FP,Qual_H.FN,
	    QualDssp_H.TP,QualDssp_H.TN,QualDssp_H.FP,QualDssp_H.FN,
	    Qual_E.TP,Qual_E.TN,Qual_E.FP,Qual_E.FN,
	    QualDssp_E.TP,QualDssp_E.TN,QualDssp_E.FP,QualDssp_E.FN,
	    c->File,c->Id,c->NRes,c->Resolution,Content,AlphaCont,BetaCont);

    if( El ) {
      fprintf(Out," FullH: YYN %2d NYY %2d YNN %2d NNY %2d ",YYN_H,NYY_H,YNN_H,NNY_H);
      fprintf(Out," FullE: YYN %2d NYY %2d YNN %2d NNY %2d ",YYN_E,NYY_E,YNN_E,NNY_E);
    }

    memset(Tmp[0],'C',c->NRes);
    memset(Tmp[1],'C',c->NRes);
    memset(Tmp[2],'C',c->NRes);

    for( i=0; i<c->NRes; i++ ) {
      if( Asn[i] == 'H' )
	Tmp[0][i] = Asn[i];
      if( PdbAsn[i] == 'H' )
	Tmp[1][i] = PdbAsn[i];
      if( DsspAsn[i] == 'H' )
	Tmp[2][i] = DsspAsn[i];
    }
    ExcludeObvious(Tmp[0],Tmp[2],Tmp[1],c->NRes);
    Difference(Tmp[0],Tmp[1],c->NRes,'H',&Qual_H);
    fprintf(Out," | ResH: YYN %3d NNY %3d YNN %3d NYY %3d",
	    Qual_H.TP,Qual_H.TN,Qual_H.FP,Qual_H.FN);

    memset(Tmp[0],'C',c->NRes);
    memset(Tmp[1],'C',c->NRes);
    memset(Tmp[2],'C',c->NRes);

    for( i=0; i<c->NRes; i++ ) {
      if( Asn[i] == 'E' )
	Tmp[0][i] = Asn[i];
      if( PdbAsn[i] == 'E' )
	Tmp[1][i] = PdbAsn[i];
      if( DsspAsn[i] == 'E' )
	Tmp[2][i] = DsspAsn[i];
    }
    ExcludeObvious(Tmp[0],Tmp[2],Tmp[1],c->NRes);
    Difference(Tmp[0],Tmp[1],c->NRes,'E',&Qual_E);
    fprintf(Out," | ResE: YYN %3d NNY %3d YNN %3d NYY %3d",
	    Qual_E.TP,Qual_E.TN,Qual_E.FP,Qual_E.FN);

    fprintf(Out," | ToH: %2d BtH: %2d WsH: %2d ",Total_H,Better_H,Worse_H);
    fprintf(Out," | ToE: %2d BtE: %2d WsE: %2d ",Total_E,Better_E,Worse_E);
    fprintf(Out,"PerCor %7.4f PerCorDssp %7.4f\n",PerCor,PerCorDssp);

    fprintf(Out,"\n");

    free(Asn);
    free(PdbAsn);
    free(DsspAsn);
  }
}


int svt_stride::Presnell(char *Asn1, int L1, char *Asn2, int L2, char SecStr, float Threshold,
	     float *Q2, float *O)
{
//  int Boundaries(char *Asn, int L, char SecondStr, int (*Bound)[2]);
  int Bound1[MAX_ASSIGN][2], Bound2[MAX_ASSIGN][2], Length1[MAX_ASSIGN],Length2[MAX_ASSIGN], NSeg1, NSeg2;
  int Overlap, MaxOverlap, TP=0, FP=0, FN=0;
  register int i, j;

  NSeg1 = Boundaries(Asn1,L1,SecStr,Bound1);
  NSeg2 = Boundaries(Asn2,L2,SecStr,Bound2);


  for(i=0; i<NSeg1; i++)
    Length1[i] = Bound1[i][1]-Bound1[i][0]+1;

  for(j=0; j<NSeg2; j++)
    Length2[j] = Bound2[j][1]-Bound2[j][0]+1;

  for(i=0; i<NSeg1; i++) {
    MaxOverlap = 0;
    for(j=0; j<NSeg2; j++) {
      Overlap = Minimum(Bound1[i][1],Bound2[j][1])-Maximum(Bound1[i][0],Bound2[j][0])+1;
      if( Overlap > MaxOverlap ) MaxOverlap = Overlap;
    }
    if( (float)MaxOverlap/(float)Length1[i] >= Threshold )
      TP++;
    else
      FN++;
  }

  for(i=0; i<NSeg2; i++) {
    MaxOverlap = 0;
    for(j=0; j<NSeg1; j++) {
      Overlap = Minimum(Bound2[i][1],Bound1[j][1])-Maximum(Bound2[i][0],Bound1[j][0])+1;
      if( Overlap > MaxOverlap ) MaxOverlap = Overlap;
    }
    if( (float)MaxOverlap/(float)Length2[i] < Threshold )
      FP++;
  }

  if( TP+FN != 0 ) {
    *Q2 = (float)TP/((float)TP+(float)FN);
    *O = (float)FP/((float)TP+(float)FN);
  }
  else {
    *Q2 = -1.0;
    *O = -1.0;
  }

  return(1);
}


int svt_stride::Sov(char *Asn1, int L1, char *Asn2, int L2, char SecStr, float Threshold, float *Q2)
{
  int Bound1[MAX_ASSIGN][2], Bound2[MAX_ASSIGN][2], Length1[MAX_ASSIGN],
      Length2[MAX_ASSIGN], NSeg1, NSeg2;
  int Overlap, MaxOverlap, TP=0, FN=0;
  register int i, j;

  NSeg1 = Boundaries(Asn1,L1,SecStr,Bound1);
  NSeg2 = Boundaries(Asn2,L2,SecStr,Bound2);


  for(i=0; i<NSeg1; i++)
    Length1[i] = Bound1[i][1]-Bound1[i][0]+1;

  for(j=0; j<NSeg2; j++)
    Length2[j] = Bound2[j][1]-Bound2[j][0]+1;

  for(i=0; i<NSeg1; i++) {
    MaxOverlap = 0;
    for(j=0; j<NSeg2; j++) {

      if( Bound1[i][0] > Bound2[j][1] || Bound1[i][1] < Bound2[j][0] ) /*No overlap */
	Overlap = 0;
      else if ( Bound2[j][0] >= Bound1[i][0] && Bound2[j][1] <= Bound1[i][1] ) /* j inside i */
	Overlap = Length2[j];
      else if ( Bound1[i][0] <= Bound2[j][0] ) /* j shifted to the right with respect to i */
	Overlap = Minimum(Bound2[j][1]-Bound2[j][0],Bound1[i][1]-Bound2[j][0])+1;
      else if( Bound1[i][0] >= Bound2[j][0] ) /* i shifted to the right with respect to j */
	Overlap = Minimum(Bound2[j][1]-Bound2[j][0],Bound2[j][1]-Bound1[i][0])+1;
      else
	Overlap = Length1[i]; /* i inside j */
      if( Overlap > MaxOverlap ) MaxOverlap = Overlap;
    }
    if( (float)MaxOverlap/(float)Length1[i] >= Threshold )
      TP++;
    else
      FN++;
  }
  if( TP+FN != 0 )
    *Q2 = (float)TP/((float)TP+(float)FN);
  else
    *Q2 = -1.0;
/*  O  = (float)TP/((float)TP+(float)FN);*/
  return(1);
}

void *svt_stride::ckalloc(size_t bytes)
{
  register void *ret;
  //void die(char *format, ... );

  if( !(ret = malloc(bytes)) ) die("Out of  memory\n");

  return ret;
}

float **svt_stride::FloatMatrix(int M, int N)
{
  int m;
  float **Matrix;

  Matrix = (float **)ckalloc(M*sizeof(float *));

  for( m=0; m<M; m++ ) Matrix[m] = (float *)ckalloc(N*sizeof(float));

  return(Matrix);
}

float ***svt_stride::FloatCube(int M, int N, int K)
{
  int m, n, k;
  float ***Cube;

  Cube = (float ***)ckalloc(M*sizeof(float **));

  for( m=0; m<M; m++ ) {
    Cube[m] = (float **)ckalloc(N*sizeof(float *));
    for( n=0; n<N; n++ )
      Cube[m][n] = (float *)ckalloc(K*sizeof(float));
  }

  for( m=0; m<M; m++ )
    for( n=0; n<N; n++ )
      for( k=0; k<K; k++ )
	Cube[m][n][k] = 0.0;

  return(Cube);
}

float ****svt_stride::Float4Dim(int M, int N, int K, int L)
{
  int m, n, k, l;
  float ****FourDim;

  FourDim = (float ****)ckalloc(M*sizeof(float ***));


  for( m=0; m<M; m++ ) {
    FourDim[m] = (float ***)ckalloc(N*sizeof(float **));
    for( n=0; n<N; n++ ) {
      FourDim[m][n] = (float **)ckalloc(K*sizeof(float*));
      for( k=0; k<K; k++ )
	FourDim[m][n][k] = (float *)ckalloc(L*sizeof(float));
    }
  }

  for( m=0; m<M; m++ )
    for( n=0; n<N; n++ )
      for( k=0; k<K; k++ )
	for( l=0; l<L; l++ )
	  FourDim[m][n][k][l] = 0.0;

  return(FourDim);
}

void svt_stride::FreeFloatMatrix(float **Matrix, int M)
{
  int m;

  for( m=0; m<M; m++ ) free(Matrix[m]);

  free(Matrix);

}

int ***svt_stride::IntCube(int M, int N, int K)
{
  int m, n, k;
  int ***Cube;

  Cube = (int ***)ckalloc(M*sizeof(int **));

  for( m=0; m<M; m++ ) {
    Cube[m] = (int **)ckalloc(N*sizeof(int *));
    for( n=0; n<N; n++ ) Cube[m][n] = (int *)ckalloc(K*sizeof(int));
  }

  for( m=0; m<M; m++ )
    for( n=0; n<N; n++ )
      for( k=0; k<K; k++ )
	Cube[m][n][k] = 0;

  return(Cube);
}


int **svt_stride::IntMatrix(int M, int N)
{
  int m;
  int **Matrix;

  Matrix = (int **)ckalloc(M*sizeof(int *));

  for( m=0; m<M; m++ ) Matrix[m] = (int *)ckalloc(N*sizeof(int));

  return(Matrix);
}

int ****svt_stride::Int4Dim(int M, int N, int K, int L)
{
  int m, n, k, l;
  int ****FourDim;

  FourDim = (int ****)ckalloc(M*sizeof(int ***));


  for( m=0; m<M; m++ ) {
    FourDim[m] = (int ***)ckalloc(N*sizeof(int **));
    for( n=0; n<N; n++ ) {
      FourDim[m][n] = (int **)ckalloc(K*sizeof(int*));
      for( k=0; k<K; k++ )
	FourDim[m][n][k] = (int *)ckalloc(L*sizeof(int));
    }
  }

  for( m=0; m<M; m++ )
    for( n=0; n<N; n++ )
      for( k=0; k<K; k++ )
	for( l=0; l<L; l++ )
	  FourDim[m][n][k][l] = 0;

  return(FourDim);
}

void svt_stride::FreeIntMatrix(int **Matrix, int M)
{
  int m;

  for( m=0; m<M; m++ ) free(Matrix[m]);

  free(Matrix);

}

char **svt_stride::CharMatrix(int M, int N)
{
  int m;
  char **Matrix;

  Matrix = (char **)ckalloc(M*sizeof(char *));

  for( m=0; m<M; m++ ) Matrix[m] = (char *)ckalloc(N*sizeof(char));

  return(Matrix);
}

void svt_stride::FreeCharMatrix(char **Matrix, int M)
{
  int m;

  for( m=0; m<M; m++ ) free(Matrix[m]);

  free(Matrix);

}

void svt_stride::FreeIntCube(int ***Cube, int M, int N)
{
  int m, n;

  for( m=0; m<M; m++ ) {
    for( n=0; n<N; n++ )
      free(Cube[m][n]);
    free(Cube[m]);
  }

  free(Cube);
}

void svt_stride::FreeFloatCube(float ***Cube, int M, int N)
{
  int m, n;

  for( m=0; m<M; m++ ) {
    for( n=0; n<N; n++ )
      free(Cube[m][n]);
    free(Cube[m]);
  }

  free(Cube);
}

void svt_stride::MergePatternsAntiPar(MPATTERN **Pat, int NPat)
{
  register int i, j;
  int DB, DW, MinDB1, MinDB2, MinDW1, MinDW2, Min, Lnk1A, Lnk1D;
  int I1A, I1D, I2A, I2D, J1A, J1D, J2A, J2D;
  char I1ACn, I1DCn, I2ACn, I2DCn, J1ACn, J1DCn, J2ACn, J2DCn;

  for( i=0; i<NPat; i++ ) {

    if( !Pat[i]->ExistPattern ) continue;

    MinDB1 = MinDB2 = MinDW1 = MinDW2 = 1000;
    Min = ERR;
    Lnk1D = Lnk1A = ERR;

    Alias(&I1D,&I1A,&I2D,&I2A,&I1DCn,&I1ACn,&I2DCn,&I2ACn,Pat[i]);

    for( j=0; j<NPat; j++ ) {

      if( i == j || !Pat[j]->ExistPattern ) continue;

      Alias(&J1D,&J1A,&J2D,&J2A,&J1DCn,&J1ACn,&J2DCn,&J2ACn,Pat[j]);

      if( Near(I1D,J1D,J1A,I1A,J2A,J2D,I2A,I2D,I1DCn,J1DCn,J1ACn,I1ACn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSide(J1A,J1D,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J2D,&Lnk1D,J2A,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( Near(I1D,J1A,J1D,I1A,J2D,J2A,I2A,I2D,I1DCn,J1ACn,J1DCn,I1ACn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSide(J1D,J1A,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J2A,&Lnk1D,J2D,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( Near(I1D,J2D,J2A,I1A,J1A,J1D,I2A,I2D,I1DCn,J2DCn,J2ACn,I1ACn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSide(J2A,J2D,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J1D,&Lnk1D,J1A,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( Near(I1D,J2A,J2D,I1A,J1D,J1A,I2A,I2D,I1DCn,J2ACn,J2DCn,I1ACn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSide(J2D,J2A,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J1A,&Lnk1D,J1D,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( Near(I1A,J1D,J1A,I1D,J2A,J2D,I2D,I2A,I1ACn,J1DCn,J1ACn,I1DCn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSide(J1A,J1D,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J2A,&Lnk1D,J2D,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( Near(I1A,J1A,J1D,I1D,J2D,J2A,I2D,I2A,I1ACn,J1ACn,J1DCn,I1DCn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSide(J1A,J1D,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J2D,&Lnk1D,J2A,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( Near(I1A,J2D,J2A,I1D,J1A,J1D,I2D,I2A,I1ACn,J2DCn,J2ACn,I1DCn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSide(J2D,J2A,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J1A,&Lnk1D,J1D,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( Near(I1A,J2A,J2D,I1D,J1D,J1A,I2D,I2A,I1ACn,J2ACn,J2DCn,I1DCn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSide(J2A,J2D,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J1D,&Lnk1D,J2D,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

    }

    for( j=0; j<NPat; j++ ) {

      if( j == Min || j == i || !Pat[j]->ExistPattern ) continue;

      Alias(&J1D,&J1A,&J2D,&J2A,&J1DCn,&J1ACn,&J2DCn,&J2ACn,Pat[j]);

      if( Near(I2D,J1D,J1A,I2A,J2A,J2D,I1A,I1D,I2DCn,J1DCn,J1ACn,I2ACn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J2A,J2D,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( Near(I2D,J1A,J1D,I2A,J2D,J2A,I1A,I1D,I2DCn,J1ACn,J1DCn,I2ACn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J2D,J2A,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( Near(I2D,J2D,J2A,I2A,J1A,J1D,I1A,I1D,I2DCn,J2DCn,J2ACn,I2ACn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J1A,J1D,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( Near(I2D,J2A,J2D,I2A,J1D,J1A,I1A,I1D,I2DCn,J2ACn,J2DCn,I2ACn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J1D,J1A,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( Near(I2A,J1D,J1A,I2D,J2A,J2D,I1D,I1A,I2ACn,J1DCn,J1ACn,I2DCn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J2D,J2A,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( Near(I2A,J1A,J1D,I2D,J2D,J2A,I1D,I1A,I2ACn,J1ACn,J1DCn,I2DCn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J2A,J2D,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( Near(I2A,J2D,J2A,I2D,J1A,J1D,I1D,I1A,I2ACn,J2DCn,J2ACn,I2DCn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J1D,J1A,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( Near(I2A,J2A,J2D,I2D,J1D,J1A,I1D,I1A,I2ACn,J2ACn,J2DCn,I2DCn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J1A,J1D,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);
      }
  }
}

void svt_stride::MergePatternsPar(MPATTERN **Pat, int NPat)
{
  register int i, j;
  int DB, DW, MinDB1, MinDB2, MinDW1, MinDW2, Min, Lnk1A, Lnk1D;
  int I1A, I1D, I2A, I2D, J1A, J1D, J2A, J2D;
  char I1ACn, I1DCn, I2ACn, I2DCn, J1ACn, J1DCn, J2ACn, J2DCn;

  for( i=0; i<NPat; i++ ) {

    if( !Pat[i]->ExistPattern ) continue;

    MinDB1 = MinDB2 = MinDW1 = MinDW2 = 1000;
    Min = ERR;
    Lnk1D = Lnk1A = ERR;

    Alias(&I1D,&I1A,&I2D,&I2A,&I1DCn,&I1ACn,&I2DCn,&I2ACn,Pat[i]);

    for( j=0; j<NPat; j++ ) {

      if( i == j || !Pat[j]->ExistPattern ) continue;

      Alias(&J1D,&J1A,&J2D,&J2A,&J1DCn,&J1ACn,&J2DCn,&J2ACn,Pat[j]);

      if( NearPar(I1D,J1D,J1A,I1A,J2A,J2D,I2A,I2D,I1DCn,J1DCn,J1ACn,I1ACn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSidePar(J1A,J1D,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J2D,&Lnk1D,J2A,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( NearPar(I1D,J1A,J1D,I1A,J2D,J2A,I2A,I2D,I1DCn,J1ACn,J1DCn,I1ACn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSidePar(J1D,J1A,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J2A,&Lnk1D,J2D,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( NearPar(I1D,J2D,J2A,I1A,J1A,J1D,I2A,I2D,I1DCn,J2DCn,J2ACn,I1ACn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSidePar(J2A,J2D,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J1D,&Lnk1D,J1A,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( NearPar(I1D,J2A,J2D,I1A,J1D,J1A,I2A,I2D,I1DCn,J2ACn,J2DCn,I1ACn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSidePar(J2D,J2A,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J1A,&Lnk1D,J1D,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( NearPar(I1A,J1D,J1A,I1D,J2A,J2D,I2D,I2A,I1ACn,J1DCn,J1ACn,I1DCn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSidePar(J1A,J1D,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J2A,&Lnk1D,J2D,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( NearPar(I1A,J1A,J1D,I1D,J2D,J2A,I2D,I2A,I1ACn,J1ACn,J1DCn,I1DCn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSidePar(J1A,J1D,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J2D,&Lnk1D,J2A,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( NearPar(I1A,J2D,J2A,I1D,J1A,J1D,I2D,I2A,I1ACn,J2DCn,J2ACn,I1DCn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSidePar(J2D,J2A,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J1A,&Lnk1D,J1D,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

      if( NearPar(I1A,J2A,J2D,I1D,J1D,J1A,I2D,I2A,I1ACn,J2ACn,J2DCn,I1DCn,&DB,&DW) &&
	  ((DB < MinDB1 && DW <= MinDW1) || (DB <= MinDB1 && DW < MinDW1) ) &&
	 RightSidePar(J2A,J2D,I1A,I1D,I2A,I2D) )
	JoinNeighbours(&Lnk1A,J1D,&Lnk1D,J2D,&Pat[i]->Nei1,Pat[j],&MinDB1,DB,&MinDW1,DW,&Min,j);

    }

    for( j=0; j<NPat; j++ ) {

      if( j == Min || j == i || !Pat[j]->ExistPattern ) continue;

      Alias(&J1D,&J1A,&J2D,&J2A,&J1DCn,&J1ACn,&J2DCn,&J2ACn,Pat[j]);

      if( NearPar(I2D,J1D,J1A,I2A,J2A,J2D,I1A,I1D,I2DCn,J1DCn,J1ACn,I2ACn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J2A,J2D,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( NearPar(I2D,J1A,J1D,I2A,J2D,J2A,I1A,I1D,I2DCn,J1ACn,J1DCn,I2ACn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J2D,J2A,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( NearPar(I2D,J2D,J2A,I2A,J1A,J1D,I1A,I1D,I2DCn,J2DCn,J2ACn,I2ACn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J1A,J1D,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( NearPar(I2D,J2A,J2D,I2A,J1D,J1A,I1A,I1D,I2DCn,J2ACn,J2DCn,I2ACn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J1D,J1A,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( NearPar(I2A,J1D,J1A,I2D,J2A,J2D,I1D,I1A,I2ACn,J1DCn,J1ACn,I2DCn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J2D,J2A,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( NearPar(I2A,J1A,J1D,I2D,J2D,J2A,I1D,I1A,I2ACn,J1ACn,J1DCn,I2DCn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J2A,J2D,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( NearPar(I2A,J2D,J2A,I2D,J1A,J1D,I1D,I1A,I2ACn,J2DCn,J2ACn,I2DCn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J1D,J1A,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);

      if( NearPar(I2A,J2A,J2D,I2D,J1D,J1A,I1D,I1A,I2ACn,J2ACn,J2DCn,I2DCn,&DB,&DW) &&
	  (( DB < MinDB2 && DW <= MinDW2) || (DB <= MinDB2 && DW < MinDW2) ) &&
	 RightSide2(Lnk1A,Lnk1D,J1A,J1D,I1A,I1D,I2A,I2D) )
	JoinNeighb(&Pat[i]->Nei2,Pat[j],&MinDB2,DB,&MinDW2,DW);
      }
  }
}

int svt_stride::RightSide2(int L_A1, int L_D1, int LnkD, int LnkA, int I1A, int I1D, int I2A, int I2D)
{

  if( ( I2A < I1D && LnkA <= I1D && LnkA <= I2A ) ||
      ( I2A > I1D && LnkA >= I1D && LnkA >= I2A ) ||
      ( I2D < I1A && LnkD <= I1A && LnkA <= I2D ) ||
      ( I2D > I1A && LnkD >= I1A && LnkD >= I2D )  )
    return(SUCCESS);
  else
    if( I2A == I1D && I2D == I1A ) {
      if( L_A1 != ERR &&
	 ( ( LnkD <= I2D && L_A1 <= I2D && LnkA >= I2A && L_D1 >= I2A ) ||
	   ( LnkD >= I2D && L_A1 >= I2D && LnkA <= I2A && L_D1 <= I2A ) ) )
	return(FAILURE);
      else
	return(SUCCESS);
    }

  return(FAILURE);
}

int svt_stride::RightSide(int LnkA, int LnkD, int I1A, int I1D, int I2A, int I2D )
{

  if( ( I1A == I2D && I1D == I2A ) ||
      ( I1A < I2D && LnkA <= I2D && LnkA <= I1A ) ||
      ( I1A > I2D && LnkA >= I2D && LnkA >= I1A ) ||
      ( I1D < I2A && LnkD <= I2A && LnkD <= I1D ) ||
      ( I1D > I2A && LnkD >= I2A && LnkD >= I1D ) )
    return(SUCCESS);

  return(FAILURE);
}

int svt_stride::RightSidePar(int LnkA, int LnkD, int I1A, int I1D, int I2A, int I2D )
{

  if( ( I1A == I2D && I1D == I2A ) ||
      ( I1A < I2D && LnkA < I2D && LnkA <= I1A && I1D <= I2A && LnkD <= I2A && LnkD <= I1D ) ||
      ( I1A > I2D && LnkA > I2D && LnkA >= I1A && I1D >= I2A && LnkD >= I2A && LnkD >= I1D ) ||
      ( I1D < I2A && LnkD < I2A && LnkD <= I1D && I1A <= I2D && LnkA <= I2D && LnkA <= I1A ) ||
      ( I1D > I2A && LnkD > I2A && LnkD >= I1D && I1A >= I2D && LnkA >= I2D && LnkA >= I1A) )
    return(SUCCESS);

  return(FAILURE);
}

void svt_stride::JoinNeighbours(int *Lnk1A, int Res1, int *Lnk1D, int Res2, MPATTERN **Nei,
		    MPATTERN *Pat, int *MinDB1, int DB, int *MinDW1, int DW, int *Min, int j)
{
  *Lnk1A = Res1;
  *Lnk1D = Res2;
  (*Nei) = Pat;
  *MinDB1 = DB;
  *MinDW1 = DW;
  *Min = j;
}

void svt_stride::JoinNeighb(MPATTERN **Nei, MPATTERN *Pat, int *MinDB2, int DB, int *MinDW2, int DW)
{
  (*Nei) = Pat;
  *MinDB2 = DB;
  *MinDW2 = DW;
}

int svt_stride::NearPar(int Res1, int Res2, int Res3, int Res4, int Res5, int Res6, int Res7, int Res8,
	 char Cn1, char Cn2, char Cn3, char Cn4, int *DistBest, int *DistWorst)
{

/*
   Res5 Res2 Res1
   Res6 Res3 Res4
*/

  int a, b, c1, d1, c, d, Nei1, Nei2;

  if( Cn1 != Cn2 || Cn3 != Cn4 ) return(FAILURE);

  if( Res1 >= Res2 && Res2 >= Res5 && Res7 >= Res1 &&
      Res4 >= Res3 && Res4 >= Res6 && Res8 >= Res4 ) {

    if( Res5 == Res2 )
      Nei1 = Res2;
    else
      Nei1 = Res2-1;

    if( Res1 == Res7 )
      Nei2 = Res1;
    else
      Nei2 = Res1+1;

    a = Nei2-Nei1;
    c1 = Nei2-Res5;

    if( Res3 == Res6 )
      Nei1 = Res3;
    else
      Nei1 = Res3-1;

    if( Res4 == Res8 )
      Nei2 = Res4;
    else
      Nei2 = Res4+1;

    b = Nei2-Nei1;
    d1 = Nei2-Res6;

  }
  else
  if( Res1 <= Res2 && Res2 <= Res5 && Res7 <= Res1 &&
      Res4 <= Res3 && Res4 <= Res6 && Res8 <= Res4 ) {

    if( Res5 == Res2 )
      Nei1 = Res2;
    else
      Nei1 = Res2+1;

    if( Res1 == Res7 )
      Nei2 = Res1;
    else
      Nei2 = Res1-1;

    a = Nei1-Nei2;
    c1 = Res1-Res7;

    if( Res3 == Res6 )
      Nei1 = Res3;
    else
      Nei1 = Res3+1;

    if( Res4 == Res8 )
      Nei2 = Res4;
    else
      Nei2 = Res4-1;

    b = Nei1-Nei2;
    d1 = Nei1-Res8;


  }
  else
   return(FAILURE);

  c = Maximum(c1,a);
  d = Maximum(d1,b);

  if( a >= 0 && b >= 0 && c >= 0 && d >= 0 &&
      ( (a <= 2 && b <= 5) || (a <= 5 && b <= 2) ) ) {
    *DistBest  = Minimum(a,b);
    *DistWorst = Maximum(c,d);
    if( *DistBest <= *DistWorst )
      return(SUCCESS);
    else
      return(FAILURE);
  }

  return(FAILURE);
}

int svt_stride::Near(int Res1, int Res2, int Res3, int Res4, int Res5, int Res6, int Res7, int Res8,
	 char Cn1, char Cn2, char Cn3, char Cn4, int *DistBest, int *DistWorst)
{

/*
   Res5 Res2 Res1
   Res6 Res3 Res4
*/

  int a, b, c1, d1, c, d, Nei1, Nei2;


  if( Cn1 != Cn2 || Cn3 != Cn4 ) return(FAILURE);


  if( Res1 >= Res2 && Res2 >= Res5 && Res7 >= Res1 &&
      Res4 <= Res3 && Res4 <= Res6 && Res8 <= Res4 ) {

    if( Res5 == Res2 )
      Nei1 = Res2;
    else
      Nei1 = Res2-1;

    if( Res1 == Res7 )
      Nei2 = Res1;
    else
      Nei2 = Res1+1;

    a = Nei2-Nei1;
    c1 = Nei2-Res5;

    if( Res3 == Res6 )
      Nei1 = Res3;
    else
      Nei1 = Res3+1;

    if( Res4 == Res8 )
      Nei2 = Res4;
    else
      Nei2 = Res4-1;

    b = Nei1-Nei2;
    d1 = Res6-Nei2;
  }
  else
    return(FAILURE);

  c = Maximum(c1,a);
  d = Maximum(d1,b);

  if( a >= 0 && b >= 0 && c >= 0 && d >= 0 &&
      ( (a <= 2 && b <= 5) || (a <= 5 && b <= 2) ) ) {
    *DistBest  = Minimum(a,b);
    *DistWorst = Maximum(c,d);
    if( *DistBest <= *DistWorst )
      return(SUCCESS);
    else
      return(FAILURE);
  }

  return(FAILURE);
}

int svt_stride::MolScript(CHAIN **Chain, int NChain, COMMAND *Cmd)
{

  int HelixBound[MAX_ASSIGN][2], SheetBound[MAX_ASSIGN][2], CoilBound[MAX_ASSIGN][2];
  int NHelix, NSheet, NCoil, i, Cn;
  char *Asn;
  FILE *fi;

  if( !(fi = fopen(Cmd->MolScriptFile,"w")) )
    return(escape(FAILURE,"\nCan not open molscript file %s\n\n",Cmd->MolScriptFile));

  fprintf(fi,"plot\n");
  fprintf(fi,"read mol \"%s\";\n",Chain[0]->File);
  fprintf(fi,"transform atom * by centre position in amino-acids\n");
  fprintf(fi,"by rotation z  0.0	\n");
  fprintf(fi,"by rotation y -260.0	\n");
  fprintf(fi,"by rotation x -40.0;\n");

  for( Cn=0; Cn<NChain; Cn++ ) {

    if( !Chain[Cn]->Valid )
       continue;

    Asn = (char *)ckalloc(Chain[Cn]->NRes*sizeof(char));

    ExtractAsn(Chain,Cn,Asn);
    for( i=0; i<Chain[Cn]->NRes; i++ )
      if( Asn[i] != 'H' && Asn[i] != 'E' )
	Asn[i] = 'C';

    NHelix = Boundaries(Asn,Chain[Cn]->NRes,'H',HelixBound);
    NSheet = Boundaries(Asn,Chain[Cn]->NRes,'E',SheetBound);
    NCoil  = Boundaries(Asn,Chain[Cn]->NRes,'C',CoilBound);

    free(Asn);

    for( i=0; i<NSheet; i++ )
      if( SheetBound[i][1] != Chain[Cn]->NRes-1 )
	SheetBound[i][1]++;
    for( i=0; i<NHelix;  i++ )
      if( HelixBound[i][1] != Chain[Cn]->NRes-1 )
	HelixBound[i][1]++;
    for( i=0; i<NCoil;   i++ )
      if( CoilBound[i][1]  != Chain[Cn]->NRes-1 )
	CoilBound[i][1]++;

    for( i=0; i<NHelix; i++ )
      fprintf(fi,"helix from %c%s to %c%s;\n",
	      Chain[Cn]->Id,Chain[Cn]->Rsd[HelixBound[i][0]]->PDB_ResNumb,
	      Chain[Cn]->Id,Chain[Cn]->Rsd[HelixBound[i][1]]->PDB_ResNumb);

    for( i=0; i<NSheet; i++ )
      fprintf(fi,"strand from %c%s to %c%s;\n",
	      Chain[Cn]->Id,Chain[Cn]->Rsd[SheetBound[i][0]]->PDB_ResNumb,
	      Chain[Cn]->Id,Chain[Cn]->Rsd[SheetBound[i][1]]->PDB_ResNumb);

    for( i=0; i<NCoil; i++ )
      fprintf(fi,"coil from %c%s to %c%s;\n",
	      Chain[Cn]->Id,Chain[Cn]->Rsd[CoilBound[i][0]]->PDB_ResNumb,
	      Chain[Cn]->Id,Chain[Cn]->Rsd[CoilBound[i][1]]->PDB_ResNumb);
  }

  fprintf(fi,"end_plot\n");
  fclose(fi);

  return(SUCCESS);
}

const char *svt_stride::OneToThree(char One)
{

  if( One == 'A' ) return("ALA");
  else if( One == 'R' ) return("ARG");
  else if( One == 'N' ) return("ASN");
  else if( One == 'D' ) return("ASP");
  else if( One == 'B' ) return("ASX");
  else if( One == 'C' ) return("CYS");
  else if( One == 'Q' ) return("GLN");
  else if( One == 'E' ) return("GLU");
  else if( One == 'Z' ) return("GLX");
  else if( One == 'G' ) return("GLY");
  else if( One == 'H' ) return("HIS");
  else if( One == 'I' ) return("ILE");
  else if( One == 'L' ) return("LEU");
  else if( One == 'K' ) return("LYS");
  else if( One == 'M' ) return("MET");
  else if( One == 'P' ) return("PRO");
  else if( One == 'F' ) return("PHE");
  else if( One == 'S' ) return("SER");
  else if( One == 'T' ) return("THR");
  else if( One == 'W' ) return("TRP");
  else if( One == 'Y' ) return("TYR");
  else if( One == 'V' ) return("VAL");
  else if( One == 'X' ) return("UNK");
  else return("***");
}



int svt_stride::OutSeq(CHAIN **Chain, int NChain, COMMAND *Cmd)
{
  int Cn, Res;
  FILE *Seq;

  if( (int)strlen(Cmd->SeqFile) == 0 )
    Seq = stdout;
  else
    if( (Seq = fopen(Cmd->SeqFile,"a")) == NULL )
      die("Error writing sequence file %s\n",Cmd->SeqFile);

  for( Cn=0; Cn<NChain; Cn++ ) {

    if( !Chain[Cn]->Valid )
       continue;

    fprintf(Seq,">%s %c  %d %7.3f\n",
	    Chain[Cn]->File,SpaceToDash(Chain[Cn]->Id),Chain[Cn]->NRes,
	    Chain[Cn]->Resolution);

    for( Res=0; Res<Chain[Cn]->NRes; Res++ ) {
      if( Res%60 == 0 && Res != 0 ) fprintf(Seq,"\n");
      fprintf(Seq,"%c",ThreeToOne(Chain[Cn]->Rsd[Res]->ResType));
    }
    fprintf(Seq,"\n");
  }
  fclose(Seq);
  exit(0);

  return(SUCCESS);
}

int svt_stride::Process_ATOM(CHAIN **Chain, int *ChainNumber,
		 BOOLEAN *First_ATOM, COMMAND *Cmd, char *rvs_one_line)
{

  char *Field[MAX_FIELD];
  BUFFER Tmp;
  int CC, NR, NA;
  static char LastRes[MAX_CHAIN][RES_FIELD];
  RESIDUE *r;

  char *BufferTmp = rvs_one_line;
  //cout << "here in process_atom";


  if( Cmd->NActive && !ChInStr(Cmd->Active,SpaceToDash(BufferTmp[21])) )
     return(SUCCESS);

  if( BufferTmp[16] != 'A' && BufferTmp[16] != ' ' && BufferTmp[16] != '1' )
    return(SUCCESS);

  if( *First_ATOM ) {
    for( CC=0; CC<MAX_CHAIN; CC++ )
      strcpy(LastRes[CC],"XXXX");
    *First_ATOM = NO;
  }

  for( CC=0; CC < *ChainNumber &&  Chain[CC]->Id != BufferTmp[21] ; CC++ ) { };

  if( CC == *ChainNumber ) {
    InitChain(&Chain[CC]);
    Chain[CC]->Id = BufferTmp[21];
    (*ChainNumber)++;
  }
  else
  if( Chain[CC]->Ter == 1 )
    return(SUCCESS);

  if( BufferTmp[34] != '.' || BufferTmp[42] != '.' || BufferTmp[50] != '.' )
    return(escape(FAILURE,"File %s has no coordinates\n",Cmd->InputFile));


  if( Cmd->Stringent && BufferTmp[63] != '.')
    return(escape(FAILURE,"File %s has no temperature factor\n",Cmd->InputFile));


  SplitString(BufferTmp+22,Field,1);
  if( strcmp(Field[0],LastRes[CC]) ) {
    if( strcmp(LastRes[CC],"XXXX") && !FindAtom(Chain[CC],Chain[CC]->NRes,"CA",&NA) ) {
      free(Chain[CC]->Rsd[Chain[CC]->NRes]);
      Chain[CC]->NRes--;
    }
    if( strcmp(LastRes[CC],"XXXX") ) Chain[CC]->NRes++;
    NR = Chain[CC]->NRes;
    strcpy(LastRes[CC],Field[0]);
    Chain[CC]->Rsd[NR] = (RESIDUE *)ckalloc(sizeof(RESIDUE));
    Chain[CC]->Rsd[NR]->Prop = NULL;
    Chain[CC]->Rsd[NR]->Inv = NULL;
    strcpy(Chain[CC]->Rsd[NR]->PDB_ResNumb,LastRes[CC]);
    Chain[CC]->Rsd[NR]->NAtom = 0;
    SplitString(BufferTmp+17,Field,1);
    strcpy(Chain[CC]->Rsd[NR]->ResType,Field[0]);
  }
  else
    NR = Chain[CC]->NRes;

  NA = Chain[CC]->Rsd[NR]->NAtom;

  if( BufferTmp[16] != ' ' ) {
    strcpy(Tmp,BufferTmp);
    Tmp[16] = ' ';
    SplitString(Tmp+12,Field,1);
  }
  else
    SplitString(BufferTmp+12,Field,1);

  r = Chain[CC]->Rsd[NR];
  strcpy(r->AtomType[NA],Field[0]);


  strcpy(Tmp,BufferTmp);
  BufferTmp[38] = ' ';
  SplitString(Tmp+30,Field,1);
  r->Coord[NA][0] = atof(Field[0]);

  strcpy(Tmp,BufferTmp);
  BufferTmp[46] = ' ';
  SplitString(Tmp+38,Field,1);
  r->Coord[NA][1] = atof(Field[0]);

  strcpy(Tmp,BufferTmp);
  BufferTmp[54] = ' ';
  SplitString(Tmp+46,Field,1);
  r->Coord[NA][2] = atof(Field[0]);

  if( BufferTmp[57] == '.' ) {
    strcpy(Tmp,BufferTmp);
    Tmp[60] = ' ';
    SplitString(Tmp+54,Field,1);
    r->Occupancy[NA] = atof(Field[0]);
  }
  else
    r->Occupancy[NA] = -1.00;

  SplitString(BufferTmp+63,Field,1);
  r->TempFactor[NA] = atof(Field[0]);

  r->NAtom++;

  if( r->NAtom > MAX_AT_IN_RES-1 ){

	return(escape(FAILURE,"Input file  has too many atoms in residue %s %s %c\n",
		  r->ResType,r->PDB_ResNumb,Chain[CC]->Id));
  }
  return(SUCCESS);
}

int svt_stride::Process_COMPND(BUFFER Buffer, enum METHOD *Method)
{
  if( strstr(Buffer,"NMR") )
    *Method = NMR;

  if( strstr(Buffer,"MODEL") )
    if( *Method == XRay )
      *Method = Model;

  return(SUCCESS);
}

int svt_stride::Process_ENDMDL(CHAIN **Chain, int *ChainNumber)
{

  int CC;

  for( CC=0; CC < *ChainNumber; CC++ )
    Chain[CC]->Ter = 1;

  return(SUCCESS);
}

int svt_stride::Process_EXPDTA(BUFFER Buffer, enum METHOD *Method)
{
  char *Field[MAX_FIELD];
  int i, NFields;

  NFields = SplitString(Buffer,Field,10);

  for( i=0; i<NFields; i++ ) {
    if( !strcmp(Field[i],"MODEL") )
      *Method = Model;
    else
    if( !strcmp(Field[i],"NMR") || !strcmp(Field[i],"/NMR$")  )
      *Method = NMR;
  }
  return(SUCCESS);
}

int svt_stride::Process_HELIX(BUFFER Buffer, CHAIN **Chain, int *ChainNumber, COMMAND *Cmd)
{
  int CC, HC;
  char *Field[MAX_FIELD];
  BUFFER Tmp;

  if( Cmd->NActive && !ChInStr(Cmd->Active,SpaceToDash(Buffer[19])) )
     return(SUCCESS);

  for( CC=0; CC < *ChainNumber && Chain[CC]->Id != Buffer[19]; CC++ ) { };

  if( CC == *ChainNumber ) {
    InitChain(&Chain[CC]);
    Chain[CC]->Id = Buffer[19];
    (*ChainNumber)++;
  }

  HC = Chain[CC]->NHelix;
  Chain[CC]->Helix[HC] =  (HELIX *)ckalloc(sizeof(HELIX));

  SplitString(Buffer+15,Field,1);

  strcpy(Chain[CC]->Helix[HC]->Res1,Field[0]);

  SplitString(Buffer+27,Field,1);

  strcpy(Chain[CC]->Helix[HC]->Res2,Field[0]);

  strcpy(Tmp,Buffer);
  Tmp[25] = ' ';
  Tmp[37] = ' ';
  SplitString(Tmp+21,Field,1);
  strcpy(Chain[CC]->Helix[HC]->PDB_ResNumb1,Field[0]);
  SplitString(Tmp+33,Field,1);
  strcpy(Chain[CC]->Helix[HC]->PDB_ResNumb2,Field[0]);

  Chain[CC]->Helix[HC]->InsCode1 = Buffer[25];
  Chain[CC]->Helix[HC]->InsCode2 = Buffer[37];

  Tmp[40] = ' ';
  SplitString(Tmp+38,Field,1);
  Chain[CC]->Helix[HC]->Class = atoi(Field[0]);

  Chain[CC]->NHelix++;

  return(SUCCESS);
}

int svt_stride::Process_JRNL(BUFFER Buffer, BOOLEAN *Published)
{
  char *Field[MAX_FIELD];

  SplitString(Buffer,Field,10);

  if( !strncmp(Field[1],"REF",3) && !strncmp(Field[2],"TO",2) &&
      !strncmp(Field[3],"BE",2)  && !strncmp(Field[4],"PUBLISHED",9) )
    *Published = NO;

  return(SUCCESS);
}


int svt_stride::Process_MODEL(enum METHOD *Method)
{

  if( *Method == XRay )
    *Method = Model;

  return(SUCCESS);
}

int svt_stride::Process_REMARK(BUFFER Buffer, enum METHOD *Method, float *Resolution, BOOLEAN *DsspAssigned)
{
  char *Field[MAX_FIELD];
  int NFields;

  NFields = SplitString(Buffer,Field,10);

  if( NFields >= 5 &&  !strncmp(Field[2],"RESOLUTION",10) &&
      !strncmp(Field[4],"ANGSTROMS",9) && isdigit(*Field[3]) )
    *Resolution = atof(Field[3]);

  if( NFields >= 9 && !strcmp(Field[2],"THESE") && !strcmp(Field[3],"COORDINATES") &&
      !strcmp(Field[4],"WERE")  && !strcmp(Field[5],"GENERATED") &&
      !strcmp(Field[6],"FROM")  && !strcmp(Field[7],"SOLUTION") &&
      ( !strcmp(Field[8],"NMR") || !strcmp(Field[8],"/NMR$") ) ) *Method = NMR;

  if( strstr(Buffer,"SANDER ") || strstr(Buffer,"SANDER,") || strstr(Buffer,"SANDER:") ||
      strstr(Buffer,"SANDER;") || strstr(Buffer,"SANDER.") || strstr(Buffer,"SANDER(") ||
      strstr(Buffer,"SANDER)") || strstr(Buffer,"DSSP") )
    *DsspAssigned = YES;

  return(SUCCESS);
}


int svt_stride::Process_SHEET(BUFFER Buffer, CHAIN **Chain, int *ChainNumber, COMMAND *Cmd)
{
  int CC, SC, STC;
  char *Field[MAX_FIELD];
  BUFFER Tmp;
  static char PreviousChain, PreviousSheetId[RES_FIELD];

  if( Cmd->NActive && !ChInStr(Cmd->Active,SpaceToDash(Buffer[21])) )
     return(SUCCESS);

  for( CC=0; CC < *ChainNumber && Chain[CC]->Id != Buffer[21]; CC++ ) { };

  if( CC == *ChainNumber ) {
    InitChain(&Chain[CC]);
    Chain[CC]->Id = Buffer[21];
    (*ChainNumber)++;
  }

  if( Chain[CC]->NSheet == -1 ) {
    PreviousChain = '*';
    strcpy(PreviousSheetId,"*");
  }

  SplitString(Buffer+7,Field,2);

  if( atoi(Field[0]) == 1 || Buffer[21] != PreviousChain ) {
    if( Buffer[21] == PreviousChain && !strcmp(PreviousSheetId,Field[1]) )
      return(FAILURE);
/*      die("Here it is! -> %s%c\n", Chain[CC]->File,Chain[CC]->Id);*/
    Chain[CC]->NSheet++;
    SC = Chain[CC]->NSheet;
    Chain[CC]->Sheet[SC] =  (SHEET *)ckalloc(sizeof(SHEET));
    Chain[CC]->Sheet[SC]->NStrand = 0;
    STC = Chain[CC]->Sheet[SC]->NStrand;
    strcpy(Chain[CC]->Sheet[SC]->SheetId,Field[1]);
  }
  else
    {
      SC = Chain[CC]->NSheet;
      STC = Chain[CC]->Sheet[SC]->NStrand;
    }

  SplitString(Buffer+17,Field,1);
  strcpy(Chain[CC]->Sheet[SC]->ResType1[STC],Field[0]);

  SplitString(Buffer+28,Field,1);
  strcpy(Chain[CC]->Sheet[SC]->ResType2[STC],Field[0]);

  strcpy(Tmp,Buffer);
  Tmp[27] = ' ';
  Tmp[38] = ' ';
  SplitString(Tmp+22,Field,1);
  strcpy(Chain[CC]->Sheet[SC]->PDB_ResNumb1[STC],Field[0]);
  SplitString(Tmp+33,Field,1);
  strcpy(Chain[CC]->Sheet[SC]->PDB_ResNumb2[STC],Field[0]);

  Chain[CC]->Sheet[SC]->InsCode1[STC] = Buffer[26];
  Chain[CC]->Sheet[SC]->InsCode2[STC] = Buffer[37];

  SplitString(Buffer+38,Field,1);
  Chain[CC]->Sheet[SC]->Sence[STC] = atoi(Field[0]);

  if( Buffer[45] != ' ' ) {
    Chain[CC]->Sheet[SC]->RegYN[STC] = 1;
    SplitString(Buffer+45,Field,1);
    strcpy(Chain[CC]->Sheet[SC]->ResTypeReg1[STC],Field[0]);
    SplitString(Buffer+60,Field,1);
    strcpy(Chain[CC]->Sheet[SC]->ResTypeReg2[STC],Field[0]);

    Tmp[55] = ' ';
    Tmp[70] = ' ';
    SplitString(Tmp+50,Field,1);
    strcpy(Chain[CC]->Sheet[SC]->PDB_ResNumbReg1[STC],Field[0]);
    SplitString(Tmp+66,Field,1);
    strcpy(Chain[CC]->Sheet[SC]->PDB_ResNumbReg2[STC],Field[0]);

    Chain[CC]->Sheet[SC]->InsCodeReg1[STC] = Buffer[54];
    Chain[CC]->Sheet[SC]->InsCodeReg2[STC] = Buffer[69];

    Tmp[45] = ' ';
    Tmp[60] = ' ';
    SplitString(Tmp+41,Field,1);
    strcpy(Chain[CC]->Sheet[SC]->AtomNameReg1[STC],Field[0]);
    SplitString(Tmp+56,Field,1);
    strcpy(Chain[CC]->Sheet[SC]->AtomNameReg2[STC],Field[0]);
  }
  else Chain[CC]->Sheet[SC]->RegYN[STC] = 0;

  strcpy(PreviousSheetId,Chain[CC]->Sheet[SC]->SheetId);
  PreviousChain = Buffer[21];
  Chain[CC]->Sheet[SC]->NStrand++;

  return(SUCCESS);
}

int svt_stride::Process_SSBOND(BUFFER Buffer, CHAIN **Chain, int *ChainNumber, COMMAND *Cmd)
{
  int CC, BC;
  char *Field[MAX_FIELD];
  BUFFER Tmp;

  if( Cmd->NActive && !ChInStr(Cmd->Active,SpaceToDash(Buffer[15])) )
     return(SUCCESS);

  CC = 0;

  if( *ChainNumber == 0 ) {
    InitChain(&Chain[CC]);
    Chain[CC]->Id = Buffer[15];
    (*ChainNumber)++;
  }

  BC = Chain[CC]->NBond;
  Chain[CC]->SSbond[BC] =  (SSBOND *)ckalloc(sizeof(SSBOND));

  strcpy(Tmp,Buffer);
  Tmp[21] = ' ';
  Tmp[35] = ' ';
  SplitString(Tmp+17,Field,1);
  strcpy(Chain[CC]->SSbond[BC]->PDB_ResNumb1,Field[0]);
  SplitString(Tmp+31,Field,1);
  strcpy(Chain[CC]->SSbond[BC]->PDB_ResNumb2,Field[0]);

  Chain[CC]->SSbond[BC]->ChainId1 = Buffer[15];
  Chain[CC]->SSbond[BC]->ChainId2 = Buffer[29];

  Chain[CC]->SSbond[BC]->InsCode1 = Buffer[21];
  Chain[CC]->SSbond[BC]->InsCode2 = Buffer[35];

  Chain[CC]->SSbond[BC]->AsnSource = Pdb;

  Chain[CC]->NBond++;

  return(SUCCESS);
}

int svt_stride::Process_TURN(BUFFER Buffer, CHAIN **Chain, int *ChainNumber, COMMAND *Cmd)
{
  int CC, TC;
  char *Field[MAX_FIELD];
  BUFFER Tmp;

  if( Cmd->NActive && !ChInStr(Cmd->Active,SpaceToDash(Buffer[19])) )
     return(SUCCESS);

  for( CC=0; CC < *ChainNumber && Chain[CC]->Id != Buffer[19]; CC++ ) { } 	;

  if( CC == *ChainNumber ) {
    InitChain(&Chain[CC]);
    Chain[CC]->Id = Buffer[19];
    (*ChainNumber)++;
  }

  TC = Chain[CC]->NTurn;
  Chain[CC]->Turn[TC] = (TURN *)ckalloc(sizeof(TURN));

  SplitString(Buffer+15,Field,1);

  strcpy(Chain[CC]->Turn[TC]->Res1,Field[0]);

  SplitString(Buffer+26,Field,1);
  strcpy(Chain[CC]->Turn[TC]->Res2,Field[0]);

  strcpy(Tmp,Buffer);
  Tmp[24] = ' ';
  Tmp[35] = ' ';
  SplitString(Tmp+20,Field,1);
  strcpy(Chain[CC]->Turn[TC]->PDB_ResNumb1,Field[0]);
  SplitString(Tmp+31,Field,1);
  strcpy(Chain[CC]->Turn[TC]->PDB_ResNumb2,Field[0]);

  Chain[CC]->Turn[TC]->InsCode1 = Buffer[24];
  Chain[CC]->Turn[TC]->InsCode2 = Buffer[35];

  Chain[CC]->NTurn++;

  return(SUCCESS);
}


/*************************************************************************
**                                                                      **
** Get PDB secondary structure assignment for every residue             **
**                                                                      **
*************************************************************************/
void svt_stride::GetPdbAsn(CHAIN **Chain, int NChain)
{

  register int i, j, k;
  int Cn, Beg, End;
  char SecondStr;
  CHAIN *c;

  for( Cn=0; Cn<NChain; Cn++ ) {

    c = Chain[Cn];

    for( i=0; i<c->NHelix; i++ ) {

      switch( c->Helix[i]->Class ) {
      case 1:  SecondStr = 'H';
	break;
      case 3:  SecondStr = 'I';
	break;
      case 5:  SecondStr = 'G';
	break;
      }

      if( PdbN2SeqN(c,c->Helix[i]->PDB_ResNumb1,&Beg) &&
	  PdbN2SeqN(c,c->Helix[i]->PDB_ResNumb2,&End) )
	for( j=Beg; j<=End; j++ )
	  if( c->Rsd[j]->Prop->PdbAsn != 'H' )
	    c->Rsd[j]->Prop->PdbAsn = SecondStr;
    }

    for( i=0; i<c->NSheet; i++ )
      for( j=0; j<c->Sheet[i]->NStrand; j++ ) {
	  if( PdbN2SeqN(c,c->Sheet[i]->PDB_ResNumb1[j],&Beg) &&
	      PdbN2SeqN(c,c->Sheet[i]->PDB_ResNumb2[j],&End) )
	    for( k=Beg; k<=End; k++ )
	      if( c->Rsd[k]->Prop->PdbAsn != 'H' )
		c->Rsd[k]->Prop->PdbAsn = 'E';
	}

    for( i=0; i<c->NTurn; i++ ) {
      if( PdbN2SeqN(c,c->Turn[i]->PDB_ResNumb1,&Beg) &&
	  PdbN2SeqN(c,c->Turn[i]->PDB_ResNumb2,&End) )
	for( j=Beg; j<=End; j++ )
	  if( c->Rsd[j]->Prop->PdbAsn != 'H' &&  c->Rsd[j]->Prop->PdbAsn != 'E' )
	    c->Rsd[j]->Prop->PdbAsn = 'T';
    }
  }
}

void svt_stride::BackboneAngles(CHAIN **Chain, int NChain)
{

  register int Res, Cn;

  for( Cn=0; Cn<NChain; Cn++ ) {

    for( Res=0; Res<Chain[Cn]->NRes; Res++ ) {
      PHI(Chain[Cn],Res);
      PSI(Chain[Cn],Res);
    }
  }
}

void svt_stride::DiscrPhiPsi(CHAIN **Chain, int NChain, COMMAND *Cmd)
{

  register int i, Res, Cn;
  RESIDUE *r;

  for( Cn=0; Cn<NChain; Cn++ ) {

    for( Res=0; Res<Chain[Cn]->NRes; Res++ ) {

      r = Chain[Cn]->Rsd[Res];

      r->Prop->PhiZn = ERR;
      r->Prop->PsiZn = ERR;

      if( Res != 0 ) {
	for( i=0; i<Cmd->NPixel; i++ )
	  if( r->Prop->Phi  >  MINPHIPSI+(float)(i)*Cmd->PhiPsiStep &&
	      r->Prop->Phi <=  MINPHIPSI+(float)(i+1)*Cmd->PhiPsiStep ) {
	    r->Prop->PhiZn = i;
	    break;
	  }
      }

      if( Res != Chain[Cn]->NRes-1 ) {
	for( i=0; i<Cmd->NPixel; i++ )
	  if( r->Prop->Psi  >  MINPHIPSI+(float)(i)*Cmd->PhiPsiStep &&
	      r->Prop->Psi <=  MINPHIPSI+(float)(i+1)*Cmd->PhiPsiStep ) {
	    r->Prop->PsiZn = i;
	    break;
	  }
      }

    }

    for(Res=0; Res<Chain[Cn]->NRes; Res++ )  {
      r = Chain[Cn]->Rsd[Res];
      if( Res != 0 && r->Prop->PsiZn == ERR )
	r->Prop->PsiZn = Chain[Cn]->Rsd[Res-1]->Prop->PsiZn;
      if( Res != Chain[Cn]->NRes-1 && r->Prop->PhiZn == ERR )
	r->Prop->PhiZn = Chain[Cn]->Rsd[Res+1]->Prop->PhiZn;
    }

  }
}

int svt_stride::PlaceHydrogens(CHAIN *Chain)
{

  int Res, i, N, C, CA, H, PlacedCnt=0;
  float Length_N_C, Length_N_CA, Length_N_H;
  RESIDUE *r, *rr;

  for( Res=1; Res<Chain->NRes; Res++ ) {

    r  = Chain->Rsd[Res];
    rr = Chain->Rsd[Res-1];

      if( !strcmp(r->ResType,"PRO") ) continue;

      /* Replace deiterium atoms by hydrogens */
      if( FindAtom(Chain,Res,"D",&H) )
	strcpy(r->AtomType[H],"H");

      if( !FindAtom(Chain,Res,"H",&H)  && FindAtom(Chain,Res,"N",&N)   &&
	   FindAtom(Chain,Res-1,"C",&C) && FindAtom(Chain,Res,"CA",&CA) ) {

	H = r->NAtom;

	Length_N_C   = Dist(r->Coord[N],rr->Coord[C]);
	Length_N_CA  = Dist(r->Coord[N],r->Coord[CA]);

	for( i=0; i<3; i++ )
	  r->Coord[H][i] = r->Coord[N][i] -
	      ( (rr->Coord[C][i] -  r->Coord[N][i])/Length_N_C +
	        (r->Coord[CA][i]  -  r->Coord[N][i])/Length_N_CA );

	Length_N_H = Dist(r->Coord[N],r->Coord[H]);

	for( i=0; i<3; i++ )
	  r->Coord[H][i] = r->Coord[N][i] +
	    DIST_N_H*(r->Coord[H][i]-r->Coord[N][i])/Length_N_H;

	strcpy(r->AtomType[H],"H");
	r->NAtom++;
	PlacedCnt++;
      }
    }
  return(PlacedCnt);
}

int svt_stride::ReadPhiPsiMap(char *MapFile, float ***PhiPsiMap, COMMAND *Cmd)
{

  int i, j, NFields, Cnt=0;
  FILE *fi;
  BUFFER Buffer;
  char *Fields[MAX_FIELD];

  Cmd->NPixel = 0;

  if( (fi = fopen(MapFile,"r")) != 0 ) {

    while( fgets(Buffer,BUFSZ,fi) != NULL ) {
      if( !(NFields = SplitString(Buffer,Fields,MAX_FIELD)) ) continue;
      if( Cmd->NPixel == 0 ) {
	if( !strcmp(tolostr(Fields[0]),"npixel") ) {
	  Cmd->NPixel = atoi(Fields[1]);
	  if( Cmd->NPixel < 1 || Cmd->NPixel > 1000 )
	    die("Wrong number of pixels in the PhiPsi Map file %s\n",MapFile);
	  *PhiPsiMap = FloatMatrix(Cmd->NPixel,Cmd->NPixel);
	}
      }
      else  {
	if( !strcmp(tolostr(Fields[0]),"pixel") ) {
	  if( (i = atoi(Fields[1])) >= 0 && i < Cmd->NPixel &&
	      (j = atoi(Fields[2])) >= 0 && j < Cmd->NPixel &&
	     Cnt ==  i*Cmd->NPixel+j &&  NFields >= 4 ) {
	    (*PhiPsiMap)[i][j] = atof(Fields[5]);
	    Cnt++;
	  }
	  else die("Error in the PhiPsi Map file %s\n",MapFile);
	}
      }
    }
    fclose(fi);
    Cmd->PhiPsiStep = (MAXPHIPSI - MINPHIPSI)/(float)Cmd->NPixel;
  }

  if( !Cmd->NPixel ) die("Error reading PhiPsiMap file %s\n",MapFile);

  return(Cmd->NPixel);
}

float **svt_stride::DefaultHelixMap(COMMAND *Cmd)
{

  register int i;

  float **Map;
  static float Data[DEFNUMPIXEL][DEFNUMPIXEL] = {

      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0009014423, 0.0041898815,
      0.0085105160, 0.0133839026, 0.0245425366, 0.0407802090, 0.0464176536, 0.0330946408,
      0.0134803243, 0.0024038462, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0007370283, 0.0077203326, 0.0269849468,
      0.0492307022, 0.0621860325, 0.0747849122, 0.0919913873, 0.0918549150, 0.0617070347,
      0.0241584498, 0.0041428790, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0041416897, 0.0287234355, 0.0835687742,
      0.1384727061, 0.1562444866, 0.1470608264, 0.1360232681, 0.1159155145, 0.0742164999,
      0.0290896539, 0.0050673936, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0009375000, 0.0156580955, 0.0757770315, 0.1856354773,
      0.2785892785, 0.2880102694, 0.2332847565, 0.1741978228, 0.1281246394, 0.0793832615,
      0.0320557840, 0.0058840578, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0048893229, 0.0437000208, 0.1617751122, 0.3399706185,
      0.4626395404, 0.4418565035, 0.3235570788, 0.2100441158, 0.1358627081, 0.0776144490,
      0.0297011137, 0.0052390974, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0136979166, 0.0917820632, 0.2773087323, 0.5047551394,
      0.6214492917, 0.5485223532, 0.3655386865, 0.2054343373, 0.1121114418, 0.0548815951,
      0.0178668182, 0.0025975490, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0246484373, 0.1396044195, 0.3594934344, 0.5710113049,
      0.6337110400, 0.5133636594, 0.3054708838, 0.1402616948, 0.0584463216, 0.0228670351,
      0.0058531328, 0.0005151099, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0265885405, 0.1365883052, 0.3163702190, 0.4545661211,
      0.4628692269, 0.3425511420, 0.1761947423, 0.0607788190, 0.0158569515, 0.0042061093,
      0.0008107311, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0152018229, 0.0738445148, 0.1630392224, 0.2269553691,
      0.2237145752, 0.1528334022, 0.0652616471, 0.0150429625, 0.0014589608, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0035156249, 0.0165251363, 0.0379281938, 0.0584417619,
      0.0619409233, 0.0404052660, 0.0136552500, 0.0016678370, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0011718750, 0.0046875002,
      0.0070312503, 0.0046875002, 0.0011718750, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0006944445, 0.0036063762, 0.0080820229, 0.0101532144, 0.0076146079,
      0.0032324446, 0.0006009616, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000}
  };


  Map = (float **)ckalloc(DEFNUMPIXEL*sizeof(float *));

  for( i=0; i<DEFNUMPIXEL; i++ )
    Map[i] = &(Data[i][0]);

  Cmd->NPixel = DEFNUMPIXEL;
  Cmd->PhiPsiStep = (MAXPHIPSI - MINPHIPSI)/(float)Cmd->NPixel;

  return(Map);
}


float **svt_stride::DefaultSheetMap(COMMAND *Cmd)
{

  register int i;
  float **Map;
  static float Data[DEFNUMPIXEL][DEFNUMPIXEL] = {

      {0.2769023776, 0.1408346891, 0.0464910716, 0.0073784725, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0047086575, 0.0218229108, 0.0569166169,
      0.1254088134, 0.2340224832, 0.3511219919, 0.4355685711, 0.4584180117, 0.4007356465},
      {0.4067636132, 0.2329865396, 0.0927943364, 0.0237838365, 0.0055147060, 0.0013786765,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0088186050, 0.0420726910, 0.1043856740,
      0.2086037844, 0.3677131534, 0.5367187858, 0.6412357688, 0.6458424330, 0.5580080152},
      {0.4286311865, 0.2678007782, 0.1282834113, 0.0529448465, 0.0220588241, 0.0055147060,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0086062262, 0.0445192643, 0.1197573245,
      0.2487278134, 0.4369854629, 0.6241853237, 0.7160459757, 0.6829043031, 0.5716546178},
      {0.3639202416, 0.2397334576, 0.1305907220, 0.0683420748, 0.0330882370, 0.0082720593,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0053559211, 0.0328565054, 0.1048930883,
      0.2402425259, 0.4295993447, 0.6026929021, 0.6669865251, 0.6039550304, 0.4841639400},
      {0.2637948096, 0.1723874062, 0.0920098722, 0.0464194641, 0.0220588241, 0.0055147060,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0030202419, 0.0224239044, 0.0804052502,
      0.1923188865, 0.3456886411, 0.4811576009, 0.5223571062, 0.4586051404, 0.3565762639},
      {0.1628032923, 0.0930610597, 0.0400134660, 0.0143100554, 0.0055147060, 0.0013786765,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0015453297, 0.0132468110, 0.0489843786,
      0.1174781919, 0.2150468081, 0.3082944453, 0.3439011276, 0.3080393970, 0.2371628135},
      {0.0825822726, 0.0338854715, 0.0092895878, 0.0012122844, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0003863324, 0.0046614520, 0.0186656341,
      0.0477515720, 0.0961741805, 0.1546680480, 0.1961039603, 0.1944279373, 0.1469529718},
      {0.0326442868, 0.0073916214, 0.0008854167, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0006347656, 0.0031504754,
      0.0104655549, 0.0272454955, 0.0570511036, 0.0941907763, 0.1088592261, 0.0785619915},
      {0.0090501504, 0.0007651417, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0007207961, 0.0035958111, 0.0131648667, 0.0318824202, 0.0425693691, 0.0292618107},
      {0.0013020834, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0013020834, 0.0052083335, 0.0078125000, 0.0052083335},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000},
      {0.0210939310, 0.0078523019, 0.0013020834, 0.0000000000, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0014204546,
      0.0071634995, 0.0169352461, 0.0272206441, 0.0357281528, 0.0395361669, 0.0343801714},
      {0.1146211401, 0.0503530800, 0.0130920913, 0.0015190972, 0.0000000000, 0.0000000000,
      0.0000000000, 0.0000000000, 0.0000000000, 0.0010016026, 0.0046167620, 0.0157516468,
      0.0453012958, 0.0937970504, 0.1454590708, 0.1861637682, 0.2019522935, 0.1764564067}
  };


  Map = (float **)ckalloc(DEFNUMPIXEL*sizeof(float *));

  for( i=0; i<DEFNUMPIXEL; i++ )
    Map[i] = &(Data[i][0]);

  Cmd->NPixel = DEFNUMPIXEL;
  Cmd->PhiPsiStep = (MAXPHIPSI - MINPHIPSI)/(float)Cmd->NPixel;

  return(Map);
}

int svt_stride::ReadPDBFile(CHAIN **Chain, int *Cn, COMMAND *Cmd, vector<char *> & rvs)
{
    int ChainCnt, InfoCnt, i;
    enum METHOD Method = XRay;
    BOOLEAN First_ATOM, Published=YES, DsspAssigned=NO;
    float Resolution = 0.0;
    char *Info[MAX_INFO], PdbIdent[5];
    RESIDUE *r;
    CHAIN *c = 0;

    *Cn= 0;
    InfoCnt = 0;
    strcpy(PdbIdent,"~~~~");

    First_ATOM = YES;
    char *rvs_one_line;

    unsigned int ii;
    unsigned int rvs_size = rvs.size();

    for (ii=0; ii<rvs_size; ii++)
    {
        rvs_one_line = rvs[ii];
        if (!Process_ATOM(Chain,Cn,&First_ATOM,Cmd,rvs_one_line ))
        {
            return 0;
        }
    }

    for( ChainCnt=0; ChainCnt< *Cn; ChainCnt++ )
    {
        c = Chain[ChainCnt];
        if( c->NRes != 0  && !FindAtom(c,c->NRes,"CA",&i) )
            c->NRes--;
        strcpy(c->File,Cmd->InputFile);

        strcpy(c->PdbIdent,PdbIdent);
        if( c->NRes != 0 )  c->NRes++;
        if( c->NSheet != -1 ) c->NSheet++;
        c->Resolution = Resolution;
        c->Method = Method;
        c->Published = Published;
        c->DsspAssigned = DsspAssigned;
        c->NInfo = InfoCnt;
        for(i=0; i<InfoCnt; i++)
        {
            c->Info[i] = (char *)ckalloc(BUFSZ*sizeof(char));
            strcpy(c->Info[i],Info[i]);
            c->Info[i][71] = '\0';
        }
        for( i=0; i<c->NRes; i++ )
        {
            r = c->Rsd[i];
            r->Inv =  (INVOLVED *)ckalloc(sizeof(INVOLVED));
            r->Prop = (PROPERTY *)ckalloc(sizeof(PROPERTY));
            r->Inv->NBondDnr = 0;
            r->Inv->NBondAcc = 0;
            r->Inv->InterchainHBonds = NO;
            r->Prop->Asn     = 'C';
            r->Prop->PdbAsn  = 'C';
            r->Prop->DsspAsn = 'C';
            r->Prop->Solv    = 0.0;
            r->Prop->Phi     = 360.0;
            r->Prop->Psi     = 360.0;
        }
    }

    for(i=0; i<InfoCnt; i++)
        free(Info[i]);

    return(SUCCESS);
}

void svt_stride::Report(CHAIN **Chain, int NChain, HBOND **HBond, COMMAND *Cmd, vector<char *> & vectorRaportString)
{

  //if( !Cmd->ReportSummaryOnly )
  //  ReportGeneral(Chain,vectorRaportString);

  //ReportSummary(Chain,NChain,vectorRaportString,Cmd);
  //ReportShort(Chain,NChain,vectorRaportString,Cmd);
  //ReportTurnTypes(Chain,NChain,vectorRaportString,Cmd);
  //ReportSSBonds(Chain,vectorRaportString);

  if( !Cmd->ReportSummaryOnly )
    ReportDetailed(Chain,NChain, Cmd, vectorRaportString);

  //if( Cmd->ReportBonds )
  //  ReportHydrBonds(Chain,NChain,HBond,vectorRaportString,Cmd);


  //if( Cmd->Measure ) {
  //  Measure(Chain,NChain,0,Cmd,Out);
  //  Measure(Chain,NChain,1,Cmd,Out);
  //}


}


void svt_stride::ReportGeneral(CHAIN **Chain, vector<char *> & vectorRaportString)
{
  register int i;
  BUFFER Bf, Tmp;

  if( !Chain[0]->NInfo )
    return;

  PrepareBuffer(Bf,Chain);

  //Glue(Bf,"REM  --------------------------------------------------------------------",vectorRaportString);
  //Glue(Bf,"REM",vectorRaportString);
  //Glue(Bf,"REM  STRIDE: Knowledge-based secondary structure assignment",vectorRaportString);
  //Glue(Bf,"REM  Please cite: D.Frishman & P.Argos, Proteins 23, 566-579, 1995",vectorRaportString);
  //Glue(Bf,"REM",vectorRaportString);
  //Glue(Bf,"REM  Residue accessible surface area calculation",vectorRaportString);
  //Glue(Bf,"REM  Please cite: F.Eisenhaber & P.Argos, J.Comp.Chem. 14, 1272-1280, 1993 ", vectorRaportString);
  //Glue(Bf,"REM               F.Eisenhaber et al., J.Comp.Chem., 1994, submitted", vectorRaportString);
  //Glue(Bf,"REM", vectorRaportString);

  sprintf(Tmp,"REM  ------------------------ ");
  strcat(Tmp,"General information");
  strcat(Tmp," -----------------------");
  //Glue(Bf,Tmp, vectorRaportString);
  //Glue(Bf,"REM", vectorRaportString);

  for( i=0; i<Chain[0]->NInfo; i++ ) {
    strcpy(Tmp,Chain[0]->Info[i]);
    Tmp[66] = '\0';
    Replace(Tmp,'\n',' ');
    //Glue(Bf,Tmp, vectorRaportString);
  }
  //Glue(Bf,"REM", vectorRaportString);
}

void svt_stride::ReportSummary(CHAIN **Chain, int NChain, vector<char *> & vectorRaportString, COMMAND *Cmd)
{
  int Cn, Width, CurrWidth, NBlocks, Tail, i, j, From, To;
  BUFFER Bf, Tmp, Tmp1;

  PrepareBuffer(Bf,Chain);

  sprintf(Tmp,"REM  -------------------- ");
  strcat(Tmp,"Secondary structure summary");
  strcat(Tmp," -------------------");
  //Glue(Bf,Tmp,vectorRaportString);

  for( Cn=0; Cn<NChain; Cn++ ) {

    if( !Chain[Cn]->Valid )
      continue;

    Width = 50;

    //Glue(Bf,"REM",vectorRaportString);

    strncpy(Tmp1,Chain[Cn]->File,40);
    Tmp1[40] = '\0';
    sprintf(Tmp,"CHN  %s %c",Tmp1,SpaceToDash(Chain[Cn]->Id));
    //Glue(Bf,Tmp,vectorRaportString);

    NBlocks = Chain[Cn]->NRes/Width;
    Tail = Chain[Cn]->NRes % Width;
    if( Tail ) NBlocks++;

    for( i=0; i<NBlocks; i++ ) {

      //Glue(Bf,"REM",vectorRaportString);
      From = i*Width;
      if( i == NBlocks-1 && Tail )
	CurrWidth = Tail;
      else
	CurrWidth = Width;
      To   = From+CurrWidth;

      sprintf(Tmp,"REM       ");
      for( j=0; j<CurrWidth; j++ )
	if( j && (j+1)%10 == 0 )
	  strcat(Tmp,".");
	else
	  strcat(Tmp," ");
      //Glue(Bf,Tmp,vectorRaportString);

      sprintf(Tmp,"SEQ  %-4d ",From+1);
      for( j=From; j<From+Width; j++ ) {
	if( j < To )
	  sprintf(Tmp1,"%c",ThreeToOne(Chain[Cn]->Rsd[j]->ResType));
	else
	  sprintf(Tmp1," ");
	strcat(Tmp,Tmp1);
      }
      sprintf(Tmp1," %4d",To);
      strcat(Tmp,Tmp1);
      //Glue(Bf,Tmp,vectorRaportString);

      sprintf(Tmp,"STR       ");
      for( j=From; j<To; j++ ) {
	if( Chain[Cn]->Rsd[j]->Prop->Asn == 'C' )
	  sprintf(Tmp1," ");
	else
	  sprintf(Tmp1,"%c",Chain[Cn]->Rsd[j]->Prop->Asn);
	strcat(Tmp,Tmp1);
      }
      strcat(Tmp,"     ");
      //Glue(Bf,Tmp,vectorRaportString);
    }

  }
}


void svt_stride::ReportDetailed(CHAIN **Chain, int NChain, COMMAND *Cmd, vector<char *> & vectorRaportString)
{
  register int i, Cn;
  RESIDUE *p;
  BUFFER Bf;
  BUFFER Tmp;
  PrepareBuffer(Bf,Chain);

  char *TmpA;

  //Glue(Bf,"REM",vectorRaportString);
  sprintf(Tmp,"REM  --------------- ");
  strcat(Tmp,"Detailed secondary structure assignment");
  strcat(Tmp,"-------------");
  //Glue(Bf,Tmp,vectorRaportString);
  //Glue(Bf,"REM",vectorRaportString);
  //Glue(Bf,"REM  |---Residue---|    |--Structure--|   |-Phi-|   |-Psi-|  |-Area-|      ",vectorRaportString);

  for( Cn=0; Cn<NChain; Cn++ )
  {
    if( !Chain[Cn]->Valid )
      continue;

    for( i=0; i<Chain[Cn]->NRes; i++ )
    {
        TmpA=new char[BUFSZ];

        p = Chain[Cn]->Rsd[i];
        sprintf(TmpA,"ASG  %3s %c %4s %4d    %c   %11s   %7.2f   %7.2f   %7.1f",
                p->ResType,SpaceToDash(Chain[Cn]->Id),p->PDB_ResNumb,i+1,
                p->Prop->Asn,Translate(p->Prop->Asn),p->Prop->Phi,
                p->Prop->Psi,p->Prop->Solv);
        //cout << TmpA << endl;

        vectorRaportString.push_back(TmpA);

        //  if( Cmd->BrookhavenAsn ) {
        //Tmp[26] = p->Prop->PdbAsn;
        //Tmp[25] = ' ';
        //Tmp[27] = ' ';
        //  }

        //  if( Cmd->DsspAsn ) {
        //Tmp[28] = p->Prop->DsspAsn;
        //Tmp[27] = ' ';
        //Tmp[29] = ' ';
        //sprintf(Tmp1," %6.1f ",p->Prop->DsspSolv);
        //strcat(Tmp,Tmp1);
        //  }
        //   Glue(Bf,Tmp,vectorRaportString);

        //delete[] TmpA;

    }
  }
}

void svt_stride::ReportHydrBonds(CHAIN **Chain, int NChain, HBOND **HBond, vector<char *> & vectorRaportString,
		     COMMAND *Cmd)
{
  register int i, k, Cn;
  int Cnt, Res;
  BUFFER Bf, Tmp, Tmp1;
  HBOND *p;
  RESIDUE *r;

  PrepareBuffer(Bf,Chain);

  //Glue(Bf,"REM",vectorRaportString);
  sprintf(Tmp,"REM  ------------------ ");
  strcat(Tmp,"Mainchain hydrogen bonds");
  strcat(Tmp," ------------------------");
  //Glue(Bf,Tmp,vectorRaportString);
  //Glue(Bf,"REM",vectorRaportString);

  //Glue(Bf,"REM  Definition of Stickle et al., J.Mol.Biol. 226:1143-1159, 1992",vectorRaportString);
  //Glue(Bf,"REM  A1 is the angle between the planes of donor complex and O..N-C",vectorRaportString);
  //Glue(Bf,"REM  A2 is the angle between the planes of acceptor complex and N..O=C",vectorRaportString);
  //Glue(Bf,"REM",vectorRaportString);

  sprintf(Tmp,"HBT  %-6d",Chain[0]->NHydrBondTotal);
  //Glue(Bf,Tmp,vectorRaportString);
  sprintf(Tmp,"HBI  %-6d",Chain[0]->NHydrBondInterchain);
  //Glue(Bf,Tmp,vectorRaportString);
  for( Cn=0; Cn<NChain; Cn++ )
    if( Chain[Cn]->Valid ) {

      sprintf(Tmp,"HBC  %-6d  %s %c %4d",
	      Chain[Cn]->NHydrBond,Chain[Cn]->File,SpaceToDash(Chain[Cn]->Id),Chain[Cn]->NRes);
      //Glue(Bf,Tmp,vectorRaportString);
    }
  //Glue(Bf,"REM",vectorRaportString);

  //Glue(Bf,"REM  |--Residue 1--|     |--Residue 2--|  N-O N..O=C O..N-C     A1     A2",vectorRaportString);

  for( Cn=0; Cn<NChain; Cn++ ) {

    if( !Chain[Cn]->Valid )
      continue;

    for( i=0; i<Chain[Cn]->NRes; i++ ) {

      r = Chain[Cn]->Rsd[i];

      Cnt = 0;
      for( k=0; k<r->Inv->NBondDnr; k++ ) {
	p = HBond[r->Inv->HBondDnr[k]];
	if( p->ExistHydrBondRose ) {
	  Res = p->Acc->A_Res;
	  sprintf(Tmp,"DNR %4s %c %4s %4d -> ",
		  r->ResType,SpaceToDash(Chain[Cn]->Id),r->PDB_ResNumb,i);

          sprintf(Tmp1,"%4s %c %4s %4d %4.1f %6.1f %6.1f %6.1f %6.1f ",
                  p->Acc->Chain->Rsd[Res]->ResType,SpaceToDash(Chain[Cn]->Id),
                  p->Acc->Chain->Rsd[Res]->PDB_ResNumb,Res,p->AccDonDist,p->AccAng,
                  p->DonAng,p->AccDonAng,p->DonAccAng);
          strcat(Tmp,Tmp1);
          //Glue(Bf,Tmp,vectorRaportString);
          Cnt++;
        }
      }

      Cnt = 0;
      for( k=0; k<r->Inv->NBondAcc; k++ ) {
	p = HBond[r->Inv->HBondAcc[k]];
	if( p->ExistHydrBondRose ) {
	  Res = p->Dnr->D_Res;
	  sprintf(Tmp,"ACC %4s %c %4s %4d -> ",
		  r->ResType,SpaceToDash(Chain[Cn]->Id),r->PDB_ResNumb,i);

          sprintf(Tmp1,"%4s %c %4s %4d %4.1f %6.1f %6.1f %6.1f %6.1f ",
                  p->Dnr->Chain->Rsd[Res]->ResType,SpaceToDash(Chain[Cn]->Id),
                  p->Dnr->Chain->Rsd[Res]->PDB_ResNumb,Res,p->AccDonDist,
                  p->AccAng,p->DonAng,p->AccDonAng,p->DonAccAng);
          strcat(Tmp,Tmp1);
          //Glue(Bf,Tmp,vectorRaportString);
          Cnt++;
        }
      }

    }
  }

}

void svt_stride::ReportSSBonds(CHAIN **Chain,vector<char *> & vectorRaportString)
{
  register int i;
  BUFFER Bf, Tmp;
  SSBOND *s;

  if( !Chain[0]->NBond ) return;

  PrepareBuffer(Bf,Chain);

  for( i=0; i<Chain[0]->NBond; i++ ) {
    s = Chain[0]->SSbond[i];
    sprintf(Tmp,"LOC  Disulfide    CYS  %4s %c      CYS   %4s %c         ",
	    s->PDB_ResNumb1,SpaceToDash(s->ChainId1),
	    s->PDB_ResNumb2,SpaceToDash(s->ChainId2));

    if( s->AsnSource == Pdb )
      strcat(Tmp,"   PDB");
    else
      strcat(Tmp,"STRIDE\n");
    //Glue(Bf,Tmp,vectorRaportString);
  }
}

void svt_stride::ReportTurnTypes(CHAIN **Chain, int NChain,vector<char *> & vectorRaportString)
{

  register int Cn, Tn;
  BUFFER Bf, Tmp;
  TURN *t;

  Tn = 0;
  for( Cn=0; Cn<NChain; Cn++ )
    if( Chain[Cn]->Valid )
       Tn += Chain[Cn]->NAssignedTurn;

  if( !Tn ) return;

  PrepareBuffer(Bf,Chain);

  for( Cn=0; Cn<NChain; Cn++ ) {
    if( !Chain[Cn]->Valid )
      continue;
    for( Tn=0; Tn<Chain[Cn]->NAssignedTurn; Tn++ ) {
      t = Chain[Cn]->AssignedTurn[Tn];
      sprintf(Tmp,"LOC  %-12s %3s  %4s %c      %3s   %4s %c",
              Translate(t->TurnType),t->Res1,t->PDB_ResNumb1,
              SpaceToDash(Chain[Cn]->Id),t->Res2,
              t->PDB_ResNumb2,SpaceToDash(Chain[Cn]->Id));

      //Glue(Bf,Tmp,vectorRaportString);
    }
  }
}


void svt_stride::ReportShort(CHAIN **Chain, int NChain, vector<char *> & vectorRaportString)
{

  register int Cn, i;
  BUFFER Bf, Tmp;
  char *Asn;
  static char *StrTypes = "HGIE";
  int Bound[MAX_ASSIGN][2], NStr;

  if( !ExistsSecStr(Chain,NChain) )
    return;

  PrepareBuffer(Bf,Chain);

  //Glue(Bf,"REM",vectorRaportString);
  //Glue(Bf,"REM",vectorRaportString);
  //Glue(Bf,"REM",vectorRaportString);

  for( ; *StrTypes!= '\0'; StrTypes++ ) {

    for( Cn=0; Cn<NChain; Cn++ ) {

      if( !Chain[Cn]->Valid )
	continue;

      Asn = (char *)ckalloc(Chain[Cn]->NRes*sizeof(char));
      ExtractAsn(Chain,Cn,Asn);
      NStr = Boundaries(Asn,Chain[Cn]->NRes,(*StrTypes),Bound);

      for( i=0; i<NStr; i++ ) {
        sprintf(Tmp,"LOC  %-12s %3s  %4s %c      %3s   %4s %c",Translate(*StrTypes),
                Chain[Cn]->Rsd[Bound[i][0]]->ResType,
                Chain[Cn]->Rsd[Bound[i][0]]->PDB_ResNumb,
                SpaceToDash(Chain[Cn]->Id),
                Chain[Cn]->Rsd[Bound[i][1]]->ResType,
                Chain[Cn]->Rsd[Bound[i][1]]->PDB_ResNumb,
                SpaceToDash(Chain[Cn]->Id));
        //Glue(Bf,Tmp,Out);
      }

      free(Asn);
    }
  }

}


void svt_stride::PrepareBuffer(BUFFER Bf, CHAIN **Chain)
{

  memset(Bf,' ',OUTPUTWIDTH);

  strcpy(Bf+OUTPUTWIDTH-5,Chain[0]->PdbIdent);
  Bf[OUTPUTWIDTH] = '\0';
  Bf[OUTPUTWIDTH-1]   = '\n';

}

void svt_stride::Glue(const char *String1, const  char *String2, vector<char *> & vectorRaportString)
{
  char *pBf;

  pBf=new char[BUFSZ+1];

  strcpy(pBf,String1);
  strcpy(pBf,String2);

  strncpy(pBf,String2,(int)strlen(String2));
  //vectorRaportString.push_back(pBf);
  delete[] pBf;

}

void svt_stride::Sheet(CHAIN **Chain, int Cn1, int Cn2, HBOND **HBond, COMMAND *Cmd, float **PhiPsiMap)
{
  MPATTERN **PatN, **PatP;
  RESIDUE *Res1, *Res3, *Res2, *Res4, *ResA, *ResB, *Res1m1, *Res3p1;
  int R1, R3, R2, R4, RA, RB, PatCntN = 0, PatCntP = 0, Beg;
  char *AntiPar1, *Par1, *AntiPar2, *Par2;
  register int i;

  PatN = (MPATTERN **)ckalloc(MAXHYDRBOND*sizeof(MPATTERN *));
  PatP = (MPATTERN **)ckalloc(MAXHYDRBOND*sizeof(MPATTERN *));

  AntiPar1  = (char *)ckalloc(Chain[Cn1]->NRes*sizeof(char)); /* Antiparallel strands */
  Par1      = (char *)ckalloc(Chain[Cn1]->NRes*sizeof(char)); /* Parallel strands */
  AntiPar2  = (char *)ckalloc(Chain[Cn2]->NRes*sizeof(char)); /* Antiparallel strands */
  Par2      = (char *)ckalloc(Chain[Cn2]->NRes*sizeof(char)); /* Parallel strands */

  for( i=0; i<Chain[Cn1]->NRes; i++ ) {
    AntiPar1[i] = 'C';
    Par1[i] = 'C';
  }

  for( i=0; i<Chain[Cn2]->NRes; i++ ) {
    AntiPar2[i] = 'C';
    Par2[i] = 'C';
  }

  for( R1=0; R1<Chain[Cn1]->NRes; R1++ ) {

    Res1   = Chain[Cn1]->Rsd[R1];

    if( (!Res1->Inv->NBondDnr && !Res1->Inv->NBondAcc) ||
        ((Cn1 != Cn2) && !Res1->Inv->InterchainHBonds) )
      continue;

    RA     = R1+1;
    R2     = R1+2;
    if (R1 != 0)
        Res1m1 = Chain[Cn1]->Rsd[R1-1];
    else
        Res1m1 = Chain[Cn1]->Rsd[R1];
    ResA   = Chain[Cn1]->Rsd[RA];
    Res2   = Chain[Cn1]->Rsd[R2];

    if( R2 >= Chain[Cn1]->NRes ||
        Res1->Prop->PhiZn == ERR || Res1->Prop->PsiZn == ERR ||
        Res2->Prop->PhiZn == ERR || Res2->Prop->PsiZn == ERR ||
        ResA->Prop->PhiZn == ERR || ResA->Prop->PsiZn == ERR )
      continue;

    if( Cn1 != Cn2 )
      Beg = 0;
    else
      Beg = R1+1;

    for( R3=Beg; R3<Chain[Cn2]->NRes; R3++ ) {

      /* Process anti-parallel strands */

      Res3   = Chain[Cn2]->Rsd[R3];

      if( (!Res3->Inv->NBondAcc && !Res3->Inv->NBondDnr ) ||
	  ((Cn1 != Cn2) && !Res3->Inv->InterchainHBonds) )
	continue;

      RB     = R3-1;
      R4     = R3-2;
      Res3p1 = Chain[Cn2]->Rsd[R3+1];
      ResB   = Chain[Cn2]->Rsd[RB];
      Res4   = Chain[Cn2]->Rsd[R4];

      if( Cn1 != Cn2 || R3 - R1 >= 3  )
	Link(HBond,Chain,Cn1,Cn2,Res1,Res3,Res3,Res1,Res1,Res3,
	     PhiPsiMap,PatN,&PatCntN,"1331",Cmd->Treshold_E1,Cmd,0);

      if( R2 < Chain[Cn1]->NRes && ((Cn1 != Cn2 && R4 >= 0) || R4-R2 >=2 ) )
	Link(HBond,Chain,Cn2,Cn1,Res3,Res1,Res2,Res4,ResB,ResA,
	     PhiPsiMap,PatN,&PatCntN,"3124",Cmd->Treshold_E1,Cmd,0);

      if( ((Cn1 != Cn2 && RB >= 0 ) || RB-R1 > 4) &&
	 ( RA >= Chain[Cn1]->NRes || (Cn1 == Cn2 && R3-RA <= 4 ) ||
	  !Link(HBond,Chain,Cn1,Cn2,Res1,Res3,Res3,ResA,NULL,Res3,
		PhiPsiMap,PatN,&PatCntN,"133A",Cmd->Treshold_E1,Cmd,1))
	 &&
	 ( R1-1 < 0 ||
	  !Link(HBond,Chain,Cn1,Cn2,Res1m1,ResB,ResB,Res1,NULL,ResB,
		PhiPsiMap,PatN,&PatCntN,"1-BB1",Cmd->Treshold_E1,Cmd,1)))
	Link(HBond,Chain,Cn1,Cn2,Res1,Res3,ResB,Res1,Res1,NULL,
	     PhiPsiMap,PatN,&PatCntN,"13B1",Cmd->Treshold_E1,Cmd,0);

      if( (RA < Chain[Cn1]->NRes && (Cn1 != Cn2 || R3-RA > 4)) &&
	 ( (Cn1 == Cn2 && RB-R1 <= 4 ) || (Cn1 != Cn2 && RB < 0 ) ||
	  !Link(HBond,Chain,Cn1,Cn2,Res1,Res3,ResB,Res1,Res1,NULL,
	        PhiPsiMap,PatN,&PatCntN,"13B1",Cmd->Treshold_E1,Cmd,1))
	 &&
	 ( R3+1 >= Chain[Cn2]->NRes ||
	  !Link(HBond,Chain,Cn1,Cn2,ResA,Res3p1,Res3,ResA,ResA,NULL,
		PhiPsiMap,PatN,&PatCntN,"A3+3A",Cmd->Treshold_E1,Cmd,1)))
	Link(HBond,Chain,Cn1,Cn2,Res1,Res3,Res3,ResA,NULL,Res3,
	     PhiPsiMap,PatN,&PatCntN,"133A",Cmd->Treshold_E1,Cmd,0);

      /* Process parallel strands */

      R4 = R3+2;
      RB = R3+1;
      ResB   = Chain[Cn2]->Rsd[RB];
      Res4   = Chain[Cn2]->Rsd[R4];

      if( (Cn1 == Cn2 && abs(R3-R1) <= 3) || R4 >= Chain[Cn2]->NRes ) continue;

      if( R2 < Chain[Cn1]->NRes && (Cn1 != Cn2 || abs(R2-R3) > 3) )
	Link(HBond,Chain,Cn2,Cn1,Res3,Res1,Res2,Res3,Res3,ResA,
	     PhiPsiMap,PatP,&PatCntP,"3123",Cmd->Treshold_E2,Cmd,0);

      if( R4 < Chain[Cn2]->NRes && (Cn1 != Cn2 || abs(R4-R1) > 3) )
	Link(HBond,Chain,Cn1,Cn2,Res1,Res3,Res4,Res1,Res1,ResB,
	     PhiPsiMap,PatP,&PatCntP,"1341",Cmd->Treshold_E2,Cmd,0);
    }
  }

  FilterAntiPar(PatN,PatCntN);
  FilterPar(PatP,PatCntP);

  MergePatternsAntiPar(PatN,PatCntN);
  MergePatternsPar(PatP,PatCntP);

  if( Cmd->Info )  {
    PrintPatterns(PatN,PatCntN,Chain,Cn1,Cn2);
    PrintPatterns(PatP,PatCntP,Chain,Cn1,Cn2);
  }

  FillAsnAntiPar(AntiPar1,AntiPar2,Chain,Cn1,Cn2,PatN,PatCntN,Cmd);
  FillAsnPar(Par1,Par2,Chain,Cn1,Cn2,PatP,PatCntP,Cmd);

  Bridge(AntiPar1,AntiPar2,Chain,Cn1,Cn2,PatN,PatCntN);
  Bridge(Par1,Par2,Chain,Cn1,Cn2,PatP,PatCntP);

  for( i=0; i<Chain[Cn1]->NRes; i++ )
    if( AntiPar1[i] == 'N' || Par1[i] == 'P' )
      Chain[Cn1]->Rsd[i]->Prop->Asn = 'E';
    else
    if( AntiPar1[i] == 'B' || Par1[i] == 'B' )
      Chain[Cn1]->Rsd[i]->Prop->Asn = 'B';
    else
    if( AntiPar1[i] == 'b' || Par1[i] == 'b' )
      Chain[Cn1]->Rsd[i]->Prop->Asn = 'b';

  for( i=0; i<Chain[Cn2]->NRes; i++ )
    if( Chain[Cn2]->Rsd[i]->Prop->Asn == 'E' )
      continue;
    else
    if( AntiPar2[i] == 'N' || Par2[i] == 'P' )
      Chain[Cn2]->Rsd[i]->Prop->Asn = 'E';
    else
    if( AntiPar2[i] == 'B' || Par2[i] == 'B' )
      Chain[Cn2]->Rsd[i]->Prop->Asn = 'B';
    else
    if( AntiPar2[i] == 'b' || Par2[i] == 'b' )
      Chain[Cn2]->Rsd[i]->Prop->Asn = 'b';


  for( i=0; i<PatCntN; i++ )
    free(PatN[i]);
  for( i=0; i<PatCntP; i++ )
    free(PatP[i]);

  free(PatN);
  free(PatP);
  free(AntiPar1);
  free(Par1);
  free(AntiPar2);
  free(Par2);

}

int svt_stride::Link(HBOND **HBond, CHAIN **Chain, int Cn1, int Cn2, RESIDUE *Res1_1,
	 RESIDUE *Res1_2, RESIDUE *Res2_2, RESIDUE *Res2_1, RESIDUE *CRes1,
	 RESIDUE *CRes2, float **PhiPsiMap, MPATTERN **Pattern, int *NumPat,
	 const char *Text, float Treshold, COMMAND *Cmd, int Test)
{

  int BondNumber1, BondNumber2, Flag = 0;
  static const char *Result[2] = {" NO \n"," YES \n"};
  float Prob1, Prob2, Conf, Coeff;

  if( (BondNumber1 = FindPolInt(HBond,Res1_1,Res1_2)) == ERR )
    return(FAILURE);

  if( (BondNumber2 = FindPolInt(HBond,Res2_2,Res2_1)) == ERR )
    return(FAILURE);

  if( CRes1 == NULL ) {
    if( CRes2->Prop->PhiZn == ERR || CRes2->Prop->PsiZn == ERR )
      return(FAILURE);
    Conf = PhiPsiMap[CRes2->Prop->PhiZn][CRes2->Prop->PsiZn];
  }
  else
  if( CRes2 == NULL ) {
    if( CRes1->Prop->PhiZn == ERR || CRes1->Prop->PsiZn == ERR )
      return(FAILURE);
    Conf = PhiPsiMap[CRes1->Prop->PhiZn][CRes1->Prop->PsiZn];
  }
  else {
    if( CRes2->Prop->PhiZn == ERR || CRes2->Prop->PsiZn == ERR ||
        CRes1->Prop->PhiZn == ERR || CRes1->Prop->PsiZn == ERR )
      return(FAILURE);
    Conf =
      0.5*(PhiPsiMap[CRes1->Prop->PhiZn][CRes1->Prop->PsiZn]+
	   PhiPsiMap[CRes2->Prop->PhiZn][CRes2->Prop->PsiZn]);
  }
  Coeff = 1+Cmd->C1_E+Cmd->C2_E*Conf;
  Prob1 = HBond[BondNumber1]->Energy*Coeff;
  Prob2 = HBond[BondNumber2]->Energy*Coeff;

  if( Prob1 < Treshold && Prob2 < Treshold ) {

    if( !Test ) {
      Pattern[*NumPat] = (MPATTERN *)ckalloc(sizeof(MPATTERN));
      Pattern[*NumPat]->ExistPattern = YES;
      Pattern[*NumPat]->Hb1 = HBond[BondNumber1];
      Pattern[*NumPat]->Hb2 = HBond[BondNumber2];
      Pattern[*NumPat]->Nei1 = NULL;
      Pattern[*NumPat]->Nei2 = NULL;
      strcpy(Pattern[*NumPat]->Type,Text);
      (*NumPat)++;
    }
    Flag = 1;
  }

  if( Cmd->Info && Flag ) {
    fprintf(stdout,"%s %c: %3s %c: %3s | %c: %3s %c: %3s | ",
	    Text,
	    Chain[Cn1]->Id,Res1_1->PDB_ResNumb,
	    Chain[Cn2]->Id,Res1_2->PDB_ResNumb,
	    Chain[Cn2]->Id,Res2_2->PDB_ResNumb,
	    Chain[Cn1]->Id,Res2_1->PDB_ResNumb);
    fprintf(stdout,"%8.6f %6.4f | ", Prob1,HBond[BondNumber1]->Energy);
    fprintf(stdout,"%8.6f %6.4f | ", Prob2,HBond[BondNumber2]->Energy);

    if( CRes1 != NULL &&
        CRes1->Prop->PhiZn != ERR && CRes1->Prop->PsiZn != ERR )
      fprintf(stdout,"%6.4f %2d %2d | ",
	      PhiPsiMap[CRes1->Prop->PhiZn][CRes1->Prop->PsiZn],
	      CRes1->Prop->PhiZn,CRes1->Prop->PsiZn);
    else
      fprintf(stdout,"000000 00 00 | ");

    if( CRes2 != NULL &&
        CRes2->Prop->PhiZn != ERR && CRes2->Prop->PsiZn != ERR )
      fprintf(stdout,"%6.4f %2d %2d | ",
	      PhiPsiMap[CRes2->Prop->PhiZn][CRes2->Prop->PsiZn],
	      CRes2->Prop->PhiZn,CRes2->Prop->PsiZn);
    else
      fprintf(stdout,"000000 00 00 | ");

    fprintf(stdout,"%s",Result[Flag]);
  }

  return(Flag);
}

void svt_stride::PrintPatterns(MPATTERN **Pat, int NPat, CHAIN **Chain, int Cn1, int Cn2)
{

  register int i;
  int D1, A1, D2, A2;


  for( i=0; i<NPat; i++ ) {
    if( !Pat[i]->ExistPattern ) continue;

    D1 = Pat[i]->Hb1->Dnr->D_Res;
    A1 = Pat[i]->Hb1->Acc->A_Res;
    D2 = Pat[i]->Hb2->Dnr->D_Res;
    A2 = Pat[i]->Hb2->Acc->A_Res;

    fprintf(stdout,"%3d %c %c ",
	    i,Pat[i]->Hb1->Dnr->Chain->Id,Pat[i]->Hb2->Dnr->Chain->Id);
    if( Pat[i]->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id )
      fprintf(stdout,"%3s(%3d) %3s(%3d) %3s(%3d) %3s(%3d)",
	      Chain[Cn1]->Rsd[D1]->PDB_ResNumb,D1,
	      Chain[Cn2]->Rsd[A1]->PDB_ResNumb,A1,
	      Chain[Cn2]->Rsd[D2]->PDB_ResNumb,D2,
	      Chain[Cn1]->Rsd[A2]->PDB_ResNumb,A2);
    else
      fprintf(stdout,"%3s(%3d) %3s(%3d) %3s(%3d) %3s(%3d)",
	      Chain[Cn2]->Rsd[D1]->PDB_ResNumb,D1,
	      Chain[Cn1]->Rsd[A1]->PDB_ResNumb,A1,
	      Chain[Cn1]->Rsd[D2]->PDB_ResNumb,D2,
	      Chain[Cn2]->Rsd[A2]->PDB_ResNumb,A2);

    if( Pat[i]->Nei1 != NULL ) {
      D1 = Pat[i]->Nei1->Hb1->Dnr->D_Res;
      A1 = Pat[i]->Nei1->Hb1->Acc->A_Res;
      D2 = Pat[i]->Nei1->Hb2->Dnr->D_Res;
      A2 = Pat[i]->Nei1->Hb2->Acc->A_Res;

      fprintf(stdout," N1 %c %c ",
	      Pat[i]->Nei1->Hb1->Dnr->Chain->Id,Pat[i]->Nei1->Hb2->Dnr->Chain->Id);
      if( Pat[i]->Nei1->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id )
	fprintf(stdout,"%3s(%3d) %3s(%3d) %3s(%3d) %3s(%3d) ",
		Chain[Cn1]->Rsd[D1]->PDB_ResNumb,D1,
		Chain[Cn2]->Rsd[A1]->PDB_ResNumb,A1,
		Chain[Cn2]->Rsd[D2]->PDB_ResNumb,D2,
		Chain[Cn1]->Rsd[A2]->PDB_ResNumb,A2);
      else
	fprintf(stdout,"%3s(%3d) %3s(%3d) %3s(%3d) %3s(%3d) ",
		Chain[Cn2]->Rsd[D1]->PDB_ResNumb,D1,
		Chain[Cn1]->Rsd[A1]->PDB_ResNumb,A1,
		Chain[Cn1]->Rsd[D2]->PDB_ResNumb,D2,
		Chain[Cn2]->Rsd[A2]->PDB_ResNumb,A2);
    }

    if( Pat[i]->Nei2 != NULL ) {
      D1 = Pat[i]->Nei2->Hb1->Dnr->D_Res;
      A1 = Pat[i]->Nei2->Hb1->Acc->A_Res;
      D2 = Pat[i]->Nei2->Hb2->Dnr->D_Res;
      A2 = Pat[i]->Nei2->Hb2->Acc->A_Res;
      fprintf(stdout," N2 %c %c ",
	      Pat[i]->Nei2->Hb1->Dnr->Chain->Id,Pat[i]->Nei2->Hb2->Dnr->Chain->Id);
      if( Pat[i]->Nei2->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id )
	fprintf(stdout,"%3s(%3d) %3s(%3d) %3s(%3d) %3s(%3d) ",
		Chain[Cn1]->Rsd[D1]->PDB_ResNumb,D1,
		Chain[Cn2]->Rsd[A1]->PDB_ResNumb,A1,
		Chain[Cn2]->Rsd[D2]->PDB_ResNumb,D2,
		Chain[Cn1]->Rsd[A2]->PDB_ResNumb,A2);
      else
	fprintf(stdout,"%3s(%3d) %3s(%3d) %3s(%3d) %3s(%3d) ",
		Chain[Cn2]->Rsd[D1]->PDB_ResNumb,D1,
		Chain[Cn1]->Rsd[A1]->PDB_ResNumb,A1,
		Chain[Cn1]->Rsd[D2]->PDB_ResNumb,D2,
		Chain[Cn2]->Rsd[A2]->PDB_ResNumb,A2);
    }
    fprintf(stdout,"\n");
  }

}


void svt_stride::Bridge(char *Asn1, char *Asn2, CHAIN **Chain, int Cn1, int Cn2, MPATTERN **Pat, int NPat)
{

  register int i;
  int B_Res;

  for( i=0; i<NPat; i++ ) {
    if( Pat[i]->Nei1 != NULL || Pat[i]->Nei2 != NULL ) continue;

    if( !strcmp(Pat[i]->Type,"1331") &&
       ( Cn1 != Cn2 || abs(Pat[i]->Hb1->Dnr->D_Res-Pat[i]->Hb1->Acc->A_Res) >= 3 ) ) {

      if( Pat[i]->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id ) {
	if( Asn1[Pat[i]->Hb1->Dnr->D_Res] == 'C' )
	  Asn1[Pat[i]->Hb1->Dnr->D_Res] = 'B';
	if( Asn2[Pat[i]->Hb1->Acc->A_Res] == 'C' )
	  Asn2[Pat[i]->Hb1->Acc->A_Res] = 'B';
      }
      else {
	if( Asn2[Pat[i]->Hb1->Dnr->D_Res] == 'C' )
	  Asn2[Pat[i]->Hb1->Dnr->D_Res] = 'B';
	if( Asn1[Pat[i]->Hb1->Acc->A_Res] == 'C' )
	  Asn1[Pat[i]->Hb1->Acc->A_Res] = 'B';
      }

    }
    else
      if( !strcmp(Pat[i]->Type,"3124") &&
	 ( Cn1 != Cn2 ||
	  (abs(Pat[i]->Hb1->Dnr->D_Res-Pat[i]->Hb1->Acc->A_Res) >= 2 &&
	   abs(Pat[i]->Hb2->Dnr->D_Res-Pat[i]->Hb2->Acc->A_Res) >= 2 ) ) ) {

	if( Pat[i]->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id ) {

	  if( Pat[i]->Hb1->Dnr->D_Res > Pat[i]->Hb2->Acc->A_Res )
	    B_Res = Pat[i]->Hb1->Dnr->D_Res-1;
	  else
	    B_Res = Pat[i]->Hb1->Dnr->D_Res+1;

	  if( Asn1[B_Res] == 'C' )
	    Asn1[B_Res] = 'B';

	  if( Pat[i]->Hb2->Dnr->D_Res > Pat[i]->Hb1->Acc->A_Res )
	    B_Res = Pat[i]->Hb2->Dnr->D_Res-1;
	  else
	    B_Res = Pat[i]->Hb2->Dnr->D_Res+1;

	  if( Asn2[B_Res] == 'C' )
	    Asn2[B_Res] = 'B';
	}
	else {
	  if( Pat[i]->Hb1->Dnr->D_Res > Pat[i]->Hb2->Acc->A_Res )
	    B_Res = Pat[i]->Hb1->Dnr->D_Res-1;
	  else
	    B_Res = Pat[i]->Hb1->Dnr->D_Res+1;

	  if( Asn2[B_Res] == 'C' )
	    Asn2[B_Res] = 'B';

	  if( Pat[i]->Hb2->Dnr->D_Res > Pat[i]->Hb1->Acc->A_Res )
	    B_Res = Pat[i]->Hb2->Dnr->D_Res-1;
	  else
	    B_Res = Pat[i]->Hb2->Dnr->D_Res+1;

	  if( Asn1[B_Res] == 'C' )
	    Asn1[B_Res] = 'B';
	}
      }
      else
	if( ( ( !strcmp(Pat[i]->Type,"3123") || !strcmp(Pat[i]->Type,"1341") ) &&
	     ( Cn1 != Cn2 ||
	      (abs(Pat[i]->Hb1->Dnr->D_Res-Pat[i]->Hb1->Acc->A_Res) > 3 &&
	       abs(Pat[i]->Hb2->Dnr->D_Res-Pat[i]->Hb2->Acc->A_Res) > 3 ) ) ) ) {

	  if( Pat[i]->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id ) {

	    if( Pat[i]->Hb1->Dnr->D_Res == Pat[i]->Hb2->Acc->A_Res ) {

	      if( Asn1[Pat[i]->Hb1->Dnr->D_Res] == 'C' )
		Asn1[Pat[i]->Hb1->Dnr->D_Res] = 'B';

	      if( Pat[i]->Hb2->Dnr->D_Res > Pat[i]->Hb1->Acc->A_Res )
		B_Res = Pat[i]->Hb2->Dnr->D_Res-1;
	      else
		B_Res = Pat[i]->Hb2->Dnr->D_Res+1;

	      if( Asn2[B_Res] == 'C' )
		Asn2[B_Res] = 'B';
	    }
	    else {
	      if( Pat[i]->Hb2->Dnr->D_Res == Pat[i]->Hb1->Acc->A_Res )

		if( Asn2[Pat[i]->Hb2->Dnr->D_Res] == 'C' )
		  Asn2[Pat[i]->Hb2->Dnr->D_Res] = 'B';

	      if( Pat[i]->Hb1->Dnr->D_Res > Pat[i]->Hb2->Acc->A_Res )
		B_Res = Pat[i]->Hb1->Dnr->D_Res-1;
	      else
		B_Res = Pat[i]->Hb1->Dnr->D_Res+1;

	      if( Asn1[B_Res] == 'C' )
		Asn1[B_Res] = 'B';
	    }
	  }
	}
	else
	  if( ( !strcmp(Pat[i]->Type,"13B1") || !strcmp(Pat[i]->Type,"133A") ) &&
	     ( Cn1 != Cn2 ||
	      (abs(Pat[i]->Hb1->Dnr->D_Res-Pat[i]->Hb1->Acc->A_Res) > 4 &&
	       abs(Pat[i]->Hb2->Dnr->D_Res-Pat[i]->Hb2->Acc->A_Res) > 4 ) ) ) {

	    if( Pat[i]->Hb1->Dnr->Chain->Id == Chain[Cn1]->Id ) {

	      if( Pat[i]->Hb1->Dnr->D_Res == Pat[i]->Hb2->Acc->A_Res ) {

		if( Asn1[Pat[i]->Hb1->Dnr->D_Res] == 'C' )
		  Asn1[Pat[i]->Hb1->Dnr->D_Res] = 'B';

		if( Pat[i]->Hb2->Dnr->D_Res > Pat[i]->Hb1->Acc->A_Res )
		  B_Res = Pat[i]->Hb2->Dnr->D_Res-1;
		else
		  B_Res = Pat[i]->Hb2->Dnr->D_Res+1;

		if( Asn2[B_Res] == 'C' )
		  Asn2[B_Res] = 'B';
	      }
	      else {
		if( Pat[i]->Hb2->Dnr->D_Res == Pat[i]->Hb1->Acc->A_Res )

		  if( Asn2[Pat[i]->Hb2->Dnr->D_Res] == 'C' )
		    Asn2[Pat[i]->Hb2->Dnr->D_Res] = 'b';

		if( Pat[i]->Hb1->Dnr->D_Res > Pat[i]->Hb2->Acc->A_Res )
		  B_Res = Pat[i]->Hb1->Dnr->D_Res-1;
		else
		  B_Res = Pat[i]->Hb1->Dnr->D_Res+1;

		if( Asn1[B_Res] == 'C' )
		  Asn1[B_Res] = 'b';
	      }
	    }
	  }

  }
}

int svt_stride::SplitString(char *Buffer, char **Fields, int MaxField)
{
  int FieldCnt, SymbCnt, FieldFlag, BuffLen;
  static char LocalBuffer[BUFSZ];


  FieldCnt =0; FieldFlag = 0;
  BuffLen = (int)strlen(Buffer) - 1;

  strcpy(LocalBuffer,Buffer);

  for(SymbCnt=0; SymbCnt<BuffLen; SymbCnt++) {
    if( (isspace(LocalBuffer[SymbCnt])) && FieldFlag == 0 && SymbCnt != BuffLen-1 ) continue;
    if( (!isspace(LocalBuffer[SymbCnt])) && FieldFlag == 1 && SymbCnt == BuffLen-1 ) {
      LocalBuffer[SymbCnt+1] = '\0';
      return(FieldCnt);
    }
    else
    if( (isspace(LocalBuffer[SymbCnt])) && FieldFlag == 1 ) {
      LocalBuffer[SymbCnt] = '\0';
      FieldFlag = 0;
      if( FieldCnt == MaxField ) return(FieldCnt);
    }
    else
    if( (!isspace(LocalBuffer[SymbCnt])) && FieldFlag == 0 ) {
      FieldFlag = 1;
      Fields[FieldCnt] = LocalBuffer+SymbCnt;
      FieldCnt++;
    }
  }

     return(FieldCnt);
 }



int svt_stride::SSBond(CHAIN **Chain, int NChain)
{

  register int Res1, Res2, Cn1, Cn2;
  int S1, S2, Bn, Cnt=0;

  for( Cn1=0; Cn1<NChain; Cn1++ )
    for( Res1=0; Res1<Chain[Cn1]->NRes; Res1++ ) {
      if( strcmp(Chain[Cn1]->Rsd[Res1]->ResType,"CYS") )
	continue;
      for( Cn2=Cn1; Cn2<NChain; Cn2++ )
	for( Res2 = ( (Cn2 == Cn1)? Res1+1 : 0) ; Res2<Chain[Cn2]->NRes; Res2++ ) {
	  if( strcmp(Chain[Cn2]->Rsd[Res2]->ResType,"CYS") )
	    continue;

	  if( !ExistSSBond(Chain,NChain,Cn1,Cn2,
			   Chain[Cn1]->Rsd[Res1]->PDB_ResNumb,
			   Chain[Cn2]->Rsd[Res2]->PDB_ResNumb) &&
	     FindAtom(Chain[Cn1],Res1,"SG",&S1) && FindAtom(Chain[Cn2],Res2,"SG",&S2) &&
	     Dist(Chain[Cn1]->Rsd[Res1]->Coord[S1],
		  Chain[Cn2]->Rsd[Res2]->Coord[S2]) <= SSDIST ) {
	    Bn = Chain[0]->NBond;
	    Chain[0]->SSbond[Bn] =  (SSBOND *)ckalloc(sizeof(SSBOND));
	    strcpy(Chain[0]->SSbond[Bn]->PDB_ResNumb1,Chain[Cn1]->Rsd[Res1]->PDB_ResNumb);
	    strcpy(Chain[0]->SSbond[Bn]->PDB_ResNumb2,Chain[Cn2]->Rsd[Res2]->PDB_ResNumb);
	    Chain[0]->SSbond[Bn]->ChainId1 = Chain[Cn1]->Id;
	    Chain[0]->SSbond[Bn]->ChainId2 = Chain[Cn2]->Id;
	    Chain[0]->SSbond[Bn]->AsnSource = Stride;
	    Chain[0]->NBond++;
	    Cnt++;
	  }
	}
    }

  return(Cnt);
}

BOOLEAN svt_stride::ExistSSBond(CHAIN **Chain,int NChain, int Cn1,int Cn2,char *Res1,char *Res2)
{

  register int i;
  SSBOND *ptr;

  for( i=0; i<Chain[0]->NBond; i++ ) {
    ptr = Chain[0]->SSbond[i];
    if( ( !strcmp(Res1,ptr->PDB_ResNumb1) &&
	  !strcmp(Res2,ptr->PDB_ResNumb2) &&
	  FindChain(Chain,NChain,ptr->ChainId1) == Cn1 &&
	  FindChain(Chain,NChain,ptr->ChainId2) == Cn2 ) ||
        ( !strcmp(Res2,ptr->PDB_ResNumb1) &&
          !strcmp(Res1,ptr->PDB_ResNumb2) &&
	  FindChain(Chain,NChain,ptr->ChainId1) == Cn2 &&
	  FindChain(Chain,NChain,ptr->ChainId2) == Cn1 ) )
      return(SUCCESS);
  }

  return(FAILURE);
}


int svt_stride::Replace(char *String, char From, char To)
{

  int Replaced=0;

  if( From == '\0' )
    return(Replaced);

  for( ; *String != '\0'; String++ )
    if( *String == From ) {
      *String = To;
    Replaced++;
    }

  return(Replaced);
}

int svt_stride::Delete(char *String, char From)
{

  int Deleted = 0;
  char *c;

  if( From == '\0' )
    return(Deleted);

  for( ; *String != '\0'; String++ )
    if( *String == From ) {
      c = String;
      for( ;; c++ ) {
	*c = *(c+1);
	if( *c == '\0' )
	  break;
      }
      Deleted++;
      String--;
    }
  return(Deleted);
}

void svt_stride::BetaTurn(CHAIN **Chain, int Cn)
{

  register int i;
  RESIDUE **r;
  TURN *t;
  int CA1, CA4, Tn;
  float Phi2, Phi3, Psi2, Psi3, Range1 = 30.0, Range2 = 45.0;
  char TurnType;

  for( i=0; i<Chain[Cn]->NRes-4; i++ ) {

    r = &Chain[Cn]->Rsd[i];

    if( r[1]->Prop->Asn == 'H' || r[2]->Prop->Asn == 'H' ||
        r[1]->Prop->Asn == 'G' || r[2]->Prop->Asn == 'G' ||
        r[1]->Prop->Asn == 'I' || r[2]->Prop->Asn == 'G' ||
       !FindAtom(Chain[Cn],i,"CA",&CA1) || !FindAtom(Chain[Cn],i+3,"CA",&CA4) ||
       Dist(r[0]->Coord[CA1],r[3]->Coord[CA4]) > 7.0 )
      continue;

    Phi2 = r[1]->Prop->Phi;
    Psi2 = r[1]->Prop->Psi;
    Phi3 = r[2]->Prop->Phi;
    Psi3 = r[2]->Prop->Psi;

    if( TurnCondition(Phi2,-60.0,Psi2,-30,Phi3,-90.0,Psi3,0,Range1,Range2) )
      TurnType = '1';
    else
    if( TurnCondition(Phi2,60.0,Psi2,30,Phi3,90.0,Psi3,0,Range1,Range2) )
      TurnType = '2';
    else
    if( TurnCondition(Phi2,-60.0,Psi2,120,Phi3,80.0,Psi3,0,Range1,Range2) )
      TurnType = '3';
    else
    if( TurnCondition(Phi2,60.0,Psi2,-120,Phi3,-80.0,Psi3,0,Range1,Range2) )
      TurnType = '4';
    else
    if( TurnCondition(Phi2,-60.0,Psi2,120,Phi3,-90.0,Psi3,0,Range1,Range2) )
      TurnType = '5';
    else
    if( TurnCondition(Phi2,-120.0,Psi2,120,Phi3,-60.0,Psi3,0,Range1,Range2) )
      TurnType = '6';
    else
    if( TurnCondition(Phi2,-60.0,Psi2,-30,Phi3,-120.0,Psi3,120,Range1,Range2) )
      TurnType = '7';
    else
      TurnType = '8';

    if( r[0]->Prop->Asn == 'C' )
      r[0]->Prop->Asn = 'T';

    if( r[1]->Prop->Asn == 'C' )
      r[1]->Prop->Asn = 'T';

    if( r[2]->Prop->Asn == 'C' )
      r[2]->Prop->Asn = 'T';

    if( r[3]->Prop->Asn == 'C' )
      r[3]->Prop->Asn = 'T';

    Tn = Chain[Cn]->NAssignedTurn;
    if (Chain[Cn]->AssignedTurn[Tn])
    {
        Chain[Cn]->AssignedTurn[Tn] = (TURN *)ckalloc(sizeof(TURN));
        t = Chain[Cn]->AssignedTurn[Tn];
        strcpy(t->Res1,r[0]->ResType);
        strcpy(t->Res2,r[3]->ResType);
        strcpy(t->PDB_ResNumb1,r[0]->PDB_ResNumb);
        strcpy(t->PDB_ResNumb2,r[3]->PDB_ResNumb);
        t->TurnType = TurnType;
        Chain[Cn]->NAssignedTurn++;
    }

  }
}


void svt_stride::GammaTurn(CHAIN **Chain, int Cn, HBOND **HBond)
{

  register int i;
  RESIDUE **r;
  TURN *t;
  int Tn;
  float Phi2, Psi2;
  char TurnType, Asn;

  for( i=0; i<Chain[Cn]->NRes-2; i++ ) {

    r = &Chain[Cn]->Rsd[i-1];

    Asn = r[2]->Prop->Asn;

    if( Asn == 'H' || Asn == 'T' || Asn == 'G' || Asn == 'I' ||
        FindBnd(HBond,r[3],r[1]) == ERR ||
        (i > 0 && FindBnd(HBond,r[3],r[0]) != ERR) ||
        (i < Chain[Cn]->NRes-3 && FindBnd(HBond,r[4],r[1]) != ERR) )
      continue;

    Phi2 = r[2]->Prop->Phi;
    Psi2 = r[2]->Prop->Psi;

    if( Phi2 > 0.0 && Psi2 < 0.0 )
      TurnType = '@';
    else
    if( Phi2 < 0.0 && Psi2 > 0.0 )
      TurnType = '&';
    else
      continue;

    if( r[1]->Prop->Asn == 'C' )
      r[1]->Prop->Asn = 'T';

    if( r[2]->Prop->Asn == 'C' )
      r[2]->Prop->Asn = 'T';

    if( r[3]->Prop->Asn == 'C' )
      r[3]->Prop->Asn = 'T';

    Tn = Chain[Cn]->NAssignedTurn;
    Chain[Cn]->AssignedTurn[Tn] = (TURN *)ckalloc(sizeof(TURN));
    t = Chain[Cn]->AssignedTurn[Tn];
    strcpy(t->Res1,r[1]->ResType);
    strcpy(t->Res2,r[3]->ResType);
    strcpy(t->PDB_ResNumb1,r[1]->PDB_ResNumb);
    strcpy(t->PDB_ResNumb2,r[3]->PDB_ResNumb);
    t->TurnType = TurnType;
    Chain[Cn]->NAssignedTurn++;
  }
}


int svt_stride::TurnCondition(float Phi2,float Phi2S,float Psi2,float Psi2S,
		  float Phi3,float Phi3S,float Psi3,float Psi3S,
		  float Range1,float Range2)
{
  if((ST_IN(Phi2,Phi2S,Range2)==YES && ST_IN(Psi2,Psi2S,Range1)==YES &&
      ST_IN(Phi3,Phi3S,Range1)==YES && ST_IN(Psi3,Psi3S,Range1)==YES)
     ||
     (ST_IN(Phi2,Phi2S,Range1)==YES && ST_IN(Psi2,Psi2S,Range2)==YES &&
      ST_IN(Phi3,Phi3S,Range1)==YES && ST_IN(Psi3,Psi3S,Range1)==YES)
     ||
     (ST_IN(Phi2,Phi2S,Range1)==YES && ST_IN(Psi2,Psi2S,Range1)==YES &&
      ST_IN(Phi3,Phi3S,Range2)==YES && ST_IN(Psi3,Psi3S,Range1)==YES)
     ||
     (ST_IN(Phi2,Phi2S,Range1)==YES && ST_IN(Psi2,Psi2S,Range1)==YES &&
      ST_IN(Phi3,Phi3S,Range1)==YES && ST_IN(Psi3,Psi3S,Range2)==YES)
     )
    return(SUCCESS);

  return(FAILURE);
}

char *svt_stride::tolostr(char *InputString)
{
  register int i;
  int Length;
  static char OutputString[BUFSZ];

  strcpy(OutputString,InputString);

  Length = (int)strlen(OutputString);

  for( i=0; i<Length; i++ )
    OutputString[i] = tolower(OutputString[i]);
  return(OutputString);
}


char svt_stride::ThreeToOne(char *Three)
{
  if( !strcmp(Three,"ALA") ) return('A');
  else if( !strcmp(Three,"ARG") ) return('R');
  else if( !strcmp(Three,"ASN") ) return('N');
  else if( !strcmp(Three,"ASP") ) return('D');
  else if( !strcmp(Three,"ASX") ) return('B');
  else if( !strcmp(Three,"CYS") ) return('C');
  else if( !strcmp(Three,"GLN") ) return('Q');
  else if( !strcmp(Three,"GLU") ) return('E');
  else if( !strcmp(Three,"GLX") ) return('Z');
  else if( !strcmp(Three,"GLY") ) return('G');
  else if( !strcmp(Three,"HIS") ) return('H');
  else if( !strcmp(Three,"ILE") ) return('I');
  else if( !strcmp(Three,"LEU") ) return('L');
  else if( !strcmp(Three,"LYS") ) return('K');
  else if( !strcmp(Three,"MET") ) return('M');
  else if( !strcmp(Three,"PRO") ) return('P');
  else if( !strcmp(Three,"PHE") ) return('F');
  else if( !strcmp(Three,"SER") ) return('S');
  else if( !strcmp(Three,"THR") ) return('T');
  else if( !strcmp(Three,"TRP") ) return('W');
  else if( !strcmp(Three,"TYR") ) return('Y');
  else if( !strcmp(Three,"VAL") ) return('V');
  else    return('X');
}


/*************************************************************************
**                                                                      **
** Find sequential residue number for a PDB residue number              **
**                                                                      **
** INPUT:  *Chain    Pointer to a protein chain                         **
**         *PdbN     String containing PDB residue number               **
**                                                                      **
** OUTPUT: *SeqN     Pointer to the sequential residue number           **
**                                                                      **
*************************************************************************/
int svt_stride::PdbN2SeqN(CHAIN *Chain, char *PdbN, int *SeqN)
{

  for( (*SeqN)=0; (*SeqN)<Chain->NRes; (*SeqN)++ )
    if( !strcmp(Chain->Rsd[(*SeqN)]->PDB_ResNumb,PdbN) )
      return(SUCCESS);

  return(FAILURE);
}

/*************************************************************************
**                                                                      **
** Find atom of specified type in a residue                             **
**                                                                      **
** INPUT:  *Chain    Pointer to a protein chain                         **
**         ResNumb   Number of residue in the protein chain             **
**         *Atom     String containing atom name                        **
**                                                                      **
** OUTPUT: *AtNumb   Pointer to the atom number in the residue          **
**                                                                      **
*************************************************************************/
int svt_stride::FindAtom(CHAIN *Chain, int ResNumb, const char *Atom, int *AtNumb)
{

  for( (*AtNumb)=0; (*AtNumb)<Chain->Rsd[ResNumb]->NAtom; (*AtNumb)++ )
    if( !strcmp(Atom,Chain->Rsd[ResNumb]->AtomType[(*AtNumb)]) )
       return(SUCCESS);

  *AtNumb = ERR;
  return(FAILURE);
}

/*************************************************************************
**                                                                      **
** Find beginning and end residues of each secondary structure element  **
** in a secondary structure assignment                                  **
**                                                                      **
** INPUT:   *Asn       One letter secondary structure assignment        **
**          L          Length of the protein chain                      **
**          SecondStr  Secondary structure type                         **
**                                                                      **
** OUTPUT:  *(Bound)[2] Two dimensional array containing numbers of     **
**                      first and last residue of each secondary        **
**                      structure element                               **
**                                                                      **
** RETURNS: Number of the Secondary structure elements                  **
**                                                                      **
*************************************************************************/
int svt_stride::Boundaries(char *Asn, int L, char SecondStr, int (*Bound)[2])
{
  register int Res;
  int NStr = 0, Flag = 0;

  for( Res=0; Res<L; Res++ ) {
    if( Asn[Res] == SecondStr && Flag == 0 ) {
      Flag = 1;
      Bound[NStr][0] = Res;
    }
    else
    if( Asn[Res] != SecondStr && Flag == 1 ) {
      Flag = 0;
      Bound[NStr++][1] = Res-1;
    }
  }
  return(NStr);
}

/*************************************************************************
**                                                                      **
** Find protein chain with a given chain identifier number              **
**                                                                      **
** INPUT:  **Chain   Array of protein chains                            **
**         NChain    Number of chains                                   **
**         ChainId   Chain identifier                                   **
**                                                                      **
** RETURNS: Number of the protein chain with identifier ChainId         **
**                                                                      **
*************************************************************************/
int svt_stride::FindChain(CHAIN **Chain, int NChain, char ChainId)
{
  register int i;

  for( i=0; i<NChain; i++ )
    if( Chain[i]->Id == ChainId )
      return(i);

  return(ERR);
}

BOOLEAN svt_stride::IsHydrogen(char *AtomName)
{

  if( ( isdigit(AtomName[0]) &&
       ( AtomName[1] == 'H' || AtomName[1] == 'D' ||
	AtomName[1] == 'T' || AtomName[1] == 'Q' ) ) ||
     AtomName[0] == 'H' || AtomName[0] == 'D' ||
     AtomName[0] == 'T' || AtomName[0] == 'Q' )
    return(SUCCESS);
  else
    return(FAILURE);
}

const char *svt_stride::Translate(char Code)
{

  static const char  * Dictionary[18] = {
    "AlphaHelix","310Helix","PiHelix","Strand","Bridge","Coil","TurnI","TurnI'",
    "TurnII","TurnII'","TurnVIa","TurnVIb","TurnVIII","TurnIV","GammaClassic",
    "GammaInv","Turn","Unknown" };

  switch(Code) {
  case 'H': return(Dictionary[0]);
  case 'G': return(Dictionary[1]);
  case 'I': return(Dictionary[2]);
  case 'E': return(Dictionary[3]);
  case 'B':
  case 'b':
            return(Dictionary[4]);
  case 'C': return(Dictionary[5]);
  case '1': return(Dictionary[6]);
  case '2': return(Dictionary[7]);
  case '3': return(Dictionary[8]);
  case '4': return(Dictionary[9]);
  case '5': return(Dictionary[10]);
  case '6': return(Dictionary[11]);
  case '7': return(Dictionary[12]);
  case '8': return(Dictionary[13]);
  case '@': return(Dictionary[14]);
  case '&': return(Dictionary[15]);
  case 'T': return(Dictionary[16]);
  default:  return(Dictionary[17]);
  }

}

char svt_stride::SpaceToDash(char Id)
{
  static char NewId;

  if( Id == ' ' )
    NewId = '-';
  else
    NewId = Id;

  return(NewId);
}

BOOLEAN svt_stride::ChInStr(char *String, char Char)
{

  if( strchr(String,toupper(Char)) ||
      strchr(String,Char) ||
      strchr(String,tolower(Char)) )
    return(YES);

  return(NO);
}

void svt_stride::ExtractAsn(CHAIN **Chain, int Cn, char *Asn)
{
  register int Res;

  for( Res=0; Res<Chain[Cn]->NRes; Res++ )
    Asn[Res] = Chain[Cn]->Rsd[Res]->Prop->Asn;
}

void svt_stride::ExtractPdbAsn(CHAIN **Chain, int Cn, char *Asn)
{
  register int Res;

  for( Res=0; Res<Chain[Cn]->NRes; Res++ )
    Asn[Res] = Chain[Cn]->Rsd[Res]->Prop->PdbAsn;
}

void svt_stride::ExtractDsspAsn(CHAIN **Chain, int Cn, char *Asn)
{
  register int Res;

  for( Res=0; Res<Chain[Cn]->NRes; Res++ )
    Asn[Res] = Chain[Cn]->Rsd[Res]->Prop->DsspAsn;
}

BOOLEAN svt_stride::ExistsSecStr(CHAIN **Chain, int NChain)
{
  register int i, Cn;

  for( Cn=0; Cn<NChain; Cn++ )
    for( i=0; i<Chain[Cn]->NRes; i++ )
      if( Chain[Cn]->Rsd[i]->Prop->Asn != 'C' )
	return(YES);

  return(NO);
}


/*************************************************************************
**                                                                      **
** Calculate the number of residues in helical or beta sheet state and  **
** what percent they constitute from the total number of residues in a  **
** protein chain                                                        **
**                                                                      **
** INPUT:   *Chain     Pointer to a protein chain                       **
**                                                                      **
** OUTPUT:  *HelAlp    Number of alpha-helical residues                 **
**          *HelPI     Number of residues in pi-helices                 **
**          *Hel310    Number of residues in 3-10 helices               **
**          *Sheet     Number of residues in beta sheet                 **
**                                                                      **
** RETURNS: Secondary structure content                                 **
**                                                                      **
*************************************************************************/
float svt_stride::SecStrContent(CHAIN *Chain, int *HelAlp, int *HelPI, int *Hel310, int *Sheet, int *Turn)
{

  int Res;
  float Content = 0.0;

  *HelAlp = 0; *HelPI = 0; *Hel310 = 0; *Sheet = 0; *Turn = 0;

  for( Res=0; Res<Chain->NRes; Res++ ) {
    switch( Chain->Rsd[Res]->Prop->PdbAsn ) {
    case 'H' : (*HelAlp)++;
               break;
    case 'G' : (*Hel310)++;
               break;
    case 'I' : (*HelPI)++;
               break;
    case 'E' : (*Sheet)++;
               break;
    case 'T' : (*Turn)++;
               break;
    }
  }

  Content = ( (float)( (*HelAlp)+(*HelPI)+(*Hel310)+(*Sheet)+(*Turn) ) )/(float)Chain->NRes;

  return(Content);
}












































































