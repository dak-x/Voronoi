#include <iostream>
#include <list>
#include <string>
#include <queue>
#include <deque>

#include <CGAL/intersections.h>
// envelope
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Exact_rational.h>
#include <CGAL/Arr_curve_data_traits_2.h>
#include <CGAL/Envelope_diagram_1.h>
#include <CGAL/envelope_2.h>
//
#include <CGAL/Point_2.h>
#include <CGAL/Line_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Segment_Delaunay_graph_Linf_traits_2.h>
#include <CGAL/Segment_Delaunay_graph_Linf_2/Bisector_Linf.h>
#include <CGAL/Segment_Delaunay_graph_site_2.h>
#include <CGAL/Polychain_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

#include "Linf2D_voronoi_traits_2.h"
#include "L2_voronoi_traits_2.h"

#include <CGAL/envelope_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>

#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/Arr_conic_traits_2.h>
#include "CGAL/L2_segment_voronoi_traits_2.h"

using namespace std;
typedef CGAL::Cartesian<double> Kernel;
struct Gt_inf
    : public CGAL::Segment_Delaunay_graph_Linf_traits_2<Kernel, CGAL::Field_with_sqrt_tag>
{
};
typedef Gt_inf::Site_2 Site_2;
typedef CGAL::SegmentDelaunayGraphLinf_2::Bisector_Linf<Gt_inf> Inf_bis;
Inf_bis bisector_linf;

typedef CGAL::Exact_predicates_exact_constructions_kernel VD_Kernel;
typedef VD_Kernel::FT Number_type;
typedef VD_Kernel::Iso_rectangle_2 Iso_rectangle_2;
typedef VD_Kernel::Point_2 VD_Point_2;
typedef VD_Kernel::Segment_2 VD_Segment_2;
typedef std::vector<VD_Point_2> Points;

typedef CGAL::Linf2D_voronoi_traits_2<VD_Kernel> VD_Traits_3;
typedef VD_Traits_3::Surface_3 VD_Surface_3;
typedef CGAL::Envelope_diagram_2<VD_Traits_3> VD_Envelope_diagram_2;

typedef CGAL::L2_voronoi_traits_2<VD_Kernel> L2_VD_Traits_3;
typedef L2_VD_Traits_3::Surface_3 L2_VD_Surface_3;
typedef CGAL::Envelope_diagram_2<L2_VD_Traits_3> L2_VD_Envelope_diagram_2;

typedef Kernel::Point_2 Point_2;
typedef Kernel::Line_2 Line_2;
typedef Kernel::Segment_2 Segment_2;
typedef Kernel::Intersect_2 Intersect_2;
typedef Kernel::Line_2 Line_2;
typedef Kernel::Ray_2 Ray_2;
typedef Kernel::Direction_2 Direction_2;
typedef Kernel::Vector_2 Vector_2;

list<Point_2> pt_list;
list<VD_Point_2> vd_pt_list;
Point_2 points[10] = {Point_2(9, 4), Point_2(10, 5), Point_2(7, 7), Point_2(4, 6), Point_2(2, 1), Point_2(9, 7), Point_2(5,8), Point_2(3,2), Point_2(2,10), Point_2(10,4)};
struct HullSegment
{
  Segment_2 e1;
  Ray_2 b;
  Segment_2 e2;
};

double L,R,D,U;
Point_2 Rgn;

struct FVDvertex
{
  Ray_2 b1;
  Point_2 p1;
  Ray_2 b2;
};

bool operator==(const FVDvertex& lhs, const FVDvertex& rhs)
{
    if(lhs.b1 == rhs.b1 && lhs.b2 == rhs.b2 && lhs.p1 == rhs.p1) return true;
    else return false;
}

bool operator==(const HullSegment& lhs, const HullSegment& rhs)
{
    if(lhs.e1 == rhs.e1 && lhs.e2 == rhs.e2 && lhs.b == rhs.b) return true;
    else return false;
}

vector<FVDvertex> removefromFVD(vector<FVDvertex> vi, FVDvertex vmax){
  vector<FVDvertex>::iterator it;
  for(it = vi.begin(); it!= vi.end(); it++)
  {
    if( *it == vmax)
    {
      vi.erase(it);
      return vi;
    }
  }
}
vector<HullSegment> removefromCircularList(vector<HullSegment> vi, HullSegment toBeRemoved)
{
  vector<HullSegment>::iterator it;
  for(it = vi.begin(); it!= vi.end(); it++)
  {
    if( *it == toBeRemoved)
    {
      vi.erase(it);
      return vi;
    }
  }
}
bool isIntersecting(HullSegment h1, HullSegment h2, Point_2* v)
{
  CGAL::cpp11::result_of<Intersect_2(Ray_2, Ray_2)>::type
      result1 = CGAL::intersection(h1.b, h2.b);
  if(result1)
  {
    const Point_2 *p1 = boost::get<Point_2>(&*result1);
    *v = *p1;
    return true;
  }
  return false;
}

