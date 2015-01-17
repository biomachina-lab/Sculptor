/***************************************************************************
                          svt_powell
			  ----------
    begin                : 07/22/2005
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_powell.h>
#include <svt_rnd.h>
#include <svt_cmath.h>
#include <svt_iostream.h>
#include <svt_messages.h>
#include <svt_exception.h>
#include <svt_time.h>

svt_powell* g_pPowell = NULL;
unsigned long int g_iTime = 0.0;
unsigned int g_iCount = 0;
vector<double> g_aVector(6);

#define POWL cout << svt_trimName("svt_powell")

#define EPSILON 1.0E-6

/**
 * conjugate gradient method, called with external score function and output file
 * \param pcode return code (0=OK, 1=reached maximum number of iterations)
 * \param pval_min initial function value
 * \param vect initial vector
 * \param len number of dimensions of the problem (length of vector)
 * \param pfunction pointer to criterion/function
 * \param max_iter maximum number of iterations
 * \param pow_outfile output file
 * \param tolerance tolerance of change, if change is below then algo assumes convergence
 * \param orientation initial orientation of search
 */
void powell(int *pcode, double *pval_min, double *vect, int len, double (*pfunction)(double *vect), unsigned max_iter, FILE *pow_outfile, double tolerance, double *init_directions );

/**
 * C function hook for scoring function
 */
double pPowFunc(double* pVec)
{
    if (g_pPowell != NULL)
    {
	unsigned int iLen = g_pPowell->getDimensions();
        vector<double> aVector;

	for(unsigned int i=0; i<iLen; i++)
	    aVector.push_back( pVec[i] );

        unsigned long iTime = svt_getToD();
        Real64 fScore = g_pPowell->score( aVector );
        g_iTime += (svt_getToD() - iTime);
        g_iCount++;

	return fScore;
    } else
        return 0.0;
}

/**
 * Constructor
 */
svt_powell::svt_powell():
 m_fTolerance (1.0E-15),
 m_pFile (NULL),
 m_iMaxIter (100),
 m_iMaxIter2Bracket(20), 
 m_iMaxIter2Minimize(50),
 m_iLine(0)
{   
};

/**
 * Destructor  
 */
svt_powell::~svt_powell()
{
};

/**
 * set start vector (call before maximize())
 * \param aVector stl-vector object, size determines the dimsionality of the search
 */
void svt_powell::setVector(vector<Real64> aVector)
{
    m_aVector = aVector;
};

/**
 * set vector
 * \param pVector Real64* object
 */
void svt_powell::setVector(Real64* pVector)
{
    for (unsigned int iIndex=0; iIndex<m_aVector.size(); iIndex++)
	m_aVector[iIndex] = pVector[iIndex];
};



/**
 * return current vector, call after maximize.
 * \return pointer
 */
vector<Real64> svt_powell::getVector()
{
    return m_aVector;
};

/**
 * set initial search direction
 * \param aDirection stl-vector object
 */
void svt_powell::setDirection(vector<Real64> aDirection)
{
    m_aDirection = aDirection;
};

/**
 * get dimensionality of the search
 */
unsigned int svt_powell::getDimensions()
{
    return m_aVector.size();
};

/**
 * set maximum number of iteration
 * \param iMaxIter maximum number of iterations
 */
void svt_powell::setMaxIter(unsigned int iMaxIter)
{
    m_iMaxIter = iMaxIter;
};

/**
 * set maximum number of iteration in the bracket search
 * \param iMaxIter2Bracket maximum number of iterations
 */
void svt_powell::setMaxIter2Bracket(unsigned int iMaxIter2Bracket)
{
    m_iMaxIter2Bracket = iMaxIter2Bracket;
};

/**
 * set maximum number of iteration in the minimization - once the bracket was identified
 * \param iMaxIter2Minimize maximum number of iterations
 */
void svt_powell::setMaxIter2Minimize(unsigned int iMaxIter2Minimize)
{
    m_iMaxIter2Minimize = iMaxIter2Minimize;
};

/**
 * get maximum number of iteration in the bracket search
 * \return iMaxIter2Bracket maximum number of iterations
 */
unsigned int svt_powell::getMaxIter2Bracket()
{
    return m_iMaxIter2Bracket;
};

/**
 * get maximum number of iteration in the minimization - once the bracket was identified
 * \param iMaxIter2Minimize maximum number of iterations
 */
