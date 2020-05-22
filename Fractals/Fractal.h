#pragma once

class Fractal: public sf::Drawable
{
public:
	Fractal();
	virtual ~Fractal();
	void Init(const sf::Vector2u& size);
	inline void IncreaseNumberOfIterations() {
		u32_NumberOfIterations += 8;
	}

	inline void IncreaseNumberOfIterationsBy(uint32_t delta) {
		u32_NumberOfIterations += delta;
	}

	inline void DecreaseNumberOfIterationsBy(uint32_t delta) {
		u32_NumberOfIterations -= delta;
	}

	inline void DecreaseNumberOfIterations() {
		u32_NumberOfIterations -= 8;
	}

	inline uint32_t GetNumberOfIterations() const {
		return u32_NumberOfIterations;
	}
	void Update();
	void Generate(sf::Vector2f pxTl, sf::Vector2f pxBr, sf::Vector2f frTl, sf::Vector2f frBr);
	
protected:
private:
	
	void GenerateColors();
	void Calculate(sf::Vector2f pxTl, sf::Vector2f pxBr, sf::Vector2f frTl, sf::Vector2f frBr);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	sf::Vector2u m_Size;
	sf::Sprite m_Fractal;
	sf::Texture m_Texture;
	std::vector<size_t> m_Pixels;
	uint32_t u32_NumberOfIterations;
	std::vector<std::tuple<uint8_t, uint8_t, uint8_t>>m_Colors;
	
	std::atomic<int> n_WorkerComplete;
	

	
};

