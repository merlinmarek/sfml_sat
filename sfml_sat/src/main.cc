#include <iostream>
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[])
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "sfml_sat");
    window.setFramerateLimit(60);

    sf::RectangleShape rects[2];
    for(auto& rect : rects)
    {
        rect.setSize(sf::Vector2f(150.0f, 100.0f));
        rect.setOrigin(75.0f, 50.0f);
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineThickness(-2.0f);
        rect.setOutlineColor(sf::Color::Black);
    }
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
                    rotation_center = rects[selected_rect].getPosition();
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
                    rects[selected_rect].setPosition(event.mouseMove.x, event.mouseMove.y);
                }
                else
                {
                    rects[selected_rect].setRotation(-180 / 3.141f * std::atan2(event.mouseMove.x - rotation_center.x, event.mouseMove.y - rotation_center.y));
                }
            }
            if(event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::White);

        //get all the points
        sf::Vector2f points[8];
        sf::Transform trans[2];
        trans[0] = rects[0].getTransform();
        trans[1] = rects[1].getTransform();
        for(int i = 0; i < 4; ++i)
        {
            points[i] = trans[0].transformPoint(rects[0].getPoint(i));
            points[i+4] = trans[1].transformPoint(rects[1].getPoint(i));
        }

        //get all the axes
        sf::Vector2f axes[8];
        for(int i = 0; i < 4; ++i)
        {
            axes[i] = points[i] - points[(i+1)%4];
            axes[i+4] = points[i+4] - points[(i+5)%4+4];
        }
        for(auto& axis : axes)
        {
            axis = axis / std::sqrt(axis.x * axis.x + axis.y * axis.y);
        }
        //project all 8 points on each axis
        bool collision = false;
        for(const auto& axis : axes)
        {
            float rect0_min, rect0_max, rect1_min, rect1_max;
            for(int i = 0; i < 4; ++i)
            {
                //project points of the first rect on each axis saving min and max
                float rect0 = axis.x * points[i].x + axis.y * points[i].y;
                float rect1 = axis.x * points[i+4].x + axis.y * points[i+4].y;
                if(i == 0)
                {
                    rect0_min = rect0_max = rect0;
                    rect1_min = rect1_max = rect1;
                }
                else
                {
                    rect0_min = std::min(rect0_min, rect0);
                    rect0_max = std::max(rect0_max, rect0);

                    rect1_min = std::min(rect1_min, rect1);
                    rect1_max = std::max(rect1_max, rect1);
                }
            }
            if(rect0_max < rect1_min || rect1_max < rect0_min)
                collision = true;
        }
        if(!collision)
        {
            rects[0].setFillColor(sf::Color::Red);
            rects[1].setFillColor(sf::Color::Red);
        }
        else
        {
            rects[0].setFillColor(sf::Color::Transparent);
            rects[1].setFillColor(sf::Color::Transparent);
        }
        for(const auto& rect : rects)
            window.draw(rect);
        window.display();
    }
}
