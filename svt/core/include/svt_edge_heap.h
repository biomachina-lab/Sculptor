/***************************************************************************
                          svt_edge_heap  -  description
                             -------------------
    begin                : 12.02.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_EDGE_HEAP_H
#define SVT_EDGE_HEAP_H

#include <svt_core.h>
#include <svt_init.h>
#include <svt_types.h>
#include <svt_node.h>
#include <svt_stlList.h>
#include <svt_stlSet.h>
#include <svt_vector3.h>
#include <svt_iostream.h>

//#pragma warning (disable: 4275)

// forward declaration
// from svt_vertex_array
class svt_vertex_array_element;
class svt_vertex_array;
// from svt_mesh
class svt_triangle_type;
typedef vector<svt_triangle_type> svt_triangle_array;

class DLLEXPORT svt_edge 
{  // kante
public:
    UInt32 v1, v2;                 // Eckpunktnummern der Kante
    svt_vector3<double> pkt;       // v strich als punkt auf den v1 und v2 zusammenfallen wuerden
    double cost;                   // kosten fuer das zusammenfallen
    // dreiecksliste wird nicht vorgehalten, weil das aus den dreieckslisten der knoten gewonnen werden kann
    // ueber diese listen ist auch herauszubekommen, ob es sich um einen rand handelt (1 gemeindames dreieck),
    // um eine normale kante (2 dr), um eine entartete kante (>2 dr) oder um keine echte kante (0 dr)

    // fuer sortierfunktion der liste genutzt; braucht VC7 (wird nicht im code benutzt)
    bool operator< (const svt_edge& x) const
    {
        return cost < x.cost;
    }
    // braucht VC7 um remove der Liste zu implementieren (wird nicht im code benutzt)
    bool operator== (const svt_edge& x) const
    {
        return (cost == x.cost) && (v1 == x.v1) && (v2 == x.v2) && (pkt == x.pkt) ;
    }
    // berechnet die Kosten und verschmelzungspunkt
    // gibt true zurueck, wenn (fast) singulaere quadrik vorgelegen hat, bei regulaerer false
    bool computeTargetCost(svt_vertex_array& ver);
};

inline ostream& operator<< (ostream& s, const svt_edge& e)
{
    s << "(" << e.v1 << ", " << e.v2 << ")->(" << e.pkt << ") " << e.cost;
    return s;
}

class DLLEXPORT svt_edgeHeap : public list<svt_edge>
{
public:
    svt_edgeHeap();

    void init(svt_vertex_array & ver, svt_triangle_array & tri, double eps);
    svt_edgeHeap::iterator addEdge(svt_edge& edge);
    svt_edgeHeap::iterator addEdgeSorted(svt_edge& edge);
    unsigned int computeAllTargetCost(svt_vertex_array &ver);
    svt_edge getFirstEdge(svt_edgeHeap::iterator& itFront);
    void delFirstEdge();
    bool edgeExists(unsigned int i, unsigned int j);

};

inline ostream& operator<< (ostream& s, list<svt_edgeHeap::iterator> l) {
    list<svt_edgeHeap::iterator>::iterator it;
    for (it = l.begin(); it != l.end(); ++it)
        s << (**it) << endl;
    return s;
}
inline ostream& operator<< (ostream& s, svt_edgeHeap& l) {
    svt_edgeHeap::iterator it;
    s << "Edge-Heap:" << endl;
    for (it = l.begin(); it != l.end(); ++it)
        s << (*it) << endl;
    return s;
}

#endif
