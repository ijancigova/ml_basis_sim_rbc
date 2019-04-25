#ifndef _TRAJECTORY_EXPERIMENT_H_
#define _TRAJECTORY_EXPERIMENT_H_

#include <dataset_interface.h>
#include <batch.h>

#include <cnn.h>
#include <log.h>


struct sTrajectoryTestResult
{
  bool output_valid;
  bool too_big_difference;
  bool best_net;
  bool long_term_without_improvement;

  float dif_error;
  float dif_error_min;
  float dif_error_max;

  float nn_error;
  float nn_error_min;
  float nn_error_max;
};

class TrajectoryExperiment
{
  protected:
    Json::Value       parameters;
    DatasetInterface  *dataset;
    Batch             *batch;

    sGeometry input_geometry, output_geometry;

  protected:
    std::string log_prefix;
    Log training_log;
    Log experiment_log;



  private:
    sTrajectoryTestResult result, best_result;

    unsigned int init_networks_try_count, epoch_count, epoch_without_improvement;
    float learning_rate, lambda, learning_rate_decay;

    float error_best;


  public:
    TrajectoryExperiment(std::string parameters_file_name, DatasetInterface *dataset);
    virtual ~TrajectoryExperiment();

    void run();

    sTrajectoryTestResult get_best_result()
    {
      return best_result;
    }

  private:
    void init();

  protected:
    virtual void best_net_log_process();

  private:
    int search_initial_net();
    CNN* load_saved_net(std::string best_net_file_name);


    sTrajectoryTestResult test(CNN *nn, bool quick_test = false);
    bool check_valid(std::vector<float> &v);


    float dif_prediction(std::vector<float> &input, std::vector<float> &required_output);
    float nn_prediction(CNN *nn, std::vector<float> &input, std::vector<float> &required_output);

    float rms(std::vector<float> &required, std::vector<float> &testing);
    void print_vector(std::vector<float> &v);

  protected:
    virtual void fill_batch();
    void save_examples(CNN *nn);

};

#endif
