// $Id: combshps.h 1282 2006-06-09 09:46:49Z alex $
/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
================================XARAHEADERSTART===========================
 
               Xara LX, a vector drawing and manipulation program.
                    Copyright (C) 1993-2006 Xara Group Ltd.
       Copyright on certain contributions may be held in joint with their
              respective authors. See AUTHORS file for details.

LICENSE TO USE AND MODIFY SOFTWARE
----------------------------------

This file is part of Xara LX.

Xara LX is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as published
by the Free Software Foundation.

Xara LX and its component source files are distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Xara LX (see the file GPL in the root directory of the
distribution); if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


ADDITIONAL RIGHTS
-----------------

Conditional upon your continuing compliance with the GNU General Public
License described above, Xara Group Ltd grants to you certain additional
rights. 

The additional rights are to use, modify, and distribute the software
together with the wxWidgets library, the wxXtra library, and the "CDraw"
library and any other such library that any version of Xara LX relased
by Xara Group Ltd requires in order to compile and execute, including
the static linking of that library to XaraLX. In the case of the
"CDraw" library, you may satisfy obligation under the GNU General Public
License to provide source code by providing a binary copy of the library
concerned and a copy of the license accompanying it.

Nothing in this section restricts any of the rights you have under
the GNU General Public License.


SCOPE OF LICENSE
----------------

This license applies to this program (XaraLX) and its constituent source
files only, and does not necessarily apply to other Xara products which may
in part share the same code base, and are subject to their own licensing
terms.

This license does not apply to files in the wxXtra directory, which
are built into a separate library, and are subject to the wxWindows
license contained within that directory in the file "WXXTRA-LICENSE".

This license does not apply to the binary libraries (if any) within
the "libs" directory, which are subject to a separate license contained
within that directory in the file "LIBS-LICENSE".


ARRANGEMENTS FOR CONTRIBUTION OF MODIFICATIONS
----------------------------------------------

Subject to the terms of the GNU Public License (see above), you are
free to do whatever you like with your modifications. However, you may
(at your option) wish contribute them to Xara's source tree. You can
find details of how to do this at:
  http://www.xaraxtreme.org/developers/

Prior to contributing your modifications, you will need to complete our
contributor agreement. This can be found at:
  http://www.xaraxtreme.org/developers/contribute/

Please note that Xara will not accept modifications which modify any of
the text between the start and end of this header (marked
XARAHEADERSTART and XARAHEADEREND).


MARKS
-----

Xara, Xara LX, Xara X, Xara X/Xtreme, Xara Xtreme, the Xtreme and Xara
designs are registered or unregistered trademarks, design-marks, and/or
service marks of Xara Group Ltd. All rights in these marks are reserved.


      Xara Group Ltd, Gaddesden Place, Hemel Hempstead, HP2 6EX, UK.
                        http://www.xara.com/

=================================XARAHEADEREND============================
 */
// Header for the shape combining operations

#ifndef INC_COMBSHPS
#define INC_COMBSHPS

//#include "selop.h"
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodepath.h"
#include "effects_stack.h"

class UndoableOperaton;
class NodeRenderableInk;
class CCAttrMap;

#define OPTOKEN_COMBINESHAPES	_T("CombineShapes")

#define OPTOKEN_ADDSHAPES		_T("AddShapes")
#define OPTOKEN_SUBTRACTSHAPES	_T("SubtractShapes")
#define OPTOKEN_INTERSECTSHAPES	_T("IntersectShapes")
#define OPTOKEN_SLICESHAPES		_T("SliceShapes")

enum CombineReason { 	COMBINE_NONE, 
						COMBINE_ADD, 
						COMBINE_SUBTRACT, 
						COMBINE_INTERSECT, 
						COMBINE_SLICE 
					};

enum ClipStyle 		{ 	CLIP_STYLE_ADD, 
						CLIP_STYLE_SUBTRACT, 
						CLIP_STYLE_INTERSECT
					};

/********************************************************************************************

>	class SelObjPathListItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Purpose:	This class encapsulates all the data needed for each path generated by the objects
				that are being combined.
				A list of all the paths that each of the selected object generates is made, and
				each path generated creates an instance of this class

********************************************************************************************/

class SelObjPathListItem : public ListItem
{
	CC_DECLARE_DYNCREATE(SelObjPathListItem)

public:
	SelObjPathListItem() : pNodePath(NULL), pCreatedByNode(NULL), pAttrMap(NULL), strokeCreatedPassbackPath(FALSE) {}	// For CC_IMPLEMENT_...

	SelObjPathListItem(NodePath* pThisNodePath,NodeRenderableInk* pThisCreatedByNode,CCAttrMap* pThisAttrMap, BOOL createdByStroke = FALSE);
	~SelObjPathListItem();
	
	Path*				GetPath();
	NodePath*			GetNodePath() 		{ return pNodePath; }
	NodeRenderableInk*	GetCreatedByNode() 	{ return pCreatedByNode; }
	CCAttrMap*			GetAttrMap()		{ return pAttrMap; }

