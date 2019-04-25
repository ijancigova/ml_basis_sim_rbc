#ifndef _CELLS_LOAD_H_
#define _CELLS_LOAD_H_

#include <json_config.h>
#include <cell_load.h>
#include <dataset_interface.h>

class CellsLoad
{
  private:
    Json::Value json_config;
    Log load_log;

    sExtremalValues extremal_values;

  public:
    std::vector<CellLoad*> cells_training;
    std::vector<CellLoad*> cells_testing;

  public:
    CellsLoad(std::string config_file_name);
    virtual ~CellsLoad();

    unsigned int training_cells_count()
    {
      return cells_training.size();
    }

    unsigned int testing_cells_count()
    {
      return cells_testing.size();
    }


    unsigned int training_steps_count()
    {
      return cells_training[0]->size();
    }

    unsigned int testing_steps_count()
    {
      return cells_testing[0]->size();
    }

    sDatasetItem create_training_dataset_item(  unsigned int time_idx,
                                                unsigned int center_cell_idx,
                                                unsigned int window_size,
                                                unsigned int padding );

    sDatasetItem create_testing_dataset_item( unsigned int time_idx,
                                              unsigned int center_cell_idx,
                                              unsigned int window_size,
                                              unsigned int padding );



  private:
    sDatasetItem create_dataset_item( unsigned int time_idx,
                                      unsigned int center_cell_idx,
                                      unsigned int window_size,
                                      unsigned int padding,
                                      std::vector<CellLoad*> &cells);

  private:
    int init();
    void load(std::vector<CellLoad*> &cells, std::string input_dir);

    std::vector<sCellPosition> compute_relative_position( std::vector<sCellPosition> &cell_position,
                                                          std::vector<sCellPosition> &center_cell_position);
    std::vector<sCellPosition> zero_position(unsigned int window_size);

    void speed_from_position();
    void find_values_ranges();
    void normalise();
    void find_values_ranges_cell(CellLoad &cell);

};


#endif
