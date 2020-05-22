#include "stdafx.h"

#include "Fractal.h"

Fractal::Fractal()
{
	
	
}

void Fractal::Init(const sf::Vector2u& size)
{
	this->m_Size = size;
	u32_NumberOfIterations = 1024;
	m_Pixels.resize(m_Size.x * m_Size.y, 0);
	m_Texture.create(m_Size.x, m_Size.y);
	
	GenerateColors();
	 
}


Fractal::~Fractal()
{
}

void Fractal::Update()
{
	std::vector<sf::Uint8> pixels;
	int j = 0;
	for (auto val : m_Pixels)
	{
		std::tuple<uint8_t, uint8_t, uint8_t> value(0,0,0);
		if (val < m_Colors.size())
		{
			value = m_Colors.at(val);

		}
		else {
			value = m_Colors.at(m_Colors.size() - val);
		}
		
		pixels.push_back(std::get<0>(value));
		pixels.push_back(std::get<1>(value));
		pixels.push_back(std::get<2>(value));
		pixels.push_back(128);
	
	}
	m_Fractal.setTexture(m_Texture);
	m_Texture.update(pixels.data());
	
}



void Fractal::Generate(sf::Vector2f pxTl, sf::Vector2f pxBr, sf::Vector2f frTl, sf::Vector2f frBr)
{
	constexpr uint8_t u8_NoOfThreads = 32;
	int nSectionWidth = (pxBr.x - pxTl.x) / u8_NoOfThreads;
	
	double dFractalWidth = (frBr.x - frTl.x) / double(u8_NoOfThreads);

	std::thread t[u8_NoOfThreads];

	for (size_t i = 0; i < u8_NoOfThreads; i++)
		t[i] = std::thread(&Fractal::Calculate, this,
			sf::Vector2f(pxTl.x + nSectionWidth * (i), pxTl.y),
			sf::Vector2f(pxTl.x + nSectionWidth * (i + 1), pxBr.y),
			sf::Vector2f(frTl.x + dFractalWidth * double(i), frTl.y),
			sf::Vector2f(frTl.x + dFractalWidth * double(i + 1), frBr.y));

	for (size_t i = 0; i < u8_NoOfThreads; i++)
		t[i].join();

	
}




void Fractal::GenerateColors()
{
	int i = 0;
	for (i = 0; i < 256; i+=8)
	{
		for (int j = 0; j < 256; j+=8) {
			for (int k = 0; k < 256; k+=8) {
				m_Colors.push_back(std::make_tuple(i, j, k));

			}
		}
	}
}

void Fractal::Calculate(sf::Vector2f pxTl, sf::Vector2f pxBr, sf::Vector2f frTl, sf::Vector2f frBr)
{
	
	double x_scale = (frBr.x - frTl.x) / (double(pxBr.x) - double(pxTl.x));
	double y_scale = (frBr.y - frTl.y) / (double(pxBr.y) - double(pxTl.y));

	double yPos = frTl.y;
	int yOffset = 0;
	int rowSize = m_Size.x;

	int x, y;

	__m256d _a, _b, _two, _four, _mask1;
	__m256d _zr, _zi, _zr2, _zi2, _cr, _ci;
	__m256d _x_pos_offsets, _x_pos, _x_scale, _x_jump;
	__m256i _one, _c, _n, _iterations, _mask2;

	_one = _mm256_set1_epi64x(1);
	_two = _mm256_set1_pd(2.0);
	_four = _mm256_set1_pd(4.0);
	_iterations = _mm256_set1_epi64x(u32_NumberOfIterations);

	_x_scale = _mm256_set1_pd(x_scale);
	_x_jump = _mm256_set1_pd(x_scale * 4);
	_x_pos_offsets = _mm256_set_pd(0, 1, 2, 3);
	_x_pos_offsets = _mm256_mul_pd(_x_pos_offsets, _x_scale);


	for (y = pxTl.y; y < pxBr.y; y++)
	{
		// Reset x_position
		_a = _mm256_set1_pd(frTl.x);
		_x_pos = _mm256_add_pd(_a, _x_pos_offsets);

		_ci = _mm256_set1_pd(yPos);

		for (x = pxTl.x; x < pxBr.x; x += 4)
		{
			_cr = _x_pos;
			_zr = _mm256_setzero_pd();
			_zi = _mm256_setzero_pd();
			_n = _mm256_setzero_si256();


		repeat:
			_zr2 = _mm256_mul_pd(_zr, _zr);
			_zi2 = _mm256_mul_pd(_zi, _zi);
			_a = _mm256_sub_pd(_zr2, _zi2);
			_a = _mm256_add_pd(_a, _cr);
			_b = _mm256_mul_pd(_zr, _zi);
			_b = _mm256_fmadd_pd(_b, _two, _ci);
			_zr = _a;
			_zi = _b;
			_a = _mm256_add_pd(_zr2, _zi2);
			_mask1 = _mm256_cmp_pd(_a, _four, _CMP_LT_OQ);
			_mask2 = _mm256_cmpgt_epi64(_iterations, _n);
			_mask2 = _mm256_and_si256(_mask2, _mm256_castpd_si256(_mask1));
			_c = _mm256_and_si256(_one, _mask2); // Zero out ones where n < iterations													
			_n = _mm256_add_epi64(_n, _c); // n++ Increase all n
			if (_mm256_movemask_pd(_mm256_castsi256_pd(_mask2)) > 0)
				goto repeat;

			m_Pixels[yOffset + x + 0] = int(_n.m256i_i64[3]);
			m_Pixels[yOffset + x + 1] = int(_n.m256i_i64[2]);
			m_Pixels[yOffset + x + 2] = int(_n.m256i_i64[1]);
			m_Pixels[yOffset + x + 3] = int(_n.m256i_i64[0]);
			_x_pos = _mm256_add_pd(_x_pos, _x_jump);
		}

		yPos += y_scale;
		yOffset += rowSize;
	}



}

void Fractal::draw(sf::RenderTarget& target, sf::RenderStates states) const
{	
	
	target.draw(m_Fractal, states);
}