unsigned int svt_powell::getMaxIter2Minimize()
{
    return m_iMaxIter2Minimize;
};

/**
 * set tolerance, if change is smaller the algorithm stops
 * \param fTolerance tolerance
 */
void svt_powell::setTolerance(Real64 fTolerance)
{
    m_fTolerance = fTolerance;
};

/**
 * set output file
 * \param pFile pointer FILE
 */
void svt_powell::setOutputFile(FILE* pFile)
{
    m_pFile = pFile;
};

/**
 * set current line 
 * \param iLine the line number
 */
void svt_powell::setLine(int iLine)
{
    m_iLine = iLine;
};

/**
 * Scoring function, please overload!
 */
Real64 svt_powell::score(vector<Real64>&)
{
  return 0;
}

/**
 * Outputs some sort of results; please overload
 */
void svt_powell::outputResult()
{
};

/**
 * Return the last time of the score calculation
 * \return the time
 */
Real64 svt_powell::getScoreTime()
{
    return ((Real64)(g_iTime) / (Real64)(g_iCount));
};

/**
 * Return the number of times the score was computed
 **/
unsigned int svt_powell::getScoreCount()
{
    return g_iCount;
};


/**
 * maximize the scoring function
 * \return true if it has converged, false if it did not reach tolerance
 */
bool svt_powell::maximize()
{
    unsigned int i;

    g_pPowell = this;
    g_iCount = 0;
    g_iTime = 0;

    // run local c routine with pPowFunc as scoring function

    // return code (0=OK, 1=reached maximum number of iterations)
    int iCode;
    // initial function value
    Real64 fValMin = score( m_aVector );
    // initial vector, copy C++ vector into C array
    Real64* pVect = new Real64[m_aVector.size()];
    for(i=0; i<m_aVector.size(); i++)
        pVect[i] = m_aVector[i];
    // initial directions vector, copy C++ vector into C array
    Real64* pDir = new Real64[m_aDirection.size()];
    for(i=0; i<m_aDirection.size(); i++)
        pDir[i] = m_aDirection[i];
    // number of dimensions of the problem (length of vector)
    int iLen = m_aVector.size();

    powell(&iCode, &fValMin, pVect, iLen, pPowFunc, m_iMaxIter, m_pFile, m_fTolerance, pDir);

    // copy resulting vector back
    for(i=0; i<m_aVector.size(); i++)
	m_aVector[i] = pVect[i];

    delete[] pVect;
    delete[] pDir;

    if (iCode == 0)
	return true;
    else
        return false;
};

///////////////////////////////////////////////////////////////////////////////
// C Implementation of Powell, adapted from WNLib
///////////////////////////////////////////////////////////////////////////////

#define GOLDEN_RATIO          1.618034
#define GOLDEN_SECTION        0.3819660
#define MAX_MAGNIFICATION     1.0
#define SWAPPING(_a,_b,_type) \
{\
  _type _tmp;\
  \
  _tmp = (_a);\
  (_a) = (_b);\
  (_b) = _tmp;\
}

int g_save_len;
double *g_buffer_vect, *g_save_vect, *g_save_direction;
double (*g_save_pfunction)(double vect[]);

void zero_vect(double *vect, unsigned long len)
{
    unsigned long i;
    for(i=0;i<len;++i)
	vect[i] = 0.0;
}

void do_vect(double **vect, unsigned long len)
{
    *vect = new double[len];
    if (*vect == NULL)
    {
	fprintf(stderr, "lib_vec> Error: Unable to satisfy memory allocation request [e.c. 18010]\n");
	exit(18010);
    }
    zero_vect(*vect,len);
}

void zero_mat(double **mat,unsigned long len_i,unsigned long len_j)
{
    unsigned long i;
    for(i=0;i<len_i;++i)
	zero_vect(mat[i],len_j);
}

void do_mat(double ***pmat,unsigned long len_i,unsigned long len_j)
{
    unsigned long i;
    *pmat = new double*[len_i];
    if (*pmat == NULL)
    {
	fprintf(stderr, "lib_vec> Error: Unable to satisfy memory allocation request [e.c. 18020]\n");
	exit(18020);
    }
    for(i=0;i<len_i;i++)
	do_vect(&((*pmat)[i]),len_j);
}

