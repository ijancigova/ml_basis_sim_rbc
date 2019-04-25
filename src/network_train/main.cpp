#include <cells_load.h>
#include <cells_dataset.h>

#include <trajectory_experiment.h>

#include <stdio.h>

#include <dataset_testing.h>


int main()
{
  srand(time(NULL));

  {
    CellsDataset dataset("dataset_params/dataset_0.json");

    {
      TrajectoryExperiment experiment("experiment_0/parameters.json", &dataset);
      experiment.run();
    }

    {
      TrajectoryExperiment experiment("experiment_1/parameters.json", &dataset);
      experiment.run();
    }

    {
      TrajectoryExperiment experiment("experiment_2/parameters.json", &dataset);
      experiment.run();
    }
  }

  {
    CellsDataset dataset("dataset_params/dataset_1.json");

    {
      TrajectoryExperiment experiment("experiment_3/parameters.json", &dataset);
      experiment.run();
    }

    {
      TrajectoryExperiment experiment("experiment_4/parameters.json", &dataset);
      experiment.run();
    }

    {
      TrajectoryExperiment experiment("experiment_5/parameters.json", &dataset);
      experiment.run();
    }
  }

  {
    CellsDataset dataset("dataset_params/dataset_2.json");

    {
      TrajectoryExperiment experiment("experiment_6/parameters.json", &dataset);
      experiment.run();
    }

    {
      TrajectoryExperiment experiment("experiment_7/parameters.json", &dataset);
      experiment.run();
    }

    {
      TrajectoryExperiment experiment("experiment_8/parameters.json", &dataset);
      experiment.run();
    }
  }

  {
    CellsDataset dataset("dataset_params/dataset_3.json");

    {
      TrajectoryExperiment experiment("experiment_9/parameters.json", &dataset);
      experiment.run();
    }

    {
      TrajectoryExperiment experiment("experiment_10/parameters.json", &dataset);
      experiment.run();
    }

    {
      TrajectoryExperiment experiment("experiment_11/parameters.json", &dataset);
      experiment.run();
    }
  }

  {
    CellsDataset dataset("dataset_params/dataset_4.json");

    {
      TrajectoryExperiment experiment("experiment_12/parameters.json", &dataset);
      experiment.run();
    }

    {
      TrajectoryExperiment experiment("experiment_13/parameters.json", &dataset);
      experiment.run();
    }

    {
      TrajectoryExperiment experiment("experiment_14/parameters.json", &dataset);
      experiment.run();
    }
  }


  printf("program done\n");
  return 0;
}
