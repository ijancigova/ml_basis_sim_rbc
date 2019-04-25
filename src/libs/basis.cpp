#include "basis.h"
#include <math.h>
#include <stdio.h>

Basis::Basis( unsigned int input_size, unsigned int output_size,
              float k, unsigned int basis_count)
{
  this->input_size = input_size;
  this->output_size = output_size;
  this->k = k;
  this->basis_count = basis_count;

  distances.resize(basis_count);
  for (unsigned int j = 0; j < distances.size(); j++)
    distances[j] = 0.0;

  used_ptr = 0;

  std::vector<float> input(input_size);
  for (unsigned int i = 0; i < input.size(); i++)
    input[i] = 0.0;

  std::vector<float> output(output_size);
  for (unsigned int i = 0; i < output.size(); i++)
    output[i] = 0.0;

  for (unsigned int i = 0; i < basis_count; i++)
  {
    for (unsigned int i = 0; i < input.size(); i++)
     input[i] = rnd();

    for (unsigned int i = 0; i < output.size(); i++)
     output[i] = 0.0;


    b_input.push_back(input);
    b_output.push_back(output);
  }
}

Basis::~Basis()
{

}


void Basis::learn(std::vector<float> &output, std::vector<float> &input)
{
  if (used_ptr < basis_count)
  {
    b_input[used_ptr] = input;
    b_output[used_ptr] = output;
    used_ptr++;
  }
  else
  {
    compute_distances(input);

    float learning_rate = 0.1;

    for (unsigned int i = 0; i < input_size; i++)
      b_input[closest_base][i]+= learning_rate*(input[i] - b_input[closest_base][i]);

    for (unsigned int i = 0; i < output_size; i++)
      b_output[closest_base][i]+= learning_rate*(output[i] - b_output[closest_base][i]);
  }

}

std::vector<float> Basis::get(std::vector<float> &input)
{
  std::vector<float> result(output_size);

  compute_distances(input);

  for (unsigned int i = 0; i < output_size; i++)
    result[i] = 0.0;

  for (unsigned int j = 0; j < used_ptr; j++)
  {
    for (unsigned int i = 0; i < output_size; i++)
      result[i]+= distances[j]*b_output[j][i];
  }

  return result;
}


void Basis::compute_distances(std::vector<float> &input)
{
  for (unsigned int j = 0; j < distances.size(); j++)
    distances[j] = 0.0;

  float min = 1000000000.0;
  float max = -min;

  closest_base = 0;

  for (unsigned int j = 0; j < used_ptr; j++)
  {
    distances[j] = similarity_function(b_input[j], input, k);

    if (distances[j] < min)
      min = distances[j];

    if (distances[j] > max)
    {
      max = distances[j];
      closest_base = j;
    }
  }

  float sum = 0.0;
  for (unsigned int j = 0; j < used_ptr; j++)
    sum+= distances[j];


  if (sum > 0.0)
  {
    for (unsigned int j = 0; j < used_ptr; j++)
      distances[j]/= sum;
  }
  else
  {
    for (unsigned int j = 0; j < used_ptr; j++)
      distances[j] = 0.0;
  }
}



float Basis::similarity_function(std::vector<float> &va, std::vector<float> &vb, float param)
{
  float result = 0.0;

  for (unsigned int i = 0; i < va.size(); i++)
  {
    float tmp = va[i] - vb[i];
    result+= tmp*tmp;
  } 

 result = exp(-k*result);


//  result = 1.0 - result/(k + result);
  return result;
}

void Basis::save(std::string file_name)
{
  printf("Basis::save %u %u\n", b_input.size(), b_output.size());

  {
    Log result_log(file_name);


    for (unsigned int j = 0; j < b_input.size(); j++)
    {
      for (unsigned int i = 0; i < b_input[j].size(); i++)
        result_log << b_input[j][i] << " ";

      for (unsigned int i = 0; i < b_output[j].size(); i++)
        result_log << b_output[j][i] << " ";

      result_log << "\n";
    }
  }
  printf("Basis::save DONE\n");

}

void Basis::save_testing_points(std::string file_name, unsigned int points_count)
{
  Log result_log(file_name);

  std::vector<float> input(input_size);

  for (unsigned int j = 0; j < points_count; j++)
  {
    for (unsigned int i = 0; i < input_size; i++)
      input[i] = rnd();

    auto output = get(input);

    for (unsigned int i = 0; i < input_size; i++)
      result_log << input[i] << " ";

    for (unsigned int i = 0; i < output_size; i++)
      result_log << output[i] << " ";

    result_log << "\n";
  }
}

float Basis::rnd()
{
  return (rand()%2000000)/1000000.0 - 1.0;
}
