#include "basis_experiment.h"



BasisExperiment::BasisExperiment(std::string dataset_params, std::string result_path)
{
  cells  = new CellsLoad(dataset_params);
  this->result_path = result_path;

  basis_count = 8000;  
  training_iterations = 50*basis_count;

  basis = new Basis(3, 3, 100.0, basis_count);
}

BasisExperiment::~BasisExperiment()
{
  delete cells;
  delete basis;
}



void BasisExperiment::learn()
{
  for (unsigned int i = 0; i < training_iterations; i++)
  {
    CellLoad *cell = cells->cells_training[rand()%cells->training_cells_count()];

    unsigned int random_idx = rand()%(cell->size()-1);

    sCellPosition required_position = cell->get(random_idx);
    sCellPosition required_position_next = cell->get(random_idx+1);

    std::vector<float> input(3);
    std::vector<float> output(3);

    input[0] = required_position.x;
    input[1] = required_position.y;
    input[2] = required_position.z;


    output[0] = required_position_next.x - required_position.x;
    output[1] = required_position_next.y - required_position.y;
    output[2] = required_position_next.z - required_position.z;
    /*
    output[0] = required_position.vx;
    output[1] = required_position.vy;
    output[2] = required_position.vz;
*/
    basis->learn(output, input);

    if ((i%100) == 0)
    {
      printf("training done %6.3f %%\n", i*100.0/training_iterations);
    }
  }

  basis->save(result_path + "basis.dat");
  basis->save_testing_points(result_path + "testing_points.dat", 30000);
}

void BasisExperiment::process()
{
  std::vector<sCellPosition> positions;

  for (unsigned int cell = 0; cell < cells->testing_cells_count(); cell++)
  {
    sCellPosition position = cells->cells_testing[cell]->get(0);
    positions.push_back(position);
  }

  std::vector<Log> trajectory_log;

  for (unsigned int cell = 0; cell < cells->testing_cells_count(); cell++)
  {
    std::string trajectory_file_name = result_path + "trajectory_prediction_error_" + std::to_string(cell) + ".dat";
    trajectory_log.push_back(Log(trajectory_file_name));
  }

  std::string summary_log_file_name = result_path + "summary.dat";
  Log summary_log(summary_log_file_name);

  float dt = 1.0/100.0;

  for (unsigned int iteration = 0; iteration < cells->testing_steps_count(); iteration++)
  {
    float error_sum = 0.0;
    float error_max = -1000000000.0;
    float error_min = -error_max;

    for (unsigned int cell = 0; cell < cells->testing_cells_count(); cell++)
    {
      sCellPosition required_position = cells->cells_testing[cell]->get(iteration);

      float error = 0.0;

      error+= pow(required_position.x - positions[cell].x, 2.0);
      error+= pow(required_position.y - positions[cell].y, 2.0);
      error+= pow(required_position.z - positions[cell].z, 2.0);
      error = sqrt(error);

      error_sum+= error/cells->testing_cells_count();

      if (error > error_max)
        error_max = error;

      if (error < error_min)
        error_min = error;

      trajectory_log[cell] << iteration << " ";

      trajectory_log[cell] << required_position.x << " ";
      trajectory_log[cell] << required_position.y << " ";
      trajectory_log[cell] << required_position.z << " ";

      trajectory_log[cell] << positions[cell].x << " ";
      trajectory_log[cell] << positions[cell].y << " ";
      trajectory_log[cell] << positions[cell].z << " ";

      trajectory_log[cell] << error << "\n";


      std::vector<float> position_v(3);
      position_v[0] = required_position.x;
      position_v[1] = required_position.y;
      position_v[2] = required_position.z;

      std::vector<float> speed = basis->get(position_v);

      positions[cell].x+= speed[0];
      positions[cell].y+= speed[1];
      positions[cell].z+= speed[2];

      if (positions[cell].x > 1.0)
        positions[cell].x = 1.0;

      if (positions[cell].x < -1.0)
        positions[cell].x = -1.0;

      if (positions[cell].y > 1.0)
        positions[cell].y = 1.0;

      if (positions[cell].y < -1.0)
        positions[cell].y = -1.0;

      if (positions[cell].z > 1.0)
        positions[cell].z = 1.0;

      if (positions[cell].z < -1.0)
        positions[cell].z = -1.0;
    }


    float error_average = error_sum;

    summary_log << iteration << " ";
    summary_log << error_average << " ";
    summary_log << error_min << " ";
    summary_log << error_max << "\n";

    if ((iteration%100) == 0)
      printf("done %6.3f %%\n", iteration*100.0/cells->testing_steps_count());
  }
}




