/***************************************************************************
                          svt_vertex_array  -  description
                             -------------------
    begin                : 21.01.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <float.h>
#include <math.h>

#include <svt_vertex_array.h>

// const double svt_vertex_array::eq_eps = 0.0000001;

/**
 * Constructor
 * \param pPos pointer to svt_pos object
 * \param pProp pointer to svt_properties object
 */
svt_vertex_array::svt_vertex_array()
{ 
    //true wird nach marching cube gesetzt in svt_core/include/svt_volume_mc_cline.h
    m_bCompareVerFlag = false;
    // werden nur auf zuruf berechnet
    m_fMinX = m_fMinY = m_fMinZ = m_fMaxX = m_fMaxY = m_fMaxZ = 0;
    m_verNumber = 0;
    m_remapped_vertices = 0;
}

void svt_vertex_array::addEdge(int i, svt_edgeHeapSet::iterator edgeIter)
{
    (*this)[i].addEdge(edgeIter);
}

list<svt_edgeHeapSet::iterator>& svt_vertex_array::getEdges(int i)
{
    return (*this)[i].getEdges();
}

int svt_vertex_array::addVertex(const svt_vector3<double>& pkt)
{
    svt_vector3<double> norm = 0.0; // {0.0, 0.0, 0.0};
    //int i;

    //for (i=0; i<3; i++)
    //    norm[i] = 0.0;

    return addVertex(pkt, norm);
}

int svt_vertex_array::addVertex(const svt_vector3<double>& pkt, const svt_color &col)
{
    svt_vector3<double> norm = 0.0; // {0.0, 0.0, 0.0};
    Point2f tex = {0.0, 0.0};

    return addVertex(pkt, norm, tex, col);
}

int svt_vertex_array::addVertex(const svt_vector3<double>& pkt, const svt_vector3<double>& norm)
{
    Point2f tex = {0.0, 0.0}; // Speicherplatz muesste noch bei addVertex Aufruf existieren
    //int i;

    //for (i=0; i<2; i++)
    //    tex[i] = 0.0;

    return addVertex(pkt, norm, tex);
}

int svt_vertex_array::addVertex(const svt_vector3<double>& pkt, const svt_vector3<double>& norm, const Point2f &tex)
{
    svt_color col; //default: (1.,1.,1.)

    return addVertex(pkt, norm, tex, col);
}

int svt_vertex_array::addVertex(const svt_vector3<double>& pkt, const svt_vector3<double>& norm, const Point2f &tex, const svt_color &col)
{
    int i;
    int indexMin = -1;

    if (getCompareFlag())
    {
        map<svt_vector_map, int>::iterator lb, ub;
        double dist, distMin = 1000.;

        lb = m_vertex_map.lower_bound(pkt-svt_vector3<double>(EQ_EPS));
        if ((ub = m_vertex_map.upper_bound(pkt)) != m_vertex_map.end())
            ++ub;
        while((lb != m_vertex_map.end()) && (lb != ub))
        {
            if ((*this)[lb->second].pkt.nearEqual(pkt))
            {
                // following line would be ok, because normaly only one 
                // vertex is nearEqual with that low EPS value and
                // averaging is for marching cube not neccessary,
                // because all values are equal:
                // return lb->second;
                dist = (*this)[lb->second].pkt.distance(pkt);
                if (dist < distMin)
                {
                    distMin = dist;
                    indexMin = lb->second;
                }
            }
            ++lb;
        }
    }

    if (indexMin != -1)
    {
        int triSize = (*this)[indexMin].tri.size();
        m_remapped_vertices++;
        // averaging normal, texture and color
        for (i=0; i<3; i++)
            (*this)[indexMin].norm[i] = (triSize * (*this)[indexMin].norm[i] + norm[i]) / (triSize + 1);
        for (i=0; i<2; i++)
            (*this)[indexMin].tex[i] = (triSize * (*this)[indexMin].tex[i] + tex[i]) / (triSize + 1);
        (*this)[indexMin].col.set( 
               (triSize * (*this)[indexMin].col.getR() + col.getR()) / (triSize + 1),
               (triSize * (*this)[indexMin].col.getG() + col.getG()) / (triSize + 1),
               (triSize * (*this)[indexMin].col.getB() + col.getB()) / (triSize + 1));
        return indexMin;
    }
    else
    { 
        // erst hier definiert, da mgl kein speicherplatz angelegt werden muss
        svt_vertex_array_element vertex;

        // in map einfuegen, wenn ein vergleich vorgesehen ist
        if (getCompareFlag())
            m_vertex_map.insert(pair<svt_vector_map, int>(pkt, m_verNumber));
    	for (i=0; i<3; i++)
    	{
    	    vertex.pkt[i] = pkt[i];
    	    vertex.norm[i] = norm[i];
    	}
    	for (i=0; i<2; i++)
    	    vertex.tex[i] = tex[i];
        vertex.col = col;

        m_verNumber++;

    	push_back(vertex);
    	return m_verNumber-1;
    }
}

