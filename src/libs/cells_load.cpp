#include "cells_load.h"

#include <experimental/filesystem>


CellsLoad::CellsLoad(std::string config_file_name)
{
  JsonConfig json(config_file_name);

  json_config = json.result;

  load_log.set_output_file_name(json_config["load_log_file_name"].asString());

  init();
}

CellsLoad::~CellsLoad()
{
  for (unsigned int i = 0; i < cells_training.size(); i++)
    delete cells_training[i];

  for (unsigned int i = 0; i < cells_testing.size(); i++)
    delete cells_testing[i];


  load_log << "CellsLoad uninit done\n";
}


sDatasetItem CellsLoad::create_training_dataset_item(  unsigned int time_idx,
                                                       unsigned int center_cell_idx,
                                                       unsigned int window_size,
                                                       unsigned int padding )
{
  return create_dataset_item(time_idx, center_cell_idx, window_size, padding, cells_training);
}

sDatasetItem CellsLoad::create_testing_dataset_item(  unsigned int time_idx,
                                                       unsigned int center_cell_idx,
                                                       unsigned int window_size,
                                                       unsigned int padding )
{
  return create_dataset_item(time_idx, center_cell_idx, window_size, padding, cells_testing);
}


sDatasetItem CellsLoad::create_dataset_item(  unsigned int time_idx,
                                              unsigned int center_cell_idx,
                                              unsigned int window_size,
                                              unsigned int padding,
                                              std::vector<CellLoad*> &cells)
{
  sDatasetItem result;

  unsigned int width    = 3 + 2*padding;
  unsigned int height   = window_size + 2*padding;
  unsigned int channels = training_cells_count();

  result.input.resize(width*height*channels);
  result.output.resize(3);

  for (unsigned int i = 0; i < result.input.size(); i++)
    result.input[i] = 0.0;

  for (unsigned int i = 0; i < result.output.size(); i++)
    result.output[i] = 0.0;

  sCellPosition future_center_cell_position = cells[center_cell_idx]->get(time_idx+1);
  std::vector<sCellPosition> center_cell_position = cells[center_cell_idx]->get_window(time_idx, window_size);

  result.output[0] = future_center_cell_position.x;
  result.output[1] = future_center_cell_position.y;
  result.output[2] = future_center_cell_position.z;


  unsigned int channel = 0;

  for (unsigned int y = 0; y < window_size; y++)
  {
    unsigned int idx = (channel*height + y + padding)*width + padding;

    result.input[idx + 0] = center_cell_position[y].x;
    result.input[idx + 1] = center_cell_position[y].y;
    result.input[idx + 2] = center_cell_position[y].z;
  }

  if (channels == 1)
    return result;

  channel++;

  for (unsigned int cell = 0; cell < training_cells_count(); cell++)
  {
    if (cell == center_cell_idx)
      continue;

    std::vector<sCellPosition> absolute_cell_position = cells[cell]->get_window(time_idx, window_size);
    std::vector<sCellPosition> cell_position = compute_relative_position(absolute_cell_position, center_cell_position);

    for (unsigned int y = 0; y < window_size; y++)
    {
      unsigned int idx = (channel*height + y + padding)*width + padding;

      result.input[idx + 0] = cell_position[y].x;
      result.input[idx + 1] = cell_position[y].y;
      result.input[idx + 2] = cell_position[y].z;
    }

    channel++;
  }

  return result;
}

int CellsLoad::init()
{
  load_log << "CellsLoad initializing\n";

  load_log << "\n\n";

  load_log << "loading training\n";
  for (unsigned int i = 0; i < json_config["training"].size(); i++)
    load(cells_training, json_config["training"][i].asString());

  load_log << "training items " << cells_training.size() << "\n";


  load_log << "\n\n";
  load_log << "loading testing\n";
  for (unsigned int i = 0; i < json_config["testing"].size(); i++)
    load(cells_testing, json_config["testing"][i].asString());

  load_log << "testing items " << cells_testing.size() << "\n";

  speed_from_position();
  find_values_ranges();
  normalise();


  load_log << "\n\n";

  load_log << "position range : ";

  load_log <<  extremal_values.min_values.x << " ";
  load_log <<  extremal_values.min_values.y << " ";
  load_log <<  extremal_values.min_values.z << " ";
  load_log << " : ";
  load_log <<  extremal_values.max_values.x << " ";
  load_log <<  extremal_values.max_values.y << " ";
  load_log <<  extremal_values.max_values.z << "\n";

  load_log << "speed range : ";

  load_log <<  extremal_values.min_values.vx << " ";
  load_log <<  extremal_values.min_values.vy << " ";
  load_log <<  extremal_values.min_values.vz << " ";
  load_log << " : ";
  load_log <<  extremal_values.max_values.vx << " ";
  load_log <<  extremal_values.max_values.vy << " ";
  load_log <<  extremal_values.max_values.vz << "\n";

/*
  for (unsigned int i = 0; i < cells_training.size(); i++)
  {
    std::string cell_file_name = "cells_training/";
    cell_file_name+= std::to_string(i)+".log";
    cells_training[i]->save(cell_file_name);
  }

  for (unsigned int i = 0; i < cells_testing.size(); i++)
  {
    std::string cell_file_name = "cells_testing/";
    cell_file_name+= std::to_string(i)+".log";
    cells_testing[i]->save(cell_file_name);
  }
*/

  return 0;
}



