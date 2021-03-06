STRIDE: Protein secondary structure assignment from atomic coordinates

Dmitrij Frishman & Patrick Argos

                       European	Molecular Biology Laboratory
                          Postfach 102209, Meyerhofstr.	1
                                  69012	Heidelberg
                                      Germany

                            FRISHMAN@EMBL-HEIDELBERG.DE
                              ARGOS@EMBL-HEIDELBERG.DE

CONTENTS

   1. About the method
   2. Copyright notice
   3. Availability
   4. Installation
   5. Using STRIDE
   6. Output format
   7. Bug reports and user feedback
   8. References 

1. About the method
STRIDE [1] is a program to recognize secondary structural elements in proteins from their atomic coordinates. It performs the same task as DSSP by Kabsch and Sander [2] but utilizes both hydrogen bond energy and mainchain dihedral angles rather than hydrogen bonds alone. It relies on database-derived recognition parameters with the crystallographers' secondary structure definitions as a standard-of- truth. Please see Frishman and Argos [1] for detailed description of the algorithm.

2. Copyright notice

All rights reserved, whether the whole or part of the program is concerned. Permission to use, copy, and modify this software and its documentation is granted for academic use, provided that:

i. this copyright notice appears in all copies of the software and related documentation;

ii. the reference given below (Frishman and Argos, 1995) must be cited in any publication of scientific results based in part or completely on the use of the program;

iii. bugs will be reported to the authors.

The use of the software in commercial activities is not allowed without a prior written commercial license agreement.

WARNING: STRIDE is provided "as-is" and without warranty of any kind, express, implied or otherwise, including without limitation any warranty of merchantability or fitness for a particular purpose. In no event will the authors be liable for any special, incidental, indirect or consequential damages of any kind, or any damages whatsoever resulting from loss of data or profits, whether or not advised of the possibility of damage, and on any theory of liability, arising out of or in connection with the use or performance of this software.

For calculation of the residue solvent accessible area the program NSC [3,4] is used and was kindly provided by Dr. F.Eisenhaber (EISENHABER@EMBL-HEIDELBERG.DE). Please direct to him all questions concerning specifically accessibility calculations.

3. Availability

Executables of STRIDE for several UNIX platforms, VAX/VMS, OpenVMS, Dos and Mac together with documentation and source code are available by anonymous FTP from ftp.ebi.ac.uk (directories /pub/software/unix/stride, /pub/software/dos/stride, /pub/software/vms/stride, /pub/software/mac/stride). We are willing to compile the program for other architectures if temporary access to them will be granted by an interested user.

Data files with STRIDE secondary structure assignments for the current release of the PDB [5] databank are in the directory /pub/databases/stride of the same site. Atomic coordinate sets can be submitted for secondary structure assignment through electronic mail to stride@embl-heildelberg.de. A mail message containing HELP in the first line will be answered with appropriate instructions. See also WWW page http://www.embl-heidelberg.de/stride/stride_info.html.

4. Installation

For UNIX, DOS and Mac no installation is needed. Just download the executable corresponding to your platform, and you are all set. For VAX and OpenVMS you need only to link the executable with a logical name; for example:

yourlogicalname:= $ $yourdiskname:[your.directory.name]stride.exe

and then use yourlogicalname as the program name.

5. Using STRIDE

The only required parameter for STRIDE is the name of the file containing a set of atomic coordinates in PDB [5] format. By default STRIDE writes to standard output, i.e. your screen. On systems that allow to redirect output you can do so to create a disk file. Help is available if you just type STRIDE without parameters. The following options are accepted:

-fFilename Write output to the file "Filename" rather than to stdout.

-h Report hydrogen bonds. By default no hydrogen bond information is included in the output.

-o Report secondary structure summary only.

-rId1Id2.. Read only chains Id1, Id2 etc. of the PDB file *). All other chains will be ignored. By default all valid protein chains are read.

-cId1Id2.. Process only chains Id1, Id2 ...etc *). Secondary structure assignment will be produced only for these chains, but other chains that are present will be taken into account while calculating residue accessible surface and detecting inter-chain hydrogen bonds and, possibly, interchain beta-sheets. By default all protein chains read are processed.

-mFilename Generate a Molscript [6] file. Using the program Molscript by Per Craulis you can create a postscript picture of your structure. You can manually edit the Molscript file produced by STRIDE to achieve the desired orientation and to include additional details.

-q[Filename] Generate sequence file in FASTA [7] format and die. Filename is optional. If no file name is specified, stdandard output is used.

All options are case- and position-insensitive.

Examples:

1. Calculate secondary structure assignment for 1ACP including hydrogen bond information:

stride 1acp.brk -h

2. Calculate secondary structure assignment for 4RUB and write the output to the file 4rub.str

stride 4rub.brk -f4rub.str

3. Calculate secondary structure assignment for chain B of 4RUB. Ignore all other chains. Generate a Molscript file 4rub.mol.

stride 4rub.brk -rb -m4rub.mol

4. Calculate secondary structure assignment for chain C of 2GLS in the presence of chains A and B. Report secondary structure summary only.

stride 2gls.brk -rabc -cc -o

6. Output format

STRIDE produces output that is easily readable both visually and with computer programs. The side effect of this conveniency is larger file size of individual STRIDE entries. Every record is 79 symbols long and has the following general format:

