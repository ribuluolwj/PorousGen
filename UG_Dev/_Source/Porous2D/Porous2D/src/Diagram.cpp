#include "../include/Voronoi/Diagram.h"
#include "../include/Voronoi/VoronoiDiagramGenerator.h"
#include "../include/Voronoi/Epsilon.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "../include/UG-Dev/uf_all.h"
#include "../include/Voronoi/Voronoi_all.h"
//#include "../include/GlobalParameters.h"
using namespace std;
using std::cout;
using std::endl;
Diagram::~Diagram() {
	for (Cell* c : cells) {
		freeVecMemory(c->halfEdges);
	}
}

Point2* Diagram::createVertex(double x, double y) {
	Point2* vert = vertexPool.newElement(Point2(x, y));
	tmpVertices.insert(vert);

	return vert;
}

Cell* Diagram::createCell(Point2 site) {
	Cell* cell = cellPool.newElement(site);
	tmpCells.insert(cell);

	return cell;
}

VoronoiEdge* Diagram::createEdge(Site* lSite, Site* rSite, Point2* vertA, Point2* vertB) {
	VoronoiEdge* edge = edgePool.newElement(VoronoiEdge(lSite, rSite));
	tmpEdges.insert(edge);

	if (vertA) edge->setStartPoint(lSite, rSite, vertA);
	if (vertB) edge->setEndPoint(lSite, rSite, vertB);

	lSite->cell->halfEdges.push_back(halfEdgePool.newElement(edge, lSite, rSite));
	rSite->cell->halfEdges.push_back(halfEdgePool.newElement(edge, rSite, lSite));

	return edge;
}

VoronoiEdge* Diagram::createBorderEdge(Site* lSite, Point2* vertA, Point2* vertB) {
	VoronoiEdge* edge = edgePool.newElement(VoronoiEdge(lSite, nullptr, vertA, vertB));
	tmpEdges.insert(edge);

	return edge;
}

// connect dangling edges (not if a cursory test tells us
// it is not going to be visible.
// return value:
//   false: the dangling endpoint couldn't be connected
//   true: the dangling endpoint could be connected
bool Diagram::connectEdge(VoronoiEdge* edge, BoundingBox bbox) {
	// skip if end point already connected
	Point2* va = edge->vertA;
	Point2* vb = edge->vertB;
	if (vb) { return true; }

	// make local copy for speed
	Site* lSite = edge->lSite;
	Site* rSite = edge->rSite;
	double lx = lSite->p.x;
	double ly = lSite->p.y;
	double rx = rSite->p.x;
	double ry = rSite->p.y;
	double fx = (lx + rx) / 2.0;
	double fy = (ly + ry) / 2.0;
	double fm, fb;

	// if we reach here, this means cells which use this edge will need
	// to be closed, whether because the edge was removed, or because it
	// was connected to the bounding box.
	lSite->cell->closeMe = true;
	rSite->cell->closeMe = true;

	// get the line equation of the bisector if line is not vertical
	if (ry != ly) {
		fm = (lx - rx) / (ry - ly);
		fb = fy - fm*fx;
	}

	// remember, direction of line (relative to left site):
	// upward: left.x < right.x
	// downward: left.x > right.x
	// horizontal: left.x == right.x
	// upward: left.x < right.x
	// rightward: left.y < right.y
	// leftward: left.y > right.y
	// vertical: left.y == right.y

	// depending on the direction, find the best side of the
	// bounding box to use to determine a reasonable start point
	// While at it, since we have the values which define the line,
	// clip the end of va if it is outside the bbox.

	// special case: vertical line
	if (ry == ly) {
		// doesn't intersect with viewport
		if (fx < bbox.xL || fx >= bbox.xR) { return false;}
		// downward
		if (lx > rx) {
			if (!va || va->y < bbox.yT) {
				va = createVertex(fx, bbox.yT);
			}
			else if (va->y >= bbox.yB) {
				return false;
			}
			vb = createVertex(fx, bbox.yB);
		}
		// upward
		else {
			if (!va || va->y > bbox.yB) {
				va = createVertex(fx, bbox.yB);
			}
			else if (va->y < bbox.yT) {
				return false;
			}
			vb = createVertex(fx, bbox.yT);
		}
	}
	// closer to vertical than horizontal, connect start point to the
	// top or bottom side of the bounding box
	else if (fm < -1.0 || fm > 1.0) {
		// downward
		if (lx > rx) {
			if (!va || va->y < bbox.yT) {
				va = createVertex((bbox.yT - fb) / fm, bbox.yT);
			}
			else if (va->y >= bbox.yB) {
				return false;
			}
			vb = createVertex((bbox.yB - fb) / fm, bbox.yB);
		}
		// upward
		else {
			if (!va || va->y > bbox.yB) {
				va = createVertex((bbox.yB - fb) / fm, bbox.yB);
			}
			else if (va->y < bbox.yT) {
				return false;
			}
			vb = createVertex((bbox.yT - fb) / fm, bbox.yT);
		}
	}
	// closer to horizontal than vertical, connect start point to the
	// left or right side of the bounding box
	else {
		// rightward
		if (ly < ry) {
			if (!va || va->x < bbox.xL) {
				va = createVertex(bbox.xL, fm*bbox.xL + fb);
			}
			else if (va->x >= bbox.xR) {
				return false;
			}
			vb = createVertex(bbox.xR, fm*bbox.xR + fb);
		}
		// leftward
		else {
			if (!va || va->x > bbox.xR) {
				va = createVertex(bbox.xR, fm*bbox.xR + fb);
			}
			else if (va->x < bbox.xL) {
				return false;
			}
			vb = createVertex(bbox.xL, fm*bbox.xL + fb);
		}
	}
	edge->vertA = va;
	edge->vertB = vb;

	return true;
}