void cp_vect(double **vect1,double **vect2,unsigned long len)
{
    memcpy(*vect1,*vect2, len*sizeof(double));
}

void add_scaled_vect(double *to_vect, double *from_vect, double scalar, unsigned long len)
{
    unsigned long i;
    for(i=0;i<len;++i)
	to_vect[i] += scalar * from_vect[i];
}

/**
 * Generate a random bit value
 * \return 1 or 0 - randomly
 */
int random_bit()
{
    if (svt_genrand()>0.5)
	return 1;
    else
	return 0;
}

/**
 * Square function
 */
double square(double x)
{
  return(x*x);
}

/**
 * Fit parabola to 3 points (x,y) y = a*(x-x0)^2 + b;
 * Return a, x0, b.
 */
void fit_parabola_3p(int *pcode, double *pa,double *px0,double *pb,
		     double x1,double y1,double x2,double y2,
		     double x3,double y3 )
{

    double x12,x23,dy12,dy23,ddy,diff;

    // sort the x's
    if(!(x1 < x3)){
	SWAPPING(x1,x3,double);
	SWAPPING(y1,y3,double);}
    if(x2 < x1) {
	SWAPPING(x1,x2,double);
	SWAPPING(y1,y2,double);}
    else if(x3 < x2) {
	SWAPPING(x2,x3,double);
	SWAPPING(y2,y3,double); }

    if((x1 == x2)||(x2 == x3)||(x1 == x3)){
	*pcode = (-1);
	return;
    }

    x12 = 0.5*(x1+x2);
    x23 = 0.5*(x2+x3);

    dy12 = (y2-y1)/(x2-x1);
    dy23 = (y3-y2)/(x3-x2);

    ddy = dy23-dy12;
    *pa = 0.5*ddy/(x23-x12);

    if(ddy == 0.0) {
	*px0 = 0.0;
	*pb = y2; }
    else {
	*px0 = (dy23*x12-dy12*x23)/ddy;
	diff = x2-(*px0);
	*pb = y2-(*pa)*diff*diff; }

    *pcode = 0;
}

/**
 * Test if a number is "good"
 * \param fX number
 * \return true if fX < 1.0e+50
 */
int number_good(double fX)
{
  return (fX < 1.0e+50);
}

/**
 * Compare two numbers
 * \param fX1 first number
 * \param fX2 second number
 */
int numcmp(double x1,double x2)
{
    double fDiff = fabs(x1-x2);

    if(x1 < x2)
    {
	return(-1);
    } else if(x1 > x2)
    {
	return(1);
    } else if (fDiff < EPSILON)
    {
	return(0);
    }
    else
    {
	int good1,good2;
	good1 = number_good(x1);
	good2 = number_good(x2);

	if(good1&&(!good2))
	    return(-1);
	else if((!good1)&&good2)
	    return(1);
	else return(0);
    }
}

void insert_new_bracket_point (double *pf0, double *pf1, double *pf2,
			       double *px0, double *px1, double *px2,
			       double xnew, double fnew)
{

    if(xnew < *px0)
    {
	/* small bracket found */
	if(numcmp(*pf0,*pf1) < 0)
	{
	    /* make it as small as possible */
	    *px2 = *px1;  *pf2 = *pf1;
	    *px1 = *px0;  *pf1 = *pf0;
	    *px0 = xnew;  *pf0 = fnew;
	} else {
	    /* make range as big as possible */
	    *px0 = xnew;  *pf0 = fnew;
	}
    }
    else if(xnew > *px2)
    {
	/* small bracket found */
	if(numcmp(*pf2,*pf1) < 0)
	{
	    /* make it as small as possible */
	    *px0 = *px1;  *pf0 = *pf1;
	    *px1 = *px2;  *pf1 = *pf2;
	    *px2 = xnew;  *pf2 = fnew;
	} else {
	    /* make range as big as possible */
	    *px2 = xnew;  *pf2 = fnew;
	}
    }
}