void svt_vertex_array::calcMinMax()
{
    // maximale X Y Z der Knoten
    m_fMinX = FLT_MAX; m_fMinY = FLT_MAX; m_fMinZ = FLT_MAX;
    // maximale X Y Z der Knoten
    m_fMaxX = -FLT_MAX; m_fMaxY = -FLT_MAX; m_fMaxZ = -FLT_MAX;

    for (svt_vertex_array::iterator it=begin(); it!=end(); ++it)
    {
        if (it->pkt[0] < m_fMinX)
            m_fMinX = it->pkt[0];
        else if (it->pkt[0] > m_fMaxX)
            m_fMaxX = it->pkt[0];
        if (it->pkt[1] < m_fMinY)
            m_fMinY = it->pkt[1];
        else if (it->pkt[1] > m_fMaxY)
            m_fMaxY = it->pkt[1];
        if (it->pkt[2] < m_fMinZ)
            m_fMinZ = it->pkt[2];
        else if (it->pkt[2] > m_fMaxZ)
            m_fMaxZ = it->pkt[2];
    }
}

float svt_vertex_array::getMinX()
{
    return m_fMinX;
}
float svt_vertex_array::getMinY()
{
    return m_fMinY;
}
float svt_vertex_array::getMinZ()
{
    return m_fMinZ;
}
float svt_vertex_array::getMaxX()
{
    return m_fMaxX;
}
float svt_vertex_array::getMaxY()
{
    return m_fMaxY;
}
float svt_vertex_array::getMaxZ()
{
    return m_fMaxZ;
}
void svt_vertex_array::setMinX(float t)
{
    m_fMinX = t;
}
void svt_vertex_array::setMinY(float t)
{
    m_fMinY = t;
}
void svt_vertex_array::setMinZ(float t)
{
    m_fMinZ = t;
}
void svt_vertex_array::setMaxX(float t)
{
    m_fMaxX = t;
}
void svt_vertex_array::setMaxY(float t)
{
    m_fMaxY = t;
}
void svt_vertex_array::setMaxZ(float t)
{
    m_fMaxZ = t;
}

bool svt_vertex_array::getCompareFlag()
{
    return m_bCompareVerFlag;
}

void svt_vertex_array::setCompareFlag(bool b)
{
    m_bCompareVerFlag = b;
}

void svt_vertex_array::getNeighbour(svt_vector3<double>& pkt, double eps, list<int>& neighList)
{
    map<svt_vector_map, int>::iterator it, it2;
    unsigned int i;
    svt_vector3<double> cmpPkt;

    if (m_vertex_map.empty())
    {
        // map muss noch angelegt werden
        for (i = 0; i < size(); i++)
            m_vertex_map.insert(pair<svt_vector_map, int>((*this)[i].pkt, i));
    }

    if ((it = m_vertex_map.find(pkt)) != m_vertex_map.end())
    {
        // untersuchung der "groesseren" pkte
        it2 = it; ++it2;
        cmpPkt = pkt + eps;
        while ((it2 != m_vertex_map.end()) && (it2->first[0] < cmpPkt[0]))
        {
            // dies ist wirklich eine schaerfer bedingung als die obere
            // die obere muss aber so sein, damit weit genug in map gegangen wird  
            if ((it2->first[1] < cmpPkt[1]) && (it2->first[1] < cmpPkt[2]))
                // hier wird nun wirklich die entfernung berechnet
                if (pkt.distance(it2->first) < eps)
                    neighList.push_back(it2->second);
            ++it2;
        }
        // untersuchung der "kleineren" pkte
        if (it != m_vertex_map.begin())
        {
            it2 = it; --it2;
            cmpPkt = pkt - eps;
            while (it2->first[0] > cmpPkt[0])
            {
                // dies ist wirklich eine schaerfer bedingung als die obere
                // die obere muss aber so sein, damit weit genug in map gegangen wird  
                if ((it2->first[1] > cmpPkt[1]) && (it2->first[1] > cmpPkt[2]))
                    // hier wird nun wirklich die entfernung berechnet
                    if (pkt.distance(it2->first) < eps)
                        neighList.push_back(it2->second);
                if (it2 == m_vertex_map.begin())
                    break; 
                --it2;
            }
        }
    }
    else
    {
        // sollte nie aufgerufen werden
        cerr << pkt << " not in vertex array!" << endl; 
        cerr << "May be you called normalizeData() without remapping!" << endl;
    }
}
