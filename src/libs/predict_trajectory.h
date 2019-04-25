#ifndef _PREDICT_TRAJECTORY_H_
#define _PREDICT_TRAJECTORY_H_


#include <cells_load.h>
#include <cnn.h>
#include <log.h>

class PredictTrajectory
{
  private:
    std::string experiment_path;

    CellsLoad *cells_original;
    CNN *nn;

    unsigned int padding, window_size;
    unsigned int width, height, channels;

  public:
    PredictTrajectory(std::string dataset_params,
                      std::string experiment_path);

    virtual ~PredictTrajectory();

    void run();

};

#endif
