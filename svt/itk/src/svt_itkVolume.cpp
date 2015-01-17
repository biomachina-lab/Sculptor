/***************************************************************************
                          svt_itkVolume
			  -------------
    begin                : 03/21/2007
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_itkVolume.h>

#define ITKPROGRESS cout << svt_trimName("svt_itk")

void svt_displayITKEventCallback(itk::Object *caller,
                             const itk::EventObject& event,
                             void *clientData)
{
    itk::ProcessObject *processObject = (itk::ProcessObject*)caller;

    svt_exception::ui()->progress( (int)(100.0 * processObject->GetProgress()) );
}
