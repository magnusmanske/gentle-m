/** \file
	\brief Contains the CGview class, which deals with CGview export
*/
#ifndef __CGview__
#define __CGview__

#include "main.h"

/// This class manages the CGview export
class CGview
	{
	public :
 	CGview ( TVector *_v = NULL ) ; ///< Constructor
 	virtual wxString getXML () ; ///< Generates the XML for the vector v and returns it as a wxString
 	virtual bool runSettingsDialog () ; ///< Runs the settings dialog prior to the actual XML generation
 	virtual void postProcess ( wxString filename ) ; ///< Runs CGview and the image viewer, if requested
 	
 	private :
  	friend class CGdialog ;
  	virtual wxString addXMLfeatureSlot ( int dir ) ; ///< Adds the features for the given direction to the XML
  	virtual wxString getColorName ( int type ) ; ///< Returns XML item comor, based on the item type
  	virtual wxString getColorName ( TVectorItem *i ) ; ///< Returns XML item color, based on the item color used in GENtle
  	virtual bool itemOverlap ( TVectorItem &i1 , TVectorItem &i2 ) ; ///< Checks if two items overlap
  	virtual void wellform ( wxString &s ) ; ///< Fixes quotes and &amp; prior to XML generation
  	
  	TVector *v ; ///< Pointer to the TVector structure to export
  	int width , height , radius ;
  	bool useDefaultColors , runCGview , runimageapp ;
  	wxString cgviewapp , imageformat ;
  	vector <int> used_types ; ///< The item types used in this vector (internal use only)
	} ;    

#endif