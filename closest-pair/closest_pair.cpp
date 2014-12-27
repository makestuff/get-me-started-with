/* 
 * Copyright (C) 2014 Chris McClelland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <cmath>

using namespace std;

// Class representing a 2D point
class Point {
	int m_x;
	int m_y;
public:
	Point(int x, int y) : m_x(x), m_y(y) { }
	Point(const Point &other) : m_x(other.m_x), m_y(other.m_y) { }
	Point &operator=(const Point &other) { m_x = other.m_x; m_y = other.m_y; return *this; }
	int getX() const { return m_x; }
	int getY() const { return m_y; }
	double getDistanceTo(const Point &other) const {
		const double dx = abs((double)other.m_x - m_x);
		const double dy = abs((double)other.m_y - m_y);
		return sqrt(dx * dx + dy * dy);
	}
	static bool predX(const Point *p1, const Point *p2) {
		return p1->getX() < p2->getX();
	}
	static bool predY(const Point *p1, const Point *p2) {
		return p1->getY() < p2->getY();
	}
};

// Define a type to avoid too much pointer madness
typedef const Point *PointPtr;

// Pretty-print for points
ostream &operator<<(ostream &os, const Point &p) {
	os << "(" << p.getX() << ", " << p.getY() << ")";
	return os;
}

// Dump vector of points to stdout
void print(const vector<PointPtr> &vec) {
	vector<PointPtr>::const_iterator i = vec.begin();
	const vector<PointPtr>::const_iterator end = vec.end();
	while ( i != end ) {
		cout << **i++ << endl;
	}
}

// Sort input vector to get X-sorted and Y-sorted vectors
void sortBothWays(
	const vector<Point> &pointVec, vector<PointPtr> &px, vector<PointPtr> &py)
{
	vector<Point>::const_iterator i = pointVec.begin();
	const vector<Point>::const_iterator end = pointVec.end();
	PointPtr p;

	// Init px & py vectors:
	px.reserve(pointVec.size()); px.clear();
	py.reserve(pointVec.size()); py.clear();
	while ( i != end ) {
		p = &*i++;
		px.push_back(p);
		py.push_back(p);
	}

	// Sort px by x and py by y:
	sort(px.begin(), px.end(), &Point::predX);
	sort(py.begin(), py.end(), &Point::predY);
}

// Brute-force closest-pair implementation, to solve the base case
pair<PointPtr, PointPtr> findClosestNaive(const PointPtr *pointArray, size_t numPoints) {
	const PointPtr *i = pointArray, *j, *a = i, *b = i + 1;
	const PointPtr *const end = pointArray + numPoints;
	const PointPtr *const last = end-1;
	double minDist = numeric_limits<double>::max(), dist;
	while ( i != last ) {
		j = i + 1;
		while ( j != end ) {
			dist = (*i)->getDistanceTo(**j);
			if ( dist < minDist ) {
				a = i;
				b = j;
				minDist = dist;
			}
			++j;
		}
		++i;
	}
	return pair<PointPtr, PointPtr>(*a, *b);
}

// Divide & conquer closest-pair implementation
pair<PointPtr, PointPtr> findClosestDNC(
	const PointPtr *px, size_t pxsize, const vector<PointPtr> &py)
{
	if ( pxsize > 8 ) {
		vector<PointPtr> qy, ry, sy;
		vector<PointPtr>::const_iterator pyi = py.begin();
		const vector<PointPtr>::const_iterator pye = py.end();
		const size_t qsize = pxsize / 2, rsize = pxsize - qsize;
		const int xmid = px[qsize]->getX();
		int x;
		pair<PointPtr, PointPtr> qp, rp, cp;
		double qpd, rpd, delta, newd;

		// Reserve enough space for the left & right partitions
		qy.reserve(qsize); ry.reserve(rsize);

		// Partition Y-sorted array
		if ( px[qsize-1]->getX() == xmid ) {
			// There is a tie, so consider y coords too. Worst case, if all points have the same x
			// coordinate, this can blow up to N^2.
			while ( pyi != pye ) {
				x = (*pyi)->getX();
				if ( x < xmid ) {
					qy.push_back(*pyi);
				} else if ( x > xmid ) {
					ry.push_back(*pyi);
				} else {
					const PointPtr *p = px + qsize;
					const PointPtr *const e = px + pxsize;
					const int y = (*pyi)->getY();
					bool inR = false;
					do {
						if ( (*p)->getY() == y ) {
							inR = true; break;
						}
						++p;
					} while ( p < e && (*p)->getX() == xmid );
					if ( inR ) {
						ry.push_back(*pyi);
					} else {
						qy.push_back(*pyi);
					}
				}
				++pyi;
			}
		} else {
			// There is not a tie, so don't bother considering y coords
			while ( pyi != pye ) {
				if ( (*pyi)->getX() < xmid ) {
					qy.push_back(*pyi);
				} else {
					ry.push_back(*pyi);
				}
				++pyi;
			}
		}

		// Find closest pair in left partition
		qp = findClosestDNC(px, qsize, qy);
		qpd = qp.first->getDistanceTo(*qp.second);

		// Find closest pair in right partition
		rp = findClosestDNC(px+qsize, rsize, ry);
		rpd = rp.first->getDistanceTo(*rp.second);

		// Compare closest pairs from both partitions to find out which is the closest
		if ( qpd < rpd ) {
			delta = qpd;
			cp = qp;
		} else {
			delta = rpd;
			cp = rp;
		}

		// Get a Y-sorted list of points whose x coord within "xmid +/- delta"
		pyi = py.begin();
		while ( pyi != pye ) {
			x = (*pyi)->getX();
			if ( x > xmid-delta && x < xmid+delta ) {
				sy.push_back(*pyi);
			}
			++pyi;
		}

		// Do a linear search to find any split pairs whose separation is less than "delta".
		const size_t sysize = sy.size();
		if ( sysize > 1 ) {
			size_t i, j;
			PointPtr p1 = 0, p2;
			for ( i = 0; i < sysize-2; ++i ) {
				for ( j = i+1; j < min(i+7, sysize); ++j ) {
					newd = sy[i]->getDistanceTo(*sy[j]);
					if ( newd < delta ) {
						delta = newd;
						p1 = sy[i];
						p2 = sy[j];
					}
				}
			}
			if ( p1 ) {
				return pair<PointPtr, PointPtr>(p1, p2);
			}
		}
		return cp;
	} else {
		return findClosestNaive(px, pxsize);
	}
}

#define RAND_POINTS

int main(void) {
	srand(7);
	vector<Point> pointVec;
	vector<PointPtr> px, py;
	#ifdef RAND_POINTS
		for ( int i = 0; i < 16384; ++i ) {
			pointVec.push_back(Point(rand(), rand()));
		}
	#else
		pointVec.push_back(Point(0, 0));
		pointVec.push_back(Point(1, 2));
		pointVec.push_back(Point(2, 5));
		pointVec.push_back(Point(3, 9));
		pointVec.push_back(Point(4, 14));
		pointVec.push_back(Point(5, 20));
		pointVec.push_back(Point(6, 27));
		pointVec.push_back(Point(7, 35));

		pointVec.push_back(Point(8, 35));
		pointVec.push_back(Point(9, 44));
		pointVec.push_back(Point(10, 54));
		pointVec.push_back(Point(11, 65));
		pointVec.push_back(Point(12, 77));
		pointVec.push_back(Point(13, 90));
		pointVec.push_back(Point(14, 104));
		pointVec.push_back(Point(15, 119));
	#endif

	// Get X-sorted and Y-sorted vectors
	sortBothWays(pointVec, px, py);

	// Try divide & conquer first
	pair<PointPtr, PointPtr> closestPair = findClosestDNC(&px[0], px.size(), py);
	cout << "Closest (DNC): " << *closestPair.first << "/" << *closestPair.second << ": " << closestPair.first->getDistanceTo(*closestPair.second) << endl;

	// Then try naive implemetation
	closestPair = findClosestNaive(&px[0], px.size());
	cout << "Closest (Naive): " << *closestPair.first << "/" << *closestPair.second << ": " << closestPair.first->getDistanceTo(*closestPair.second) << endl;

	return 0;
}
