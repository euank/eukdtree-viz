#ifndef POINTT_H
#define POINTT_H

#include <algorithm>
#include <iostream>
#include <cmath>

class point_t
{
  public:
    point_t()
    {
      x = y = 0;
    };

    point_t(double x1,double y1)
    {
      x = x1; y = y1;
    };

    point_t(const point_t& rhs)
    {
      x = rhs.x;
      y = rhs.y;
    }

    double distance(point_t *&rhs)
    {
      return sqrt((x-rhs->x) * (x - rhs->x) + (y - rhs->y) * (y - rhs->y));
    }

    double getx() { return x; }
    double gety() { return y; }

    const double getcx() const { return x; }
    const double getcy() const { return y; } 

    int dim() { return 2; };

    const double &operator[](int i) const{ return (i==0) ? x : y;};
    double &operator[](int i) { return (i==0) ? x : y;};

    friend std::ostream &operator<<(std::ostream &s, const point_t &rhs)
    { s << rhs.x << ", " << rhs.y; return s; }
    friend std::ostream& operator<<(std::ostream &s, point_t *rhs)
    { return(s << (*rhs)); }

    friend std::istream& operator>>(std::istream &s, point_t &rhs)
    {	s >> rhs.x; s >> rhs.y; return s; }
    friend std::istream& operator>>(std::istream& s, point_t *rhs)
    { return(s >> (*rhs)); }

  private:
    double x;
    double y;
};

class PointAxisCompare
{
  public:
    PointAxisCompare(int inaxis = 0) :
      axis(inaxis) {};

    bool operator()(point_t *&p1,
        point_t *&p2)
    {
      if(axis == 0)
        return p1->getx() < p2->getx();
      return p1->gety() < p2->gety();
      //return p1[axis] < p2[axis];
    }

    bool operator()(const point_t* p1, const point_t* p2) const
    {
      if(axis == 0)
        return p1->getcx() < p2->getcx();
      return p1->getcy() < p2->getcy();
    }
    /*
       bool operator<(point_t *&p1, point_t *&p2)
       {
       return p1[axis] < p2[axis];
       }
       */
  private:
    int axis;
};
#endif
