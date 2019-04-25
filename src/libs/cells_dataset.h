#ifndef _CELLS_DATASET_H_
#define _CELLS_DATASET_H_

#include <cells_load.h>
#include <dataset_interface.h>

class CellsDataset: public DatasetInterface
{
  private:

    CellsLoad *cells_load;

    unsigned int padding, window_size;

  public:
    CellsDataset(std::string config_file_name);
    virtual ~CellsDataset();

    void save_training_item(unsigned int idx, unsigned int name_idx);



};


#endif
