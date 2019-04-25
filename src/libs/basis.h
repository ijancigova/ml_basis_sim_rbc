#ifndef _BASIS_H_
#define _BASIS_H_

#include <vector>
#include <string>
#include <log.h>

class Basis
{
  private:
    std::vector<float> distances;

    std::vector<std::vector<float>> b_input;
    std::vector<std::vector<float>> b_output;

    float k;
    unsigned int basis_count, used_ptr;
    unsigned int input_size, output_size;

    unsigned int closest_base;

  public:
    Basis(unsigned int input_size, unsigned int output_size,
          float k, unsigned int basis_count);

    virtual ~Basis();


    void learn(std::vector<float> &output, std::vector<float> &input);

    std::vector<float> get(std::vector<float> &input);

    void save(std::string file_name);
    void save_testing_points(std::string file_name, unsigned int points_count);


  private:
    void compute_distances(std::vector<float> &input);
    float similarity_function(std::vector<float> &va, std::vector<float> &vb, float param);
    float rnd();
};

#endif
