/***************************************************************************
                          svt_quadric  -  description
                             -------------------
    begin                : 07.02.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include "math.h"

#include "svt_quadric.h"

/**
 * calculate the determinant of the upper left submatrix
 * \return determinant
 */
double svt_quadric::det_part() const
{
    return a2*b2*c2 + 2*ab*bc*ac - a2*bc*bc - b2*ac*ac - c2*ab*ab;
}

/**
 * set quadric 
 * \param a x value of normalized normal vector
 * \param b y value of normalized normal vector
 * \param c z value of normalized normal vector
 * \param d distance to origin
 */
void svt_quadric::init(double a, double b, double c, double d)
{
    a2 = a*a;  ab = a*b;  ac = a*c;  ad = a*d;
               b2 = b*b;  bc = b*c;  bd = b*d;
	                      c2 = c*c;  cd = c*d;
			                         d2 = d*d;
}

/**
 * set quadric 
 * \param 10 values of the symmetric quadric
 * la2 lab lac lad
 *     lb2 lbc lbd
 *         lc2 lcd
 *             ld2
 */
void svt_quadric::init(double la2, double lab, double lac, double lad, double lb2, double lbc, double lbd, double lc2, double lcd, double ld2)
{
    a2 = la2;  ab = lab;  ac = lac;  ad = lad;
               b2 = lb2;  bc = lbc;  bd = lbd;
	                      c2 = lc2;  cd = lcd;
			                         d2 = ld2;
}

/**
 * multiply quadric with a scalar
 * \param s scalar
 * \return multiplied quadric
 */
const svt_quadric& svt_quadric::operator*=(double s)
{
    a2 *= s;  ab *= s;  ac *= s;  ad *= s;
              b2 *= s;  bc *= s;  bd *= s;
                        c2 *= s;  cd *= s;
			                      d2 *= s;
    return *this;
}

/**
 * the result is the quadric multiplied with a scalar
 * \param s scalar
 * \return multiplied quadric
 */
const svt_quadric svt_quadric::operator*(double s) const
{
    return svt_quadric(*this) *= s;
}

/**
 * assign a quadric
 * \param Q quadric
 * \return assigned quadric
 */
svt_quadric& svt_quadric::operator=(const svt_quadric& Q)
{
    if (this == &Q) return *this;  // time-saving self-test

    a2 = Q.a2;  ab = Q.ab;  ac = Q.ac;  ad = Q.ad;
                b2 = Q.b2;  bc = Q.bc;  bd = Q.bd;
		                    c2 = Q.c2;  cd = Q.cd;
                                        d2 = Q.d2;
    return *this;
}

/**
 * add a quadric
 * \param Q quadric
 * \return result after adding quadric
 */
const svt_quadric& svt_quadric::operator+=(const svt_quadric& Q)
{
    a2 += Q.a2;  ab += Q.ab;  ac += Q.ac;  ad += Q.ad;
                 b2 += Q.b2;  bc += Q.bc;  bd += Q.bd;
		                      c2 += Q.c2;  cd += Q.cd;
			                               d2 += Q.d2;
    return *this;
}

/**
 * the result is the quadric added with another
 * \param Q quadric
 * \return result after adding quadric
 */
const svt_quadric svt_quadric::operator+(const svt_quadric& Q) const
{
    return svt_quadric(*this) += Q;
}

/**
 * evaluate the quadric error metric vQv
 * \param v vector 
 * \return quadric error
 */
double svt_quadric::evaluate(const svt_vector3<double>& v) const
{
    return v[0]*v[0]*a2 + 2*v[0]*v[1]*ab + 2*v[0]*v[2]*ac + 2*v[0]*ad
	                    +   v[1]*v[1]*b2 + 2*v[1]*v[2]*bc + 2*v[1]*bd
	                                     +   v[2]*v[2]*c2 + 2*v[2]*cd
	                                                      + d2;
}

/**
 * evaluate the optimal vertex with the minimal quadric error vQv
 * \param v vector is set to optimal vertex
 * \return true if quadric nearly singular, otherwise false
 */
bool svt_quadric::optimize(svt_vector3<double>& v) const
{
    double det = det_part();

    // DET_EPS ist verhaeltnismaessig gross, da determinate schlecht mit singularitaet der matrix korreliert
    if (fabs(det) > DET_EPS)
    {
        v[0] = (ad*bc*bc + ab*bd*c2 + ac*b2*cd - ab*bc*cd - ac*bd*bc - ad*b2*c2) / det;
        v[1] = (a2*bc*cd + ac*bd*ac + ad*ab*c2 - ad*bc*ac - ac*ab*cd - a2*bd*c2) / det;
        v[2] =(ad*b2*ac + ab*ab*cd + a2*bd*bc - a2*b2*cd - ab*bd*ac - ad*ab*bc) / det;
        return true;
    } 
    else
        return false;
}

/**
 * evaluate the optimal vertex with the minimal quadric error vQv between two points and their middlepoint
 * \param v vector is set to optimal vertex
 * \param v1 point 1 
 * \param v2 point 2
 * \return value of the quadric error for suboptimal point
 */
double svt_quadric::optimize(svt_vector3<double>& v, const svt_vector3<double>& v1, const svt_vector3<double>& v2) const
{
    svt_vector3<double> mid;
    double c1, c2, c3;
    
    mid = (v1 + v2)/2.;

    // als letzte moeglichkeit werden die punkte selber und der mittelpunkt untersucht und pkt mit geringsten kosten genommen
    c1 = evaluate(v1); c2 = evaluate(v2); c3 = evaluate(mid);
    if ((c1 < c2) && (c1 < c3))
    {
        v = v1; 
        return c1;
    }
    else if ((c2 < c1) && (c2 < c3))
    {
        v = v2;
        return c2;
    }
    else // sind sie zb alle gleich gross, wird der mittelpunkt genommen
    {
        v = mid;
        return c3;
    }

    return 0.0;
}

/**
 * 1-norm of quadric matrix: maximal sum of rows
 * \return 1-norm of quadric matrix
 */
double svt_quadric::norm() const
{
    double max, tmp;
    max = a2+ab+ac+ad;
    if ((tmp = (ab+b2+bc+bd)) > max)
        max = tmp;
    if ((tmp = (ac+bc+c2+cd)) > max)
        max = tmp;
    if ((tmp = (ad+bd+cd+d2)) > max)
        max = tmp;
    return max;
}

