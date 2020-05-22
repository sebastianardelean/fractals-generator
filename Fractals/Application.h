#pragma once
#include "Fractal.h"
class Application
{
public:
	Application();
	~Application();
	void Run();
protected:
private:
	void Update();
	void Draw();
	void OnResize();

	sf::RenderWindow m_Window;
	sf::Text m_TextIterations;
	sf::Text m_TextTimeTaken;
	sf::Text m_TextPause;
	sf::Font m_Font;
	Fractal m_Fractal;
	sf::Vector2f v_StartPan;
	sf::Vector2f v_Offset;
	sf::Vector2f v_Scale;
	bool b_IsPaused;

};