void BasisExperiment::process_local_prediction_error()
{
  std::vector<Log> trajectory_log;

  for (unsigned int cell = 0; cell < cells->testing_cells_count(); cell++)
  {
    std::string trajectory_file_name = result_path + "local_position_prediction_error_" + std::to_string(cell) + ".dat";
    trajectory_log.push_back(Log(trajectory_file_name));
  }

  for (unsigned int iteration = 0; iteration < cells->testing_steps_count(); iteration++)
  {
    for (unsigned int cell = 0; cell < cells->testing_cells_count(); cell++)
    {
      sCellPosition required_position = cells->cells_testing[cell]->get(iteration);

      trajectory_log[cell] << iteration << " ";

      trajectory_log[cell] << required_position.x << " ";
      trajectory_log[cell] << required_position.y << " ";
      trajectory_log[cell] << required_position.z << " ";

      trajectory_log[cell] << required_position.vx << " ";
      trajectory_log[cell] << required_position.vy << " ";
      trajectory_log[cell] << required_position.vz << " ";


      std::vector<float> position_v(3);
      position_v[0] = required_position.x;
      position_v[1] = required_position.y;
      position_v[2] = required_position.z;

      std::vector<float> speed = basis->get(position_v);

      trajectory_log[cell] << speed[0] << " ";
      trajectory_log[cell] << speed[1] << " ";
      trajectory_log[cell] << speed[2] << "\n";
    }

    if ((iteration%100) == 0)
      printf("done %6.3f %%\n", iteration*100.0/cells->testing_steps_count());
  }
}




void BasisExperiment::predict_on_random_seed(std::string config_file_name)
{
  CellsLoad cells(config_file_name);

  std::vector<sCellPosition> positions(cells.training_cells_count());

  for (unsigned int i = 0; i < positions.size(); i++)
  {
    positions[i] = cells.cells_training[i]->get(0);
  }

  std::vector<Log> trajectory_log;

  for (unsigned int cell = 0; cell < cells.training_cells_count(); cell++)
  {
    std::string trajectory_file_name = result_path + "trajectory_random_seed_" + std::to_string(cell) + ".dat";
    trajectory_log.push_back(Log(trajectory_file_name));
  }


  for (unsigned int iteration = 0; iteration < 6000; iteration++)
  {
    for (unsigned int cell = 0; cell < cells.training_cells_count(); cell++)
    {
      trajectory_log[cell] << iteration << " ";

      trajectory_log[cell] << positions[cell].x << " ";
      trajectory_log[cell] << positions[cell].y << " ";
      trajectory_log[cell] << positions[cell].z << "\n";


      std::vector<float> position_v(3);
      position_v[0] = positions[cell].x;
      position_v[1] = positions[cell].y;
      position_v[2] = positions[cell].z;

      std::vector<float> speed = basis->get(position_v);

      positions[cell].x+= speed[0];
      positions[cell].y+= speed[1];
      positions[cell].z+= speed[2];

      if (positions[cell].x > 1.0)
        positions[cell].x = 1.0;

      if (positions[cell].x < -1.0)
        positions[cell].x = -1.0;

      if (positions[cell].y > 1.0)
        positions[cell].y = 1.0;

      if (positions[cell].y < -1.0)
        positions[cell].y = -1.0;

      if (positions[cell].z > 1.0)
        positions[cell].z = 1.0;

      if (positions[cell].z < -1.0)
        positions[cell].z = -1.0;
    }
  }
}

void BasisExperiment::save_basis()
{
  unsigned int height = 300;
  unsigned int width  = 900;

  Image image_xyz(width, height);
  Image image_x(width, height);
  Image image_y(width, height);
  Image image_z(width, height);

  for (unsigned int y = 0; y < height; y++)
    for (unsigned int x = 0; x < width; x++)
    {
      float y_ = ((y*1.0/height) - 0.5)*2.0;
      float x_ = ((x*1.0/width) - 0.5)*2.0;
      float z_ = 0.0;

      std::vector<float> input(3);
      input[0] = x_;
      input[1] = y_;
      input[2] = z_;


      std::vector<float> output;
      output = basis->get(input);

      for (unsigned int i = 0; i < output.size(); i++)
        output[i] = (output[i] + 1.0)/2.0;

      image_xyz.pixels[y][x].b[0] = output[0];
      image_xyz.pixels[y][x].b[1] = output[1];
      image_xyz.pixels[y][x].b[2] = output[2];

      image_x.pixels[y][x].b[0] = output[0];
      image_x.pixels[y][x].b[1] = output[0];
      image_x.pixels[y][x].b[2] = output[0];

      image_y.pixels[y][x].b[0] = output[1];
      image_y.pixels[y][x].b[1] = output[1];
      image_y.pixels[y][x].b[2] = output[1];

      image_z.pixels[y][x].b[0] = output[2];
      image_z.pixels[y][x].b[1] = output[2];
      image_z.pixels[y][x].b[2] = output[2];
    }

  image_xyz.save(result_path+"basis_xyz.png");
  image_x.save(result_path+"basis_x.png");
  image_y.save(result_path+"basis_y.png");
  image_z.save(result_path+"basis_z.png");
}