	ListRange*			GetEffectStack()
	{
		if (pEffectStack==NULL && pCreatedByNode->FindParent()!=NULL)
			pEffectStack = EffectsStack::GetEffectsStackFromNode(pCreatedByNode);
		if (pEffectStack==NULL && pNodePath->FindParent()!=NULL)
			pEffectStack = EffectsStack::GetEffectsStackFromNode(pNodePath);
		return pEffectStack;
	}

	BOOL GetStrokeCreatedPassbackPath ()	{ return (strokeCreatedPassbackPath); }

private:
	NodePath*			pNodePath;
	NodeRenderableInk*	pCreatedByNode;
	CCAttrMap*			pAttrMap;
	ListRange*			pEffectStack;

	BOOL strokeCreatedPassbackPath;		// was this item created by a stroke being passed back?
};


/********************************************************************************************

>	class SelObjPathList : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Purpose:	This class encapsulates all the data needed for each selected object.

				One instance of this class is generated for each selected object.

********************************************************************************************/

class SelObjPathList : public ListItem
{
	CC_DECLARE_DYNCREATE(SelObjPathList)

public:
	SelObjPathList() : pSelNode(NULL) { ProducedPaths = FALSE; }
	SelObjPathList(Node* pThisSelNode) : pSelNode(pThisSelNode) { ProducedPaths = FALSE; }

	// Destroy the list on destruction
	~SelObjPathList() { ListOfSelObjPaths.DeleteAll(); }

	Node* GetSelNode() 			{ return pSelNode; }
	INT32  GetCount()			{ return ListOfSelObjPaths.GetCount(); }
	BOOL  HasProducedPaths()	{ return ProducedPaths; }
	
	void SetProducedPaths(BOOL state) { ProducedPaths = state; }

	SelObjPathListItem* GetFirstItem() 							{ return (SelObjPathListItem*) ListOfSelObjPaths.GetHead(); }
	SelObjPathListItem* GetLastItem() 							{ return (SelObjPathListItem*) ListOfSelObjPaths.GetTail(); }
	SelObjPathListItem* GetNextItem(SelObjPathListItem* pItem) 	{ return (SelObjPathListItem*) ListOfSelObjPaths.GetNext(pItem); }
	void				AddTailItem(SelObjPathListItem* pItem) 	{ ListOfSelObjPaths.AddTail(pItem); }

private:
	Node*	pSelNode;			// ptr to the selected object
	List 	ListOfSelObjPaths;	// List of paths generated by the object
	BOOL 	ProducedPaths;		// TRUE if any of the paths in the above list produced one or more new paths
};


/********************************************************************************************

>	class OpCombineShapes : public SelOperation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/95
	Purpose:	Base class op that does all the shape combining ops, like add, subtract and intersect,etc

********************************************************************************************/

class OpCombineShapes : public SelOperation
{
	CC_DECLARE_DYNCREATE( OpCombineShapes )   

public:
	OpCombineShapes();
	~OpCombineShapes();

	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);

	virtual void Do(OpDescriptor*);
	virtual void GetOpName(String_256*);

protected:
	CombineReason 	Reason;

private:
	BOOL ConvertToShapes(Range* pRange, BOOL bCombineEffectBitmaps);
	BOOL CreateSelObjPathLists(BOOL bCombineEffectBitmaps);
	BOOL CreateSelObjPathList(Node* pNode,
								SelObjPathList* pSelObjPathList,
								BOOL IsFirstNode = TRUE,
								BOOL bCombineEffectBitmaps = TRUE,
								BecomeAReason reason = BECOMEA_PASSBACK);

	BOOL DoAddShapes();
	BOOL AddPathsFromList(Path* pPath,SelObjPathList* pSelObjPathList);
	BOOL AddPaths(Path* pPath1,Path* pPath2);
	BOOL AddOrMergePaths(Path* pPath1, Path* pPath2);

	BOOL DoCombineShapes(ClipStyle Style, BOOL bCombineEffectBitmaps);
	BOOL CombinePathsFromList(	ClipStyle Style,
								Path* pClipPath,
								Path* pStrokedClipPath,
								SelObjPathList* pSelObjPathList,
								BOOL bCombineEffectBitmaps = TRUE);
	BOOL CombinePaths(ClipStyle Style,Path* pClipPath,Path* pStrokedClipPath,Path* pSrcPath,Path* pDestPath);

	BOOL DoHideListedNodes(BOOL bCombineEffectBitmaps);
	BOOL DoSelectResultNodes(BOOL bCombineEffectBitmaps);
	BOOL ApplyAttributes(NodeRenderableInk* pSrcNode,NodeRenderableInk* pDestNode,CCAttrMap* pAttrMap, SelObjPathListItem* pListItem = NULL);
	BOOL ApplyEffects(SelObjPathListItem* pListItem, NodeRenderableInk* pDestNode, NodeRenderableInk* pParentEffect);

	BOOL 	StrokePathToPath(Path* pPath,Path* pDest);
	BOOL 	ConvertClipLineToShape(Path* pClipPath,Path* pSrcPath,Path* pDestPath);
	DocRect GetBoundingRect(Path* pPath);

	BOOL DoInsertNewNode(NodeRenderableBounded* pNewNode);

	UINT32 AdjustAttributesForStroke(CCAttrMap* pAttrMap, SelObjPathListItem* pListItem = NULL);
