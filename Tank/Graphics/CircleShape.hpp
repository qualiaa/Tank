#ifndef TANK_CIRCLESHAPE_HPP
#define TANK_CIRCLESHAPE_HPP

#include <SFML/Graphics/CircleShape.hpp>
#include "Shape.hpp"

namespace tank {

class CircleShape : public Shape
{
    sf::CircleShape circleShape_;

public:
    CircleShape(float radius = 0);

    void setRadius(float radius) { circleShape_.setRadius(radius); }
    float getRadius() const { return circleShape_.getRadius(); }

    virtual void setFillColor(Color colour) override;
    virtual void setOutlineColor(Color colour) override;
    virtual void setOutlineThickness(float) override;
    virtual Color const& getFillColor() const override;
    virtual Color const& getOutlineColor() const override;
    virtual float getOutlineThickness() const override;

    virtual Vectorf getSize() const override;
    virtual void setScale(float scale) override;
    virtual void setScale(Vectorf scale) override;
    virtual Vectorf getScale() const override;

    virtual void setOrigin(Vectorf o) override;
    virtual Vectorf getOrigin() const override;

    virtual void draw(Vectorf parentPos = {},
                      float parentRot = 0,
                      Camera const& = Camera()) override;
};

}

#endif /* TANK_CICLESHAPE_HPP */
