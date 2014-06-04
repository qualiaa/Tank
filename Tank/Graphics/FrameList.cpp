// Copyright (©) Jamie Bayne, David Truby, David Watson 2013-2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "FrameList.hpp"

#include <algorithm>

namespace tank
{

FrameList::FrameList(std::string file, Vector<unsigned int> frameDims)
        : Image(file), frameDimensions_(frameDims)
{
    Image::setClip({0, 0, frameDims.x, frameDims.y});
    // image_.setSize(frameDims);
}

void FrameList::add(std::string name, std::vector<unsigned int> const& frames,
                    std::chrono::milliseconds time)
{
    // TODO: validate arguments
    // Create new Animation
    animations_.push_back({name, frames, time});
}

void FrameList::remove(std::string name)
{
    // Find the animation by name
    auto iter = std::find_if_not(
            animations_.begin(), animations_.end(),
            [&name](Animation& anim) { return anim.name == (name); });

    // Remove the animation from the animations list
    if (iter != animations_.end()) {
        animations_.erase(iter);
    }
}

void FrameList::select(std::string name, bool loop,
                       std::function<void()> callback)
{
    // Check that the requested animation is not already playing
    if (not currentAnimation_ || currentAnimation_->name != name) {
        // search for the name requested
        for (auto& anim : animations_) {
            if (anim.name == name) {
                currentAnimation_ = &anim;
                currentFrame_ = 0;
                loop_ = loop;
                callback_ = callback;
            }
        }
    }
}

void FrameList::refresh()
{
    // Only play if there is a selected animation
    if (currentAnimation_) {
        // Check if we need to change animation frame
        bool playNextFrame = animTimer_.getTicks() > currentAnimation_->time;

        if (playNextFrame) {
            ++currentFrame_;

            animTimer_.start(); // Reset timer

            // Check if we've finished the animation
            unsigned const int lastFrame = currentAnimation_->frameList.size();

            if (currentFrame_ >= lastFrame) {
                currentFrame_ = 0;

                callback_();

                // If the animation doesn't loop, stop it
                if (not loop_) {
                    // Reset all properties (callback, timer, currentFrame, etc)
                    stop();
                }
            }
        }
    }

    // Animation may have ended now. If not, need to set clipping mask for image
    if (currentAnimation_) {
        // Start at first frame
        unsigned int frame = currentAnimation_->frameList[currentFrame_];

        // Set clipping rectangle according to current frame
        Image::setClip(frameDimensions_, frame, clipRect_);
    }
}

void FrameList::start()
{
    if (not animTimer_.isStarted()) {
        animTimer_.start();
    }
}

void FrameList::pause()
{
    animTimer_.pause();
}

void FrameList::resume()
{
    if (animTimer_.isPaused() and currentAnimation_ != nullptr) {
        animTimer_.resume();
    }
}

void FrameList::stop()
{
    // Change appearance to first frame
    currentFrame_ = 0;
    refresh();

    // Unset member variables
    animTimer_.stop();
    callback_ = [] {};
    currentAnimation_ = nullptr;
}

void FrameList::draw(Vectorf parentPos, float parentRot, Vectorf parentOri,
                     Camera const& cam)
{
    // TODO: Move this somewhere else and make draw const
    refresh();
    Image::draw(parentPos, parentRot, parentOri, cam);
}

void FrameList::setClip(Vectoru dimensions, unsigned int index, Rectu clip)
{
    // TODO: This needs testing with rectangular dimensions
    Rectu new_clip = {0, 0, dimensions.x, dimensions.y};

    Vectoru usefulSize = {
            frameDimensions_.x - (frameDimensions_.x % dimensions.x),
            frameDimensions_.y - (frameDimensions_.y % dimensions.y)};

    new_clip.x = (dimensions.x * index) % usefulSize.x;
    new_clip.y = dimensions.y * ((dimensions.x * index) / usefulSize.x);

    if (clip != Rectu{0, 0, 0, 0}) {
        new_clip.x += clip.x;
        new_clip.y += clip.y;
        new_clip.w = clip.w;
        new_clip.h = clip.h;
    }

    setClip(clip);
}

void addWalkingFrameList(FrameList& anim, std::chrono::milliseconds time)
{
    unsigned int xFrames =
            anim.getTextureSize().x / anim.getFrameDimensions().x;

    std::vector<unsigned int> up;
    std::vector<unsigned int> right;
    std::vector<unsigned int> down;
    std::vector<unsigned int> left;

    for (unsigned int i = 0; i < xFrames; ++i) {
        up.push_back(i);
        right.push_back(i + xFrames);
        down.push_back(i + xFrames * 2);
        left.push_back(i + xFrames * 3);
    }
    anim.add("walk_up", up, time);
    anim.add("walk_right", right, time);
    anim.add("walk_down", down, time);
    anim.add("walk_left", left, time);
}
}
