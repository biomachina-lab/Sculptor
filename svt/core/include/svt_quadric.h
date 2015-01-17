/***************************************************************************
                          svt_quadric  -  description
                             -------------------
    begin                : 07.02.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_QUADRIC_H
#define SVT_QUADRIC_H

#include <svt_core.h>
#include <svt_types.h>
#include <svt_iostream.h>
#include <svt_vector3.h>

// minimal determinant value for nearly singular quadrics
#define DET_EPS 0.0001

/** 
  * quadric
  *@author Maik Boltes
  */
class DLLEXPORT svt_quadric
{
public:
    /**
     * Constructors
     */
    svt_quadric() { clear(); }
    svt_quadric(double a, double b, double c, double d)
	{ init(a, b, c, d); }
    svt_quadric(const svt_vector3<double>& n, double d)
	{ init(n[0], n[1], n[2], d); }
    svt_quadric(const double* n, double d)
	{ init(n[0], n[1], n[2], d); }
    svt_quadric(const svt_quadric& Q) { *this = Q; }

    /**
     * set all elements of the quadric to the same value
     * \param val value to which the elements of the quadric will set 
     */
    void clear(double val=0.0) { a2=ab=ac=ad=b2=bc=bd=c2=cd=d2=val; }
    /**
     * multiply quadric with a scalar
     * \param s scalar
     * \return multiplied quadric
     */ 
    const svt_quadric& operator*=(double s);
    /**
     * the result is the quadric multiplied with a scalar
     * \param s scalar
     * \return multiplied quadric
     */ 
    const svt_quadric operator*(double s) const;
    /**
     * assign a quadric
     * \param Q quadric
     * \return assigned quadric
     */ 
    svt_quadric& operator=(const svt_quadric& Q);
    /**
     * add a quadric
     * \param Q quadric
     * \return result after adding quadric
     */ 
    const svt_quadric& operator+=(const svt_quadric& Q);
    /**
     * the result is the quadric added with another
     * \param Q quadric
     * \return result after adding quadric
     */
    const svt_quadric operator+(const svt_quadric& Q) const;

    /**
     * calculate the determinant of the upper left submatrix
     * \return determinant
     */ 
    double det_part() const;

    /**
     * evaluate the quadric error metric vQv
     * \param v vector 
     * \return quadric error
     */ 
    double evaluate(const svt_vector3<double>& v) const;
    /**
     * evaluate the optimal vertex with the minimal quadric error vQv
     * \param v vector is set to optimal vertex
     * \return true if quadric nearly singular, otherwise false
     */ 
    bool optimize(svt_vector3<double>& v) const;
    /**
     * evaluate the optimal vertex with the minimal quadric error vQv between two points and their middlepoint
     * \param v vector is set to optimal vertex
     * \param v1 point 1 
     * \param v2 point 2
     * \return value of the quadric error for suboptimal point
     */ 
    double optimize(svt_vector3<double>& v, const svt_vector3<double>& v1, const svt_vector3<double>& v2) const;

    /**
     * 1-norm of quadric matrix: maximal sum of rows
     * \return 1-norm of quadric matrix
     */
    double norm() const;

    /**
     * stream out quadric in short form
     * \param out output stream
     * \return output stream 
     */ 
    ostream& write(ostream& out)
	{
	    return out << a2 << " " << ab << " " << ac << " " << ad << " "
		       << b2 << " " << bc << " " << bd << " " << c2 << " "
		       << cd << " " << d2;
	}

    /**
     * stream out quadric in long form
     * \param out output stream
     * \return output stream 
     */ 
    ostream& write_full(ostream& out)
	{
	    return out << a2 << " " << ab << " " << ac << " " << ad << " "
		       << ab << " " << b2 << " " << bc << " " << bd << " "
		       << ac << " " << bc << " " << c2 << " " << cd << " "
		       << ad << " " << bd << " " << cd << " " << d2;
	}


    /**
     * stream in quadric in short form
     * \param in input stream
     * \return input stream 
     */ 
    istream& read(istream& in)
	{
	    return in >> a2 >> ab >> ac >> ad >> b2
		      >> bc >> bd >> c2 >> cd >> d2;
	}
    

    /**
     * stream in quadric in long form
     * \param in input stream
     * \return input stream 
     */ 
    istream& read_full(istream& in)
	{
	    return in >> a2 >> ab >> ac >> ad
		      >> ab >> b2 >> bc >> bd
		      >> ac >> bc >> c2 >> cd
		      >> ad >> bd >> cd >> d2;
	}

    /**
     * set quadric 
     * \param n normalized normal vector
     * \param d distance to origin
     */ 
    void init(const svt_vector3<double> &n, double d)
	{ init(n[0], n[1], n[2], d); }

    /**
     * set quadric 
     * \param 10 values of the symmetric quadric
     * la2 lab lac lad
     *     lb2 lbc lbd
     *         lc2 lcd
     *             ld2
     */ 
    void init(double a2, double ab, double ac, double ad, double b2, double bc, double bd, double c2, double cd, double d2);

private:
    /**
     * set quadric 
     * \param a x value of normalized normal vector
     * \param b y value of normalized normal vector
     * \param c z value of normalized normal vector
     * \param d distance to origin
     */ 
    void init(double a, double b, double c, double d);

    // values of the symmetric quadric
    double a2, ab, ac, ad;
    double     b2, bc, bd;
    double         c2, cd;
    double             d2;
};

/**
 * stream out quadric
 * \param out output stream
 * \param Q quadric
 * \return output stream 
 */ 
inline ostream& operator<<(ostream& out, svt_quadric& Q) {return Q.write(out);}
/**
 * stream in quadric
 * \param in input stream
 * \param Q quadric
 * \return input stream 
 */ 
inline istream& operator>>(istream& in, svt_quadric& Q) { return Q.read(in); }

#endif
