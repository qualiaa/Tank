/* This file is part of Tank.
 *
 * Tank is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tank is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License and
 * the GNU Lesser General Public Licence along with Tank. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Copyright 2013 (©) Jamie Bayne, David Truby, David Watson.
 */

#ifndef TANK_FRAMELIST_HPP
#define TANK_FRAMELIST_HPP

#include <string>
#include <vector>
#include <functional>
#include "Graphic.hpp"
#include "Image.hpp"
#include "../Utility/Vector.hpp"
#include "../Utility/Rect.hpp"
#include "../Utility/Timer.hpp"

namespace tank
{

/*!
 * \brief Represents an image with multiple frames and stores animations for
 * that image.
 */
class FrameList final : public Graphic
{
    struct Animation
    {
        std::string name;
        std::vector<unsigned int> frameList;
        std::chrono::milliseconds time;
    };

    Image image_;
    Animation* currentAnimation_ {nullptr};
    unsigned int currentFrame_ {0};
    Timer animTimer_;
    bool loop_ {false};
    Vectoru frameDimensions_ {0, 0};
    std::function<void()> callback_ = []{};
    std::vector<Animation>  animations_;

public:
    FrameList() = default;
    /*!
     * \brief Construct an Animation with a Texture.
     * \param t Image to give the animation.
     * \param frameDims size of each image in the Texture.
     */
    FrameList(Image const&, Vector<unsigned int> frameDimensions);

    /*!
     * \brief Add an animation
     *
     * Create a named list of frames to play in sequence, to be displayed  at a
     * fixed rate
     *
     * \param name String to identify the animation with later.
     * \param frames std::initializer_list of frames.
     * \param frameTime Time between each frame in milliseconds (ms)
     */
    void add(std::string name, const std::vector<unsigned int>& frames,
             std::chrono::milliseconds frameTime);

    void remove(std::string name);

    void select(std::string name, bool loop = true,
                std::function<void()> callback = []{});

    /*!
     * \brief Update the animation to the current frame. Called by draw
     */
    void refresh();

    /*!
     * \brief Draw the animation.
     *
     * Trying to draw an animation without a texture will crash the game!
     *
     * \param pos Position at which to draw the texture.
     */
    void draw(Vectorf parentPos = {},
              float parentRot = 0, 
              Camera const& = Camera()) override;

    /*!
     * \brief Start the animation
     */
    void start();

    /*!
     * \brief Pause the animation.
     */
    void pause();

    /*!
     * \brief Resume the animation.
     */
    void resume();

    /*!
     * \brief Stop the animation.
     * Sets the state of the animation to the first frame.
     */
    void stop();

    /*!
     * \return If the animation is playing.
     */
    bool playing()
    {
        return currentAnimation_;
    }
    std::string currentAnimation()
    {
        if (not playing())
        {
            return "";
        }
        return currentAnimation_->name;
    }

    /*!
     * \brief Set the texture of the FrameList.
     * \param texture the Texture to set.
     * \param frameDims the dimensions of each image in the texture.
     */
    void setImage(Image const&, Vectoru frameDims);

    virtual void setPos(Vectorf pos) { image_.setPos(pos); }
    virtual Vectorf getPos() const { return image_.getPos(); }
    virtual bool isRelativeToParent() { return image_.isRelativeToParent(); }

    virtual void setRotation(float angle) { image_.setRotation(angle); }
    virtual float getRotation() const { return image_.getRotation(); }

    void setClip(Rectu clip) { image_.setClip(clip); }
    Rectu getClip() const { return image_.getClip(); }

    void setOrigin(Vectorf origin) override { image_.setOrigin(origin); }
    Vectorf getOrigin() const override { return image_.getOrigin(); }

    void setSize(Vectorf size) { image_.setSize(size); }
    Vectorf getSize() const override { return image_.getSize(); }


    Vectoru getFrameDimensions() const { return frameDimensions_; }
    virtual void setScale(float scale) override
    {
        image_.setScale(scale);
    }
    virtual void setScale(Vectorf scale) override
    {
        image_.setScale(scale);
    }
    virtual Vectorf getScale() const override
    {
        return image_.getScale();
    }
    virtual void drawRelativeToParent(bool relative)
    {
        image_.drawRelativeToParent(relative);
    }
    virtual Vectoru getTextureSize() const
    {
        return image_.getTextureSize();
    }
};

// TODO: Use enum to specify image format
/*!
 * \brief Generate a walking animations for Animation
 *
 * Generates four animations: walk_up, walk_right, walk_down, walk_left.
 * Currently, they are generated from image rows going down in that order.
 */
void addWalkingAnimation(FrameList& frames, std::chrono::milliseconds time);

} // tank

#endif /* TANK_FRAMELIST_HPP */
