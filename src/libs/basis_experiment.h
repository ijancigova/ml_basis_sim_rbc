#ifndef _BASIS_EXPERIMENT_H_
#define _BASIS_EXPERIMENT_H_

#include "basis.h"
#include "cells_load.h"
#include <image.h>

class BasisExperiment
{
  private:
    CellsLoad *cells;
    Basis *basis;

    std::string result_path;

    unsigned int training_iterations, basis_count;

  public:
    BasisExperiment(std::string dataset_params, std::string result_path);
    virtual ~BasisExperiment();

    void learn();
    void process();

    void predict_on_random_seed(std::string config_file_name);
    void process_local_prediction_error();
    
    void save_basis();
};

#endif
