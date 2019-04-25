#ifndef _CELL_LOAD_H_
#define _CELL_LOAD_H_

#include <string>
#include <vector>
#include <log.h>

struct sCellPosition
{
  float x, y, z;
  float vx, vy, vz;
};

struct sExtremalValues
{
  sCellPosition min_values, max_values;
};

class CellLoad
{
  private:

    Log *load_log;

    std::vector<sCellPosition> trajectory;

    std::string file_name;

    sExtremalValues extremal_values;

  public:

    CellLoad( std::string file_name,
              Log *load_log);

    virtual ~CellLoad();

    unsigned int size()
    {
      return trajectory.size();
    }

    sCellPosition get(unsigned int idx)
    {
      if (idx >= trajectory.size())
        idx = trajectory.size()-1;
        
      return trajectory[idx];
    }

    std::string get_name()
    {
      return file_name;
    }

    std::vector<sCellPosition> get_window(unsigned int idx, unsigned int window_size);

    void save(std::string file_name);
    void print();


    sExtremalValues get_extremal_values()
    {
      return extremal_values;
    }

    void set_extremal_values(sExtremalValues extremal_values)
    {
      this->extremal_values = extremal_values;
    }

    void speed_from_position();

    void find_extremal_values();

    void normalise();




  private:
    void load_file(std::string file_name);
    sCellPosition parse_line(std::string line);

    void remove_comma(std::string &str);

};


#endif