// line-clipping code taken from:
//   Liang-Barsky function by Daniel White
//   http://www.skytopia.com/project/articles/compsci/clipping.html
// A bit modified to minimize code paths
bool Diagram::clipEdge(VoronoiEdge* edge, BoundingBox bbox, int edgeNum) {
 //   time_t nowtime;  
//    nowtime = time(NULL); //获取日历时间   
//    char tmpTime[64];   
//    strftime(tmpTime,sizeof(tmpTime),"%Y.%m.%d-%H:%M:%S",localtime(&nowtime));   
//	std::string logTime = tmpTime;
//	char tmpString[256];
////	UF_UI_write_listing_window("====================================================================\n");
//	UF_UI_write_listing_window("-----------------------------clip begin-----------------------------\n");
////	UF_UI_write_listing_window("====================================================================\n");
//	sprintf(tmpString, "ClipEdges Time:%s\n",logTime.c_str());
//	UF_UI_write_listing_window(tmpString);
	double ax = edge->vertA->x;
	double ay = edge->vertA->y;
	double bx = edge->vertB->x;
	double by = edge->vertB->y;
	double t0 = 0.0;
	double t1 = 1.0;
	double dx = bx - ax;
	double dy = by - ay;
//	sprintf(tmpString, "%d\tStartPoint:(%f,%f)\tEndPoint:(%f,%f)\n",edgeNum,ax,ay,bx,by);
//	UF_UI_write_listing_window(tmpString);
//	sprintf(tmpString, "%d\tBoudingBox:(x:%f,%f; y:%f,%f)\n",edgeNum, bbox.xL, bbox.xR, bbox.yB, bbox.yT);
//	UF_UI_write_listing_window(tmpString);
	// left
//	sprintf(tmpString, "->\tClipping leftside...\n");
//	UF_UI_write_listing_window(tmpString);
	double q = ax - bbox.xL;
	if (dx == 0 && q < 0)
	{ 
//		sprintf(tmpString, "--->\tEdge is out of the box at \"Left Side\" and parallel to left boder of box.\n");
//		UF_UI_write_listing_window(tmpString);
//		sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//		UF_UI_write_listing_window(tmpString);
//		sprintf(tmpString, "--->\tError L101!\n");
//		UF_UI_write_listing_window(tmpString);
		return false;
	}
	double r = -q / dx;
	if (dx>0)
	{
//		sprintf(tmpString, "--->\tEdge is going rightwards.\n");
//		UF_UI_write_listing_window(tmpString);
		if (r > t1) 
		{ 
//			sprintf(tmpString, "--->\tEdge is out of the box at \"left Side\".\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tError L201!\n");
//			UF_UI_write_listing_window(tmpString);
			return false; 
		}
		if (r > t0) 
		{
			t0 = r;
//			sprintf(tmpString, "--->\tEdge's leftside is clipped .\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tClip L202!\n");
//			UF_UI_write_listing_window(tmpString);
		}
//		sprintf(tmpString, "--->\tNoClip L203!\n");
//		UF_UI_write_listing_window(tmpString);
	}
	else if (dx < 0)
	{
//		sprintf(tmpString, "--->\tEdge is going leftwards.\n");
//		UF_UI_write_listing_window(tmpString);
		if (r < t0)
		{
//			sprintf(tmpString, "--->\tEdge is out of the box at \"left Side\".\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tError L301!\n");
//			UF_UI_write_listing_window(tmpString);
			return false;
		}
		if (r < t1)
		{
			t1 = r;
//			sprintf(tmpString, "--->\tEdge's leftside is clipped .\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tClip L302!\n");
//			UF_UI_write_listing_window(tmpString);
		}
//		sprintf(tmpString, "--->\tNoClip L303!\n");
//		UF_UI_write_listing_window(tmpString);
	}
	// right
//	sprintf(tmpString, "->\tClipping rightside...\n");
//	UF_UI_write_listing_window(tmpString);
	q = bbox.xR - ax;
	if (dx == 0 && q<0)
	{ 
//		sprintf(tmpString, "--->\tEdge is out of the box at \"Right Side\" and parallel to left boder of box.\n");
//		UF_UI_write_listing_window(tmpString);
//		sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//		UF_UI_write_listing_window(tmpString);
//		sprintf(tmpString, "--->\tError R101!\n");
//		UF_UI_write_listing_window(tmpString);
		return false; 
	}
	r = q / dx;
	if (dx<0)
	{
//		sprintf(tmpString, "--->\tEdge is going leftwards.\n");
//		UF_UI_write_listing_window(tmpString);
		if (r>t1)
		{ 
//			sprintf(tmpString, "--->\tEdge is out of the box at \"Right Side\".\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tError R201!\n");
//			UF_UI_write_listing_window(tmpString);
			return false;
		}
		if (r>t0) 
		{ 
			t0 = r;
//			sprintf(tmpString, "--->\tEdge's rightside is clipped .\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tClip R202!\n");
//			UF_UI_write_listing_window(tmpString);
		}
//		sprintf(tmpString, "--->\tNoClip R203!\n");
//		UF_UI_write_listing_window(tmpString);
	}
	else if (dx>0) 
	{
//		sprintf(tmpString, "--->\tEdge is going rightwards.\n");
//		UF_UI_write_listing_window(tmpString);
		if (r<t0) 
		{ 
//			sprintf(tmpString, "--->\tEdge is out of the box at \"Right Side\".\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tError 3201!\n");
//			UF_UI_write_listing_window(tmpString);
			return false;
		}
		if (r<t1) 
		{ 
			t1 = r; 
//			sprintf(tmpString, "--->\tEdge's rightside is clipped .\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tClip R302!\n");
//			UF_UI_write_listing_window(tmpString);
		}
//		sprintf(tmpString, "--->\tNoClip R303!\n");
//		UF_UI_write_listing_window(tmpString);
	}
	// bottom        
//	sprintf(tmpString, "->\tClipping bottomside...\n");
//	UF_UI_write_listing_window(tmpString);
	q = ay - bbox.yB;
	if (dy == 0 && q<0)
	{  
//		sprintf(tmpString, "--->\tEdge is out of the box at \"Bottom Side\" and parallel to left boder of box.\n");
//		UF_UI_write_listing_window(tmpString);
//		sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//		UF_UI_write_listing_window(tmpString);
//		sprintf(tmpString, "--->\tError B101!\n");
//		UF_UI_write_listing_window(tmpString);
		return false;
	}
	r = -q / dy;
	if (dy>0)
	{
//		sprintf(tmpString, "--->\tEdge is going upwards.\n");
//		UF_UI_write_listing_window(tmpString);
		if (r>t1) 
		{ 
//			sprintf(tmpString, "--->\tEdge is out of the box at \"Bottom Side\".\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tError B201!\n");
//			UF_UI_write_listing_window(tmpString);
			return false; 
		}
		if (r>t0) 
		{ 
			t0 = r; 
//			sprintf(tmpString, "--->\tEdge's bottomside is clipped .\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tClip B202!\n");
//			UF_UI_write_listing_window(tmpString);
		}
//		sprintf(tmpString, "--->\tNoClip B203!\n");
//		UF_UI_write_listing_window(tmpString);
	}
	else if (dy<0)
	{
//		sprintf(tmpString, "--->\tEdge is going downwards.\n");
//		UF_UI_write_listing_window(tmpString);
		if (r<t0) 
		{ 
//			sprintf(tmpString, "--->\tEdge is out of the box at \"Bottom Side\".\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tError B301!\n");
//			UF_UI_write_listing_window(tmpString);
			return false; 
		}
		if (r<t1)
		{ 
			t1 = r; 
//			sprintf(tmpString, "--->\tEdge's bottomside is clipped .\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tClip B302!\n");
//			UF_UI_write_listing_window(tmpString);
		}
//		sprintf(tmpString, "--->\tNoClip B303!\n");
//		UF_UI_write_listing_window(tmpString);
	}
	// top
//	sprintf(tmpString, "->\tClipping topside...\n");
//	UF_UI_write_listing_window(tmpString);
	q = bbox.yT - ay;
	if (dy == 0 && q<0)
	{
//		sprintf(tmpString, "--->\tEdge is out of the box at \"Top Side\" and parallel to left boder of box.\n");
//		UF_UI_write_listing_window(tmpString);
//		sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//		UF_UI_write_listing_window(tmpString);
//		sprintf(tmpString, "--->\tError T101!\n");
//		UF_UI_write_listing_window(tmpString);
		return false;
	}
	r = q / dy;
	if (dy<0)
	{
//		sprintf(tmpString, "--->\tEdge is going downwards.\n");
//		UF_UI_write_listing_window(tmpString);
		if (r>t1) 
		{ 
//			sprintf(tmpString, "--->\tEdge is out of the box at \"Top Side\".\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tError T201!\n");
//			UF_UI_write_listing_window(tmpString);
			return false; 
		}
		if (r>t0)
		{ 
			t0 = r; 
//			sprintf(tmpString, "--->\tEdge's topside is clipped .\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tClip T202!\n");
//			UF_UI_write_listing_window(tmpString);
		}
//		sprintf(tmpString, "--->\tNoClip T203!\n");
//		UF_UI_write_listing_window(tmpString);
	}
	else if (dy>0)
	{
//		sprintf(tmpString, "--->\tEdge is going upwards.\n");
//		UF_UI_write_listing_window(tmpString);
		if (r<t0) 
		{  
//			sprintf(tmpString, "--->\tEdge is out of the box at \"Top Side\".\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tEdge is added to ToRemove events.\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tError T301!\n");
//			UF_UI_write_listing_window(tmpString);
			return false;
		}
		if (r<t1) 
		{
			t1 = r; 
//			sprintf(tmpString, "--->\tEdge's topside is clipped .\n");
//			UF_UI_write_listing_window(tmpString);
//			sprintf(tmpString, "--->\tClip T302!\n");
//			UF_UI_write_listing_window(tmpString);
		}
//		sprintf(tmpString, "--->\tNoClip T303!\n");
//		UF_UI_write_listing_window(tmpString);
	}
	// if we reach this point, Voronoi edge is within bbox
	// if t0 > 0, va needs to change
	// we need to create a new vertex rather
	// than modifying the existing one, since the existing
	// one is likely shared with at least another edge
	if (t0 > 0)
	{
		edge->vertA = createVertex(ax + t0*dx, ay + t0*dy);
		double vAx = (edge->vertA->x);
		double vAy = (edge->vertA->y);
	}
	// if t1 < 1, vb needs to change
	// we need to create a new vertex rather
	// than modifying the existing one, since the existing
	// one is likely shared with at least another edge
	if (t1 < 1)
	{
		edge->vertB = createVertex(ax + t1*dx, ay + t1*dy);
		double vBx = (edge->vertB->x);
		double vBy = (edge->vertB->y);
	}
	// va and/or vb were clipped, thus we will need to close
	// cells which use this edge.
	if (t0 > 0 || t1 < 1)
	{
		edge->lSite->cell->closeMe = true;
		edge->rSite->cell->closeMe = true;
	}
//	sprintf(tmpString, "%d\tNewStartPoint:(%f,%f)\tNewEndPoint:(%f,%f)\n",edgeNum,edge->vertA->x,edge->vertA->y,edge->vertB->x,edge->vertB->y);
//	UF_UI_write_listing_window(tmpString);

//	edgeNum++;
	return true;
}

