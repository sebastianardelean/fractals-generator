#include "stdafx.h"
#include "Fractal.h"
#include "Application.h"

Application::Application(): m_Window(sf::VideoMode(1280, 720),"Fractal Generator"),m_Fractal()
{
    m_Window.setFramerateLimit(60);
    m_Fractal.Init(sf::Vector2u(m_Window.getSize().x, m_Window.getSize().y));
    v_Offset = sf::Vector2f(0.0, 0.0);
    v_Scale = sf::Vector2f(m_Window.getSize().x / 2.0, m_Window.getSize().y);
    v_StartPan = sf::Vector2f(0.0, 0.0);

    m_Font.loadFromFile("D:\DejaVuSans.ttf");
    m_TextIterations.setFont(m_Font);
    m_TextIterations.setPosition(10.f, 10.f);
    m_TextIterations.setCharacterSize(24); // in pixels, not points!
    
    m_TextIterations.setFillColor(sf::Color::White);


    m_TextTimeTaken.setFont(m_Font);
    m_TextTimeTaken.setPosition(10.f, 10.f);
    m_TextTimeTaken.setCharacterSize(24); // in pixels, not points!
    
    m_TextTimeTaken.setFillColor(sf::Color::White);

    
    m_TextPause.setFont(m_Font);
    m_TextPause.setPosition(10.f, 10.f);
    m_TextPause.setCharacterSize(24); // in pixels, not points!

    m_TextPause.setFillColor(sf::Color::Red);

    b_IsPaused = false;
    
   
}

Application::~Application()
{
   
}

void Application::Run()
{

    while (m_Window.isOpen()) {
        Update();
        Draw();
    }
}

void Application::Update()
{
    sf::Event event;
    static bool shouldRedraw = true;
    static sf::Event::EventType lastMouseButtonEvent = sf::Event::MouseButtonReleased;
    
    auto screenToWorld = [](const sf::Vector2f vScale, const sf::Vector2f vOffset, const sf::Vector2f& n, sf::Vector2f& v) {
        v.x = (double)(n.x) / vScale.x + vOffset.x;
        v.y = (double)(n.y) / vScale.y + vOffset.y;
    };

    auto textTool = [](sf::Text *textObj, std::string str,sf::Vector2f position,unsigned int charSize,sf::Color color) {
        
        textObj->setPosition(10.f+position.x, 10.f+position.y);
        textObj->setCharacterSize(charSize); 
        textObj->setString(str);
        textObj->setFillColor(color);
    };

    while (m_Window.pollEvent(event))
    {
        auto position = sf::Mouse::getPosition(m_Window);
        sf::Vector2f mousePosition = sf::Vector2f(position.x, position.y);
        sf::Vector2f mouseBeforeZoom;
        sf::Vector2f mouseAfterZoom;

        sf::Vector2f pixel_TopLeft = sf::Vector2f( 0,0 );
        sf::Vector2f pixel_BottomRight = sf::Vector2f(m_Window.getSize().x, m_Window.getSize().y);
        sf::Vector2f frac_TopLeft = sf::Vector2f(-2.0, -1.0 );
        sf::Vector2f frac_BottomRight = sf::Vector2f(1.0, 1.0 );

        if (event.type == sf::Event::Closed)
            m_Window.close();
        
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            v_StartPan = mousePosition;
            lastMouseButtonEvent = sf::Event::MouseButtonPressed;
            shouldRedraw = true;
        }

        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            if (lastMouseButtonEvent == sf::Event::MouseButtonPressed) {
                v_Offset.x -= (mousePosition.x - v_StartPan.x) / v_Scale.x;
                v_Offset.y -= (mousePosition.y - v_StartPan.y) / v_Scale.y;

                v_StartPan = mousePosition;
                shouldRedraw = true;
                lastMouseButtonEvent = sf::Event::MouseButtonReleased;
            }
        }

        if (event.type == sf::Event::MouseWheelMoved) {
            if (event.mouseWheel.delta > 0) {
                m_Fractal.IncreaseNumberOfIterationsBy(event.mouseWheel.delta);
            }
            else {
                m_Fractal.DecreaseNumberOfIterationsBy(event.mouseWheel.delta);
            }
            shouldRedraw = true;
        }

        screenToWorld(v_Scale,v_Offset,mousePosition, mouseBeforeZoom);
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Q) {
                v_Scale.x *= 1.1;
                v_Scale.y *= 1.1;
                shouldRedraw = true;
            }
            if (event.key.code == sf::Keyboard::A) {
                v_Scale.x *= 0.9;
                v_Scale.y *= 0.9;
                shouldRedraw = true;
            }
            if (event.key.code == sf::Keyboard::Add) {
                m_Fractal.IncreaseNumberOfIterations();
                shouldRedraw = true;
            }

            if (event.key.code == sf::Keyboard::Subtract) {
                m_Fractal.DecreaseNumberOfIterations();
                shouldRedraw = true;
            }
            if (event.key.code == sf::Keyboard::P) {
                b_IsPaused = !b_IsPaused;
            }
        }
        screenToWorld(v_Scale, v_Offset, mousePosition, mouseAfterZoom);

        v_Offset += (mouseBeforeZoom - mouseAfterZoom);


        screenToWorld(v_Scale, v_Offset, pixel_TopLeft, frac_TopLeft);
        screenToWorld(v_Scale, v_Offset, pixel_BottomRight, frac_BottomRight);

        if (shouldRedraw && !b_IsPaused) {
            auto tp1 = std::chrono::high_resolution_clock::now();
            m_Fractal.Generate(pixel_TopLeft,pixel_BottomRight,frac_TopLeft,frac_BottomRight);
            m_Fractal.Update();
            auto tp2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsedTime = tp2 - tp1;
            
            textTool(&m_TextTimeTaken, "Time Taken:" + std::to_string(elapsedTime.count())+" s",
                sf::Vector2f(0, 0), 24, sf::Color::White);
            shouldRedraw = false;
        }
     
  
        textTool(&m_TextIterations, "Number of Iterations:"+ std::to_string(m_Fractal.GetNumberOfIterations()),
            sf::Vector2f(0,30),24,sf::Color::White);

        if (b_IsPaused) {
            textTool(&m_TextPause, "Paused",sf::Vector2f(0, 60), 24, sf::Color::White);
        } 
        else {
            textTool(&m_TextPause, "", sf::Vector2f(0, 60), 24, sf::Color::White);
        }
        
    }
}

void Application::Draw()
{
    m_Window.resetGLStates();
    m_Window.clear(sf::Color::Black);
    m_Window.draw(m_Fractal);
    m_Window.draw(m_TextTimeTaken);
    m_Window.draw(m_TextIterations);
    m_Window.draw(m_TextPause);
    m_Window.display();
}


void Application::OnResize()
{
}

