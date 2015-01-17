/***************************************************************************
                          sculptor_codebook_displacement.h  -  description
                             -------------------
    begin                : Friday Jan 12 2007
    author               : Mirabela Rusu
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SCULPTOR_CODEBOOK_DISPLACEMENT_H
#define SCULPTOR_CODEBOOK_DISPLACEMENT_H

#include<svt_basics.h>
#include<svt_node.h>
#include<svt_container.h>
#include<svt_point_cloud.h>
#include<sculptor_document.h>


/**
 * Display the divergence/displacement between two sets of codebook vectors
 */
class DLLEXPORT sculptor_codebook_displacement : public svt_node  
{
    protected:
	svt_container* m_pCont;
        
        //documents 
        sculptor_document *m_pTailDoc;
        sculptor_document *m_pHeadDoc;
        
        svt_node * m_pTopNode; 
        
        svt_vector4<Real64> m_oOrigin;
	
	// the point clouds containing the tails and the heads of the arrows
	svt_point_cloud_pdb< svt_vector4<Real64> > m_pTailCV;
	svt_point_cloud_pdb< svt_vector4<Real64> > m_pHeadCV;
        
        //the pairing of the CV as given by the rigid body docking
        vector<int> m_oMatch;
	
        //radius
	Real32 m_fSphereRadius;
	Real32 m_fCylinderRadius;
	Real32 m_fConeRadius;
	
	//Value between 0..1 that represent the percentage of the height used by the cylinder
	Real32 m_fRho;
	
	//Color RBG from 0..1
	svt_color *m_pCylinderColor;
	svt_color *m_pConeColor;
	svt_color *m_pSphereColor;
	
	//number of steps used for the drawing of cylinders and Cones; Larges the number, better the arrow looks like 
	unsigned int m_iSteps;
	
	// bool value indicating if the dimensions of the arrows are Ajunted based on the distance between the points
	// if true : the m_f...Radius will not be used, the Radius will be proportional with the distance... 
	bool m_bAjustDimensions;
        
        // old transformations
        svt_matrix4<Real64> m_oOldTransTail;
        svt_matrix4<Real64> m_oOldTransHead;
        
    public:
    
	/**
	 * Constructor
	 */
	sculptor_codebook_displacement(sculptor_document *pTailDoc, sculptor_document *pHeadDoc, svt_vector4<Real64> oOrigin, svt_node* pTopNode,
				       Real32 fSphereRadius = 1.8f, Real32 fCylinderRadius = 1.5f, Real32 fConeRadius = 3.0f, Real32 fRho = 0.75f);
	
	/**
	 * Destructor
	 */
	~sculptor_codebook_displacement();
	
	//
	// Set Parameters
	//
        
        /**
         *set the Tail Document
         */
        void setTail(sculptor_document *pTailDoc);
        
        /**
         *set the Head Document
         */
        void setHead(sculptor_document *pHeadDoc);

	/**
 	 * set origin
 	 */
	void setOrigin (svt_vector4<Real64> oOrigin);
	
        /**
         *get the Tail Document
         */
        sculptor_document* getTail(){return m_pTailDoc;}
        
        /**
         *get the Head Document
         */
        sculptor_document* getHead(){return m_pHeadDoc;}
	
	/**
	 * set CylinderRadius
	 */
	void setCylinderRadius(Real32 fCylinderRadius = 1.0f){m_fCylinderRadius = fCylinderRadius;};
	
	/**
	 * set SphereRadius
	 */
	void setSphereRadius(Real32 fSphereRadius = 1.5f){m_fSphereRadius = fSphereRadius;};
	
	/**
	 * set ConeRadius
	 */
	void setConeRadius(Real32 fConeRadius = 3.0f){m_fConeRadius = fConeRadius;};
	
	/**
	 * set lenght proportion : how long is the cylinder from the full lenght of the arrows 
	 */
	void setRho(Real32 fRho = 0.75f){m_fRho = fRho;}
	
	/**
	 * set color 
	 */
	void setColor(svt_color *pCol = new svt_color(1,1,1) ){ setCylinderColor(pCol); setConeColor(pCol); setSphereColor(pCol);};
	
	/**
	 * set color 
	 */
	void setCylinderColor(svt_color *pCol){m_pCylinderColor = pCol;};
	
	/**
	 * set color 
	 */
	void setConeColor(svt_color *pCol){m_pConeColor = pCol;};
	
	/**
	 * set color 
	 */
	void setSphereColor(svt_color *pCol){m_pSphereColor = pCol;};
	
	/**
	 * set Ajust Dimension
	 * \param bAjustDimension bool value indicating if the dimensions of the arrows are Ajunted based on the distance between the points
	 * if true : the m_f...Radius will not be used, the Radius will be proportional with the distance... 
	 */
	void setAjustDimensions(bool bAjustDimension){m_bAjustDimensions = bAjustDimension;}

	/**
	 * Add Geometrical object: Arrows (Sphere, Cylinder and Closed Cone) for each pair of vectors 
	 */
	void addArrows();
	
	/**
	 * DrawGL
	 */
	void drawGL();
	
};

#endif

