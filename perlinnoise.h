#pragma once

//#include "ridgemultifrac.h"

class PerlinNoise
{
public:

  // Constructor
    PerlinNoise();
    PerlinNoise(float _persistence, float _frequency, float _amplitude, int _octaves, int _randomseed);


  // Get Height
    float GetHeight(float x, float y) ;

  // Get
  float Persistence()  { return persistence; }
  float Frequency()    { return frequency;   }
  float Amplitude()    { return amplitude;   }
  int    Octaves()      { return octaves;     }
  int    RandomSeed()   { return randomseed;  }

  // Set
  void Set(float _persistence, float _frequency, float _amplitude, int _octaves, int _randomseed);

  void SetPersistence(float _persistence) { persistence = _persistence; }
  void SetFrequency(  float _frequency)   { frequency = _frequency;     }
  void SetAmplitude(  float _amplitude)   { amplitude = _amplitude;     }
  void SetOctaves(    int    _octaves)     { octaves = _octaves;         }
  void SetRandomSeed( int    _randomseed)  { randomseed = _randomseed;   }

private:

    float Total(float i, float j);
    float GetValue(float x, float y);
    float Interpolate(float x, float y, float a);
    float Noise(int x, int y);

    float persistence, frequency, amplitude;
    int octaves, randomseed;
    //RidgedMF
    //ridgemultifrac Ridges;
};
