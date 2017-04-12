#include <iostream>
#include <SFML/Graphics.hpp>

bool collision(const sf::ConvexShape& obj1, const sf::ConvexShape& obj2) 
{
    sf::Vector2f axes[obj1.getPointCount() + obj2.getPointCount()];
    sf::Vector2f obj1_points[obj1.getPointCount()];
    sf::Vector2f obj2_points[obj2.getPointCount()];
    for(int i = 0; i < obj1.getPointCount(); ++i)
        obj1_points[i] = obj1.getTransform().transformPoint(obj1.getPoint(i));
    for(int i = 0; i < obj2.getPointCount(); ++i)
        obj2_points[i] = obj2.getTransform().transformPoint(obj2.getPoint(i));

    for(int i = 0; i < obj1.getPointCount(); ++i)
        axes[i] = obj1_points[i] - obj1_points[(i+1)%obj1.getPointCount()];
    for(int i = 0; i < obj2.getPointCount(); ++i)
        axes[i + obj1.getPointCount()] = obj2_points[i] - obj2_points[(i+1)%obj2.getPointCount()];

    bool collision = true;
    for(auto axis : axes)
    {
        axis /= std::sqrt(axis.x * axis.x + axis.y * axis.y);
        std::swap(axis.x, axis.y);
        axis.y *= -1;
        float obj1_min, obj1_max, obj2_min, obj2_max;

        obj1_min = obj2_min = 100000;
        obj1_max = obj2_max = -100000;

        for(int i = 0; i < obj1.getPointCount(); ++i)
        {
            float projection = obj1_points[i].x * axis.x + obj1_points[i].y * axis.y;
            obj1_min = std::min(obj1_min, projection);
            obj1_max = std::max(obj1_max, projection);
        }
        for(int i = 0; i < obj2.getPointCount(); ++i)
        {
            float projection = obj2_points[i].x * axis.x + obj2_points[i].y * axis.y;
            obj2_min = std::min(obj2_min, projection);
            obj2_max = std::max(obj2_max, projection);
        }
        if(obj1_min > obj2_max || obj2_min > obj1_max)
        {
            collision = false;
        }
        if(!collision)
            break;
    }
    return collision;
}

int main(int argc, char* argv[])
{
    sf::RenderWindow window(sf::VideoMode(1000, 600), "sfml_sat");
    window.setFramerateLimit(60);

    sf::ConvexShape objects[2];
    for(auto& obj : objects)
    {
        obj.setOutlineColor(sf::Color::Black);
        obj.setOutlineThickness(-2.0f);
        obj.setFillColor(sf::Color::Transparent);
    }
    objects[0].setOrigin(50, 25);
    objects[0].setPointCount(3);
    objects[0].setPoint(0, sf::Vector2f(0, 0));
    objects[0].setPoint(1, sf::Vector2f(0, 50));
    objects[0].setPoint(2, sf::Vector2f(100, 50));
    objects[0].setPosition(100, 100);

    objects[1].setOrigin(50, 25);
    objects[1].setPointCount(5);
    objects[1].setPoint(0, sf::Vector2f(0, 0));
    objects[1].setPoint(1, sf::Vector2f(0, 50));
    objects[1].setPoint(2, sf::Vector2f(100, 50));
    objects[1].setPoint(3, sf::Vector2f(100, 30));
    objects[1].setPoint(4, sf::Vector2f(50, 0));
    objects[1].setPosition(200, 200);

    int selected_object = 0;
    bool moving = true;
    sf::Vector2f rotation_center;

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::KeyPressed)
                if(event.key.code == sf::Keyboard::Tab)
                    selected_object = 1 - selected_object;
            if(event.type == sf::Event::MouseButtonPressed)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    moving = false;
                    rotation_center = objects[selected_object].getPosition();
                }
            }
            if(event.type == sf::Event::MouseButtonReleased)
            {
                if(event.mouseButton.button == sf::Mouse::Left && !moving)
                {
                    sf::Mouse::setPosition(sf::Vector2i(rotation_center.x, rotation_center.y), window);
                    moving = true;
                }
            }
            if(event.type == sf::Event::MouseMoved)
            {
                if(moving)
                {
                    objects[selected_object].setPosition(event.mouseMove.x, event.mouseMove.y);
                }
                else
                {
                    objects[selected_object].setRotation(-180 / 3.141f * std::atan2(event.mouseMove.x - rotation_center.x, event.mouseMove.y - rotation_center.y));
                }
            }
            if(event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::White);

        if(collision(objects[0], objects[1]))
        {
            objects[0].setFillColor(sf::Color::Red);
            objects[1].setFillColor(sf::Color::Red);
        }
        else
        {
            objects[0].setFillColor(sf::Color::Transparent);
            objects[1].setFillColor(sf::Color::Transparent);
        }

        for(const auto& obj : objects)
            window.draw(obj);
        window.display();
    }
}
