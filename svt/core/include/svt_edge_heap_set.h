/***************************************************************************
                          svt_edge_heap_set  -  description
                             -------------------
    begin                : 12.02.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_EDGE_HEAP_SET_H
#define SVT_EDGE_HEAP_SET_H

#include <svt_core.h>
#include <svt_init.h>
#include <svt_types.h>
#include <svt_node.h>
#include <svt_stlList.h>
#include <svt_stlMap.h>
#include <svt_vector3.h>
#include <svt_iostream.h>
#include <svt_edge_heap.h>

// forward declaration
// from svt_vertex_array
class svt_vertex_array_element;
class svt_vertex_array;
// from svt_mesh
class svt_triangle_type;
typedef vector<svt_triangle_type> svt_triangle_array;

class DLLEXPORT svt_edgeHeapSet : public multimap<double, svt_edge>
{
public:
    svt_edgeHeapSet()
    {
        m_regularQuadricNumber = 0;
        m_singularQuadricNumber = 0;
    }
    void set(svt_edgeHeap& edgeHeap, svt_vertex_array & ver);
    svt_edgeHeapSet::iterator addEdgeSorted(svt_edge& edge);
    svt_edge getFirstEdge(svt_edgeHeapSet::iterator& itFront);
    void reCalcInsert(unsigned int v, svt_vertex_array & ver);

    unsigned int getRegularQuadricNumber()
    {
        return m_regularQuadricNumber;
    }
    unsigned int getSingularQuadricNumber()
    {
        return m_singularQuadricNumber;
    }
    void setRegularQuadricNumber(unsigned int n)
    {
        m_regularQuadricNumber = n;
    }
    void setSingularQuadricNumber(unsigned int n)
    {
        m_singularQuadricNumber = n;
    }

protected:
    unsigned int m_regularQuadricNumber;
    unsigned int m_singularQuadricNumber;
};

inline ostream& operator<< (ostream& s, list<svt_edgeHeapSet::iterator> l) {
    list<svt_edgeHeapSet::iterator>::iterator it;
    for (it = l.begin(); it != l.end(); ++it)
        s << (**it).second << endl;
    return s;
}
inline ostream& operator<< (ostream& s, svt_edgeHeapSet& l) {
    svt_edgeHeapSet::iterator it;
    s << "Edge-Heap:" << endl;
    for (it = l.begin(); it != l.end(); ++it)
        s << (*it).second << endl;
    return s;
}

#endif
