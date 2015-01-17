/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/


void TransList::addTransform()
{
    char pName[256];
    
    sprintf( pName, "%s %i", m_pDoc->getName(), m_pList->count() );
    
    if (m_pDoc != NULL)
    {
	if ( m_pDoc->addTransform(pName) )
	{
	    m_pList->insertItem(pName);
	    m_pList->setCurrentItem( m_pList->count()-1 );
	}
    }
}


void TransList::removeTransform()
{
    if (m_pList->count() == 0)
	return;
    
    if (m_pDoc != NULL)
    {
	m_pDoc->removeTransform( m_pList->currentItem() );    
	m_pList->removeItem( m_pList->currentItem() );
    }
}


void TransList::refine()
{
    if (m_pDoc != NULL)
	m_pDoc->refineTransform( m_pList->currentItem() );    
}


void TransList::setAsDocked()
{

}

void TransList::applyTransform()
{
    if (m_pDoc != NULL)
	m_pDoc->applyTransform( m_pList->currentItem() );    
}


void TransList::renameTransform()
{
    bool ok;
    unsigned int iItem = m_pList->currentItem();
    QString oAlt = m_pList->text( iItem );
    QString oText = QInputDialog::getText(
            "Sculptor", "Enter new name:", QLineEdit::Normal,
            oAlt, &ok, this );
    if ( ok && !oText.isEmpty() ) 
    {
	m_pDoc->setTransformName( m_pList->currentItem(), oText );
	m_pList->changeItem( oText, m_pList->currentItem() );
    }
}

void TransList::upTransform()
{
   if (m_pDoc != NULL && m_pList->currentItem() > 0)
    {
       unsigned int iItem = m_pList->currentItem();
       QString oTmp = m_pList->text( iItem );
       m_pList->removeItem( iItem );
       m_pList->insertItem( oTmp, iItem-1 );
       m_pList->setCurrentItem( iItem-1 ); 
       
       m_pDoc->transformUp( iItem );
   }
}


void TransList::downTransform()
{
   if (m_pDoc != NULL && m_pList->currentItem() < (int)(m_pList->count())-1)
    {
       unsigned int iItem = m_pList->currentItem();
       QString oTmp = m_pList->text( iItem );
       m_pList->removeItem( iItem );
       m_pList->insertItem( oTmp, iItem+1 );
       m_pList->setCurrentItem( iItem+1 ); 
       
       m_pDoc->transformDown( iItem );
   }
}


void TransList::clearTransforms()
{
   if (m_pDoc != NULL)
    {
       m_pDoc->removeAllTransforms();
       m_pList->clear();
   }
}
