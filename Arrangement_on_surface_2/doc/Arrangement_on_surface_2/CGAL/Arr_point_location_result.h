/*!
\ingroup PkgArrangement2PointLocation

The macro `CGAL_ARR_POINT_LOCATION_VERSION` can be used to configure
the point-location query API. In particular, it determines which version
of the result type of the point-location and vertical ray-shooting queries
should be used by models of the concepts `ArrangementPointLocation_2`
and `ArrangementVerticalRayShoot_2`, and by the free function
`locate`. The `CGAL_ARR_POINT_LOCATION_VERSION` should be defined before any \cgal header
is included.

`CGAL_ARR_POINT_LOCATION_VERSION` == 1, the result type is set to be `CGAL::Object`.
`CGAL_ARR_POINT_LOCATION_VERSION` == 2, the result type is set to be
`boost::variant<Vertex_const_handle,Halfedge_const_handle,Face_const_handle>`, where `Vertex_const_handle`, `Halfedge_const_handle`, and
`Face_const_handle` are the corresponding nested types in an `Arrangement_2` instance.


\sa `ArrangementPointLocation_2`
\sa `ArrangementVerticalRayShoot_2`
\sa `CGAL::Arr_point_location_result<Arrangement>`
*/
#define CGAL_ARR_POINT_LOCATION_VERSION

namespace CGAL {

/*!
\ingroup PkgArrangement2PointLocation

A binary metafunction to determine the return type of a point-location
or vertical ray-shoot query.

\tparam Arrangement must be an instance of the `Arrangement<Traits,Dcel>` class template.

\sa `ArrangementPointLocation_2`
\sa `ArrangementVerticalRayShoot_2`
\sa `CGAL::Arr_naive_point_location<Arrangement>`
\sa `CGAL::Arr_walk_along_line_point_location<Arrangement>`
\sa `CGAL::Arr_landmarks_point_location<Arrangement,Generator>`
\sa `CGAL::Arr_trapezoid_ric_point_location<Arrangement>`
\sa `CGAL_ARR_POINT_LOCATION_VERSION`
*/
template <class Arrangement>
class Arr_point_location_result
{
public:
  /// The type of a point-location or vertical ray-shoot query return type.
  typedef Hidden_type Type;
}; /* end Arr_walk_along_line_point_location */
} /* end namespace CGAL */