void expand_width(double *pwidth, double f0,double f1,double f2,
		  double x0,double x1,double x2)
{
    int code;
    double a,cen,b,diff;

    (*pwidth) *= GOLDEN_RATIO;

    fit_parabola_3p(&code,&a,&cen,&b,x0,f0,x1,f1,x2,f2);

    if((code != 0)||(a <= 0))
	(*pwidth) *= MAX_MAGNIFICATION/GOLDEN_RATIO;
    else
    {
	diff = cen-x1;
	diff = fabs(diff);
	diff *= 2;

        if(diff > *pwidth)
        {
            if(diff > (*pwidth)*MAX_MAGNIFICATION/GOLDEN_RATIO)
            {
                (*pwidth) *= MAX_MAGNIFICATION/GOLDEN_RATIO;

            } else {

                (*pwidth) = diff;
            }
        }
    }
}

int xnew_is_new(double xnew,double x0,double x1,double x2)
{
    return ((xnew > x0)&&(xnew != x1)&&(xnew < x2));
}

void golden_section_probe(int *psuccess, double *pxnew, double x0, double x1, double x2)
{
    double diff01,diff12;

    diff01 = x1 - x0;
    diff12 = x2 - x1;

    if(diff01 > diff12)
	*pxnew = x1 - GOLDEN_SECTION*diff01;
    else
	*pxnew = x1 + GOLDEN_SECTION*diff12;

    *psuccess = xnew_is_new(*pxnew,x0,x1,x2);
}

void insert_new_trap_point (double *pf0, double *pf1, double *pf2, double *px0, double *px1, double *px2, double xnew, double fnew)
{
    if(xnew < *px1)
    {
	if(numcmp(fnew,*pf1) < 0)
	{
	    *px2 = *px1;  *pf2 = *pf1;
	    *px1 = xnew;  *pf1 = fnew;

	} else {

	    *px0 = xnew; *pf0 = fnew;
	}
    }
    else if(xnew > *px1)
    {
	if(numcmp(fnew,*pf1) < 0)
	{
	    *px0 = *px1;  *pf0 = *pf1;
	    *px1 = xnew;  *pf1 = fnew;

	} else {

	    *px2 = xnew; *pf2 = fnew;
	}
    }
}

/*
 * find the minimum in a parabola like situation
 */
void trap_minimum(double *pf0, double *pf1, double *pf2, double *px0, double *px1, double *px2, double f_goal, double (*pfunction)(double x), int max_iterations)
{
    // assume *pf0,*pf1,*pf2 already computed
  
    double xnew,fnew;
    int success;
    int improvement_count;

    improvement_count = 0;

    if ((numcmp(*pf0,f_goal) <= 0) || (numcmp(*pf2,f_goal) <= 0))
	++improvement_count;

    while((numcmp(*pf0,*pf1) > 0)||(numcmp(*pf2,*pf1) > 0))
    {
        if(improvement_count >= max_iterations)
            break;

	golden_section_probe(&success,&xnew,*px0,*px1,*px2);

        if(!success)
            break;

	fnew = (*pfunction)(xnew);

	insert_new_trap_point(pf0,pf1,pf2,px0,px1,px2,xnew,fnew);

	if((numcmp(*pf1,f_goal) <= 0) && (numcmp(fnew,f_goal) <= 0) )
            ++improvement_count;

    }
}

void bracket_minimum(int *pcode, double *pf0, double *pf1, double *pf2, double *px0, double *px1, double *px2, double (*pfunction)(double x))
{
    int iteration_count,cmp;
    double xnew,fnew,widthg;

    widthg = *px2 - *px0;

    iteration_count = 0;

    for(;;)
    {
        svt_exception::ui()->busyRotate();

   	// minimum successfully bracketed
	if((numcmp(*pf1,*pf0) < 0)&&(numcmp(*pf1,*pf2) < 0))
	{
	    *pcode = 0;
	    return;
	}

	++iteration_count;
	if(iteration_count > g_pPowell->getMaxIter2Bracket())
	{
	    *pcode = -2;
	    return;
	}

	expand_width(&widthg,*pf0,*pf1,*pf2,*px0,*px1,*px2);

	cmp = numcmp(*pf0,*pf2);

	if(cmp < 0)
	    xnew = *px0 - widthg;
	else
	    if(cmp > 0)
		xnew = *px2 + widthg;
	    else
		if(random_bit())
		    xnew = *px0 - widthg;
		else
		    xnew = *px2 + widthg;

	fnew = (*pfunction)(xnew);

        insert_new_bracket_point(pf0,pf1,pf2,px0,px1,px2,xnew,fnew);
    }
}

