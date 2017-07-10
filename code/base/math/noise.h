#pragma once

namespace base
{
//////////////////////////////////////////////////////////////////////////////
//   ~ N O I S E   CLASS ~  
//   Author: Chris Stephenson
//    Email: chris@icezip.com
//  Summary: A class for generating N dimensional noise
//           The noise generated has the following charcteristics:
//             - noise produced between -1 and +1
//             - pseudo-random (same value produced by same vector)
//             - features roughly the same size (no high or low spatial frequencies)
//////////////////////////////////////////////////////////////////////////////

auto cubic_spline = [](float t) { return (t * t * (3.0f - 2.0f * t)); }
auto linear_interpolation = [](float t, float a, float b) { return (a + t * (b - a)); }

template<int N> // N must be > 0
class Noise 
{
public:
	template<class R>

	Noise(R random)
	{
		int i, j, k;
		
		for(i = 0; i < buffer_size; i++)
		{
			// pointers
			pointers[i] = i;
		
			// gradients
			for(j = 0; j < N; j++)
				gradients[i][j] = (float)((random() % (buffer_size + buffer_size)) - buffer_size) / buffer_size;

			// normalize gradients
			float s = 0.0f;

			for(j = 0; j < N; j++)
				s += gradients[i][j] * gradients[i][j];
				s = 1.0f / sqrtf(s);
			
			for(j = 0; j < N; j++)
				gradients[i][j] *= s;
		}

		// randomize pointers
		while (--i)
		{
			k = pointers[i];
			pointers[i] = pointers[j = (random() & buffer_mask)];
			pointers[j] = k;
		}

		// copy pointers & gradients to 2nd half of array
		for(i = 0; i < buffer_size + buffer_pad; i++)
		{
			pointers[buffer_size + i] = pointers[i];
			for(j = 0; j < N; j++)
				gradients[buffer_size + i][j] = gradients[i][j];
		}
	}

	// get some noise based on vector vec
	float get(float vec[N])
	{
		int ivec[N];
		int p[N]; // pointer offsets
		float r[N]; // r + ivec = vec (to be dotted with the gradients)
		float s[N]; // interpolation values
		
		// set up all the variables
		for(int i = 0; i < N; i++)
		{
			ivec[i] = floorf(vec[i]);
			p[i] = ivec[i] & buffer_mask;
			r[i] = vec[i] - ivec[i];
			s[i] = cubic_spline(r[i]);
		}
		
		// call helper function
		return _get(p, r, s, 0, 0);	
	}
	
private:
	// recursive helper function for get(float vec[N])
	inline float _get(int p[N], float r[N], float s[N], int cumulative_p, int n)
	{
		if(n == N) // base case: return r dot gradient
		{
			float *g = gradients[cumulative_p];
			float val = 0.0f;
			for(int i = 0; i < N; i++)
				val += r[i] * g[i];
			return val;
		}
		else
		// recursive case: return interpolation of values
		{	float a, b;
			a = _get(p, r, s, pointers[cumulative_p + p[n]], n + 1);
			r[n]-=1.0f;
			b = _get(p, r, s, pointers[cumulative_p + p[n] + 1], n + 1);
			r[n]+=1.0f;
			return linear_interpolation(s[n], a, b);
		}
	}
	enum // some private constants
	{	buffer_size = 0x100, // buffer size, must be a power of 2 for mask to work
		buffer_mask = 0xff, // ANDing (&) is much faster than MODing (%)
		buffer_pad = 2 // a little extra space for simplification of methods
	};

	// important variables	int pointers[buffer_size + buffer_size + buffer_pad];
	float gradients[buffer_size + buffer_size + buffer_pad][N];
};

}
