#ifndef _EDGE_H_
#define _EDGE_H_

#include "Point2.h"

struct Site;

struct VoronoiEdge {
	Site* lSite;
	Site* rSite;
	Point2* vertA;
	Point2* vertB;

	VoronoiEdge() : lSite(nullptr), rSite(nullptr), vertA(nullptr), vertB(nullptr) {};
	VoronoiEdge(Site* _lSite, Site* _rSite) : lSite(_lSite), rSite(_rSite), vertA(nullptr), vertB(nullptr) {};
	VoronoiEdge(Site* lS, Site* rS, Point2* vA, Point2* vB) : lSite(lS), rSite(rS), vertA(vA), vertB(vB) {};

	void setStartPoint(Site* _lSite, Site* _rSite, Point2* vertex);
	void setEndPoint(Site* _lSite, Site* _rSite, Point2* vertex);
};

struct HalfVoronoiEdge {
	Site* site;
	VoronoiEdge* edge;
	double angle;

	HalfVoronoiEdge() : site(nullptr), edge(nullptr) {};
	HalfVoronoiEdge(VoronoiEdge* e, Site* lSite, Site* rSite);

	inline Point2* startPoint();
	inline Point2* endPoint();
};

inline Point2* HalfVoronoiEdge::startPoint() {
	return (edge->lSite == site) ? edge->vertA : edge->vertB;
}

inline Point2 * HalfVoronoiEdge::endPoint() {
	return (edge->lSite == site) ? edge->vertB : edge->vertA;
}

#endif