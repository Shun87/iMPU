#ifndef	__XMLPARSER__
#define __XMLPARSER__
#include "wordParser.h"

/************************************************************************/
/*		define and struct                                               */
/************************************************************************/

#define MAXNAMELEN		MAXWORDLENGHTH
#define MAXVALUELEN		MAXWORDLENGHTH

#define XML_HEADER		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"

typedef struct _XMLTAG_{
	struct _XMLTAG_* pChild;
	struct _XMLTAG_* pParent;
	struct _XMLTAG_* pBrother;
	char* value;
	char tagname[MAXNAMELEN];
} XMLTAG, *PXMLTAG;

/************************************************************************/
/*     XML                                                              */
/************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//parser the XML sentence 
PXMLTAG XMLParse(char *pXML);
//Destroy the XML tree
void	XMLDestroy(PXMLTAG XMLTree);

//compose the XML sentence from the XML tree, return length of pSentence, return 0 if failed.
int		XMLCompose(PXMLTAG XMLTree, char* pSentence, int totaltlen);

PXMLTAG XMLTag_Create (char *pName);			
PXMLTAG XMLTag_CreateLeaf (char *pName, char *pValue);

char*	XMLTag_GetName (PXMLTAG pXMLTag);
void	XMLTag_SetName (PXMLTAG pXMLTag, char *pName);
char*	XMLTag_GetValue (PXMLTAG pXMLTag);
void	XMLTag_SetValue (PXMLTAG pXMLTag, char *pValue);

int		XMLTag_GetSubTagNum (PXMLTAG pXMLTag);
void	XMLTag_AddSubTag (PXMLTAG pXMLTag, PXMLTAG pSubXMLTAG);

void	XMLTag_DeleteSubTag (PXMLTAG pXMLTag, PXMLTAG pSubTag);
void	XMLTag_RemoveSubTag (PXMLTAG pXMLTag, PXMLTAG pSubTag);
void	XMLTag_DeleteSubTagByIdx (PXMLTAG pXMLTag, int idx);
void	XMLTag_RemoveSubTagByIdx (PXMLTAG pXMLTag, int idx);
void	XMLTag_DeleteSubTagByName (PXMLTAG pXMLTag, char *pName);
void	XMLTag_RemoveSubTagByName (PXMLTAG pXMLTag, char *pName);

PXMLTAG XMLTag_FindSubTag (PXMLTAG pXMLTag, char *pName);
PXMLTAG XMLTag_GetSubTag (PXMLTAG pXMLTag, int idx);

#ifdef __cplusplus
}
#endif

#endif//__XMLPARSER__
