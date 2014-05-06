// Copyright (©) Jamie Bayne, David Truby, David Watson 2013-2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "CoordinateSystem.hpp"
#include <SFML/Graphics/Transformable.hpp>

namespace tank
{

Transform InertialFrame::getTransform(observing_ptr<InertialFrame> iner)
{
    // The transformation from root to iner
    Transform tInv;
    // The transformation from root to this
    Transform t;

    // Get the transforamation from root to iner and the root of iner
    observing_ptr<InertialFrame> currentFrame = iner;
    observing_ptr<InertialFrame> nextFrame = currentFrame->getParentFrame();
    while (nextFrame != currentFrame)
    {
        tInv = currentFrame->getTransformFromParent();
        currentFrame = nextFrame;
        nextFrame = currentFrame->getParentFrame();
    }
    observing_ptr<InertialFrame> inertialFrameRoot = currentFrame;

    // Get the transforamation from root to this and the root of this
    currentFrame = this;
    nextFrame = currentFrame->getParentFrame();
    while (nextFrame != currentFrame)
    {
        t = currentFrame->getTransformFromParent();
        currentFrame = nextFrame;
        nextFrame = currentFrame->getParentFrame();
    }

    if (inertialFrameRoot != currentFrame)
    {
        throw std::domain_error("Incompatible universes");
    }
    return tInv.inverse()(t);
}

} // tank
