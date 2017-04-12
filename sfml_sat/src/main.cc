#include <iostream>
#include <SFML/Graphics.hpp>

sf::Font font;

bool collision(const sf::ConvexShape& obj1, const sf::ConvexShape& obj2, sf::RenderWindow& window)
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
    float xpos = 60.0f;
    for(auto axis : axes)
    {
        axis /= std::sqrt(axis.x * axis.x + axis.y * axis.y);
        std::swap(axis.x, axis.y);
        axis.y *= -1;
        sf::RectangleShape axis_shape;
        axis_shape.setSize({2, 60});
        axis_shape.setOrigin(1, 0);
        axis_shape.setPosition(xpos,300);
        axis_shape.setFillColor(sf::Color::Blue);
        axis_shape.setRotation(-std::atan2(axis.x, axis.y) * 180.0f / 3.141f);
        window.draw(axis_shape);
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
        sf::Text o1i("" + std::to_string(obj1_min), font);
        o1i.setCharacterSize(12);
        o1i.setColor(sf::Color::Black);
        sf::Text o1a("" + std::to_string(obj1_max), font);
        o1a.setCharacterSize(12);
        o1a.setColor(sf::Color::Black);
        sf::Text o2i("" + std::to_string(obj2_min), font);
        o2i.setCharacterSize(12);
        o2i.setColor(sf::Color::Black);
        sf::Text o2a("" + std::to_string(obj2_max), font);
        o2a.setCharacterSize(12);
        o2a.setColor(sf::Color::Black);

        o1i.setPosition(xpos-40, 400); 
        o1a.setPosition(xpos-40, 440);
        o2i.setPosition(xpos-40, 480);
        o2a.setPosition(xpos-40, 520);
        window.draw(o1i);
        window.draw(o1a);
        window.draw(o2i);
        window.draw(o2a);

        float max = std::max(obj2_max, obj1_max); 
        float min = std::min(obj2_min, obj1_min);
        float dist = max - min;
        obj1_min -= min;
        obj2_min -= min;
        obj1_max -= min;
        obj2_max -= min;

        float fac = 60/dist;
        obj1_min *= fac;
        obj2_min *= fac;
        obj1_max *= fac;
        obj2_max *= fac;

        sf::RectangleShape points[4];
        for(auto& point : points)
        {
            point.setSize({5.0f, 10.0f});
            point.setOrigin(2.5f, 2.5f);
            point.setPosition(xpos, 300);
        }
        points[0].move(axis * obj2_min);
        points[1].move(axis * obj2_max);
        points[0].setFillColor(sf::Color::Green);
        points[1].setFillColor(sf::Color::Blue);
        points[2].move(axis * obj1_min);
        points[3].move(axis * obj1_max);
        points[2].setFillColor(sf::Color::Magenta);
        points[3].setFillColor(sf::Color::Red);
        for(auto point : points)
            window.draw(point);

        if(obj1_min > obj2_max || obj2_min > obj1_max)
        {
            collision = false;
        }
        //if(!collision)
        xpos += 125;
        //    break;
    }
    return collision;
}

void draw_normals(const sf::ConvexShape& obj, sf::RenderWindow& window)
{
    for(int i = 0; i < obj.getPointCount(); ++i)
    {
        sf::Vector2f point_1 = obj.getTransform().transformPoint(obj.getPoint(i));
        sf::Vector2f point_2 = obj.getTransform().transformPoint(obj.getPoint((i+1)%obj.getPointCount()));
        sf::Vector2f axis_position = 0.5f * (point_1 + point_2);
        sf::Vector2f axis_direction = point_1 - point_2;
        axis_direction /= std::sqrt(axis_direction.x * axis_direction.x + axis_direction.y * axis_direction.y);
        std::swap(axis_direction.x, axis_direction.y);
        axis_direction.y *= -1;
        sf::RectangleShape line;
        line.setSize({2, 50});
        line.setOrigin(1.0f, 0);
        line.setFillColor(sf::Color::Blue);
        line.setPosition(axis_position);
        line.setRotation(-std::atan2(axis_direction.x, axis_direction.y) * 180.0f / 3.141f);
        window.draw(line);
    }
}

int main(int argc, char* argv[])
{
    font.loadFromFile("../../sfml_sat/res/FreeMono.ttf");
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
    objects[1].setPoint(4, sf::Vector2f(80, 0));
    objects[1].setPosition(200, 200);

    int selected_rect = 0;
    bool moving = true;
    sf::Vector2f rotation_center;

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::KeyPressed)
                if(event.key.code == sf::Keyboard::Tab)
                    selected_rect = 1 - selected_rect;
            if(event.type == sf::Event::MouseButtonPressed)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    moving = false;
                    rotation_center = objects[selected_rect].getPosition();
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
                    objects[selected_rect].setPosition(event.mouseMove.x, event.mouseMove.y);
                }
                else
                {
                    objects[selected_rect].setRotation(-180 / 3.141f * std::atan2(event.mouseMove.x - rotation_center.x, event.mouseMove.y - rotation_center.y));
                }
            }
            if(event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::White);

        if(collision(objects[0], objects[1], window))
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
        for(const auto& obj : objects)
            draw_normals(obj, window);
        window.display();
    }
}
