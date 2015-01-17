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


void VolSelection::setSelection(int)
{
    if (m_pMinX->value() > m_pMaxX->value() )
        m_pMinX->setValue( m_pMaxX->value() - 1 );
    if (m_pMinY->value() > m_pMaxY->value() )
        m_pMinY->setValue( m_pMaxY->value() - 1 );
    if (m_pMinZ->value() > m_pMaxZ->value() )
        m_pMinZ->setValue( m_pMaxZ->value() - 1 );

    m_pSitus->setSelection(
			   m_pMinX->value(), m_pMaxX->value(),
			   m_pMinY->value(), m_pMaxY->value(),
			   m_pMinZ->value(), m_pMaxZ->value()
			  );
}


void VolSelection::setSitus( situs_document * pSitus )
{
    m_pSitus = pSitus;
}

void VolSelection::crop()
{
    m_pSitus->crop(
		   m_pMinX->value(), m_pMaxX->value(),
		   m_pMinY->value(), m_pMaxY->value(),
		   m_pMinZ->value(), m_pMaxZ->value()
		  );

    accept();
}


void VolSelection::cancelCrop()
{
    m_pSitus->cancelCrop();
}