// Connect/cut edges at bounding box
void Diagram::clipEdges(BoundingBox bbox) {
	// connect all dangling edges to bounding box
	// or get rid of them if it can't be done
	std::vector<VoronoiEdge*> toRemove;

	int edgeNum = 0;
	for(VoronoiEdge* edge : tmpEdges) {
		// edge is removed if:
		//   it is wholly outside the bounding box
		//   it is looking more like a point than a line
		if (!connectEdge(edge, bbox) || !clipEdge(edge, bbox, edgeNum) 
				|| (eq_withEpsilon(edge->vertA->x, edge->vertB->x) && eq_withEpsilon(edge->vertA->y, edge->vertB->y)))
		{
			edge->vertA = edge->vertB = nullptr;
			toRemove.push_back(edge);
		}
		edgeNum++;
	}
	for (VoronoiEdge* e : toRemove) {
		std::vector<HalfVoronoiEdge*>* halfEdges;
		size_t edgeCount;
		HalfVoronoiEdge* he;

		//remove lSite halfEdges
		halfEdges = &e->lSite->cell->halfEdges;
		edgeCount = halfEdges->size();
		while (edgeCount)
		{
			he = halfEdges->at(--edgeCount);
			if (he->edge == e)
			{
				halfEdges->erase(halfEdges->begin() + edgeCount);
				halfEdgePool.deleteElement(he);
			}
		}

		//remove rSite halfEdges
		halfEdges = &e->rSite->cell->halfEdges;
		edgeCount = halfEdges->size();
		while (edgeCount)
		{
			he = halfEdges->at(--edgeCount);
			if (he->edge == e)
			{
				halfEdges->erase(halfEdges->begin() + edgeCount);
				halfEdgePool.deleteElement(he);
			}
		}

		//remove edge
//		int n = tmpEdges.size();
//		char msg[256];
//		sprintf(msg, "halfEdges numb:%d", n);
//		uc1601(msg, 1);
		tmpEdges.erase(e);
		edgePool.deleteElement(e);
	}
}

