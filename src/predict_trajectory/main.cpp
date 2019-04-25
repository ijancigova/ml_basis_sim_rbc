#include <stdio.h>
#include <predict_trajectory.h>


int main()
{
  srand(time(NULL));

  {
    PredictTrajectory  predict( "dataset_params/dataset_0.json",
                                "experiment_0/");

    predict.run();
  }


  {
    PredictTrajectory  predict( "dataset_params/dataset_0.json",
                                "experiment_1/");

    predict.run();
  }

  {
    PredictTrajectory  predict( "dataset_params/dataset_0.json",
                                "experiment_2/");

    predict.run();
  }




  {
    PredictTrajectory  predict( "dataset_params/dataset_1.json",
                                "experiment_3/");

    predict.run();
  }


  {
    PredictTrajectory  predict( "dataset_params/dataset_1.json",
                                "experiment_4/");

    predict.run();
  }

  {
    PredictTrajectory  predict( "dataset_params/dataset_1.json",
                                "experiment_5/");

    predict.run();
  }

  {
    PredictTrajectory  predict( "dataset_params/dataset_2.json",
                                "experiment_6/");

    predict.run();
  }


  {
    PredictTrajectory  predict( "dataset_params/dataset_2.json",
                                "experiment_7/");

    predict.run();
  }

  {
    PredictTrajectory  predict( "dataset_params/dataset_2.json",
                                "experiment_8/");

    predict.run();
  }



  printf("program done\n");
  return 0;
}
