// Gmsh - Copyright (C) 1997-2018 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@onelab.info>.

#ifndef _MFACE_H_
#define _MFACE_H_

#include <functional>
#include <vector>
#include "MVertex.h"
#include "MEdge.h"
#include "SVector3.h"
#include "GmshMessage.h"
#include "GmshDefines.h"

template <class t> class fullMatrix;

// A mesh face.
class MFace {
private:
  std::vector<MVertex *> _v;
  std::vector<char> _si; // sorted indices

public:
  typedef std::vector<MVertex *>::size_type size_type;

public:
  MFace() {}
  MFace(MVertex *v0, MVertex *v1, MVertex *v2, MVertex *v3 = 0);
  MFace(const std::vector<MVertex *> &v);
  size_type getNumVertices() const { return _v.size(); }
  MVertex *getVertex(const int i) const { return _v[i]; }
  MVertex *getSortedVertex(const int i) const { return _v[int(_si[i])]; }
  MEdge getEdge(const int i) const
  {
    return MEdge(getVertex(i), getVertex((i + 1) % getNumVertices()));
  }

  bool computeCorrespondence(const MFace &, int &, bool &) const;

  void getOrderedVertices(std::vector<MVertex *> &verts) const
  {
    for(size_type i = 0; i < getNumVertices(); i++)
      verts.push_back(getSortedVertex(i));
  }
  void getOrderedVertices(const MVertex **const verts) const
  {
    for(size_type i = 0; i < getNumVertices(); i++) {
      verts[i] = getSortedVertex(i);
    }
  }
  double approximateArea() const;
  SVector3 normal() const;
  SVector3 tangent(int num) const
  {
    SVector3 t0(_v[1]->x() - _v[0]->x(), _v[1]->y() - _v[0]->y(),
                _v[1]->z() - _v[0]->z());
    t0.normalize();
    if(!num) return t0;
    SVector3 n = normal();
    SVector3 t1 = crossprod(n, t0);
    return t1;
  }
  SPoint3 barycenter() const
  {
    SPoint3 p(0., 0., 0.);
    int n = getNumVertices();
    for(int i = 0; i < n; i++) {
      const MVertex *v = getVertex(i);
      p[0] += v->x();
      p[1] += v->y();
      p[2] += v->z();
    }
    p[0] /= static_cast<double>(n);
    p[1] /= static_cast<double>(n);
    p[2] /= static_cast<double>(n);
    return p;
  }
  SPoint3 interpolate(const double &u, const double &v) const
  {
    SPoint3 p(0.0, 0.0, 0.0);
    int n = getNumVertices();
    if(n == 3) {
      const double ff[3] = {1.0 - u - v, u, v};
      for(int i = 0; i < n; i++) {
        MVertex *v = getVertex(i);
        p[0] += v->x() * ff[i];
        p[1] += v->y() * ff[i];
        p[2] += v->z() * ff[i];
      }
    }
    else if(n == 4) {
      const double ff[4] = {(1 - u) * (1. - v), (1 + u) * (1. - v),
                            (1 + u) * (1. + v), (1 - u) * (1. + v)};
      for(int i = 0; i < n; i++) {
        MVertex *v = getVertex(i);
        p[0] += v->x() * ff[i] * 0.25;
        p[1] += v->y() * ff[i] * 0.25;
        p[2] += v->z() * ff[i] * 0.25;
      }
    }
    else
      Msg::Error(
        "Cannot interpolate inside a polygonal MFace with more than 4 edges");
    return p;
  }
};

inline bool operator==(const MFace &f1, const MFace &f2)
{
  if(f1.getNumVertices() != f2.getNumVertices()) return false;
  for(MFace::size_type i = 0; i < f1.getNumVertices(); i++)
    if(f1.getSortedVertex(i) != f2.getSortedVertex(i)) return false;
  return true;
}

inline bool operator!=(const MFace &f1, const MFace &f2)
{
  if(f1.getNumVertices() != f2.getNumVertices()) return true;
  for(MFace::size_type i = 0; i < f1.getNumVertices(); i++)
    if(f1.getSortedVertex(i) != f2.getSortedVertex(i)) return true;
  return false;
}

struct Equal_Face : public std::binary_function<MFace, MFace, bool> {
  bool operator()(const MFace &f1, const MFace &f2) const { return (f1 == f2); }
};

struct Less_Face : public std::binary_function<MFace, MFace, bool> {
  bool operator()(const MFace &f1, const MFace &f2) const
  {
    if(f1.getNumVertices() != f2.getNumVertices())
      return f1.getNumVertices() < f2.getNumVertices();
    for(MFace::size_type i = 0; i < f1.getNumVertices(); i++) {
      if(f1.getSortedVertex(i)->getNum() < f2.getSortedVertex(i)->getNum())
        return true;
      if(f1.getSortedVertex(i)->getNum() > f2.getSortedVertex(i)->getNum())
        return false;
    }
    return false;
  }
};

class MFaceN {
private:
  int _type;
  int _order;
  std::vector<MVertex *> _v;

public:
  typedef std::vector<MVertex *>::size_type size_type;

public:
  MFaceN() {}
  MFaceN(int type, int order, const std::vector<MVertex *> &v);

  int getPolynomialOrder() const { return _order; }
  int getType() const { return _type; }
  bool isTriangular() const { return _type == TYPE_TRI; }
  size_type getNumVertices() const { return (int)_v.size(); }
  int getNumCorners() const { return isTriangular() ? 3 : 4; }
  int getNumVerticesOnBoundary() const { return getNumCorners() * _order; }

  MVertex *getVertex(int i) const { return _v[i]; }
  const std::vector<MVertex *> &getVertices() const { return _v; }

  MEdgeN getHighOrderEdge(int num, int sign) const;
  MFace getFace() const;

  SPoint3 pnt(double u, double v) const;
  SVector3 tangent(double u, double v, int num) const;
  SVector3 normal(double u, double v) const;
  void frame(double u, double v, SVector3 &t0, SVector3 &t1, SVector3 &n) const;
  void frame(double u, double v, SPoint3 &p, SVector3 &t0, SVector3 &t1,
             SVector3 &n) const;

  void repositionInnerVertices(const fullMatrix<double> *) const;
};

#endif