//	void DeleteExtraStrokeAttributes(CCAttrMap* pAttrMap, SelObjPathListItem* pListItem = NULL);

	BOOL BeginSlowJob();

	SelObjPathList* GetFirstList() 						{ return (SelObjPathList*) ListOfSelObjPathLists.GetHead(); }
	SelObjPathList* GetLastList() 						{ return (SelObjPathList*) ListOfSelObjPathLists.GetTail(); }
	SelObjPathList* GetNextList(SelObjPathList* pItem) 	{ return (SelObjPathList*) ListOfSelObjPathLists.GetNext(pItem); }

	ListRange* CopyRange(Range* pRange);

	List 		ListOfSelObjPathLists;
	ListRange*	pListOfResults;
	DocRect		OriginalBoundingRect;
	SelRange* 	pSelRange;	// Set up at the start of Do()
	INT32		NumPaths;
	INT32		JobCount;
	INT32 		NodeInsertCount;

	Node*				pContextNode;
	AttachNodeDirection AttachDir;
};


/********************************************************************************************

>	class OpAddShapes : public OpCombineShapes

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/95
	Purpose:	Op that invokes the Add Shapes operation performed by OpCombineShapes

********************************************************************************************/

class OpAddShapes : public OpCombineShapes
{
	CC_DECLARE_DYNCREATE( OpAddShapes )   

public:
	OpAddShapes()	{ Reason = COMBINE_ADD; }
};

/********************************************************************************************

>	class OpSubtractShapes : public OpCombineShapes

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/95
	Purpose:	Op that invokes the Add Shapes operation performed by OpCombineShapes

********************************************************************************************/

class OpSubtractShapes : public OpCombineShapes
{
	CC_DECLARE_DYNCREATE( OpSubtractShapes )   

public:
	OpSubtractShapes()	{ Reason = COMBINE_SUBTRACT; }
};


/********************************************************************************************

>	class OpIntersectShapes : public OpCombineShapes

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/95
	Purpose:	Op that invokes the Add Shapes operation performed by OpCombineShapes

********************************************************************************************/

class OpIntersectShapes : public OpCombineShapes
{
	CC_DECLARE_DYNCREATE( OpIntersectShapes )   

public:
	OpIntersectShapes()	{ Reason = COMBINE_INTERSECT; }
};


/********************************************************************************************

>	class OpSliceShapes : public OpCombineShapes

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/95
	Purpose:	Op that invokes the Add Shapes operation performed by OpCombineShapes

********************************************************************************************/

class OpSliceShapes : public OpCombineShapes
{
	CC_DECLARE_DYNCREATE( OpSliceShapes )   

public:
	OpSliceShapes()	{ Reason = COMBINE_SLICE; }
};


/***********************************************************************************************

>	class CombineBecomeA: public BecomeA

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Purpose:	This is the class that is passed to other nodes when the combine op gets them to 
				to turn into other types via their DoBecomeA() method

***********************************************************************************************/

class CombineBecomeA : public BecomeA
{
CC_DECLARE_MEMDUMP(CombineBecomeA);
public:
	CombineBecomeA(BecomeAReason ThisReason, UndoableOperation* pOp, SelObjPathList* pThisSelObjPathList,Node* pThisSelNode, BOOL IsFirstNode = FALSE) :
				BecomeA(ThisReason,CC_RUNTIME_CLASS(NodePath),pOp,TRUE,IsFirstNode), pSelObjPathList(pThisSelObjPathList)
				{ strokeCreatedPassbackPath = FALSE; }

	// This function should be called when Reason == BECOMEA_PASSBACK 
	virtual BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap);

	BOOL IsCombineBecomeA() { return TRUE; }

	// CGS (13/12/2000):  allows us to flag whether the path that is being passed back was created within
	// PathProcessorStroke::DoBecomeA ().  This allows us to apply attributes correctly to objects when
	// using OpCombineShapes.  The value is used to set the corresponding value within SelObjPathListItem's,
	// so that we can control the flow of execution within OpCombineShapes::AdjustAttributesForStroke ()
	void SetStrokeCreatedPassbackPath (BOOL newVal) { strokeCreatedPassbackPath = newVal; }

private:
	SelObjPathList*	pSelObjPathList;

	BOOL strokeCreatedPassbackPath;		// is the next path being passed back being created by a stroke?
};


#endif  // INC_COMBSHPS


