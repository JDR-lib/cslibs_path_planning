/**


    @year   2015
    @author Karsten Bohlmann
    @email  bohlmann@gmail.com
    @file   shape.h


*/

#ifndef SHAPE_H
#define SHAPE_H
#include<Eigen/StdVector>
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector2d)
#include <memory>
#include "utils_general/MathHelper.h"
#include <Eigen/Core>
#include <map>
#include <list>
#include <deque>

#include <iostream>


// ***todo templatisieren http://eigen.tuxfamily.org/dox/TopicFunctionTakingEigenTypes.html
// Fwrd. decl:
   namespace Eigen { template<class T> class aligned_allocator; }
namespace path_geom {

    const double DIST_EPS   = 1e-5;
    const double ANGLE_EPS  = 0.01*M_PI/180.0;

    const int    ARC_LEFT   = +1; // arc/circle in mathematically positive direction
    const int    ARC_RIGHT   = -1; // arc/circle in mathematically negative direction
    const int    BACKWARD=-1;
    const int    FORWARD = 1;


    /**
    * eigen::aligned stl containers
    */
   template <class CLASS1,class CLASS2=CLASS1>
   struct aligned
   {

        typedef std::pair<CLASS1,CLASS2> pair;
        typedef std::list<CLASS1, Eigen::aligned_allocator<CLASS1> > list;

        typedef std::vector<CLASS1, Eigen::aligned_allocator<CLASS1> > vector;
        typedef std::deque<CLASS1, Eigen::aligned_allocator<CLASS1> > deque;
        typedef std::map<CLASS1,CLASS2,std::less<CLASS1>,Eigen::aligned_allocator<std::pair<const CLASS1,CLASS2> > > map;
        typedef std::multimap<CLASS1,CLASS2,std::less<CLASS1>,Eigen::aligned_allocator<std::pair<const CLASS1,CLASS2> > > multimap_t;

   };
   /**
    * eigen::aligend make_shared replacement
    */
   template <class T, class... Args>
   shared_ptr<T> make_aligned(Args&&... args)
   {
       return std::allocate_shared<T>(Eigen::aligned_allocator<T>(),std::forward<Args>(args)...);
   }



class PathPose
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    PathPose() :
        pos_(0.0,0.0){}
    PathPose(double x, double y, double theta)
        :pos_(x,y),theta_(theta),weight_(0.0) {
    }

    PathPose & operator= (const PathPose& src) {
        pos_=src.pos_;
        theta_=src.theta_;
        weight_=src.weight_;
        return (*this);
    }

    PathPose & assign (const Eigen::Vector2d& pos, double theta) {
        pos_=pos;
        theta_ = theta;
        return (*this);
    }

    bool isIdentical(const PathPose& other, double dist_tol=path_geom::DIST_EPS,
                     double angle_tol=path_geom::ANGLE_EPS) {
        return ((pos_-other.pos_).norm()<dist_tol &&
              MathHelper::AngleDelta( theta_, other.theta_)<angle_tol);
    }

    bool isIdentical(const Eigen::Vector2d& other, double dist_tol=path_geom::DIST_EPS) {
        return ((pos_-other).norm()<dist_tol);
    }

    friend ostream& operator<<(ostream& os, const PathPose& pp);



    Eigen::Vector2d pos_;
    double theta_ = 0.0;
    double weight_ = 0.0;


};

typedef std::vector<PathPose,Eigen::aligned_allocator<PathPose>> PathPoseVec;


class Shape
{
public:
    virtual bool selectStartPoint(const Eigen::Vector2d& start,  double tol=path_geom::DIST_EPS) = 0;
    virtual bool selectEndPoint(const Eigen::Vector2d& end,  double tol=path_geom::DIST_EPS) = 0;
    virtual Eigen::Vector2d startPoint() const = 0 ;
    virtual Eigen::Vector2d endPoint() const = 0 ;
    virtual void toPoints(double resolution, std::vector<Eigen::Vector2d>& points) = 0;
    virtual void toPoses(double resolution, PathPoseVec& res_poses, int move_direction,
                         bool with_end_pose) = 0;

    virtual double distanceTo(const Eigen::Vector2d& point) const = 0;
    virtual Eigen::Vector2d nearestPointTo(const Eigen::Vector2d& p) const  = 0;

};
}
#endif // SHAPE_H
