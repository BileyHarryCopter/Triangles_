#pragma once
#include "triangle.hpp"
#include "point.hpp"
#include "vector.hpp"
#include "line.hpp"
#include "segment.hpp"
#include "location.hpp"
#include "triangle_intersect_algorithms.hpp"

namespace Geometry
{

template<std::floating_point F>
bool are_intersecting (const Point<F>& p1, const Point<F>& p2)
{
    return p1 == p2;
}

template<std::floating_point F>
bool are_intersecting (const Point<F>& point, const Segment<F>& segment)
{
    Vector PF {point, segment.F_};
    Vector PS {point, segment.S_};

    if (!are_collinear(PF, PS))
        return false;
    else if (scalar_product(PF,PS) < 0.0 || segment.F_ == point || segment.S_ == point)
        return true;
    else
        return false;
}

template<std::floating_point F>
bool are_intersecting (const Point<F>& point, const Triangle<F>& triangle)
{
    if (Location::define_prhc(triangle.P_, triangle.Q_, triangle.R_, point) != Location::Loc3D::On)
        return false;
    
    return Location::laying_in_same_half(triangle.P_, triangle.Q_, triangle.R_, point) != Location::LocPoints::InDifferent &&
           Location::laying_in_same_half(triangle.Q_, triangle.R_, triangle.P_, point) != Location::LocPoints::InDifferent &&
           Location::laying_in_same_half(triangle.R_, triangle.P_, triangle.Q_, point) != Location::LocPoints::InDifferent;
}

template<std::floating_point F>
bool are_intersecting (const Segment<F>& segment, const Point<F>& point)
{
    return are_intersecting(point, segment);
}

template<std::floating_point F>
bool are_intersecting (const Segment<F>& seg1, const Segment<F>& seg2)
{
    Vector F1S1 {seg1.F_, seg1.S_};
    Vector F2S2 {seg2.F_, seg2.S_};
    
    Line line1 {seg1.F_, F1S1}, line2 {seg2.F_, F2S2};
    if (!are_intersecting(line1, line2))
        return false;

    if (line1 == line2)
    {
        Segment seg2_cpy {seg2};

        const auto& A1 = seg1.F_;
        const auto& B1 = seg1.S_;
        if (scalar_product(F1S1, F2S2) < 0.0)
            seg2_cpy.swap_points();
        const auto& A2 = seg2_cpy.F_;
        const auto& B2 = seg2_cpy.S_;

        Vector B1A2 {B1, A2};
        Vector B2A1 {B2, A1};

        auto prod_B1A2_B2A1 = scalar_product(B1A2, B2A1);

        return prod_B1A2_B2A1 > 0.0 || Compare::are_equal(prod_B1A2_B2A1, 0.0); 
    }

    return Location::laying_in_same_half(seg1.F_, seg1.S_, seg2.F_, seg2.S_) != Location::LocPoints::InSame &&
           Location::laying_in_same_half(seg2.F_, seg2.S_, seg1.F_, seg1.S_) != Location::LocPoints::InSame;
}

namespace Algorithm
{

template<std::floating_point F>
bool seg_tr_intersecting_2D(const Segment<F>& seg, const Triangle<F>& tr)
{
    if (are_intersecting(seg.F_, tr) || are_intersecting(seg.S_, tr))
        return true;

    return are_intersecting(seg, Segment {tr.P_, tr.Q_}) ||
           are_intersecting(seg, Segment {tr.Q_, tr.R_}) ||
           are_intersecting(seg, Segment {tr.R_, tr.P_});
}

template<std::floating_point Float>
bool seg_tr_intersecting_3D(const Segment<Float>& seg, const Triangle<Float>& tr, Location::Loc3D F_loc, Location::Loc3D S_loc)
{
    const auto& P = tr.P_, Q = tr.Q_, R = tr.R_;
    Segment seg_cpy = seg;

    if (F_loc == Location::Loc3D::On)
    {
        seg_cpy.swap_points();
        F_loc = S_loc;
    }

    const auto& F = seg_cpy.F_, S = seg_cpy.S_;

    Location::Loc3D outside {};
    if (F_loc == Location::Loc3D::Above)
        outside = Location::Loc3D::Below;
    else
        outside = Location::Loc3D::Above;

    return Location::define_prhc(P, R, F, S) != outside &&
           Location::define_prhc(R, Q, F, S) != outside &&
           Location::define_prhc(Q, P, F, S) != outside;
}
} // namespace Algorithm

template<std::floating_point F>
bool are_intersecting (const Segment<F>& segment, const Triangle<F>& triangle)
{
    auto F_loc = Location::define_prhc(triangle.P_, triangle.Q_, triangle.R_, segment.F_);
    auto S_loc = Location::define_prhc(triangle.P_, triangle.Q_, triangle.R_, segment.S_);

    if (F_loc != Location::Loc3D::On && S_loc != Location::Loc3D::On && F_loc == S_loc)
        return false;

    if (F_loc == Location::Loc3D::On && S_loc == Location::Loc3D::On)
        return Algorithm::seg_tr_intersecting_2D(segment, triangle);
    else
        return Algorithm::seg_tr_intersecting_3D(segment, triangle, F_loc, S_loc);
}

template<std::floating_point F>
bool are_intersecting (const Triangle<F>& triangle, const Point<F>& point)
{
    return are_intersecting(point, triangle);
}

template<std::floating_point F>
bool are_intersecting (const Triangle<F>& triangle, const Segment<F>& segment)
{
    return are_intersecting(segment, triangle);
}

template<std::floating_point F>
bool are_intersecting (const Triangle<F>& tr_1, const Triangle<F>& tr_2)
{
    auto P1_loc = Location::define_prhc (tr_2.P_, tr_2.Q_, tr_2.R_, tr_1.P_);
    auto Q1_loc = Location::define_prhc (tr_2.P_, tr_2.Q_, tr_2.R_, tr_1.Q_);
    auto R1_loc = Location::define_prhc (tr_2.P_, tr_2.Q_, tr_2.R_, tr_1.R_);

    if (P1_loc != Location::Loc3D::On && P1_loc == Q1_loc && Q1_loc == R1_loc)
        return false;
    else if (P1_loc == Location::Loc3D::On && Q1_loc == Location::Loc3D::On && R1_loc == Location::Loc3D::On) 
        return Algorithm::intersection_in_2D (tr_1, tr_2);
    else 
        return Algorithm::intersection_in_3D (tr_1, tr_2, P1_loc, Q1_loc, R1_loc);
}
} // namespace Geometry