void order_args(double *pf0,double *pf1,double *pf2, double *px0, double *px1,double *px2)
{
    if(*px0 > *px2)
    {
	SWAPPING(*px0,*px2,double);
	SWAPPING(*pf0,*pf2,double);
    }
    if(*px0 > *px1)
    {
	SWAPPING(*px0,*px1,double);
	SWAPPING(*pf0,*pf1,double);
    } else {
	if(*px1 > *px2)
	{
	    SWAPPING(*px1,*px2,double);
	    SWAPPING(*pf1,*pf2,double);
	}
    }
}

/*
 * find bracked and identify minimum
 */
void minimize_1d_raw(int *pcode, double *pf0, double *pf1, double *pf2, double *px0, double *px1, double *px2, double f_goal, double (*pfunction)(double x), int max_iterations )
{
    // assume *pf0,*pf1,*pf2 already computed

    double original_f1,original_x1;

    order_args(pf0,pf1,pf2,px0,px1,px2);

    original_f1 = *pf1;
    original_x1 = *px1;

    //bracket the minimum
    bracket_minimum(pcode,pf0,pf1,pf2,px0,px1,px2,pfunction);

    //once the bracked if found, search for the minimum in the "parabola"
    if(*pcode == 0)
	trap_minimum(pf0,pf1,pf2,px0,px1,px2,f_goal,pfunction,max_iterations);

    else if(*pcode == -2)
    {
	if((original_f1 == *pf0)&&(original_f1 == *pf1)&&(original_f1 == *pf2))
	{
	    *px0 = *px1 = *px2 = original_x1;

	    *pcode = 0;
	}
    }
}

//
// Willy:
//

double powell_line_function(double x)
{
    cp_vect(&g_save_vect,&g_buffer_vect,g_save_len);
    add_scaled_vect(g_save_vect,g_save_direction,x,g_save_len);
    return ((*g_save_pfunction)(g_save_vect));
}

void powell_line_minimize(double *vect, double *direction, int len, double *pval_min, double (*pfunction)(double *vect) )
{
    double ax,bx,cx,fa,fb,fc;
    int code;

    cp_vect(&g_buffer_vect,&vect,len);
    g_save_vect = vect;
    g_save_len = len;
    g_save_direction = direction;
    g_save_pfunction = pfunction;

    ax = -1.0;
    bx = 0.0;
    cx = 1.0;
    fa = powell_line_function(ax);
    fb = *pval_min;
    fc = powell_line_function(cx);

    minimize_1d_raw(&code,&fa,&fb,&fc,&ax,&bx,&cx,fb,&powell_line_function,g_pPowell->getMaxIter2Minimize());

    cp_vect(&vect,&g_buffer_vect,len);

    // do not move if no improvement
    if(*pval_min == fb)
	return;

    add_scaled_vect(vect,direction,bx,len);

    *pval_min = fb;
}

/**
 * conjugate gradient method, called with external score function and output file
 * \param pcode return code (0=OK, 1=reached maximum number of iterations)
 * \param pval_min initial function value
 * \param vect initial vector
 * \param len number of dimensions of the problem (length of vector)
 * \param pfunction pointer to criterion/function
 * \param max_iter maximum number of iterations
 * \param pow_outfile output file
 * \param tolerance tolerance of change, if change is below then algo assumes convergence
 * \param orientation initial orientation of search
 */
