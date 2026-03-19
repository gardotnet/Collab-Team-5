#include "NodeInfoForCellSearch.h"
#include "WorldGridCell.h"

UNodeInfoForCellSearch::UNodeInfoForCellSearch()
{
	
}
UNodeInfoForCellSearch::UNodeInfoForCellSearch(TObjectPtr<AWorldGridCell> cell, TObjectPtr<UNodeInfoForCellSearch> parent,
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