void CellsLoad::load(std::vector<CellLoad*> &cells, std::string input_dir)
{
   for (auto & p : std::experimental::filesystem::directory_iterator(input_dir))
   {
     std::string cell_file;
     cell_file = p.path();

     if (std::experimental::filesystem::path(cell_file).extension() == ".dat")
       cells.push_back(new CellLoad(cell_file, &load_log));
   }
}

std::vector<sCellPosition> CellsLoad::compute_relative_position( std::vector<sCellPosition> &cell_position,
                                                                 std::vector<sCellPosition> &center_cell_position)
{
   std::vector<sCellPosition> result(cell_position.size());

   for (unsigned int i = 0; i < cell_position.size(); i++)
   {
     result[i].x = center_cell_position[i].x - cell_position[i].x;
     result[i].y = center_cell_position[i].y - cell_position[i].y;
     result[i].z = center_cell_position[i].z - cell_position[i].z;
   }

   return result;
 }

  std::vector<sCellPosition> CellsLoad::zero_position(unsigned int window_size)
  {
    std::vector<sCellPosition> result(window_size);

    for (unsigned int i = 0; i < result.size(); i++)
    {
      result[i].x = 0.0;
      result[i].y = 0.0;
      result[i].z = 0.0;
    }

    return result;
  }

void CellsLoad::speed_from_position()
{
  for (unsigned int i = 0; i < cells_training.size(); i++)
    cells_training[i]->speed_from_position();

  for (unsigned int i = 0; i < cells_testing.size(); i++)
    cells_testing[i]->speed_from_position();
}

void CellsLoad::find_values_ranges()
{
    float max = -1000000000.0;

    extremal_values.max_values.x = max;
    extremal_values.max_values.y = max;
    extremal_values.max_values.z = max;

    extremal_values.max_values.vx = max;
    extremal_values.max_values.vy = max;
    extremal_values.max_values.vz = max;

    extremal_values.min_values.x =  -max;
    extremal_values.min_values.y =  -max;
    extremal_values.min_values.z =  -max;

    extremal_values.min_values.vx = -max;
    extremal_values.min_values.vy = -max;
    extremal_values.min_values.vz = -max;

    for (unsigned i = 0; i < cells_training.size(); i++)
      find_values_ranges_cell(*cells_training[i]);

    for (unsigned i = 0; i < cells_testing.size(); i++)
      find_values_ranges_cell(*cells_testing[i]);
}

void CellsLoad::normalise()
{
  for (unsigned i = 0; i < cells_training.size(); i++)
  {
    cells_training[i]->set_extremal_values(extremal_values);
    cells_training[i]->normalise();
  }
  for (unsigned i = 0; i < cells_testing.size(); i++)
  {
    cells_testing[i]->set_extremal_values(extremal_values);
    cells_testing[i]->normalise();
  }

}

void CellsLoad::find_values_ranges_cell(CellLoad &cell)
{
  cell.find_extremal_values();

  sExtremalValues cell_extremal = cell.get_extremal_values();

  if (cell_extremal.max_values.x > extremal_values.max_values.x)
    extremal_values.max_values.x = cell_extremal.max_values.x;

  if (cell_extremal.max_values.y > extremal_values.max_values.y)
    extremal_values.max_values.y = cell_extremal.max_values.y;

  if (cell_extremal.max_values.z > extremal_values.max_values.z)
    extremal_values.max_values.z = cell_extremal.max_values.z;

  if (cell_extremal.max_values.vx > extremal_values.max_values.vx)
    extremal_values.max_values.vx = cell_extremal.max_values.vx;

  if (cell_extremal.max_values.vy > extremal_values.max_values.vy)
    extremal_values.max_values.vy = cell_extremal.max_values.vy;

  if (cell_extremal.max_values.vz > extremal_values.max_values.vz)
    extremal_values.max_values.vz = cell_extremal.max_values.vz;

  if (cell_extremal.min_values.x < extremal_values.min_values.x)
    extremal_values.min_values.x = cell_extremal.min_values.x;

  if (cell_extremal.min_values.y < extremal_values.min_values.y)
    extremal_values.min_values.y = cell_extremal.min_values.y;

  if (cell_extremal.min_values.z < extremal_values.min_values.z)
    extremal_values.min_values.z = cell_extremal.min_values.z;

  if (cell_extremal.min_values.vx < extremal_values.min_values.vx)
    extremal_values.min_values.vx = cell_extremal.min_values.vx;

  if (cell_extremal.min_values.vy < extremal_values.min_values.vy)
    extremal_values.min_values.vy = cell_extremal.min_values.vy;

  if (cell_extremal.min_values.vz < extremal_values.min_values.vz)
    extremal_values.min_values.vz = cell_extremal.min_values.vz;
}
