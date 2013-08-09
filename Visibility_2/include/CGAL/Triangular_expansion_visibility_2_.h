// Copyright (c) 2013 Technical University Braunschweig (Germany).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
//
// Author(s):  Michael Hemmer <michael.hemmer@cgal.org>
//             

#ifndef CGAL_TRIANGULAR_EXPANSION_VISIBILITY_2_H
#define CGAL_TRIANGULAR_EXPANSION_VISIBILITY_2_H

#include <CGAL/Arrangement_2.h>
#include <boost/shared_ptr.hpp>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>

namespace CGAL {

template<class Arrangement_2,class RegularizationTag> 
class Triangular_expansion_visibility_2 {
  typedef typename Arrangement_2::Geometry_traits_2     Geometry_traits_2;
  typedef typename Geometry_traits_2::Kernel            K;
public:
  // Currently only consider with same type for both
  typedef Arrangement_2			                Input_arrangement_2;
  typedef Arrangement_2					Output_arrangement_2;
  typedef typename Arrangement_2::Halfedge_const_handle Halfedge_const_handle;
  typedef typename Arrangement_2::Halfedge_handle       Halfedge_handle;
  typedef typename Arrangement_2::Ccb_halfedge_const_circulator
                                                  Ccb_halfedge_const_circulator;
  typedef typename Arrangement_2::Face_const_handle     Face_const_handle;

  typedef typename K::Point_2           Point_2;
  typedef typename Geometry_traits_2::Ray_2             Ray_2;
  typedef typename Geometry_traits_2::Segment_2         Segment_2;
  typedef typename Geometry_traits_2::Line_2            Line_2;
  typedef typename Geometry_traits_2::Vector_2          Vector_2;
  typedef typename Geometry_traits_2::Direction_2       Direction_2;
  typedef typename Geometry_traits_2::FT                Number_type;
  typedef typename Geometry_traits_2::Object_2          Object_2;

  typedef RegularizationTag                       Regularization_tag;
  typedef CGAL::Tag_true                          Supports_general_polygon_tag;
  typedef CGAL::Tag_true                          Supports_simple_polygon_tag;    

private:
  typedef CGAL::Triangulation_vertex_base_2<K>                     Vb;
  typedef CGAL::Constrained_triangulation_face_base_2<K>           Fb;
  typedef CGAL::Triangulation_data_structure_2<Vb,Fb>              TDS;
  typedef CGAL::No_intersection_tag                                Itag;
  typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag> CDT;
  

private:
  const Input_arrangement_2* p_arr;
  boost::shared_ptr<CDT> p_cdt; 

public: 
  Triangular_expansion_visibility_2() : p_arr(NULL){}

  /*! Constructor given an arrangement and the Regularization tag. */
  Triangular_expansion_visibility_2 (const Input_arrangement_2 &arr)
    : p_arr(&arr){
    init_cdt(); 
  }

  bool is_attached() {
    return (p_arr != NULL);
  }

  void attach(const Input_arrangement_2 &arr) {
    p_arr = &arr;
    init_cdt(); 
  }

  void detach() {
    p_arr = NULL; 
    p_cdt = boost::shared_ptr<CDT>();
  }

  const Input_arrangement_2& arr() {
    return *p_arr;
  }

  typename CDT::Edge get_edge(typename CDT::Face_handle fh, int i){
    return std::make_pair(fh,i);
  }