double WeightOfPoint(Point_2 p1)
{
  return max(max( abs(p1.x()-L),abs(p1.x()-R) ), max( abs(p1.y()-U),abs(p1.y()-D) ));
}

FVDvertex findMaxWt(vector<FVDvertex> FVDset)
{
  FVDvertex vmax = FVDset[0];
  for(int i=1;i<FVDset.size();i++)
  {
    if(WeightOfPoint(vmax.p1)<WeightOfPoint(FVDset[i].p1))
    {
      vmax = FVDset[i];
    }
  }
  return vmax;
}

int findBisector(vector<HullSegment> circularList, FVDvertex v)
{
  int n = circularList.size();
  for(int i=0;i<n;i++)
  {
    if(circularList[i].b == v.b1 && circularList[(i+1)%n].b == v.b2)
    {
      return i;
    }
  }
  return -1;
}

pair<double,double> findslopes(Segment_2 e1, Segment_2 e2, Point_2 &intx)
{
  double dy1 = e1.direction().dy();
  double dx1 = e1.direction().dx();
  double dy2 = e2.direction().dy();
  double dx2 = e2.direction().dx();

  if(dy1 == 0 and dy2 == 0)
  {
    intx = Point_2((e1.source().x()+e2.source().x())/2, (e1.source().y()+e2.source().y())/2);
    return make_pair(0,0);
  }
  if(dx1 == 0 and dy1/dx1 > 0)
  {
    CGAL::cpp11::result_of<Intersect_2(Line_2, Line_2)>::type
      result1 = CGAL::intersection(Line_2(e1), Line_2(e2));
    const Point_2 *p1 = boost::get<Point_2>(&*result1);
    intx = *p1;
    return make_pair(1,(1+2*(dy1/dx1)));
  }
  if(dx1 == 0 and dy1/dx1 < 0)
  {
    CGAL::cpp11::result_of<Intersect_2(Line_2, Line_2)>::type
      result1 = CGAL::intersection(Line_2(e1), Line_2(e2));
    const Point_2 *p1 = boost::get<Point_2>(&*result1);
    intx = *p1;
    return make_pair(1,(-1+2*(dy1/dx1)));
  }
  if(dy1/dx1>=0 and dy2/dx2>=0)
  {
    double b1 = min(dy1/dx1, dy2/dx2);
    double b2 = max(dy1/dx1, dy2/dx2);
    CGAL::cpp11::result_of<Intersect_2(Line_2, Line_2)>::type
      result1 = CGAL::intersection(Line_2(e1), Line_2(e2));
    const Point_2 *p1 = boost::get<Point_2>(&*result1);
    intx = *p1;
    return make_pair(-1, ((b1+b2+2*b1*b2)/(b1 + b2 + 2)));
  }
  if(dy1/dx1<0 and dy2/dx2<0)
  {
    double b1 = max(dy1/dx1, dy2/dx2);
    double b2 = min(dy1/dx1, dy2/dx2);
    CGAL::cpp11::result_of<Intersect_2(Line_2, Line_2)>::type
      result1 = CGAL::intersection(Line_2(e1), Line_2(e2));
    const Point_2 *p1 = boost::get<Point_2>(&*result1);
    intx = *p1;
    return make_pair(1, ((-b1-b2+2*b1*b2)/(b1 + b2 - 2)));
  }
  if(dy1/dx1>0 and dy2/dx2<0)
  {
    double b1 = max(dy1/dx1, dy2/dx2);
    double b2 = min(dy1/dx1, dy2/dx2);
    CGAL::cpp11::result_of<Intersect_2(Line_2, Line_2)>::type
      result1 = CGAL::intersection(Line_2(e1), Line_2(e2));
    const Point_2 *p1 = boost::get<Point_2>(&*result1);
    intx = *p1;
    return make_pair( (b2-b1+2*b1*b2)/(b1+b2),(b1+b2)/(b1-b2+2) );
  }
}

