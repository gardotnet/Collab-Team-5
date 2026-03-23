#include "NodeInfoForCellSearch.h"
#include "Terminal_Recall/GridCellClasses/Harry/Nodes/TraversalNode.h"

UNodeInfoForCellSearch::UNodeInfoForCellSearch()
{
	
}
UNodeInfoForCellSearch::UNodeInfoForCellSearch(TObjectPtr<ATraversalNode> cell, TObjectPtr<UNodeInfoForCellSearch> parent,
                                               float gCost, float hCost)
{
	this->Cell = cell;
	this->ParentCell = parent;
	this->GCost = gCost;
	this->HCost = hCost;
	this->FCost = gCost + hCost;
}
void UNodeInfoForCellSearch::UpdateNodeInfo(TObjectPtr<UNodeInfoForCellSearch> parent, float gCost, float hCost)
{
	this->ParentCell = parent;
	this->GCost = gCost;
	this->HCost = hCost;
	this->FCost = gCost + hCost;
}