// Close the cells.
// The cells are bound by the supplied bounding box.
// Each cell refers to its associated site, and a list
// of halfedges ordered counterclockwise.
void Diagram::closeCells(BoundingBox bbox) {
	Point2* va;
	Point2* vb;
	Point2* vz;
	VoronoiEdge* edge;
	std::vector<HalfVoronoiEdge*>* halfEdges;

	for (Cell* cell : tmpCells) {
		// prune, order halfedges counterclockwise, then add missing ones
		// required to close cells
		halfEdges = &cell->halfEdges;
		std::sort(halfEdges->begin(), halfEdges->end(), Cell::edgesCCW);
		size_t nHalfEdges = halfEdges->size();
		if (!nHalfEdges) {
			continue;
		}
		if (!cell->closeMe) {
			continue;
		}
		// find first 'unclosed' point.
		// an 'unclosed' point will be the end point of a halfedge which
		// does not match the start point of the following halfedge
		size_t iLeft = 0;
		while (iLeft < nHalfEdges) {
			va = (*halfEdges)[iLeft]->endPoint();
			vz = (*halfEdges)[(iLeft + 1) % nHalfEdges]->startPoint();
			// if end point is not equal to start point, we need to add the missing
			// halfedge(s) up to vz
			if (abs(va->x - vz->x) >= EPSILON || abs(va->y - vz->y) >= EPSILON) {
				// find entry point:
				bool foundEntryPoint = false;
				bool finished = false;
				bool lastBorderSegment = false;

				// walk downward along left side
				if (eq_withEpsilon(va->x, bbox.xL) && lt_withEpsilon(va->y, bbox.yB)) {
					foundEntryPoint = true;
					lastBorderSegment = eq_withEpsilon(vz->x, bbox.xL);
					vb = lastBorderSegment ? vz : createVertex(bbox.xL, lastBorderSegment ? vz->y : bbox.yB);
					edge = createBorderEdge(&cell->site, va, vb);
					++iLeft;
					halfEdges->insert(halfEdges->begin() + iLeft, halfEdgePool.newElement(edge, &cell->site, nullptr));
					++nHalfEdges;
					if (lastBorderSegment) { finished = true; }
					va = vb;
				}

				// walk rightward along bottom side
				if ((!finished && foundEntryPoint) || (eq_withEpsilon(va->y, bbox.yB) && lt_withEpsilon(va->x, bbox.xR))) {
					foundEntryPoint = true;
					lastBorderSegment = eq_withEpsilon(vz->y, bbox.yB);
					vb = lastBorderSegment ? vz : createVertex(lastBorderSegment ? vz->x : bbox.xR, bbox.yB);
					edge = createBorderEdge(&cell->site, va, vb);
					++iLeft;
					halfEdges->insert(halfEdges->begin() + iLeft, halfEdgePool.newElement(edge, &cell->site, nullptr));
					++nHalfEdges;
					if (lastBorderSegment) { finished = true; }
					va = vb;
				}

				// walk upward along right side
				if ((!finished && foundEntryPoint) || (eq_withEpsilon(va->x, bbox.xR) && gt_withEpsilon(va->y, bbox.yT))) {
					foundEntryPoint = true;
					lastBorderSegment = eq_withEpsilon(vz->x, bbox.xR);
					vb = lastBorderSegment ? vz : createVertex(bbox.xR, lastBorderSegment ? vz->y : bbox.yT);
					edge = createBorderEdge(&cell->site, va, vb);
					++iLeft;
					halfEdges->insert(halfEdges->begin() + iLeft, halfEdgePool.newElement(edge, &cell->site, nullptr));
					++nHalfEdges;
					if (lastBorderSegment) { finished = true; }
					va = vb;
				}

				// walk leftward along top side
				if ((!finished && foundEntryPoint) || (eq_withEpsilon(va->y, bbox.yT) && gt_withEpsilon(va->x, bbox.xL))) {
					lastBorderSegment = eq_withEpsilon(vz->y, bbox.yT);
					vb = lastBorderSegment ? vz : createVertex(lastBorderSegment ? vz->x : bbox.xL, bbox.yT);
					edge = createBorderEdge(&cell->site, va, vb);
					++iLeft;
					halfEdges->insert(halfEdges->begin() + iLeft, halfEdgePool.newElement(edge, &cell->site, nullptr));
					++nHalfEdges;
					if (lastBorderSegment) { finished = true; }
					va = vb;

					// walk downward along left side
					if (!finished) {
						lastBorderSegment = eq_withEpsilon(vz->x, bbox.xL);
						vb = lastBorderSegment ? vz : createVertex(bbox.xL, lastBorderSegment ? vz->y : bbox.yB);
						edge = createBorderEdge(&cell->site, va, vb);
						++iLeft;
						halfEdges->insert(halfEdges->begin() + iLeft, halfEdgePool.newElement(edge, &cell->site, nullptr));
						++nHalfEdges;
						if (lastBorderSegment) { finished = true; }
						va = vb;
					}

					// walk rightward along bottom side
					if (!finished) {
						lastBorderSegment = eq_withEpsilon(vz->y, bbox.yB);
						vb = lastBorderSegment ? vz : createVertex(lastBorderSegment ? vz->x : bbox.xR, bbox.yB);
						edge = createBorderEdge(&cell->site, va, vb);
						++iLeft;
						halfEdges->insert(halfEdges->begin() + iLeft, halfEdgePool.newElement(edge, &cell->site, nullptr));
						++nHalfEdges;
						if (lastBorderSegment) { finished = true; }
						va = vb;
					}

					// walk upward along right side
					if (!finished) {
						lastBorderSegment = eq_withEpsilon(vz->x, bbox.xR);
						vb = lastBorderSegment ? vz : createVertex(bbox.xR, lastBorderSegment ? vz->y : bbox.yT);
						edge = createBorderEdge(&cell->site, va, vb);
						++iLeft;
						halfEdges->insert(halfEdges->begin() + iLeft, halfEdgePool.newElement(edge, &cell->site, nullptr));
						++nHalfEdges;
					}
				}
			}
			++iLeft;
		}
		cell->closeMe = false;
	}
}