Line_2 LinfBisector(Segment_2 e1, Segment_2 e2)
{
  Point_2 intx;
  if(e1.direction().dx() == 0 && e2.direction().dx() == 0)
  {
    return Line_2(1,0,-(e1.source().x() + e2.source().x())/2);
  }
  pair<double,double> slopes = findslopes(e1,e2, intx);
  Line_2 bisector1 = Line_2(intx, Vector_2(1,slopes.first));
  Line_2 bisector2 = Line_2(intx, Vector_2(1,slopes.second));

  double ycheck1 = bisector1.y_at_x(intx.x()+1);
  Point_2 pcheck1 = Point_2(intx.x()+1,ycheck1);

  double ycheck2 = bisector1.y_at_x(intx.x()-1);
  Point_2 pcheck2 = Point_2(intx.x()-1,ycheck1);

  Line_2 l1 = Line_2(e1);
  double sign1 = (l1.a()*pcheck1.x() + l1.b()*pcheck1.y() + l1.c())*(l1.a()*Rgn.x() + l1.b()*Rgn.y() + l1.c());
  Line_2 l2 = Line_2(e2);
  double sign2 = (l2.a()*pcheck1.x() + l2.b()*pcheck1.y() + l2.c())*(l2.a()*Rgn.x() + l2.b()*Rgn.y() + l2.c());

  if(sign1>=0 && sign2>=0) return bisector1;

  sign1 = (l1.a()*pcheck2.x() + l1.b()*pcheck2.y() + l1.c())*(l1.a()*Rgn.x() + l1.b()*Rgn.y() + l1.c());
  sign2 = (l2.a()*pcheck2.x() + l2.b()*pcheck2.y() + l2.c())*(l2.a()*Rgn.x() + l2.b()*Rgn.y() + l2.c());

  if(sign1>=0 && sign2>=0) return bisector1;

  return bisector2;  
}

bool PointComp(Point_2 a, Point_2 b)
{
  return a.x() > b.x();
}
void print_error_message(string s)
{
  cerr << s << endl;
  return;
}
void print_ray(Point_2 p, CGAL::Direction_2<Kernel> d)
{
  cout << "R " << p << " " << d << " \n";
}
// Plot the segment p1---p2
void print_segment(Point_2 p1, Point_2 p2)
{
  cout << "S " << p1 << " " << p2 << " \n";
}
// Plot the point p
void print_point(Point_2 p)
{
  cout << "P " << p << " \n";
}
// Plot the line a.x + b.y + c = 0
void print_line(Line_2 l)
{
  printf("L %f %f %f \n", l.a(), l.b(), l.c());
};

// to check whether a point lies in a quadrants as described in the paper
bool PointInRegion(Point_2 p, double y, double x, int a)
{
  // for quad 1
  if (a == 1)
  {
    if (p.x() >= x && p.y() >= y)
    {
      return true;
    }
    else
      return false;
  }
  // for quad 2
  if (a == 2)
  {
    if (p.x() <= x && p.y() >= y)
    {
      return true;
    }
    else
      return false;
  }
  // for quad 3
  if (a == 3)
  {
    if (p.x() <= x && p.y() <= y)
    {
      return true;
    }
    else
      return false;
  }
  // for quad 4
  if (a == 4)
  {
    if (p.x() >= x && p.y() <= y)
    {
      return true;
    }
    else
      return false;
  }
  return false;
}

// function to check whether a segment lies partially inside a region
bool intersectsRegion(Segment_2 *s, double y, double x, int a, Segment_2 *sMod)
{

  // find intx points with both the lines
  if (a == 1 || a == 3)
  {
    if ((s->source().y() - s->target().y()) / (s->source().x() - s->target().x()) > 0)
      return false;
  }
  if (a == 2 || a == 4)
  {
    if ((s->source().y() - s->target().y()) / (s->source().x() - s->target().x()) < 0)
      return false;
  }
  if (a == 1 || a == 2)
  {
    Line_2 lcheck = Line_2(*s);
    if (((lcheck.a() * x + lcheck.b() * y + lcheck.c()) / lcheck.b()) > 0)
      return false;
  }

  if (a == 3 || a == 4)
  {
    Line_2 lcheck = Line_2(*s);
    if (((lcheck.a() * x + lcheck.b() * y + lcheck.c()) / lcheck.b()) < 0)
      return false;
  }

  Line_2 lin(0, 1, -y);
  CGAL::cpp11::result_of<Intersect_2(Segment_2, Line_2)>::type
      result1 = CGAL::intersection(*s, lin);

  Line_2 lin1(1, 0, -x);
  CGAL::cpp11::result_of<Intersect_2(Segment_2, Line_2)>::type
      result2 = CGAL::intersection(*s, lin1);
  // both intersections
  if (result1 && result2)
  {
    const Point_2 *p1 = boost::get<Point_2>(&*result1);
    const Point_2 *p2 = boost::get<Point_2>(&*result2);
    (*sMod) = Segment_2(*p1, *p2);
    return true;
  }
  // else if both intxs are not there

  return false;
}

