#include "predict_trajectory.h"
#include <math.h>


PredictTrajectory::PredictTrajectory( std::string dataset_params,
                                      std::string experiment_path)
{
  JsonConfig json(dataset_params);

  this->experiment_path = experiment_path;

  padding = json.result["padding"].asInt();
  window_size = json.result["window_size"].asInt();


  cells_original  = new CellsLoad(dataset_params);

  width    = 3 + 2*padding;
  height   = window_size + 2*padding;
  channels = cells_original->testing_cells_count();

  sGeometry input_geometry, output_geometry;
  input_geometry.w = width;
  input_geometry.h = height;
  input_geometry.d = channels;

  output_geometry.w = 1;
  output_geometry.h = 1;
  output_geometry.d = 3;

  nn = new CNN(experiment_path+"trained/cnn_config.json", input_geometry, output_geometry);
}

PredictTrajectory::~PredictTrajectory()
{
  delete cells_original;
  delete nn;
}

void PredictTrajectory::run()
{
  std::vector<Log> trajectory_log;

  for (unsigned int particle = 0; particle < channels; particle++)
  {
    std::string trajectory_file_name = experiment_path + "trajectory/" + std::to_string(particle) + ".dat";
    trajectory_log.push_back(Log(trajectory_file_name));
  }

  std::string summary_log_file_name = experiment_path + "trajectory/summary.dat";
  Log summary_log(summary_log_file_name);

  unsigned int iteration = window_size*2;

  float error_accumualted = 0.0;
  for (; iteration < cells_original->testing_steps_count(); iteration++)
  {
    float error_all_particles = 0.0;

    for (unsigned int particle = 0; particle < channels; particle++)
    {
      sDatasetItem item;

      item = cells_original->create_testing_dataset_item(
                                                        iteration,
                                                        particle,
                                                        window_size,
                                                        padding
                                                      );

      std::vector<float> nn_output(item.output.size());

      nn->forward_vector(nn_output, item.input);

      float error = 0.0;
      for (unsigned int i = 0; i < item.output.size(); i++)
      {
        error+= (item.output[i] - nn_output[i])*(item.output[i] - nn_output[i]);
      }
      error = sqrt(error);
      error_all_particles+= error;

      trajectory_log[particle] << iteration << " ";

      trajectory_log[particle] << item.output[0] << " ";
      trajectory_log[particle] << item.output[1] << " ";
      trajectory_log[particle] << item.output[2] << " ";

      trajectory_log[particle] << nn_output[0] << " ";
      trajectory_log[particle] << nn_output[1] << " ";
      trajectory_log[particle] << nn_output[2] << " ";

      trajectory_log[particle] << error << "\n";
    }

    error_all_particles = error_all_particles/channels;

    error_accumualted+= error_all_particles;

    summary_log << iteration << " ";
    summary_log << error_all_particles << " ";
    summary_log << error_accumualted << "\n";
  }
}
