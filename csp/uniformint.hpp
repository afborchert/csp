#ifndef UNIFORM_INT_HPP
#define UNIFORM_INT_HPP

#include <random>

/* simple class for a pseudo-random generator producing
   uniformely distributed integers */
class UniformIntDistribution {
   public:
      UniformIntDistribution() : engine(std::random_device()()) {}
      /* return number in the range of [0..upper_limit) */
      unsigned int draw(unsigned int upper_limit) {
	 return std::uniform_int_distribution<unsigned int>
	    (0, upper_limit-1)(engine);
      }
      bool flip() {
	 return draw(2);
      }
   private:
      std::mt19937 engine;
};

#endif
