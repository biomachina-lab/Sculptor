/***************************************************************************
                          svt_vertex_array  -  description
                             -------------------
    begin                : 21.01.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_VERTEX_ARRAY_H
#define SVT_VERTEX_ARRAY_H

#include <svt_iostream.h>

#include <svt_types.h>
#include <svt_basics.h>
#include <svt_core.h>
#include <svt_color.h>

#include <svt_stlList.h>
#include <svt_stlVector.h>
#include <svt_stlMap.h>

#include <svt_edge_heap_set.h>
#include <svt_quadric.h>
#include <svt_pm.h>
//#include <svt_vertex_set.h>

//#define VER_EPS 0.000000000001

//#pragma warning (disable: 4275)

template <typename T> class svt_vector3;

class svt_vertex_array_element {    // Knotenstruktur
public:
    svt_vector3<double> pkt;        // Punktkoordinaten
    svt_vector3<double> norm;       // Normale am Knoten zum glatten shading bei Zeichnung
    Point2f tex;                    // Texturkoordinate
    svt_color col;                  // Farbe des Knoten
    list<int> tri;  // verkettete Liste mit Dreiecken, in den der Knoten vorkommt (einfach verkettet ist wohl nicht ueberall verfuegbar)
    // !!!!! waehrend der Vereinfachung darf keine Kopie erstellt werden, da sonst die iteratoren nicht mehr an die richtige stelle zeigen !!!!!
    list<svt_edgeHeapSet::iterator> edge;  // Liste mit Iteratoren (Zeigern) auf Kanten, in den Knoten vorkommt
    svt_quadric quadric;            // Quadric gemaess Garland/Heckbert

    svt_vertex_array_element()
    {
    }

    virtual ~svt_vertex_array_element()
    {
    }

    svt_vertex_array_element(const svt_vertex_array_element &rhs)
    {
        int i;

    	for (i=0; i<3; i++)
    	{
    	    pkt[i] = rhs.pkt[i];
    	    norm[i] = rhs.norm[i];
    	}
    	for (i=0; i<2; i++)
    	    tex[i] = rhs.tex[i];
        col = rhs.col;
        tri = rhs.tri;
        edge = rhs.edge;
        quadric = rhs.quadric;
    }

    svt_vertex_array_element& operator=(const svt_pm_ver& rhs)
    {
        pkt = rhs.pkt;
        norm = rhs.norm;
        tex[0] = rhs.tex[0];
        tex[1] = rhs.tex[1]; 
        col = rhs.col; 
        return *this;
    }

    void addEdge(svt_edgeHeapSet::iterator& edgeIter) 
    { 
        edge.push_back(edgeIter); 
    }

    list<svt_edgeHeapSet::iterator>& getEdges() 
    { 
        return edge; 
    }
};

inline ostream& operator<< (ostream& s, svt_vertex_array_element& v) {
    int i;
    list<int>::iterator it;

    s << "pkt( ";
    for (i= 0; i<3; i++)
        s << v.pkt[i] << " ";
    s << "), norm( ";
    for (i= 0; i<3; i++)
        s << v.norm[i] << " ";
    s << "), tex( ";
    for (i= 0; i<2; i++)
        s << v.tex[i] << " ";
    s << "), tri( ";
    for (it = v.tri.begin(); it != v.tri.end(); ++it)
        s << (*it)  << " ";
    s << "), quadric: " << v.quadric;
    s << ", edge:" << endl << v.edge;
    return s;
}

class svt_vector_map : public svt_vector3<double>
{
public:
    svt_vector_map() : svt_vector3<double>() {}
    svt_vector_map(svt_vector3<double> rhs) : svt_vector3<double>(rhs) {}
    bool operator< (const svt_vector_map& x) const
    {
        if (((*this)[0] < x[0]) || 
            (((*this)[0] == x[0]) && ((*this)[1] < x[1])) ||
            (((*this)[0] == x[0]) && ((*this)[1] == x[1]) && ((*this)[2] < x[2])))
            return true;
        else
            return false;
    }

};

/**
 * Hold a vertex array (aka vertex)
 *@author Maik Boltes
 */
class DLLEXPORT svt_vertex_array : public vector<svt_vertex_array_element> 
{
public:
    /**
     * Constructor
     */
    svt_vertex_array();

    void addEdge(int i, svt_edgeHeapSet::iterator edgeIter);
    list<svt_edgeHeapSet::iterator>& getEdges(int i);

    int addVertex(const svt_vector3<double>& pkt);
    int addVertex(const svt_vector3<double>& pkt, const svt_color &col);
    int addVertex(const svt_vector3<double>& pkt, const svt_vector3<double>& norm);
    int addVertex(const svt_vector3<double>& pkt, const svt_vector3<double>& norm, const Point2f& tex);
    int addVertex(const svt_vector3<double>& pkt, const svt_vector3<double>& norm, const Point2f &tex, const svt_color &col);

    void calcMinMax();
    float getMinX();
    float getMinY();
    float getMinZ();
    float getMaxX();
    float getMaxY();
    float getMaxZ();
    void setMinX(float);
    void setMinY(float);
    void setMinZ(float);
    void setMaxX(float);
    void setMaxY(float);
    void setMaxZ(float);

    /**
     * delete all vertices
     */
    void deleteAll()
    {
        m_fMinX = m_fMinY = m_fMinZ = m_fMaxX = m_fMaxY = m_fMaxZ = 0;
        m_verNumber = 0;
        m_remapped_vertices = 0;
        clear();
        m_vertex_map.clear();
    }

    bool getCompareFlag();
    void setCompareFlag(bool b);
    void getNeighbour(svt_vector3<double>& pkt, double eps, list<int>& neighList);
    map<svt_vector_map, int>& getVertexMap() { return m_vertex_map; }

    unsigned int getNumber() { return m_verNumber; }
    void setNumber(unsigned int i) { m_verNumber = i; }

    int getRemappedVertices() { return m_remapped_vertices; }

//     // Constants (also used in mesh class)
//     static const double eq_eps;

protected:

    // Flags
    // for comparing vertex position with all other vertices by adding it to the vertex array
    bool m_bCompareVerFlag;
    int  m_remapped_vertices; // anzahl der durch einen anderen knoten ersetzten knoten
    // datenstruktur um gleiche knoten zu eliminieren
    // map ermoeglicht einfuegen und suchen in logarithmischer zeit
    // set waere mit reinen indizes sparsamer, aber der zugriff
    // ueber vergleichsfunktion statt op< schlecht, da op== dann
    // ueber vergl.fkt definiert wird, was hier unerwuenscht ist
    map<svt_vector_map, int> m_vertex_map;

    // die wirklich im aktuell sichtbaren Netz befindlichen punkte
    // nicht die groesse des vektors
    int m_verNumber;

    // minimale X Y Z der Knoten - wird nur auf zuruf berechnet
    float m_fMinX;
    float m_fMinY;
    float m_fMinZ;
    // maximale X Y Z der Knoten - wird nur auf zuruf berechnet
    float m_fMaxX;
    float m_fMaxY;
    float m_fMaxZ;
};

inline ostream& operator<< (ostream& s, svt_vertex_array& v) {
    unsigned int i;

    for (i= 0; i<v.size(); i++)
        s << "Vertex " << i << ": " << v[i] << endl;
    return s;
}

#endif