void Diagram::finalize() {
	cells.reserve(tmpCells.size());
	for (Cell* c : tmpCells) {
		cells.push_back(c);
	}
	tmpCells.clear();

	edges.reserve(tmpEdges.size());
	for (VoronoiEdge* e : tmpEdges) {
		edges.push_back(e);
	}
	tmpEdges.clear();

	vertices.reserve(tmpVertices.size());
	for (Point2* v : tmpVertices) {
		vertices.push_back(v);
	}
	tmpVertices.clear();
}

void Diagram::printDiagram() {
	if (cells.size()) {
		for (Cell* c : cells) {
			cout << c->site.p << endl;
			for (HalfVoronoiEdge* e : c->halfEdges) {
				cout << '\t' << *e->startPoint() << endl;
			}
			cout << endl;
		}
		for (VoronoiEdge* e : edges) {
			cout << *e->vertA << " -> " << *e->vertB << endl;
		}
		cout << endl;
	}
	else {
		for (Cell* c : tmpCells) {
			cout << c->site.p << endl;
			for (HalfVoronoiEdge* e : c->halfEdges) {
				Point2* pS = e->startPoint();
				Point2* pE = e->endPoint();

				cout << '\t';
				if (pS) cout << *pS;
				else cout << "null";
				cout << " -> ";
				if (pE) cout << *pE;
				else cout << "null";
				cout << endl;
			}
			cout << endl;
		}
		for (VoronoiEdge* e : tmpEdges) {
			if (e->vertA)
				cout << *e->vertA;
			else
				cout << "null";
			cout << " -> ";
			if (e->vertB)
				cout << *e->vertB;
			else
				cout << "null";
			cout << endl;
		}
		cout << endl;
	}
	cout << "=============================================" << endl;
}
//std::string  Get_Current_Date()
//{
//    time_t nowtime;  
//    nowtime = time(NULL); //获取日历时间   
//    char tmp[64];   
//    strftime(tmp,sizeof(tmp),"%Y.%m.%d-%H:%M:%S",localtime(&nowtime));   
//    return tmp;
//}