bool isInRegion(Segment_2 *s, double y, double x, int a)
{
  // for quad 1
  if (a == 1)
  {
    if (s->source().x() >= x && s->target().x() >= x && s->source().y() >= y && s->target().y() >= y)
    {
      return true;
    }
    else
      return false;
  }

  // for quad 2
  if (a == 2)
  {
    if (s->source().x() <= x && s->target().x() <= x && s->source().y() >= y && s->target().y() >= y)
    {
      return true;
    }
    else
      return false;
  }

  // for quad 3
  if (a == 3)
  {
    if (s->source().x() <= x && s->target().x() <= x && s->source().y() <= y && s->target().y() <= y)
    {
      return true;
    }
    else
      return false;
  }

  // for quad 4
  if (a == 4)
  {
    if (s->source().x() >= x && s->target().x() >= x && s->source().y() <= y && s->target().y() <= y)
    {
      return true;
    }
    else
      return false;
  }
  return false;
}
namespace envelope
{
  typedef CGAL::Cartesian<double> Kernel;
  typedef Kernel::Point_2 Point_2;
  typedef Kernel::Segment_2 Segment_2;
  typedef CGAL::Arr_segment_traits_2<Kernel> Segment_traits_2;
  typedef Segment_traits_2::Curve_2 Curve_2;
  typedef CGAL::Envelope_diagram_1<Segment_traits_2> Diagram_1;

  Diagram_1::Edge_const_handle e;
  Diagram_1::Vertex_const_handle v;

  // If envl_type := True Find upper envelope
  // If envl_type := False Find lower envelope
  void find_envelope(list<Segment_2> &Quadrant, int id, bool envl_type, list<Point_2> &envelope)
  {
    std::list<Curve_2> segment_list;
    Diagram_1 min_diag;

    for (auto it = Quadrant.begin(); it != Quadrant.end(); it++)
    {
      if (it->source() == it->target())
      {
        envelope.push_back(it->source());
        continue;
      }
      segment_list.push_back(Curve_2(it->source(), it->target()));
    }
    // COMPUTING THE RESPECTIVE ENVELOPE
    if (envl_type)
    {
      upper_envelope_x_monotone_2(segment_list.begin(), segment_list.end(), min_diag);
      cerr << "COMPUTED UPPER ENVELOPE " << id << endl;
    }
    else
    {
      lower_envelope_x_monotone_2(segment_list.begin(), segment_list.end(), min_diag);
      cerr << "COMPUTED LOWER ENVELOPE " << id << endl;
    }
    e = min_diag.leftmost();
    // OUTPUTING THE ENVELOPE POINTS
    while (e != min_diag.rightmost())
    {
      v = e->right();
      auto pt = v->point();
      envelope.push_back(pt);
      e = v->right();
    }
    min_diag.clear();
    segment_list.clear();
  }
} // namespace envelope

void option1(list<Point_2> &pt_list)
{
  if (pt_list.empty())
  {
    print_error_message(("No points are there"));
    return;
  }
  if (pt_list.size() != 2)
  {
    print_error_message(("Exactly two points should be there for L-inf bisector"));
    return;
  }
  //-- now calculate the l-inf bisector----------------------------------
  std::list<Point_2>::iterator it;
  it = pt_list.begin();
  Point_2 p = *it;
  Site_2 sp = Site_2::construct_site_2(p);
  ++it;
  Point_2 q = *it;
  Site_2 sq = Site_2::construct_site_2(q);
  Inf_bis::Polychainline pcl = bisector_linf(sp, sq);
  Inf_bis::Polychainline::Vertex_const_iterator it1 = pcl.vertices_begin();

  if (pcl.size() == 1)
  {
    Point_2 firstpt = *it1;
    CGAL::Direction_2<Kernel> incomingDir = pcl.get_incoming();
    CGAL::Direction_2<Kernel> outgoingDir = pcl.get_outgoing();
    cout << "Ray emnating from " << firstpt << " in direction " << incomingDir << " and " << outgoingDir << endl;
  }

  else if (pcl.size() == 2)
  {
    Point_2 firstpt = *it1;
    ++it1;
    Point_2 lastpt = *it1;
    cout << "Segment whose end points are " << firstpt << " and " << lastpt << endl;
  }

  else
  {
    Point_2 firstpt = *it1;
    cout << "Ray emnating from " << firstpt << endl;
    Inf_bis::Polychainline::Vertex_const_iterator it2 = it1 + 1;
    for (; it2 != pcl.vertices_end(); ++it1, ++it2)
    {
      Point_2 x1 = *it1;
      Point_2 x2 = *it2;
      cout << "Segment whose end points are " << x1 << "and " << x2 << endl;
    }
  }
}

