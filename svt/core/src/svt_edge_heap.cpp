/***************************************************************************
                          svt_edge_heap  -  description
                             -------------------
    begin                : 12.02.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt_includes
#include <svt_stlList.h>
#include <svt_edge_heap.h>
#include <svt_vertex_array.h>
#include <svt_mesh.h>

// gibt true zurueck, wenn (fast) singulaere quadrik vorgelegen hat, bei regulaerer false
bool svt_edge::computeTargetCost(svt_vertex_array& ver)
{
    bool regulaer;
    svt_quadric q = ver[v1].quadric + ver[v2].quadric;

    // wenn untermatrix nicht invertierbar, also optimaler punkt nicht zu berechnen, 
    // werden endpunkte und mittelpunkt untersucht oder besser auf der Geraden optimalen Platz suchen
    if (!(regulaer = q.optimize(pkt)))
        cost = q.optimize(pkt, ver[v1].pkt, ver[v2].pkt);
    else
        cost = q.evaluate(pkt);

    return !regulaer;
}

svt_edgeHeap::svt_edgeHeap()
{
}

// bestimmt alle kanten des netzes
// falls eps groesser 0, werden auch nicht zusammenhaengende punkte als kante aufgenommen
void svt_edgeHeap::init(svt_vertex_array & ver, svt_triangle_array & tri, double eps)
{
    // alle echten kanten einsammeln in o(n)
    vector<bool> visit(ver.size(), false);
    unsigned int i, j, k;
    list<unsigned int> vecList;
    svt_edge edge;
    list<int>::iterator it;
    list<int> neighList;
    int st;
    unsigned int p1 = 0, p2;

    for (i=0; i < ver.size(); i++)
        if (!visit[i])
        {
            visit[i] = true;
            for (it = ver[i].tri.begin(); it != ver[i].tri.end(); ++it)
                for (j = 0; j < 3; j++)
            	    if (tri[*it].ver[j] == i)
                        for (k = 1; k < 3; k++)
                            if (!visit[tri[*it].ver[(j+k)%3]] && (find(vecList.begin(), vecList.end(), tri[*it].ver[(j+k)%3]) == vecList.end()))
                            {
                                edge.v1 = i; 
                                edge.v2 = tri[*it].ver[(j+k)%3];
                                vecList.push_front(edge.v2);
                                addEdge(edge);
                            }
            vecList.clear();
        }
    cout << "(" << (st = size()) << " in mesh; " << flush;
    // knoten nah beieinander such ueber intelligente datensruktur, sonst o(n2)
    // bei marching cube nicht unbedingt noetig, wenn bei erzeugung schon gleiche knoten auf einen gemappt werden
    if (eps > 0)
    {
        printf("  0%%"); fflush(stdout);
        // diese eps-grenze zwischen dem auf einer komplexen datenstruktur
        // beruhenden nachbarschaftssuchen und dem einfachen vergleich
        // jedes knoten mit jedem berueht auf der festen maximalen netzausdehnung von 2
        // und den erfahrungen aus messungen mit bones.smf
        if (eps < 0.06)
        {
            for (i=0; i < ver.size(); i++)
            {
                ver.getNeighbour(ver[i].pkt, eps, neighList);
                for (it = neighList.begin(); it != neighList.end(); ++it)
                {
                    if (!edgeExists(i, *it))
                    {
                        edge.v1 = i; 
                        edge.v2 = *it;
                        addEdge(edge);
                    }
                }
                neighList.clear();
                // da berechnung bei grossem eps sehr lange dauern kann anzeige von %
                // if, da sonst ausgabe zuviel zeit in anspruch nimmt
                if ((p2 = ((100*i)/ver.size())) > p1)
                {
                    p1 = p2;
                    printf("\b\b\b\b%3d%%", p1); fflush(stdout);
                }
            }
        }
        else // einfacher vergleich jeden knotens mit jedem!
        {
            for (i=0; i < ver.size(); i++)
            {
                for (j=i+1; j < ver.size(); j++)
                {
                    if (ver[i].pkt.distance(ver[j].pkt)<eps)
                    {
                        if (!edgeExists(i, j))
                        {
                            edge.v1 = i; 
                            edge.v2 = j;
                            addEdge(edge);
                        }
                    }
                }
                // da berechnung bei grossem eps sehr lange dauern kann anzeige von %
                // if, da sonst ausgabe zuviel zeit in anspruch nimmt
                if ((p2 = ((100*i)/ver.size())) > p1)
                {
                    p1 = p2;
                    printf("\b\b\b\b%3d%%", p1); fflush(stdout);
                }
            }
        }
        printf("\b\b\b\b"); fflush(stdout);
    }
    cout << (size() - st) << " vertex pairs because of threshold value " << eps << ")" << flush;
}

svt_edgeHeap::iterator svt_edgeHeap::addEdge(svt_edge& edge)
{
    push_front(edge);
    return begin();
}

svt_edgeHeap::iterator svt_edgeHeap::addEdgeSorted(svt_edge& edge)
{
    svt_edgeHeap::iterator it = begin();
    while ((it != end()) && ((*it) < edge))
    {
        ++it;
    }
    return insert(it, edge);
}

// gibt anzahl (fast) singulaerer quadricen bei berechnung zurueck
unsigned int svt_edgeHeap::computeAllTargetCost(svt_vertex_array &ver)
{
    unsigned int sing = 0;
    svt_edgeHeap::iterator it;

    for (it = begin(); it != end(); ++it)
        if (it->computeTargetCost(ver))
            sing++; // erhoehen, wenn (fast) singulaere quadric vorlag

    return sing;
}

// kante mit geringsten kosten holen und entfernen
// it gibt einen Iterator auf die geloeschte Stelle zurueck
svt_edge svt_edgeHeap::getFirstEdge(svt_edgeHeap::iterator& itFront)
{
    itFront = begin();
    svt_edge edge = front();
    delFirstEdge();
    return edge;
}

void svt_edgeHeap::delFirstEdge()
{
    pop_front();
}

bool svt_edgeHeap::edgeExists(unsigned int i, unsigned int j)
{
    svt_edgeHeap::iterator it;
    for (it = begin(); it != end(); ++it)
        if (((it->v1 == i) && (it->v2 == j)) || ((it->v1 == j) && (it->v2 == i)))
            return true;
    return false;
}