  Point_2 ray_seg_intersection(
      const Point_2& q, const Point_2& b, // the ray 
      const Point_2& s, const Point_2& t) // the segment 
  {
    Ray_2 ray(q,b);
    Segment_2 seg(s,t);
    CGAL::Object obj = typename K::Intersect_2()(ray,seg); 
    return object_cast<Point_2>(obj);
  }

  
  template<class OIT> 
  OIT expand_edge(
      const Point_2& q, 
      const Point_2& left, 
      const Point_2& right, 
      typename CDT::Face_handle fh, 
      int index, 
      OIT oit){
    
    // go into the new face  
    typename CDT::Face_handle nfh = fh->neighbor(index); 
    
    // get indices of neighbors 
    int nindex = nfh->index(fh); // index of new vertex and old face 
    int rindex = p_cdt->ccw(nindex); // index of face behind right edge 
    int lindex = p_cdt-> cw(nindex); // index of face behind left edge 
    
    // get vertices seen from entering edge 
    typename CDT::Vertex_handle nvh = nfh->vertex(nindex);
    typename CDT::Vertex_handle rvh = nfh->vertex(p_cdt->cw (nindex));
    typename CDT::Vertex_handle lvh = nfh->vertex(p_cdt->ccw(nindex));
    
    // get edges seen from entering edge 
    typename CDT::Edge re = get_edge(nfh,p_cdt->ccw(nindex));
    typename CDT::Edge le = get_edge(nfh,p_cdt-> cw(nindex));
    
    // do orientation computation once for new vertex 
    typename K::Orientation_2 orientation = 
      p_cdt->geom_traits().orientation_2_object();
    CGAL::Orientation ro = orientation(q,right,nvh->point());
    CGAL::Orientation lo = orientation(q,left ,nvh->point());
    
    std::cout << q << std::endl 
              << right << std::endl 
              << left << std::endl  
              << nvh->point() << std::endl  ;

    std::cout << (ro == CGAL::COUNTERCLOCKWISE) << " " << (lo == CGAL::CLOCKWISE) << std::endl; 
    
    //right edge is seen if new vertex is counter clockwise of right boarder 
    if(ro == CGAL::COUNTERCLOCKWISE){
      if(p_cdt->is_constrained(re)){
        // the edge is constrained
        // report intersection with right boarder ray 
        // if it is not already the right vertex (already reported)
        if(right != rvh->point()){
          *oit++ =
            ray_seg_intersection(q,right,nvh->point(),rvh->point());
        }
        
        // then report intersection with left boarder if it exists
        if(lo == CGAL::COUNTERCLOCKWISE){
          *oit++ = 
            ray_seg_intersection(q,left,nvh->point(),rvh->point());
        }
      }else{
        // the edge is not a constrained 
        if(lo == CGAL::COUNTERCLOCKWISE){
          // no split needed and return 
          return expand_edge(q,left,right,nfh,rindex,oit);
        }else{
          // spliting at new vertex 
          oit = expand_edge(q,nvh->point(),right,nfh,rindex,oit);
        }        
      }
    }
    
    // determin whether new vertex needs to be reported 
    if(ro != CGAL::CLOCKWISE && lo != CGAL::COUNTERCLOCKWISE){
      *oit++ = nvh->point(); 
    }
    
    //left edge is seen if new vertex is clockwise of left boarder 
    if(lo == CGAL::CLOCKWISE){
      if(p_cdt->is_constrained(le)){
        // the edge is constrained
        // report interesection with right boarder if exists 
        if(ro == CGAL::CLOCKWISE){
          *oit++ = ray_seg_intersection(q,right,nvh->point(),lvh->point());
        }
        // then report intersection with left boarder ray 
        // if it is not already the left vertex (already reported)
        if(left != lvh->point())
          *oit++ =
            ray_seg_intersection(q,left,nvh->point(),lvh->point());
        return oit; 
      }else{
        // the edge is not a constrained 
        if(ro == CGAL::CLOCKWISE){
          // no split needed and return
          return expand_edge(q,left,right,nfh,lindex,oit);
        }else{
          // spliting at new vertex 
          return expand_edge(q,left,nvh->point(),nfh,rindex,oit);
        }        
      }
    }
    
    // never reached ;)    
    assert(false);
    
  }

  Face_const_handle visibility_region(Point_2 &q,
      const Face_const_handle face,
      Output_arrangement_2 &out_arr
  ) {
    std::cout <<" ======================== "<< std::endl ;
    std::vector<Point_2> raw_output; 
    typename CDT::Face_handle cdt_face = p_cdt->locate(q);
     
    raw_output.push_back(cdt_face->vertex(1)->point());
    if(!p_cdt->is_constrained(get_edge(cdt_face,0))){
      std::cout << "edge 0 is not constrained" << std::endl;
      expand_edge(
          q,
          cdt_face->vertex(2)->point(),
          cdt_face->vertex(1)->point(),
          cdt_face,0,std::back_inserter(raw_output));
    }

    raw_output.push_back(cdt_face->vertex(2)->point());
    if(!p_cdt->is_constrained(get_edge(cdt_face,1))){
      std::cout << "edge 1 is not constrained" << std::endl;
      expand_edge(
          q,
          cdt_face->vertex(0)->point(),
          cdt_face->vertex(2)->point(),
          cdt_face,1,std::back_inserter(raw_output));
    }
    
    raw_output.push_back(cdt_face->vertex(0)->point());
    if(!p_cdt->is_constrained(get_edge(cdt_face,2))){
      std::cout << "edge 2 is not constrained" << std::endl;
      expand_edge(
          q,
          cdt_face->vertex(1)->point(),
          cdt_face->vertex(0)->point(),
          cdt_face,2,std::back_inserter(raw_output));
    }

    std::cout << raw_output.size() << std::endl; 
    
    // TODO: handle needles and report arr at same time 
    std::vector<Segment_2> segments; 
    for(int i = 0; i <raw_output.size();i++){
      std::cout << raw_output[i] << std::endl; 
      segments.push_back(Segment_2(raw_output[i],raw_output[(i+1)%raw_output.size()]));
    }
    // use something more clever 
    CGAL::insert(out_arr,segments.begin(),segments.end());

    std::cout << out_arr.number_of_faces() << std::endl; 
    assert(out_arr.number_of_faces()== 2);
    
  }

  Face_const_handle visibility_region(const Point_2 &q,
      const Halfedge_const_handle he,
      Output_arrangement_2 &out_arr
  ) {

  }

  
  void init_cdt(){
    std::cout << "init_cdt" <<std::endl;
    //todo, avoid copy by using modified iterator 
    std::vector<std::pair<Point_2,Point_2> > constraints; 
    for(typename Input_arrangement_2::Edge_const_iterator eit = p_arr->edges_begin();
        eit != p_arr->edges_end(); eit++){
      Point_2 source = eit->source()->point(); 
      Point_2 target = eit->target()->point(); 
      std::cout << source << " -- " << target << std::endl; 
      constraints.push_back(std::make_pair(source,target));
    }      
    p_cdt = boost::shared_ptr<CDT>(new CDT(constraints.begin(),constraints.end()));
  }
};

} // namespace CGAL

#endif