void option2(list<Point_2> &pt_list)
{

  std::list<Point_2>::iterator it;
  for (it = pt_list.begin(); it != pt_list.end(); ++it)
  {
    vd_pt_list.push_back(VD_Point_2(it->x(), it->y()));
    print_point(*it);
  }

  if (vd_pt_list.empty())
  {
    print_error_message(("No mark selected"));
    return;
  }

  VD_Envelope_diagram_2 *m_envelope_diagram;
  m_envelope_diagram = new VD_Envelope_diagram_2();
  CGAL::lower_envelope_3(
      vd_pt_list.begin(),
      vd_pt_list.end(),
      *m_envelope_diagram);
  // The edges of voronoi diagram
  cerr << "The Voronoi diagram is" << endl;
  VD_Envelope_diagram_2::Edge_const_iterator eit;
  for (eit = m_envelope_diagram->edges_begin();
       eit != m_envelope_diagram->edges_end(); eit++)
  {
    if (eit->curve().is_segment())
    {
      // Case when VD is only a segment
      Point_2 p1(
          to_double(eit->curve().segment().source().x()),
          to_double(eit->curve().segment().source().y()));
      Point_2 p2(
          to_double(eit->curve().segment().target().x()),
          to_double(eit->curve().segment().target().y()));
      cerr << "A segment " << p1 << " and " << p2 << endl;
      print_segment(p1, p2);
    }
    else if (eit->curve().is_ray())
    {
      Point_2 p(
          to_double(eit->curve().ray().source().x()),
          to_double(eit->curve().ray().source().y()));
      CGAL::Direction_2<Kernel> d(
          to_double(eit->curve().ray().direction().dx()),
          to_double(eit->curve().ray().direction().dy()));
      cerr << "A Ray emanating from " << p << " in direction " << d << endl;
      print_ray(p, d);
    }
    else if (eit->curve().is_line())
    {
      Line_2 l(
          to_double(eit->curve().line().a()),
          to_double(eit->curve().line().b()),
          to_double(eit->curve().line().c()));
      cout << "A Line of form ax+by+c with a,b,c " << l << endl;
    }
  }
}

void option3(list<Point_2> &pt_list)
{

  std::list<Point_2>::iterator it;
  for (it = pt_list.begin(); it != pt_list.end(); ++it)
  {
    print_point(*it);
    vd_pt_list.push_back(VD_Point_2(it->x(), it->y()));
  }

  if (vd_pt_list.empty())
  {
    print_error_message(("No mark selected"));
    return;
  }
  L2_VD_Envelope_diagram_2 *m_envelope_diagram;
  m_envelope_diagram = new L2_VD_Envelope_diagram_2();
  CGAL::lower_envelope_3(
      vd_pt_list.begin(),
      vd_pt_list.end(),
      *m_envelope_diagram);

  // print the edges
  for (L2_VD_Envelope_diagram_2::Edge_const_iterator eit =
           m_envelope_diagram->edges_begin();
       eit != m_envelope_diagram->edges_end();
       eit++)
  {
    if (eit->curve().is_segment())
    {
      Point_2 p1(
          to_double(eit->curve().segment().source().x()),
          to_double(eit->curve().segment().source().y()));
      Point_2 p2(
          to_double(eit->curve().segment().target().x()),
          to_double(eit->curve().segment().target().y()));
      cerr << "Segment whose endpoints are " << p1 << " and " << p2 << endl;
      print_segment(p1, p2);
    }
    else if (eit->curve().is_ray())
    {
      Point_2 p(
          to_double(eit->curve().ray().source().x()),
          to_double(eit->curve().ray().source().y()));
      CGAL::Direction_2<Kernel> d(
          to_double(eit->curve().ray().direction().dx()),
          to_double(eit->curve().ray().direction().dy()));
      cerr << "Ray emnating from " << p << " in direction " << d << endl;
      print_ray(p, d);
    }
    else if (eit->curve().is_line())
    {
      Line_2 l(
          to_double(eit->curve().line().a()),
          to_double(eit->curve().line().b()),
          to_double(eit->curve().line().c()));
      cerr << "Line of form ax+by+c with a,b,c as " << l << endl;
      print_line(l);
    }
  }
}