void powell(int *pcode, double *pval_min, double *vect, int len, double (*pfunction)(double *vect), unsigned max_iter, FILE *pow_outfile, double tolerance, double *init_directions )
{
    int i,j;
    int ibig = 0;
    unsigned int iteration;
    double t,fptt,fp,del;
    double *pt,*ptt,*xit;
    double **xi;
    double last_val_min;

    iteration = 0;

    do_vect(&g_buffer_vect,len);
    do_mat(&xi,len,len);
    do_vect(&pt,len);
    do_vect(&ptt,len);
    do_vect(&xit,len);

    //  initialize_powell_directions
    zero_mat(xi,len,len);
    for(i=0;i<len;++i)
    {
	xi[i][i]=init_directions[i];
    }

    *pval_min = (*pfunction)(vect);
    cp_vect(&pt,&vect,len);

    POWL << "Powell conjugate gradient method" << endl;
    POWL << "  Tolerance: " << tolerance << endl;
    POWL << "  Max. Iterations: " << max_iter << endl;
    POWL << "  Max. Iterations to Bracket: " << g_pPowell->getMaxIter2Bracket() << endl;
    POWL << "  Max. Iterations to Minimize: " << g_pPowell->getMaxIter2Minimize() << endl;
    
    try
    {
	svt_exception::ui()->busyPopup("Powell optimization running, please wait...");

        for(iteration=0;;++iteration)
	{
            last_val_min = *pval_min;
            fp = *pval_min;
            del = 0.0;

            //POWL << "   "; fflush( stdout );

            for(i=0;i<len;++i)
            {
                svt_exception::ui()->busyRotate();

                //cout << " " << i+1 << "/" << len; fflush( stdout );
		g_pPowell->setLine(i);

                fptt = *pval_min;
                if (fabs(init_directions[i]) > EPSILON)
                {
                    powell_line_minimize(vect,xi[i],len,pval_min,pfunction);
                    if(fabs(fptt-(*pval_min)) > del)
                    {
                        del = fabs(fptt-(*pval_min));
                        ibig = i;
                    }
                }
            }
	    g_pPowell->setLine(len+1);// to mark that is out of the line maximization

            //cout << endl;

            if(*pval_min == last_val_min)
	    {
		svt_exception::ui()->busyPopdown();

                *pcode = 0;
                delete xit;
                delete ptt;
                delete pt;
                delete g_buffer_vect;
                for(i=0;i<len;i++)
                    delete xi[i];
                return;
            }

	    //inform user about outcome
	    //score and time per calculation
            POWL << " "; cout.setf(ios::right); cout.width(2); cout << iteration; cout << " - Score: "; cout.setf(ios::left); cout.precision(3); cout.width(2); cout << pPowFunc( vect );
            cout.precision(3);
            cout << " - " << ((Real64)(g_iTime) / (Real64)(g_iCount));
            cout << " - " << g_iCount <<  endl;

	    //set the current vector
	    g_pPowell->setVector( vect );

	    //output results - to be overloaded
	    g_pPowell->outputResult();
	    
            if (pow_outfile != NULL)
                for(i=0;i<len;++i)
                    fprintf(pow_outfile," %7.3f", vect[i]);

            // uncomment one of the following options
            // maximization
            if (pow_outfile != NULL)
                fprintf(pow_outfile,"    %10.7E %d\n",-(*pval_min), iteration+1);

            // minimization
            //if (pow_outfile != NULL)
            // fprintf(pow_outfile,"    %10.7E %d\n",(*pval_min), iteration+1);

            if (iteration >= max_iter)
	    {
		svt_exception::ui()->busyPopdown();

                *pcode = 1;
                delete xit;
                delete ptt;
                delete pt;
                delete g_buffer_vect;
                for(i=0;i<len;i++)
                    delete xi[i];
                return;
            }

            // original
            //if(2.0*fabs(fp-(*pval_min)) <= tolerance*(fabs(fp)+fabs(*pval_min)))
            // fixed
            if( fabs(*pval_min - last_val_min) <= tolerance*fabs(*pval_min) )
	    {
		svt_exception::ui()->busyPopdown();

                *pcode = 0;
                delete xit;
                delete ptt;
                delete pt;
                delete g_buffer_vect;
                for(i=0;i<len;i++)
                    delete xi[i];
                return;
            }

            for(j=0;j<len;++j)
            {
                ptt[j] = 2.0*vect[j]-pt[j];
                xit[j] = vect[j]-pt[j];
            }

            cp_vect(&pt,&vect,len);
            fptt = (*pfunction)(ptt);
            if(fptt < fp)
            {
                t = 2.0*(fp-2.0*(*pval_min)+fptt)*square(fp-(*pval_min)-del) - del*square(fp-fptt);
                if(t < 0.0)
                {
                    powell_line_minimize(vect,xit,len,pval_min,pfunction);
                    cp_vect(&(xi[ibig]),&xit,len);
                }
            }
        }

	svt_exception::ui()->busyPopdown();

    } catch (svt_userInterrupt&)
    {
	svt_exception::ui()->busyPopdown();

        *pcode = 1;
        delete xit;
        delete ptt;
        delete pt;
        delete g_buffer_vect;
        for(i=0;i<len;i++)
            delete xi[i];
        return;
    }


}
