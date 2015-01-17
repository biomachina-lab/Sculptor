/***************************************************************************
                          svt_edge_heap_set  -  description
                             -------------------
    begin                : 12.02.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt_includes
#include <svt_stlList.h>
#include <svt_edge_heap_set.h>
#include <svt_vertex_array.h>
#include <svt_mesh.h>

// svt_edgeHeap auf svt_edgeHeapSet uebertragen
// dabei wird automatisch einsortiert
// ver.edge muessen alle eingetragen werden, da es nicht mehr bei svt_edgeHeap.init geschieht
void svt_edgeHeapSet::set(svt_edgeHeap& edgeHeap, svt_vertex_array & ver)
{
    svt_edgeHeap::iterator it;
    svt_edgeHeapSet::iterator it2;

    clear(); // zuvor alle elemente loeschen
    for (it = edgeHeap.begin(); it != edgeHeap.end(); ++it)
    {
        it2 = insert(make_pair(it->cost, *it));
        ver[it->v1].addEdge(it2);
        ver[it->v2].addEdge(it2);
    }
}

svt_edgeHeapSet::iterator svt_edgeHeapSet::addEdgeSorted(svt_edge& edge)
{
    return insert(make_pair(edge.cost, edge));
}

// kante mit geringsten kosten holen und entfernen
// it gibt einen Iterator auf die geloeschte Stelle zurueck
svt_edge svt_edgeHeapSet::getFirstEdge(svt_edgeHeapSet::iterator& itFront)
{
    itFront = begin();
    svt_edge edge = (*itFront).second;
    erase(itFront);
    return edge;
}

// kante zum neu bewerten herausnehmen und sortiert wieder einfuegen
void svt_edgeHeapSet::reCalcInsert(unsigned int v, svt_vertex_array & ver)
{
    list<svt_edgeHeapSet::iterator>::iterator it, it2;
    svt_edgeHeapSet::iterator it3;
    svt_edge edge;
    unsigned int vo;
    it = ver[v].edge.begin();
    while (it != ver[v].edge.end())
    {
        edge = (**it).second; // merken der kante
        // knotennummer des anderen knoten der kante
        if (v == edge.v2)
            vo = edge.v1;
        else
            vo = edge.v2;
        erase(*it); // loeschen aus kanten-heap
        // loeschen der kante in liste des anderen knoten
        it2 = ver[vo].edge.begin();
        while (it2 != ver[vo].edge.end())
        {
            if ((*it2) == (*it))
                it2 = ver[vo].edge.erase(it2);
            else
                ++it2;
        }
        // loeschen aus edge-liste des knoten (daher nicht it zu erhoehen)
        it = ver[v].edge.erase(it);

        // neuberechnung von zielknoten und kosten
        if (edge.computeTargetCost(ver))
            m_singularQuadricNumber++; // wenn singulaere quadrik, dann mitzaehlen
        else 
            m_regularQuadricNumber++; // wenn regulaere quadrik, dann mitzaehlen
        it3 = addEdgeSorted(edge); // einfuegen der kante in den kanten-heap 
        // verweis auf die kante in die edge-liste beider knoten einfuegen
        // vorne, damit keine endlosschleife
        ver[vo].edge.push_front(it3);
        ver[v].edge.push_front(it3);
    }
}