Position	Description

1-3	Record code
4-5	Not used
6-73	Data
74-75	Not used
75-79	Four letter PDB code (if	available)

Below follows the description of each record type.

Code Description and format of data

REM Remarks and blank lines

Format: free

HDR Header. Protein name, date of file creation and PDB code

Format: free

CMP Compound.Full name of the molecule and identifying information

Format: free

SRC Species, organ, tissue, and mutant from which the molecule has been obtained

Format: free

AUT Names of the structure authors

Format: free

CHN File name and PDB chain identifier*).

Format: File name beginning from position 6 followed by one space and one-letter chain identifier

SEQ Amino acid sequence

Format:  6-9  First residue PDB number
 11-60 Sequence
 62-65 Last residue PDB number

STR Secondary structure summary Format: 11-60 Secondary structure assignment **)

LOC    Location of secondary structure elements

Format:  6-17 Element name
 19-21 First residue name
 32-26 First residue PDB number
 28-28 First residue chain	identifier
 36-38 Last residue name
 42-45 Last residue PDB number
 47-47 Last residue chain identifier

ASG    Detailed secondary structure assignment

Format:  6-8  Residue name
 10-10 Protein chain identifier
 12-15 PDB	residue	number
 17-20 Ordinal residue number
 25-25 One	letter secondary structure code	**)
 27-39 Full secondary structure name
 43-49 Phi	angle
 53-59 Psi	angle
 65-69 Residue solvent accessible area

DNR    Donor residue

Format:  6-8  Donor residue name
 10-10 Protein chain identifier
 12-15 PDB	residue	number
 17-20 Ordinal residue number
 26-28 Acceptor residue name
 30-30 Protein chain identifier
 32-35 PDB	residue	number
 37-40 Ordinal residue number
 42-45 N..0 distance
 47-52 N..O=C angle
 54-59 O..N-C angle
 61-66 Angle between the planes of donor
complex and	O..N-C
 68-73 angle between the planes of acceptor
complex and	N..O=C
ACC    Acceptor residue

Format:  6-8  Acceptor residue name
 10-10 Protein chain identifier
 12-15 PDB	residue	number
 17-20 Ordinal residue number
 26-28 Donor residue name
 30-30 Protein chain identifier
 32-35 PDB	residue	number
 37-40 Ordinal residue number
 42-45 N..0 distance
 47-52 N..O=C angle
 54-59 O..N-C angle
 61-66 Angle between the planes of donor
complex and	O..N-C
 68-73 angle between the planes of acceptor
complex and	N..O=C

HDR, CMP, SCR and AUT records are directly copied from the PDB file, if supplied by the authors. If only the secondary structure summary is requested, only CHN, SEQ, STR and LOC records will be output. Hydrogen bond information (records DNR and ACC) was made very redundant to facilitate human reading and will not be reported by default.

*) IMPORTANT NOTE: if the protein chain identifier is ' ' (space), it will be substituted by '-' (dash) everywhere in the STRIDE output. The same is true for command line parameters involving chain identifiers where you have to specify '-' instead of ' '.

**) One-letter secondary structure code is nearly the same as used in DSSP [2] (see Frishman and Argos [1] for details):

    H	    Alpha helix
    G	    3-10 helix
    I	    PI-helix
    E	    Extended conformation
    B or	b   Isolated bridge
    T	    Turn
    C	    Coil (none of the above)

For each record (data line) except those with codes REM and STR the number of fields is consistent and is readily suitable for processing with external tools, such as awk, perl, etc.

7. Bug reports and user feedback

Please send your suggestions, questions and bug reports to FRISHMAN@EMBL-HEIDELBERG.DE. Send your contact address to get information on updates and new features.

8. References

   1. Frishman,D & Argos,P. (1995) Knowledge-based secondary structure assignment. Proteins: structure, function and genetics, in the press.
   2. Kabsch,W. & Sander,C. (1983) Dictionary of protein secondary structure: pattern recognition of hydrogen-bonded and geometrical features. Biopolymers, 22: 2577-2637.
   3. Eisenhaber, F. and Argos, P. (1993) Improved strategy in analytic surface calculation for molecular systems: handling of singularities and computational efficiency. J. comput. Chem. 14, 1272-1280.
   4. Eisenhaber, F., Lijnzaad, P., Argos, P., Sander, C., and Scharf,
   5. (1995) The double cubic lattice method: efficient approaches to numerical integration of surface area and volume and to dot surface contouring of molecular assemblies. J. comput. Chem. 16, 273-284.
   6. Bernstein, F.C., Koetzle, T.F., Williams, G.J., Meyer, E.F., Brice, M.D., Rodgers, J.R., Kennard, O., Shimanouchi, T., and Tasumi, M. (1977) The protein data bank: a computer-based archival file for macromolecular structures. J. Mol. Biol. 112, 535-542.
   7. Kraulis, P.J. (1991) MOLSCRIPT: a program to produce both detailed and schematic plots of protein structures. J. Appl. Cryst. 24, 946-950.
   8. Pearson, W.R. (1990) Rapid and sensitive sequence comparison with FASTP and FASTA. Methods. Enzymol. 183, 63-98. 

