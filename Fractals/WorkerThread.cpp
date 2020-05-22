#include "stdafx.h"
#include "WorkerThread.h"

WorkerThread::WorkerThread()
{
	m_PixTl=sf::Vector2f(0.0f,0.0f);
	m_PixBr=sf::Vector2f(0.0f, 0.0f);
	m_FracTl = sf::Vector2f(0.0f, 0.0f);
	m_FracBr = sf::Vector2f(0.0f, 0.0f);
	u32_NumberOfIterations = 0;
	b_IsAlive = true;
	n_ScreenWidth = 0;
	m_Pixels = nullptr;
}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::Start(const sf::Vector2f pixTl, const sf::Vector2f pixBr, const sf::Vector2f fracTl, const sf::Vector2f fracBr, const uint32_t iterations)
{
	m_PixTl = pixTl;
	m_PixBr = pixBr;
	m_FracTl = fracTl;
	m_FracBr = fracBr;
	u32_NumberOfIterations = iterations;
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_ConditionStart.notify_one();
}

void WorkerThread::CreateFractal()
{
	while (b_IsAlive) {
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_ConditionStart.wait(lock);

		double x_scale = (m_FracBr.x - m_FracTl.x) / (double(m_FracBr.x) - double(m_PixTl.x));
		double y_scale = (m_FracBr.y - m_FracTl.y) / (double(m_FracBr.y) - double(m_PixTl.y));

		double yPos = m_FracTl.y;
		int yOffset = 0;
		int rowSize = n_ScreenWidth;

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


		for (y = m_PixTl.y; y < m_PixBr.y; y++)
		{
			// Reset x_position
			_a = _mm256_set1_pd(m_FracTl.x);
			_x_pos = _mm256_add_pd(_a, _x_pos_offsets);

			_ci = _mm256_set1_pd(yPos);

			for (x = m_PixTl.x; x < m_PixBr.x; x += 4)
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
}
