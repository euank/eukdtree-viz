#ifndef KDTREE_H
#define KDTREE_H
#include <algorithm>
#include <iostream>
#include <vector>
#include <limits.h>
#include <limits>
#include <cmath>
#include "point.h"
#define GUI

template <typename T, typename P, typename C>
class kdTree
{
  public:
    kdTree()
    { root = NULL; };
    void insert(std::vector<P>& x, const T& min, const T& max)
    { root = insert(root,x,min,max,0); };

    void nn(T &t, P &p, double &r)
    {
      r =  std::numeric_limits<double>::infinity();
      nn(t, root, p, r);
    }

    void knn(T &t, std::vector<P> &pts, double &r, int &k)
    {
      r =  std::numeric_limits<double>::infinity();
      knn(t, root, pts, r, k);
    }


    bool empty()
    {
      return root == NULL;
    }
    void range(const T &t1, const T &t2, std::vector<P> &pts)
    {
      range(root, t1, t2, pts);
    }

    void clear()
    {
      clear(root);
    }

    std::vector<point_t> getLines()
    {
      std::vector<point_t> lines;
      getLines(root, lines);
      return lines;
    }	

    friend std::ostream &operator<<(std::ostream &s, const kdTree &rhs)
    { rhs.printTree(s, rhs.root); return s; };
    friend std::ostream &operator<<(std::ostream &s, kdTree *rhs)
    { return(s <<  (*rhs)); };

    ~kdTree()
    {
      this->clear();
    }

  private:

    struct kdnode_t {
      P data;
      T min,max;
      kdnode_t *left;
      kdnode_t *right;
      int axis; 
      kdnode_t(const P& d=P(),
          const T& iy=T(),
          const T& ix=T(),
          kdnode_t *l = NULL,
          kdnode_t *r = NULL,
          int a = 0) :\
        data(d), min(iy), max(ix),
        left(l),right(r),axis(a) {};
    };
    kdnode_t* root;
    kdnode_t* insert(kdnode_t *&t, std::vector<P>&x, const T& min, const T& max, int d)
    {
      int axis = x.empty() ? 0 : d%x[0]->dim();

      int m =0;
#ifndef GUI
      //std::cout << "depth: " << d << std::endl << "size: " << x.size() << std::endl;
#endif
      P median;
      T _min, _max;

      std::vector<P> left, right;

      typename std::vector<P>::iterator  it;

      if(x.empty()) return NULL;

      sort(x.begin(), x.end(),C(axis));

      m = x.size()/2;

      for(int i=0;i<(int)x.size();i++)
      {
#ifndef GUI
        std::cout << x[i] << std::endl;
        std::cout << "min: << " << min << " max: " << max << std::endl;
#endif
        if(i<m)
          left.push_back(x[i]);
        else if(i > m)
          right.push_back(x[i]);
        else
          median = x[m];
      }
      kdnode_t* node = new kdnode_t(median,min,max,NULL,NULL,axis);
      if(!left.empty())
      {
        _min = min;
        _max = max;
        _max[axis]=(*median)[axis];
        node->left = insert(node, left, _min, _max, d+1);
      }
      if(!right.empty())
      {
        _min = min;
        _max = max;
        _min[axis]=(*median)[axis];//	_max[axis] = max[axis] - (max[axis] - (*node->data)[axis]);
        node->right = insert(node, right, _min, _max, d+1);
      }
      return node;
    }

    void nn(T &q, kdnode_t *t,P &p,double &r)
    {
      if(t==NULL) return;
      double tmpdis  = q.distance(t->data);
      if(tmpdis < r)
      {
        r = tmpdis;
        p = t->data;
      }
      int axis = t->axis;
      if(q[axis] <= (*t->data)[axis])
      {
        nn(q,t->left,p,r);

        if(q[axis] + r > (*t->data)[axis])
          nn(q,t->right,p,r);
      }
      else
      {
        nn(q,t->right,p,r);
        if(q[axis] - r <= (*t->data)[axis])
          nn(q,t->left,p,r);
      }
    }

    void knn(T &q,kdnode_t *t, std::vector<P> &p, double &r, int k)
    {
      if(t==NULL) return;
      double dist  = q.distance(t->data);
      int axis = t->axis;
      typename std::vector<P>::iterator pit;

      if((int)p.size() < k)
      {
        if(p.empty() || (dist > q.distance(p.back())))
        {
          p.push_back(t->data);
        }
        else
        {
          for(pit = p.begin(); pit != p.end(); pit++)
          {
            if(dist < q.distance(*pit))
            {
              p.insert(pit,1,t->data);
              break;
            }
          }
        }
      }
      else
      {
        for(pit = p.begin();pit != p.end(); pit++)
        {
          if(dist < q.distance(*pit))
          {
            p.insert(pit,1,t->data);
            p.pop_back();
            break;
          }
        }
      }

      r = q.distance(p.back());
      if(q[axis] <= (*t->data)[axis])
      {
        knn(q,t->left,p,r,k);
        if((q[axis] + r) >= (*t->data)[axis])
        {
          knn(q,t->right,p,r,k);
        }
      }
      else
      {
        knn(q,t->right,p,r,k);
        if((q[axis] - r) <= (*t->data)[axis])
        {
          knn(q,t->left,p,r,k);
        }
      }
    }

    void range(kdnode_t *&node, const T& min, const T& max, std::vector<P> &_range)
    {
      if(node == NULL) return;

      int axis = node->axis;
      int axisp1 = (axis+1) % node->data->dim();

      if((((*node->data)[axis] > min[axis] )
            && ((*node->data)[axis] < max[axis]))
          && (((*node->data)[axisp1]) > min[axisp1])
          && ((*node->data)[axisp1] < max[axisp1]))
      {
        _range.push_back(node->data);
        range(node->left,min,max,_range);
        range(node->right,min,max,_range);
      }
      else
      {
        if((*node->data)[axis] < min[axis])
        {
          range(node->right,min,max,_range);
        }
        else if((*node->data)[axis] > max[axis])
        {
          range(node->left,min,max,_range);
        }
        else
        {
          range(node->left,min,max,_range);
          range(node->right,min,max,_range);
        }
      }
    }



    void getLines(kdnode_t *node, std::vector<point_t> &lines)
    {
      if(node == NULL) return;
      int axis = node->axis;
      if(axis)
      {
        lines.push_back(point_t((node->min)[(axis+1)%2],(*node->data)[axis]));
        lines.push_back(point_t((node->max)[(axis+1)%2],(*node->data)[axis]));
      }
      else
      {
        lines.push_back(point_t((*node->data)[axis],(node->min)[(axis+1)%2]));
        lines.push_back(point_t((*node->data)[axis],(node->max)[(axis+1)%2]));
      }
      getLines(node->left,lines);
      getLines(node->right,lines);
    }


    void printTree(std::ostream &s, kdnode_t *n) const
    {
      if(n == NULL) return;
      printTree(s,n->left);
      s << (*n->data) << std::endl;
      printTree(s,n->right);
    }

    void clear(kdnode_t *n)
    {
      if(n == NULL) return;
      clear(n->left);
      clear(n->right);
      delete(n);
      n = NULL;
    }

    bool cmp(P &p1, P &p2)
    {
      return p1[1] < p2[1];
    }
    bool cmp2(P &p1, P &p2)
    {
      return p1[0] < p2[0];
    }
};
#endif