void option4(list<Point_2> &pt_list)
{
  list<Segment_2> seg_list;
  // test segments
  for (int i = 0; i < 10; i += 2)
  {
    seg_list.push_back(Segment_2(points[i], points[i + 1]));
  }
  cerr << "The given segments are " << endl;
  list<Segment_2>::iterator it;
  L = min(seg_list.begin()->source().x(), seg_list.begin()->target().x());
  R = max(seg_list.begin()->source().x(), seg_list.begin()->target().x());
  D = min(seg_list.begin()->source().y(), seg_list.begin()->target().y());
  U = max(seg_list.begin()->source().y(), seg_list.begin()->target().y());

  for (it = seg_list.begin(); it != seg_list.end(); it++)
  {
    L = min(L,min(it->source().x(), it->target().x()));
    R = max(R,max(it->source().x(), it->target().x()));
    D = min(D,min(it->source().y(), it->target().y()));
    U = max(U,max(it->source().y(), it->target().y()));
  }
  for (it = seg_list.begin(); it != seg_list.end(); it++)
  {
    cerr << "Segment with end points " << it->source() << " , " << it->target() << endl;
    //print_segment(it->source(), it->target());
  }
  // Find Region R

  // find ln y coord
  double ln = max(seg_list.begin()->source().y(), seg_list.begin()->target().y());
  // find ls y coord
  double ls = min(seg_list.begin()->source().y(), seg_list.begin()->target().y());
  // find le x coord
  double le = max(seg_list.begin()->source().x(), seg_list.begin()->target().x());
  // find lw x coord
  double lw = min(seg_list.begin()->source().x(), seg_list.begin()->target().x());
  for (it = seg_list.begin(); it != seg_list.end(); it++)
  {
    ln = min(ln, max(it->source().y(), it->target().y()));
    ls = max(ls, min(it->source().y(), it->target().y()));
    le = min(le, max(it->source().x(), it->target().x()));
    lw = max(lw, min(it->source().x(), it->target().x()));
  }
  Rgn = Point_2((ln+ls)/2, (lw+le)/2);
  cerr << endl;
  cerr << "The Region R is " << endl;
  cerr << "ls = " << ls << " ln= " << ln << endl;
  cerr << "lw = " << lw << " le= " << le << endl;

  // dummy segment to store modified segment
  Segment_2 *sMod = new Segment_2(Point_2(1, 1), Point_2(1, 1));

  list<Segment_2> Quadrant1;
  list<Segment_2> Quadrant2;
  list<Segment_2> Quadrant3;
  list<Segment_2> Quadrant4;

  for (it = seg_list.begin(); it != seg_list.end(); it++)
  {
    // find segments straddling quadrant-1
    if (intersectsRegion(&(*it), ls, lw, 1, sMod))
      Quadrant1.push_back(*sMod);
    // find segments straddling quadrant-2
    if (intersectsRegion(&(*it), ls, le, 2, sMod))
      Quadrant2.push_back(*sMod);
    // find segments straddling quadrant-3
    if (intersectsRegion(&(*it), ln, le, 3, sMod))
      Quadrant3.push_back(*sMod);
    // find segments straddling quadrant-4
    if (intersectsRegion(&(*it), ln, lw, 4, sMod))
      Quadrant4.push_back(*sMod);
  }

  // print the quad-1 segments
  cerr << "Quadrant-1" << endl;
  for (it = Quadrant1.begin(); it != Quadrant1.end(); it++)
  {
    cerr << "Segment with end points " << it->source() << " , " << it->target() << endl;
    //print_segment(it->source(), it->target());
  }
  // print the quad-2 segments
  cerr << "Quadrant-2" << endl;
  for (it = Quadrant2.begin(); it != Quadrant2.end(); it++)
  {
    cerr << "Segment with end points " << it->source() << " , " << it->target() << endl;
    //print_segment(it->source(), it->target());
  }
  // print the quad-3 segments
  cerr << "Quadrant-3" << endl;
  for (it = Quadrant3.begin(); it != Quadrant3.end(); it++)
  {
    cerr << "Segment with end points " << it->source() << " , " << it->target() << endl;
    //print_segment(it->source(), it->target());
  }
  // print the quad-4 segments
  cerr << "Quadrant-4" << endl;
  for (it = Quadrant4.begin(); it != Quadrant4.end(); it++)
  {
    cerr << "Segment with end points " << it->source() << " , " << it->target() << endl;
    //print_segment(it->source(), it->target());
  }

  /// Determining the Envelopes
  {
    list<Point_2> Envelope1, Envelope2, Envelope3, Envelope4;
    envelope::find_envelope(Quadrant1, 1, true, Envelope1);
    envelope::find_envelope(Quadrant2, 2, true, Envelope2);
    envelope::find_envelope(Quadrant3, 3, false, Envelope3);
    envelope::find_envelope(Quadrant4, 4, false, Envelope4);

    if (Envelope1.empty())
      Envelope1.push_back(Point_2(lw, ls));
    if (Envelope2.empty())
      Envelope2.push_back(Point_2(le, ls));
    if (Envelope3.empty())
      Envelope3.push_back(Point_2(le, ln));
    if (Envelope4.empty())
      Envelope4.push_back(Point_2(lw, ln));

    // ====================================================================
    // circular queue implementation
    {
      list<HullSegment> FarthestHull;
      list<Point_2>::iterator it;
      for (it = Envelope1.begin(); it != Envelope1.end(); it++)
      {
        Point_2 curr_point = *it;
        HullSegment h1;
        h1.b = Ray_2(curr_point, Direction_2(Vector_2(-1, -1)));
        if (it == Envelope1.begin())
        {
          h1.e1 = Segment_2(*Envelope4.begin(), curr_point);
        }
        else
        {
          h1.e1 = Segment_2(*(std::prev(it)), *it);
        }
        if (*it == *Envelope1.rbegin())
        {
          h1.e2 = Segment_2(curr_point, *Envelope2.begin());
        }
        else
        {
          h1.e2 = Segment_2(*it, *(std::next(it)));
        }
        FarthestHull.push_back(h1);
      }
      // push envelope 2
      for (it = Envelope2.begin(); it != Envelope2.end(); it++)
      {
        Point_2 curr_point = *it;
        HullSegment h1;
        h1.b = Ray_2(curr_point, Direction_2(Vector_2(1, -1)));
        if (it == Envelope2.begin())
        {
          h1.e1 = Segment_2(*Envelope1.begin(), curr_point);
        }
        else
        {
          h1.e1 = Segment_2(*(std::prev(it)), *it);
        }
        if (*it == *Envelope2.rbegin())
        {
          h1.e2 = Segment_2(curr_point, *Envelope3.rbegin());
        }
        else
        {
          h1.e2 = Segment_2(*it, *(std::next(it)));
        }
        FarthestHull.push_back(h1);
      }

      // push envelope 3
      reverse(Envelope3.begin(), Envelope3.end());
      for (it = Envelope3.begin(); it != Envelope3.end(); it++)
      {
        Point_2 curr_point = *it;
        HullSegment h1;
        h1.b = Ray_2(curr_point, Direction_2(Vector_2(1, 1)));
        if (it == Envelope3.begin())
        {
          h1.e1 = Segment_2(*Envelope2.rbegin(), curr_point);
        }
        else
        {
          h1.e1 = Segment_2(*(std::prev(it)), *it);
        }
        if (*it == *Envelope3.rbegin())
        {
          h1.e2 = Segment_2(curr_point, *Envelope4.rbegin());
        }
        else
        {
          h1.e2 = Segment_2(*it, *(std::next(it)));
        }
        FarthestHull.push_back(h1);
      }

      // push envelope 4
      reverse(Envelope4.begin(), Envelope4.end());
      for (it = Envelope4.begin(); it != Envelope4.end(); it++)
      {
        Point_2 curr_point = *it;
        HullSegment h1;
        h1.b = Ray_2(curr_point, Direction_2(Vector_2(-1, 1)));
        if (it == Envelope4.begin())
        {
          h1.e1 = Segment_2(*Envelope3.rbegin(), curr_point);
        }
        else
        {
          h1.e1 = Segment_2(*(std::prev(it)), *it);
        }
        if (*it == *Envelope4.rbegin())
        {
          h1.e2 = Segment_2(curr_point, *Envelope1.begin());
        }
        else
        {
          h1.e2 = Segment_2(*it, *(std::next(it)));
        }
        FarthestHull.push_back(h1);
      }

      list<HullSegment>::iterator it1;
      cerr << "The farthest Hull is" << endl;

      print_line(Line_2(0, 1, -ln));
      print_line(Line_2(0, 1, -ls));
      print_line(Line_2(1, 0, -lw));
      print_line(Line_2(1, 0, -le));

      for (it1 = FarthestHull.begin(); it1 != FarthestHull.end(); it1++)
      {
        print_ray(it1->b.source(),it1->b.direction());
        print_segment(it1->e1.source(), it1->e1.target());
        print_segment(it1->e2.source(), it1->e2.target());
      }

      vector<HullSegment> circularList;
      for(it1 = FarthestHull.begin(); it1 != FarthestHull.end(); it1++)
      {
        circularList.push_back(*it1);
      }
      cerr<<"circular list created"<<endl;
      vector<FVDvertex> FVDSet;
      int n = circularList.size();
      for(int i=0;i<n;i++)
      {
        cerr<<circularList[i].b<<endl;
      }
      for(int i=0;i<n;i++)
      {
        Point_2 v;
        if(isIntersecting(circularList[i], circularList[(i+1)%n], &v))
        {
          cerr<<v<<endl;
          FVDvertex FVDv1;
          FVDv1.b1 = circularList[i].b;
          FVDv1.p1 = v;
          FVDv1.b2 = circularList[(i+1)%n].b;
          FVDSet.push_back(FVDv1);
        }
      }
      cerr<<"vertex set constructed of size:"<<FVDSet.size()<<endl;
      vector<Point_2> VoronoiDiagram;

      while(circularList.size() > 2 && FVDSet.size()>0)
      {
        FVDvertex vmax = findMaxWt(FVDSet);
        cerr<<"Found point:"<<vmax.p1<<" with max wt. from FVDset"<<endl;
        FVDSet = removefromFVD(FVDSet, vmax);
        cerr<<"removed point:"<<vmax.p1<<"from FVDset"<<endl;
        cerr<<"size of FVDset is:"<<FVDSet.size()<<endl;
        int idx = findBisector(circularList,vmax);
        if(idx == -1) continue;
        VoronoiDiagram.push_back(vmax.p1);
        HullSegment toBeRemoved1, toBeRemoved2;
        toBeRemoved1 = circularList[idx];
        toBeRemoved2 = circularList[(idx+1)%n];
        cerr<<"picked up "<<toBeRemoved1.b<<" and "<<toBeRemoved2.b<<endl;
        HullSegment h1;
        h1.e1 = circularList[idx].e1;
        h1.e2 = circularList[(idx+1)%n].e2;
        cerr<<"Calculating Linf bisector for "<<circularList[idx].e1<<" and "<<circularList[(idx+1)%n].e2<<endl;
        Line_2 unboundedBisector = LinfBisector(circularList[idx].e1, circularList[(idx+1)%n].e2);
        cerr<<"Linf bisector for above edges found"<<endl;

      CGAL::cpp11::result_of<Intersect_2(Line_2, Ray_2)>::type
      result1 = CGAL::intersection(unboundedBisector, circularList[(idx-1+n)%n].b);
      CGAL::cpp11::result_of<Intersect_2(Line_2, Ray_2)>::type
      result2 = CGAL::intersection(unboundedBisector, circularList[(idx+2)%n].b);
      if(result1 && result2)
      {
        cerr<<"intersected with both"<<endl;
        const Point_2 *firstIntx = boost::get<Point_2>(&*result1);
        cerr<<"calculated intx with first"<<endl;

        const Point_2 *secondIntx = boost::get<Point_2>(&*result1);
        cerr<<"calculated intx with second"<<endl;
        if(WeightOfPoint(*firstIntx)>WeightOfPoint(*secondIntx))
        {
          h1.b = Ray_2(*firstIntx,unboundedBisector);
          FVDvertex v1;
          v1.b1 = circularList[(idx-1+n)%n].b;
          v1.b2 = h1.b;
          v1.p1 = *firstIntx;
          FVDSet.push_back(v1);
          print_ray(h1.b.source(), h1.b.direction());
          cerr<<"first was greater pushing in set"<<endl;
        }else{
          h1.b = Ray_2(*secondIntx,unboundedBisector);
          FVDvertex v1;
          v1.b1 = h1.b;
          v1.b2 = circularList[(idx+2)%n].b;
          v1.p1 = *secondIntx;
          FVDSet.push_back(v1);
          print_ray(h1.b.source(), h1.b.direction());
          cerr<<"second was greater pushing in set"<<endl;
        }
      }
      else if(result1)
      {
          const Point_2 *firstIntx = boost::get<Point_2>(&*result1);
          h1.b = Ray_2(*firstIntx,unboundedBisector);
          FVDvertex v1;
          v1.b1 = circularList[(idx-1+n)%n].b;
          v1.b2 = h1.b;
          v1.p1 = *firstIntx;
          FVDSet.push_back(v1);
          print_ray(h1.b.source(), h1.b.direction());
          cerr<<"first was only present"<<endl;
      }
      else if(result2)
      {
          const Point_2 *secondIntx = boost::get<Point_2>(&*result1);
          h1.b = Ray_2(*secondIntx,unboundedBisector);
          FVDvertex v1;
          v1.b1 = h1.b;
          v1.b2 = circularList[(idx+2)%n].b;
          v1.p1 = *secondIntx;
          FVDSet.push_back(v1);
          print_ray(h1.b.source(), h1.b.direction());
          cerr<<"second was only present"<<endl;
      }
      else{
        cerr<<"no intersection"<<endl;
        continue;
      }
      

      circularList.insert(circularList.begin()+idx,h1);
      circularList= removefromCircularList(circularList, toBeRemoved1);
      circularList= removefromCircularList(circularList, toBeRemoved2);
      cerr<<"Removed both bisectors from the set"<<endl;
      cerr<<"size of circularList is:"<<circularList.size()<<endl;
      n--;
      }

    vector<Point_2>::iterator it2;
    for(it2= VoronoiDiagram.begin(); it2 != VoronoiDiagram.end(); it2++)
    {
      print_point(*it2);
    } 
    }

  }
}

int main()
{


  // ---------------------------------------------------------
  // Test inputs for L_inf Bisectors and VD -------------------------
  pt_list.push_back(points[0]);
  pt_list.push_back(points[1]);
  pt_list.push_back(points[2]);
  pt_list.push_back(points[3]);
  pt_list.push_back(points[4]);
  //pt_list.push_back(points[5]);

  //for( auto x: pt_list){
  //print_point(x);
  //}

  int Option;
  cerr << "DEBUG: Select a Option" << endl;
  cin >> Option;
  // L_Inf Bisector -----------------------------------------------------------------
  if (Option == 1)
  {
    option1(pt_list);
  }
  // -----------------------------------------------------------------------------------------
  // Voronoi diagram for points in Linf Space
  // ------------------------------------------------------------------------------------------------
  else if (Option == 2)
  {
    option2(pt_list);
  }
  //Voronoi Diagram in L2 space for points
  else if (Option == 3)
  {
    option3(pt_list);
  }
  else if (Option == 4)
  {
    option4(pt_list);
  }
  return 0;
}
