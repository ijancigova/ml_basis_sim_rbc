#include "cell_load.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <math.h>


CellLoad::CellLoad(std::string file_name,
                   Log *load_log)
{
  this->load_log   = load_log;
  this->file_name  = file_name;

  load_file(file_name);
}

CellLoad::~CellLoad()
{

}


std::vector<sCellPosition> CellLoad::get_window(unsigned int idx, unsigned int window_size)
{
  std::vector<sCellPosition> result(window_size);

  for (unsigned int i = 0; i < window_size; i++)
  {
    result[i] = trajectory[idx - i];
  }

  return result;
}

void CellLoad::save(std::string file_name)
{
  Log cell_log(file_name);

  for (unsigned int i = 0; i < trajectory.size(); i++)
  {
    cell_log << i << " ";

    cell_log << trajectory[i].x << " ";
    cell_log << trajectory[i].y << " ";
    cell_log << trajectory[i].z << " ";

    cell_log << trajectory[i].vx << " ";
    cell_log << trajectory[i].vy << " ";
    cell_log << trajectory[i].vz << "\n";
  }
}

void CellLoad::print()
{
  for (unsigned int i = 0; i < trajectory.size(); i++)
  {
    std::cout << i << " ";

    std::cout << trajectory[i].x << " ";
    std::cout << trajectory[i].y << " ";
    std::cout << trajectory[i].z << " ";

    std::cout << trajectory[i].vx << " ";
    std::cout << trajectory[i].vy << " ";
    std::cout << trajectory[i].vz << "\n";
  }
}

void CellLoad::speed_from_position()
{
  trajectory[0].vx = 0.0;
  trajectory[0].vy = 0.0;
  trajectory[0].vz = 0.0;

  float k = 1.0;

  for (unsigned int i = 1; i < trajectory.size(); i++)
  {
    trajectory[i].vx = k*(trajectory[i].x - trajectory[i-1].x);
    trajectory[i].vy = k*(trajectory[i].y - trajectory[i-1].y);
    trajectory[i].vz = k*(trajectory[i].z - trajectory[i-1].z);
  }
}

void CellLoad::find_extremal_values()
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

  for (unsigned int i = 0; i < trajectory.size(); i++)
  {
    if (trajectory[i].x > extremal_values.max_values.x)
      extremal_values.max_values.x = trajectory[i].x;

    if (trajectory[i].y > extremal_values.max_values.y)
      extremal_values.max_values.y = trajectory[i].y;

    if (trajectory[i].z > extremal_values.max_values.z)
      extremal_values.max_values.z = trajectory[i].z;

    if (trajectory[i].vx > extremal_values.max_values.vx)
      extremal_values.max_values.vx = trajectory[i].vx;

    if (trajectory[i].vy > extremal_values.max_values.vy)
      extremal_values.max_values.vy = trajectory[i].vy;

    if (trajectory[i].vz > extremal_values.max_values.vz)
      extremal_values.max_values.vz = trajectory[i].vz;

    if (trajectory[i].x < extremal_values.min_values.x)
      extremal_values.min_values.x = trajectory[i].x;

    if (trajectory[i].y < extremal_values.min_values.y)
      extremal_values.min_values.y = trajectory[i].y;

    if (trajectory[i].z < extremal_values.min_values.z)
      extremal_values.min_values.z = trajectory[i].z;

    if (trajectory[i].vx < extremal_values.min_values.vx)
      extremal_values.min_values.vx = trajectory[i].vx;

    if (trajectory[i].vy < extremal_values.min_values.vy)
      extremal_values.min_values.vy = trajectory[i].vy;

    if (trajectory[i].vz < extremal_values.min_values.vz)
      extremal_values.min_values.vz = trajectory[i].vz;
  }
}


void CellLoad::load_file(std::string file_name)
{
  *load_log << "loading cell : " << file_name << " ";

  std::ifstream ifs(file_name);



  if (ifs.is_open())
  {
    std::string line;

    std::getline(ifs, line);

    while(std::getline(ifs, line))
    {
      remove_comma(line);

      sCellPosition result = parse_line(line);
      trajectory.push_back(result);
    }

    *load_log << " done\n";
  }
  else
    *load_log << " ERROR\n";

}


sCellPosition CellLoad::parse_line(std::string line)
{
  sCellPosition result;
  unsigned int iteration;

  std::stringstream ss(line);

  ss >> iteration;
  ss >> result.x;
  ss >> result.y;
  ss >> result.z;
  ss >> result.vx;
  ss >> result.vy;
  ss >> result.vz;

  return result;
}



void CellLoad::normalise()
{
  float k = 0.0, q = 0.0;

  float min_value = -1.0;
  float max_value = 1.0;

    k = (max_value - min_value)/(extremal_values.max_values.x - extremal_values.min_values.x);
    q = max_value - k*extremal_values.max_values.x;


    for (unsigned int i = 0; i < trajectory.size(); i++)
      trajectory[i].x = k*trajectory[i].x + q;

    if (extremal_values.max_values.y == extremal_values.min_values.y)
    {
      k = 0.0;
      q = 0.0;
    }
    else
    {
      k = (max_value - min_value)/(extremal_values.max_values.y - extremal_values.min_values.y);
      q = max_value - k*extremal_values.max_values.y;
    }

    for (unsigned int i = 0; i < trajectory.size(); i++)
      trajectory[i].y = k*trajectory[i].y + q;


    if (extremal_values.max_values.z == extremal_values.min_values.z)
    {
      k = 0.0;
      q = 0.0;
    }
    else
    {
      k = (max_value - min_value)/(extremal_values.max_values.z - extremal_values.min_values.z);
      q = max_value - k*extremal_values.max_values.z;
    }

    for (unsigned int i = 0; i < trajectory.size(); i++)
      trajectory[i].z = k*trajectory[i].z + q;


  {
    if (extremal_values.max_values.vx == extremal_values.min_values.vx)
    {
      k = 0.0;
      q = 0.0;
    }
    else
    {
      k = (max_value - min_value)/(extremal_values.max_values.vx - extremal_values.min_values.vx);
      q = max_value - k*extremal_values.max_values.vx;
    }

    for (unsigned int i = 0; i < trajectory.size(); i++)
      trajectory[i].vx = k*trajectory[i].vx + q;
  }

  {
    if (extremal_values.max_values.vy == extremal_values.min_values.vy)
    {
      k = 0.0;
      q = 0.0;
    }
    else
    {
      k = (max_value - min_value)/(extremal_values.max_values.vy - extremal_values.min_values.vy);
      q = max_value - k*extremal_values.max_values.vy;
    }

    for (unsigned int i = 0; i < trajectory.size(); i++)
      trajectory[i].vy = k*trajectory[i].vy + q;
  }

  {
    if (extremal_values.max_values.vz == extremal_values.min_values.vz)
    {
      k = 0.0;
      q = 0.0;
    }
    else
    {
      k = (max_value - min_value)/(extremal_values.max_values.vz - extremal_values.min_values.vz);
      q = max_value - k*extremal_values.max_values.vz;
    }

    for (unsigned int i = 0; i < trajectory.size(); i++)
      trajectory[i].vz = k*trajectory[i].vz + q;
  }

}





void CellLoad::remove_comma(std::string &str)
{
  for (unsigned int i = 0; i < str.size(); i++)
    if (str[i] == ',')
      str[i] = ' ';
}
