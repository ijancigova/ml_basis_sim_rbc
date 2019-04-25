#include "cells_dataset.h"

#include <math.h>

CellsDataset::CellsDataset(std::string config_file_name)
             :DatasetInterface()
{
  cells_load = new CellsLoad(config_file_name);

  JsonConfig json(config_file_name);

  padding = json.result["padding"].asInt();
  window_size = json.result["window_size"].asInt();

  width    = 3 + 2*padding;
  height   = window_size + 2*padding;
  channels = cells_load->training_cells_count();

  unsigned int step_increment = json.result["dataset_step_increment"].asInt();

  if (step_increment == 0)
    step_increment = 1;

  for (unsigned int cell = 0; cell < cells_load->training_cells_count(); cell++)
  for (unsigned int step = window_size; step < cells_load->training_steps_count(); step+= step_increment)
  {
    sDatasetItem item = cells_load->create_training_dataset_item(step, cell, window_size, padding);

    training.push_back(item);
  }


  for (unsigned int cell = 0; cell < cells_load->training_cells_count(); cell++)
  for (unsigned int step = window_size; step < cells_load->testing_steps_count(); step+= step_increment)
  {
    sDatasetItem item = cells_load->create_testing_dataset_item(step, cell, window_size, padding);
 
    testing.push_back(item);
  }


  for (unsigned int i = 0 ; i < 20; i++)
    print_testing_item(rand()%testing.size());
/*
  for (unsigned int i = 0 ; i < 1000; i++)
    save_training_item(rand()%training.size(), i);
*/

  printf("CellsDataset size : %u %u\n", (unsigned int)training.size(), (unsigned int)testing.size());
}

CellsDataset::~CellsDataset()
{
  delete cells_load;
}

void CellsDataset::save_training_item(unsigned int idx, unsigned int name_idx)
{
  std::string file_name;
  file_name = "examples/" + std::to_string(name_idx) + ".log";

  Log example_log(file_name);

  unsigned int line_size = 2*padding + 3;
  unsigned int ptr = padding + padding*(line_size);

  for (unsigned int j = 0; j < height-2*padding; j++)
  {
    example_log << training[idx].input[ptr+0] << " ";
    example_log << training[idx].input[ptr+1] << " ";
    example_log << training[idx].input[ptr+2] << "\n";

    ptr+= line_size;
  }
}
